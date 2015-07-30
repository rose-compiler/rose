
#include "sage3basic.h"
#include "KLT/utils.hpp"

namespace KLT {

namespace Utils {

SgExpression * translateExpression(SgExpression * expr, const symbol_map_t & symbol_map) {
  SgExpression * res = SageInterface::copyExpression(expr);

  std::vector<SgVarRefExp *> var_refs = SageInterface::querySubTree<SgVarRefExp>(expr);
  std::vector<SgVarRefExp *>::const_iterator it_var_ref;
  for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++) {
    SgVariableSymbol * symbol = (*it_var_ref)->get_symbol();
    // TODO
  }

  return res;
}

SgStatement * translateStatement(SgStatement * stmt, const symbol_map_t & symbol_map) {
  SgStatement * res = SageInterface::copyStatement(stmt);

  // TODO

  return res;
}

}

}

