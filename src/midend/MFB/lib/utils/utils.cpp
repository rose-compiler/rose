
#include "sage3basic.h"

#include "MFB/utils.hpp"

namespace MFB {

namespace Utils {

SgVariableSymbol * getExistingSymbolOrBuildDecl( const std::string & name, SgType * type, SgScopeStatement * scope, SgInitializer * init) {
  SgVariableSymbol * sym = scope->lookup_variable_symbol(name);
  if (sym == NULL) {
    SgVariableDeclaration * decl = SageBuilder::buildVariableDeclaration(name, type, init, scope);
    SageInterface::appendStatement(decl, scope);
    sym = scope->lookup_variable_symbol(name);
  }
  assert(sym != NULL);
  return sym;
}

SgExpression * translateConstExpression(
  SgExpression * expr,
  const std::map<SgVariableSymbol *, SgVariableSymbol *> & param_to_local,
  const std::map<SgVariableSymbol *, SgVariableSymbol *> & iter_to_local
) {
  SgExpression * result = SageInterface::copyExpression(expr);

  std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator it_sym_to_local;

  if (isSgVarRefExp(result)) {
    // Catch an issue when reading looptree from file. In this case, 'expr' may not have a valid parent.
    // If 'expr' is a SgVarRefExp, it causes an assertion to fail in SageInterface::replaceExpression

    SgVarRefExp * var_ref = (SgVarRefExp *)result;

    SgVariableSymbol * var_sym = var_ref->get_symbol();

    SgVariableSymbol * local_sym = NULL;

    it_sym_to_local = param_to_local.find(var_sym);
    if (it_sym_to_local != param_to_local.end())
      local_sym = it_sym_to_local->second;

    it_sym_to_local = iter_to_local.find(var_sym);
    if (it_sym_to_local != iter_to_local.end()) {
      assert(local_sym == NULL); // implies VarRef to a variable symbol which is both parameter and iterator... It does not make sense!

      local_sym = it_sym_to_local->second;
    }

    assert(local_sym != NULL); // implies VarRef to an unknown variable symbol (neither parameter or iterator)

    return SageBuilder::buildVarRefExp(local_sym);
  }
  
  std::vector<SgVarRefExp *> var_refs = SageInterface::querySubTree<SgVarRefExp>(result);
  std::vector<SgVarRefExp *>::const_iterator it_var_ref;
  for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++) {
    SgVarRefExp * var_ref = *it_var_ref;
    SgVariableSymbol * var_sym = var_ref->get_symbol();

    SgVariableSymbol * local_sym = NULL;

    it_sym_to_local = param_to_local.find(var_sym);
    if (it_sym_to_local != param_to_local.end())
      local_sym = it_sym_to_local->second;

    it_sym_to_local = iter_to_local.find(var_sym);
    if (it_sym_to_local != iter_to_local.end()) {
      assert(local_sym == NULL); // implies VarRef to a variable symbol which is both parameter and iterator... It does not make sense!

      local_sym = it_sym_to_local->second;
    }

    assert(local_sym != NULL); // implies VarRef to an unknown variable symbol (neither parameter or iterator)

    SageInterface::replaceExpression(var_ref, SageBuilder::buildVarRefExp(local_sym), true);
  }

  return result;
}

}

}

