#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <cmath>

#include <TFile.h>
#include <TH1.h>

#include "jets_file.h"
#include "running_stat.h"
#include "binner.h"

using namespace std;

struct hist_filler {
  void operator()(TH1* h, double x) { h->Fill(x); }
};

int main(int argc, char **argv)
{
  if (argc!=3) {
    cout << "Usage: " << argv[0] << " input.jets output.root" << endl;
    return 0;
  }

  jets_file jets(argv[1]);
  jet j;

  const double R = jets.coneR();
  const double step = 0.1;
  const double tol = 0.5; // overflow tolerance in step units
  const size_t nbins = lrint(R/step); // apparently int(10*0.7)==6
  binner<TH1*,hist_filler> profiles(nbins,0.,R);
  
  TFile *f = new TFile(argv[2],"recreate");
  if (f->IsZombie()) exit(1);

  for (size_t i=1;i<=profiles.nbins();++i) {
    const double up_edge = profiles.up_edge(i);
    const double low_edge = profiles.low_edge(i);
    profiles[i] = new TH1F(
      Form("r_%.1f_%.1f",low_edge,up_edge),
      Form("%.1f #leq r < %.1f",low_edge,up_edge),
      100,0,1);
  }
  profiles[profiles.nbins()+1] = new TH1F(
    Form("r_%.1f",profiles.low_edge(profiles.nbins()+1)),
    Form("r #geq %.1f",profiles.low_edge(profiles.nbins()+1)),
    100,0,1);

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
    // for (size_t i=0;i<nbins;++i) profiles.fill_bin(i+1,_prof[i]);

    for (size_t i=1;i<=nbins+1;++i) profiles.fill_bin(i,prof[i]);

    ++njets;
  }

  f->Write();
  f->Close();
  delete f;

  return 0;
}
