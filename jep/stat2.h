#ifndef jep_stat2_h
#define jep_stat2_h

namespace jep {

template<typename D, typename H> // D & H dereference to double
double statistic(D data, H hypoth, int n, void(*fcn)(double&,double,double),
                 double start=0.) {
  double stat = start;
  D di = data;
  H hi = hypoth;
  for (int i=0; i<n; ++i) {
    fcn(stat,*di,*hi); // accumulate statistic
    ++di; // increment iterators
    ++hi;
  }
  return stat;
}

inline double sq(double x) { return x*x; }

void chi2(double& stat, double data, double hypoth) {
  stat += sq(hypoth - data)/hypoth;
}

void like(double& stat, double data, double hypoth) {
  if ( data>0. && hypoth>0. )
    stat *= hypoth/data;
}

}

#endif
