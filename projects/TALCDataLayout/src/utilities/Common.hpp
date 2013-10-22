/*
 * common.h
 *
 *  Created on: Oct 15, 2012
 *      Author: Kamal Sharma
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <iostream>
#include <ostream>
#include <fstream>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;

#define foreach BOOST_FOREACH

extern std::ostream *tracePtr;
#define trace (*tracePtr)
#define traceerror (std::cerr)

namespace FieldTypes {
enum TYPE {
	INT, FLOAT, DOUBLE, UNKNOWN
};
}

namespace CompilerTypes {
enum TYPE {
	UNKNOWN, GCC, ICC, XLC, PGI
};
}

namespace AccessTypes {
enum TYPE {
	READ, WRITE, READWRITE
};
}

#ifndef MIN
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#endif

#endif /* COMMON_H_ */
