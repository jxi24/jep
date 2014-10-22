#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include <TFile.h>
#include <TKey.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>

using namespace std;

#define test(var) \
  cout <<"\033[36m"<< #var <<"\033[0m"<< " = " << var << endl;

TCanvas canv;

// key class for map of histograms
// so they hists are ordered by pt ************************
struct mkey {
  string name, pref;
  double pt[2];

  mkey(const string& name): name(name) {
    // get pt bin edges from the name
    size_t _pt = this->name.find("_pt");
    pref = this->name.substr(0,_pt);
    string pt_str = this->name.substr(_pt+3);
    size_t delim = pt_str.find('_');
    pt[0] = atof( pt_str.substr(0,delim).c_str() );
    pt_str = pt_str.substr(delim+1);
    pt[1] = atof( pt_str.substr(0,pt_str.find('_')).c_str() );
  }

  // comparison operator to sort hists in map by pt
  bool operator<(const mkey& other) const {
    if ( pt[0] == other.pt[0] ) {
      if ( pt[1] == other.pt[1] ) {
        if ( pref == other.pref ) {
          return ( name < other.name );
        } else return ( pref < other.pref );
      } else return ( pt[1] < other.pt[1] );
    } else return ( pt[0] < other.pt[0] );
  }
};

// base hist selector class *******************************
class hist_select {
protected:
  vector< pair<string,vector<TH1*> > > h_;

  static const char * const names[];
  static const Color_t color[];

public:
  hist_select() { }
  virtual ~hist_select() { }

  typedef pair<mkey,vector<TH1*> > el;
  virtual void operator() (const el& e) =0;

  virtual void save(const string& fname) {

    canv.SaveAs((fname+'[').c_str());
    for (size_t a=0, an=h_.size(); a<an; ++a) {

      canv.Clear();
      TLegend leg(0.75,0.66,0.95,0.82);
      leg.SetFillColor(0);

      vector<TH1*>& vh = h_[a].second;
      for (size_t b=0, bn=vh.size(); b<bn; ++b) {
        TH1 *h = vh[b];

        h->SetLineWidth(2);
        h->SetLineColor(color[b]);
        h->SetMarkerColor(color[b]);
        leg.AddEntry(h,Form("%s N=%.0f",names[b],h->GetEntries()));
        if (b==0) {
          h->SetTitle(h_[a].first.c_str());
          h->Draw();
        } else h->Draw("same");
      }

      leg.Draw();
      canv.SaveAs(fname.c_str());

    }
    canv.SaveAs((fname+']').c_str());
  }
};
const char * const hist_select::names[] = {"gluon", "quark", "higgs"};
const Color_t hist_select::color[] = {2,3,4,5,6,7};

// select average profile hists ***************************
class avg_prof: public hist_select {
public:
  avg_prof(): hist_select() { }
  virtual ~avg_prof() { }
  virtual void operator() (const el& e) {
    const mkey& key = e.first;

    if (!key.pref.compare("avg_prof")) {
      stringstream ss;
      ss << "Average jet energy profile for "
         << key.pt[0] << " #leq pt < " << key.pt[1];

      h_.push_back( make_pair( ss.str(), e.second ) );
    }
  }
};

// select chi2_I by hypotheses ****************************
class chi2_I_hyp: public hist_select {
public:
  chi2_I_hyp(): hist_select() { }
  virtual ~chi2_I_hyp() { }
  virtual void operator() (const el& e) {
    const mkey& key = e.first;
    const size_t delim = key.pref.rfind('_');

    if (!key.pref.substr(delim).compare("chi2_I")) {
      test(key.name)
    }
  }
};

// ********************************************************
/*struct testfcn {
  void operator() (const pair<mkey,vector<TH1*> >& e) {
    test(e.first.name)
    test(e.first.pref)
    test(e.second.size())
  }
};*/

// MAIN ***************************************************
int main(int argc, char *argv[])
{
  // parse arguments
  if ( argc<2 ) {
    cout << "Usage: " << argv[0]
         << " gluon.root quark.root higgs.root" << endl;
    return 0;
  }
  const short nf = argc-1;

  // open files
  TFile* f[nf];
  for (short i=0;i<nf;++i) {
    f[i] = new TFile(argv[i+1],"read");
    if (f[i]->IsZombie()) return 1;
  }

  // container of histograms
  map<mkey,vector<TH1*> > hists;

  // read files
  for (short i=0;i<nf;++i) {
    TKey *key;
    TIter nextkey(f[i]->GetListOfKeys());
    while ((key = (TKey*)nextkey())) {
      TH1 *hist = dynamic_cast<TH1*>( key->ReadObj() );
      string name(hist->GetName());
      if (name.find("_pt")==string::npos) continue;
      hists[name].push_back(hist);
    }
  }

  gStyle->SetOptStat(0);

  for_each(hists.begin(), hists.end(), avg_prof()).save("avg_prof.pdf");


/*
  double ymin = h[0]->GetMinimum(), ymax = h[0]->GetMaximum();
  for (size_t i=1, size=h.size(); i<size; ++i) {
    double min = h[i]->GetMinimum(), max = h[i]->GetMaximum();
    if (min<ymin) ymin = min;
    if (max>ymax) ymax = max;
  }
  h[0]->SetAxisRange(ymin*1.05,ymax*1.05,"Y");
  h[0]->SetTitle(argv[3]);
*/

  for (short i=0;i<nf;++i) {
    f[i]->Close();
    delete f[i];
  }

  return 0;
}
