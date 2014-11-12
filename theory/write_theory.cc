#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>
#include <sstream>
#include <vector>

#include <boost/program_options.hpp>

#include "LHAPDF/LHAPDF.h"

#include "jep/writer.h"

using namespace std;
namespace po = boost::program_options;

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
  // START OPTIONS **************************************************
  jep::header head;
  string dir, pdf_name, conf_file, out_suffix;

  try {
    // General Options ------------------------------------
    po::options_description all_opt("Options");
    all_opt.add_options()
      ("help,h", "produce help message")
      ("dir,o", po::value<string>(&dir)->default_value(""),
       "output files directory")

      ("rnum",  po::value<jep::num_t>(&head.r_num),  "number of r points")
      ("rmin",  po::value<jep::val_t>(&head.r_min),  "minimum cone radius")
      ("rstep", po::value<jep::val_t>(&head.r_step), "cone radius increment")

      ("Enum",  po::value<jep::num_t>(&head.E_num),  "number of E points")
      ("Emin",  po::value<jep::val_t>(&head.E_min),  "minimum jet energy")
      ("Estep", po::value<jep::val_t>(&head.E_step), "jet energy increment")

      ("pdf", po::value<string>(&pdf_name)->default_value("CT10nnlo"),
       "LHAPDF::PDF name")

      ("out-suffix", po::value<string>(&out_suffix),
       "output file suffix")

      ("conf,c", po::value<string>(&conf_file), "read configuration file")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, all_opt), vm);
    po::notify(vm);
    if (vm.count("conf"))
      po::store(po::parse_config_file<char>(conf_file.c_str(), all_opt), vm);
    po::notify(vm);

    // Options Properties ---------------------------------
    if (argc==1 || vm.count("help")) {
      cout << all_opt << endl;
      return 0;
    }

    // trim dir string
    while (*(dir.end()-2) == '/') dir.resize(dir.size()-1);
    if (*(--dir.end()) != '/') dir += '/';

    // prepend underscore to file suffix
    if (out_suffix.size()) out_suffix = '_'+out_suffix;

    // Necessary options ----------------------------------
    vector<string> rec_opt;
    rec_opt.push_back("rnum");
    rec_opt.push_back("rmin");
    rec_opt.push_back("rstep");
    rec_opt.push_back("Enum");
    rec_opt.push_back("Emin");
    rec_opt.push_back("Estep");

    for (size_t i=0, size=rec_opt.size(); i<size; ++i) {
      if (!vm.count(rec_opt[i]))
      { cerr << "Missing command --" << rec_opt[i] << endl; return 1; }
    }

  } catch(exception& e) {
    cerr << "Arguments error: " <<  e.what() << endl;
    return 1;
  }
  // END OPTIONS ****************************************************

  LHAPDF::PDF* pdf = LHAPDF::mkPDF(pdf_name.c_str(),0);

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

  for (jep::num_t pi=0; pi<nump; ++pi) { // particle loop
    const jep::particle_t p = particles[pi];
    head.pid = p;

    string name(jep::pid_name(p));
    cout << "Particle: " << name << ' ' << setw(2) << p << ' ';

    stringstream ss;
    ss << dir << name << out_suffix << ".jep";
    const char* fname = ss.str().c_str();

    cout << fname << endl;

    jep::writer* w = new jep::writer(fname,head);
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
