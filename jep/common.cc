#include "jep/common.h"

//#include "jep/exception.h"

namespace jep {

val_t header::E(num_t i) const {
  return E_min + i*E_step;
}
val_t header::r(num_t i) const {
  return r_min + i*r_step;
}

val_t header::E_max() const {
  static val_t E_max = E(E_num-1);
  return E_max;
}
val_t header::r_max() const {
  static val_t r_max = r(r_num-1);
  return r_max;
}

/*
#define ERROR jepex(__func__,__LINE__,"particle_t")

unsigned char particle_t::init_pdg_id(unsigned char id) {
  if (id==1 || id==21 || id==25) {
    return id;
  } else {
    throw ERROR << "No jet energy profile for particle pid " << id;
  }
}

particle_t::particle_t(): pdg_id(0) { }
particle_t::particle_t(const particle_t& p): pdg_id(init_pdg_id(p.pdg_id)) { }
particle_t::particle_t(unsigned char pid): pdg_id(init_pdg_id(pid)) { }
particle_t::~particle_t() { }
particle_t& particle_t::operator=(particle_t p) { pdg_id = p.pdg_id; return *this; }
particle_t& particle_t::operator=(unsigned char pid) { pdg_id = pid; return *this; }

bool particle_t::operator==(particle_t p) const { return (pdg_id == p.pdg_id); }
bool particle_t::operator!=(particle_t p) const { return (pdg_id != p.pdg_id); }
bool particle_t::operator==(unsigned char pid) const { return (pdg_id == pid); }
bool particle_t::operator!=(unsigned char pid) const { return (pdg_id != pid); }

unsigned char particle_t::pid() const { return pdg_id; }
particle_t::operator unsigned char() const { return pdg_id; }
particle_t::operator int() const { return (int)pdg_id; }

const char* particle_t::name() const {
  switch (pdg_id) {
    case  1: return "quark"; break;
    case 21: return "gluon"; break;
    case 25: return "higgs"; break;
    default: return "";
  }
}
*/

const char* pid_name(particle_t p) {
  switch (p) {
    case  1: return "quark"; break;
    case 21: return "gluon"; break;
    case 25: return "higgs"; break;
    default: return "";
  }
}

}
