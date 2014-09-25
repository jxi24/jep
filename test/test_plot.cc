#include <iostream>
#include <cstdlib>
#include <string>

#include <TH1.h>
#include <TCanvas.h>
#include <TAxis.h>

#include "jep/reader.h"

using namespace std;

#define test(var) \
  cout <<"\033[36m"<< #var <<"\033[0m"<< " = " << var << endl;

int main(int argc, char** argv)
{
  if (argc!=5) {
    cout << "Usage:" << endl;
    cout << "(1) " << argv[0] << " quark_file gluon_file higgs_file E(double)" << endl;
    return 1;
  }

  // Energy value
  jep::val_t E = atof(argv[4]);

  // Canvas
  // ...

  for (unsigned char fi=0; fi<3; ++fi) { // loop over particles

    // open jep data file
    const char* file = argv[fi+1];
    jep::reader* f = new jep::reader(file);
    const jep::header& h = f->get_header();
    cout << "Reading " << file << ": " << jep::pid_name(h.pid) << endl;

    // Book histogram
    // ..

    // Read data
    // ...
    /*
        vector<jep::val_t> psi = f->psi(E); // particle_t, energy
        cout << "r\tpsi" << endl;
        for (jep::num_t i=0; i<h.r_num; ++i) {
          cout << h.r(i) << '\t' << psi[i] << endl;
        }
    */

    // close jep file
    delete f;
  }

  // Save canvas
  // ...

  return 0;
}
