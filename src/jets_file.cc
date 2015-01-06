#include "jets_file.h"

#include <fstream>

// jet ****************************************************

double jet::  r(size_t i) const { return c[i].r;   }
double jet:: Et(size_t i) const { return c[i].Et;  }
short  jet::pid(size_t i) const { return c[i].pid; }
size_t jet::size() const { return c.size(); }

double jet::Et() const { return _Et; }

// jets_file **********************************************

jets_file::jets_file(const char* filename)
: dat( new std::ifstream(filename, std::ifstream::binary) ),
  _cur_jet(0)
{
  dat->read((char*)&_cone_r,   flt_size);
  dat->read((char*)&_num_jets, szt_size);
}
jets_file::~jets_file() { delete dat; }

size_t jets_file::njets() const { return _num_jets; }
size_t jets_file::  cur() const { return _cur_jet;  }
float  jets_file::coneR() const { return _cone_r;   }

bool jets_file::next(jet& j) {
  ++_cur_jet;

  if (_cur_jet>_num_jets) return false;

  // read jet Et
  double jet_Et;
  dat->read((char*)&jet_Et, dbl_size);
  j._Et = jet_Et;

  j.c.clear(); // clear jet constituents

  size_t nc; // number of constituents
  jet::constit c; // current constituent

  dat->read((char*)&nc, szt_size);
  j.c.reserve(nc);

  for (size_t i=0; i<nc; ++i) {
    dat->read((char*)&c.r,   dbl_size);
    dat->read((char*)&c.Et,  dbl_size);
    dat->read((char*)&c.pid, srt_size);
    j.c.push_back(c);
  }

  return true;
}

const std::streamsize jets_file::flt_size = sizeof(float);
const std::streamsize jets_file::dbl_size = sizeof(double);
const std::streamsize jets_file::srt_size = sizeof(short);
const std::streamsize jets_file::szt_size = sizeof(size_t);
