#ifndef jep_statistics_h
#define jep_statistics_h

#include "jep/common.h"
#include "jep/reader.h"
#include "jep/exception.h"

#include <vector>
#include <string>
#include <cmath>
#include <iostream>


namespace jep {

    std::vector<val_t> statistics(int mode, std::vector<double> data, val_t E, val_t Rmax);
    std::vector<val_t> getTheory(std::string file, val_t E);
    val_t chi2(std::vector<val_t> theory, std::vector<double> data);
    val_t dchi2(std::vector<val_t> theory, std::vector<double> data, val_t Rmax);
    val_t loglike(std::vector<val_t> theory, std::vector<double> data);
    val_t dloglike(std::vector<val_t> theory, std::vector<double> data, val_t Rmax);
}

#endif
