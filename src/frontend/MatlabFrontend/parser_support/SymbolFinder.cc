#include "sage3basic.h" 
#include "SymbolFinder.h"

bool SymbolFinder::symbol_exists(std::string symbolName, SgScopeStatement* scope)
  {
    /*    SgScopeStatement* currentScope = scope;

    SgScopeStatement* globalScope = SageInterface::getGlobalScope(scope);

    while(currentScope != NULL)  
    {
	if(currentScope->symbol_exists(symbolName))
	  return true;
	
	//getEnclosingScope() on global scope throws exception which we cannot catch
	if(currentScope == globalScope)
	  return false;

	currentScope = SageInterface::getEnclosingScope(currentScope);
	}*/

    SgVariableSymbol *symbol = SageInterface::lookupVariableSymbolInParentScopes(symbolName, scope);

    if(symbol != NULL)
      return true;
    else
      return false;
  }
