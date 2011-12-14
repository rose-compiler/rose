#ifndef ROSE_GENERIC_DATAFLOW_COMMON_H
#define ROSE_GENERIC_DATAFLOW_COMMON_H

#include <sage3.h>
#include <list>
using std::list;
#include <map>
using std::map;
using std::pair;
using std::make_pair;
#include <set>
using std::set;
#include <vector>
using std::vector;
#include <string>
using std::string;
#include <iostream>
using std::ostream;
using std::ofstream;
#include <sstream>
using std::stringstream;
using std::ostringstream;
using std::endl;
using std::cout;
using std::cerr;

#include "AnalysisDebuggingUtils.h"

using namespace VirtualCFG;

const int ZERO = 0;
//const int SPECIAL = 1;
const int INF = 10101010;
const std::string ZEROStr = "0";
//const std::string SPECIALStr = "$";

inline bool XOR(bool x, bool y) { return x != y; }

#define SgDefaultFile Sg_File_Info::generateDefaultFileInfoForTransformationNode()

/* #############################
   ######### T Y P E S #########
   ############################# */

typedef long long quad;
//typedef quad variable;

typedef std::map<quad, quad>                     m_quad2quad;
typedef std::map<quad, std::string>              m_quad2str;
typedef std::map<quad, m_quad2quad>              m_quad2map;
typedef std::pair<quad, quad>                    quadpair;
typedef std::list<quad>                          quadlist;
typedef std::map<quad, quadpair>                 m_quad2quadpair;
typedef std::map<quad, bool>                     m_quad2bool;

#endif
