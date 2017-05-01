#ifndef LTSMIN_CONNECTION_H
#define LTSMIN_CONNECTION_H

//to-do: license, author etc.

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

//SPRAY includes
#include "Flow.h"

//BOOST includes
#include "boost/algorithm/string.hpp"
#include "boost/algorithm/string/trim.hpp"
#include "boost/algorithm/string/regex.hpp"
#include "boost/regex.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/unordered_map.hpp"

using namespace SPRAY;
using namespace std;

namespace CodeThorn {

  class LtsminConnection {
    public:
      // model checking of "ltlProperty" on "processes"
      PropertyValue checkPropertyParPro(string ltlProperty, std::map<int, Flow*> processes);

    private:
      pair<string, string> generateDotGraphs(map<int, Flow*> processes);
      map<int, Flow*> componentAbstraction(map<int, Flow*> processes, ComponentApproximation compAbstraction);

      boost::unordered_map<std::set<int> , pair<std::string, std::string> > _generatedDotFiles;
  };
};
#endif
