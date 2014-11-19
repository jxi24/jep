#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <sstream>
#include <ctime>

#include <boost/program_options.hpp>

#include <TH1.h>

#include "jep/writer.h"
#include "jets_file.h"
#include "running_stat.h"

using namespace std;
namespace po = boost::program_options;

#define test(var) \
  cout <<"\033[36m"<< #var <<"\033[0m"<< " = " << var << endl;

// MAIN ***************************************************
int main(int argc, char *argv[])
{
  // START OPTIONS **************************************************
  jep::header head;
  vector<string> in_jets;
  string out_base, conf_file;

  try {
    // General Options ------------------------------------
    po::options_description all_opt("Options");
    all_opt.add_options()
      ("help,h", "produce help message")
      ("input,i", po::value< vector<string> >(&in_jets),
       "input jets files")
      ("output,o", po::value<string>(&out_base),
       "output files base name, _mean.jep & _err.jep are produced")

      ("rnum",  po::value<jep::num_t>(&head.r_num),  "number of r points")
      ("rmin",  po::value<jep::val_t>(&head.r_min),  "minimum cone radius")
      ("rstep", po::value<jep::val_t>(&head.r_step), "cone radius increment")

      ("Enum",  po::value<jep::num_t>(&head.E_num),  "number of E points")
      ("Emin",  po::value<jep::val_t>(&head.E_min),  "minimum jet energy")
      ("Estep", po::value<jep::val_t>(&head.E_step), "jet energy increment")

      ("conf,c", po::value<string>(&conf_file), "read configuration file")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, all_opt), vm);
    po::notify(vm);
    if (vm.count("conf"))
      po::store(po::parse_config_file<char>(conf_file.c_str(), all_opt), vm);
    po::notify(vm);

    // Options Properties ---------------------------------
    if (argc==1 || vm.count("help")) {
      cout << all_opt << endl;
      exit(0);
    }

    // Necessary options ----------------------------------
    vector<string> rec_opt;
    rec_opt.push_back("input");
    rec_opt.push_back("output");
    rec_opt.push_back("rnum");
    rec_opt.push_back("rmin");
    rec_opt.push_back("rstep");
    rec_opt.push_back("Enum");
    rec_opt.push_back("Emin");
    rec_opt.push_back("Estep");

    for (size_t i=0, size=rec_opt.size(); i<size; ++i) {
      if (!vm.count(rec_opt[i]))
      { cerr << "Missing command --" << rec_opt[i] << endl; exit(1); }
    }

  } catch(exception& e) {
    cerr << "Arguments error: " <<  e.what() << endl;
    exit(1);
  }
  // END OPTIONS ****************************************************

  test(head.r_num)
  test(head.r_min)
  test(head.r_max())
  test(head.r_step)
  test(head.E_num)
  test(head.E_min)
  test(head.E_max())
  test(head.E_step)
  cout << endl;

  TH1D *profile = new TH1D("profile","",head.r_num,head.r_min,head.r_max());

  // ******************************************************
  // Read jets files
  // ******************************************************

  const size_t num_jets_files = in_jets.size();
  vector<jets_file*> jets(num_jets_files);

  for (size_t i=0; i<num_jets_files; ++i) {
    jets[i] = new jets_file(in_jets[i].c_str());

    cout << in_jets[i] << ": njets=" << jets[i]->njets()
         << " R=" << jets[i]->coneR() << endl;

    if (jets[i]->coneR()-head.r_max()>0.001) {
      cerr << "Different cone radius" << endl;
      exit(1);
    }
  }

  // ******************************************************
  // Compute profile statistics
  // ******************************************************

  running_stat stat[head.E_num][head.r_num];

  for (size_t i=0; i<num_jets_files; ++i) { // loop over jets files
    static jet j;

    // Loop over jets *************************************************
    cout << "Number of jets: " << jets[i]->njets() << endl;
    time_t time1=time(0), time2;
    unsigned seconds = 0;
    long njets = -1; // total number jets
    long ngood =  0; // number of processed jets

    while (jets[i]->next(j)) { // loop over jets in a file
      ++njets;

      // find E bin
      int E_bin = 0;
      if (j.Et()<head.E(0)) continue;
      while (j.Et()>=head.E(E_bin+1)) {
        ++E_bin;
        if (E_bin==head.E_num-1) break;
      }
      if (j.Et()>=head.E_max()) continue;

      // fill profile
      profile->Reset();
      for (size_t c=0,nc=j.size(); c<nc; ++c) // 1st loop over jet constituents
        profile->Fill(j.r(c),j.Et(c));

      // normalize profile
      profile->SetBinContent(0,0);
      profile->SetBinContent(head.r_num+1,0);
      Double_t integral = profile->Integral();
      if (isnormal(integral)) profile->Scale(1./integral);

      // push running_stats
      for (jep::num_t r_bin=0; r_bin<head.r_num; ++r_bin) // 2nd loop over jet constituents
        stat[E_bin][r_bin].push( profile->GetBinContent(r_bin+1) );

      // Increment successful jets counter
      ++ngood;

      // timed counter
      if ( difftime(time2=time(0),time1) > 1 ) {
        ++seconds;
        cout << setw(10) << njets
             << setw( 7) << seconds << 's';
        cout.flush();
        for (char i=0;i<18;++i) cout << '\b';
        time1 = time2;
      }

    }
    cout << setw(10) << jets[i]->njets()
         << setw( 7) << seconds << 's' << endl << endl;
    cout << "Processed " << ngood << " jets" << endl;
  }

  // ******************************************************
  // Write output files
  // ******************************************************

  jep::writer* mean = new jep::writer((out_base+"_mean.jep").c_str(),head);
  jep::val_t psi_mean[head.r_num];

  jep::writer* err = new jep::writer((out_base+"_err.jep").c_str(),head);
  jep::val_t psi_err[head.r_num];

  // don't need two iterators because headers are identical
  jep::writer::Er_iter it = mean->begin();

  //cout << endl << fixed << setprecision(5);

  do { // E loop
    //cout << "E = " << it.E() << endl;
    do { // r loop

      psi_mean[it.ri()] = stat[it.Ei()][it.ri()].mean();
      psi_err [it.ri()] = stat[it.Ei()][it.ri()].stdev();

      //cout << setw(7) << stat[it.Ei()][it.ri()].mean() << ' '
      //     << setw(7) << stat[it.Ei()][it.ri()].num() << endl;

    } while ( it.next_r() );

    //cout << endl;

    // write jep data files
    mean->write(psi_mean);
    err ->write(psi_err);

  } while ( it.next_E() );

  // ******************************************************

  delete mean;
  delete err;

  delete profile;

  for (size_t i=0; i<num_jets_files; ++i) delete jets[i];

  return 0;
}
