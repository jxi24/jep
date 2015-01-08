#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <numeric>
#include <ctime>

#include <TRandom3.h>
#include <TFile.h>
#include <TH1.h>

#include "running_stat.h"
#include "jep/reader.h"
#include "jep/stat2.h"

#define test(var) \
  cout <<"\033[36m"<< #var <<"\033[0m"<< " = " << var << endl;

using namespace std;

typedef vector<double> vect;
typedef vect::iterator iter;

template<class T>
void operator/=(vector<T>& v, T x) {
  for (iter it=v.begin(),end=v.end();it!=end;++it) *it /= x;
}
/*
template<class T>
vector<T> sums(const vector<T>& v) {
  vector<T> s(v);
  for (vector<T>::iterator it=s.begin(), end=s.end(); it!=end; ) {
    vector<T>::iterator it1 = it++;
    *it += *it1;
  }
  return s;
}
template<class T>
vector<T> diffs(const vector<T>& v) {
  vector<T> d(v);
  for (vector<T>::iterator it=d.rbegin(), end=d.rend(); it!=end; ) {
    vector<T>::iterator it1 = it++;
    *it += *it1;
  }
  return d;
}
*/

class stat_tests {
  TH1D *stat_chi2_I, *stat_chi2_d, *stat_like_I, *stat_like_d;
  const vect *hypoth, *dhypoth, *data, *ddata;

public:
  stat_tests(const char* name,
             const vect *hypoth, const vect *dhypoth,
             const vect *data,   const vect *ddata)
  : stat_chi2_I( new TH1D(Form("chi2_I_%s",name),"chi2_I",100,0,2) ),
    stat_chi2_d( new TH1D(Form("chi2_d_%s",name),"chi2_d",100,0,2) ),
    stat_like_I( new TH1D(Form("like_I_%s",name),"like_I",100,-25,6) ),
    stat_like_d( new TH1D(Form("like_d_%s",name),"like_d",100,-25,6) ),
    hypoth(hypoth), dhypoth(dhypoth), data(data), ddata(ddata)
  { }
  ~stat_tests() { }

  void fill() {
    stat_chi2_I->Fill( // integral chi2
      jep::statistic(data->begin(), hypoth->begin(),
                     hypoth->size(), jep::chi2)
    );
    stat_chi2_d->Fill( // derivative chi2
      jep::statistic(ddata->begin(), dhypoth->begin(),
                     dhypoth->size(), jep::chi2)
    );
    stat_like_I->Fill( // integral log-likelihood
      -2.*log( jep::statistic(data->begin(), hypoth->begin(),
                              hypoth->size(), jep::like, 1.) )
    );
    stat_like_d->Fill( // integral log-likelihood
      -2.*log( jep::statistic(ddata->begin(), dhypoth->begin(),
                              dhypoth->size(), jep::like, 1.) )
    );
  }
};

int main(int argc, char **argv)
{
  if (argc!=6) {
    cout << argv[0] << " profile.jep stdev.txt output.root energy num" << endl;
    return 0;
  }

  jep::reader* jf = new jep::reader(argv[1]);
  const jep::header& head = jf->get_header();
  cout << "Profile: " << argv[1] << ": " << jep::pid_name(head.pid) << endl;
  const size_t nr = head.r_num;

  vect stdev;
  stdev.reserve(nr);
  {
    fstream file(argv[2]);
    cout << "Stdev  : " << argv[2] << endl;
    double x;
    while (file >> x) stdev.push_back(x);
  }
  if (nr!=stdev.size()) {
    cerr << "More stdevs then radius values" << endl;
    exit(1);
  }

  TRandom3 rand(time(0));
  vector<running_stat> dpseudo_stats(nr);
  const vector<jep::val_t> prof = jf->psi(atof(argv[4]));
  vector<jep::val_t> dprof(nr);
  adjacent_difference( prof.begin(), prof.end(), dprof.begin() );
  vect  pseudo(nr); // pseudo data profile
  vect dpseudo(nr);

  TFile *f = new TFile(argv[3],"recreate");
  if (f->IsZombie()) exit(1);

  //         prof  pseudo
  stat_tests gluon_gluon("glue_glue", &prof, &dprof, &pseudo, &dpseudo);

  cout << fixed << setprecision(8);
  cout << "\nInput:\nmean        stdev" << endl;
  for (size_t i=0;i<nr;++i)
    cout << dprof[i] << "  " << stdev[i] << endl;

  for (unsigned long k=0, num_mc=atol(argv[5]); k<num_mc; ++k) {

    // Generate pseudo data profile
    for (size_t i=0;i<nr;++i) {
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

    // statistics testing
    gluon_gluon.fill();
  }

  cout << "\nPseudo-data:\nmean        stdev" << endl;
  for (size_t i=0;i<nr;++i)
    cout << dpseudo_stats[i].mean() << "  " << dpseudo_stats[i].stdev() << endl;

  f->Write();
  cout << "\nWrote: " << f->GetName() << endl;
  delete f;
  delete jf;

  return 0;
}
