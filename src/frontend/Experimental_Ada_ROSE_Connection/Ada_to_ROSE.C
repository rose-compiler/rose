#include "sage3basic.h"

#include <numeric>
#include <type_traits>

#include "Diagnostics.h"
#include "rose_config.h"
#include "sageGeneric.h"
#include "sageBuilder.h"
#include "Ada_to_ROSE_translation.h"
#include "Ada_to_ROSE.h"
#include "AdaMaker.h"
#include "AdaUtil.h"
#include "AdaExpression.h"
#include "AdaStatement.h"

// turn on all GCC warnings after include files have been processed
#pragma GCC diagnostic warning "-Wall"
#pragma GCC diagnostic warning "-Wextra"

namespace sb = SageBuilder;
namespace si = SageInterface;

namespace Ada_ROSE_Translation
{

//
// logger

Sawyer::Message::Facility adalogger;

void logInit()
{
  adalogger = Sawyer::Message::Facility("ADA-Frontend", Rose::Diagnostics::destination);
}

//
// declaration store and retrieval

namespace
{
  /// stores a mapping from Unit_ID to constructed root node in AST
  //~ map_t<int, SgDeclarationStatement*> asisUnitsMap;

  /// stores a mapping from Element_ID to SgInitializedName
  map_t<int, SgInitializedName*> asisVarsMap;

  /// stores a mapping from Element_ID to Exception declaration
  map_t<int, SgInitializedName*> asisExcpsMap;

  /// stores a mapping from Declaration_ID to SgDeclarationStatement
  map_t<int, SgDeclarationStatement*> asisDeclsMap;

  /// stores a mapping from Element_ID to ROSE type declaration
  map_t<int, SgDeclarationStatement*> asisTypesMap;

  /// stores a mapping from an Element_ID to a loop statement
  /// \todo this should be localized in the AstContext class
  map_t<int, SgStatement*> asisLoopsMap;

  /// stores a mapping from string to builtin type nodes
  map_t<std::string, SgType*> adaTypesMap;
} // anonymous namespace

//~ map_t<int, SgDeclarationStatement*>& asisUnits() { return asisUnitsMap; }
map_t<int, SgInitializedName*>&      asisVars()  { return asisVarsMap;  }
map_t<int, SgInitializedName*>&      asisExcps() { return asisExcpsMap; }
map_t<int, SgDeclarationStatement*>& asisDecls() { return asisDeclsMap; }
map_t<int, SgDeclarationStatement*>& asisTypes() { return asisTypesMap; }
map_t<int, SgStatement*>&            asisLoops() { return asisLoopsMap; }
map_t<std::string, SgType*>&         adaTypes()  { return adaTypesMap;  }
ASIS_element_id_to_ASIS_MapType&     asisIdMap() { return asisMap;      }


//
// auxiliary classes and functions

LabelManager::~LabelManager()
{
  for (GotoContainer::value_type el : gotos)
    el.first->set_label(&lookupNode(labels, el.second));
}

void LabelManager::label(Element_ID id, SgLabelStatement& lblstmt)
{
  SgLabelStatement*& mapped = labels[id];

  ROSE_ASSERT(mapped == nullptr);
  mapped = &lblstmt;
}

void LabelManager::gotojmp(Element_ID id, SgGotoStatement& gotostmt)
{
  gotos.emplace_back(&gotostmt, id);
}

AstContext AstContext::scope_npc(SgScopeStatement& s) const
{
  AstContext tmp{*this};

  tmp.the_scope = &s;
  return tmp;
}

AstContext AstContext::scope(SgScopeStatement& s) const
{
  ROSE_ASSERT(s.get_parent());

  return scope_npc(s);
}

AstContext AstContext::labels(LabelManager& lm) const
{
  AstContext tmp{*this};

  tmp.all_labels = &lm;
  return tmp;
}

/// attaches the source location information from \ref elem to
///   the AST node \ref n.
void attachSourceLocation(SgLocatedNode& n, Element_Struct& elem)
{
  Source_Location_Struct& loc  = elem.Source_Location;
  std::string             unit{loc.Unit_Name};

  // \todo consider deleting existing source location information
  //~ delete n.get_file_info();
  //~ delete n.get_startOfConstruct();
  //~ delete n.get_endOfConstruct();

  n.set_file_info       (&mkFileInfo(unit, loc.First_Line, loc.First_Column));
  n.set_startOfConstruct(&mkFileInfo(unit, loc.First_Line, loc.First_Column));
  n.set_endOfConstruct  (&mkFileInfo(unit, loc.Last_Line,  loc.Last_Column));
}

namespace
{
  /// clears all mappings created during translation
  void clearMappings()
  {
    //~ asisUnits().clear();
    asisIdMap().clear();
    asisVars().clear();
    asisExcps().clear();
    asisDecls().clear();
    asisTypes().clear();
    asisLoops().clear();
    adaTypes().clear();
  }


  void handleElement(Element_Struct& elem, AstContext ctx, bool isPrivate = false);
  void handleStmt(Element_Struct& elem, AstContext ctx);
  void handleExceptionHandler(Element_Struct& elem, SgTryStmt& tryStmt, AstContext ctx);

  void handleElementID(Element_ID id, AstContext ctx)
  {
    handleElement(retrieveAs<Element_Struct>(asisIdMap(), id), ctx);
  }

  /// if @ref isPrivate @ref dcl's accessibility is set to private;
  /// otherwise nothing.
  void
  privatize(SgDeclarationStatement& dcl, bool isPrivate)
  {
    if (!isPrivate) return;

    dcl.get_declarationModifier().get_accessModifier().setPrivate();
  }

  //
  // forward getters

  struct NameInfo
  {
      NameInfo(std::string id, std::string full, SgScopeStatement* scope, Element_Struct* el)
      : ident(id), fullName(full), parent(scope), asisElem(el)
      {}

      std::string       ident;
      std::string       fullName;
      SgScopeStatement* parent;
      Element_Struct*   asisElem;

      Element_ID id() const { return SG_DEREF(asisElem).ID; }

    private:
      NameInfo() = delete;
    //~ Element_ID        asisID;
  };

  SgType&
  getAccessType(Definition_Struct& def, AstContext ctx);

  SgDeclarationStatement&
  getAliasedID(Element_ID elid, AstContext ctx);

  SgType&
  getDeclTypeID(Element_ID id, AstContext ctx);

  SgType&
  getDefinitionType(Definition_Struct& def, AstContext ctx);

  SgType&
  getDefinitionTypeID(Element_ID defid, AstContext ctx);

  NameInfo
  getExprName(Element_Struct& elem, AstContext ctx);

  SgNode&
  getExprType(Expression_Struct& elem, AstContext ctx);

  SgNode&
  getExprTypeID(Element_ID tyid, AstContext ctx);

  NameInfo
  getName(Element_Struct& elem, AstContext ctx);

  NameInfo
  getNameID(Element_ID el, AstContext ctx);

  SgClassDeclaration&
  getParentRecordDecl(Definition_Struct& def, AstContext ctx);

  SgClassDeclaration&
  getParentRecordDeclID(Element_ID defid, AstContext ctx);

  SgVariableDeclaration&
  getParm(Element_Struct& elem, AstContext ctx);

  SgAdaRangeConstraint&
  getRangeConstraint(Element_ID el, AstContext ctx);

  SgClassDefinition&
  getRecordBody(Record_Definition_Struct& rec, AstContext ctx);

  SgType&
  getVarType(Declaration_Struct& decl, AstContext ctx);


  //
  // Creator functors

  struct ElemCreator
  {
      explicit
      ElemCreator(AstContext astctx, bool privateItems = false)
      : ctx(astctx), privateElems(privateItems)
      {}

      void operator()(Element_Struct& elem)
      {
        handleElement(elem, ctx, privateElems);
      }

    private:
      AstContext ctx;
      bool       privateElems;
  };

  struct StmtCreator
  {
      explicit
      StmtCreator(AstContext astctx)
      : ctx(astctx)
      {}

      void operator()(Element_Struct& elem)
      {
        handleStmt(elem, ctx);
      }

      AstContext ctx;
  };

  struct ExHandlerCreator
  {
      ExHandlerCreator(AstContext astctx, SgTryStmt& tryStatement)
      : ctx(astctx), tryStmt(tryStatement)
      {}

      void operator()(Element_Struct& elem)
      {
        handleExceptionHandler(elem, tryStmt, ctx);
      }

      AstContext ctx;
      SgTryStmt& tryStmt;
  };

  struct HandlerTypeCreator
  {
      HandlerTypeCreator(AstContext astctx)
      : ctx(astctx)
      {}

      void operator()(Element_Struct& elem)
      {
        ROSE_ASSERT(elem.Element_Kind == An_Expression);

        Expression_Struct& asisexpr  = elem.The_Union.Expression;
        SgInitializedName* exception = isSgInitializedName(&getExprType(asisexpr, ctx));
        SgExpression&      exref     = mkExceptionRef(SG_DEREF(exception), ctx.scope());
        SgType&            extype    = mkExceptionType(exref);

        lst.push_back(&extype);
      }

      operator SgType&() const
      {
        ROSE_ASSERT(lst.size() > 0);

        if (lst.size() == 1)
          return SG_DEREF(lst[0]);

        return mkTypeUnion(lst);
      }

      AstContext           ctx;
      std::vector<SgType*> lst;
  };


  //
  // Creator classes

  struct ParmlistCreator
  {
      ParmlistCreator(SgFunctionParameterList& parms, AstContext astctx)
      : parmlist(parms), ctx(astctx)
      {}

      // CallableDeclaration is either derived from function declaration,
      // or an accept statement.
      template <class CallableDeclaration>
      ParmlistCreator(CallableDeclaration& callable, AstContext astctx)
      : ParmlistCreator(SG_DEREF(callable.get_parameterList()), astctx)
      {}

      void operator()(Element_Struct& elem)
      {
        SgVariableDeclaration&    decl = getParm(elem, ctx);
        SgInitializedNamePtrList& args = parmlist.get_args();

        for (SgInitializedName* parm : decl.get_variables())
        {
          parm->set_parent(&parmlist);
          args.push_back(parm);
        }
      }

    private:
      SgFunctionParameterList& parmlist;
      AstContext               ctx;
  };


  struct NameCreator
  {
      typedef NameInfo              result_type;
      typedef std::vector<NameInfo> result_container;

      explicit
      NameCreator(AstContext astctx)
      : ctx(astctx)
      {}

      void operator()(Element_Struct& elem)
      {
        names.push_back(getName(elem, ctx));
      }

      operator result_container() { return names; }

    private:
      AstContext       ctx;
      result_container names;
  };


  struct IfStmtCreator
  {
      typedef std::pair<SgExpression*, SgStatement*> branch_type;
      typedef std::vector<branch_type>               branch_container;

      explicit
      IfStmtCreator(AstContext astctx)
      : ctx(astctx)
      {}

      void commonBranch(Path_Struct& path, SgExpression* cond = nullptr)
      {
        SgBasicBlock& block     = mkBasicBlock();
        ElemIdRange   thenStmts = idRange(path.Sequence_Of_Statements);

        traverseIDs(thenStmts, asisIdMap(), StmtCreator{ctx.scope_npc(block)});
        branches.emplace_back(cond, &block);
      }

      void conditionedBranch(Path_Struct& path)
      {
        commonBranch(path, &getExprID(path.Condition_Expression, ctx));
      }

      void operator()(Element_Struct& elem)
      {
        Path_Struct& path = elem.The_Union.Path;

        switch (path.Path_Kind)
        {
          case An_If_Path:
            {
              ROSE_ASSERT(branches.size() == 0);
              conditionedBranch(path);
              break;
            }

          case An_Elsif_Path:
            {
              ROSE_ASSERT(branches.size() != 0);
              conditionedBranch(path);
              break;
            }

          case An_Else_Path:
            {
              ROSE_ASSERT(branches.size() != 0);
              commonBranch(path);
              break;
            }

          default:
            ROSE_ASSERT(false);
        }
      }

      static
      SgStatement*
      createIfStmt(SgStatement* elsePath, branch_type thenPath)
      {
        ROSE_ASSERT(thenPath.first && thenPath.second);

        return sb::buildIfStmt(thenPath.first, thenPath.second, elsePath);
      }

      operator SgStatement&()
      {
        SgStatement* elseStmt = nullptr;

        if (!branches.back().first)
        {
          elseStmt = branches.back().second;
          branches.pop_back();
        }

        return SG_DEREF( std::accumulate(branches.rbegin(), branches.rend(), elseStmt, createIfStmt) );
      }

    private:
      AstContext       ctx;
      branch_container branches;
  };

  struct CaseStmtCreator
  {
      typedef std::pair<SgExpression*, SgStatement*> branch_type;
      typedef std::vector<branch_type>               branch_container;

      CaseStmtCreator(AstContext astctx, SgSwitchStatement& caseStmt)
      : ctx(astctx), caseNode(caseStmt)
      {}

      void operator()(Element_Struct& elem)
      {
        Path_Struct& path = elem.The_Union.Path;

        switch (path.Path_Kind)
        {
          case A_Case_Path:
            {
              ElemIdRange                caseChoices = idRange(path.Case_Path_Alternative_Choices);
              std::vector<SgExpression*> choices     = traverseIDs(caseChoices, asisIdMap(), ExprSeqCreator{ctx});
              ElemIdRange                caseBlock   = idRange(path.Sequence_Of_Statements);

              // \todo reconsider the "reuse" of SgCommaOp
              //   SgCommaOp is only used to separate discrete choices in case-when
              ROSE_ASSERT(choices.size());
              SgExpression&              caseCond    = SG_DEREF( std::accumulate( choices.begin()+1, choices.end(),
                                                                                  choices.front(),
                                                                                  sb::buildCommaOpExp
                                                                                ));

              SgBasicBlock&              block       = mkBasicBlock();
              SgCaseOptionStmt*          sgnode      = sb::buildCaseOptionStmt(&caseCond, &block);

              attachSourceLocation(SG_DEREF(sgnode), elem);
              sgnode->set_has_fall_through(false);
              caseNode.append_case(sgnode);
              traverseIDs(caseBlock, asisIdMap(), StmtCreator{ctx.scope(block)});
              break;
            }

          default:
            ROSE_ASSERT(false);
        }
      }

    private:
      AstContext         ctx;
      SgSwitchStatement& caseNode; // \todo change to SgAdaCaseStmt
  };



  //
  // some(?) functions

  struct MakeTyperef : sg::DispatchHandler<SgType*>
  {
    typedef sg::DispatchHandler<SgType*> base;

    MakeTyperef()
    : base()
    {}

    void handle(SgNode& n)               { SG_UNEXPECTED_NODE(n); }

    void handle(SgType& n)               { res = &n; }
    void handle(SgTypedefDeclaration& n) { res = &mkTypedefType(n); }
    void handle(SgClassDeclaration& n)   { res = &mkRecordType(n); }
    void handle(SgAdaTaskTypeDecl& n)    { res = &mkAdaTaskType(n); }
  };


  SgNode&
  getExprType(Expression_Struct& typeEx, AstContext ctx)
  {
    SgNode* res = nullptr;

    switch (typeEx.Expression_Kind)
    {
      case An_Identifier:
        {
          // is it a type?
          // typeEx.Corresponding_Name_Declaration ?
          res = findFirst(asisTypes(), typeEx.Corresponding_Name_Definition);

          if (!res)
          {
            // is it an exception?
            // typeEx.Corresponding_Name_Declaration ?
            res = findFirst(asisExcps(), typeEx.Corresponding_Name_Definition);
          }

          if (!res)
          {
            // is it a predefined Ada type?
            res = findFirst(adaTypes(), std::string{typeEx.Name_Image});
          }

          if (!res)
          {
            // what is it?
            logWarn() << "unknown type name: " << typeEx.Name_Image << std::endl;

            ROSE_ASSERT(!PRODUCTION_CODE);
            res = sb::buildVoidType();
          }

          break /* counted in getExpr */;
        }

      case A_Selected_Component:
        {
          res = &getExprTypeID(typeEx.Selector, ctx);
          break /* counted in getExpr */;
        }

      default:
        logWarn() << "Unknown type expression: " << typeEx.Expression_Kind << std::endl;
        ROSE_ASSERT(!PRODUCTION_CODE);
        res = sb::buildVoidType();
    }

    return SG_DEREF(res);
  }

  SgNode&
  getExprTypeID(Element_ID tyid, AstContext ctx)
  {
    Element_Struct& elem = retrieveAs<Element_Struct>(asisIdMap(), tyid);
    ROSE_ASSERT(elem.Element_Kind == An_Expression);

    return getExprType(elem.The_Union.Expression, ctx);
  }


  SgType&
  getAccessType(Definition_Struct& def, AstContext ctx)
  {
    ROSE_ASSERT(def.Definition_Kind == An_Access_Definition);

    Access_Definition_Struct& access = def.The_Union.The_Access_Definition;

    switch (access.Access_Definition_Kind)
    {
      case An_Anonymous_Access_To_Variable:            // [...] access subtype_mark
      case An_Anonymous_Access_To_Constant:            // [...] access constant subtype_mark
      case An_Anonymous_Access_To_Procedure:           // access procedure
      case An_Anonymous_Access_To_Protected_Procedure: // access protected procedure
      case An_Anonymous_Access_To_Function:            // access function
      case An_Anonymous_Access_To_Protected_Function:  // access protected function
        {
          logWarn() << "ak: " << access.Access_Definition_Kind << std::endl;
          break;
        }

      case Not_An_Access_Definition: /* break; */ // An unexpected element
      default:
        logWarn() << "ak? " << access.Access_Definition_Kind << std::endl;
        ROSE_ASSERT(false);
    }

    ROSE_ASSERT(false);
    return SG_DEREF(sb::buildVoidType());
  }

  SgType&
  getDeclType(Element_Struct& elem, AstContext ctx)
  {
    if (elem.Element_Kind == An_Expression)
    {
      SgNode& basenode = getExprType(elem.The_Union.Expression, ctx);
      SgType* res      = sg::dispatch(MakeTyperef(), &basenode);

      return SG_DEREF(res);
    }

    ROSE_ASSERT(elem.Element_Kind == A_Definition);
    Definition_Struct& def = elem.The_Union.Definition;

    if (def.Definition_Kind == An_Access_Definition)
      return getAccessType(def, ctx);

    ROSE_ASSERT(false);
    return SG_DEREF(sb::buildVoidType());
  }

  SgType&
  getDeclTypeID(Element_ID id, AstContext ctx)
  {
    return getDeclType(retrieveAs<Element_Struct>(asisIdMap(), id), ctx);
  }

  SgType&
  getDefinitionTypeID(Element_ID defid, AstContext ctx);

  SgType&
  getDefinitionType(Definition_Struct& def, AstContext ctx)
  {
    SgType* res = nullptr;

    switch (def.Definition_Kind)
    {
      case A_Subtype_Indication:
        {
          Subtype_Indication_Struct& subtype   = def.The_Union.The_Subtype_Indication;

          res = &getDeclTypeID(subtype.Subtype_Mark, ctx);

          // \todo if there is no subtype constraint, shall we produce
          //       a subtype w/ NoConstraint, or leave the original type?
          if (subtype.Subtype_Constraint)
          {
            SgAdaTypeConstraint& range = getRangeConstraint(subtype.Subtype_Constraint, ctx);

            res = &mkAdaSubtype(SG_DEREF(res), range);
          }

          /* unused fields:
                bool       Has_Null_Exclusion;
          */
          break;
        }

      case A_Component_Definition:
        {
          Component_Definition_Struct& component = def.The_Union.The_Component_Definition;

          res = &getDefinitionTypeID(component.Component_Definition_View, ctx);
          break;
        }

      default:
        logWarn() << "Unhandled type definition: " << def.Definition_Kind << std::endl;
        res = sb::buildVoidType();
        ROSE_ASSERT(!PRODUCTION_CODE);
    }

    return SG_DEREF(res);
  }

  SgType&
  getDefinitionTypeID(Element_ID defid, AstContext ctx)
  {
    if (isInvaldId(defid))
    {
      logWarn() << "undefined type id: " << defid << std::endl;
      return SG_DEREF(sb::buildVoidType());
    }

    Element_Struct&     elem = retrieveAs<Element_Struct>(asisIdMap(), defid);
    ROSE_ASSERT(elem.Element_Kind == A_Definition);

    return getDefinitionType(elem.The_Union.Definition, ctx);
  }

  SgClassDeclaration&
  getParentRecordDecl(Definition_Struct& def, AstContext ctx)
  {
    ROSE_ASSERT(def.Definition_Kind == A_Subtype_Indication);

    Subtype_Indication_Struct& subtype = def.The_Union.The_Subtype_Indication;
    ROSE_ASSERT (subtype.Subtype_Constraint == 0);

    Element_Struct&            subelem = retrieveAs<Element_Struct>(asisIdMap(), subtype.Subtype_Mark);
    ROSE_ASSERT(subelem.Element_Kind == An_Expression);

    SgNode*                    basenode = &getExprType(subelem.The_Union.Expression, ctx);
    SgClassDeclaration*        res = isSgClassDeclaration(basenode);

    return SG_DEREF(res);
  }

  SgClassDeclaration&
  getParentRecordDeclID(Element_ID defid, AstContext ctx)
  {
    Element_Struct&     elem = retrieveAs<Element_Struct>(asisIdMap(), defid);
    ROSE_ASSERT(elem.Element_Kind == A_Definition);

    return getParentRecordDecl(elem.The_Union.Definition, ctx);
  }

  SgType&
  getVarType(Declaration_Struct& decl, AstContext ctx)
  {
    ROSE_ASSERT(  decl.Declaration_Kind == A_Variable_Declaration
               || decl.Declaration_Kind == A_Constant_Declaration
               || decl.Declaration_Kind == A_Component_Declaration
               || decl.Declaration_Kind == A_Deferred_Constant_Declaration
               );

    return getDefinitionTypeID(decl.Object_Declaration_View, ctx);
  }


  SgClassDefinition&
  getRecordBody(Record_Definition_Struct& rec, AstContext ctx)
  {
    SgClassDefinition&        sgnode = SG_DEREF( sb::buildClassDefinition() );
    ElemIdRange               components = idRange(rec.Record_Components);
    //~ ElemIdRange               implicits  = idRange(rec.Implicit_Components);

    traverseIDs(components, asisIdMap(), ElemCreator{ctx.scope_npc(sgnode)});

    // how to represent implicit components
    //~ traverseIDs(implicits, asisIdMap(), ElemCreator{ctx.scope_npc(sgnode)});

    /* unused nodes:
         Record_Component_List Implicit_Components
    */
    return sgnode;
  }

  SgClassDefinition&
  getRecordBodyID(Element_ID recid, AstContext ctx)
  {
    Element_Struct&           elem = retrieveAs<Element_Struct>(asisIdMap(), recid);
    ROSE_ASSERT(elem.Element_Kind == A_Definition);

    Definition_Struct&        def = elem.The_Union.Definition;
    ROSE_ASSERT(def.Definition_Kind == A_Record_Definition);

    return getRecordBody(def.The_Union.The_Record_Definition, ctx);
  }


  struct TypeFundamental
  {
    SgNode* n;
    bool    hasAbstract;
    bool    hasLimited;
    bool    hasTagged;
  };

  TypeFundamental
  getTypeFoundation(Declaration_Struct& decl, AstContext ctx)
  {
    ROSE_ASSERT( decl.Declaration_Kind == An_Ordinary_Type_Declaration );

    TypeFundamental         res{nullptr, false, false, false};
    Element_Struct&         elem = retrieveAs<Element_Struct>(asisIdMap(), decl.Type_Declaration_View);
    ROSE_ASSERT(elem.Element_Kind == A_Definition);

    Definition_Struct&      def = elem.The_Union.Definition;
    ROSE_ASSERT(def.Definition_Kind == A_Type_Definition);

    /* unused fields:
       Definition_Struct
         bool                           Has_Null_Exclusion;
    */
    Type_Definition_Struct& typenode = def.The_Union.The_Type_Definition;

    switch (typenode.Type_Kind)
    {
      case A_Derived_Type_Definition:              // 3.4(2)     -> Trait_Kinds
        {
          /*
             unused fields: (derivedTypeDef)
                Declaration_List     Implicit_Inherited_Declarations;
          */

          res.n = &getDefinitionTypeID(typenode.Parent_Subtype_Indication, ctx);
          break;
        }

      case A_Derived_Record_Extension_Definition:  // 3.4(2)     -> Trait_Kinds
        {
          SgClassDefinition&  def    = getRecordBodyID(typenode.Record_Definition, ctx);
          SgClassDeclaration& basecl = getParentRecordDeclID(typenode.Parent_Subtype_Indication, ctx);
          SgBaseClass&        parent = mkRecordParent(basecl);

          def.append_inheritance(&parent);

          /*
          Declaration_List     Implicit_Inherited_Declarations;
          Declaration_List     Implicit_Inherited_Subprograms;
          Declaration          Corresponding_Parent_Subtype;
          Declaration          Corresponding_Root_Type;
          Declaration          Corresponding_Type_Structure;
          Expression_List      Definition_Interface_List;
          */
          res.n = &def;
          break;
        }

      case A_Floating_Point_Definition:            // 3.5.7(2)
        {
          SgExpression&         digits     = getExprID_opt(typenode.Digits_Expression, ctx);
          SgAdaRangeConstraint& constraint = getRangeConstraint(typenode.Real_Range_Constraint, ctx);

          res.n = &mkAdaFloatType(digits, constraint);
          break;
        }

      case An_Unconstrained_Array_Definition:      // 3.6(2)
        {
          ElemIdRange                indicesAsis = idRange(typenode.Index_Subtype_Definitions);
          std::vector<SgExpression*> indicesSeq  = traverseIDs(indicesAsis, asisIdMap(), ExprSeqCreator{ctx});
          SgExprListExp&             indicesAst  = SG_DEREF(sb::buildExprListExp(indicesSeq));
          SgType&                    compType    = getDefinitionTypeID(typenode.Array_Component_Definition, ctx);

          res.n = &mkArrayType(compType, indicesAst);
          /* unused fields:
          */
          break ;
        }

      case A_Tagged_Record_Type_Definition:        // 3.8(2)     -> Trait_Kinds
        {
          SgClassDefinition& def = getRecordBodyID(typenode.Record_Definition, ctx);

          //~ logInfo() << "tagged ? " << typenode.Has_Tagged << std::endl;

          /* unused fields:
                bool                 Has_Private;
                bool                 Has_Tagged;
                Declaration_List     Corresponding_Type_Operators;
          */
          res = TypeFundamental{&def, typenode.Has_Abstract, typenode.Has_Limited, true};
          break;
        }


      case Not_A_Type_Definition: /* break; */     // An unexpected element
      case An_Enumeration_Type_Definition:         // 3.5.1(2)
      case A_Signed_Integer_Type_Definition:       // 3.5.4(3)
      case A_Modular_Type_Definition:              // 3.5.4(4)
      case A_Root_Type_Definition:                 // 3.5.4(14):  3.5.6(3)
      case An_Ordinary_Fixed_Point_Definition:     // 3.5.9(3)
      case A_Decimal_Fixed_Point_Definition:       // 3.5.9(6)
      case A_Constrained_Array_Definition:         // 3.6(2)
      case A_Record_Type_Definition:               // 3.8(2)     -> Trait_Kinds
      //  //|A2005 start
      case An_Interface_Type_Definition:           // 3.9.4      -> Interface_Kinds
      //  //|A2005 end
      case An_Access_Type_Definition:              // 3.10(2)    -> Access_Type_Kinds
      default:
        {
          logWarn() << "unhandled type kind " << typenode.Type_Kind << std::endl;
          ROSE_ASSERT(!PRODUCTION_CODE);
          res.n = sb::buildVoidType();
        }
    }

    ROSE_ASSERT(res.n);
    return res;
  }


  SgAdaTaskSpec&
  getTaskSpec(Declaration_Struct& decl, AstContext ctx)
  {
    ROSE_ASSERT(decl.Declaration_Kind == A_Task_Type_Declaration);

    SgAdaTaskSpec&          sgnode = mkAdaTaskSpec();

    if (decl.Type_Declaration_View == 0)
      return sgnode;

    sgnode.set_hasMembers(true);

    // Definition_ID Discriminant_Part
    Element_Struct&         elem = retrieveAs<Element_Struct>(asisIdMap(), decl.Type_Declaration_View);
    ROSE_ASSERT(elem.Element_Kind == A_Definition);

    Definition_Struct&      def = elem.The_Union.Definition;
    ROSE_ASSERT(def.Definition_Kind == A_Task_Definition);

    Task_Definition_Struct& tasknode = def.The_Union.The_Task_Definition;

    // visible items
    {
      ElemIdRange range = idRange(tasknode.Visible_Part_Items);

      traverseIDs(range, asisIdMap(), ElemCreator{ctx.scope_npc(sgnode)});
    }

    // private items
    {
      ElemIdRange range = idRange(tasknode.Private_Part_Items);
      ROSE_ASSERT((!range.empty()) == tasknode.Is_Private_Present);

      traverseIDs(range, asisIdMap(), ElemCreator{ctx.scope_npc(sgnode), true /* private items */});
    }

    /* unused fields: (Task_Definition_Struct)
         bool                  Has_Task;
    */
    return sgnode;
  }


  SgAdaTaskBody&
  getTaskBody(Declaration_Struct& decl, AstContext ctx)
  {
    ROSE_ASSERT(decl.Declaration_Kind == A_Task_Body_Declaration);

    SgAdaTaskBody& sgnode = mkAdaTaskBody();
    ElemIdRange    decls = idRange(decl.Body_Declarative_Items);
    ElemIdRange    stmts = idRange(decl.Body_Statements);

    traverseIDs(decls, asisIdMap(), StmtCreator{ctx.scope_npc(sgnode)});
    traverseIDs(stmts, asisIdMap(), StmtCreator{ctx.scope_npc(sgnode)});

    return sgnode;
  }


  NameInfo
  singleName(Declaration_Struct& decl, AstContext ctx)
  {
    ElemIdRange     range = idRange(decl.Names);
    ROSE_ASSERT(range.size() == 1);

    return getNameID(*range.first, ctx);
  }


  struct ScopeQuery : sg::DispatchHandler<SgScopeStatement*>
  {
    template <class SageNode>
    void def(SageNode& n)
    { res = n.get_definition(); }

    void handle(SgNode& n)               { SG_UNEXPECTED_NODE(n); }
    void handle(SgAdaPackageSpecDecl& n) { def(n); }
    void handle(SgAdaPackageBodyDecl& n) { def(n); }
  };

  SgScopeStatement&
  getScope(Element_Struct& elem, AstContext ctx)
  {
    ROSE_ASSERT(elem.Element_Kind == An_Expression);

    Expression_Struct& expr = elem.The_Union.Expression;
    ROSE_ASSERT (expr.Expression_Kind == An_Identifier);

    SgDeclarationStatement* dcl = findFirst(asisDecls(), expr.Corresponding_Name_Definition, expr.Corresponding_Name_Declaration);
    SgScopeStatement*       res = sg::dispatch(ScopeQuery(), dcl);

    return SG_DEREF(res);
  }

  /*
  SgDeclarationStatement*
  getDeclID_opt(Element_ID declid, AstContext ctx)
  {
    Element_Struct& elem = retrieveAs<Element_Struct>(asisIdMap(), declid);
    ROSE_ASSERT(elem.Element_Kind == An_Expression);

    return getDecl_opt(elem.The_Union.Expression, ctx)
  }
  */

  SgDeclarationStatement&
  getAliased(Expression_Struct& expr, AstContext ctx)
  {
    if (expr.Expression_Kind == An_Identifier)
      return SG_DEREF(getDecl_opt(expr, ctx));

    ROSE_ASSERT(expr.Expression_Kind == A_Selected_Component);
    return getAliasedID(expr.Selector, ctx);
  }

  SgDeclarationStatement&
  getAliasedID(Element_ID declid, AstContext ctx)
  {
    Element_Struct& elem = retrieveAs<Element_Struct>(asisIdMap(), declid);
    ROSE_ASSERT(elem.Element_Kind == An_Expression);

    return getAliased(elem.The_Union.Expression, ctx);
  }


  SgAdaRangeConstraint&
  getRangeConstraint(Element_ID el, AstContext ctx)
  {
    if (isInvaldId(el))
    {
      logWarn() << "Uninitialized element [range constraint]" << std::endl;
      return mkAdaRangeConstraint(mkRangeExp());
    }

    SgAdaRangeConstraint* res = nullptr;
    Element_Struct&       elem = retrieveAs<Element_Struct>(asisIdMap(), el);
    ROSE_ASSERT(elem.Element_Kind == A_Definition);

    Definition_Struct&    def = elem.The_Union.Definition;
    ROSE_ASSERT(def.Definition_Kind == A_Constraint);

    Constraint_Struct&    constraint = def.The_Union.The_Constraint;

    switch (constraint.Constraint_Kind)
    {
      case A_Simple_Expression_Range:             // 3.2.2: 3.5(3)
        {
          SgExpression& lb       = getExprID(constraint.Lower_Bound, ctx);
          SgExpression& ub       = getExprID(constraint.Upper_Bound, ctx);
          SgRangeExp&   rangeExp = mkRangeExp(lb, ub);

          res = &mkAdaRangeConstraint(rangeExp);
          break;
        }

      case A_Range_Attribute_Reference:           // 3.5(2)
        {
          SgExpression& expr     = getExprID(constraint.Range_Attribute, ctx);
          SgRangeExp&   rangeExp = SG_DEREF(isSgRangeExp(&expr));

          res = &mkAdaRangeConstraint(rangeExp);
          break;
        }

      case Not_A_Constraint: /* break; */         // An unexpected element
      case A_Digits_Constraint:                   // 3.2.2: 3.5.9
      case A_Delta_Constraint:                    // 3.2.2: J.3
      case An_Index_Constraint:                   // 3.2.2: 3.6.1
      case A_Discriminant_Constraint:             // 3.2.2
      default:
        logWarn() << "Unhandled constraint: " << constraint.Constraint_Kind << std::endl;
        ROSE_ASSERT(!PRODUCTION_CODE);
        res = &mkAdaRangeConstraint(mkRangeExp());
    }

    return SG_DEREF(res);
  }


  SgTypeModifier
  handleModes(Mode_Kinds asisKind)
  {
    SgTypeModifier res;

    switch (asisKind)
    {
      case A_Default_In_Mode:
        res.setDefault();
        break;

      case An_In_Mode:
        res.setIntent_in();
        break;

      case An_Out_Mode:
        res.setIntent_out();
        break;

      case An_In_Out_Mode:
        res.setIntent_inout();
        break;

      case Not_A_Mode: /* break; */
      default:
        ROSE_ASSERT(false);
        break;
    }

    return res;
  }


  SgExpression*
  getVarInit(Declaration_Struct& decl, AstContext ctx)
  {
    if (decl.Declaration_Kind == A_Deferred_Constant_Declaration)
      return nullptr;

    ROSE_ASSERT(  decl.Declaration_Kind == A_Variable_Declaration
               || decl.Declaration_Kind == A_Constant_Declaration
               || decl.Declaration_Kind == A_Parameter_Specification
               || decl.Declaration_Kind == A_Real_Number_Declaration
               || decl.Declaration_Kind == An_Integer_Number_Declaration
               || decl.Declaration_Kind == A_Component_Declaration
               );

    //~ logWarn() << "decl.Initialization_Expression = " << decl.Initialization_Expression << std::endl;
    if (decl.Initialization_Expression == 0) return nullptr;

    return &getExprID_opt(decl.Initialization_Expression, ctx);
  }


  SgExpression*
  cloneNonNull(SgExpression* exp)
  {
    if (exp == nullptr) return nullptr;

    return si::deepCopy(exp);
  }

  typedef std::unique_ptr<SgExpression> GuardedExpression;

  SgInitializedNamePtrList
  constructInitializedNamePtrList( std::map<int, SgInitializedName*>& m,
                                   const NameCreator::result_container& names,
                                   SgType& dcltype,
                                   GuardedExpression initexpr = nullptr
                                 )
  {
    SgInitializedNamePtrList lst;
    int                      num      = names.size();

    for (int i = 0; i < num; ++i)
    {
      ROSE_ASSERT(names.at(i).fullName == names.at(i).ident);

      const std::string& name = names.at(i).fullName;
      Element_ID         id   = names.at(i).id();
      SgInitializedName& dcl  = mkInitializedName(name, dcltype, cloneNonNull(initexpr.get()));

      attachSourceLocation(dcl, retrieveAs<Element_Struct>(asisIdMap(), id));

      lst.push_back(&dcl);
      recordNonUniqueNode(m, id, dcl);
    }

    return lst;
  }

  SgVariableDeclaration&
  getParm(Element_Struct& elem, AstContext ctx)
  {
    typedef NameCreator::result_container name_container;

    ROSE_ASSERT(elem.Element_Kind == A_Declaration);

    Declaration_Struct&      asisDecl = elem.The_Union.Declaration;
    ROSE_ASSERT(asisDecl.Declaration_Kind == A_Parameter_Specification);

    // SgType&                   dcltype = tyModifier(getVarType(decl, ctx));
    ElemIdRange              range    = idRange(asisDecl.Names);
    name_container           names    = traverseIDs(range, asisIdMap(), NameCreator{ctx});
    SgType&                  parmtype = getDeclTypeID(asisDecl.Object_Declaration_View, ctx);
    GuardedExpression        initexpr{getVarInit(asisDecl, ctx)};
    SgInitializedNamePtrList dclnames = constructInitializedNamePtrList(asisVars(), names, parmtype, std::move(initexpr));
    SgVariableDeclaration&   sgnode   = mkParameter(dclnames, handleModes(asisDecl.Mode_Kind), ctx.scope());

    attachSourceLocation(sgnode, elem);
    /* unused fields:
         bool                           Has_Aliased;
         bool                           Has_Null_Exclusion;
    */
    return sgnode;
  }

  //
  // types

  typedef SgType& (*TypeModiferFn) (SgType&);

  SgType& tyIdentity(SgType& ty) { return ty; }

  SgType& tyConstify(SgType& ty) { return SG_DEREF(sb::buildConstType(&ty)); }


  //
  // helper function for combined handling of variables and constant declarations

  void
  handleNumVarCstDecl(Declaration_Struct& decl, AstContext ctx, bool isPrivate, SgType& dcltype, Element_Struct& elem)
  {
    typedef NameCreator::result_container name_container;

    ElemIdRange              range    = idRange(decl.Names);
    name_container           names    = traverseIDs(range, asisIdMap(), NameCreator{ctx});
    SgScopeStatement&        scope    = ctx.scope();
    GuardedExpression        initexp{getVarInit(decl, ctx)};
    SgInitializedNamePtrList dclnames = constructInitializedNamePtrList(asisVars(), names, dcltype, std::move(initexp));
    SgVariableDeclaration&   vardcl   = mkVarDecl(dclnames, scope);

    attachSourceLocation(vardcl, elem);
    privatize(vardcl, isPrivate);
    scope.append_statement(&vardcl);

    ROSE_ASSERT(vardcl.get_parent() == &scope);
  }

  void
  handleNumberDecl(Declaration_Struct& decl, AstContext ctx, bool isPrivate, SgType& numty, Element_Struct& elem)
  {
    SgType& cstty = SG_DEREF( sb::buildConstType(&numty) );

    handleNumVarCstDecl(decl, ctx, isPrivate, cstty, elem);
  }

  void
  handleVarCstDecl(Declaration_Struct& dcl, AstContext ctx, bool isPrivate, TypeModiferFn tyModifier, Element_Struct& elem)
  {
    handleNumVarCstDecl(dcl, ctx, isPrivate, tyModifier(getVarType(dcl, ctx)), elem);
  }

  struct MakeDeclaration : sg::DispatchHandler<SgDeclarationStatement*>
  {
    typedef sg::DispatchHandler<SgDeclarationStatement*> base;

    MakeDeclaration(const std::string& name, SgScopeStatement& scope, TypeFundamental basis)
    : base(), dclname(name), dclscope(scope), foundation(basis)
    {}

    void handle(SgNode& n)            { SG_UNEXPECTED_NODE(n); }
    void handle(SgType& n)            { res = &mkTypeDecl(dclname, n, dclscope); }

    void handle(SgClassDefinition& n)
    {
      SgClassDeclaration&    rec = mkRecordDecl(dclname, n, dclscope);
      SgDeclarationModifier& mod = rec.get_declarationModifier();

      if (foundation.hasAbstract) mod.setAdaAbstract();
      if (foundation.hasLimited)  mod.setAdaLimited();
      if (foundation.hasTagged)   mod.setAdaTagged();

      res = &rec;
    }

    std::string       dclname;
    SgScopeStatement& dclscope;
    TypeFundamental   foundation;
  };

  struct DeclareType
  {
      DeclareType(TypeFundamental what, AstContext astctx, bool privateItems)
      : foundation(what), scope(astctx.scope()), privateElems(privateItems)
      {}

      void operator()(const NameInfo& nameelem)
      {
        ROSE_ASSERT(nameelem.fullName == nameelem.ident);

        const std::string&      name = nameelem.fullName;
        Element_ID              id   = nameelem.id();
        SgDeclarationStatement* dcl = sg::dispatch(MakeDeclaration(name, scope, foundation), foundation.n);
        ROSE_ASSERT(dcl);

        privatize(*dcl, privateElems);
        scope.append_statement(dcl);
        recordNode(asisTypes(), id, *dcl);
        ROSE_ASSERT(dcl->get_parent() == &scope);
      }

    private:
      TypeFundamental   foundation;
      SgScopeStatement& scope;
      bool              privateElems;
    // AstContext        ctx;
  };


  struct DeclarePrivateType
  {
      DeclarePrivateType(SgType& tyrep, AstContext astctx, bool privateItems)
      : ty(tyrep), scope(astctx.scope()), privateElems(privateItems)
      {}

      void operator()(const NameInfo& nameelem)
      {
        ROSE_ASSERT(nameelem.fullName == nameelem.ident);

        const std::string&      name = nameelem.fullName;
        Element_ID              id   = nameelem.id();
        SgDeclarationStatement* dcl  = &mkTypeDecl(name, ty, scope);
        ROSE_ASSERT(dcl);

        privatize(*dcl, privateElems);
        scope.append_statement(dcl);
        recordNode(asisTypes(), id, *dcl);
        ROSE_ASSERT(dcl->get_parent() == &scope);
      }

    private:
      SgType&           ty;
      SgScopeStatement& scope;
      bool              privateElems;
  };


  struct ParameterCompletion
  {
      ParameterCompletion(ElemIdRange paramrange, AstContext astctx)
      : range(paramrange), ctx(astctx)
      {}

      void operator()(SgFunctionParameterList& lst, SgScopeStatement& parmscope)
      {
        traverseIDs(range, asisIdMap(), ParmlistCreator{lst, ctx.scope(parmscope)});
      }

    private:
      ElemIdRange range;
      AstContext  ctx;

      ParameterCompletion() = delete;
  };

/*
  struct DeclareTaskType
  {
    DeclareTaskType(SgAdaTaskSpec& taskspec, AstContext astctx, bool privateItems)
    : spec(taskspec), scope(astctx.scope()), privateElems(privateItems)
    {}

    void operator()(const NameInfo& nameelem)
    {
      const std::string& name = nameelem.first;
      Element_ID         id   = nameelem.second;
      SgAdaTaskTypeDecl& dcl  = mkAdaTaskTypeDecl(name, spec);

      privatize(dcl, privateElems);
      scope.append_statement(&dcl);
      recordNode(asisTypes(), id, dcl);
    }

    SgAdaTaskSpec&    spec;
    SgScopeStatement& scope;
    bool              privateElems;
  };
*/

  SgBasicBlock& getFunctionBody(SgFunctionDeclaration& defdcl)
  {
    SgFunctionDefinition& def = SG_ASSERT_TYPE(SgFunctionDefinition, SG_DEREF(defdcl.get_definition()));

    return SG_DEREF(def.get_body());
  }

  void setOverride(SgDeclarationModifier& sgmod, bool isOverride)
  {
    if (isOverride)
      sgmod.setOverride();
    else
      sgmod.unsetOverride();
  }

  typedef std::pair<SgTryStmt*, SgBasicBlock*> TryBlockNodes;

  TryBlockNodes
  createTryBlockIfNeeded(bool hasHandlers, SgBasicBlock& outer)
  {
    if (!hasHandlers) return TryBlockNodes(nullptr, &outer);

    SgBasicBlock& tryBlock = mkBasicBlock();
    SgTryStmt&    tryStmt  = mkTryStmt(tryBlock);

    //~ link_parent_child(outer, as<SgStatement>(tryStmt), SgBasicBlock::append_statement);
    outer.append_statement(&tryStmt);
    ROSE_ASSERT(tryStmt.get_parent() == &outer);

    return TryBlockNodes(&tryStmt, &tryBlock);
  }


  void handleDeclaration(Element_Struct& elem, AstContext ctx, bool isPrivate = false)
  {
    ROSE_ASSERT(elem.Element_Kind == A_Declaration);

    Declaration_Struct& decl = elem.The_Union.Declaration;

    switch (decl.Declaration_Kind)
    {
      case A_Package_Declaration:                    // 7.1(2)
        {
          SgScopeStatement&     outer   = ctx.scope();
          NameInfo              adaname = singleName(decl, ctx);
          //~ SgAdaPackageSpecDecl& sgnode  = mkAdaPackageSpecDecl(adaname.ident, outer);
          SgAdaPackageSpecDecl& sgnode  = mkAdaPackageSpecDecl(adaname.ident, SG_DEREF(adaname.parent));
          SgAdaPackageSpec&     pkgspec = SG_DEREF(sgnode.get_definition());

          ROSE_ASSERT(adaname.parent);
          sgnode.set_scope(adaname.parent);
          //~ sgnode.set_scope(&outer);
          logTrace() << "package decl " << adaname.ident
                     << " (" <<  adaname.fullName << ")"
                     << std::endl;

          recordNode(asisDecls(), elem.ID, sgnode);
          //~ recordNode(asisDecls(), adaname.id(), sgnode);

          privatize(sgnode, isPrivate);
          attachSourceLocation(sgnode, elem);
          outer.append_statement(&sgnode);
          ROSE_ASSERT(sgnode.get_parent() == &outer);

          ROSE_ASSERT(sgnode.search_for_symbol_from_symbol_table());

          // visible items
          {
            ElemIdRange range = idRange(decl.Visible_Part_Declarative_Items);

            traverseIDs(range, asisIdMap(), ElemCreator{ctx.scope(pkgspec)});
          }

          // private items
          {
            ElemIdRange range = idRange(decl.Private_Part_Declarative_Items);
            ROSE_ASSERT((!range.empty()) == decl.Is_Private_Present);

            traverseIDs(range, asisIdMap(), ElemCreator{ctx.scope(pkgspec), true /* private items */});
          }

          /* unused nodes:
                 Pragma_Element_ID_List         Pragmas;
                 Element_ID                     Corresponding_End_Name;
                 bool                           Is_Name_Repeated;
                 Declaration_ID                 Corresponding_Declaration;
           */
          break;
        }

      case A_Package_Body_Declaration:               // 7.2(2)
        {
          logTrace() << "package body decl" << std::endl;

          SgScopeStatement&     outer   = ctx.scope();
          Element_ID            specID  = decl.Corresponding_Declaration;
          SgAdaPackageSpecDecl& specdcl = lookupNodeAs<SgAdaPackageSpecDecl>(asisDecls(), specID);
          SgAdaPackageBodyDecl& sgnode  = mkAdaPackageBodyDecl(specdcl, outer);
          SgAdaPackageBody&     pkgbody = SG_DEREF(sgnode.get_definition());

          sgnode.set_scope(specdcl.get_scope());
          outer.append_statement(&sgnode);
          ROSE_ASSERT(sgnode.get_parent() == &outer);

          // declarative items
          {
            ElemIdRange range = idRange(decl.Body_Declarative_Items);

            traverseIDs(range, asisIdMap(), ElemCreator{ctx.scope(pkgbody)});
          }

          // statements
          {
            ElemIdRange range = idRange(decl.Body_Statements);

            traverseIDs(range, asisIdMap(), ElemCreator{ctx.scope(pkgbody)});
          }

          /*
           * unused nodes:
                 Pragma_Element_ID_List         Pragmas;
                 Element_ID                     Corresponding_End_Name;
                 Exception_Handler_List         Body_Exception_Handlers;
                 Declaration_ID                 Body_Block_Statement;
                 bool                           Is_Name_Repeated;
                 bool                           Is_Subunit;
                 Declaration_ID                 Corresponding_Body_Stub;
           */
          break;
        }

      case A_Function_Declaration:                   // 6.1(4)   -> Trait_Kinds
      case A_Procedure_Declaration:                  // 6.1(4)   -> Trait_Kinds
        {
          logTrace() << (decl.Declaration_Kind == A_Function_Declaration ? "function" : "procedure") << " decl"
                     << std::endl;

          const bool             isFunc  = decl.Declaration_Kind == A_Function_Declaration;
          SgScopeStatement&      outer   = ctx.scope();
          NameInfo               adaname = singleName(decl, ctx);
          ElemIdRange            range   = idRange(decl.Parameter_Profile);
          SgType&                rettype = isFunc ? getDeclTypeID(decl.Result_Profile, ctx)
                                                  : SG_DEREF(sb::buildVoidType());

          ROSE_ASSERT(adaname.fullName == adaname.ident);
          SgFunctionDeclaration& sgnode  = mkProcedure(adaname.fullName, outer, rettype, ParameterCompletion(range, ctx));

          setOverride(sgnode.get_declarationModifier(), decl.Is_Overriding_Declaration);
          recordNode(asisDecls(), elem.ID, sgnode);
          //~ recordNode(asisDecls(), adaname.id(), sgnode);

          privatize(sgnode, isPrivate);
          attachSourceLocation(sgnode, elem);
          outer.append_statement(&sgnode);
          ROSE_ASSERT(sgnode.get_parent() == &outer);

          /* unhandled fields

             bool                          Has_Abstract
             bool                          Is_Not_Overriding_Declaration
             bool                          Is_Dispatching_Operation
             Declaration_ID                Corresponding_Declaration
             Declaration_ID                Corresponding_Body
             Declaration_ID                Corresponding_Subprogram_Derivation
             Type_Definition_ID            Corresponding_Type

           +func:
             bool                          Is_Not_Null_Return
             Declaration_ID                Corresponding_Equality_Operator

             break;
          */

          break;
        }

      case A_Function_Body_Declaration:              // 6.3(2)
      case A_Procedure_Body_Declaration:             // 6.3(2)
        {
          logTrace() << (decl.Declaration_Kind == A_Function_Body_Declaration ? "function" : "procedure") << " body decl"
                     << std::endl;

          const bool             isFunc  = decl.Declaration_Kind == A_Function_Body_Declaration;
          SgScopeStatement&      outer   = ctx.scope();
          NameInfo               adaname = singleName(decl, ctx);
          ElemIdRange            params  = idRange(decl.Parameter_Profile);
          SgType&                rettype = isFunc ? getDeclTypeID(decl.Result_Profile, ctx)
                                                  : SG_DEREF(sb::buildVoidType());

          ROSE_ASSERT(adaname.fullName == adaname.ident);
          SgFunctionDeclaration& sgnode  = mkProcedureDef(adaname.fullName, outer, rettype, ParameterCompletion(params, ctx));
          SgBasicBlock&          declblk = getFunctionBody(sgnode);

          recordNode(asisDecls(), elem.ID, sgnode);
          //~ recordNode(asisDecls(), adaname.id(), sgnode);
          privatize(sgnode, isPrivate);
          attachSourceLocation(sgnode, elem);
          outer.append_statement(&sgnode);
          ROSE_ASSERT(sgnode.get_parent() == &outer);

          ElemIdRange            hndlrs  = idRange(decl.Body_Exception_Handlers);
          logWarn() << "block ex handlers: " << hndlrs.size() << std::endl;

          TryBlockNodes          trydata = createTryBlockIfNeeded(hndlrs.size() > 0, declblk);
          SgTryStmt*             trystmt = trydata.first;
          SgBasicBlock&          stmtblk = SG_DEREF(trydata.second);

          {
            ElemIdRange range = idRange(decl.Body_Declarative_Items);

            traverseIDs(range, asisIdMap(), ElemCreator{ctx.scope(declblk)});
          }

          {
            LabelManager lblmgr;
            ElemIdRange  range = idRange(decl.Body_Statements);

            traverseIDs(range, asisIdMap(), StmtCreator{ctx.scope(stmtblk).labels(lblmgr)});
          }

          if (trystmt)
          {
            traverseIDs(hndlrs, asisIdMap(), ExHandlerCreator{ctx.scope(declblk), SG_DEREF(trystmt)});
          }

          /* unhandled field
             Declaration_ID                 Body_Block_Statement;

           +func:
             bool                           Is_Not_Null_Return

             break;
          */
          break;
        }

      case A_Private_Type_Declaration:               // 3.2.1(2):7.3(2) -> Trait_Kinds
        {
          typedef NameCreator::result_container name_container;

          logTrace() << "Private Type "
                     << "\n  abstract: " << decl.Has_Abstract
                     << "\n  limited: " << decl.Has_Limited
                     << "\n  private: " << decl.Has_Private
                     << std::endl;
          ElemIdRange     range  = idRange(decl.Names);
          name_container  names  = traverseIDs(range, asisIdMap(), NameCreator{ctx});
          SgType&         opaque = mkDefaultType();

          ROSE_ASSERT(ctx.scope().get_parent());
          std::for_each(names.begin(), names.end(), DeclarePrivateType{opaque, ctx, isPrivate});

          /*
            bool                           Has_Abstract;
            bool                           Has_Limited;
            bool                           Has_Private;
            Definition_ID                  Discriminant_Part;
            Definition_ID                  Type_Declaration_View;
            Declaration_ID                 Corresponding_Type_Declaration;
            Declaration_ID                 Corresponding_Type_Completion;
            Declaration_ID                 Corresponding_Type_Partial_View;
            Declaration_ID                 Corresponding_First_Subtype;
            Declaration_ID                 Corresponding_Last_Constraint;
            Declaration_ID                 Corresponding_Last_Subtype;
          */

          break;
        }

      case An_Ordinary_Type_Declaration:            // 3.2.1(3)
        {
          typedef NameCreator::result_container name_container;

          logTrace() << "Ordinary Type "
                     << "\n  abstract: " << decl.Has_Abstract
                     << "\n  limited: " << decl.Has_Limited
                     << std::endl;
          ElemIdRange     range = idRange(decl.Names);
          name_container  names = traverseIDs(range, asisIdMap(), NameCreator{ctx});
          TypeFundamental ty    = getTypeFoundation(decl, ctx);

          ROSE_ASSERT(ctx.scope().get_parent());
          std::for_each(names.begin(), names.end(), DeclareType{ty, ctx, isPrivate});

          /* unused fields
              bool                           Has_Abstract;
              bool                           Has_Limited;
              Definition_ID                  Discriminant_Part;
              Declaration_ID                 Corresponding_Type_Declaration;
              Declaration_ID                 Corresponding_Type_Partial_View;
              Declaration_ID                 Corresponding_First_Subtype;
              Declaration_ID                 Corresponding_Last_Constraint;
              Declaration_ID                 Corresponding_Last_Subtype;
          */
          break;
        }


      //~ case A_Subtype_Declaration:                    // 3.2.2(2)
        //~ {

          //~ /* unused fields:
                //~ Declaration_ID                 Corresponding_First_Subtype;
                //~ Declaration_ID                 Corresponding_Last_Constraint;
                //~ Declaration_ID                 Corresponding_Last_Subtype;
          //~ */
          //~ break ;
        //~ }

      case A_Variable_Declaration:                   // 3.3.1(2) -> Trait_Kinds
        {
          handleVarCstDecl(decl, ctx, isPrivate, tyIdentity, elem);
          /* unused fields:
               bool                           Has_Aliased;
          */
          break;
        }

      case An_Integer_Number_Declaration:            // 3.3.2(2)
        {
          handleNumberDecl(decl, ctx, isPrivate, SG_DEREF(sb::buildIntType()), elem);

          /* unused fields:
          */
          break;
        }

      case A_Deferred_Constant_Declaration:          // 3.3.1(6):7.4(2) -> Trait_Kinds
      case A_Constant_Declaration:
        {
          handleVarCstDecl(decl, ctx, isPrivate, tyConstify, elem);
          /* unused fields:
               bool                           Has_Aliased;
          */
          break;
        }

      case A_Real_Number_Declaration:                // 3.5.6(2)
        {
          handleNumberDecl(decl, ctx, isPrivate, SG_DEREF(sb::buildFloatType()), elem);

          /* unused fields:
           */
          break;
        }

      case A_Loop_Parameter_Specification:           // 5.5(4)   -> Trait_Kinds
        {
          ROSE_ASSERT(!isPrivate);

          NameInfo               adaname = singleName(decl, ctx);
          ROSE_ASSERT(adaname.fullName == adaname.ident);

          SgType&                vartype = SG_DEREF( sb::buildIntType() ); // \todo
          SgInitializedName&     loopvar = mkInitializedName(adaname.fullName, vartype, nullptr);
          SgScopeStatement&      scope   = ctx.scope();

          recordNode(asisVars(), adaname.id(), loopvar);

          SgVariableDeclaration& sgnode  = mkVarDecl(loopvar, scope);

          attachSourceLocation(sgnode, elem);
          scope.append_statement(&sgnode);
          ROSE_ASSERT(sgnode.get_parent() == &scope);

          /* unused fields:
               bool                           Has_Reverse;
               Discrete_Subtype_Definition_ID Specification_Subtype_Definition;
           */
          break;
        }

      case A_Parameter_Specification:                // 6.1(15)  -> Trait_Kinds
        {
          // handled in getParm
          // break;
          ROSE_ASSERT(false);
        }

      case A_Task_Type_Declaration:                  // 9.1(2)
        {
          SgAdaTaskSpec&     spec    = getTaskSpec(decl, ctx);
          NameInfo           adaname = singleName(decl, ctx);
          ROSE_ASSERT(adaname.fullName == adaname.ident);
          SgAdaTaskTypeDecl& sgnode  = mkAdaTaskTypeDecl(adaname.fullName, spec, ctx.scope());

          attachSourceLocation(sgnode, elem);
          privatize(sgnode, isPrivate);
          ctx.scope().append_statement(&sgnode);
          ROSE_ASSERT(sgnode.get_parent() == &ctx.scope());
          recordNode(asisTypes(), adaname.id(), sgnode);
          recordNode(asisDecls(), adaname.id(), sgnode);

          /* unused fields:
               bool                           Has_Task;
               Element_ID                     Corresponding_End_Name;
               Definition_ID                  Discriminant_Part;
               Definition_ID                  Type_Declaration_View;
               Declaration_ID                 Corresponding_Type_Declaration;
               Declaration_ID                 Corresponding_Type_Partial_View;
               Declaration_ID                 Corresponding_First_Subtype;
               Declaration_ID                 Corresponding_Last_Constraint;
               Declaration_ID                 Corresponding_Last_Subtype;
               bool                           Is_Name_Repeated;
               Declaration_ID                 Corresponding_Declaration;
               Declaration_ID                 Corresponding_Body;
               Expression_List                Declaration_Interface_List;
           */
           break;
        }

      case A_Task_Body_Declaration:                  // 9.1(6)
        {
          SgAdaTaskBody&          tskbody = getTaskBody(decl, ctx);
          NameInfo                adaname = singleName(decl, ctx);
          Element_ID              declID  = decl.Corresponding_Declaration;
          SgDeclarationStatement* tskdecl = findNode(asisDecls(), declID);
          ROSE_ASSERT(adaname.fullName == adaname.ident);

          // \todo \review not sure why a task body could be independently created
          SgAdaTaskBodyDecl&      sgnode  = tskdecl ? mkAdaTaskBodyDecl(*tskdecl, tskbody, ctx.scope())
                                                    : mkAdaTaskBodyDecl(adaname.fullName, tskbody, ctx.scope());

          attachSourceLocation(sgnode, elem);
          privatize(sgnode, isPrivate);
          ctx.scope().append_statement(&sgnode);
          ROSE_ASSERT(sgnode.get_parent() == &ctx.scope());
          //~ recordNode(asisDecls(), elem.ID, sgnode);
          recordNode(asisDecls(), adaname.id(), sgnode);

          /* unused fields:
               bool                           Has_Task;
               Pragma_Element_ID_List         Pragmas;
               Element_ID                     Corresponding_End_Name;
               Exception_Handler_List         Body_Exception_Handlers;
               Declaration_ID                 Body_Block_Statement;
               bool                           Is_Name_Repeated;
               Declaration_ID                 Corresponding_Declaration;
               bool                           Is_Subunit;
               Declaration_ID                 Corresponding_Body_Stub;
          */
          break;
        }

      case An_Entry_Declaration:                     // 9.5.2(2)
        {
          NameInfo        adaname = singleName(decl, ctx);
          ElemIdRange     range   = idRange(decl.Parameter_Profile);

          ROSE_ASSERT(adaname.fullName == adaname.ident);
          SgAdaEntryDecl& sgnode  = mkAdaEntryDecl(adaname.fullName, ctx.scope(), ParameterCompletion(range, ctx));

          attachSourceLocation(sgnode, elem);
          privatize(sgnode, isPrivate);
          ctx.scope().append_statement(&sgnode);
          ROSE_ASSERT(sgnode.get_parent() == &ctx.scope());
          //~ recordNode(asisDecls(), elem.ID, sgnode);
          recordNode(asisDecls(), adaname.id(), sgnode);

          /* unused fields:
               bool                           Is_Overriding_Declaration;
               bool                           Is_Not_Overriding_Declaration;
               Declaration_ID                 Corresponding_Body;
               Discrete_Subtype_Definition_ID Entry_Family_Definition;
          */
          break;
        }

      case An_Exception_Declaration:                 // 11.1(2)
        {
          typedef NameCreator::result_container name_container;

          ElemIdRange              range    = idRange(decl.Names);
          name_container           names    = traverseIDs(range, asisIdMap(), NameCreator{ctx});
          SgScopeStatement&        scope    = ctx.scope();
          SgType&                  excty    = lookupNode(adaTypes(), std::string{"Exception"});
          SgInitializedNamePtrList dclnames = constructInitializedNamePtrList(asisExcps(), names, excty);
          SgVariableDeclaration&   sgnode   = mkExceptionDecl(dclnames, scope);

          attachSourceLocation(sgnode, elem);
          privatize(sgnode, isPrivate);
          scope.append_statement(&sgnode);
          ROSE_ASSERT(sgnode.get_parent() == &scope);
          break;
        }

      case A_Component_Declaration:                  // 3.8(6)
        {
          handleVarCstDecl(decl, ctx, isPrivate, tyIdentity, elem);
          /* unused clause:
                Pragma_Element_ID_List         Corresponding_Pragmas;
                Element_ID_List                Aspect_Specifications;
                Representation_Clause_List     Corresponding_Representation_Clauses;
                bool                           Has_Aliased;
          */
          break;
        }

/*
 * only for Ada 2012
 *
      case An_Expression_Function_Declaration:       // 6.8
        {
          / * unhandled fields - 2012 feature
          bool                           Is_Not_Null_Return;
          Parameter_Specification_List   Parameter_Profile;
          Element_ID                     Result_Profile;
          Expression_ID                  Result_Expression;
          bool                           Is_Overriding_Declaration;
          bool                           Is_Not_Overriding_Declaration;
          Declaration_ID                 Corresponding_Declaration;
          Type_Definition_ID             Corresponding_Type;
          bool                           Is_Dispatching_Operation;
          * /

          break ;
        }
*/

      case A_Package_Renaming_Declaration:           // 8.5.3(2)
        {
          NameInfo                adaname = singleName(decl, ctx);

          if (decl.Renamed_Entity < 0)
          {
            logWarn() << "skipping unknown renaming: " << adaname.ident << "/" << adaname.fullName
                      << ": " << elem.ID << " / " << decl.Renamed_Entity
                      << std::endl;
            return;
          }

          SgDeclarationStatement& aliased = getAliasedID(decl.Renamed_Entity, ctx);
          SgScopeStatement&       scope   = ctx.scope();
          SgAdaRenamingDecl&      sgnode  = mkAdaRenamingDecl(adaname.ident, aliased, scope);

          attachSourceLocation(sgnode, elem);
          privatize(sgnode, isPrivate);
          scope.append_statement(&sgnode);
          ROSE_ASSERT(sgnode.get_parent() == &scope);
          /* unhandled field
               Declaration_ID                 Corresponding_Declaration;
               Expression_ID                  Corresponding_Base_Entity;
          */
          break;
        }

      case A_Choice_Parameter_Specification:         // 11.2(4)
        {
          // handled in handleExceptionHandler
          ROSE_ASSERT(false);
          break;
        }

      case Not_A_Declaration: /* break; */           // An unexpected element
      case A_Protected_Type_Declaration:             // 9.4(2)
      case An_Incomplete_Type_Declaration:           // 3.2.1(2):3.10(2)
      case A_Tagged_Incomplete_Type_Declaration:     //  3.10.1(2)
      case A_Private_Extension_Declaration:          // 3.2.1(2):7.3(3) -> Trait_Kinds
      case A_Single_Task_Declaration:                // 3.3.1(2):9.1(3)
      case A_Single_Protected_Declaration:           // 3.3.1(2):9.4(2)
      case An_Enumeration_Literal_Specification:     // 3.5.1(3)
      case A_Discriminant_Specification:             // 3.7(5)   -> Trait_Kinds
      case A_Generalized_Iterator_Specification:     // 5.5.2    -> Trait_Kinds
      case An_Element_Iterator_Specification:        // 5.5.2    -> Trait_Kinds
      case A_Return_Variable_Specification:          // 6.5
      case A_Return_Constant_Specification:          // 6.5
      case A_Null_Procedure_Declaration:             // 6.7
      case An_Object_Renaming_Declaration:           // 8.5.1(2)
      case An_Exception_Renaming_Declaration:        // 8.5.2(2)
      case A_Procedure_Renaming_Declaration:         // 8.5.4(2)
      case A_Function_Renaming_Declaration:          // 8.5.4(2)
      case A_Generic_Package_Renaming_Declaration:   // 8.5.5(2)
      case A_Generic_Procedure_Renaming_Declaration: // 8.5.5(2)
      case A_Generic_Function_Renaming_Declaration:  // 8.5.5(2)
      case A_Protected_Body_Declaration:             // 9.4(7)
      case An_Entry_Body_Declaration:                // 9.5.2(5)
      case An_Entry_Index_Specification:             // 9.5.2(2)
      case A_Procedure_Body_Stub:                    // 10.1.3(3)
      case A_Function_Body_Stub:                     // 10.1.3(3)
      case A_Package_Body_Stub:                      // 10.1.3(4)
      case A_Task_Body_Stub:                         // 10.1.3(5)
      case A_Protected_Body_Stub:                    // 10.1.3(6)
      case A_Generic_Procedure_Declaration:          // 12.1(2)
      case A_Generic_Function_Declaration:           // 12.1(2)
      case A_Generic_Package_Declaration:            // 12.1(2)
      case A_Package_Instantiation:                  // 12.3(2)
      case A_Procedure_Instantiation:                // 12.3(2)
      case A_Function_Instantiation:                 // 12.3(2)
      case A_Formal_Object_Declaration:              // 12.4(2)  -> Mode_Kinds
      case A_Formal_Type_Declaration:                // 12.5(2)
      case A_Formal_Incomplete_Type_Declaration:
      case A_Formal_Procedure_Declaration:           // 12.6(2)
      case A_Formal_Function_Declaration:            // 12.6(2)
      case A_Formal_Package_Declaration:             // 12.7(2)
      case A_Formal_Package_Declaration_With_Box:    // 12.7(3)
      default:
        logWarn() << "unhandled declaration kind: " << decl.Declaration_Kind << std::endl;
        ROSE_ASSERT(!PRODUCTION_CODE);
    }
  }


  /// Functor to create an import statement (actually, a declaration)
  ///   for each element of a with clause; e.g., with Ada.Calendar, Ada.Clock;
  ///   yields two important statements, each of them is its own declaration
  ///   that can be referenced.
  struct WithClauseCreator
  {
      explicit
      WithClauseCreator(AstContext astctx)
      : ctx(astctx)
      {}

      void operator()(Element_Struct& el)
      {
        ROSE_ASSERT (el.Element_Kind == An_Expression);
        //~ else if (el.Element_Kind == A_Definition)
          //~ res = &getDefinitionExpr(el.The_Union.Definition, ctx);

        NameInfo                   imported = getName(el, ctx);
        SgExpression&              res = mkUnresolvedName(imported.fullName, ctx.scope());
        std::vector<SgExpression*> elems{&res};
        SgImportStatement&         sgnode = mkWithClause(elems);

        recordNode(asisDecls(), el.ID, sgnode);
        attachSourceLocation(sgnode, el);
        ctx.scope().append_statement(&sgnode);

        // an imported element may not be in the AST
        //   -> add the mapping if the element has not been seen.
        Element_Struct&            asisElem = SG_DEREF(imported.asisElem);
        Expression_Struct&         asisExpr = asisElem.The_Union.Expression;
        Element_ID                 impID    = asisExpr.Corresponding_Name_Declaration;

        recordNonUniqueNode(asisDecls(), impID, sgnode);
        ROSE_ASSERT(sgnode.get_parent() == &ctx.scope());
      }

    private:
      AstContext                 ctx;

      WithClauseCreator() = delete;
  };

  void handleClause(Element_Struct& elem, AstContext ctx)
  {
    Clause_Struct& clause = elem.The_Union.Clause;

    switch (clause.Clause_Kind)
    {
      case A_With_Clause:                // 10.1.2
        {
          ElemIdRange        range  = idRange(clause.Clause_Names);

          traverseIDs(range, asisIdMap(), WithClauseCreator{ctx});
          /* unused fields:
              bool   Has_Limited
          */
          break;
        }

      case Not_A_Clause: /* break; */    // An unexpected element
      case A_Use_Package_Clause:         // 8.4
      case A_Use_Type_Clause:            // 8.4
      case A_Use_All_Type_Clause:        // 8.4: Ada 2012
      case A_Representation_Clause:      // 13.1     -> Representation_Clause_Kinds
      case A_Component_Clause:           // 13.5.1
      default:
        logWarn() << "unhandled clause kind: " << clause.Clause_Kind << std::endl;
        ROSE_ASSERT(!PRODUCTION_CODE);
    }
  }

  void handleElement(Element_Struct& elem, AstContext ctx, bool isPrivate)
  {
    switch (elem.Element_Kind)
    {
        case A_Declaration:             // Asis.Declarations
        {
          handleDeclaration(elem, ctx, isPrivate);
          break;
        }

      case A_Clause:                  // Asis.Clauses
        {
          // currently only handles with clauses
          handleClause(elem, ctx);
          break;
        }

      case A_Defining_Name:           // Asis.Declarations
        {
          // handled by getName
          ROSE_ASSERT(false);
          break;
        }

      case A_Statement:               // Asis.Statements
        {
          // handled in StmtCreator
          ROSE_ASSERT(false);
          break;
        }

      case An_Expression:             // Asis.Expressions
        {
          // handled by getExpr
          ROSE_ASSERT(false);
          break;
        }

      case An_Exception_Handler:      // Asis.Statements
        {
          // handled by handleExceptionHandler
          ROSE_ASSERT(false);
          break;
        }

      case A_Definition:              // Asis.Definitions
        {
          // records (one of many definitions) are handled by getRecordBody
          Definition_Struct& def = elem.The_Union.Definition;

          logWarn() << "Unhandled element " << elem.Element_Kind
                    << "\n  definition kind: " << def.Definition_Kind
                    << std::endl;

          ROSE_ASSERT(false && !PRODUCTION_CODE);
          break;
        }

      case Not_An_Element: /* break; */ // Nil_Element
      case A_Path:                    // Asis.Statements
      case A_Pragma:                  // Asis.Elements
      case An_Association:            // Asis.Expressions
      default:
        logWarn() << "Unhandled element " << elem.Element_Kind << std::endl;
        ROSE_ASSERT(!PRODUCTION_CODE);
    }
  }

  /// labels a statement with a block label or a sequence of labels (if needed)
  /// @{
  SgStatement&
  labelIfNeeded(SgStatement& stmt, std::string lblname, Defining_Name_ID lblid, AstContext ctx)
  {
    ROSE_ASSERT(lblid > 0);

    SgNode&           parent  = SG_DEREF(stmt.get_parent());
    SgLabelStatement& sgn     = mkLabelStmt(lblname, stmt, ctx.scope());
    Element_Struct&   lblelem = retrieveAs<Element_Struct>(asisIdMap(), lblid);

    //~ copyFileInfo(stmt, sgn);
    attachSourceLocation(sgn, lblelem);
    sgn.set_parent(&parent);
    ctx.labels().label(lblid, sgn);

    ROSE_ASSERT(stmt.get_parent() == &sgn);
    return sgn;
  }

  std::string
  getLabelName(Element_ID el, AstContext ctx)
  {
    return getNameID(el, ctx).fullName;
  }

  template <class SageAdaStmt>
  SgStatement&
  labelIfNeeded(SageAdaStmt& stmt, Defining_Name_ID lblid, AstContext ctx)
  {
    ROSE_ASSERT(lblid >= 0);

    if (!lblid) return stmt;

    std::string lblname = getLabelName(lblid, ctx);

    stmt.set_string_label(lblname);
    return labelIfNeeded(stmt, lblname, lblid, ctx);
  }

  SgStatement&
  labelIfNeeded(SgStatement& stmt, Statement_Struct& adastmt, AstContext ctx)
  {
    typedef NameCreator::result_container name_container;

    ElemIdRange    range  = idRange(adastmt.Label_Names);
    name_container names  = traverseIDs(range, asisIdMap(), NameCreator{ctx});
    SgStatement*   sgnode = std::accumulate( names.rbegin(), names.rend(),
                                             &stmt,
                                             [&](SgStatement* labeled, const NameInfo& el) -> SgStatement*
                                             {
                                               ROSE_ASSERT(el.fullName == el.ident);
                                               return &labelIfNeeded(SG_DEREF(labeled), el.fullName, el.id(), ctx);
                                             }
                                           );

    return SG_DEREF(sgnode);
  }

  /// @}


  /// completes statements by setting source locations, parent node,
  /// adding labels (if needed)...
  /// @{
  void
  completeStmt(SgStatement& sgnode, Element_Struct& elem, AstContext ctx)
  {
    ROSE_ASSERT(elem.Element_Kind == A_Statement);

    attachSourceLocation(sgnode, elem);
    sgnode.set_parent(&ctx.scope());

    Statement_Struct& stmt = elem.The_Union.Statement;
    SgStatement&      sgn  = labelIfNeeded(sgnode, stmt, ctx);

    ctx.scope().append_statement(&sgn);
    ROSE_ASSERT(sgn.get_parent() == &ctx.scope());
  }

  template <class SageAdaStmt>
  void
  completeStmt(SageAdaStmt& sgnode, Element_Struct& elem, AstContext ctx, Defining_Name_ID lblid)
  {
    ROSE_ASSERT(elem.Element_Kind == A_Statement);

    attachSourceLocation(sgnode, elem);
    sgnode.set_parent(&ctx.scope());

    SgStatement&      sgn0 = labelIfNeeded(sgnode, lblid, ctx);
    Statement_Struct& stmt = elem.The_Union.Statement;
    SgStatement&      sgn  = labelIfNeeded(sgn0, stmt, ctx);

    ctx.scope().append_statement(&sgn);
    ROSE_ASSERT(sgn.get_parent() == &ctx.scope());
  }

  /// @}

  NameCreator::result_container
  queryDeclNames(Declaration_ID id, AstContext ctx)
  {
    if (id == 0) return NameCreator::result_container();

    Element_Struct& elem = retrieveAs<Element_Struct>(asisIdMap(), id);

    ROSE_ASSERT(elem.Element_Kind == A_Declaration);
    Declaration_Struct&      asisDecl = elem.The_Union.Declaration;

    ROSE_ASSERT((asisDecl.Declaration_Kind == A_Choice_Parameter_Specification));

    // SgType&                   dcltype = tyModifier(getVarType(decl, ctx));
    ElemIdRange              range    = idRange(asisDecl.Names);

    return traverseIDs(range, asisIdMap(), NameCreator{ctx});
  }


  void handleExceptionHandler(Element_Struct& elem, SgTryStmt& tryStmt, AstContext ctx)
  {
    typedef NameCreator::result_container name_container;

    ROSE_ASSERT(elem.Element_Kind == An_Exception_Handler);

    Exception_Handler_Struct& ex      = elem.The_Union.Exception_Handler;
    name_container            names   = queryDeclNames(ex.Choice_Parameter_Specification, ctx);

    if (names.size() == 0)
    {
      names.emplace_back(std::string{}, std::string{}, &ctx.scope(), &elem);
    }

    ROSE_ASSERT (names.size() == 1);
    ElemIdRange              tyRange = idRange(ex.Exception_Choices);
    SgType&                  extypes = traverseIDs(tyRange, asisIdMap(), HandlerTypeCreator{ctx});
    SgInitializedNamePtrList lst     = constructInitializedNamePtrList(asisVars(), names, extypes);
    SgBasicBlock&            body    = mkBasicBlock();

    ROSE_ASSERT(lst.size() == 1);
    SgCatchOptionStmt&       sgnode  = mkExceptionHandler(SG_DEREF(lst[0]), body);
    ElemIdRange              range   = idRange(ex.Handler_Statements);

    logWarn() << "catch handler" << std::endl;
    sg::linkParentChild(tryStmt, as<SgStatement>(sgnode), &SgTryStmt::append_catch_statement);
    sgnode.set_trystmt(&tryStmt);

    traverseIDs(range, asisIdMap(), StmtCreator{ctx.scope(body)});

    /* unused fields:
         Pragma_Element_ID_List Pragmas;
    */
  }

  void handleStmt(Element_Struct& elem, AstContext ctx)
  {
    logTrace() << "a statement (in progress) " << elem.Element_Kind << std::endl;

    // declarations are statements too
    if (elem.Element_Kind == A_Declaration)
    {
      handleDeclaration(elem, ctx);
      return;
    }

    ROSE_ASSERT(elem.Element_Kind == A_Statement);

    Statement_Struct& stmt = elem.The_Union.Statement;

    switch (stmt.Statement_Kind)
    {
      case A_Null_Statement:                    // 5.1
        {
          completeStmt(mkNullStmt(), elem, ctx);
          /* unused fields:
          */
          break;
        }

      case An_Assignment_Statement:             // 5.2
        {
          SgExpression& lhs    = getExprID(stmt.Assignment_Variable_Name, ctx);
          SgExpression& rhs    = getExprID(stmt.Assignment_Expression, ctx);
          SgExpression& assign = SG_DEREF(sb::buildAssignOp(&lhs, &rhs));
          SgStatement&  sgnode = SG_DEREF(sb::buildExprStatement(&assign));

          completeStmt(sgnode, elem, ctx);
          /* unused fields:
          */
          break;
        }

      case An_If_Statement:                     // 5.3
        {
          ElemIdRange  range  = idRange(stmt.Statement_Paths);
          SgStatement& sgnode = traverseIDs(range, asisIdMap(), IfStmtCreator{ctx});

          completeStmt(sgnode, elem, ctx);
          /* unused fields:
          */
          break;
        }

      case A_Case_Statement:                    // 5.4
        {
          SgExpression&      caseexpr = getExprID(stmt.Case_Expression, ctx);
          SgBasicBlock&      casebody = mkBasicBlock();
          SgSwitchStatement& sgnode   = mkAdaCaseStmt(caseexpr, casebody);
          ElemIdRange        range    = idRange(stmt.Statement_Paths);

          traverseIDs(range, asisIdMap(), CaseStmtCreator{ctx.scope(casebody), sgnode});
          completeStmt(sgnode, elem, ctx);
          /* unused fields:
          */
          break;
        }

      case A_While_Loop_Statement:              // 5.5
        {
          SgExpression& cond     = getExprID(stmt.While_Condition, ctx);
          SgBasicBlock& block    = mkBasicBlock();
          ElemIdRange   adaStmts = idRange(stmt.Loop_Statements);
          SgWhileStmt&  sgnode   = mkWhileStmt(cond, block);

          completeStmt(sgnode, elem, ctx, stmt.Statement_Identifier);

          recordNode(asisLoops(), elem.ID, sgnode);
          traverseIDs(adaStmts, asisIdMap(), StmtCreator{ctx.scope(block)});
          /* unused fields:
                Pragma_Element_ID_List    Pragmas;
                Element_ID                Corresponding_End_Name;
          */
          break;
        }

        case A_Loop_Statement:                    // 5.5
        {
          SgBasicBlock&  block    = mkBasicBlock();
          ElemIdRange    adaStmts = idRange(stmt.Loop_Statements);
          SgAdaLoopStmt& sgnode   = mkLoopStmt(block);

          completeStmt(sgnode, elem, ctx, stmt.Statement_Identifier);

          recordNode(asisLoops(), elem.ID, sgnode);
          traverseIDs(adaStmts, asisIdMap(), StmtCreator{ctx.scope(block)});

          /* unused fields:
                Pragma_Element_ID_List    Pragmas;
                Element_ID                Corresponding_End_Name;
                bool                      Is_Name_Repeated;
          */
          break;
        }

      case A_For_Loop_Statement:                // 5.5
        {
          SgBasicBlock&       block  = mkBasicBlock();
          SgForStatement&     sgnode = mkForStatement(block);
          Element_Struct&     forvar = retrieveAs<Element_Struct>(asisIdMap(), stmt.For_Loop_Parameter_Specification);
          SgForInitStatement& forini = SG_DEREF( sb::buildForInitStatement(sgnode.getStatementList()) );

          sg::linkParentChild(sgnode, forini, &SgForStatement::set_for_init_stmt);
          handleDeclaration(forvar, ctx.scope_npc(sgnode));
          completeStmt(sgnode, elem, ctx, stmt.Statement_Identifier);
          ROSE_ASSERT(sgnode.getStatementList().size() <= 1 /* \todo should be 0 */);

          // \todo this swap is needed, otherwise the variable declaration ends
          //       up in the body instead of the initializer.. ???
          std::swap(forini.get_init_stmt(), block.get_statements());

          ElemIdRange         loopStmts = idRange(stmt.Loop_Statements);

          recordNode(asisLoops(), elem.ID, sgnode);
          traverseIDs(loopStmts, asisIdMap(), StmtCreator{ctx.scope(block)});

          /* unused fields:
               Pragma_Element_ID_List Pragmas;
               Element_ID             Corresponding_End_Name;
          */
          break;
        }

      case A_Block_Statement:                   // 5.6
        {
          SgBasicBlock& sgnode   = mkBasicBlock();
          ElemIdRange   blkDecls = idRange(stmt.Block_Declarative_Items);
          ElemIdRange   blkStmts = idRange(stmt.Block_Statements);
          ElemIdRange   exHndlrs = idRange(stmt.Block_Exception_Handlers);
          logWarn() << "block ex handlers: " << exHndlrs.size() << std::endl;

          TryBlockNodes trydata  = createTryBlockIfNeeded(exHndlrs.size() > 0, sgnode);
          SgTryStmt*    tryblk   = trydata.first;
          SgBasicBlock& block    = SG_DEREF(trydata.second);

          completeStmt(block, elem, ctx, stmt.Statement_Identifier);
          traverseIDs(blkDecls, asisIdMap(), StmtCreator{ctx.scope(sgnode)});
          traverseIDs(blkStmts, asisIdMap(), StmtCreator{ctx.scope(block)});

          if (tryblk)
          {
            traverseIDs(exHndlrs, asisIdMap(), ExHandlerCreator{ctx.scope(sgnode), SG_DEREF(tryblk)});
          }

          /* unused fields:
                Pragma_Element_ID_List    Pragmas;
                Element_ID                Corresponding_End_Name;
                bool                      Is_Name_Repeated;
                bool                      Is_Declare_Block;
          */
          break;
        }

      case An_Exit_Statement:                   // 5.7
        {
          SgStatement&  exitedLoop    = lookupNode(asisLoops(), stmt.Corresponding_Loop_Exited);
          SgExpression& exitCondition = getExprID_opt(stmt.Exit_Condition, ctx);
          const bool    loopIsNamed   = stmt.Exit_Loop_Name > 0;
          SgStatement&  sgnode        = mkAdaExitStmt(exitedLoop, exitCondition, loopIsNamed);

          completeStmt(sgnode, elem, ctx);
          /* unused fields:
               Expression_ID          Exit_Loop_Name;
          */
          break;
        }

      case A_Goto_Statement:                    // 5.8
        {
          SgGotoStatement& sgnode = SG_DEREF( sb::buildGotoStatement() );

          ctx.labels().gotojmp(stmt.Goto_Label, sgnode);

          completeStmt(sgnode, elem, ctx);
          /* unused fields:
                Statement_ID           Corresponding_Destination_Statement;
          */
          break;
        }

      case A_Procedure_Call_Statement:          // 6.4
        {
          SgDeclarationStatement& tgt    = lookupNode(asisDecls(), stmt.Corresponding_Called_Entity);
          SgFunctionDeclaration*  fundcl = isSgFunctionDeclaration(&tgt);
          SgFunctionRefExp&       funref = SG_DEREF(sb::buildFunctionRefExp(fundcl));
          ElemIdRange             range  = idRange(stmt.Call_Statement_Parameters);
          SgExprListExp&          arglst = traverseIDs(range, asisIdMap(), ArgListCreator{ctx});
          SgStatement&            sgnode = SG_DEREF(sb::buildFunctionCallStmt(&funref, &arglst));

          completeStmt(sgnode, elem, ctx);
          /* unused fields:
              bool        Is_Prefix_Notation
              bool        Is_Dispatching_Call
              bool        Is_Call_On_Dispatching_Operation
              Element_ID  Called_Name
              Declaration Corresponding_Called_Entity_Unwound
          */
          break;
        }

      case A_Return_Statement:                  // 6.5
        {
          Element_Struct* exprel = retrieveAsOpt<Element_Struct>(asisIdMap(), stmt.Return_Expression);
          SgExpression*   retval = exprel ? &getExpr(*exprel, ctx) : NULL;
          SgStatement&    sgnode = SG_DEREF( sb::buildReturnStmt(retval) );

          completeStmt(sgnode, elem, ctx);
          /* unused fields:
          */
          break;
        }

      case An_Accept_Statement:                 // 9.5.2
        {
          SgExpression&           entryref = getExprID(stmt.Accept_Entry_Direct_Name, ctx);
          SgExpression&           idx      = getExprID_opt(stmt.Accept_Entry_Index, ctx);
          SgAdaAcceptStmt&        sgnode   = mkAdaAcceptStmt(entryref, idx);

          completeStmt(sgnode, elem, ctx);

          ElemIdRange             params   = idRange(stmt.Accept_Parameters);
          AstContext              parmctx  = ctx.scope(SG_DEREF(sgnode.get_parameterScope()));

          traverseIDs(params, asisIdMap(), ParmlistCreator{sgnode, parmctx});

          ElemIdRange             stmts   = idRange(stmt.Accept_Body_Statements);

          if (stmts.empty())
          {
            SgStatement&          noblock = mkNullStmt();

            sg::linkParentChild(sgnode, noblock, &SgAdaAcceptStmt::set_body);
          }
          else
          {
            SgBasicBlock&         block   = mkBasicBlock();

            sg::linkParentChild(sgnode, as<SgStatement>(block), &SgAdaAcceptStmt::set_body);
            traverseIDs(stmts, asisIdMap(), StmtCreator{parmctx.scope(block)});
          }

          /* unused fields:
              Pragma_Element_ID_List       Pragmas;
              Element_ID                   Corresponding_End_Name;
              bool                         Is_Name_Repeated;
              Statement_List               Accept_Body_Exception_Handlers;
              Declaration_ID               Corresponding_Entry;
          */
          break;
        }

      case A_Raise_Statement:                   // 11.3
        {
          SgExpression&   raised = getExprID(stmt.Raised_Exception, ctx);
          SgStatement&    sgnode = mkRaiseStmt(raised);

          completeStmt(sgnode, elem, ctx);
          /* unused fields:
               Expression_ID          Associated_Message;
          */
          break;
        }


      //|A2005 start
      case An_Extended_Return_Statement:        // 6.5
      //|A2005 end

      case Not_A_Statement: /* break; */        // An unexpected element
      case An_Entry_Call_Statement:             // 9.5.3
      case A_Requeue_Statement:                 // 9.5.4
      case A_Requeue_Statement_With_Abort:      // 9.5.4
      case A_Delay_Until_Statement:             // 9.6
      case A_Delay_Relative_Statement:          // 9.6
      case A_Terminate_Alternative_Statement:   // 9.7.1
      case A_Selective_Accept_Statement:        // 9.7.1
      case A_Timed_Entry_Call_Statement:        // 9.7.2
      case A_Conditional_Entry_Call_Statement:  // 9.7.3
      case An_Asynchronous_Select_Statement:    // 9.7.4
      case An_Abort_Statement:                  // 9.8
      case A_Code_Statement:                    // 13.8 assembly
      default:
        logWarn() << "Unhandled statement " << stmt.Statement_Kind << std::endl;
        ROSE_ASSERT(!PRODUCTION_CODE);
    }
  }

  NameInfo
  getExprName(Element_Struct& elem, AstContext ctx)
  {
    ROSE_ASSERT(elem.Element_Kind == An_Expression);

    Expression_Struct& idex  = elem.The_Union.Expression;

    if (idex.Expression_Kind == An_Identifier)
    {
      std::string        ident{idex.Name_Image};

      return NameInfo{ ident, ident, &ctx.scope(), &elem };
    }

    ROSE_ASSERT(idex.Expression_Kind == A_Selected_Component);

    NameInfo compound  = getNameID(idex.Prefix, ctx);
    NameInfo component = getNameID(idex.Selector, ctx);

    return NameInfo{ component.ident,
                     compound.fullName + "." + component.fullName,
                     &ctx.scope(),
                     component.asisElem
                   };
  }

  NameInfo
  getName(Element_Struct& elem, AstContext ctx)
  {
    if (elem.Element_Kind == An_Expression)
      return getExprName(elem, ctx);

    if (elem.Element_Kind != A_Defining_Name)
      logWarn() << "unexpected elem kind (getName)" << elem.Element_Kind << std::endl;

    ROSE_ASSERT(elem.Element_Kind == A_Defining_Name);

    Defining_Name_Struct& asisname = elem.The_Union.Defining_Name;
    SgScopeStatement*     parent = &ctx.scope();
    std::string           ident{asisname.Defining_Name_Image};
    std::string           name{asisname.Defining_Name_Image};

    switch (asisname.Defining_Name_Kind)
    {
      case A_Defining_Expanded_Name:
        {
          NameInfo        defname    = getNameID(asisname.Defining_Selector, ctx);
          Element_Struct& prefixelem = retrieveAs<Element_Struct>(asisIdMap(), asisname.Defining_Prefix);

          ident  = defname.ident;
          parent = &getScope(prefixelem, ctx);
          break;
        }

      case A_Defining_Identifier:
        // nothing to do, the fields are already set
        break;

      case Not_A_Defining_Name:
        /* break; */

      case A_Defining_Character_Literal:   // 3.5.1(4)
      case A_Defining_Enumeration_Literal: // 3.5.1(3)
      case A_Defining_Operator_Symbol:     // 6.1(9)
      default:
        logWarn() << "unknown name kind " << asisname.Defining_Name_Kind
                  << " (" << name << ")"
                  << std::endl;
        ROSE_ASSERT(!PRODUCTION_CODE);
    }

    return NameInfo{ ident, name, parent, &elem };
  }

  NameInfo
  getNameID(Element_ID el, AstContext ctx)
  {
    return getName(retrieveAs<Element_Struct>(asisIdMap(), el), ctx);
  }


  void handleUnit(Unit_Struct& adaUnit, AstContext ctx);

  // ADA

  struct UnitCreator
  {
      explicit
      UnitCreator(AstContext astctx)
      : ctx(astctx)
      {}

      void operator()(Unit_Struct& adaUnit)
      {
        handleUnit(adaUnit, ctx);
      }

      void operator()(Unit_Struct_List_Struct& adaUnit)
      {
        (*this)(adaUnit.Unit);
      }

      AstContext ctx;
  };

  struct PrnUnitHeader
  {
    explicit
    PrnUnitHeader(Unit_Struct& adaUnit)
    : unit(adaUnit)
    {}

    Unit_Struct& unit;
  };

  std::ostream& operator<<(std::ostream& os, PrnUnitHeader obj)
  {
    os << "\n " << obj.unit.ID                  << " (ID)"
       << "\n " << obj.unit.Unit_Full_Name      << " (Unit_Full_Name)"
       << "\n " << obj.unit.Unique_Name         << " (Unique_Name)"
       << "\n " << obj.unit.Text_Name           << " (Text_Name)"
       << "\n " << obj.unit.Object_Name         << " (Object_Name)"
       << "\n " << obj.unit.Exists              << " (Exists)"
       << "\n " << obj.unit.Can_Be_Main_Program << " (Can_Be_Main_Program)"
       << "\n " << obj.unit.Is_Body_Required    << " (Is_Body_Required)"
       << "\n " << obj.unit.Unit_Declaration    << " (Unit_Declaration)"
       ;

    return os;
  }

  void handleUnit(Unit_Struct& adaUnit, AstContext ctx)
  {
    // dispatch based on unit kind
    switch (adaUnit.Unit_Kind)
    {
      case A_Function:
      case A_Procedure:
        {
          logTrace() << "A " << (adaUnit.Unit_Kind == A_Function ? "function" : "procedure")
                     << PrnUnitHeader(adaUnit)
                     << "\n " << adaUnit.Corresponding_Parent_Declaration << " (Corresponding_Parent_Declaration)"
                     << "\n " << adaUnit.Corresponding_Body << " (Corresponding_Body)"
                     << std::endl;

          ElemIdRange range = idRange(adaUnit.Context_Clause_Elements);

          traverseIDs(range, asisIdMap(), ElemCreator{ctx});
          handleElementID(adaUnit.Unit_Declaration, ctx);

          /* unused optional elems:
               Unit_ID             Corresponding_Parent_Declaration
               Unit_ID             Corresponding_Declaration;
               Unit_ID             Corresponding_Body;
               Unit_List           Subunits;
          */
          /* break; */
          break;
        }

      case A_Function_Body:
      case A_Procedure_Body:
        {
          logTrace() << "A " << (adaUnit.Unit_Kind == A_Function_Body ? "function" : "procedure") << " body"
                     << PrnUnitHeader(adaUnit)
                     << std::endl;
          ElemIdRange range = idRange(adaUnit.Context_Clause_Elements);

          traverseIDs(range, asisIdMap(), ElemCreator{ctx});
          handleElementID(adaUnit.Unit_Declaration, ctx);

          /* unused optional elems:
               Unit_ID             Corresponding_Parent_Declaration;
               Unit_ID             Corresponding_Declaration;
               Unit_List           Subunits;
          */

          /* break; */
          break;
        }

      case A_Package:
        {
          logTrace() << "A package"
                     << PrnUnitHeader(adaUnit)
                     << std::endl;

          ElemIdRange           elemRange = idRange(adaUnit.Context_Clause_Elements);
          UnitIdRange           unitRange = idRange(adaUnit.Corresponding_Children);

          if (elemRange.size() || unitRange.size())
          {
            logWarn() << "   elems# " << elemRange.size()
                      << "\n    subs# " << unitRange.size()
                      << std::endl;

            traverseIDs(elemRange, asisIdMap(), ElemCreator{ctx});
            //~ traverseIDs(unitRange, asisIdMap(), UnitCreator(ctx));
          }

          //~ handleElementID(adaUnit.Unit_Declaration, ctx.scope(pkgspec));
          handleElementID(adaUnit.Unit_Declaration, ctx);

          /* unused fields:
          */
          break;
        }

      case A_Package_Body:
        {
          logTrace() << "A package body"
                     << PrnUnitHeader(adaUnit)
                     << std::endl;

          ElemIdRange           elemRange = idRange(adaUnit.Context_Clause_Elements);
          UnitIdRange           unitRange = idRange(adaUnit.Corresponding_Children);

          if (elemRange.size() || unitRange.size())
          {
            logWarn() << "   elems# " << elemRange.size()
                      << "\n    subs# " << unitRange.size()
                      << std::endl;

            traverseIDs(elemRange, asisIdMap(), ElemCreator{ctx});
            //~ traverseIDs(unitRange, asisIdMap(), UnitCreator(ctx));
          }

          handleElementID(adaUnit.Unit_Declaration, ctx);
          break;
        }

      case Not_A_Unit: /* break; */
      case A_Package_Instance:
      case A_Generic_Package:

      case A_Generic_Procedure:
      case A_Generic_Function:

      case A_Procedure_Instance:
      case A_Function_Instance:

      case A_Procedure_Renaming:
      case A_Function_Renaming:
      case A_Package_Renaming:

      case A_Generic_Procedure_Renaming:
      case A_Generic_Function_Renaming:
      case A_Generic_Package_Renaming:

      //  A unit interpreted only as the completion of a function: or a unit
      //  interpreted as both the declaration and body of a library
      //  function. Reference Manual 10.1.4(4)

      case A_Procedure_Body_Subunit:
      case A_Function_Body_Subunit:
      case A_Package_Body_Subunit:
      case A_Task_Body_Subunit:
      case A_Protected_Body_Subunit:

      case A_Nonexistent_Declaration:
      //  A unit that does not exist but is:
      //    1) mentioned in a with clause of another unit or:
      //    2) a required corresponding library_unit_declaration
      case A_Nonexistent_Body:
      //  A unit that does not exist but is:
      //     1) known to be a corresponding subunit or:
      //     2) a required corresponding library_unit_body
      case A_Configuration_Compilation:
      //  Corresponds to the whole content of a compilation with no
      //  compilation_unit, but possibly containing comments, configuration
      //  pragmas, or both. Any Context can have at most one unit of
      //  case A_Configuration_Compilation kind. A unit of
      //  case A_Configuration_Compilation does not have a name. This unit
      //  represents configuration pragmas that are "in effect".
      //
      //  GNAT-specific note: In case of GNAT the requirement to have at most
      //  one unit of case A_Configuration_Compilation kind does not make sense: in
      //  GNAT compilation model configuration pragmas are contained in
      //  configuration files, and a compilation may use an arbitrary number
      //  of configuration files. That is, (Elements representing) different
      //  configuration pragmas may have different enclosing compilation units
      //  with different text names. So in the ASIS implementation for GNAT a
      //  Context may contain any number of units of
      //  case A_Configuration_Compilation kind
      case An_Unknown_Unit:

      default:
        logWarn() << "unit kind unhandled: " << adaUnit.Unit_Kind << std::endl;
        ROSE_ASSERT(!PRODUCTION_CODE);
    }
  }

  void initializeAdaTypes(SgGlobal& global)
  {
    SgAdaPackageSpec& hiddenScope = mkBareNode<SgAdaPackageSpec>();

    hiddenScope.set_parent(&global);

    adaTypes()[std::string{"Integer"}]   = sb::buildIntType();
    adaTypes()[std::string{"Character"}] = sb::buildCharType();

    // \todo items
    adaTypes()[std::string{"Float"}]     = sb::buildFloatType();  // Float is a subtype of Real
    adaTypes()[std::string{"Positive"}]  = sb::buildIntType();    // Positive is a subtype of int
    adaTypes()[std::string{"Natural"}]   = sb::buildIntType();    // Natural is a subtype of int
    adaTypes()[std::string{"Boolean"}]   = sb::buildBoolType();   // Boolean is an enumeration of True and False

    adaTypes()[std::string{"Exception"}] = sb::buildOpaqueType("Exception", &hiddenScope);
  }
}

void secondConversion(Nodes_Struct& headNodes, SgSourceFile* file)
{
  ROSE_ASSERT(file);

  logInit();
  logInfo() << "Building ROSE AST .." << std::endl;

  Unit_Struct_List_Struct* adaLimit = 0;
  Unit_Struct_List_Struct* adaUnit  = headNodes.Units;
  SgGlobal&                astScope = SG_DEREF(file->get_globalScope());

  initializeAdaTypes(astScope);
  traverse(adaUnit, adaLimit, UnitCreator{AstContext{astScope}});
  clearMappings();

  logTrace() << "Generating DOT file: " << "adaTypedAst.dot" << std::endl;
  generateDOT(&astScope, "adaTypedAst");

  file->set_processedToIncludeCppDirectivesAndComments(false);
  logInfo() << "Building ROSE AST done" << std::endl;
}



}
