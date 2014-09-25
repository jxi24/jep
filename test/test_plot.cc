#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>

#include <TCanvas.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TAxis.h>
#include <TLegend.h>

#include "jep/reader.h"

using namespace std;

#define test(var) \
  cout <<"\033[36m"<< #var <<"\033[0m"<< " = " << var << endl;

#define nparticles 3

int main(int argc, char** argv)
{
  if (argc!=nparticles+3) {
    cout << "Usage: " << argv[0]
         << " quark_file gluon_file higgs_file E(double) plot_file" << endl;
    return 1;
  }

  // data points arrays
  const double E = atof(argv[nparticles+1]);
  vector<jep::val_t> psi[nparticles];

  // first file header
  jep::header head;

  // open jep data files
  for (unsigned char i=0; i<nparticles; ++i) { // loop over particles

    // open jep data file
    const char* file = argv[i+1];
    jep::reader* f = new jep::reader(file);
    const jep::header& h = f->get_header();
    cout << "Reading " << file << ": " << jep::pid_name(h.pid) << endl;

    if (i==0) head = h;
    else if ( h != head ) {
      cout << "Header in \"" << file
           << "\" does not match those of previous files" << endl;
      return 1;
    }

    // Read data
    psi[i] = f->psi(E);

    // close jep file
    delete f;
  }

  // radii array
  jep::val_t r_[head.r_num];
  r_[0] = head.r_min;
  for (jep::num_t i=1; i<head.r_num; ++i) {
    r_[i] = r_[i-1] + head.r_step;
  }

  TCanvas canv; // Canvas
  TLegend leg(0.91,0.7,0.99,0.9); // Legend

  // Graphs
  TMultiGraph *mg = new TMultiGraph();

  for (unsigned char i=0; i<nparticles; ++i) {
    TGraph *gr = new TGraph(head.r_num,r_,&psi[i][0]);
    gr->SetLineColor(2+i);
    gr->SetMarkerColor(2+i);
    gr->SetFillColor(0);
    gr->SetLineWidth(2);

    mg->Add(gr);
    switch (i) {
      case 0: leg.AddEntry(gr,"quark"); break;
      case 1: leg.AddEntry(gr,"gluon"); break;
      case 2: leg.AddEntry(gr,"higgs"); break;
    }
  }

  mg->Draw("ac");
  stringstream title;
  title << "Jet energy profile at E = " << E;
  mg->SetTitle(title.str().c_str());
  gPad->Update();

  mg->GetXaxis()->SetRangeUser(r_[0],r_[head.r_num-1]);
  mg->GetYaxis()->SetRangeUser(0.,1.);
  mg->GetXaxis()->SetTitle("Cone radius, r");
  mg->GetYaxis()->SetTitle("Energy fraction, #psi");

  leg.SetFillColor(0);
  leg.Draw();

  canv.SaveAs(argv[5]);

  delete mg;

  return 0;
}
