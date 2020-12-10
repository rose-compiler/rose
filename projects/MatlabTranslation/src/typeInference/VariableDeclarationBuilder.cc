#include <set>
#include <algorithm>
#include <iostream> // TODO: delete later

#include "VariableDeclarationBuilder.h"

#include "rose.h"
#include "sageGeneric.h"

#include "utility/utils.h"
#include "transformations/MatlabSimpleTransformer.h"
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

static inline
bool
fakeFunctionReference(SgVarRefExp* varref)
{
  return isSgFunctionCallExp(varref->get_parent());
}

struct VariableDeclInserter
{
    typedef std::map<std::string, SgInitializedName*> InsertedDeclContainer;

    explicit
    VariableDeclInserter(SgFunctionDeclaration* functionDecl)
    : insertedDecls(), fundecl(functionDecl)
    {}

    void createVarDecl(SgVarRefExp* varref)
    {
      ROSE_ASSERT(varref);

      std::string varname = ru::nameOf(varref);

      const bool  ignore = (  MatlabToCpp::forLoopIterationVariable(varref, varname)
                           || fakeFunctionReference(varref)
                           || FastNumericsRoseSupport::isParameter(varref, fundecl)
                           || varname == "nargin"
                           );

      if (ignore) { return; }

      InsertedDeclContainer::iterator pos = insertedDecls.find(varname);

      if (pos == insertedDecls.end())
      {
        SgType* vartype = FastNumericsRoseSupport::getInferredType(varref);
        ROSE_ASSERT(vartype);

        SgScopeStatement*      scope   = varPlacementScope(varref);
        SgVariableDeclaration* vardecl = sb::buildVariableDeclaration(varname, vartype, NULL, scope);
        ROSE_ASSERT(vardecl);

        SgInitializedName* 	   ininame = vardecl->get_decl_item(varname);
        ROSE_ASSERT(ininame);

        SgVariableSymbol*      varsym  = isSgVariableSymbol(ininame->search_for_symbol_from_symbol_table());
        ROSE_ASSERT(varsym);

        /// >>>> why is this needed?
        ininame = varsym->get_declaration();
        ROSE_ASSERT(ininame);

        vardecl->get_variables().at(0) = ininame;
        ininame->set_parent(vardecl);
        /// <<<< ???

        si::prependStatement(vardecl, scope);

        auto ins = insertedDecls.insert(std::make_pair(varname, ininame));
        ROSE_ASSERT(ins.second);

        pos = ins.first;
      }

      SgInitializedName*     varinit = pos->second;
      SgScopeStatement*      ascope  = sg::ancestor<SgScopeStatement>(varref);
      SgVarRefExp*           refnode = sb::buildVarRefExp(varinit, ascope /* not used */);
      ROSE_ASSERT(refnode);

      si::replaceExpression(varref, refnode, false /* keep old expr */);
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
