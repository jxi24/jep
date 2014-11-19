#include <iostream>
#include <vector>

#include <TFile.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TH1.h>

using namespace std;

typedef vector<TH1*>::iterator iter;

int main(int argc, char *argv[])
{
  if ( argc<4 ) {
    cout << "Usage: " << argv[0]
         << " in.root out.pdf title hist1 hist2 ..." << endl;
    return 0;
  }

  TFile *f = new TFile(argv[1],"read");

  vector<TH1*> h;

  for (int i=4; i<argc; ++i) {
    TObject* obj = f->Get(argv[i]);
    if (!obj) {
      cout << "File " << argv[1] << " has no object "
           << argv[i] << endl;
      return 1;
    }
    h.push_back( dynamic_cast<TH1*>(obj) );
  }

  double ymin = h[0]->GetMinimum(), ymax = h[0]->GetMaximum();
  for (size_t i=1, size=h.size(); i<size; ++i) {
    double min = h[i]->GetMinimum(), max = h[i]->GetMaximum();
    if (min<ymin) ymin = min;
    if (max>ymax) ymax = max;
  }
  h[0]->SetAxisRange(ymin*1.05,ymax*1.05,"Y");
  h[0]->SetTitle(argv[3]);

  TCanvas canv;
  TLegend leg(0.75,0.92-0.05*h.size(),0.95,0.92);
  leg.SetFillColor(0);
  gStyle->SetOptStat(0); 

  for (size_t i=0, size=h.size(); i<size; ++i) {
    h[i]->SetLineWidth(2);
    h[i]->SetLineColor(i+2);
    h[i]->SetMarkerColor(i+2);
    leg.AddEntry(h[i],h[i]->GetName());
    if (i==0) h[i]->Draw();
    else h[i]->Draw("same");
  }

  leg.Draw();
  canv.SaveAs(argv[2]);

  f->Close();
  delete f;

  return 0;
}
