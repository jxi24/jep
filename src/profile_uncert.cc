#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <cmath>

#include "jets_file.h"
#include "running_stat.h"
#include "binner.h"

using namespace std;

struct stat_filler {
  void operator()(running_stat& v, double x) { v.push(x); }
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
  const double step = 0.1;
  const double tol = 0.5; // overflow tolerance in step units
  const size_t nbins = lrint(R/step); // apparently int(10*0.7)==6
  binner<running_stat,stat_filler> profiles(nbins,0.,R);

  long njets = 0;
  while (jets.next(j)) { // loop over jets in a file

    hist<> prof(nbins,0.,R);

    for (size_t i=0,n=j.size();i<n;++i) {
      double r = j.r(i);
      if (r>=R) if (r<R+tol*step) r = R-0.5*step;
      prof.fill(r,j.Et(i));
    }
    prof.normalize();

    // vector<double> _prof = prof.partial_sums();
    // for (size_t i=0;i<nbins;++i) profiles.fill(step*(i+0.5),_prof[i]);

    for (size_t i=1;i<=nbins+1;++i) profiles.fill(step*(i-0.5),prof[i]);

    ++njets;
  }

  cout << njets << " jets" << endl;
  cout << showpoint << fixed;
  cout << setw( 5) << "r"
       << setw(12) << "mean"
       << setw(12) << "stdev" << endl;
  for (size_t i=1;i<=nbins+1;++i) {
    cout << setw( 5) << setprecision(2) << profiles.up_edge(i)
         << setw(12) << setprecision(8) << profiles[i].mean()
         << setw(12)                    << profiles[i].stdev() << endl;
  }

  return 0;
}
