#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <utility>
#include <map>
#include <ctime>
#include <exception>
#include <stdexcept>
#include <cmath>

#include <TFile.h>
#include <TH1.h>

#include "jep/reader.h"

#define test(var) \
  cout <<"\033[36m"<< #var <<"\033[0m"<< " = " << var << endl;

using namespace std;

// Read jets file *****************************************

class jets_file;

class jet {

  struct constit {
    double r, Et;
    short pid;
  };

  double _Et;
  vector<constit> c; // constituents

public:
  const double  r(size_t i) const { return c[i].r;   }
  const double Et(size_t i) const { return c[i].Et;  }
  const short pid(size_t i) const { return c[i].pid; }
  size_t size() const { return c.size(); }

  const double Et() const { return _Et; }

friend class jets_file;
};

class jets_file {
  ifstream dat;
  size_t _cur_jet;
  float  _cone_r;
  size_t _num_jets;

  static const streamsize
    flt_size, dbl_size, srt_size, szt_size;

public:
  jets_file(const char* filename)
  : dat(filename, std::ifstream::binary), _cur_jet(0)
  {
    dat.read((char*)&_cone_r,   flt_size);
    dat.read((char*)&_num_jets, szt_size);
  }

  size_t njets() const { return _num_jets; }
  size_t   cur() const { return _cur_jet;  }
  float  coneR() const { return _cone_r;   }

  bool next(jet& j) {
    ++_cur_jet;

    if (_cur_jet>_num_jets) return false;

    // read jet Et
    double jet_Et;
    dat.read((char*)&jet_Et, dbl_size);
    j._Et = jet_Et;

    j.c.clear(); // clear jet constituents

    size_t nc; // number of constituents
    jet::constit c; // current constituent

    dat.read((char*)&nc, szt_size);
    for (size_t i=0; i<nc; ++i) {
      dat.read((char*)&c.r,   dbl_size);
      dat.read((char*)&c.Et,  dbl_size);
      dat.read((char*)&c.pid, srt_size);
      j.c.push_back(c);
    }

    return true;
  }
};

const streamsize jets_file::flt_size = sizeof(float);
const streamsize jets_file::dbl_size = sizeof(double);
const streamsize jets_file::srt_size = sizeof(short);
const streamsize jets_file::szt_size = sizeof(size_t);

// histogram wrapper
// ********************************************************
class hist {
  struct binning {
    int nbins; double min, max;
    binning(): nbins(100), min(0.), max(100.) { }
  };
  static map<string,binning> binnings;
  static vector<const hist*> all;

  binning b;
  pair<int,double> underflow, overflow;
  TH1F * h;

public:
  hist(): b(), underflow(0,0.), overflow(0,0.), h(NULL) { }

  hist(const string& name,const string& title)
  : b(binnings[name]), underflow(0,b.min), overflow(0,b.max),
    h( new TH1F(name.c_str(),title.c_str(),b.nbins,b.min,b.max) )
  {
    all.push_back(this);
  }
  void Fill(double x) {
    h->Fill(x);
    if (x>=b.max) {
      ++overflow.first;
      if (x>overflow.second) overflow.second = x;
    }
    else if (x<b.min) {
      ++underflow.first;
      if (x<underflow.second) underflow.second = x;
    }
  }

  static void read_binnings(const char* filename) {
    ifstream binsfile(filename);

    string hname;
    binning b;
    while ( binsfile >> hname ) {
      binsfile >> b.nbins;
      binsfile >> b.min;
      binsfile >> b.max;

      binnings[hname] = b;
    }
  }

  static void finish() {
    for (vector<const hist*>::iterator it=all.begin(), end=all.end(); it<end; ++it) {
      const hist* h = *it;
      if (h->underflow.first) {
        cout << "Underflow in " << h->h->GetName()
             << ": N="<<h->underflow.first << " min="<<h->underflow.second << endl;
      }
      if (h->overflow.first) {
        cout << "Overflow in " << h->h->GetName()
             << ": N="<<h->overflow.first << " max="<<h->overflow.second << endl;
      }
      delete h;
    }
  }
};
map<string,hist::binning> hist::binnings;
vector<const hist*> hist::all;

// get jet energy profile
// ********************************************************
bool profile(
  const jet& jet,
  jep::val_t* integral_, // array of size h.r_num to return jet energy profile
  jep::val_t* derivative_, // array of size h.r_num to return jet energy profile
  const jep::header& h, // to get profile parameters
  bool warn=true, // print message if constituents outside tolerance
  jep::val_t tol_frac=1.1)  // fractional tolerance for h.r_max()
{
  const size_t nconst = jet.size();
  const jep::val_t r_max_tol = h.r_max()*tol_frac;

  if (!nconst) // if no constituents
    throw runtime_error("Jet has no constituents");

  // variables
  size_t     ci = 0; // current constituent
  jep::num_t ri = 0; // current radius index
  jep::val_t rv = h.r_min; // current radius value

  for (; ci<nconst; ++ci) {
    if (jet.r(ci)>rv) {
      ++ri;
      if (ri==h.r_num) { // handle overflow

        --ri;
        for (; ci<nconst; ++ci) {
          if (jet.r(ci)>r_max_tol) {
            if (warn) cerr << "Jet constituents outside cone" << endl;
            return false;
          }

          derivative_[ri] += jet.Et(ci);
        }

        break;
      }
      
      rv += h.r_step;
      derivative_[ri] = 0.;
      integral_[ri] = 0.;
    }
    derivative_[ri] += jet.Et(ci);
  }

  // integrate
  for (short i=0; i<h.r_num; ++i)
    for (short k=0; k<=i; ++k) integral_[i] += derivative_[k];

  // normalize
  for (short i=0; i<h.r_num; ++i) {
    derivative_[i] /= integral_[h.r_num-1];
    integral_  [i] /= integral_[h.r_num-1];
  }

  return true;
}

// statistics *********************************************
template<typename D, typename H> // D & H dereference to double
double statistic(D data, H hypoth, int n, void(*fcn)(double&,double,double),
                 double start=0.) {
  double stat = start;
  D di = data;
  H hi = hypoth;
  for (int i=0; i<n; ++i) {
    fcn(stat,*di,*hi); // accumulate statistic
    ++di; // increment iterators
    ++hi;
  }
  return stat;
}

inline double sq(double x) { return x*x; }

void chi2(double& stat, double data, double hypoth) {
  stat += sq(hypoth - data)/hypoth;
}

void like(double& stat, double data, double hypoth) {
  if ( data>0. && hypoth>0. )
    stat *= hypoth/data;
}

typedef vector< pair<string,jep::reader*> >::iterator fjep_iter;

// MAIN ***************************************************
int main(int argc, char *argv[])
{
  // Arguments ******************************************************
  if ( argc<5 ) {
    cout << "Usage: " << argv[0]
         << " jets_file.jets binning.bins stat_plots.root "
            "gluon:profile1.jep quark:profile2.jep ..." << endl;
    return 0;
  }

  // Read files *****************************************************
  vector< pair<string,jep::reader*> > fjep;

  { // Parse jep files arguments
    map<string,string> fjepnames;

    for (int i=4;i<argc;++i) {
      string arg(argv[i]);
      size_t col = arg.find_first_of(':');
      if (col==string::npos) {
        cerr << "Bad argument " << i << ": " << arg << endl;
        return 1;
      }
      fjepnames[arg.substr(0,col)] = arg.substr(col+1);
    }

    cout << "\nJet energy profiles:" << endl;
    for (map<string,string>::iterator it=fjepnames.begin(),
         end=fjepnames.end(); it!=end; ++it)
    {
      cout << "  " << it->first << ": " << it->second << endl;
      fjep.push_back( make_pair(
        it->first, new jep::reader(it->second.c_str())
      ) );
    }
    cout << endl;
  }

  // reference to the first header
  const jep::header& h = fjep.begin()->second->get_header();

  // check jep headers compatibility
  for (fjep_iter it=fjep.begin()+1, end=fjep.end(); it!=end; ++it) {
    if ( h != it->second->get_header() ) {
      cerr << "Incompatible jep file headers" << endl;
      return 1;
    }
  }

  jets_file jets(argv[1]);
  jet j;

  // current profile
  jep::val_t prof[h.r_num], dprof[h.r_num];

  // average profile
  jep::val_t prof_avg[h.r_num];
  for (jep::num_t i=0; i<h.r_num; ++i) prof_avg[i] = 0.;

  clock_t last_time = clock();
  short num_sec = 0; // seconds elapsed

  cout << "Theory cone R: " << h.r_max() << endl;
  cout << "Data   cone R: " << jets.coneR() << endl;

  TFile *fout = new TFile(argv[3],"recreate");

  // Book histograms ************************************************
  hist::read_binnings(argv[2]);

  hist *h_num_const = new hist("num_const","Number of jet constituents");

  TH1F *h_avg_prof = new TH1F("avg_prof","",h.r_num,h.r_min-h.r_step,h.r_max());
        h_avg_prof->SetTitle("Average jet energy profile;"
                             "Cone radius, r;Energy fraction, #psi");
        h_avg_prof->SetStats(0);

  map<fjep_iter,hist*> stat_chi2_I, stat_chi2_d,
                       stat_like_I, stat_like_d;

  for (fjep_iter it=fjep.begin(), end=fjep.end(); it!=end; ++it) {
    stat_chi2_I[it] = new hist(
      "chi2_I_"+it->first,
      "#chi^{2} for "+it->first+" hypothesis with integral profile"
      ";#chi^{2};"
    );

    stat_chi2_d[it] = new hist(
      "chi2_d_"+it->first,
      "#chi^{2} for "+it->first+" hypothesis with differential profile"
      ";#chi^{2};"
    );

    stat_like_I[it] = new hist(
      "like_I_"+it->first,
      "log L for "+it->first+" hypothesis with integral profile"
      ";log L;"
    );

    stat_like_d[it] = new hist(
      "like_d_"+it->first,
      "log L for "+it->first+" hypothesis with differential profile"
      ";log L;"
    );
  }

  ofstream errlog("stat2_log.txt");

  // Loop over jets *************************************************
  cout << "Number of jets: " << jets.njets() << endl;
  cout << endl;
  long njets = -1; // total number jets
  long ngood =  0; // number of processed jets
  while (jets.next(j)) {
    ++njets;

    // get current jet profile
    try {

      profile(j,prof,dprof,h,false);

    } catch (exception& e) {
      errlog << setw(6) << njets << ' ' << e.what() << endl;
      continue;
    }

/*
    cout << "jet Et = " << j.Et() << endl << endl;

    cout << setw(10) << left << "r" << ' ' << "Et" << endl;
    for (size_t i=0, end=j.size(); i<end; ++i) {
      cout << setw(10) << left << j.r(i) << ' ' << j.Et(i) << endl;
    }
    cout << endl;

    for (jep::num_t i=0;i<h.r_num;++i) {
      cout << h.r(i) << ' ' << prof[i] << endl;
    }
    cout << endl;

    for (jep::num_t i=0;i<h.r_num;++i) {
      cout << h.r(i) << ' ' << dprof[i] << endl;
    }
    cout << endl;

    break;
*/

    // average jet profile
    for (jep::num_t i=0; i<h.r_num; ++i) prof_avg[i] += prof[i];

    // calculate statistics
    bool process_jet = true;
    for (fjep_iter it=fjep.begin(), end=fjep.end(); it!=end; ++it) {
      try {
        const vector<jep::val_t> hypoth = it->second->psi(j.Et());

        jep::val_t dhypoth[h.r_num];
        dhypoth[0] = hypoth[0];
        for (jep::num_t k=h.r_num-1; k>0; --k) {
          dhypoth[k] = hypoth[k] - hypoth[k-1];
        }

        stat_chi2_I[it]->Fill( // integral chi2
          statistic(prof, hypoth.begin(), h.r_num, chi2)
        );

        stat_chi2_d[it]->Fill( // derivative chi2
          statistic(dprof, dhypoth, h.r_num, chi2)
        );

        stat_like_I[it]->Fill( // integral log-likelihood
          log( statistic(prof, hypoth.begin(), h.r_num, like, 1.) )
        );

        stat_like_d[it]->Fill( // integral log-likelihood
          log( statistic(dprof, dhypoth, h.r_num, like, 1.) )
        );

      } catch (exception& e) {
        errlog << setw(6) << njets << ' ' << e.what() << endl;
        process_jet = false;
        break;
      }
    }
    if (!process_jet) continue; // skip jet

    // Fill histograms
    h_num_const->Fill(j.size());

    // Increment successful jets counter
    ++ngood;

    // timed counter
    if ( (clock()-last_time)/CLOCKS_PER_SEC > 1 ) {
      ++num_sec;
      cout << setw(10) << jets.cur()
           << setw( 7) << num_sec << 's';
      cout.flush();
      for (char i=0;i<19;++i) cout << '\b';
      last_time = clock();
    }

  }
  cout << right;
  cout << setw(10) << jets.njets()
       << setw( 7) << num_sec << 's' << endl << endl;
  cout << "Processed " << ngood << " jets" << endl;

  // Histograms *****************************************************
  for (jep::num_t i=0; i<h.r_num; ++i) {
    h_avg_prof->SetBinContent(i+1,prof_avg[i]/njets);
  }

  hist::finish();

  // Clean up *******************************************************
  fout->Write();
  fout->Close();
  delete fout;

  for (fjep_iter it=fjep.begin(), end=fjep.end(); it!=end; ++it)
    delete it->second;

  errlog.close();

  return 0;
}
