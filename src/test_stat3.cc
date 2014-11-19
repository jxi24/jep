#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <utility>
#include <map>
#include <ctime>
#include <exception>
#include <stdexcept>
#include <cmath>

#include <TFile.h>
#include <TH1.h>

#include "jep/reader.h"
#include "jep/stat2.h"
#include "test/jets_file.h"
#include "test/hist_wrap.h"
#include "test/profile.h"

#define test(var) \
  cout <<"\033[36m"<< #var <<"\033[0m"<< " = " << var << endl;

using namespace std;

typedef vector< pair<string,jep::reader*> >::iterator fjep_iter;

const int n_pt_bins = 20;
const double pt_bins[n_pt_bins] = {
   37,  45,  55,  63,  73,  84,  97, 112, 128,  148,
  166, 186, 208, 229, 250, 277, 304, 340, 380, 1000
};

// MAIN ***************************************************
int main(int argc, char *argv[])
{
  // Arguments ******************************************************
  if ( argc<5 ) {
    cout << "Usage: " << argv[0]
         << " jets_file.jets binning.bins stat_plots.root "
            "gluon:profile1.jep quark:profile2.jep ..." << endl;
    return 0;
  }

  // Read files *****************************************************
  vector< pair<string,jep::reader*> > fjep;

  { // Parse jep files arguments
    map<string,string> fjepnames;

    for (int i=4;i<argc;++i) {
      string arg(argv[i]);
      size_t col = arg.find_first_of(':');
      if (col==string::npos) {
        cerr << "Bad argument " << i << ": " << arg << endl;
        return 1;
      }
      fjepnames[arg.substr(0,col)] = arg.substr(col+1);
    }

    cout << "\nJet energy profiles:" << endl;
    for (map<string,string>::iterator it=fjepnames.begin(),
         end=fjepnames.end(); it!=end; ++it)
    {
      cout << "  " << it->first << ": " << it->second << endl;
      fjep.push_back( make_pair(
        it->first, new jep::reader(it->second.c_str())
      ) );
    }
    cout << endl;
  }

  // reference to the first header
  const jep::header& h = fjep.begin()->second->get_header();

  // check jep headers compatibility
  for (fjep_iter it=fjep.begin()+1, end=fjep.end(); it!=end; ++it) {
    if ( h != it->second->get_header() ) {
      cerr << "Incompatible jep file headers" << endl;
      return 1;
    }
  }

  jets_file jets(argv[1]);
  jet j;

  // current profile
  jep::val_t prof[h.r_num], dprof[h.r_num];

  // average profile
  jep::val_t prof_avg[n_pt_bins-1][h.r_num];
  int prof_avg_n[n_pt_bins-1];
  for (int k=0; k<n_pt_bins-1; ++k) {
    prof_avg_n[k] = 0;
    for (jep::num_t i=0; i<h.r_num; ++i)
      prof_avg[k][i] = 0.;
  }

  // timing
  clock_t last_time = clock();
  short num_sec = 0; // seconds elapsed

  cout << "Theory cone R: " << h.r_max() << endl;
  cout << "Data   cone R: " << jets.coneR() << endl;

  TFile *fout = new TFile(argv[3],"recreate");

  // Book histograms ************************************************
  hist::read_binnings(argv[2],"([[:alnum:]]*)_([[:alnum:]])_([[:alnum:]]*)");

  hist *h_num_const = new hist("num_const","Number of jet constituents");

  vector<TH1F*> h_avg_prof;

  map<fjep_iter,vector<hist*> >
    stat_chi2_I, stat_chi2_d,
    stat_like_I, stat_like_d;

  for (int i=1; i<n_pt_bins; ++i) {
    stringstream ss;
    ss << "_pt" << pt_bins[i-1] << '_' << pt_bins[i];

    h_avg_prof.push_back( new TH1F(("avg_prof"+ss.str()).c_str(),"",
                          h.r_num,h.r_min-h.r_step,h.r_max()) );
    h_avg_prof.back()->SetTitle("Average jet energy profile;"
                                "Cone radius, r;Energy fraction, #psi");
    h_avg_prof.back()->SetStats(0);
    h_avg_prof.back()->SetAxisRange(0,1,"Y");

    for (fjep_iter it=fjep.begin(), end=fjep.end(); it!=end; ++it) {

        stat_chi2_I[it].push_back( new hist(
          "chi2_I_"+it->first+ss.str(),
          "#chi^{2} for "+it->first+" hypothesis with integral profile"
          ";#chi^{2};"
        ) );

        stat_chi2_d[it].push_back( new hist(
          "chi2_d_"+it->first+ss.str(),
          "#chi^{2} for "+it->first+" hypothesis with differential profile"
          ";#chi^{2};"
        ) );

        stat_like_I[it].push_back( new hist(
          "like_I_"+it->first+ss.str(),
          "log L for "+it->first+" hypothesis with integral profile"
          ";log L;"
        ) );

        stat_like_d[it].push_back( new hist(
          "like_d_"+it->first+ss.str(),
          "log L for "+it->first+" hypothesis with differential profile"
          ";log L;"
        ) );
    }
  }

  ofstream errlog("stat2_log.txt");

  // Loop over jets *************************************************
  cout << "Number of jets: " << jets.njets() << endl;
  cout << endl;
  long njets = -1; // total number jets
  long ngood =  0; // number of processed jets
  while (jets.next(j)) {
    ++njets;

//    if (njets<41592) continue;
//    test(njets)

    // find pt bin
    int pt_bin = 0;
    if (j.Et()<pt_bins[0]) continue;
    while (j.Et()>=pt_bins[pt_bin+1]) {
      ++pt_bin;
      if (pt_bin==n_pt_bins-1) break;
    }
    if (n_pt_bins-pt_bin==1) continue;

//    test(pt_bin)
//    test(pt_bins[pt_bin])

    // get current jet profile
    try {

      profile(j,prof,dprof,h,false);

    } catch (exception& e) {
      errlog << setw(6) << njets << ' ' << e.what() << endl;
      continue;
    }

/*
    cout << "jet Et = " << j.Et() << endl << endl;

    cout << setw(10) << left << "r" << ' ' << "Et" << endl;
    for (size_t i=0, end=j.size(); i<end; ++i) {
      cout << setw(10) << left << j.r(i) << ' ' << j.Et(i) << endl;
    }
    cout << endl;

    for (jep::num_t i=0;i<h.r_num;++i) {
      cout << h.r(i) << ' ' << prof[i] << endl;
    }
    cout << endl;

    for (jep::num_t i=0;i<h.r_num;++i) {
      cout << h.r(i) << ' ' << dprof[i] << endl;
    }
    cout << endl;

    break;
*/

    // average jet profile
    for (jep::num_t i=0; i<h.r_num; ++i) prof_avg[pt_bin][i] += prof[i];
    ++prof_avg_n[pt_bin];

    // calculate statistics
    bool process_jet = true;
    for (fjep_iter it=fjep.begin(), end=fjep.end(); it!=end; ++it) {
      try {
        const vector<jep::val_t> hypoth = it->second->psi(j.Et());

        jep::val_t dhypoth[h.r_num];
        dhypoth[0] = hypoth[0];
        for (jep::num_t k=h.r_num-1; k>0; --k) {
          dhypoth[k] = hypoth[k] - hypoth[k-1];
        }

        stat_chi2_I[it][pt_bin]->Fill( // integral chi2
          jep::statistic(prof, hypoth.begin(), h.r_num, jep::chi2)
        );

        stat_chi2_d[it][pt_bin]->Fill( // derivative chi2
          jep::statistic(dprof, dhypoth, h.r_num, jep::chi2)
        );

        stat_like_I[it][pt_bin]->Fill( // integral log-likelihood
          -2.*log( jep::statistic(prof, hypoth.begin(), h.r_num, jep::like, 1.) )
        );

        stat_like_d[it][pt_bin]->Fill( // integral log-likelihood
          -2.*log( jep::statistic(dprof, dhypoth, h.r_num, jep::like, 1.) )
        );

      } catch (exception& e) {
        errlog << setw(6) << njets << ' ' << e.what() << endl;
        process_jet = false;
        break;
      }
    }
    if (!process_jet) continue; // skip jet

    // Fill histograms
    h_num_const->Fill(j.size());

    // Increment successful jets counter
    ++ngood;

    // timed counter
    if ( (clock()-last_time)/CLOCKS_PER_SEC > 1 ) {
      ++num_sec;
      cout << setw(10) << jets.cur()
           << setw( 7) << num_sec << 's';
      cout.flush();
      for (char i=0;i<19;++i) cout << '\b';
      last_time = clock();
    }

  }
  cout << right;
  cout << setw(10) << jets.njets()
       << setw( 7) << num_sec << 's' << endl << endl;
  cout << "Processed " << ngood << " jets" << endl;

  // Histograms *****************************************************
  for (int k=0; k<n_pt_bins-1; ++k) {
    if (prof_avg_n[k] > 0) {
      for (jep::num_t i=0; i<h.r_num; ++i) {
        h_avg_prof[k]->SetBinContent(i+1,prof_avg[k][i]/prof_avg_n[k]);
      }
      h_avg_prof[k]->SetEntries(prof_avg_n[k]);
    }
  }

  hist::delete_all();

  // Clean up *******************************************************
  fout->Write();
  fout->Close();
  delete fout;

  for (fjep_iter it=fjep.begin(), end=fjep.end(); it!=end; ++it)
    delete it->second;

  errlog.close();

  return 0;
}
