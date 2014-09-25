#include "jep/statistics.h"

#include <vector>
#include <string>
#include <cmath>

#include "jep/exception.h"

#define ERROR jepex(__func__,__LINE__,"reader")

enum particles { G, Q, H, NPART};

namespace jep {

    std::vector<val_t> statistics(int mode, std::vector<double> data, val_t E, val_t Rmax, val_t Rmin) {
        std::vector<val_t> theory;
        std::string file;
        vector<val_t> stats;

        for(unsigned char n = 0; n < NPART; n++) {
            theory = getTheory(file, E);
            if(theory.size() != data.size()) throw ERROR << "Theory and Data have different number of points";
            if(mode == 1) stats.push_back(chi2(theory, data));
            else if(mode == 2) stats.push_back(dchi2(theory, data, Rmax, Rmin));
            else throw ERROR << "Not a valid statistical test";
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

    val_t chi2(std::vector<val_t> theory, std::vector<double> data) {
        val_t chi = 0.0;

        for(unsigned char n = 0; n < data.size(); n++) {
            chi += pow(theory[n] - (val_t)data[n],2)/pow(theory[n],2);
        }

        return chi;
    }

    val_t dchi2(std::vector<val_t> theory, std::vector<double> data, val_t Rmax, val_t Rmin) {
        val_t chi = 0.0;
        val_t dtheory, ddata;
        val_t rstep = (Rmax-Rmin)/data.size();

        for(unsigned char n = 0; n < data.size()-1; n++) {
            dtheory = (theory[n+1]-theory[n])/rstep;
            ddata = (val_t)(data[n+1]-data[n])/rstep;

            chi += pow(dtheory - ddata,2)/pow(dtheory,2);
        }

        return chi;
    }

}//End Namespace jep
