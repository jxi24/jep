#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <cmath>
#include <ctime>

#include <TFile.h>
#include <TH1.h>

#include "jep/reader.h"
#include "jep/statistics.h"
#include "jep/common.h"

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

  vector<constit> c; // constituents

public:
  const double  r(size_t i) const { return c[i].r;   }
  const double Et(size_t i) const { return c[i].Et;  }
  const short pid(size_t i) const { return c[i].pid; }
  size_t size() const { return c.size(); }

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

// get jet energy profile
// ********************************************************
bool profile(
  const jet& jet,
  double* profile_, // array of size h.r_num to return jet energy profile
  const jep::header& h, // to get profile parameters
  bool warn=true, // print message if constituents outside tolerance
  bool integral=true, // integral profile
  double tol_frac=1.1)  // fractional tolerance for h.r_max()
{
  const size_t nconst = jet.size();
  const double r_max_tol = h.r_max()*tol_frac;

  if (!nconst) // if no constituents
    cerr << "Jet has no constituents" << endl;

  // variables
  size_t ci = 0; // current constituent
  short  ri = 0; // current radius index
  double rv = h.r_min; // current radius value

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

          profile_[ri] += jet.Et(ci);
        }

        break;
      }
      
      rv += h.r_step;
      profile_[ri] = 0.;
    }
    profile_[ri] += jet.Et(ci);
  }

  if (integral) {
    for (short i=h.r_num-1; i>=0; --i)
      for (short k=0; k<i; ++k) profile_[i] += profile_[k];
    for (short i=0; i<h.r_num; ++i) profile_[i] /= profile_[h.r_num-1];
  } else {
    double integral = 0.;
    for (short i=0; i<h.r_num; ++i) integral += profile_[i];
    for (short i=0; i<h.r_num; ++i) profile_[i] /= integral;
  }

  return true;
}

// MAIN ***************************************************
int main(int argc, char *argv[])
{
  // Arguments ******************************************************
  if ( argc!=4 ) {
    cout << "Usage: " << argv[0]
         << " jets_file.jets profile.jep stat_plots.root" << endl;
    return 0;
  }

  // Read files *****************************************************
  jep::reader* f = new jep::reader(argv[2]);
  const jep::header& h = f->get_header();

  jets_file jets(argv[1]);
  jet j;

  double prof[h.r_num];

  // average profile
  double prof_avg[h.r_num];
  for (jep::num_t i=0; i<h.r_num; ++i) prof_avg[i] = 0.;

  clock_t last_time = clock();
  short num_sec = 0; // seconds elapsed

  cout << "Theory cone R: " << h.r_max() << endl;
  cout << "Data   cone R: " << jets.coneR() << endl;
  cout << "Number of jets: " << jets.njets() << endl;
  cout << endl;

  TFile *fout = new TFile(argv[3],"recreate");

  // Book histograms ************************************************
  TH1F *h_num_const = new TH1F("num_const","",100,0,100);
        h_num_const->SetTitle("Number of jet constituents");
  TH1F *h_avg_prof = new TH1F("avg_prof","",h.r_num,h.r_min-h.r_step,h.r_max());
        h_avg_prof->SetTitle("Average jet energy profile;"
                             "Cone radius, r;Energy fraction, #psi");

  // Loop over jets *************************************************
  long njets = 0; // number of processed jets
  while (jets.next(j)) {

    /*cout << setw(10) << left << "r" << ' ' << "Et" << endl;
    for (size_t i=0, end=j.size(); i<end; ++i) {
      cout << setw(10) << left << j.r(i) << ' ' << j.Et(i) << endl;
    }
    cout << endl;

    profile(j,prof,h);
    for (jep::num_t i=0;i<h.r_num;++i) {
      cout << h.r(i) << ' ' << prof[i] << endl;
    }

    break;*/

    profile(j,prof,h,false);
    for (jep::num_t i=0; i<h.r_num; ++i) prof_avg[i] += prof[i];

    h_num_const->Fill(j.size());

    ++njets;

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

  // Fill histograms ************************************************
  for (jep::num_t i=0; i<h.r_num; ++i) {
    h_avg_prof->SetBinContent(i+1,prof_avg[i]/njets);
  }

  // Clean up *******************************************************
  fout->Write();
  fout->Close();
  delete fout;

  return 0;
}
