#ifndef test_stat_profile_h
#define test_stat_profile_h

// get jet energy profile
// ********************************************************
bool profile(
  const jet& jet,
  jep::val_t* integral_, // array of size h.r_num to return jet energy profile
  jep::val_t* derivative_, // array of size h.r_num to return jet energy profile
  const jep::header& h, // to get profile parameters
  bool warn=true, // print message if constituents outside tolerance
  jep::val_t tol_frac=1.1)  // fractional tolerance for h.r_max()
{
  const size_t nconst = jet.size();
  const jep::val_t r_max_tol = h.r_max()*tol_frac;

  if (!nconst) // if no constituents
    throw std::runtime_error("Jet has no constituents");

  // variables
  size_t     ci = 0; // current constituent
  jep::num_t ri = 0; // current radius index
  jep::val_t rv = h.r_min; // current radius value

  for (; ci<nconst; ++ci) {
    if (jet.r(ci)>rv) {
      ++ri;
      if (ri==h.r_num) { // handle overflow

        --ri;
        for (; ci<nconst; ++ci) {
          if (jet.r(ci)>r_max_tol) {
            if (warn) std::cerr << "Jet constituents outside cone" << std::endl;
            return false;
          }

          derivative_[ri] += jet.Et(ci);
        }

        break;
      }
      
      rv += h.r_step;
      derivative_[ri] = 0.;
      integral_[ri] = 0.;
    }
    derivative_[ri] += jet.Et(ci);
  }

  // integrate
  for (short i=0; i<h.r_num; ++i)
    for (short k=0; k<=i; ++k) integral_[i] += derivative_[k];

  // normalize
  for (short i=0; i<h.r_num; ++i) {
    derivative_[i] /= integral_[h.r_num-1];
    integral_  [i] /= integral_[h.r_num-1];
  }

  return true;
}

#endif
