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

#include <unordered_map>

//BOOST includes
#include "boost/algorithm/string.hpp"
#include "boost/algorithm/string/trim.hpp"
#include "boost/algorithm/string/regex.hpp"
#include "boost/regex.hpp"
#include "boost/lexical_cast.hpp"

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
      LtsminConnection(std::string ltlFormulasFile):
        _startTransitionAnnotation("") { 
	init(ltlFormulasFile);
      };
      LtsminConnection(EdgeAnnotationMap& annotationMap): 
        _annotationMap(annotationMap),
        _startTransitionAnnotation("") {};
      LtsminConnection(EdgeAnnotationMap& annotationMap, std::string ltlFormulasFile): 
        _annotationMap(annotationMap),
        _startTransitionAnnotation("") {
	init(ltlFormulasFile);
      };
      void init(std::string ltlFormulasFile);
      std::string ltlFormula2LtsminSyntax(std::string formula);
      // model checking all LTL properties in "_ltlResults" on "processes"
    PropertyValueTable* checkLtlPropertiesParPro(std::map<int, CodeThorn::Flow*> processes);
      // model checking of "ltlProperty" on "processes"
    PropertyValue checkPropertyParPro(std::string ltlProperty, std::map<int, CodeThorn::Flow*> processes);

    private:
      bool isMustTransition(Edge e, std::map<int, CodeThorn::Flow*>& processes, std::set<std::string> mayOnlyCommunication);
      bool isMustTransition(Edge e, std::map<int, CodeThorn::Flow*>& processes);
      void loadFormulae(std::istream& input);
      std::string& parseWeakUntil(std::string& ltl_string);
      std::pair<std::string, std::string> generateDotGraphs(std::map<int, CodeThorn::Flow*> processes);
      std::map<int, CodeThorn::Flow*> componentAbstraction(std::map<int, CodeThorn::Flow*> processes, ComponentApproximation compAbstraction);

      EdgeAnnotationMap _annotationMap;
      std::string _startTransitionAnnotation;
      PropertyValueTable* _ltlResults;
    std::unordered_map<std::set<int> , std::pair<std::string, std::string>, boost::hash<std::set<int>> > _generatedDotFiles;
  };
};
#endif
