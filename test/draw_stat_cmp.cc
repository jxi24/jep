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

using namespace std;

#define test(var) \
  cout <<"\033[36m"<< #var <<"\033[0m"<< " = " << var << endl;

const char * const data_names[] = {"gluon", "quark", "higgs"};
const Color_t color[] = {2,3,4,5,6,7};

// key class for map of histograms ************************
// ******
template <typename T>
class tracker {
public:
  typedef set<string>::iterator iter;
  typedef vector<string> key;

private:
  size_t n, pos;
  const boost::regex regex;

  vector< set<string> > tokens;
  boost::unordered_map<key,T> _map;

public:
  tracker(size_t n, size_t pos, const string& pattern)
  : n(n), pos(pos), regex(pattern), tokens(n) { }

  bool match(const string& name, key& k, const T& x) {
    static boost::smatch result;
    if ( boost::regex_search(name, result, regex) ) {
      for (size_t i=1,ni=result.size();i<ni;++i)
        k[pos+i-1] = string(result[i].first, result[i].second);

      _map[k] = x;
      for (size_t i=0;i<n;++i) {
        tokens[i].insert( k[i] );
      }

      return true;
    } else return false;
  }

  iter begin(size_t i) const { return tokens.at(i).begin(); }
  iter   end(size_t i) const { return tokens.at(i).end();   }

  T& operator[](const key& k) {
    if ( _map.count(k)==0 ) {
      stringstream ss;
      ss << "map has no key:";
      for (size_t i=0;i<n;++i) ss << " ("<<i<<')' << k[i];
      throw runtime_error(ss.str());
    } else return _map[k];
  }
};

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
  typedef tracker<TH1*> tr;
  tr avg_prof(2,1,
    "avg_prof_(pt[[:digit:]]*_[[:digit:]]*)"
  );
  tr stat(4,1,
    "([[:alnum:]]*_[dI])_([[:alpha:]]*)_(pt[[:digit:]]*_[[:digit:]]*)"
  );

  // read files
  for (short i=0;i<nf;++i) {
    static tr::key mkey2(2), mkey4(4);
    mkey2[0] = data_names[i];

    static TKey *fkey;
    TIter nextkey(f[i]->GetListOfKeys());
    while ((fkey = (TKey*)nextkey())) {
      TH1 *hist = dynamic_cast<TH1*>( fkey->ReadObj() );
      if ( avg_prof.match(hist->GetName(),mkey2,hist) ) continue;
      if (     stat.match(hist->GetName(),mkey4,hist) ) continue;
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
  for (tr::iter a=avg_prof.begin(1), ae=avg_prof.end(1); a!=ae; ++a) {

    canv.Clear();
    TLegend leg(0.75,0.66,0.95,0.82);
    leg.SetFillColor(0);

    int c=0;
    for (tr::iter b=avg_prof.begin(0), be=avg_prof.end(0); b!=be; ++b) {

        static tr::key key(2);
        key[1] = *a;
        key[0] = *b;
        TH1 *h = avg_prof[key];

        h->SetLineWidth(2);
        h->SetLineColor(color[c]);
        h->SetMarkerColor(color[c]);
        leg.AddEntry(h,Form("%s N=%.0f",b->c_str(),h->GetEntries()));
        if (c==0) {
          h->SetTitle(("Average jet energy profile for "+*a).c_str());
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
