#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>
#include <ctime>

#include <TRandom3.h>

#include "running_stat.h"

using namespace std;

typedef vector<double> vect;
typedef vect::iterator iter;

void operator/=(vect& v, double x) {
  for (iter it=v.begin(),end=v.end();it!=end;++it)
    *it /= x;
}

int main(int argc, char **argv)
{
  if (argc!=3) {
    cout << argv[0] << " stdev.txt num" << endl;
    return 0;
  }
  const unsigned long num_mc = atol(argv[2]);

  vect stdev;
  {
    fstream file(argv[1]);
    double x;
    while (file >> x) stdev.push_back(x);
  }
  const size_t nr = stdev.size();

  for (size_t i=0;i<nr;++i)
    cout << stdev[i] << endl;
  cout << endl;

  TRandom3 rand(time(0));
  vect pseudo(nr); // pseudo data profile
  vector<running_stat> pseudo_stats(nr);
  for (unsigned long k=0;k<num_mc;++k) {
    for (size_t i=0;i<nr;++i) {
      static double psi;
      do {
        psi = rand.Gaus(0.5,stdev[i]);
      } while ( psi < 0. || 1. < psi );
      pseudo[i] = psi;
    }
    pseudo /= accumulate(pseudo.begin(),pseudo.end(),0.);
    for (size_t i=0;i<nr;++i) {
      pseudo_stats[i].push(pseudo[i]);
    }
  }

  for (size_t i=0;i<nr;++i)
    cout << pseudo_stats[i].stdev() << endl;


  return 0;
}
