#include "jep/statistics.h"

#include <vector>
#include <string>

#include "jep/exception.h"

#define ERROR jepex(__func__,__LINE__,"reader")

namespace jep {

    std::vector<val_t> statistics(int mode, std::vector<double> data) {
        val_t E = data[data.size()-1];
        std::vector<val_t> theory;
        std::string file;
        vector<val_t> stats;

        for(unsigned char n = 0; n < NPART; n++) {
            theory = getTheory(file, E);
            if(mode == 1) stats.pushback(chi2(theory, data));
            else if(mode == 2) stats.pushback(loglike(theory, data));
            else ERROR << "Not a valid statistical test";
        }
        
        return stats;
    }

    std::vector<val_t> getTheory(std::string file, val_t E) {
       reader* f = new reader(file.c_str());
       const header& h = f->get_header();
       std::vector<val_t> psi = f->psi(E);

       delete f;

       return psi;
    }

}
