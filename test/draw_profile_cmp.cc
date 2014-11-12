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

#include "test/propmap.h"

using namespace std;

#define test(var) \
  cout <<"\033[36m"<< #var <<"\033[0m"<< " = " << var << endl;

const char * const data_names[] = {"gluon", "quark", "higgs"};
const Color_t color[] = {2,3,4,5,6,7};

class range_p: public prop< pair<float,float> > {
  pair<float,float> init(const string& s) {
    const size_t sep = s.find_first_of('_');
    return make_pair(
      atof( s.substr(0,sep).c_str() ),
      atof( s.substr(sep+1).c_str() )
    );
  }
public:
  range_p(const string& s)
  : prop< pair<float,float> >(init(s)) { }
};
// http://stackoverflow.com/questions/1318458/template-specialization-of-template-class
template<> std::string prop< std::pair<float,float> >::str() const {
  std::stringstream ss;
  ss << x.first << ' ' << x.second;
  return ss.str();
}

bool parse_avg_prof(const string& name, vector<prop_ptr>& key) {
  static const boost::regex regex("avg_prof_pt([[:digit:]]*_[[:digit:]]*)");
  static boost::smatch result;
  if ( boost::regex_search(name, result, regex) ) {
    key[2] = new range_p( string(result[1].first, result[1].second) );
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
  if ( argc<3 ) {
    cout << "Usage: " << argv[0]
         << " gluon_mc.root quark_mc.root higgs_mc.root ..." << endl
         << " gluon_th.root quark_th.root higgs_th.root ..." << endl
    return 0;
  }

  const short nf = (argc-1)/2;

  // open files
  TFile* f[nf][2];
  for (short i=0;i<nf;++i) {
    for (short j=0;j<2;++j) {
      f[i][j] = new TFile(argv[j*nf+i+1],"read");
      if (f[i][j]->IsZombie()) return 1;
    }
  }

  // trackers of histograms
  propmap<TH1*> profiles(3);
  vector<prop_ptr> pkey(3);

  // read files
  for (short j=0;j<2;++j) {
    if (j==0) pkey[0] = new prop<string>("mc");
    else      pkey[1] = new prop<string>("theory");

    for (short i=0;i<nf;++i) {

           if (i==0) pkey[1] = new prop<string>("gluon");
      else if (i==1) pkey[1] = new prop<string>("quark");
      else if (i==2) pkey[1] = new prop<string>("higgs");
      
      static TKey *fkey;
      TIter nextkey(f[i]->GetListOfKeys());
      while ((fkey = (TKey*)nextkey())) {
        TH1 *hist = dynamic_cast<TH1*>( fkey->ReadObj() );
        if ( parse_avg_prof(hist->GetName(),pkey) ) {
          avg_prof.insert(pkey,hist);
          continue;
        }
      }
    }
  }

  gStyle->SetOptStat(0);
  TCanvas canv;

  // ******************************************************
  // Average profiles
  // ******************************************************
  canv.SaveAs((dir+"avg_prof.pdf[").c_str());
  pmloop(avg_prof,a,1) {

    canv.Clear();
    TLegend leg(0.75,0.66,0.95,0.82);
    leg.SetFillColor(0);

    int c=0;
    pmloop(avg_prof,b,0) {

        key2[1] = *a;
        key2[0] = *b;
        TH1 *h;
        avg_prof.get(key2,h);

        h->SetLineWidth(2);
        h->SetLineColor(color[c]);
        h->SetMarkerColor(color[c]);
        leg.AddEntry(h,Form("%s N=%.0f",(*b)->str().c_str(),h->GetEntries()));
        if (c==0) {
          h->SetTitle(("Average jet energy profile for pT "+(*a)->str()).c_str());
          h->Draw();
        } else h->Draw("same");
        ++c;

    }

    leg.Draw();
    canv.SaveAs((dir+"avg_prof.pdf").c_str());

  }
  canv.SaveAs((dir+"avg_prof.pdf]").c_str());

  // Close files
  for (short i=0;i<nf;++i) {
    f[i]->Close();
    delete f[i];
  }

  return 0;
}
