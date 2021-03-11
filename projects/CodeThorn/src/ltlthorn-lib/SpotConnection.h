#ifndef SPOT_CONNECTION_H
#define SPOT_CONNECTION_H

#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <algorithm>

#include "LTLRersMapping.h"

#include "rose_config.h"
#ifdef HAVE_SPOT

//CodeThorn includes
#include "EState.h"
#include "SpotTgba.h"
#include "ParProSpotTgba.h"
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
#include "ltlvisit/simplify.hh"
#include "ltlvisit/relabel.hh"
//#include "ltlast/atomic_prop.hh"

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

  struct LtlProperty {
    int propertyNumber;
    std::string ltlString;
  };

  /*! 
   * \brief Checks LTL properties on CodeThorn's TransitionGraph using the SPOT library.
   * \author Marc Jasper
   * \date 2014, 2015, 2016, 2017.
   */
  class SpotConnection {
    public:
      SpotConnection();
      //constructors with automatic initialization
      SpotConnection(std::string ltl_formulae_file);
      SpotConnection(std::list<std::string> ltl_formulae);
      //an initilaization that reads in a text file with ltl formulae (RERS 2014 format). Extracts the behaviorProperties
      // and creates an ltlResults table of the respective size (all results initialized to be "unknown").
      void init(std::string ltl_formulae_file);
      // initializes the SpotConnection to check the ltl formulae that are passed in as a list of strings.
      void init(std::list<std::string> ltl_formulae);
      //Takes a CodeThorn STG as a model and checks for all ltl formulae loaded into this SpotConnection object wether or not the model
      // satisfies them. Writes results into the "ltlResults" member of this object, only checks properties for which no results exist yet.
      // "inVals" and "outVals" refer to the input and output alphabet of the ltl formulae to be checked.
      // (If "withCounterExample" is selected, there will be a third column for falsified formulae with a corresonding
      // counterexample input sequence. If "spuriousNoAnswers" is set to true, falsified properties will be reported also for STGs 
      // that are not precise.)
      void checkLtlProperties(TransitionGraph& stg,
                              CodeThorn::LtlRersMapping ltlRersMapping, bool withCounterExample, bool spuriousNoAnswers);
      // variant for model checking the state space of parallel automata
      void checkLtlPropertiesParPro(ParProTransitionGraph& stg, bool withCounterexample, bool spuriousNoAnswers, set<std::string> annotationsOfModeledTransitions);
      // similar to "checkLtlProperties" above, but only checks a single property (property id specified as a parameter)
      void checkSingleProperty(int propertyNum, TransitionGraph& stg,
                               CodeThorn::LtlRersMapping ltlRersMapping, bool withCounterexample, bool spuriousNoAnswers);
      // model checking of "ltlProperty" on "stg"
      PropertyValue checkPropertyParPro(string ltlProperty, ParProTransitionGraph& stg, set<std::string> annotationsOfModeledTransitions);
      ParProSpotTgba* toTgba(ParProTransitionGraph& stg);
      // returns a pointer to the LTL results table.
      PropertyValueTable* getLtlResults();
      // resets the LTL results table to all unknown properties.
      void resetLtlResults();
      // only resets a specific entry in the results table.
      void resetLtlResults(int property);
      //maps an integer ioVal to its ioVal'th letter in the alphabet. Prepended by 'i' for 1 to "maxIntVal"
      // and 'o' for "maxIntVal" to 26 (RERS format)
      std::string int2PropName(int ioVal, int maxInVal);
      void setModeLTLDriven(bool ltlDriven);
      // read in an LTL formula string and return a formula with the same semantics in SPIN's syntax
      std::string spinSyntax(std::string ltlFormula);
      // returns a set of strings that represent the atomic propositions used in "ltlFormula"
      std::set<std::string> atomicPropositions(std::string ltlFormula);

    private:
      //Removes every "WU" in a string with 'W". Necessary because only accepts this syntax.
      string& parseWeakUntil(std::string& ltl_string);
      // surrounds a string with "!( ... )", therefore negating its represented formula
      void negateFormula(std::string& ltl_string);
      // returns the set of atomic propositions (boolean variables in the LTL formulae) in the list of LTL properties
      spot::ltl::atomic_prop_set* getAtomicProps();
      // returns the set of atomic propositions (boolean variables in the LTL formulae) in "ltlFormula"
      spot::ltl::atomic_prop_set* getAtomicProps(std::string ltlFormula);
      // returns a set of atomic propositions (variables in the LTL formulae) representing the given "ioVals" I/O values.
      // "maxInputVal" determines the greatest value to be prepended with an 'i' for input. Every integer greater than that
      // will be prepended with 'o'
      spot::ltl::atomic_prop_set* getAtomicProps(CodeThorn::LtlRersMapping ltlRersMapping);
      // reads in a list of LTL formulas (file in RERS format, meaining only lines containing formulae begin with '(')
      std::list<std::string>* loadFormulae(istream& input);

      // check a single LTL property and update the results table
      void checkAndUpdateResults(LtlProperty property, SpotTgba* ct_tgba, TransitionGraph& stg, 
                                                     bool withCounterexample, bool spuriousNoAnswers);
      //returns true if the given model_tgba satisfies the ltl formula ltl_string. returns false otherwise. 
      // The dict parameter is the model_tgba's dictionary of atomic propsitions. ce_ptr is an out parameter 
      // for a counter-example in case one is found by SPOT.
      bool checkFormula(spot::tgba* ct_tgba, std::string ltl_string, spot::bdd_dict* dict, std::string** ce_ptr = 0);

      //returns a SPOT run (used for counter-examples). condensed information (IO only and Input only)
      std::string* formatRun(std::string& run);
       //take a spot run string representation and filter input/output behavior (newer, more modular version than "filter_run_IO_only")
      std::string filterCounterexample(std::string spotRun, bool includeOutputStates = false);
      //take a spot run string representation and filter input/output behavior
      std::string* filter_run_IO_only(string& spotRun, bool inputOnly = false);
      //small helper function for filter_run_IO_only
      std::string formatIOChar(std::string prop, bool firstEntry, bool cycleStart);
      
      //a list of all properties 
      //      std::list<LtlProperty> behaviorProperties; 
      //a container for the results of the LTL property evaluation
      PropertyValueTable* ltlResults=nullptr;
      bool modeLTLDriven=false;
  };
};
#else

#include "TransitionGraph.h"
#include "ParProTransitionGraph.h"
#include "PropertyValueTable.h"

namespace CodeThorn {

  class ParProSpotTgba;

  class SpotConnection {
    public:
      SpotConnection();
      SpotConnection(std::string ltl_formulae_file);
      SpotConnection(std::list<std::string> ltl_formulae);
      void init(std::string ltl_formulae_file);
      void init(std::list<std::string> ltl_formulae);
      void checkLtlProperties(TransitionGraph& stg,
					CodeThorn::LtlRersMapping ltlRersMapping, bool withCounterExample, bool spuriousNoAnswers);
      void checkLtlPropertiesParPro(ParProTransitionGraph& stg, bool withCounterexample, bool spuriousNoAnswers, set<std::string> annotationsOfModeledTransitions);
      void checkSingleProperty(int propertyNum, TransitionGraph& stg,
						CodeThorn::LtlRersMapping ltlRersMapping, bool withCounterexample, bool spuriousNoAnswers);
      PropertyValue checkPropertyParPro(string ltlProperty, ParProTransitionGraph& stg, set<std::string> annotationsOfModeledTransitions);
      ParProSpotTgba* toTgba(ParProTransitionGraph& stg);
      PropertyValueTable* getLtlResults();
      void resetLtlResults();
      void resetLtlResults(int property);
      void setModeLTLDriven(bool ltlDriven);
      std::string spinSyntax(std::string ltlFormula);
      std::set<std::string> atomicPropositions(std::string ltlFormula);

      // deprecated, use ltlRersMapping.getIOString(ioVal) instead, only used by Solver 10
      std::string int2PropName(int ioVal, int maxInVal);

  private:
      void reportUndefinedFunction();
  };
};
#endif // end of HAVE_SPOT guard

#endif // end of SPOT_CONNECTION_H guard
