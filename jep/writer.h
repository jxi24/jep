#ifndef jep_writer_h
#define jep_writer_h

#include "jep/common.h"

#include <iosfwd>

namespace jep {

class writer {
  protected:
    const char* const filename;
    const header& head; // reference to the header struct
    const size_t row_size;
    std::ofstream *dat;

  public:
    writer(const char* filename, const header& head);
    virtual ~writer();

    virtual void write(const val_t* row) const;
    // expects an array of size head.r_num

    class Er_iter {
      const header& head;
      val_t fE, fr;
      num_t iE, ir;

      public:
      Er_iter(const writer* w);
      bool next_E();
      bool next_r();
      val_t  E() const;
      val_t  r() const;
      num_t Ei() const;
      num_t ri() const;
    };

    virtual Er_iter begin() const;

};

}

#endif
