#include <iostream>
#include <cmath>
#include <cstdlib>
#include <string>
#include <iomanip>

#include "LHAPDF/LHAPDF.h"

#include "jep/writer.h"

using namespace std;

#define test(var) \
  cout <<"\033[36m"<< #var <<"\033[0m"<< " = " << var << endl;

// Functions to calculate particle jet profiles *********************

double psi_higgs(double E, double r) {
  static double a = sqrt(2.9);
  static double mh = 125;
  static double zmin;

  zmin = pow(mh/E,2)/(pow(mh/E,2)+pow(a*r,2));
  return 1.0/6.0-pow(zmin,2)/2.0+pow(zmin,3)/3.0;
}

extern "C" {
  void __terms_MOD_calc_terms(
    int &iparton, double &r, double &R, double &E, double &alphas, double &resum
  );
}

inline double calc_terms(int iparton, double r, double R, double E, double alphas) {
  double resum;
  __terms_MOD_calc_terms(iparton, r, R, E, alphas, resum);
  return resum;
}

// MAIN *************************************************************

int main(int argc, char** argv)
{
  string dir(argc>1 ? argv[1] : "");
  while (*(dir.end()-2) == '/') dir.resize(dir.size()-1);
  if (*(--dir.end()) != '/') dir += '/';

  LHAPDF::PDF* pdf = LHAPDF::mkPDF("CT10nnlo",0);

  jep::header head;
  head.r_num  = 91;
  head.r_min  = 0.1;
  head.r_step = 0.01;
  head.E_num  = 100;
  head.E_min  = 10;
  head.E_step = 10;

  test(head.r_num)
  test(head.r_min)
  test(head.r_step)
  test(head.E_num)
  test(head.E_min)
  test(head.E_step)
  cout << endl;

  test(head.E_max())
  test(head.r_max())
  cout << endl;

  // maximum radius
  const jep::val_t R = head.r_max();

  const jep::num_t nump = 3;
  jep::particle_t particles[nump] = {jep::quark, jep::gluon, jep::higgs};

  jep::val_t psi[head.r_num]; // book array of psi values

  double alphas;

  for (jep::num_t pi; pi<nump; ++pi) { // particle loop
    const jep::particle_t p = particles[pi];
    head.pid = p;

    string name(jep::pid_name(p));
    cout << "Particle: " << name << ' ' << setw(2) << p << ' ';

    if (argc>1) name = dir+name;
    name += ".jep";
    cout << name << endl;

    jep::writer* w = new jep::writer(name.c_str(),head);
    jep::writer::Er_iter it = w->begin();

    do { // E loop

      const jep::val_t E = it.E();
      if (p!=jep::higgs) alphas = pdf->alphasQ2(E*E);

      do { // r loop

        //cout << "E=" << E << " r=" << it.r() << endl;

        if (p==jep::higgs) psi[it.ri()] = psi_higgs(E,it.r());
        else psi[it.ri()] = calc_terms(p, it.r(), R, E, alphas);

      } while ( it.next_r() );

      // normalize
      for(jep::num_t i = 0; i < head.r_num; ++i) psi[i] /= psi[head.r_num-1];

      w->write(psi); // write jep data file

    } while ( it.next_E() );

    delete w; // close jep data file

  }

  delete pdf;

  return 0;
}
