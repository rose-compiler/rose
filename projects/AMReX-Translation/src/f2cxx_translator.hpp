

#ifndef _F2CXX_TRANSLATOR_HPP

#define _F2CXX_TRANSLATOR_HPP 1

#include "rose.h"

namespace f2cxx
{
  struct DeclMaker
  {
    typedef std::pair<SgProcedureHeaderStatement*, SgFunctionDeclaration*> mapping;

    explicit
    DeclMaker(SgSourceFile& cppfile)
    : glob(cppfile.get_globalScope()), declmap()
    {
      ROSE_ASSERT(glob);
    }

    void operator()(SgProcedureHeaderStatement* n);

    std::vector<mapping>& decls() { return declmap; }

    SgGlobal*            glob;
    std::vector<mapping> declmap;
  };

  struct Translator
  {
    explicit
    Translator(SgSourceFile& cppfile)
    : cxx(&cppfile)
    {
      SgGlobal&                        glob = sg::deref(cppfile.get_globalScope());
      SgNamespaceSymbol&               nsp  = sg::deref(glob.lookup_namespace_symbol("amrex"));
      SgNamespaceDeclarationStatement& ndcl = sg::deref(nsp.get_declaration());

      amrexscope = ndcl.get_definition();
      ROSE_ASSERT(amrexscope);
    }

    void operator()(DeclMaker::mapping p);

    SgSourceFile*                  cxx;
    SgScopeStatement*              amrexscope;
    std::vector<SgScopeStatement*> scopes;
  };
}

#endif /* _F2CXX_TRANSLATOR_HPP */
