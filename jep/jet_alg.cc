#include "jep/jet_alg.h"

#include <iostream>

#include "jep/exception.h"

#define test(var) \
  std::cout <<"\033[36m"<< #var <<"\033[0m"<< " = " << var << std::endl;

#define ERROR jepex(__func__,__LINE__)

using namespace std;
using fastjet::PseudoJet;

namespace jep {

// get jet energy profile
// ********************************************************
vector<double>
profile(const PseudoJet& jet,
        double r_max, double r_step, unsigned short r_num)
{
  if (!jet.has_constituents()) // if no constituents
    throw ERROR << "Jet has no constituents";

  // get jet constituents ( returns object, not reference )
  const vector<PseudoJet> constituents = jet.constituents();

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
    cr = jet.delta_R(*it); // get constituent radius
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

// get jet particle of origin
// ********************************************************
typedef vector<const shower_info*>::iterator shower_iter;

const shower_info* origin(const PseudoJet& jet, unsigned short num_parts)
{
  if (!jet.has_constituents()) // if no constituents
    throw ERROR << "Jet has no constituents";

  // get jet constituents ( returns object, not reference )
  // sorted by pt
  const vector<PseudoJet> constituents
    = fastjet::sorted_by_pt(jet.constituents());

  // intermediate particles
  vector<const shower_info*> current;

  // number of particles to track back
  const unsigned short N = (
    num_parts<constituents.size() ? num_parts : constituents.size()
  );
  for (unsigned short i=0; i<N; ++i) {
    if (!constituents[i].has_user_info()) // if no user info
      throw ERROR << "Jet constituent "<<i<<" has no user info";

    current.push_back( static_cast<const shower_info*>(
      constituents[i].user_info_ptr()
    ) );

    test(constituents[i].Et())
  }


  while (current.size()>1) {
    for (size_t k=0; k<current.size(); ++k) cout << current[k]->id() << ' ';
    cout << endl;

    vector<const shower_info*> next;

    for (shower_iter it=current.begin(), endi=current.end(); it!=endi; ++it)
    {
      const shower_info* m[2] = { (*it)->m1(), (*it)->m2() };

      for (unsigned char i=0; i<2; ++i) {
        if (!m[i]) continue;

        bool seen = false;
        for (shower_iter jt=next.begin(), endj=next.end(); jt!=endj; ++jt)
        {
          seen = ( m[i]->id() == (*jt)->id() );
          if (seen) break;
        }
        if (!seen) next.push_back(m[i]);
      }
    }

    if (next.size()==0) {
      cerr << "Warning in jep::origin: "
              "jet origin tracked to initial state particle" << endl;
      return current.front();
    }

    current = next;
  }

  cout << current[0]->id() << endl;
  return current.front();
}

// class derived from fastjet::PseudoJet::UserInfoBase
// for tracking particle evolution within the event shower
// ********************************************************
shower_info::shower_info(
  short pid, short status,
  short m1, short m2, short d1, short d2
)
: id_(counter), pid_(pid), status_(status),
  m1_(m1), m2_(m2), d1_(d1), d2_(d2)
{
  infos[counter] = this;
  ++counter;
}

shower_info::~shower_info() { }

shower_info** shower_info::infos = NULL;
unsigned short shower_info::size=0;
unsigned short shower_info::counter=0;

void shower_info::init(unsigned short N) {
  size = N;
  infos = new shower_info*[size];
}
void shower_info::clear() {
  //for (unsigned short i=0;i<size;++i) delete infos[i];
  // FastJet deletes these appratenly
  // need to confirm
  delete[] infos;
  infos = NULL;
  counter=0;
}

shower_info* shower_info::add(
  short pid, short status,
  short m1, short m2, short d1, short d2
) {
  if (counter==size) ERROR << "More shower_info's then expected";
  return new shower_info(pid,status,m1,m2,d1,d2);
}

unsigned short shower_info::id() const { return id_; }
short shower_info::pid() const { return pid_; }
short shower_info::status() const { return status_; }

const shower_info* shower_info::m1() const {
  if (m1_ == -1) return NULL;
  return infos[m1_];
}
const shower_info* shower_info::m2() const {
  if (m2_ == -1) return NULL;
  return infos[m2_];
}
const shower_info* shower_info::d1() const {
  if (d1_ == -1) return NULL;
  return infos[d1_];
}
const shower_info* shower_info::d2() const {
  if (d2_ == -1) return NULL;
  return infos[d2_];
}

} // end jep namespace
