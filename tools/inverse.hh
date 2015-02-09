#ifndef inverse_hh
#define inverse_hh

#include <algorithm>

template<unsigned n=100, class V=double>
class inverse {
public:
  typedef V val_t;

private:
  val_t x_[n], y_[n];

public:
  template<class F>
  inverse(const F& fcn, val_t min, val_t max) {
    const unsigned maxi = n-1;
    const val_t step = (max-min)/maxi;

    x_[0]    = fcn(min);
    x_[maxi] = fcn(max);

    if ( x_[0] < x_[maxi] ) {
      y_[0]    = min;
      y_[maxi] = max;
      for (unsigned i=1; i<maxi; ++i)
        x_[i] = fcn( y_[i] = min + i*step );

    } else {
      std::swap(x_[0],x_[maxi]);
      y_[0]    = max;
      y_[maxi] = min;
      for (unsigned i=maxi; i!=0; --i)
        x_[i] = fcn( y_[i] = max - i*step );
    }
  }

  val_t operator()(val_t x) const {
    static const unsigned maxi = n-1;

    // return 0 for undefined values
    if (x<x_[0] || x_[maxi]<x) return 0.;

    // initial guess : linear in y
    unsigned i = (x-x_[0])*maxi/(y_[maxi]-y_[0]);

    // step direction
    const bool forward = ( x > x_[i] );

    // encompasing indices
    unsigned i1, i2;
    if (forward) {
      while (x > x_[i]) ++i;
      i1=i-1; i2=i;
    } else {
      while (x < x_[i]) --i;
      i1=i; i2=i+1;
    }

    // linear interpolation
    return y_[i1] + (y_[i2]-y_[i1])*(x-x_[i1])/(x_[i2]-x_[i1]);
  }

  static unsigned size() { return n; }
};

#endif
