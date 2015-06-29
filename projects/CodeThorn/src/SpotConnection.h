#ifndef SPOT_CONNECTION_H
#define SPOT_CONNECTION_H

//to-do: license, author etc.

#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <algorithm>

//CodeThorn includes
#include "StateRepresentations.h"
#include "SpotTgba.h"
#include "PropertyValueTable.h"

//SPOT includes
#include "ltlparse/public.hh"
#include "ltlvisit/destroy.hh"
#include "tgba/tgbaexplicit.hh"
#include "tgba/tgbaproduct.hh"
#include "tgbaparse/public.hh"
#include "tgbaalgos/save.hh"
#include "ltlast/allnodes.hh"
#include "tgbaalgos/scc.hh"
#include "tgbaalgos/cutscc.hh"
#include "ltlparse/ltlfile.hh"
#include "tgbaalgos/ltl2tgba_fm.hh"
#include "ltlast/atomic_prop.hh"
#include "ltlvisit/apcollect.hh"
#include "ltlenv/environment.hh"
#include "ltlparse/public.hh"
#include "tgbaalgos/ltl2tgba_fm.hh"
#include "tgba/tgbaproduct.hh"
#include "tgbaalgos/gtec/gtec.hh"
#include "misc/timer.hh"
#include "ltlast/formula.hh"
#include "tgbaalgos/replayrun.hh"
#include "tgbaalgos/projrun.hh"

//BOOST includes
#include "boost/algorithm/string.hpp"
#include "boost/algorithm/string/trim.hpp"
#include "boost/algorithm/string/regex.hpp"
#include "boost/regex.hpp"
#include "boost/lexical_cast.hpp"

using namespace std;

namespace CodeThorn {

  struct FormulaPlusResult {
    std::string ltlString;
    bool expectedRes;
  };

  // define LtlProperty std::pair<int, std::string>
  struct LtlProperty {
    int propertyNumber;
    std::string ltlString;
  };

  typedef std::list<FormulaPlusResult*> ltlData;
  
  // an interface used to test LTL formulas on CodeThorns TransitionGraphs
  class SpotConnection {
    public:
      SpotConnection() {};
      //constructor with automatic initialization
      SpotConnection(std::string ltl_formulae_file) {init(ltl_formulae_file);};
      //an initilaization that reads in a text file with ltl formulae (RERS 2014 format). Extracts the behaviorProperties
      // and creates an ltlResults table of the respective size (all results initialized to be "unknown").
      void init(std::string ltl_formulae_file);
      //Takes a CodeThorn STG as a model and checks for all ltl formulae loaded into this SpotConnection object wether or not the model
      // satisfies them. Writes results into the "ltlResults" member of this object, only checks properties for which no results exist yet.
      // "inVals" and "outVals" refer to the input and output alphabet of the ltl formulae to be checked.
      // (If "withCounterExample" is selected, there will be a third column for falsified formulae with a corresonding
      // counterexample input sequence. If "spuriousNoAnswers" is set to true, falsified properties will be reported also for STGs 
      // that are not precise.)
      void checkLtlProperties(TransitionGraph& stg,
					std::set<int> inVals, std::set<int> outVals, bool withCounterExample, bool spuriousNoAnswers);
      // similar to "checkLtlProperties" above, but only checks a single property (property id specified as a parameter)
      void checkSingleProperty(int propertyNum, TransitionGraph& stg,
						std::set<int> inVals, std::set<int> outVals, bool withCounterexample, bool spuriousNoAnswers);
      //takes a SPOT TGBA text file and a file containing LTL formulae plus expected solutions (see RERS solutions examples).
      // utilizes the SPOT library to check whether the expected solutions are correct on the given model tgba.
      // deprecated, the interfaced version below is now used.
      void compareResults(std::string tgba_file, std::string ltl_fsPlusRes_file);
      // same purpose as compareResults(...) above. This version provides a dynamic link to the SPOT library for possible
      // on-the-fly computation. "inVals" and "outVals" refer to the input and output alphabet of the ltl formulae 
      // that will be checked.
      void compareResults(TransitionGraph& stg, std::string ltl_fsPlusRes_file, 
					std::set<int> inVals, std::set<int> outVals);
      // returns a pointer to the LTL results table.
      PropertyValueTable* getLtlResults();
      // resets the LTL results table to all unknown properties.
      void resetLtlResults();
      // only resets a specific entry in the results table.
      void resetLtlResults(int property);

    private:
      //Removes every "WU" in a string with 'W". Necessary because only accepts this syntax.
      string& parseWeakUntil(std::string& ltl_string);
      // surrounds a string with "!( ... )", therefore negating its represented formula
      void negateFormula(std::string& ltl_string);
      // returns a set of atomic propositions (variables in the LTL formulae) representing the given "ioVals" I/O values.
      // "maxInputVal" determines the greatest value to be prepended with an 'i' for input. Every integer greater than that
      // will be prepended with 'o'
      spot::ltl::atomic_prop_set* getAtomicProps(std::set<int> ioVals, int maxInputVal);
      // reads in a list of LTL formulas (file in RERS format, meaining only lines containing formulae begin with '(')
      std::list<std::string>* loadFormulae(istream& input);
      // takes a text file of RERS solutions and parses the formulae with their corresponding solutions (true/false)
      ltlData* parseSolutions(istream& input);

      // check a single LTL property and update the results table
      void checkAndUpdateResults(LtlProperty property, SpotTgba* ct_tgba, TransitionGraph& stg, 
                                                     bool withCounterexample, bool spuriousNoAnswers);
      //returns true if the given model_tgba satisfies the ltl formula ltl_string. returns false otherwise. 
      // The dict parameter is the model_tgba's dictionary of atomic propsitions. ce_ptr is an out parameter 
      // for a counter-example in case one is found by SPOT.
      bool checkFormula(spot::tgba* ct_tgba, std::string ltl_string, spot::bdd_dict* dict, std::string** ce_ptr = 0);
      // returns a string statement about how SPOT's verification result compares to the expected result.
      std::string comparison(bool spotRes, bool expectedRes, std::string& ltlFormula, std::string& ce);

      //returns a SPOT run (used for counter-examples). condensed information (IO only and Input only)
      std::string* formatRun(std::string& run);
       //take a spot run string representation and filter input/output behavior (newer, more modular version than "filter_run_IO_only")
      std::string filterCounterexample(std::string spotRun, bool includeOutputStates = false);
      //take a spot run string representation and filter input/output behavior
      std::string* filter_run_IO_only(string& spotRun, bool inputOnly = false);
      //small helper function for filter_run_IO_only
      std::string formatIOChar(std::string prop, bool firstEntry, bool cycleStart);

      //maps an integer ioVal to its ioVal'th letter in the alphabet. Prepended by 'i' for 1 to "maxIntVal"
      // and 'o' for "maxIntVal" to 26 (RERS format)
      std::string int2PropName(int ioVal, int maxInVal);

      //returns a list of all those properties that still have an unknown value as of now. The returned
      // list has to be deleted by the calling function.
      std::list<LtlProperty>* getUnknownFormulae();
      
      //a list of all properties 
      std::list<LtlProperty> behaviorProperties; 
      //a container for the results of the LTL property evaluation
      PropertyValueTable* ltlResults;
  };
};
#endif
