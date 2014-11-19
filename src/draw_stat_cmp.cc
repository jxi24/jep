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
         << " out_dir particle_mc.root ..." << endl;
    return 0;
  }

  string dir(argv[1]);
  if (dir[dir.size()-1]!='/') dir += '/';

  // property map of root files
  boost::unordered_map<string,TFile*> files;

  for (int i=2;i<argc;++i) {
    string name(argv[i]);
    for (int j=0;j<3;++j) {
      if (name.find(particle_names[j])!=string::npos) {
        TFile *f = new TFile(argv[i],"read");
        if (f->IsZombie()) return 1;
        files[particle_names[j]] = f;
      } else if (j==3) {
        cerr << "File name \'"<<name<<"\' does not specify particle" << endl;
        return 1;
      }
    }
  }

  // property map of histograms
  propmap<TH1*> stat(4);
  vector<prop_ptr> hkey(4);

  // ******************************************************
  // Read files
  // ******************************************************
  for (boost::unordered_map<string,TFile*>::iterator
       it=files.begin(), end=files.end(); it!=end; ++it)
  {
    hkey[0] = new prop<string>(it->first);

    static TKey *fkey;
    TIter nextkey(it->second->GetListOfKeys());
    while ((fkey = (TKey*)nextkey())) {
      TH1 *hist = dynamic_cast<TH1*>( fkey->ReadObj() );
      if ( parse_stat(hist->GetName(),hkey) ) {
        stat.insert(hkey,hist);
      }
    }
  }

  gStyle->SetOptStat(0);
  TCanvas canv;

  // ******************************************************
  // Compare hypotheses for the same pseudo-data
  // ******************************************************
  pmloop(stat,origin,0) {
    hkey[0] = *origin;
    pmloop(stat,method,1) {
      hkey[1] = *method;
      string title = hkey[0]->str()+" pseudo-data: "+hkey[1]->str();
      string file_name = dir+"origin_"+hkey[0]->str()+'_'+hkey[1]->str()+".pdf";

      canv.SaveAs((file_name+'[').c_str());
      pmloop(stat,pt,3) {
        hkey[3] = *pt;

        canv.Clear();
        TLegend leg(0.75,0.77,0.95,0.82);
        leg.SetFillColor(0);
        leg.SetHeader("Hypotheses:");

        double xmin=0, xmax=0;
        double ymin=0, ymax=0;
        vector<TH1*> h_;
        pmloop(stat,hypoth,2) {
          hkey[2] = *hypoth;

          h_.push_back(NULL);
          const size_t hi = h_.size()-1;
          TH1*& h = h_[hi];
          stat.get(hkey,h);

          h->SetLineWidth(2);
          h->SetLineColor(color[hi]);
          h->SetMarkerColor(color[hi]);
          leg.AddEntry(h,hkey[2]->str().c_str());
          leg.SetY1( leg.GetY1()-0.05 );
          if (hi==0) {
            xmin = h->GetXaxis()->GetXmin();
            xmax = h->GetXaxis()->GetXmax();
            ymin = min_in_range(h,xmin,xmax);
            ymax = max_in_range(h,xmin,xmax);
          } else {
            double min = min_in_range(h,xmin,xmax),
                   max = max_in_range(h,xmin,xmax);
            if (min<ymin) ymin = min;
            if (max>ymax) ymax = max;
          }

        }

        h_[0]->SetTitle(Form("%s N=%.0f",( title+" pT "+hkey[3]->str() ).c_str(),h_[0]->GetEntries()));
        h_[0]->SetAxisRange(ymin*1.05,ymax*1.05,"Y");
        h_[0]->Draw();
        for (size_t i=1, size=h_.size(); i<size; ++i) {
          h_[i]->Draw("same");
        }

        leg.Draw();
        canv.SaveAs(file_name.c_str());

      }
      canv.SaveAs((file_name+']').c_str());
    }
  }

  // ******************************************************
  // Compare the same hypothesis for different pseudo-data
  // ******************************************************
  pmloop(stat,hypoth,2) {
    hkey[2] = *hypoth;
    pmloop(stat,method,1) {
      hkey[1] = *method;
      string title = hkey[2]->str()+" hypothesis: "+hkey[1]->str();
      string file_name = dir+"hypoth_"+hkey[2]->str()+'_'+hkey[1]->str()+".pdf";

      canv.SaveAs((file_name+'[').c_str());
      pmloop(stat,pt,3) {
        hkey[3] = *pt;

        canv.Clear();
        TLegend leg(0.75,0.77,0.95,0.82);
        leg.SetFillColor(0);
        leg.SetHeader("Pseudo-data:");

        double xmin=0, xmax=0;
        double ymin=0, ymax=0;
        vector<TH1*> h_;
        pmloop(stat,origin,0) {
          hkey[0] = *origin;

          h_.push_back(NULL);
          const size_t hi = h_.size()-1;
          TH1*& h = h_[hi];
          stat.get(hkey,h);

          h->SetLineWidth(2);
          h->SetLineColor(color[hi]);
          h->SetMarkerColor(color[hi]);
          leg.AddEntry(h,Form("%s N=%.0f",hkey[0]->str().c_str(),h->GetEntries()));
          leg.SetY1( leg.GetY1()-0.05 );
          if (hi==0) {
            xmin = h->GetXaxis()->GetXmin();
            xmax = h->GetXaxis()->GetXmax();
            ymin = min_in_range(h,xmin,xmax);
            ymax = max_in_range(h,xmin,xmax);
          } else {
            double min = min_in_range(h,xmin,xmax),
                   max = max_in_range(h,xmin,xmax);
            if (min<ymin) ymin = min;
            if (max>ymax) ymax = max;
          }

        }

        h_[0]->SetTitle(( title+" pT "+hkey[3]->str() ).c_str());
        h_[0]->SetAxisRange(ymin*1.05,ymax*1.05,"Y");
        h_[0]->Draw();
        for (size_t i=1, size=h_.size(); i<size; ++i) {
          h_[i]->Draw("same");
        }

        leg.Draw();
        canv.SaveAs(file_name.c_str());

      }
      canv.SaveAs((file_name+']').c_str());
    }
  }

  return 0;
}
