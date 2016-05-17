// Author: Marc Jasper, 2016.

#ifndef DOT_GRAPH_CFG_FRONTEND_H
#define DOT_GRAPH_CFG_FRONTEND_H

#include <iostream>
#include <fstream>

#include "sage3basic.h"

// SPRAY includes
#include "Flow.h"

// CodeThorn includes


// BOOST includes
#include "boost/lexical_cast.hpp"
#include "boost/unordered_set.hpp"
#include "boost/regex.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/algorithm/string/trim.hpp"
#include "boost/algorithm/string/regex.hpp"

namespace SPRAY {

  // Parses a .dot-file digraph into a Flow CFG
  class DotGraphCfgFrontend {
  public:
    // initializing the dot graph frontend
    //DotGraphCfgFrontend();
    // parse the CFG contained in the "filename" .dot-file and return a Flow CFG.
    Flow parseDotCfg(std::string filename);
  };

} // end of namespace SPRAY

#endif
