#include <iostream>
#include <string>
#include <vector>

#include <boost/regex.hpp>

#include <TFile.h>
#include <TKey.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TAxis.h>

#include "propmap.h"

using namespace std;

#define test(var) \
  cout <<"\033[36m"<< #var <<"\033[0m"<< " = " << var << endl;

const char * const particle_names[] = {"gluon", "quark", "higgs"};
const Color_t color[] = {2,3,4,5,6,7};

bool parse_hist(const string& name, vector<prop_ptr>& key) {
  static const boost::regex regex(
    "([[:alnum:]]*_[dI])_([[:alpha:]]*)_([[:alpha:]]*)");
  static boost::smatch result;
  if ( boost::regex_search(name, result, regex) ) {
    key[0] = new prop<string>( string(result[1].first, result[1].second) );
    key[1] = new prop<string>( string(result[2].first, result[2].second) );
    key[2] = new prop<string>( string(result[3].first, result[3].second) );

    return true;
  } else return false;
}

double min_in_range(const TH1* h, double xmin, double xmax) {
  Int_t bmin = h->FindFixBin(xmin);
  Int_t bmax = h->FindFixBin(xmax);
  Int_t nbins = h->GetNbinsX();
  if (bmin==0) ++bmin;
  if (bmax>nbins) bmax = nbins;
  double min = h->GetBinContent(bmax);
  double y;
  for (Int_t i=bmin; i<bmax; ++i) {
    y = h->GetBinContent(i);
    if (y<min) min = y;
  }
  return min;
}
double max_in_range(const TH1* h, double xmin, double xmax) {
  Int_t bmin = h->FindFixBin(xmin);
  Int_t bmax = h->FindFixBin(xmax);
  Int_t nbins = h->GetNbinsX();
  if (bmin==0) ++bmin;
  if (bmax>nbins) bmax = nbins;
  double max = h->GetBinContent(bmax);
  double y;
  for (Int_t i=bmin; i<bmax; ++i) {
    y = h->GetBinContent(i);
    if (max<y) max = y;
  }
  return max;
}

// MAIN ***************************************************
int main(int argc, char *argv[])
{
  // parse arguments
  if ( argc!=3 ) {
    cout << "Usage: " << argv[0] << " in.root out.pdf" << endl;
    return 0;
  }

  // property map of histograms
  propmap<TH1*> hmap(3);
  vector<prop_ptr> hkey(3);

  // Read file
  TFile *f = new TFile(argv[1],"read");

  static TKey *fkey;
  TIter nextkey(f->GetListOfKeys());
  while ((fkey = (TKey*)nextkey())) {
    TH1 *hist = (TH1*)fkey->ReadObj();
    if ( parse_hist(hist->GetName(),hkey) ) {
      hmap.insert(hkey,hist);
    }
  }

  gStyle->SetOptStat(0);
  TCanvas canv;

  // ******************************************************

  canv.SaveAs(Form("%s[",argv[2]));
  pmloop(hmap,a,1) {
    hkey[1] = *a;
    pmloop(hmap,method,0) {
      hkey[0] = *method;

      canv.Clear();
      TLegend leg(0.80,0.80,0.98,0.98);
      leg.SetFillColor(0);
      leg.SetHeader("Hypotheses:");

      short hi = 0;
      TH1 *h1 = NULL;
      double xmin=0, xmax=0;
      double ymin=0, ymax=0;

      pmloop(hmap,b,2) {
        hkey[2] = *b;

        TH1 *h = NULL;
        hmap.get(hkey,h);

        h->SetLineWidth(2);
        h->SetLineColor(color[hi]);
        h->SetMarkerColor(color[hi]);
        leg.AddEntry(h,(*b)->str().c_str());
        leg.SetY1( leg.GetY1()-0.03 );
        if (hi==0) {
          xmin = h->GetXaxis()->GetXmin();
          xmax = h->GetXaxis()->GetXmax();
          ymin = min_in_range(h,xmin,xmax);
          ymax = max_in_range(h,xmin,xmax);
          h1 = h;
          h->SetTitle(((*method)->str()+" | pseudo-data: "+(*a)->str()).c_str());
          h->Draw("C");
        } else {
          double min = min_in_range(h,xmin,xmax),
                 max = max_in_range(h,xmin,xmax);
          if (min<ymin) ymin = min;
          if (max>ymax) ymax = max;
          h1->SetAxisRange(ymin*1.05,ymax*1.05,"Y");
          h->Draw("sameC");
        }

        ++hi;
      }
      leg.Draw();
      canv.SaveAs(argv[2]);

    }
  }
  canv.SaveAs(Form("%s]",argv[2]));

  delete f;

  return 0;
}
