
// argv[1]: tgba-file-name (model)
// argv[2]: ltl-formulas-and-solutions-file-name

#ifndef SPOTDRIVER_H
#define SPOTDRIVER_H

#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

//SPOT includes
#include "ltlvisit/destroy.hh"
#include "ltlvisit/apcollect.hh"
#include "ltlenv/environment.hh"
#include "ltlparse/public.hh"
#include "ltlparse/ltlfile.hh"
#include "ltlast/formula.hh"
#include "ltlast/allnodes.hh"
#include "ltlast/atomic_prop.hh"
#include "tgba/tgbaproduct.hh"
#include "tgba/tgbaexplicit.hh"
#include "tgbaparse/public.hh"
#include "tgbaalgos/replayrun.hh"
#include "tgbaalgos/projrun.hh"
#include "tgbaalgos/gtec/gtec.hh"
#include "tgbaalgos/ltl2tgba_fm.hh"
#include "tgbaalgos/save.hh"
#include "tgbaalgos/scc.hh"
#include "tgbaalgos/cutscc.hh"

//BOOST includes
#include "boost/algorithm/string/trim.hpp"
#include "boost/algorithm/string/regex.hpp"
#include "boost/regex.hpp"

using namespace std;

struct FormulaPlusResult {
  std::string ltlString;
  bool expectedRes;
};

typedef std::list<FormulaPlusResult*> ltlData;

//Validate statements about ltl formulas on a given model. 
//Takes a tgba file as a model and a text file with LTL formulae and their expected results (RERS solution file format).
//Outputs for each formula if the expected result is correct or if an error is present.
// argv[1]: tgba-file-name (model)
// argv[2]: ltl-formulas-and-solutions-file-name
int main(int argc, char* argv[]);

//parse ltl formulas and their expected results (RERS format). Returns a list of those data pairs. 
ltlData* parse(istream& input);

//check if a ltl-formula on a given model matches an expected result (all passed in as parameters). 
//Returns true if the expected result is correct, false otherwise. Prints out error message if false.
bool checkFormula(spot::tgba_explicit_string* model_tgba, std::string ltl_string, 
                  bool expectedRes, spot::bdd_dict* dict);
//Transform every occurance of "WU" into "W" to match the syntax of the SPOT library
void parseWeakUntil(std::string& ltl_string);
//Negates an LTL formula string (by adding !( ... ) )
void negateFormula(std::string& ltl_string);

//print a SPOT run and adds additional, reduced output (IO only and Input only)
void displayRun(string& run);
//take a spot run string representation and filter input/ouput behavior
std::string* filter_run_IO_only(string spotRun, bool inputOnly = false);
//small helper function for filter_run_IO_only (code reuse)
std::string formatIOChar(std::string prop, bool firstEntry, bool cycleStart);

#endif
