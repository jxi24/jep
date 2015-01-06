#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <cmath>

#include "jets_file.h"
#include "running_stat.h"
#include "binner.h"

using namespace std;

#define test(var) \
  cout <<"\033[36m"<< #var <<"\033[0m"<< " = " << var << endl;

struct stat_filler {
  void operator()(running_stat& v, double x) {
    v.push(x);
  }
};

int main(int argc, char **argv)
{
  if (argc!=2) {
    cout << "Usage: " << argv[0] << " input.jets" << endl;
    return 0;
  }

  jets_file jets(argv[1]);
  jet j;

  const double R = jets.coneR();
  const size_t nbins = lrint(10*R); // apparently int(10*0.7)==6
  binner<running_stat,stat_filler> profiles(nbins,0.,R);

  long njets = 0;
  while (jets.next(j)) { // loop over jets in a file

    hist<> prof(nbins,0.,R);

    for (size_t i=0,n=j.size();i<n;++i) prof.fill(j.r(i),j.Et(i));
    prof.normalize();

    for (size_t i=1;i<=nbins;i++) profiles.fill(0.1*i,prof[i]);

    ++njets;
  }

  cout << njets << " jets" << endl;
  cout << showpoint << setprecision(8) << fixed;
  cout << setw(12) << "r-up"
       << setw(12) << "mean"
       << setw(12) << "stdev" << endl;
  for (size_t i=1;i<=nbins;++i) {
    cout << setw(12) << profiles.up_edge(i)
         << setw(12) << profiles[i].mean()
         << setw(12) << profiles[i].stdev() << endl;
  }

  return 0;
}
