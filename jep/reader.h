#ifndef jep_reader_h
#define jep_reader_h

#include "jep/common.h"

#include <iosfwd>
#include <vector>

namespace jep {

class reader {
  protected:
    const char* const filename;
    std::ifstream * const dat;
    const header head; // header
    val_t** const psi_; // two dimensional array of values
                        // i.e. array of rows

    virtual header read_header();
    virtual val_t** init_psi();

    virtual val_t polint(unsigned char n, val_t x, const val_t* x_, const val_t* y_) const;

    static const size_t header_size;

    // keep track of what's currently read
    mutable const val_t* current_psi[4];
    mutable val_t current_E[4];
    mutable num_t cE; // current(last used) particle & energy indexes

  public:
    // constructor **************************************************
    reader(const char* filename);

    // delete *******************************************************
    virtual ~reader();

    // converters ***************************************************
    virtual void to_ascii(const char* filename) const;

    // getter *******************************************************
    virtual std::vector<val_t> psi(val_t E) const;
    virtual std::vector<val_t> psi_avg(num_t Enum, val_t Emin, val_t Emax) const;
    virtual std::vector<val_t> dpsi(val_t E) const;

    virtual const header& get_header() const;

};

}

#endif
