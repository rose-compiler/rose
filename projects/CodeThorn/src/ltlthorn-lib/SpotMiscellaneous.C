#include "sage3basic.h"
#include "rose_config.h"
#ifdef HAVE_SPOT

#include "SpotMiscellaneous.h"

using namespace std;

std::string SpotMiscellaneous::spinSyntax(std::string ltlFormula) {
  spot::ltl::parse_error_list pel;
  const spot::ltl::formula* formula = spot::ltl::parse(ltlFormula, pel);
  if (spot::ltl::format_parse_errors(std::cerr, ltlFormula, pel)) {
    formula->destroy();						
    cerr<<"Error: ltl format error."<<endl;
    ROSE_ASSERT(0);
  }
  bool prefixAtomicPropositions = true;
  if (prefixAtomicPropositions) {
    boost::unordered_map<std::string, std::string> newNames;
    spot::ltl::atomic_prop_set* sap = spot::ltl::atomic_prop_collect(formula);
    for (spot::ltl::atomic_prop_set::iterator i=sap->begin(); i!=sap->end(); i++) {
      string newName = "p_" + (*i)->name();
      newNames[(*i)->name()] = newName;
    }
    SpotRenameVisitor visitor(newNames);
    formula = visitor.recurse(formula);
  }

  string result = spot::ltl::to_spin_string(formula);
  formula->destroy();
  return result;
}

#endif // end of "#ifdef HAVE_SPOT"
