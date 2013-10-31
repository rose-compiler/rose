/*
 * TALC.hpp
 *
 *  Created on: Oct 17, 2012
 *      Author: Kamal Sharma
 */

#ifndef TALC_HPP_
#define TALC_HPP_

#include <iostream>
#include <ostream>
#include <fstream>

static std::ofstream null("/dev/null");
static std::ostream nullTrace(null.rdbuf());
std::ostream *tracePtr = &nullTrace;

int *AllocateInt(const void *context, const char *name, const int numEntities) ;
float *AllocateFloat(const void *context, const char *name, const int numEntities) ;
double *AllocateDouble(const void *context, const char *name, const int numEntities) ;


#endif /* TALC_HPP_ */
