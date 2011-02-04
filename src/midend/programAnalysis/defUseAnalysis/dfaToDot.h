#ifndef DFA_TO_DOT_H
#define DFA_TO_DOT_H

//#include "rose.h"
#include <string>
#include <map>
#include <set>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cctype>
#include <stdint.h>

#include "filteredCFG.h"
#include "DefUseAnalysis.h"

class LivenessAnalysis;

namespace VirtualCFG {

  template < typename FilterFunction > 
     std::ostream & dfaToDot(std::ostream & o,
                             std::string graphName,
                             std::vector < FilteredCFGNode < FilterFunction > > start,
                             DefUseAnalysis* dfa);
  template < typename FilterFunction > 
     std::ostream & dfaToDot(std::ostream & o,
                             std::string graphName,
                             std::vector < FilteredCFGNode < FilterFunction > > start,
                             DefUseAnalysis* dfa,
                             LivenessAnalysis* live);
}
#endif
