#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <numeric>
#include <utility>
#include <ctime>

#include <boost/program_options.hpp>

#include <TRandom3.h>
#include <TFile.h>
#include <TH1.h>

#include "running_stat.h"
#include "jep/reader.h"
#include "jep/stat2.h"

#define test(var) \
  cout <<"\033[36m"<< #var <<"\033[0m"<< " = " << var << endl;

using namespace std;
namespace po = boost::program_options;

template<class T>
void operator/=(vector<T>& v, T x) {
  for (vector<jep::val_t>::iterator it=v.begin(),
       end=v.end();it!=end;++it) *it /= x;
}

size_t nr;

struct profile {
  jep::header head;
  vector<jep::val_t> prof, dprof, stdev, pseudo, dpseudo;
  vector<running_stat> dpseudo_stats;

  static TRandom3 rand;

  profile(jep::particle_t pid, const pair<string,string>& fname, jep::val_t E)
  {
    jep::reader* jf = new jep::reader(fname.first);
    head = jf->get_header();
    const char* pname = jep::pid_name(pid);
    cout << pname << " file: " << fname.first << endl;
    if (head.pid != pid) {
      cerr << pname << " file header has pid of "
           << jep::pid_name(head.pid) << endl;
      exit(1);
    }
    prof = jf->psi(E);
    delete jf;

    dprof.resize(head.r_num);
    adjacent_difference( prof.begin(), prof.end(), dprof.begin() );

    stdev.reserve(head.r_num);
    fstream sf(fname.second.c_str());
    cout << "stdev: " << fname.second << endl;
    double x;
    while (sf >> x) stdev.push_back(x);
    if (head.r_num!=stdev.size()) {
      cerr << "num stdevs != num radius values" << endl;
      exit(1);
    }

    pseudo.resize(head.r_num);
    dpseudo.resize(head.r_num);
    dpseudo_stats.resize(head.r_num);
  }

  void compare(const profile& other) {
    if ( head.cmprad(other.head) ) {
      cerr << jep::pid_name(head.pid) << " and "
           << jep::pid_name(other.head.pid)
           << " profiles have different radii" << endl;
      exit(1);
    }
  }

  void new_pseudo() {
    // Generate pseudo data profile
    for (size_t i=0;i<head.r_num;++i) {
      static double psi;
      do {
        psi = rand.Gaus(dprof[i],stdev[i]);
      } while ( psi < 0. || 1. < psi );
      dpseudo[i] = psi;
    }
    dpseudo /= accumulate(dpseudo.begin(),dpseudo.end(),0.);
    partial_sum( dpseudo.begin(), dpseudo.end(), pseudo.begin() );

    // push to running stat
    for (size_t i=0;i<nr;++i) dpseudo_stats[i].push(dpseudo[i]);
  }
};
TRandom3 profile::rand;

ostream& operator<<(ostream& out, const profile& p) {
  out << jep::pid_name(p.head.pid) << endl;
  out << "mean                      stdev" << endl;
  out << "Input       Pseudo-data   Input       Pseudo-data" << endl;
  for (size_t i=0;i<nr;++i)
    out << p.dprof[i] << "  " << p.dpseudo_stats[i].mean() << "    "
        << p.stdev[i] << "  " << p.dpseudo_stats[i].stdev() << endl;
  out << endl;
  return out;
}

class stat_tests {
  TH1D *stat_chi2_I, *stat_chi2_d, *stat_like_I, *stat_like_d;
  const vector<jep::val_t> *prof, *dprof, *pseudo, *dpseudo;

public:
  stat_tests(const profile &hypoth, const profile &data)
  : stat_chi2_I( new TH1D(
      Form("chi2_I_%s_%s",jep::pid_name(hypoth.head.pid),
                          jep::pid_name(  data.head.pid)),
      "chi2_I",100,0,2)
    ),
    stat_chi2_d( new TH1D(
      Form("chi2_d_%s_%s",jep::pid_name(hypoth.head.pid),
                          jep::pid_name(  data.head.pid)),
      "chi2_d",100,0,2)
    ),
    stat_like_I( new TH1D(
      Form("like_I_%s_%s",jep::pid_name(hypoth.head.pid),
                          jep::pid_name(  data.head.pid)),
      "like_I",100,-25,6)
    ),
    stat_like_d( new TH1D(
      Form("like_d_%s_%s",jep::pid_name(hypoth.head.pid),
                          jep::pid_name(  data.head.pid)),
      "like_d",100,-25,6)
    ),
    prof(&hypoth.prof), dprof(&hypoth.dprof),
    pseudo(&data.pseudo), dpseudo(&data.dpseudo)
  { }
  ~stat_tests() { }

  void fill() {
    stat_chi2_I->Fill( // integral chi2
      jep::statistic(pseudo->begin(), prof->begin(), nr, jep::chi2)
    );
    stat_chi2_d->Fill( // derivative chi2
      jep::statistic(dpseudo->begin(), dprof->begin(), nr, jep::chi2)
    );
    stat_like_I->Fill( // integral log-likelihood
      -2.*log(
        jep::statistic(pseudo->begin(), prof->begin(), nr, jep::like, 1.)
      )
    );
    stat_like_d->Fill( // integral log-likelihood
      -2.*log(
        jep::statistic(dpseudo->begin(), dprof->begin(), nr, jep::like, 1.)
      )
    );
  }
};

namespace std {
  istream& operator>>(istream& in, pair<string,string>& ss) {
    string s;
    in >> s;
    const size_t sep = s.find(':');
    if (sep==string::npos) {
      cerr << "separator : expected in argument " << s << endl;
      exit(1);
    } else {
      ss.first  = s.substr(0,sep);
      ss.second = s.substr(sep+1);
    }
    return in;
  }
}

int main(int argc, char **argv)
{
  // START OPTIONS **************************************************
  pair<string,string> quark_files, gluon_files, higgs_files;
  string hist_file;
  double E;
  unsigned long num_mc;
  UInt_t seed = time(0);

  try {
    // General Options ------------------------------------
    po::options_description all_opt("Options");
    all_opt.add_options()
      ("help,h", "produce help message")
      ("quark", po::value< pair<string,string> >(&quark_files)->required(),
       "input quark jep file")
      ("gluon", po::value< pair<string,string> >(&gluon_files)->required(),
       "input gluon jep file")
      ("higgs", po::value< pair<string,string> >(&higgs_files)->required(),
       "input higgs jep file")
      ("output,o", po::value<string>(&hist_file)->required(),
       "output root file with histograms")
      ("energy,e", po::value<double>(&E)->required(),
       "jet Et")
      ("num,n", po::value<unsigned long>(&num_mc)->required(),
       "number of pseudo-data profiles to generate")
      ("seed,s", po::value<UInt_t>(&seed),
       "TRandom3 seed, if not given, time(0) is used")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, all_opt), vm);
    if (argc == 1 || vm.count("help")) {
      cout << all_opt << endl;
      exit(0);
    }
    po::notify(vm);
  }
  catch(exception& e) {
    cerr << "\033[31mError: " <<  e.what() <<"\033[0m"<< endl;
    exit(1);
  }
  // END OPTIONS ****************************************************

  // read jep files
  profile quark(jep::quark,quark_files,E);
  profile gluon(jep::gluon,gluon_files,E); gluon.compare(quark);
  profile higgs(jep::higgs,higgs_files,E); higgs.compare(quark);

  nr = quark.head.r_num;

  cout << "\nseed = " << seed << endl;
  profile::rand.SetSeed(seed);

  TFile *f = new TFile(hist_file.c_str(),"recreate");
  if (f->IsZombie()) exit(1);

  // prof pseudo
  stat_tests tests[9] = {
    stat_tests(quark,quark),
    stat_tests(quark,gluon),
    stat_tests(quark,higgs),
    stat_tests(gluon,quark),
    stat_tests(gluon,gluon),
    stat_tests(gluon,higgs),
    stat_tests(higgs,quark),
    stat_tests(higgs,gluon),
    stat_tests(higgs,higgs)
  };

  for (unsigned long k=0; k<num_mc; ++k) {
    // Generate pseudo data profiles
    quark.new_pseudo();
    gluon.new_pseudo();
    higgs.new_pseudo();

    // statistics testing
    for (short i=0;i<9;++i) tests[i].fill();
  }

  cout << fixed << setprecision(8) << endl;
  cout << quark << gluon << higgs;

  f->Write();
  cout << "Wrote: " << f->GetName() << endl;
  delete f;

  return 0;
}
