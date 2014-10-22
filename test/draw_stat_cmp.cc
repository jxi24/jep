#include <iostream>
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

const short nf_max = 3;

// names, colors, etc.
const char * const names[3] = {"gluon", "quark", "higgs"};
const Color_t color[3] = {2,3,4};

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

  bool operator<(const mkey& other) const {
    if ( pt[0] == other.pt[0] ) {
      if ( pt[1] == other.pt[1] ) {
        return ( pref < other.pref );
      } else return ( pt[1] < other.pt[1] );
    } else return ( pt[0] < other.pt[0] );
  }
};

// draw average profile ***********************************
struct draw_avg_prof {
  string fname;
  draw_avg_prof(const char* fname): fname(fname) { }
  void operator() (const pair<mkey,vector<TH1*> >& e) const {
    const mkey& key = e.first;

    if (!key.pref.compare("avg_prof")) {

      const vector<TH1*>& h = e.second;

      canv.Clear();
      TLegend leg(0.75,0.66,0.95,0.82);
      leg.SetFillColor(0);

      for (size_t i=0, size=h.size(); i<size; ++i) {
        h[i]->SetLineWidth(2);
        h[i]->SetLineColor(color[i]);
        h[i]->SetMarkerColor(color[i]);
        leg.AddEntry(h[i],Form("%s N=%.0f",names[i],h[i]->GetEntries()));
        if (i==0) h[i]->Draw();
        else h[i]->Draw("same");
      }
      h[0]->SetTitle(Form("Average jet energy profile for "
                          "%.0f #leq pt < %.0f",key.pt[0],key.pt[1]));

      leg.Draw();
      canv.SaveAs(fname.c_str());

    }
  }
};

// draw chi2_I by hypotheses ******************************
struct draw_chi2_I_hyp {
  string fname;
  size_t d;
  draw_chi2_I_hyp(const char* fname, short data_file_num)
  : fname(fname), d(data_file_num) { }
  void operator() (const pair<mkey,vector<TH1*> >& e) const {
    const mkey& key = e.first;
    const size_t delim = key.pref.rfind('_');

    if (!key.pref.substr(delim).compare("avg_prof")) {

      TH1 * const h = e.second[h];

      canv.Clear();
      TLegend leg(0.75,0.66,0.95,0.82);
      leg.SetFillColor(0);

      h->SetLineWidth(2);
      h->SetLineColor(color[i]);
      h->SetMarkerColor(color[i]);
      leg.AddEntry(h[i],Form("%s N=%.0f",names[i],h[i]->GetEntries()));
      if (i==0) h[i]->Draw();
      else h[i]->Draw("same");
      h[0]->SetTitle(Form("Average jet energy profile for "
                          "%.0f #leq pt < %.0f",key.pt[0],key.pt[1]));

      leg.Draw();
      canv.SaveAs(fname.c_str());

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
  //typedef map<mkey,vector<TH1*> >::iterator iter;

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

  canv.SaveAs("avg_prof.pdf[");
  for_each(hists.begin(), hists.end(), draw_avg_prof("avg_prof.pdf"));
  canv.SaveAs("avg_prof.pdf]");


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
