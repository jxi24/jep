#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <stdexcept>

#include <boost/regex.hpp>
#include <boost/unordered_map.hpp>

#include <TFile.h>
#include <TKey.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>

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
    key[1] = new range_p( string(result[1].first, result[1].second) );

    //for (size_t i=0;i<2;++i) test(key[i]->str())

    return true;
  } else return false;
}

bool parse_stat(const string& name, vector<prop_ptr>& key) {
  static const boost::regex regex(
    "([[:alnum:]]*_[dI])_([[:alpha:]]*)_pt([[:digit:]]*_[[:digit:]]*)");
  static boost::smatch result;
  if ( boost::regex_search(name, result, regex) ) {
    key[1] = new prop<string>( string(result[1].first, result[1].second) );
    key[2] = new prop<string>( string(result[2].first, result[2].second) );
    key[3] = new range_p     ( string(result[3].first, result[3].second) );

    return true;
  } else return false;
}

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

  // trackers of histograms
  propmap<TH1*> avg_prof(2);
  propmap<TH1*> stat(4);

  vector<prop_ptr> key2(2);
  vector<prop_ptr> key4(4);

  key2[0] = new prop<string>("gluon");
  key4[0] = new prop<string>("gluon");

  test("test")

  // read files
  for (short i=0;i<nf;++i) {
    static TKey *fkey;
    TIter nextkey(f[i]->GetListOfKeys());
    while ((fkey = (TKey*)nextkey())) {
      TH1 *hist = dynamic_cast<TH1*>( fkey->ReadObj() );
      if ( parse_avg_prof(hist->GetName(),key2) ) {
        avg_prof.insert(key2,hist);
        continue;
      }
      if ( parse_stat(hist->GetName(),key4) ) {
        stat.insert(key4,hist);
        continue;
      }
    }
  }

/*
  for (tr::iter a=avg_prof.begin(1), ae=avg_prof.end(1); a!=ae; ++a)
    for (tr::iter b=avg_prof.begin(0), be=avg_prof.end(0); b!=be; ++b) {
      cout << *a << " " << *b << " : ";

      static tr::key key(2);
      key[1] = *a;
      key[0] = *b;
      cout << avg_prof[key]->GetName() << endl;
    }
*/
/*
  for (tr::iter a=stat.begin(1), ae=stat.end(1); a!=ae; ++a)
    for (tr::iter b=stat.begin(2), be=stat.end(2); b!=be; ++b)
      cout << *a << " " << *b << endl;
*/

  gStyle->SetOptStat(0);
  TCanvas canv;

  canv.SaveAs("avg_prof.pdf[");
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
          h->SetTitle(("Average jet energy profile for "+(*a)->str()).c_str());
          h->Draw();
        } else h->Draw("same");
        ++c;

    }

    leg.Draw();
    canv.SaveAs("avg_prof.pdf");

  }
  canv.SaveAs("avg_prof.pdf]");




//  for_each(hists.begin(), hists.end(), avg_prof()).save("avg_prof.pdf");

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
