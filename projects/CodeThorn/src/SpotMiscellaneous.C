#include "sage3basic.h"
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
    spot::ltl::relabeling_map relabeling;
    spot::ltl::atomic_prop_set* sap = spot::ltl::atomic_prop_collect(formula);
    for (spot::ltl::atomic_prop_set::iterator i=sap->begin(); i!=sap->end(); i++) {
      string newName = "p_" + (*i)->name();
      const spot::ltl::atomic_prop* relabeledProp = spot::ltl::atomic_prop::instance(newName, (*i)->env());
      relabeling[*i] = relabeledProp;
    }
    formula = spot::ltl::relabel(formula, spot::ltl::Pnn, &relabeling);
  }

  string result = spot::ltl::to_spin_string(formula);
  formula->destroy();
  return result;
}
