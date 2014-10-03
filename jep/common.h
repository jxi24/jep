#ifndef jep_common_h
#define jep_common_h

namespace jep {

typedef double val_t;
typedef unsigned short num_t; // max 65,535
typedef short particle_t;

/*
class particle_t {
  unsigned char pdg_id;
  unsigned char init_pdg_id(unsigned char id);
  public:
  particle_t();
  particle_t(const particle_t& p);
  particle_t(unsigned char pid);
  ~particle_t();
  particle_t& operator=(particle_t p);
  particle_t& operator=(unsigned char pid);
  bool operator==(particle_t p) const;
  bool operator!=(particle_t p) const;
  bool operator==(unsigned char pid) const;
  bool operator!=(unsigned char pid) const;
  unsigned char pid() const;
  operator unsigned char() const;
  operator int() const;
  const char* name() const;
};
*/

static const particle_t quark =  1;
static const particle_t gluon = 21;
static const particle_t higgs = 25;

const char* pid_name(particle_t p);

struct header {
  particle_t pid; // PDG particle id

  num_t E_num;
  val_t E_min;
  val_t E_step;

  num_t r_num;
  val_t r_min;
  val_t r_step;

  val_t E(num_t i) const;
  val_t r(num_t i) const;

  val_t E_max() const;
  val_t r_max() const;

  bool operator==(const header& rhs) const;
  bool operator!=(const header& rhs) const;
};

}

#endif
