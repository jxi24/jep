#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <utility>
#include <ctime>

#include <TLorentzVector.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TAxis.h>
#include <TLegend.h>

#include "jep/reader.h"
#include "jep/statistics.h"
#include "jep/common.h"

#define test(var) \
  cout <<"\033[36m"<< #var <<"\033[0m"<< " = " << var << endl;

using namespace std;
using namespace jep;

class jets_file;

class jet {
  TLorentzVector j; // jet 4-momentum
  vector< pair<TLorentzVector,int> > c; // constituents' 4-momenta & pid

public:
  const TLorentzVector& mom() const { return j; }
  const TLorentzVector& operator[](size_t i) const { return c[i].first; }
  const int pid(size_t i) const { return c[i].second; }
  size_t size() const { return c.size(); }

  double Et(size_t i) const { return c[i].first.Et(); }
  double  r(size_t i) const { return j.DeltaR(c[i].first); }

friend class jets_file;
};

class jets_file {
  ifstream dat;
  size_t _cur_jet;
  float  _cone_r;
  size_t _num_jets;

  static const streamsize
    mom_size, int_size, szt_size;

public:
  jets_file(const char* filename)
  : dat(filename, std::ifstream::binary), _cur_jet(0)
  {
    dat.read((char*)&_cone_r, sizeof(float));
    dat.read((char*)&_num_jets, szt_size);
  }

  size_t njets() const { return _num_jets; }
  size_t   cur() const { return _cur_jet;  }
  float  coneR() const { return _cone_r;   }

  bool next(jet& j) {
    ++_cur_jet;

    if (_cur_jet>=_num_jets) return false;

    j.c.clear();

    double mom[4];
    size_t n;
    int pid;

    dat.read((char*)&mom, mom_size);
    j.j = TLorentzVector(mom[1],mom[2],mom[3],mom[0]);

    dat.read((char*)&n, szt_size);
    for (size_t i=0; i<n; ++i) {
      dat.read((char*)&mom, mom_size);
      dat.read((char*)&pid, int_size);
      j.c.push_back(
        make_pair( TLorentzVector(mom[1],mom[2],mom[3],mom[0]),
                   pid )
      );
    }

    return true;
  }
};

const streamsize jets_file::mom_size = sizeof(double)*4;
const streamsize jets_file::int_size = sizeof(int);
const streamsize jets_file::szt_size = sizeof(size_t);

// get jet energy profile
// ********************************************************
bool profile(
  const jet& jet, double* profile_,
  double r_min, double r_step, unsigned short r_num,
  double tolerance=0.1, bool warn=true)
{
  size_t nconst = jet.size();

  if (!nconst) // if no constituents
    cerr << "Jet has no constituents" << endl;

  // Does jet have no constituents outside r_max
  bool within_cone = true;

  // variables
  size_t ci = 0; // current constituent
  double ri = 0; // current radius index
  double rv = r_min; // current radius value

  for (; ci<nconst; ++ci) {
    if (jet.r[ci]>rv) {
      ++ri;
      if (ri==r_num) { // handle overflow

        --ri;
        for (; ci<nconst; ++ci) {
          
        }

        break;
      }
      
      rv += r_step;
      profile_[ri] = 0.;
    }
    profile_[ri] += jet.Et[ci];
  }

  // loop over constituents
  for ( vector<PseudoJet>::const_iterator it = constituents.begin();
        it != constituents.end(); ++it )
  {
    cr = it->delta_R(jet); // get constituent radius
    Et = it->Et(); // get constituent transverse energy
    pr = r_max; // reset profile radius

    // if constituent is within the radius, add it's energy
    unsigned short i;
    for (i = r_num-1;; --i) {
      if (cr<=pr) E_[i] += Et;
      else {
        if (r_num-i==1) { // constituent is outside of jet
          if ( (cr-r_max)/r_max > tolerance ) { // constituent is outside of tolerance
            within_cone = false;
            if (warn)
              cerr << "<jep::"<<__func__<<':'<<__LINE__
                   << ">: Jet has a constituent outside r_max="
                   << r_max << ", with r=" << cr << endl;
          } else E_[i] += Et; // within tolerance
        }
        break;
      }

      if (i==0) break;
      pr -= r_step;
    }

  }

  return E_;
}

// MAIN *************************************************************
int main(int argc, char *argv[]){

  if ( argc!=4 ) {
    cout << "Usage: "<<argv[0]<<" jets_file.jets profile.jep stat_plots.pdf" << endl;
    return 0;
  }

  jep::reader* f = new jep::reader(argv[2]);
  const jep::header& h = f->get_header();

  jets_file jets(argv[1]);
  jet j;

  clock_t last_time = clock();
  short num_sec = 0; // seconds elapsed

  cout << "Cone R: " << jets.coneR() << endl;
  cout << "Jets: " << jets.njets() << endl;

  while (jets.next(j)) {

    

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
  cout << setw(10) << jets.njets()
       << setw( 7) << num_sec << 's' << endl << endl;

  return 0;
}
