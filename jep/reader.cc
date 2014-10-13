#include "jep/reader.h"

#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <iomanip>

#include "jep/exception.h"


#include <iostream>

#define test(var) \
  std::cout <<"\033[36m"<< #var <<"\033[0m"<< " = " << var << std::endl;


#define ERROR jepex(__func__,__LINE__,"reader")

namespace jep {

const size_t reader::header_size = sizeof(header);

// reader ***********************************************************
reader::reader(const char* filename)
: filename(filename),
  dat( new std::ifstream(filename, std::ifstream::binary) ),
  head(read_header()), psi_(init_psi()),
  cE(head.E_num) // initialize this to impossible value
{
  const size_t val_t_size = sizeof(val_t);

  for (num_t e=0;e<head.E_num;++e)
    for (num_t r=0;r<head.r_num;++r)
      dat->read((char*)&psi_[e][r], val_t_size);
}

// head file header
// also validates the file
header reader::read_header() {
  // get file beginning, end, size
  const std::streampos file_beg = dat->tellg();
  dat->seekg(0, std::ios::end); // rewind to end of file
  const size_t file_size = dat->tellg() - file_beg;
  dat->seekg(0, std::ios::beg); // rewind to beginning of file

  if (file_size <= header_size)
    throw ERROR << "File \""<<filename<<"\" contains no data";

  header head;
  dat->read((char*)&head, header_size);

  if (
    (file_size-header_size)/sizeof(val_t) // number of rows
    != (size_t)head.E_num*(size_t)head.r_num

  ) throw ERROR << "File \""<<filename<<"\" contains unexpected number of rows";

  return head;
}

// allocate the data array
val_t** reader::init_psi() {
  val_t** psi_ = new val_t*[head.E_num];
  for (unsigned e=0;e<head.E_num;++e)
    psi_[e] = new val_t[head.r_num];
  return psi_;
}

reader::~reader() {
  // close and delete file stream
  dat->close();
  delete dat;

  // delete data array
  for (unsigned e=0;e<head.E_num;++e) {
    delete[] psi_[e];
  }
  delete[] psi_;
}

const header& reader::get_header() const {
  return head;
}

// ASCII converter **************************************************
void reader::to_ascii(const char* filename) const {

  std::ofstream txt(filename);
  if (txt.is_open())
  {
    txt << "# Header" << std::endl;
    txt << "pid    = " << pid_name(head.pid) << std::endl;
    txt << "E_num  = " << head.E_num  << std::endl;
    txt << "E_min  = " << head.E_min  << std::endl;
    txt << "E_step = " << head.E_step << std::endl;
    txt << "r_num  = " << head.r_num  << std::endl;
    txt << "r_min  = " << head.r_min  << std::endl;
    txt << "r_step = " << head.r_step << std::endl;
    txt << std::endl;

    txt << "# Data" << std::endl;
    txt << std::setw(6) << std::left << "Energy";
    for (num_t i=0;i<head.r_num;++i)
      txt << "  " << "r = " << std::setw(4) << std::left << head.r_min+i*head.r_step;
    txt << std::endl;

    for (num_t e=0;e<head.E_num;++e) {
      txt << std::setw(6) << std::left << head.E(e);

      for (num_t r=0;r<head.r_num;++r) {
        txt << "  "
            << std::setw(8) << std::left
            << std::fixed << std::setprecision(6)
            << psi_[e][r];
        txt.unsetf(std::ios::fixed);
      }

      txt << std::endl;
    }

    // close ascii output
    txt.close();
  } else throw ERROR << "Unable to open file " << filename;
}

// get interpolated psi values **************************************
#define interp_n 4 // number of inteprolation points
std::vector<val_t> reader::psi(val_t E) const {

  if ( E<head.E_min )
    throw ERROR << "Jet is too soft for profiling. No data for E="<<E;

  if ( E>head.E_max() )
    throw ERROR << "Jet cannot be profiled. No data for E="<<E<<" > "<<head.E_max();

  num_t iE = (E-head.E_min)/head.E_step;

  if (iE != cE) { // if can't recycle
    cE = iE;

    // number of inteprolation points
    //interp_n = ( ( iE==0 || iE>=head.E_num-2 ) ? 3 : 4 );

    if (0<iE && iE<head.E_num-3) --iE;
    else if (iE>=head.E_num-3) iE = head.E_num-4; // handle right boundary

    for (unsigned char i=0;i<interp_n;++i) { // sample points
      current_E[i] = head.E(iE + i);
      current_psi[i] = psi_[iE + i];
    }
  }

  // vector of interpolated values
  std::vector<val_t> psis(head.r_num,0);

  for (num_t r=0;r<head.r_num;++r) { // radia
    val_t psi_n[interp_n];

    for (unsigned char i=0;i<interp_n;++i) // sample points
      psi_n[i] = current_psi[i][r];

    psis[r] = polint(interp_n, E, current_E, psi_n);
  }

  return psis;
}

/* Polint: Adapted from Numerical Recipes:
 * Preforms a polynomial interpolation of order n-1
 * x_ and y_ must arrays the size of n
 */

val_t reader::polint(unsigned char n, val_t x, const val_t* x_, const val_t* y_) const {
  unsigned char i, m, ns = 1;
  val_t den, dift, ho, hp, w;
  val_t dif = fabs(x - x_[0]);

  val_t c[n];
  val_t d[n];

  for (i = 0; i < n; ++i) {
      if ((dift = fabs(x - x_[i])) < dif) {
          ns = i;
          dif = dift;
      }
      c[i] = y_[i];
      d[i] = y_[i];
  }
  val_t y = y_[ns--];

  for (m = 1; m < (n - 1); ++m) {
      for (i = 0; i < n - m; ++i) {
          ho = x_[i] - x;
          hp = x_[i + m] - x;
          w = c[i + 1] - d[i];
          if(std::abs(den = ho - hp) < 1.E-16)
            throw ERROR << "den too small";

          den = w/den;
          d[i] = hp*den;
          c[i] = ho*den;
      }

      y += ( 2.*ns < (n - m) ? c[ns + 1] : d[ns--] );
  }

  return y;
}

} // end jep namespace
