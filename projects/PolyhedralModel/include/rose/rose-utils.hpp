/**
 * \file include/rose/rose-utils.hpp
 * \brief Define functions for template usage.
 * \author Tristan Vanderbruggen
 * \version 0.1
 * 
 * \bug Including "rose.h" before "ppl.hh" cause error with gmp library used by PPL\n
 * It's imply all PPL dependent files need to be include before Rose dependent files !\n\n
 * first error (others are very similar):\n
 * /usr/include/gmpxx.h: In static member function ‘static bool __gmp_binary_equal::eval(const __mpz_struct*, long unsigned int)’:\n
 * /usr/include/gmpxx.h:762: error: ‘1.0e+0’ cannot be used as a function\n\n
 * So, I include "maths/PPLUtils.hpp" here... and I include this header first in every rose-related header.
 */

#ifndef _ROSE_UTILS_HPP_
#define _ROSE_UTILS_HPP_

#include "maths/PPLUtils.hpp"

#include "rose/Variable.hpp"

#include <iostream>

std::ostream & operator << (std::ostream & out, SgExprStatement & arg);

std::ostream & operator << (std::ostream & out, SgInitializedName & arg);

std::ostream & operator << (std::ostream & out, RoseVariable & arg);

std::ostream & operator << (std::ostream & out, const SgExprStatement & arg);

std::ostream & operator << (std::ostream & out, const SgInitializedName & arg);

std::ostream & operator << (std::ostream & out, const RoseVariable & arg);

char * varToCstr(const SgInitializedName * var);
char * varToCstr(const RoseVariable var);

#endif /* _ROSE_UTILS_HPP_ */

