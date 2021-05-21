
#include <sage3basic.h>
#include <sageInterface.h>
#include <sageGeneric.h>
#include <VariableIdMapping.h>

#include <unordered_set>

#include "RoseCompatibility.h"
#include "ClassHierarchyAnalysis.h"

namespace si = SageInterface;
namespace ct = CodeThorn;


namespace
{
  std::string typeNameOf(const SgClassDeclaration* dcl)
  {
    return SG_DEREF(dcl).get_name();
  }

  std::string typeNameOf(const SgClassDefinition& def)
  {
    return typeNameOf(def.get_declaration());
  }

  struct ExcludeTemplates
  {
    void handle(SgTemplateClassDeclaration&)          {}
    void handle(SgTemplateClassDefinition&)           {}
    void handle(SgTemplateFunctionDeclaration&)       {}
    void handle(SgTemplateFunctionDefinition&)        {}
    void handle(SgTemplateMemberFunctionDeclaration&) {}
    void handle(SgTemplateVariableDeclaration&)       {}
    //~ void handle(SgTemplateTypedefDeclaration&)        {} // may need some handling
  };

  struct NodeCollector : ExcludeTemplates
  {
      typedef std::unordered_set<SgNode*> NodeTracker;

      NodeCollector(NodeTracker& visited, ct::ClassAnalysis& classes, ct::CastAnalysis& casts)
      : visitedNodes(&visited), allClasses(&classes), allCasts(&casts)
      {}

      void addClass(SgClassDefinition& n)
      {
        allClasses->emplace(&n, ct::ClassData{});
      }

      void descend(SgNode& n);

      using ExcludeTemplates::handle;

      void handle(SgNode& n)                  { descend(n); }

      // classes
      void handle(SgClassDefinition& n)
      {
        SgClassDeclaration&     decl   = SG_DEREF(n.get_declaration());
        SgDeclarationStatement* defdcl = decl.get_definingDeclaration();
        SgClassDeclaration&     clsdef = SG_DEREF(isSgClassDeclaration(defdcl));

        // exclude secondary declarations
        if (clsdef.get_definition() != &n)
          return;

        //~ logInfo() << "extracting " << typeNameOf(&n) << std::endl;

        addClass(n);
        descend(n);
      }

      // casts
      void handle(SgCastExp& n)
      {
        SgExpression& expr = SG_DEREF(n.get_operand());

        // store cast types and cast expressions
        (*allCasts)[ct::CastDesc{expr.get_type(), n.get_type()}].push_back(&n);

        descend(n);
      }

    private:
      NodeTracker*       visitedNodes;
      ct::ClassAnalysis* allClasses;
      ct::CastAnalysis*  allCasts;
  };

  template <class SetT, class ElemT>
  inline
  bool alreadyProcessed(SetT& s, const ElemT& e)
  {
    return !s.insert(e).second;
  }

  void NodeCollector::descend(SgNode& n)
  {
    if (alreadyProcessed(*visitedNodes, &n))
      return;

    *this = sg::traverseChildren(std::move(*this), n);
  }
}

namespace CodeThorn
{

std::string typeNameOf(ClassKeyType key)
{
  return ::typeNameOf(SG_DEREF(key));
}

std::vector<ct::VariableId>
getDataMembers(RoseCompatibilityBridge& astctx, const SgClassDefinition* clazz)
{
  std::vector<ct::VariableId> res;

  for (SgDeclarationStatement* cand : clazz->get_members())
  {
    SgVariableDeclaration* dcl  = isSgVariableDeclaration(cand);

    if (dcl && !si::isStatic(dcl))
    {
      SgInitializedNamePtrList& lst = dcl->get_variables();

      ROSE_ASSERT(lst.size() == 1 && lst[0]);
      res.push_back(astctx.vmap().variableId(lst[0]));
    }
  }

  return res;
}


void inheritanceEdges( const SgClassDefinition* def,
                       std::function<void(const SgClassDefinition*, const SgClassDefinition*, bool, bool)> fn
                     )
{
  const SgClassDefinition&  clazz = SG_DEREF(def);
  const SgBaseClassPtrList& bases = clazz.get_inheritances();

  for (SgBaseClass* pbase : bases)
  {
    SgBaseClass&         base   = SG_DEREF(pbase);
    const bool           direct = base.get_isDirectBaseClass();

    if (!direct) continue;

    SgClassDeclaration*  pdecl  = base.get_base_class();

    if (!pdecl)
    {
      logWarn() << "base class declaration is not available "
                << typeid(base).name()
                << std::endl;

      continue;
    }

    SgBaseClassModifier&    bmod   = SG_DEREF(base.get_baseClassModifier());
    SgClassDeclaration&     decl   = SG_DEREF(pdecl);
    SgDeclarationStatement* defdcl = decl.get_definingDeclaration();
    SgClassDeclaration&     clsdef = SG_DEREF(isSgClassDeclaration(defdcl));

    fn(def, clsdef.get_definition(), bmod.isVirtual(), direct);
  }
}


void extractFromProject(ClassAnalysis& classes, CastAnalysis& casts, ProjectType n)
{
  NodeCollector::NodeTracker visited;

  sg::dispatch(NodeCollector{visited, classes, casts}, n);
}


bool classHasVirtualMethods(ClassKeyType clazz)
{
  const SgClassDefinition&             cldef = SG_DEREF(clazz);
  const SgDeclarationStatementPtrList& lst   = cldef.get_members();
  auto                                 aa    = lst.begin();
  auto                                 zz    = lst.end();

  return zz != std::find_if( aa, zz,
                             [](const SgDeclarationStatement* elem) -> bool
                             {
                               const SgMemberFunctionDeclaration* memfn = isSgMemberFunctionDeclaration(elem);

                               return memfn && memfn->get_functionModifier().isVirtual();
                             }
                           );
}


namespace
{
  ClassKeyType classDefinition_opt(SgDeclarationStatement& n)
  {
    SgDeclarationStatement* defdcl = n.get_definingDeclaration();

    return defdcl ? SG_ASSERT_TYPE(SgClassDeclaration, *defdcl).get_definition()
                  : nullptr
                  ;
  }

  ClassKeyType classDefinition_opt(const SgClassType& n)
  {
    return classDefinition_opt(SG_DEREF( n.get_declaration() ));
  }

  struct ClassDefinition : sg::DispatchHandler<std::pair<ClassKeyType, TypeKeyType> >
  {
      void descend(const SgNode* n)               { res = sg::dispatch(*this, n); }

      void handle(const SgNode& n)                { SG_UNEXPECTED_NODE(n); }
      void handle(const SgType& n)                { res = std::make_pair(nullptr, &n); }

      void handle(const SgModifierType& n)        { descend(n.get_base_type()); }
      void handle(const SgTypedefType& n)         { descend(n.get_base_type()); }
      void handle(const SgPointerType& n)         { descend(n.get_base_type()); }
      void handle(const SgReferenceType& n)       { descend(n.get_base_type()); }
      void handle(const SgRvalueReferenceType& n) { descend(n.get_base_type()); }
      void handle(const SgDeclType& n)            { descend(SG_DEREF(n.get_base_expression()).get_type()); }
      //~ void handle(SgAutoType& n)            { /* what is the real type? */ }

      void handle(const SgClassType& n)           { res = std::make_pair(classDefinition_opt(n), &n); }

      static
      ReturnType get(TypeKeyType n);
  };
}


std::pair<ClassKeyType, TypeKeyType>
getClassCastInfo (TypeKeyType n)
{
  return sg::dispatch(ClassDefinition{}, n);
}

std::ostream& operator<<(std::ostream& os, const FileInfo& fi)
{
  Sg_File_Info& where = SG_DEREF(SG_DEREF(fi.node).get_startOfConstruct());

  return os << "@" << where.get_line() << ":" << where.get_col();
}

std::string unparseToString(CastKeyType cast)
{
  return cast->unparseToString();
}

int typeOf(CastKeyType cast)
{
  return static_cast<int>(cast->get_cast_type());
}

}
