#ifndef ROSE_isnan_H
#define ROSE_isnan_H

// In case anyone included a C header file, since isnan is a macro in C
#undef isnan

#include <boost/math/special_functions/fpclassify.hpp>

template<typename T>
bool rose_isnan(T x) {
    return boost::math::isnan(x);
}

#endif
