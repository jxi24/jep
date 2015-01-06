#ifndef binner_h
#define binner_h

#include <limits>

/*
 * Same convention as in ROOT TH1:
 * bin = 0;       underflow bin
 * bin = 1;       first bin with low-edge xlow INCLUDED
 * bin = nbins;   last bin with upper-edge xup EXCLUDED
 * bin = nbins+1; overflow bin
 */

template<class T, class Filler, class B = double>
class binner {
public:
  typedef T val_t;
  typedef B bin_t;
  typedef typename std::vector<bin_t>::iterator edge_iter;
  typedef typename std::vector<val_t>::iterator val_iter;
  typedef typename std::vector<bin_t>::size_type size_type;

  binner(size_type nbinsx, bin_t xlow, bin_t xup)
  : _nbins(nbinsx), _edges(_nbins+1), _values(_nbins+2)
  {
    const bin_t step = (xup-xlow)/_nbins;
    for (unsigned i=0;i<=_nbins;++i) {
      _edges[i] = xlow + i*step;
    }
  }
  template<class Bins> binner(const Bins& bins)
  : _nbins(bins.size()-1), _edges(_nbins+1), _values(_nbins+2)
  {
    for (size_type i=0,n=_edges.size();i<n;++i) {
      _edges[i] = bins[i];
    }
  }

  size_type fill(bin_t b) {
    size_type i = _nbins;
    for (;;--i) {
      if (b >= _edges[i]) {
        ++i;
        break;
      }
      if (i==0) break;
    }
    _fill(_values[i]);
    return i;
  }

  template<class X>
  size_type fill(bin_t b, const X& x) {
    size_type i = _nbins;
    for (;;--i) {
      if (b >= _edges[i]) {
        ++i;
        break;
      }
      if (i==0) break;
    }
    _fill(_values[i],x);
    return i;
  }

  val_t& operator[](size_type i) {
    return _values.at(i);
  }
  const val_t& operator[](size_type i) const {
    return _values.at(i);
  }

  bin_t low_edge(size_type i) const {
    return i ? _edges.at(i-1) : -std::numeric_limits<bin_t>::infinity();
  }
  bin_t up_edge(size_type i) const {
    return i<_edges.size() ? _edges.at(i) : std::numeric_limits<bin_t>::infinity();
  }

  size_type nbins() const {
    return _nbins;
  }

  const std::vector<bin_t>& edges() const {
    return _edges;
  }
  const std::vector<val_t>& values() const {
    return _values;
  }

protected:
  size_type _nbins;
  std::vector<bin_t> _edges;
  std::vector<val_t> _values;

  static Filler _fill;
};

struct hist_filler {
  void operator()(double& v, double x) { v += x; }
  void operator()(double& v) { v += 1; }
};

template<class T = double, class B = double>
class hist: public binner<T, hist_filler, B> {
public:
  typedef T val_t;
  typedef B bin_t;
  typedef typename std::vector<bin_t>::iterator edge_iter;
  typedef typename std::vector<val_t>::iterator val_iter;
  typedef typename std::vector<bin_t>::size_type size_type;

  hist(size_type nbinsx, bin_t xlow, bin_t xup)
  : binner<T, hist_filler, B>(nbinsx,xlow,xup) { }
  template<class Bins> hist(const Bins& bins)
  : binner<T, hist_filler, B>(bins) { }

  void normalize(bool with_overflow=false) {
    val_t integral = (with_overflow ? this->_values[0] : this->_values[1]);
    for (size_type i = (with_overflow ? 1 : 2),
                   n = this->_nbins+(with_overflow ? 2 : 1);
                   i<n; ++i) integral += this->_values[i];
    for (size_type i=0,n=this->_nbins+2;i<n;++i) this->_values[i] /= integral;
  }
};

#endif
