#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <numeric>
#include <ctime>

#include <TRandom3.h>

#include "running_stat.h"
#include "jep/reader.h"

#define test(var) \
  cout <<"\033[36m"<< #var <<"\033[0m"<< " = " << var << endl;

using namespace std;

typedef vector<double> vect;
typedef vect::iterator iter;

void operator/=(vect& v, double x) {
  for (iter it=v.begin(),end=v.end();it!=end;++it)
    *it /= x;
}

int main(int argc, char **argv)
{
  if (argc!=5) {
    cout << argv[0] << " profile.jep stdev.txt energy num" << endl;
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
  vect pseudo(nr); // pseudo data profile
  vector<running_stat> pseudo_stats(nr);
  const vector<jep::val_t> profile = jf->dpsi(atof(argv[3]));

  cout << fixed << setprecision(8);
  cout << "\nInput:\nmean        stdev" << endl;
  for (size_t i=0;i<nr;++i)
    cout << profile[i] << "  " << stdev[i] << endl;

  for (unsigned long k=0, num_mc=atol(argv[4]); k<num_mc; ++k) {
    for (size_t i=0;i<nr;++i) {
      static double psi;
      do {
        psi = rand.Gaus(profile[i],stdev[i]);
      } while ( psi < 0. || 1. < psi );
      pseudo[i] = psi;
    }
    pseudo /= accumulate(pseudo.begin(),pseudo.end(),0.);
    for (size_t i=0;i<nr;++i) {
      pseudo_stats[i].push(pseudo[i]);
    }
  }

  cout << "\nPseudo-data:\nmean        stdev" << endl;
  for (size_t i=0;i<nr;++i)
    cout << pseudo_stats[i].mean() << "  " << pseudo_stats[i].stdev() << endl;

  

  delete jf;

  return 0;
}
