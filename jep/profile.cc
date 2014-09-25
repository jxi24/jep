#include "jep/profile.h"

#include <vector>

#include <fastjet/PseudoJet.hh>

#include "jep/exception.h"

#define test(var) \
  std::cout <<"\033[36m"<< #var <<"\033[0m"<< " = " << var << std::endl;

#define ERROR jepex(__func__,__LINE__)

using namespace std;
using fastjet::PseudoJet;

namespace jep {

vector<double>
profile(const PseudoJet* jet,
        double r_max, double r_step, unsigned short r_num)
{
  if (!jet->has_constituents()) // if no constituents
    throw ERROR << "Jet has no constituents";

  // get jet constituents ( returns object, not reference )
  const vector<PseudoJet> constituents = jet->constituents();

  // allocate container for jet energy profile
  vector<double> E_(r_num,0);

  // variables
  //const double r_max = r_min + (r_num-1)*r_step;
  double cr; // current constituent radius
  double pr; // current profile radius
  double Et; // current constituent transverse energy

  // loop over constituents
  for ( vector<PseudoJet>::const_iterator it = constituents.begin();
        it != constituents.end(); ++it )
  {
    cr = jet->delta_R(*it); // get constituent radius
    Et = it->Et(); // get constituent transverse energy
    pr = r_max; // reset profile radius

    // if constituent is within the radius, add it's energy
    unsigned short i;
    for (i = r_num-1;; --i) {
      if (cr<=pr) E_[i] += Et;
      else break;

      if (i==0) break;
      pr -= r_step;
    }

    if (i==r_num-1)
      throw ERROR << "Jet has a constituents outside r_max";
  }

  return E_;
}

} // end jep namespace
