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
  const unsigned short id_;
  const short pid_, status_, m1_, m2_, d1_, d2_;

  shower_info(short pid, short status,
              short m1, short m2, short d1, short d2);

  static shower_info** infos;
  static unsigned short size, counter;

public:
  virtual ~shower_info();

  unsigned short id() const; // number in the particles list of event
  short pid() const; // pdg code
  short status() const;

  const shower_info* m1() const;
  const shower_info* m2() const;
  const shower_info* d1() const;
  const shower_info* d2() const;

  static void init(unsigned short N);
  static void clear();

  static shower_info* add(short pid, short status,
    short m1, short m2, short d1, short d2);

  static void draw(const char* fname);

};

// function to get jet particle of origin
// ********************************************************
const shower_info*
origin(const fastjet::PseudoJet& jet,
       unsigned short num_parts=5);

}

#endif
