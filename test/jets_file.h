#ifndef stdhep_root_h
#define stdhep_root_h

#include <vector>
#include <iosfwd>

class jets_file;

class jet {

  struct constit {
    double r, Et;
    short pid;
  };

  double _Et;
  std::vector<constit> c; // constituents

public:
  double  r(size_t i) const;
  double Et(size_t i) const;
  short pid(size_t i) const;
  size_t size() const;

  double Et() const;

friend class jets_file;
};

class jets_file {
  std::ifstream *dat;
  size_t _cur_jet;
  float  _cone_r;
  size_t _num_jets;

  static const std::streamsize
    flt_size, dbl_size, srt_size, szt_size;

public:
  jets_file(const char* filename);
  ~jets_file();

  size_t njets() const;
  size_t   cur() const;
  float  coneR() const;

  bool next(jet& j);
};

#endif
