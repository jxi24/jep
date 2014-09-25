#ifndef jep_profiler_h
#define jep_profiler_h

#include <vector>

namespace fastjet {
  class PseudoJet;
}

namespace jep {

std::vector<double>
profile(const fastjet::PseudoJet* jet,
        double r_max, double r_step, unsigned short r_num);

}

#endif
