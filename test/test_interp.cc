#include <iostream>
#include <cmath>
#include <cstdlib>
#include <string>

#include "jep/reader.h"

using namespace std;

#define test(var) \
  cout <<"\033[36m"<< #var <<"\033[0m"<< " = " << var << endl;

void usage(const char* prog) {
  cout << "Usage:" << endl;
  cout << "(1) " << prog << " E(double)" << endl;
  cout << "(2) " << prog << " filename E(double)" << endl;
}

int main(int argc, char** argv)
{
  jep::val_t E;
  string file("test.jep");

  if (argc==1) {
    usage(argv[0]);
    return 0;
  } else if (argc==2) {
    E = atof(argv[1]);
  } else if (argc==3) {
    file = argv[1];
    E = atof(argv[2]);
  } else {
    usage(argv[0]);
    return 1;
  }

  jep::reader* f = new jep::reader(file.c_str());
  const jep::header& h = f->get_header();

  test(h.r_num)
  test(h.r_min)
  test(h.r_step)
  test(h.r_max())
  test(h.E_num)
  test(h.E_min)
  test(h.E_step)
  test(h.E_max())
  test(jep::pid_name(h.pid))
  cout << endl;

  test(E)
  cout << endl;

  vector<jep::val_t> psi = f->psi(E); // particle_t, energy

  cout << "r\tpsi" << endl;
  for (jep::num_t i=0; i<h.r_num; ++i) {
    cout << h.r(i) << '\t' << psi[i] << endl;
  }

  delete f;

  return 0;
}
