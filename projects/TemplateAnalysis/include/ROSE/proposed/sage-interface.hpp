
#ifndef ROSE_PROPOSED_SAGE_INTERFACE
#define ROSE_PROPOSED_SAGE_INTERFACE

class SgScopeStatement;
class SgDeclarationStatement;
class SgNamespaceDeclarationStatement;
class SgClassDeclaration;
class SgTypedefDeclaration;

class SgSymbol;
class SgClassSymbol;
class SgNamespaceSymbol;
class SgTypedefSymbol;

#include <string>

namespace ROSE {
  namespace proposed {
    namespace SageInterface {

      SgScopeStatement * getScopeDefinedBy(SgDeclarationStatement * decl);
      SgScopeStatement * getScopeDefinedBy(SgSymbol * sym);
      SgScopeStatement * getScopeDefinedBy(SgClassDeclaration * decl);
      SgScopeStatement * getScopeDefinedBy(SgClassSymbol * sym);
      SgScopeStatement * getScopeDefinedBy(SgNamespaceDeclarationStatement * decl);
      SgScopeStatement * getScopeDefinedBy(SgNamespaceSymbol * sym);
      SgScopeStatement * getScopeDefinedBy(SgTypedefDeclaration * decl);
      SgScopeStatement * getScopeDefinedBy(SgTypedefSymbol * sym);
      SgScopeStatement * getChildScopeByName(const std::string & name, SgScopeStatement * scope);

      SgSymbol * getSymbolFromString(std::string qualname, SgScopeStatement * scope);

    }
  }
}

#endif

