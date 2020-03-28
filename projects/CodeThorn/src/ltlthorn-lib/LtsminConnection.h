#ifndef LTSMIN_CONNECTION_H
#define LTSMIN_CONNECTION_H

#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <algorithm>
//#include <unordered_map>   
// NOTE: Using Boost instead because std::unordered_map does not support 
//       automatic hashing of std::pair values.

//CodeThorn includes
#include "PropertyValueTable.h"
#include "ParProAnalyzer.h" // definition of ComponentAbstraction
#include "DotGraphCfgFrontend.h" // typedef EdgeAnnotationMap

//CodeThorn includes
#include "Flow.h"

//BOOST includes
#include "boost/algorithm/string.hpp"
#include "boost/algorithm/string/trim.hpp"
#include "boost/algorithm/string/regex.hpp"
#include "boost/regex.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/unordered_map.hpp"

using namespace CodeThorn;
using namespace std;

namespace CodeThorn {

  /*! 
   * \brief Uses LTSmin to check LTL properties on synchronized process graphs (prototype implementation).
   * \author Marc Jasper
   * \date 2017.
   * \details Graphs are translated to Petri nets. 
   */
  class LtsminConnection {
    public:
      LtsminConnection(): _startTransitionAnnotation("") {};
      //constructors with automatic initialization
      LtsminConnection(string ltlFormulasFile):
        _startTransitionAnnotation("") { 
	init(ltlFormulasFile);
      };
      LtsminConnection(EdgeAnnotationMap& annotationMap): 
        _annotationMap(annotationMap),
        _startTransitionAnnotation("") {};
      LtsminConnection(EdgeAnnotationMap& annotationMap, string ltlFormulasFile): 
        _annotationMap(annotationMap),
        _startTransitionAnnotation("") {
	init(ltlFormulasFile);
      };
      void init(string ltlFormulasFile);
      string ltlFormula2LtsminSyntax(string formula);
      // model checking all LTL properties in "_ltlResults" on "processes"
      PropertyValueTable* checkLtlPropertiesParPro(map<int, Flow*> processes);
      // model checking of "ltlProperty" on "processes"
      PropertyValue checkPropertyParPro(string ltlProperty, std::map<int, Flow*> processes);

    private:
      bool isMustTransition(Edge e, map<int, Flow*>& processes, set<string> mayOnlyCommunication);
      bool isMustTransition(Edge e, map<int, Flow*>& processes);
      void loadFormulae(istream& input);
      string& parseWeakUntil(std::string& ltl_string);
      pair<string, string> generateDotGraphs(map<int, Flow*> processes);
      map<int, Flow*> componentAbstraction(map<int, Flow*> processes, ComponentApproximation compAbstraction);

      EdgeAnnotationMap _annotationMap;
      string _startTransitionAnnotation;
      PropertyValueTable* _ltlResults;
      boost::unordered_map<std::set<int> , pair<std::string, std::string> > _generatedDotFiles;
  };
};
#endif
