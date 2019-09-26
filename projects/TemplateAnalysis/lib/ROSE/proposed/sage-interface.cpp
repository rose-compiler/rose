
#include "ROSE/proposed/sage-interface.hpp"

#include "sage3basic.h"

namespace ROSE { namespace proposed { namespace SageInterface {

SgScopeStatement * getScopeDefinedBy(SgDeclarationStatement * decl) {
  ROSE_ASSERT(decl != NULL);

  SgClassDeclaration * xdecl = isSgClassDeclaration(decl);
  SgTypedefDeclaration * tddecl = isSgTypedefDeclaration(decl);
  SgNamespaceDeclarationStatement * nspdecl = isSgNamespaceDeclarationStatement(decl);

  SgScopeStatement * scope = NULL;
  if (xdecl != NULL) {
    scope = getScopeDefinedBy(xdecl);
  } else if (tddecl != NULL) {
    scope = getScopeDefinedBy(tddecl);
  } else if (nspdecl != NULL) {
    scope = getScopeDefinedBy(nspdecl);
  } else {
    ROSE_ASSERT(false);
  }
  return scope;
}

SgScopeStatement * getScopeDefinedBy(SgSymbol * sym) {
  ROSE_ASSERT(sym != NULL);

  SgClassSymbol * csym = isSgClassSymbol(sym);
  SgTypedefSymbol * tdsym = isSgTypedefSymbol(sym);
  SgNamespaceSymbol * nspsym = isSgNamespaceSymbol(sym);

  SgScopeStatement * scope = NULL;
  if (csym != NULL) {
    scope = getScopeDefinedBy(csym);
  } else if (tdsym != NULL) {
    scope = getScopeDefinedBy(tdsym);
  } else if (nspsym != NULL) {
    scope = getScopeDefinedBy(nspsym);
  } else {
    ROSE_ASSERT(false);
  }
  return scope;
}

SgScopeStatement * getScopeDefinedBy(SgClassDeclaration * decl) {
  ROSE_ASSERT(decl != NULL);

  SgDeclarationStatement * def_decl = decl->get_definingDeclaration();
  ROSE_ASSERT(def_decl != NULL);
  SgClassDeclaration * def_cdecl = isSgClassDeclaration(def_decl);
  ROSE_ASSERT(def_cdecl != NULL);
  SgClassDefinition * cdefn = def_cdecl->get_definition();
  ROSE_ASSERT(cdefn != NULL);

  return cdefn;
}

SgScopeStatement * getScopeDefinedBy(SgClassSymbol * sym) {
  ROSE_ASSERT(sym != NULL);

  SgClassDeclaration * decl = sym->get_declaration();
  ROSE_ASSERT(decl != NULL);

  return getScopeDefinedBy(decl);
}

SgScopeStatement * getScopeDefinedBy(SgNamespaceDeclarationStatement * decl) {
  ROSE_ASSERT(decl != NULL);

  SgDeclarationStatement * def_decl = decl->get_definingDeclaration();
  ROSE_ASSERT(def_decl != NULL);
  SgNamespaceDeclarationStatement * def_nspdecl = isSgNamespaceDeclarationStatement(def_decl);
  ROSE_ASSERT(def_nspdecl != NULL);
  SgNamespaceDefinitionStatement * nspdefn = def_nspdecl->get_definition();
  ROSE_ASSERT(nspdefn != NULL);
  SgNamespaceDefinitionStatement * glob_nspdefn = nspdefn->get_global_definition();
  ROSE_ASSERT(glob_nspdefn != NULL);

  return glob_nspdefn;
}

SgScopeStatement * getScopeDefinedBy(SgNamespaceSymbol * sym) {
  ROSE_ASSERT(sym != NULL);

  SgNamespaceDeclarationStatement * decl = sym->get_declaration();
  ROSE_ASSERT(decl != NULL);

  return getScopeDefinedBy(decl);
}

SgScopeStatement * getScopeDefinedBy(SgTypedefDeclaration * decl) {
  ROSE_ASSERT(decl != NULL);

  SgType * btype = decl->get_base_type();
  ROSE_ASSERT(btype != NULL);
  btype = btype->stripTypedefsAndModifiers();
  ROSE_ASSERT(btype != NULL);
  SgNamedType * ntype = isSgNamedType(btype);
  ROSE_ASSERT(ntype != NULL);
  SgDeclarationStatement * declstmt = ntype->get_declaration();
  ROSE_ASSERT(declstmt != NULL);

  return getScopeDefinedBy(declstmt);
}

SgScopeStatement * getScopeDefinedBy(SgTypedefSymbol * sym) {
  ROSE_ASSERT(sym != NULL);

  SgTypedefDeclaration * decl = sym->get_declaration();
  ROSE_ASSERT(decl != NULL);

  return getScopeDefinedBy(decl);
}

SgScopeStatement * getChildScopeByName(const std::string & name, SgScopeStatement * scope) {
  ROSE_ASSERT(scope != NULL);

  SgSymbol * sym = scope->lookup_symbol(name);
  ROSE_ASSERT(sym != NULL);

  scope = SageInterface::getScopeDefinedBy(sym);
  ROSE_ASSERT(scope);

  return scope;
}

SgSymbol * getSymbolFromString(std::string qualname, SgScopeStatement * scope) {
  ROSE_ASSERT( !qualname.empty() );

  if (qualname.find("::") == 0) {
    ROSE_ASSERT(isSgGlobal(scope) != NULL); // FIXME should just reset the scope to global_scope_accross_file
    qualname = qualname.substr(2);
  }

  SgSymbol * sym = NULL;

  std::string name;
  std::string::size_type current = 0;
  std::string::size_type next;
  do {
    next = qualname.find("::", current);
    if (next != std::string::npos) {
      name = qualname.substr(current, next - current);
      next += 2;
    } else {
      name = qualname.substr(current);
    }
    ROSE_ASSERT(!name.empty());

    sym = scope->lookup_symbol(name); // TODO what if there is more than one match???
    ROSE_ASSERT(sym != NULL);

    scope = SageInterface::getScopeDefinedBy(sym);
    ROSE_ASSERT(scope != NULL);

    current = next;
  } while (current != std::string::npos);

  return sym;
}

}}}

