#include "jep/writer.h"

#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>

#include "jep/exception.h"
#define ERROR jepex(__func__,__LINE__,"writer")

namespace jep {

// writer ***********************************************************
writer::writer(const char* filename, const header& head)
: filename(filename), head(head), row_size(head.r_num*sizeof(val_t)),
  dat( new std::ofstream(filename, std::ofstream::binary) )
{
  try {

    dat->write((char*)&head, sizeof(header));

  } catch (std::exception& e) { throw ERROR << e.what(); }
}

writer::~writer() {
  dat->flush();

  std::streampos end = dat->tellp();
  dat->seekp(0);
  if ( (row_size*head.E_num+sizeof(header)) != (size_t)(end-dat->tellp()) )
    throw ERROR << "Written jep file "<<filename<<" has wrong size";

  delete dat;
}

void writer::write(const val_t* row) const
{
  try {

    dat->write((char*)row, row_size);

  } catch (std::exception& e) { throw ERROR << e.what(); }
}

// Er_iter **********************************************************
writer::Er_iter::Er_iter(const writer* w)
: head(w->head), fE(head.E_min), fr(head.r_min),
  iE(0), ir(0)
{ }

bool writer::Er_iter::next_E() {
  if (++iE == head.E_num) return false;
  else {
    fE += head.E_step;
    ir = 0;
    fr = head.r_min;
    return true;
  }
}

bool writer::Er_iter::next_r() {
  if (++ir == head.r_num) return false;
  else {
    fr += head.r_step;
    return true;
  }
}

val_t writer::Er_iter::E() const { return fE; }
val_t writer::Er_iter::r() const { return fr; }
num_t writer::Er_iter::Ei() const { return iE; }
num_t writer::Er_iter::ri() const { return ir; }

writer::Er_iter writer::begin() const { return Er_iter(this); }

} // end jep namespace
