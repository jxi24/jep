#include <iostream>
#include <string>
#include <vector>
#include <sstream>

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

const char * const particle_names[3] = {"gluon", "quark", "higgs"};
const Color_t color[] = {2,3,4,5,6,7};

const prop_ptr mc_p     = new prop<string>("mc");
const prop_ptr theory_p = new prop<string>("theory");

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

bool parse_avg_prof_mc(const string& name, vector<prop_ptr>& key) {
  static const boost::regex regex("avg_prof_pt([[:digit:]]*_[[:digit:]]*)");
  static boost::smatch result;
  if ( boost::regex_search(name, result, regex) ) {
    key[2] = new range_p( string(result[1].first, result[1].second) );
    return true;
  } else return false;
}

bool parse_avg_prof_theory(const string& name, vector<prop_ptr>& key) {
  static const boost::regex regex("h_E_([[:digit:]]*_[[:digit:]]*)");
  static boost::smatch result;
  if ( boost::regex_search(name, result, regex) ) {
    key[2] = new range_p( string(result[1].first, result[1].second) );
    return true;
  } else return false;
}

bool parse_file(const string& name, vector<prop_ptr>& key) {
  for (int i=0;i<3;++i) {
    if (name.find(particle_names[i])!=string::npos) {
      key[0] = new prop<string>(particle_names[i]);
    } else if (i==3) {
      cerr << "File name \'"<<name<<"\' does not specify particle" << endl;
      return false;
    }
  }

  if (name.find("mc")!=string::npos) {
    key[1] = mc_p;
  } else
  if (name.find("theory")!=string::npos) {
    key[1] = theory_p;
  }
  else {
    cerr << "File name \'"<<name<<"\' does not specify mc or theory" << endl;
    return false;
  }

  return true;
}

// MAIN ***************************************************
int main(int argc, char *argv[])
{
  // parse arguments
  if ( argc<3 ) {
    cout << "Usage: " << argv[0]
         << " output_plot_file.pdf particle_code.root ..." << endl;
    return 0;
  }

  // property map of root files
  propmap<TFile*> files(2);
  vector<prop_ptr> fkey(2);

  for (int i=2;i<argc;++i) {
    if ( parse_file(argv[i],fkey) ) {
      TFile *f;
      files.insert(fkey, f = new TFile(argv[i],"read") );
      if (f->IsZombie()) return 1;
    } else return 1;
  }

  // property map of histograms
  propmap<TH1*> profiles(3);
  vector<prop_ptr> pkey(3);

  // ******************************************************
  // Read files
  // ******************************************************
  pmloop(files,particle,0) {
    pmloop(files,code,1) {

      fkey[0] = *particle;
      fkey[1] = *code;
      static TFile *f;
      if ( !files.get(fkey,f) ) continue;

      for (size_t i=0;i<2;++i) pkey[i] = fkey[i];

      static TKey *tkey;
      TH1 *hist;
      TIter nextkey(f->GetListOfKeys());
      if (fkey[1] == mc_p) {
        while ((tkey = (TKey*)nextkey())) {
          hist = dynamic_cast<TH1*>( tkey->ReadObj() );
          if ( parse_avg_prof_mc(hist->GetName(),pkey) ) {
            profiles.insert(pkey,hist);
          }
        }
      } else
      if (fkey[1] == theory_p) {
        while ((tkey = (TKey*)nextkey())) {
          hist = dynamic_cast<TH1*>( tkey->ReadObj() );
          if ( parse_avg_prof_theory(hist->GetName(),pkey) ) {
            profiles.insert(pkey,hist);
          }
        }
      }

    }
  }

  gStyle->SetOptStat(0);
  TCanvas canv;

  // ******************************************************
  // Average profiles
  // ******************************************************
  canv.SaveAs(Form("%s[",argv[1]));
  pmloop(profiles,range,2) {

    canv.Clear();
    TLegend leg(0.75,0.82,0.975,0.82);
    leg.SetFillColor(0);

    pmloop(profiles,code,1) {
      int c=0;
      pmloop(profiles,particle,0) {

        pkey[0] = *particle;
        pkey[1] = *code;
        pkey[2] = *range;
        TH1 *h = NULL;
        if ( profiles.get(pkey,h) ) {

          h->SetLineWidth(2);
          h->SetLineColor(color[c]);
          h->SetMarkerColor(color[c]);
          stringstream leg_ent;
          leg_ent << pkey[0]->str() <<' '<< pkey[1]->str();
          if (pkey[1]==theory_p) h->SetLineStyle(2);
          else leg_ent <<" N="<< h->GetEntries();
          leg.AddEntry(h,leg_ent.str().c_str());
          if (c==0 && pkey[1]==mc_p) {
            h->SetTitle(("Average jet energy profile for pT "+pkey[2]->str()).c_str());
            h->Draw();
          } else h->Draw("same");
          leg.SetY1( leg.GetY1()-0.05 );
          ++c;

        }
      }
    }

    leg.Draw();
    canv.SaveAs(argv[1]);

  }
  canv.SaveAs(Form("%s]",argv[1]));

  // Close files
  pmloop(files,particle,0) {
    pmloop(files,code,1) {
      fkey[0] = *particle;
      fkey[1] = *code;
      static TFile *f = NULL;
      if ( files.get(fkey,f) ) delete f;
    }
  }

  return 0;
}
