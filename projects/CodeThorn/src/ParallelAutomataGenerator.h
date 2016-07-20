// Author: Marc Jasper, 2016.

#ifndef PARALLEL_AUTOMATA_GENERATOR_H
#define PARALLEL_AUTOMATA_GENERATOR_H

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

  class NumberGenerator {
  public:
    NumberGenerator(int start);
    int next();
  private:
    int _start;
    int _current;
  };

  class CfaAnnotationGenerator {
  public:
    CfaAnnotationGenerator(int cfaId);
    std::string next();
  private:
    int _cfaId;
    int _currentId;
  };

  class ParallelAutomataGenerator {
  public:
    std::vector<Flow*> randomlySyncedCircleAutomata(int numAutomata, std::pair<int,int> numSyncsBounds, 
						     std::pair<int,int> numCirclesBounds, std::pair<int,int> lengthBounds, 
						    std::pair<int,int> numIntersectionsBounds);
    Flow* randomCircleAutomaton(int numCircles, 
				std::pair<int,int> lengthBounds, std::pair<int,int> numIntersectionsBounds, 
				NumberGenerator& labelIds, CfaAnnotationGenerator& annotations);
    void randomlySynchronizeAutomata(std::vector<Flow*> automata, std::pair<int,int> numSyncsBounds,
				     CfaAnnotationGenerator& synchedAnnotations);

  private:
    int randomIntInRange(std::pair<int,int> range);
    Edge chooseRandomEdge(Flow* cfa);
    Label chooseRandomNode(Flow* cfa);
  };

} // end of namespace SPRAY

#endif
