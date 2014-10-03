#ifndef jep_algorithms_h
#define jep_algorithms_h

#include <vector>

#include <fastjet/PseudoJet.hh>

namespace jep {

// function to get jet energy profile
// ********************************************************
std::vector<double>
profile(const fastjet::PseudoJet& jet,
        double r_max, double r_step, unsigned short r_num);

// class derived from fastjet::PseudoJet::UserInfoBase
// for tracking particle evolution within the event shower
// ********************************************************
class shower_info: public fastjet::PseudoJet::UserInfoBase {
private:
  const int _id, _pid, _status, _m1, _m2;

  shower_info(int id, int pid, int status, int m1, int m2);

  static shower_info** infos;
  static unsigned size, counter;

public:
  virtual ~shower_info();

  int id() const; // number in the particles list of event
  int pid() const; // pdg code
  int status() const;

  const shower_info* m1() const;
  const shower_info* m2() const;

  void trace_back(std::vector<const shower_info*>& v) const;

  static void init(unsigned N);
  static void clear();

  static shower_info* add(int id, int pid, int status, int m1, int m2);
};

// Jet validator
// Check if a jet originated from a process
// ********************************************************
class jet_validator {
  // array of pointers to info of leading constituents
  const shower_info** lead;
  // number of leading constituents
  unsigned short nlead;

public:
  jet_validator(const fastjet::PseudoJet& jet, unsigned short nlead);
  virtual ~jet_validator();

  bool is_from_higgs_bb() const;
};

} // end jep namespace
#endif
