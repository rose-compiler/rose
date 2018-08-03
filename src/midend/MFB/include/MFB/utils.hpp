
#ifndef __MFB_UTILS_HPP__
#define __MFB_UTILS_HPP__



class SgVariableSymbol;
class SgType;
class SgScopeStatement;
class SgInitializer;
class SgExpression;
class SgFunctionSymbol;
class SgSymbol;
class SgDeclarationScope;
class SgTemplateParameter;
class SgTemplateArgument;

#include <vector>

typedef std::vector<SgTemplateParameter*> SgTemplateParameterPtrList;
typedef std::vector<SgTemplateArgument*> SgTemplateArgumentPtrList;

#include <string>
#include <map>

namespace MFB {

namespace Utils {

SgVariableSymbol * getExistingSymbolOrBuildDecl(const std::string & name, SgType * type, SgScopeStatement * scope, SgInitializer * init = NULL);

SgExpression * translateConstExpression(
  SgExpression * expr,
  const std::map<SgVariableSymbol *, SgVariableSymbol *> & param_to_local,
  const std::map<SgVariableSymbol *, SgVariableSymbol *> & iter_to_local
);

// Build: 'expr'->'array'['idx'] or 'expr'->'array'['idx'].'field'
SgExpression * buildPtrArrElemField(SgExpression * expr, SgVariableSymbol * array, SgExpression * idx, SgVariableSymbol * field = NULL);

// Build: 'func'('var', 'idx')
SgExpression * buildCallVarIdx(size_t idx, SgVariableSymbol * var, SgFunctionSymbol * func);

void collect_symbol_template_info(SgSymbol * sym, SgSymbol * & tpl_sym, SgDeclarationScope * & nrscope, SgTemplateParameterPtrList * & tpl_params, SgTemplateArgumentPtrList * & tpl_args);

}

}

#endif /* __MFB_UTILS_HPP__ */

