#include "jep/jet_alg.h"

#include <iostream>
#include <algorithm>

#include "jep/exception.h"

#define test(var) \
  std::cout <<"\033[36m"<< #var <<"\033[0m"<< " = " << var << std::endl;

#define ERROR jepex(__func__,__LINE__)

using namespace std;
using fastjet::PseudoJet;

namespace jep {

// ********************************************************
// get jet energy profile
// ********************************************************
vector<double>
profile(const PseudoJet& jet,
        double r_max, double r_step, unsigned short r_num,
        bool& within_cone, double tolerance, bool warn)
{
  if (!jet.has_constituents()) // if no constituents
    throw ERROR << "Jet has no constituents";

  // Does jet have no constituents outside r_max
  within_cone = true;

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

// ********************************************************
// Jet validator
// Check if a jet originated from a process
// ********************************************************

#define ERROR2 jepex(__func__,__LINE__,"jet_validator")

jet_validator::jet_validator(const PseudoJet& jet, unsigned short nlead)
: lead(new const shower_info*[nlead]), nlead(nlead)
{
  if (!jet.has_constituents()) // if no constituents
    throw ERROR2 << "Jet has no constituents";

  // get jet constituents ( returns object, not reference )
  // sorted by pt
  const vector<PseudoJet> constituents
    = fastjet::sorted_by_pt(jet.constituents());

  const size_t ncons = constituents.size();
  if (ncons<nlead) this->nlead = ncons;

  for (unsigned short i=0; i<this->nlead; ++i) {
    if (!constituents[i].has_user_info()) // if no user info
      throw ERROR2 << "Jet constituent "<<i<<" has no user info";

    lead[i] = static_cast<const shower_info*>(
      constituents[i].user_info_ptr()
    );

    //cout << lead[i]->id() << "  " << constituents[i].Et() << endl;
  }
}

jet_validator::~jet_validator() {
  delete[] lead;
}

// check if jet is from higgs->bb-bar process
bool jet_validator::is_from_higgs_bb() const
{
  vector<const shower_info*> chain[nlead];

  for (unsigned short i=0; i<nlead; ++i) {
    lead[i]->trace_back(chain[i]);
    const size_t size = chain[i].size();

    size_t k = size;
    const shower_info* p;

    // find initial state Higgs
    while (k>0) {
      p = chain[i][--k]; // current particle

      if (p->pid()==25) // is Higgs
        if (p->status()==3) break; // is initial state
    }

    // if no Higgs in initial state
    if (k==0) return false;

    // is next particle b or b-bar?
    p = chain[i][--k];
    if ( abs(p->pid())!=5 || p->status()!=3 ) return false;
  }

  return true;
}

// check if jet is from gluon scattering
bool jet_validator::is_from_gluons() const
{
  for (unsigned short i=0; i<nlead; ++i)
    if (!is_from_gluons(lead[i])) return false;

  return true;
}

// ??? Does it matter which gluons originated the jet?

bool jet_validator::is_from_gluons(const shower_info* p) const
{
  short nm = 0, // number of mothers
        ng = 0; // number of gluon mothers

  const shower_info* m[2] = { p->m1(), p->m2() };
  for (short i=0;i<2;++i) {
    if (m[i]) {
      ++nm;
      if (m[i]->pid()==21) { if (m[i]->status()==3) ++ng; }
      else if (is_from_gluons(m[i])) ++ng;
    }
  }

  if (nm==0) return false;
  if (ng<nm) return false;
  return true;
}

// check if jet is from quark scattering
bool jet_validator::is_from_quarks() const
{
  for (unsigned short i=0; i<nlead; ++i)
    if (!is_from_quarks(lead[i])) return false;

  return true;
}

bool jet_validator::is_from_quarks(const shower_info* p) const
{
  short nm = 0, // number of mothers
        nq = 0; // number of quark mothers

  const shower_info* m[2] = { p->m1(), p->m2() };
  for (short i=0;i<2;++i) {
    if (m[i]) {
      ++nm;
      if (m[i]->status()==3) {
        int pid_abs = abs(m[i]->pid());
        if ( // quarks pid's
          pid_abs==1 ||
          pid_abs==2 ||
          pid_abs==3 ||
          pid_abs==4 ||
          pid_abs==5 ||
          pid_abs==6
        ) ++nq;
      }
      else if (is_from_quarks(m[i])) ++nq;
    }
  }

  if (nm==0) return false;
  if (nq<nm) return false;
  return true;
}

// ********************************************************
// class derived from fastjet::PseudoJet::UserInfoBase
// for tracking particle evolution within the event shower
// ********************************************************
shower_info::shower_info(int id, int pid, int status, int m1, int m2)
: _id(id), _pid(pid), _status(status), _m1(m1), _m2(m2)
{
  infos[counter] = this;
  ++counter;
}

shower_info::~shower_info() { }

shower_info** shower_info::infos = NULL;
unsigned shower_info::size=0;
unsigned shower_info::counter=0;

void shower_info::init(unsigned N) {
  size = N;
  infos = new shower_info*[size];
}
void shower_info::clear() {
  //for (unsigned short i=0;i<size;++i) delete infos[i];
  // FastJet deletes these apparently
  // need to confirm
  delete[] infos;
  infos = NULL;
  counter=0;
}

shower_info* shower_info::add(int id, int pid, int status, int m1, int m2) {
  if (counter==size) ERROR << "More shower_info's then expected";
  return new shower_info(id,pid,status,m1,m2);
}

int shower_info::id() const { return _id; }
int shower_info::pid() const { return _pid; }
int shower_info::status() const { return _status; }

const shower_info* shower_info::m1() const {
  if (_m1 == -1) return NULL;
  return infos[_m1];
}
const shower_info* shower_info::m2() const {
  if (_m2 == -1) return NULL;
  return infos[_m2];
}

void shower_info::trace_back(vector<const shower_info*>& v) const {
  if (_m1 != -1) {
    const shower_info* m1 = infos[_m1];
    v.push_back(m1);
    m1->trace_back(v);
  }
  if (_m2 != -1) {
    const shower_info* m2 = infos[_m2];
    v.push_back(m2);
    m2->trace_back(v);
  }
}

} // end jep namespace
