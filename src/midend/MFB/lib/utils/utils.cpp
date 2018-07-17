
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

SgExpression * buildPtrArrElemField(SgExpression * expr, SgVariableSymbol * array, SgExpression * idx, SgVariableSymbol * field) {
  SgExpression * result = SageBuilder::buildArrowExp(expr, SageBuilder::buildVarRefExp(array));
  if (idx != NULL) result = SageBuilder::buildPntrArrRefExp(result, idx);
  if (field != NULL) result = SageBuilder::buildDotExp(result, SageBuilder::buildVarRefExp(field));
  return result;
}

SgExpression * buildCallVarIdx(size_t idx, SgVariableSymbol * var, SgFunctionSymbol * accessor) {
  return SageBuilder::buildFunctionCallExp(accessor, SageBuilder::buildExprListExp(SageBuilder::buildVarRefExp(var), SageBuilder::buildIntVal(idx)));
}

void collect_symbol_template_info(SgSymbol * sym, SgSymbol * & tpl_sym, SgDeclarationScope * & nrscope, SgTemplateParameterPtrList * & tpl_params, SgTemplateArgumentPtrList * & tpl_args) {
  if (isSgTemplateSymbol(sym)) {
    SgTemplateDeclaration * decl = isSgTemplateSymbol(sym)->get_declaration();
    ROSE_ASSERT(decl != NULL);

    nrscope = decl->get_nonreal_decl_scope();
    tpl_params = &(decl->get_templateParameters());

  } else if (isSgTemplateClassSymbol(sym)) {
    SgTemplateClassDeclaration * decl = isSgTemplateClassDeclaration(isSgClassSymbol(sym)->get_declaration());
    ROSE_ASSERT(decl != NULL);

    nrscope = decl->get_nonreal_decl_scope();
    tpl_params = &(decl->get_templateParameters());
    tpl_args = &(decl->get_templateSpecializationArguments());

    SgScopeStatement * scope = decl->get_scope();
    ROSE_ASSERT(scope != NULL);
    tpl_sym = scope->lookup_template_class_symbol(decl->get_templateName(), NULL, NULL);

  } else if (isSgClassSymbol(sym)) {
    SgClassDeclaration * decl = isSgClassSymbol(sym)->get_declaration();
    ROSE_ASSERT(decl != NULL);

    SgTemplateInstantiationDecl * idecl = isSgTemplateInstantiationDecl(decl);
    if (idecl != NULL) {
      tpl_args = &(idecl->get_templateArguments());
      tpl_sym = idecl->get_templateDeclaration() ? idecl->get_templateDeclaration()->search_for_symbol_from_symbol_table() : NULL;
    }

  } else if (isSgTemplateMemberFunctionSymbol(sym)) {
    SgTemplateMemberFunctionDeclaration * decl = isSgTemplateMemberFunctionDeclaration(isSgTemplateMemberFunctionSymbol(sym)->get_declaration());
    ROSE_ASSERT(decl != NULL);

    nrscope = decl->get_nonreal_decl_scope();
    tpl_params = &(decl->get_templateParameters());
    tpl_args = &(decl->get_templateSpecializationArguments());

    SgScopeStatement * scope = decl->get_scope();
    ROSE_ASSERT(scope != NULL);
    tpl_sym = scope->lookup_template_member_function_symbol(decl->get_template_name(), decl->get_type(), NULL);

  } else if (isSgMemberFunctionSymbol(sym)) {
    SgMemberFunctionDeclaration * decl = isSgMemberFunctionSymbol(sym)->get_declaration();
    ROSE_ASSERT(decl != NULL);

    SgTemplateInstantiationMemberFunctionDecl * idecl = isSgTemplateInstantiationMemberFunctionDecl(decl);
    if (idecl != NULL) {
      tpl_args = &(idecl->get_templateArguments());
      tpl_sym = idecl->get_templateDeclaration() ? idecl->get_templateDeclaration()->search_for_symbol_from_symbol_table() : NULL;
    }

  } else if (isSgTemplateFunctionSymbol(sym)) {
    SgTemplateFunctionDeclaration * decl = isSgTemplateFunctionDeclaration(isSgTemplateFunctionSymbol(sym)->get_declaration());
    ROSE_ASSERT(decl != NULL);

    nrscope = decl->get_nonreal_decl_scope();
    tpl_params = &(decl->get_templateParameters());
    tpl_args = &(decl->get_templateSpecializationArguments());

    SgScopeStatement * scope = decl->get_scope();
    ROSE_ASSERT(scope != NULL);
    tpl_sym = scope->lookup_template_function_symbol(decl->get_template_name(), decl->get_type(), NULL);

  } else if (isSgFunctionSymbol(sym)) {
    SgFunctionDeclaration * decl = isSgFunctionSymbol(sym)->get_declaration();
    ROSE_ASSERT(decl != NULL);

    SgTemplateInstantiationFunctionDecl * idecl = isSgTemplateInstantiationFunctionDecl(decl);
    if (idecl != NULL) {
      tpl_args = &(idecl->get_templateArguments());
      tpl_sym = idecl->get_templateDeclaration() ? idecl->get_templateDeclaration()->search_for_symbol_from_symbol_table() : NULL;
    }

  } else if (isSgTemplateTypedefSymbol(sym)) {
    SgTemplateTypedefDeclaration * tpl_decl = isSgTemplateTypedefDeclaration(isSgTemplateTypedefSymbol(sym)->get_declaration());
    SgTemplateInstantiationTypedefDeclaration * tpl_inst = isSgTemplateInstantiationTypedefDeclaration(isSgTemplateTypedefSymbol(sym)->get_declaration());
    if (tpl_decl != NULL) {
      nrscope = tpl_decl->get_nonreal_decl_scope();
      tpl_params = &(tpl_decl->get_templateParameters());
      tpl_args = &(tpl_decl->get_templateSpecializationArguments());
      tpl_sym = tpl_decl->search_for_symbol_from_symbol_table();
      ROSE_ASSERT(tpl_sym != NULL);
    } else if (tpl_inst) {
      tpl_args = &(tpl_inst->get_templateArguments());

      SgTemplateTypedefDeclaration * tpl_decl = tpl_inst->get_templateDeclaration();
      ROSE_ASSERT(tpl_decl != NULL);

      tpl_sym = tpl_decl->search_for_symbol_from_symbol_table();
      ROSE_ASSERT(tpl_sym != NULL);
    } else {
      ROSE_ASSERT(false);
    }

  } else if (isSgTypedefSymbol(sym)) {
    SgTypedefDeclaration * decl = isSgTypedefSymbol(sym)->get_declaration();
    ROSE_ASSERT(decl != NULL);
#if 1
    SgTemplateTypedefDeclaration * tpl_decl = isSgTemplateTypedefDeclaration(decl); // that should not happen as it should be a SgTemplateTypedefSymbol
    SgTemplateInstantiationTypedefDeclaration * tpl_inst = isSgTemplateInstantiationTypedefDeclaration(decl);
    if (tpl_decl != NULL || tpl_inst != NULL) {
      ROSE_ASSERT(false); // FIXME
    }
#endif
    SgTemplateInstantiationTypedefDeclaration * idecl = isSgTemplateInstantiationTypedefDeclaration(decl);
    if (idecl != NULL) {
      tpl_args = &(idecl->get_templateArguments());
      tpl_sym = idecl->get_templateDeclaration() ? idecl->get_templateDeclaration()->search_for_symbol_from_symbol_table() : NULL;
    }

  } else if (isSgTemplateVariableSymbol(sym)) {
    SgInitializedName * iname = isSgTemplateVariableSymbol(sym)->get_declaration();
    ROSE_ASSERT(iname != NULL);

    SgTemplateVariableDeclaration * decl = isSgTemplateVariableDeclaration(iname->get_parent());
    ROSE_ASSERT(decl != NULL);

    nrscope = decl->get_nonreal_decl_scope();
    tpl_params = &(decl->get_templateParameters());
    tpl_args = &(decl->get_templateSpecializationArguments());

  } else if (isSgVariableSymbol(sym)) {
    SgInitializedName * iname = isSgVariableSymbol(sym)->get_declaration();
    ROSE_ASSERT(iname != NULL);

    SgNode * parent = iname->get_parent();
    ROSE_ASSERT(parent != NULL);

    // FIXME Parent can be a SgVariableDeclaration, or a SgTemplateParameter, or a SgTemplateArgument
    // FIXME There is no SgTemplateInstantiationVariableDeclaration

  } else if (isSgNonrealSymbol(sym)) {
    SgNonrealDecl * decl = isSgNonrealSymbol(sym)->get_declaration();
    ROSE_ASSERT(decl != NULL);

    nrscope = decl->get_nonreal_decl_scope();
    tpl_params = &(decl->get_tpl_params());
    tpl_args = &(decl->get_tpl_args());
    if (decl->get_templateDeclaration()) {
      tpl_sym = decl->get_templateDeclaration()->search_for_symbol_from_symbol_table();
      ROSE_ASSERT(tpl_sym != NULL);
    }
  }
}

}

}

