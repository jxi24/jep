#include <iostream>
#include <cmath>
#include <cstdlib>

#include "jep/writer.h"
#include "jep/reader.h"

using namespace std;

#define test(var) \
  cout <<"\033[36m"<< #var <<"\033[0m"<< " = " << var << endl;

double psi_higgs(double E, double r) {
  static double a = sqrt(2.9);
  static double mh = 125;
  static double zmin;

  zmin = pow(mh/E,2)/(pow(mh/E,2)+pow(a*r,2));
  return 1.0/6.0-pow(zmin,2)/2.0+pow(zmin,3)/3.0;
}

int main()
{
  jep::header head;
  head.r_num  = 10;
  head.r_min  = 0.1;
  head.r_step = 0.1;
  head.E_num  = 11;
  head.E_min  = 500;
  head.E_step = 10;
  head.pid    = 25;

  test(sizeof(jep::particle_t))
  test(sizeof(jep::num_t))
  test(sizeof(jep::val_t))
  test(sizeof(jep::header))
  cout << endl;

  jep::writer* w = new jep::writer("test.jep",head);
  jep::writer::Er_iter it = w->begin();

  jep::val_t psi[head.r_num];

  // loop over particles would be on the outside

  do { // E loop
    do { // r loop

      //cout << "E=" << it.E() << " r=" << it.r() << endl;

      psi[it.ri()] = psi_higgs(it.E(),it.r());

    } while ( it.next_r() );

    for(jep::num_t i = 0; i < head.r_num; ++i) {
      psi[i] /= psi[head.r_num-1];
    }

    w->write(psi);

  } while ( it.next_E() );

  delete w;

  jep::reader* r = new jep::reader("test.jep");
  r->to_ascii("test.txt");

  delete r;

  return 0;
}
