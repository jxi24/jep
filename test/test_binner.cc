#include <iostream>
#include <iomanip>
#include <string>

#include <TRandom3.h>

#include "binner.h"

using namespace std;

struct filler {
  void operator()(float& v) {
    ++v;
  }
};

int main (int argc, char **argv)
{
  TRandom3 rnd;

  binner<float,filler> hist(10,0.,10.);

  // for (int i=0;i<100000;++i) {
  //   hist.fill(rnd.Gaus(5.5,2.));
  // }

  hist.fill(-2);
  hist.fill(5);
  hist.fill(4.9);
  hist.fill(5.1);
  hist.fill(10);
  hist.fill(9.9);
  hist.fill(111);

  for (size_t i=0,n=hist.nbins()+2;i<n;++i) {
    cout << setw(5)  << hist.low_edge(i)
         << setw(10) << hist[i] << endl;
  }

  return 0;
}
