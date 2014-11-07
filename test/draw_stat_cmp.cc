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
    key[1] = new range_p( string(result[1].first, result[1].second) );
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
         << " gluon.root quark.root higgs.root ... out_dir" << endl;
    return 0;
  }

  string dir(argv[argc-1]);
  if (dir.substr(dir.size()-6).compare(".root")) {
    if (dir[dir.size()-1]!='/') dir += '/';
  } else dir = string();

  const short nf = ( dir.size() ? argc-2 : argc-1 );

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

  // read files
  for (short i=0;i<nf;++i) {
    switch (i) {
      case 0: {
        key2[0] = new prop<string>("gluon");
        key4[0] = new prop<string>("gluon");
      } break;
      case 1: {
        key2[0] = new prop<string>("quark");
        key4[0] = new prop<string>("quark");
      } break;
      case 2: {
        key2[0] = new prop<string>("higgs");
        key4[0] = new prop<string>("higgs");
      } break;
      default: return 1; break;
    }

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

  // ******************************************************
  // Compare hypotheses for the same pseudo-data
  // ******************************************************
  pmloop(stat,origin,0) {
    key4[0] = *origin;
    pmloop(stat,method,1) {
      key4[1] = *method;
      string title = key4[0]->str()+" pseudo-data: "+key4[1]->str();
      string file_name = dir+"origin_"+key4[0]->str()+'_'+key4[1]->str()+".pdf";

      canv.SaveAs((file_name+'[').c_str());
      pmloop(stat,pt,3) {
        key4[3] = *pt;

        canv.Clear();
        TLegend leg(0.75,0.66,0.95,0.82);
        leg.SetFillColor(0);
        leg.SetHeader("Hypotheses:");

        double xmin, xmax;
        double ymin, ymax;
        vector<TH1*> h_;
        pmloop(stat,hypoth,2) {
          key4[2] = *hypoth;

          h_.push_back(NULL);
          const size_t hi = h_.size()-1;
          TH1*& h = h_[hi];
          stat.get(key4,h);

          h->SetLineWidth(2);
          h->SetLineColor(color[hi]);
          h->SetMarkerColor(color[hi]);
          leg.AddEntry(h,Form("%s N=%.0f",key4[2]->str().c_str(),h->GetEntries()));
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

        h_[0]->SetTitle(( title+" pT "+key4[3]->str() ).c_str());
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
    key4[2] = *hypoth;
    pmloop(stat,method,1) {
      key4[1] = *method;
      string title = key4[2]->str()+" hypothesis: "+key4[1]->str();
      string file_name = dir+"hypoth_"+key4[2]->str()+'_'+key4[1]->str()+".pdf";

      canv.SaveAs((file_name+'[').c_str());
      pmloop(stat,pt,3) {
        key4[3] = *pt;

        canv.Clear();
        TLegend leg(0.75,0.66,0.95,0.82);
        leg.SetFillColor(0);
        leg.SetHeader("Pseudo-data:");

        double xmin, xmax;
        double ymin, ymax;
        vector<TH1*> h_;
        pmloop(stat,origin,0) {
          key4[0] = *origin;

          h_.push_back(NULL);
          const size_t hi = h_.size()-1;
          TH1*& h = h_[hi];
          stat.get(key4,h);

          h->SetLineWidth(2);
          h->SetLineColor(color[hi]);
          h->SetMarkerColor(color[hi]);
          leg.AddEntry(h,Form("%s N=%.0f",key4[0]->str().c_str(),h->GetEntries()));
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

        h_[0]->SetTitle(( title+" pT "+key4[3]->str() ).c_str());
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

  for (short i=0;i<nf;++i) {
    f[i]->Close();
    delete f[i];
  }

  return 0;
}
