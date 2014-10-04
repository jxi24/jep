#include "jep/statistics.h"
#include <sstream>

#define ERROR jepex(__func__,__LINE__,"statistics")
#define NPART 3

namespace jep {

    std::vector<val_t> statistics(int mode, std::vector<double> data, val_t E, val_t Rmax) {
        std::vector<val_t> theory;
        std::string file;
        std::vector<val_t> stats;

        for(unsigned char n = 0; n < NPART; n++) {
            std::stringstream ss;
            switch (n) {
                case 0:
                    ss << "gluon_";
                    break;
                case 1:
                    ss << "quark_";
                    break;
                case 2:
                    ss << "higgs_";
                    break;
                default:
                    throw ERROR << "Jet energy profile does not exist for that particle";
            }
            ss << (int)Rmax*10 << ".jep";
            file = ss.str();

            theory = getTheory(file, E);
            if(theory.size() != data.size()) throw ERROR << "Theory and Data have different number of points";
            if(mode == 1) stats.push_back(chi2(theory, data));
            else if(mode == 2) stats.push_back(dchi2(theory, data, Rmax));
            else if(mode == 3) stats.push_back(loglike(theory, data));
            else if(mode == 4) stats.push_back(dloglike(theory, data, Rmax));
            else throw ERROR << "Not a valid statistical test";
        }
        
        return stats;
    }

    std::vector<val_t> getTheory(std::string file, val_t E) {
       return reader(file.c_str()).psi(E);
    }

    val_t chi2(std::vector<val_t> theory, std::vector<double> data) {
        val_t chi = 0.0;

        for(unsigned char n = 0; n < data.size(); n++) {
            chi += pow(theory[n] - (val_t)data[n],2)/theory[n];
        }

        return chi;
    }

    val_t dchi2(std::vector<val_t> theory, std::vector<double> data, val_t Rmax) {
        val_t chi = 0.0;
        val_t dtheory, ddata;
        val_t rstep = (Rmax - 0.1)/data.size();

        for(unsigned int n = 0; n < data.size()-1; n++) {
            dtheory = (theory[n+1]-theory[n])/rstep;
            ddata = (val_t)(data[n+1]-data[n])/rstep;

            chi += pow(dtheory - ddata,2)/dtheory;
        }

        return chi;
    }

    val_t loglike(std::vector<val_t> theory, std::vector<double> data) {
        val_t logl = 1.0;

        for(unsigned int n = 0; n < data.size(); n++) {
            if( data[n] != 0 && theory[n] != 0) logl *= theory[n]/data[n];
        }

        return log(logl);
    }

    val_t dloglike(std::vector<val_t> theory, std::vector<double> data, val_t Rmax) {
        val_t logl = 1.0;
        val_t dtheory, ddata;
        val_t rstep = (Rmax - 0.1)/data.size();

        for(unsigned int n = 0; n < data.size()-1; n++) {
            dtheory = (theory[n+1]-theory[n])/rstep;
            ddata = (val_t)(data[n+1]-data[n])/rstep;

            if( ddata != 0 && dtheory != 0) logl *= std::abs(dtheory/ddata);
            if(logl != logl) std::cout << data[n+1] << " " << data[n] << std::endl;
        }

        return log(logl);
    }

}//End Namespace jep
