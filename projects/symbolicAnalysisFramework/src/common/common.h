#ifndef SAF_COMMON_H
#define SAF_COMMON_H

#include "rose.h"
#include <list>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;
using namespace VirtualCFG;

const int ZERO = 0;
//const int SPECIAL = 1;
const int INF = 10101010;
const std::string ZEROStr = "0";
//const std::string SPECIALStr = "$";

/* ###############################
   ######### M A C R O S #########
   ############################### */
#ifndef min
#define min(x, y) ( (x) < (y) ? x : y )
#endif

#ifndef max
#define max(x, y) ( (x) > (y) ? x : y )
#endif

#ifndef XOR
#define XOR(x, y)  ( ((x) || (y)) && !((x) && (y)) ) 
#endif

#define SgDefaultFile Sg_File_Info::generateDefaultFileInfoForTransformationNode()

/* #############################
   ######### T Y P E S #########
   ############################# */

typedef long long quad;
//typedef quad variable;

typedef map<quad, quad>                     m_quad2quad;
typedef map<quad, std::string>              m_quad2str;
typedef map<quad, m_quad2quad>              m_quad2map;
typedef pair<quad, quad>                    quadpair;
typedef list<quad>                          quadlist;
typedef map<quad, quadpair>                 m_quad2quadpair;
typedef map<quad, bool>                     m_quad2bool;

class printable
{
	public:
	virtual string str(string indent="")=0;
};

/* #####################################
   ######### F U N C T I O N S #########
   ##################################### */

string itostr(int num);

#endif
