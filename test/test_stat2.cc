#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <utility>
#include <map>
#include <ctime>

#include <TLorentzVector.h>

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

int main(int argc, char *argv[]){

  if ( argc!=2 ) {
    cout << "Usage: "<<argv[0]<<" file.jets" << endl;
    return 0;
  }

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
