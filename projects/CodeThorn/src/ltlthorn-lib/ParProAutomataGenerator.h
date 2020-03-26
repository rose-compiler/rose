
#ifndef PARALLEL_AUTOMATA_GENERATOR_H
#define PARALLEL_AUTOMATA_GENERATOR_H

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

  /*! 
   * \brief Generates consecutive integer values (simple counter).
   * \author Marc Jasper
   * \date 2016.
   */
  class NumberGenerator {
  public:
    NumberGenerator(int start);
    int next();
  private:
    int _current;
  };

  /*! 
   * \brief Generates default transition annotations.
   * \author Marc Jasper
   * \date 2016.
   */
  class CfaAnnotationGenerator {
  public:
    CfaAnnotationGenerator(int cfaId);
    std::string next();
  private:
    int _cfaId;
    int _currentId;
  };

  /*! 
   * \brief Generates random synchronized process graphs based on intersecting loops.
   * \author Marc Jasper
   * \date 2016.
   */
  class ParProAutomataGenerator {
  public:
    std::vector<Flow*> randomlySyncedCircleAutomata(int numAutomata, std::pair<int,int> numSyncsBounds, 
						     std::pair<int,int> numCirclesBounds, std::pair<int,int> lengthBounds, 
						    std::pair<int,int> numIntersectionsBounds);
    Flow* randomCircleAutomaton(int numCircles, 
				std::pair<int,int> lengthBounds, std::pair<int,int> numIntersectionsBounds, 
				NumberGenerator& labelIds, CfaAnnotationGenerator& annotations);
    void randomlySynchronizeAutomata(std::vector<Flow*> automata, std::pair<int,int> numSyncsBounds);

  private:
    Edge chooseRandomEdge(Flow* cfa);
    Label chooseRandomNode(Flow* cfa);
  };

} // end of namespace CodeThorn

#endif
