#include <set>
#include <algorithm>
#include <iostream> // TODO: delete later

#include "VariableDeclarationBuilder.h"

#include "rose.h"
#include "sageGeneric.h"

#include "utility/utils.h"
#include "FastNumericsRoseSupport.h"
#include "TypeAttribute.h"


namespace si = SageInterface;
namespace sb = SageBuilder;
namespace ru = RoseUtils;

static inline
SgVarRefExp* isSgVarRefExpWrapper(SgNode *node)
{
  return isSgVarRefExp(node);
}

static
SgScopeStatement* varPlacementScope(SgScopeStatement* cand)
{
  ROSE_ASSERT(cand);

  if (isSgGlobal(cand) || isSgFunctionDefinition(cand->get_parent()))
    return cand;

  return varPlacementScope(sg::ancestor<SgScopeStatement>(cand));
}

static
SgScopeStatement* varPlacementScope(SgVarRefExp* varRef)
{
  ROSE_ASSERT(varRef);

  return varPlacementScope(sg::ancestor<SgScopeStatement>(varRef));
}


struct VariableDeclInserter
{
    typedef std::map<std::string, SgVariableDeclaration*> InsertedDeclContainer;

    explicit
    VariableDeclInserter(SgFunctionDeclaration* functionDecl)
    : insertedDecls(), fundecl(functionDecl)
    {}

    void createVarDecl(SgVarRefExp* varref)
    {
      ROSE_ASSERT(varref);

      std::string varname = ru::nameOf(varref);
      const bool  ignore = (  isSgMatlabForStatement(varref->get_parent())
                           || isSgFunctionCallExp(varref->get_parent())
                           || FastNumericsRoseSupport::isParameter(varref, fundecl)
                           || varname == "nargin"
                           );

      std::cerr << "**** " << varname << " ";
      if (ignore) { std::cerr << "~" << std::endl; return; }

      InsertedDeclContainer::iterator pos = insertedDecls.find(varname);

      if (pos == insertedDecls.end())
      {
        std::cerr << "!";
        SgType* vartype = FastNumericsRoseSupport::getInferredType(varref);

        if (!vartype)
        {
          std::cerr << " >" << varref->unparseToString() << " has no type"
                    << std::endl;
          ROSE_ASSERT(false);
        }

        SgScopeStatement*      scope   = varPlacementScope(varref);
        SgVariableDeclaration* vardecl = sb::buildVariableDeclaration(varname, vartype, NULL, scope);
        ROSE_ASSERT(vardecl);

        si::prependStatement(vardecl, scope);

        auto ins = insertedDecls.insert(std::make_pair(varname, vardecl));
        ROSE_ASSERT(ins.second);

        pos = ins.first;
      }

      SgVariableDeclaration* dclnode = pos->second;
      SgVarRefExp*           refnode = sb::buildVarRefExp(dclnode);
      ROSE_ASSERT(refnode);

      si::replaceExpression(varref, refnode, false /* keep old expr */);
      std::cerr << std::endl;
    }

    void operator()(SgNode* n) { createVarDecl(isSgVarRefExp(n)); }

  private:
    InsertedDeclContainer  insertedDecls;
    SgFunctionDeclaration* fundecl;
};

namespace MatlabAnalysis
{
  void buildVariableDeclarations(SgFunctionDeclaration* fundecl)
  {
    ROSE_ASSERT(fundecl);

    SgScopeStatement* functionBody = fundecl->get_definition();
    if (functionBody == NULL) return;

    Rose_STL_Container<SgNode*> varrefs = NodeQuery::querySubTree(functionBody, V_SgVarRefExp);

    std::for_each(varrefs.begin(), varrefs.end(), VariableDeclInserter(fundecl));
  }
}
