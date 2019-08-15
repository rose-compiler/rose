
#ifndef DOT_GRAPH_CFG_FRONTEND_H
#define DOT_GRAPH_CFG_FRONTEND_H

#include <iostream>
#include <fstream>

#include "sage3basic.h"

// CodeThorn includes
#include "Flow.h"

// CodeThorn includes

// BOOST includes
#include "boost/lexical_cast.hpp"
#include "boost/unordered_set.hpp"
#include "boost/regex.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/algorithm/string/trim.hpp"
#include "boost/algorithm/string/regex.hpp"

namespace CodeThorn {

  typedef boost::unordered_map<std::string, boost::unordered_map<int, std::list<Edge> > > EdgeAnnotationMap;
  typedef std::pair<std::list<Flow>, EdgeAnnotationMap> CfgsAndAnnotationMap;

  /*! 
   * \brief Parses a .dot file digraph into a Flow CFG (using string annotations).
   * \author Marc Jasper
   * \date 2016.
   */
  class DotGraphCfgFrontend {
  public:
    // initializing the dot graph frontend
    //DotGraphCfgFrontend();
    // parse the CFG contained in the "filename" .dot-file and return a Flow CFG.
    Flow parseDotCfg(std::string filename);
    // parse the CFGs contained in the "filename" .dot-file and return a list of Flow CFGs.
    CfgsAndAnnotationMap parseDotCfgs(std::string filename);

  private:
    size_t determineStartNode(boost::unordered_set<size_t>& mostRecentSourceNodes, 
			      boost::unordered_set<size_t>& mostRecentTargetNodes);
  };

} // end of namespace CodeThorn

#endif
