// vim:et sta sw=4 ts=4:
#include <rose.h>

// DQ (2/9/2010): Testing use of ROE to compile ROSE.
#ifndef USE_ROSE

#include <string>
#include <algorithm>

#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

#include "sageGeneric.hpp"

#include "RtedSymbols.h"
#include "DataStructures.h"
#include "RtedTransformation.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;

// This really belongs in the non-existent DataStructures.cpp
const std::string RtedForStmtProcessed::Key( "Rted::ForStmtProcessed" );


/// \brief describes the number of indirections
struct TypeStructureInfo
{
  SgType*     type;
  AddressDesc desc;

  TypeStructureInfo()
  : type(NULL), desc(rted_obj())
  {}
};



/**
 * Helper Functions
 *
 *
 * This function returns the statement that
 * surrounds a given Node or Expression
 ****************************************/

bool is_main_func(const SgFunctionDefinition& fndef)
{
  const SgFunctionDeclaration* fndecl = fndef.get_declaration();
  ROSE_ASSERT(fndecl);

  const std::string fnname = fndecl->get_name().str();

  return (  isSgGlobal(fndef.get_scope())
         && (  fnname == "main"
            || fnname == "upc_main"
            )
         );
}

SgUpcBarrierStatement* buildUpcBarrierStatement()
{
  SgExpression*          exp = NULL;
  SgUpcBarrierStatement* barrier_stmt = new SgUpcBarrierStatement(exp);

  setOneSourcePositionForTransformation(barrier_stmt);
  return barrier_stmt;
}

typedef SgUpcLocalsizeofExpression SgUpcLocalsizeofOp;

#if NOTUSED

static
SgUpcLocalsizeofOp* buildUpcLocalsizeofOp(SgExpression* exp)
{
  ROSE_ASSERT(exp);

  SgUpcLocalsizeofOp* lsizeof = new SgUpcLocalsizeofOp(exp, NULL, NULL);

  exp->set_parent(lsizeof);
  setOneSourcePositionForTransformation(lsizeof);

  return lsizeof;
}

#endif /* NOTUSED */

SgStatement* getSurroundingStatement(SgExpression& n)
{
  return &sg::ancestor<SgStatement>(n);
}

SgStatement* getSurroundingStatement(SgInitializedName& n)
{
  return &sg::ancestor<SgStatement>(n);
}

SgAggregateInitializer* genAggregateInitializer(SgExprListExp* initexpr, SgType* type)
{
  SgAggregateInitializer* res = buildAggregateInitializer(initexpr, type);

  // not sure whether the explicit braces flag needs to be set
  res->set_need_explicit_braces(true);

  return res;
}


SgExprStatement* insertCheck(InsertLoc iloc, SgStatement* stmt, SgFunctionSymbol* checker, SgExprListExp* args)
{
  ROSE_ASSERT(stmt && checker && args);

  SgFunctionRefExp*  funRef = buildFunctionRefExp(checker);
  SgFunctionCallExp* funcCallExp = buildFunctionCallExp(funRef, args);
  SgExprStatement*   exprStmt = buildExprStatement(funcCallExp);

  SageInterface::insertStatement(stmt, exprStmt, iloc == ilBefore);

  return exprStmt;
}

SgExprStatement* insertCheck(InsertLoc iloc, SgStatement* stmt, SgFunctionSymbol* checker, SgExprListExp* args, const std::string& comment)
{
  SgExprStatement* exprStmt = insertCheck(iloc, stmt, checker, args);

  attachComment(exprStmt, "", PreprocessingInfo::before);
  attachComment(exprStmt, comment, PreprocessingInfo::before);

  return exprStmt;
}

SgExprStatement*
insertCheckOnStmtLevel(InsertLoc iloc, SgExpression* checked_node, SgFunctionSymbol* checker, SgExprListExp* args)
{
  ROSE_ASSERT(checked_node);

  return insertCheck(iloc, getSurroundingStatement(*checked_node), checker, args);
}

bool isFunctionParameter(const SgInitializedName& n)
{
   return isSgFunctionDeclaration(n.get_parent() -> get_parent());
}

bool isStructMember(const SgInitializedName& n)
{
   return isSgClassDefinition(n.get_parent() -> get_parent());
}


/**
 * Follow the base type of @c type until we reach a non-typedef.
 */
SgType* skip_Typedefs( SgType* type ) {
     if( isSgTypedefType( type ))
        return skip_Typedefs(
            isSgTypedefType( type ) -> get_base_type() );

     return type;
}

SgType* skip_ReferencesAndTypedefs( SgType* type ) {
     if( isSgTypedefType( type )) {
        return skip_ReferencesAndTypedefs(
            isSgTypedefType( type ) -> get_base_type() );
    } else if( isSgReferenceType( type )) {
        // resolve reference to reference ... to pointer
        return skip_ReferencesAndTypedefs(
            isSgReferenceType( type ) -> get_base_type() );
    }

    return type;
}


SgBasicBlock& requiresParentIsBasicBlock(SgStatement& stmt)
{
  SgLocatedNode* block = ensureBasicBlockAsParent(&stmt);
  SgBasicBlock*  res = isSgBasicBlock(block);

  ROSE_ASSERT(res);
  return *res;
}


//
//

static
SgCastExp* cstyle_ctor(SgType* type, SgAggregateInitializer* exp)
{
  ROSE_ASSERT(type && exp);

  return buildCastExp(exp, type, SgCastExp::e_C_style_cast);
}

static
SgCastExp* cstyle_ctor(SgType* type, SgAggregateInitializer* exp, size_t args)
{
  ROSE_ASSERT(exp->get_initializers()->get_expressions().size() == args);

  return cstyle_ctor(type, exp);
}

SgCastExp*
RtedTransformation::ctorTypeDesc(SgAggregateInitializer* exp) const
{
  return cstyle_ctor(roseTypeDesc(), exp, 3);
}

SgCastExp*
RtedTransformation::ctorSourceInfo(SgAggregateInitializer* exp) const
{
  return cstyle_ctor(roseFileInfo(), exp, 3);
}

SgCastExp*
RtedTransformation::ctorAddressDesc(SgAggregateInitializer* exp) const
{
  return cstyle_ctor(roseAddressDesc(), exp, 2);
}

SgCastExp*
RtedTransformation::ctorTypeDescList(SgAggregateInitializer* exp) const
{
  return cstyle_ctor(buildArrayType(roseTypeDesc(), NULL), exp);
}

SgCastExp*
RtedTransformation::ctorDimensionList(SgAggregateInitializer* exp) const
{
  return cstyle_ctor(buildArrayType(roseDimensionType(), NULL), exp);
}


SgAggregateInitializer*
RtedTransformation::mkAddressDesc(AddressDesc desc) const
{
  SgExprListExp* initexpr = buildExprListExp();

  appendExpression(initexpr, buildIntVal(desc.levels));
  appendExpression(initexpr, buildIntVal(desc.shared_mask));

  return genAggregateInitializer(initexpr, roseAddressDesc());
}

enum UpcSharedKind
{
  usUnknown = 0,
  usNonshared = 1,
  usShared = 2,
  usDistributed = 3
};

static
std::pair<UpcSharedKind, long>
upcSharedFlag(const SgModifierType& n)
{
  std::pair<UpcSharedKind, long> res(usNonshared, 0);
  const SgTypeModifier&          modifier = n.get_typeModifier();
  const SgUPC_AccessModifier&    upcModifier = modifier.get_upcModifier();

  if (upcModifier.get_isShared())
  {
    res.second = upcModifier.get_layout();
    res.first = (res.second != 0 ? usDistributed : usShared);
  }

  return res;
}

struct SharedQueryResult
{
  std::pair<UpcSharedKind, long> shared;
  const SgType*                  base;

  SharedQueryResult()
  : shared(usUnknown, 0), base(NULL)
  {}

  UpcSharedKind kind()   const { return shared.first; }
  long          layout() const { return shared.second; }
};

struct SharedQueryHandler
{
  SharedQueryResult   res;

  SharedQueryHandler()
  : res()
  {}

  void handle(const SgNode&) { assert(false); }
  void handle(const SgType&) { }

  void handle(const SgModifierType& n)
  {
    res.shared = upcSharedFlag(n);
    res.base = n.get_base_type();
  }

  void handle(const SgArrayType& n)
  {
    res.base = n.get_base_type();
  }

  void handle(const SgTypedefType& n)
  {
    res.base = n.get_base_type();
  }

  operator SharedQueryResult() { return res; }
};

static
std::pair<UpcSharedKind, long>
sharedTest(const SgType* n)
{
  SharedQueryResult res = sg::dispatch(SharedQueryHandler(), n);

  while (res.kind() <= usNonshared && res.base)
  {
    res = sg::dispatch(SharedQueryHandler(), res.base);
  }

  return res.shared;
}

bool isUpcShared(const SgType* n)
{
  return sharedTest(n).first >= usShared;
}

long upcBlocksize(const SgType* n)
{
  return sharedTest(n).second;
}


static
bool isStaticVariable(const SgInitializedName& n)
{
  return n.get_declaration()->get_declarationModifier().get_storageModifier().isStatic();
}


AllocKind varAllocKind(const SgInitializedName& n)
{
  if (!isSgGlobal(n.get_scope()) && !isStaticVariable(n)) return akStack;

  if (isUpcShared(n.get_type())) return akUpcSharedGlobal;

  return akGlobal;
}

SgExpression*
RtedTransformation::getExprBelowAssignment( SgExpression* exp ) {
    SgExpression* parent = isSgExpression( exp->get_parent() );

    while(  parent
            && !(
                isSgAssignOp( parent )
//                || isSgAssignInitializer( parent )
                || isSgAndAssignOp( parent )
                || isSgDivAssignOp( parent )
                || isSgIorAssignOp( parent )
                || isSgLshiftAssignOp( parent )
                || isSgMinusAssignOp( parent )
                || isSgModAssignOp( parent )
                || isSgMultAssignOp( parent )
                || isSgPlusAssignOp( parent )
                || isSgPointerAssignOp( parent )
                || isSgRshiftAssignOp( parent )
                || isSgXorAssignOp( parent)
            )) {

        exp = parent;
        parent = isSgExpression( parent->get_parent() );
    }
    return exp;
}

bool
RtedTransformation::isthereAnotherDerefOpBetweenCurrentAndAssign(SgExpression* exp ) {
    SgExpression* parent = isSgExpression( exp->get_parent() );

    while(  parent
            && !(
                isSgAssignOp( parent )
//                || isSgAssignInitializer( parent )
                || isSgAndAssignOp( parent )
                || isSgDivAssignOp( parent )
                || isSgIorAssignOp( parent )
                || isSgLshiftAssignOp( parent )
                || isSgMinusAssignOp( parent )
                || isSgModAssignOp( parent )
                || isSgMultAssignOp( parent )
                || isSgPlusAssignOp( parent )
                || isSgPointerAssignOp( parent )
                || isSgRshiftAssignOp( parent )
                || isSgXorAssignOp( parent)
            )) {
      if (isSgPointerDerefExp(parent))
      return true;

        exp = parent;
        parent = isSgExpression( parent->get_parent() );
    }
    return false;
}

SgArrayType* isUsableAsSgArrayType( SgType* type ) {
    return isSgArrayType( skip_ReferencesAndTypedefs( type ));
}

SgReferenceType* isUsableAsSgReferenceType( SgType* type ) {
    return isSgReferenceType( skip_Typedefs( type ));
}

bool
RtedTransformation::isUsedAsLvalue( SgExpression* exp ) {
    if( NULL == exp ) return false;

    SgExpression* ancestor = getExprBelowAssignment( exp );
    SgBinaryOp* assign = isSgBinaryOp( ancestor -> get_parent() );
    return(
        assign && assign -> get_lhs_operand() == ancestor
    );
}

bool isConstructor( SgMemberFunctionDeclaration* mfun )
{
    if (!mfun) return false;
    // \pp \todo consider replacing with SageInterface::isCtor

    SgClassDeclaration* cdef = mfun -> get_associatedClassDeclaration();

    // \pp I am not sure why this should work ...
    //     e.g., why should be the mangeled name of the constructor
    //     (which encodes parameters and types) be the same as the class
    //     name?
    // check hasClassConstructor for another variant ...

    // a constructor is a member function whose name matches that
    return (  mfun -> get_mangled_name() == cdef -> get_mangled_name()
              // SgClassDefinition.get_mangled_name can return the
              // non-mangled name!
           || mfun -> get_name() == cdef -> get_mangled_name()
           );
}

template <class Iterator>
struct CtorSelector
{
  Iterator it;

  explicit
  CtorSelector(Iterator iter)
  : it(iter)
  {}

  void operator()(SgDeclarationStatement* decl)
  {
    SgMemberFunctionDeclaration* mfun = isSgMemberFunctionDeclaration( decl->get_definingDeclaration() );
    if (!isConstructor(mfun)) return;

    *it = mfun;
    ++it;
  }
};

template <class Iterator>
static inline
CtorSelector<Iterator> ctorSelector(Iterator iter)
{
  return CtorSelector<Iterator>(iter);
}

void appendConstructors(SgClassDefinition* cdef, SgMemberFunctionDeclarationPtrList& constructors)
{
  SgDeclarationStatementPtrList& members = cdef -> get_members();

  std::for_each( members.begin(), members.end(), ctorSelector(std::back_inserter(constructors)) );
}

bool isNormalScope( SgScopeStatement* n ) {
  return  (  isSgBasicBlock( n )
          || isSgIfStmt( n )
          || isSgWhileStmt( n )
          || isSgDoWhileStmt( n )
          || isSgForStatement( n )
          || isSgUpcForAllStatement( n )
          );
}

bool
RtedTransformation::isInInstrumentedFile( SgNode* n ) {
  ROSE_ASSERT( n );
  std::string file_name = n -> get_file_info() -> get_filename();

  return rtedfiles.find(file_name) != rtedfiles.end();
}

std::string removeSpecialChar(std::string str) {
  string searchString="\"";
  string replaceString="'";
  string::size_type pos = 0;
  while ( (pos = str.find(searchString, pos)) != string::npos ) {
    str.replace( pos, searchString.size(), replaceString );
    ++pos;
  }
  return str;
}

SgVarRefExp* genVarRef( SgInitializedName* initName )
{
    ROSE_ASSERT( initName );

    SgScopeStatement* scope = initName -> get_scope();
    SgVariableSymbol* varsym = isSgVariableSymbol(scope->lookup_symbol(initName->get_name()));

    return buildVarRefExp(varsym);
}


/// \brief   we have some expression, e.g. a var ref to m, but we want as much
///          of the expression as necessary to properly refer to m.
/// \return  greatest lvalue expression ancestor (e.g the operand of an
///          expression statement or assign op).
/// \example &foo.bar.m, instead of the erroneous &m
static
SgExpression* getUppermostLvalue( SgExpression* exp )
{
  if (exp) cerr << "Checking if exp has parent: " << exp->unparseToString() << "  :" << endl;

  if (exp==NULL || exp->get_parent()==NULL) return exp;

  // was: SgType* const orig_type = exp->get_type(); // see comment below
  SgDotExp*     parent = isSgDotExp( exp->get_parent() );

  // \pp \note why only SgDotExp? what about DotStar, Arrows, and DotArrow?
  while (parent)
  {
      // in C++, the rhs of the dot may be a member function, which we'd want
      // to ignore
      SgExpression* rhs = parent -> get_rhs_operand();
      if ( isSgMemberFunctionRefExp( rhs ))
      {
          // \pp ???? is this legal C++?
          // in, e.g.
          //  foo.bar = 9001
          //  foo.bar.=( 9001 )
          // we want to stop at the dot exp
          //  foo.bar
          break;
      }

      exp = parent;
      parent = isSgDotExp( parent->get_parent() );
  }

  // \pp added assert; if the function operates according to its description
  //     then the returned type has to be the same as the field type.
  // does not hold for tests/Cxx/simple/union2
  // ROSE_ASSERT(exp->get_type() == orig_type);

  return exp;
}

SgFunctionDeclaration* RtedTransformation::getDefiningDeclaration( SgFunctionCallExp* fn_call ) {
    SgFunctionDeclaration* fn_decl = fn_call -> getAssociatedFunctionDeclaration();
    ROSE_ASSERT( fn_decl );

    SgFunctionDeclaration* rv =
        isSgFunctionDeclaration( fn_decl -> get_definingDeclaration() );
    if( rv ) return rv;


    // fn_decl wasn't the defining declaration and the definition wasn't in the
    // same AST.  Time to search.
    class anon : public ROSE_VisitTraversal {

        SgFunctionDeclaration* & rv_ref;
        const SgName mangled_name;

        public:
            anon( SgFunctionDeclaration* & _rv, const SgName & _mangled_name )
                : rv_ref( _rv ), mangled_name( _mangled_name ) {}

            void visit( SgNode* node ) {
                SgFunctionDefinition*  fn_def = isSgFunctionDefinition( node );
                ROSE_ASSERT( fn_def );
                SgFunctionDeclaration* fn_decl = fn_def -> get_declaration();
                ROSE_ASSERT( fn_decl );

                if( mangled_name == fn_decl -> get_mangled_name()) {
                    rv_ref = fn_decl;
                }
            }
    } visitor( rv, fn_decl -> get_mangled_name() );
    SgFunctionDefinition::traverseMemoryPoolNodes( visitor );

    // may as well set the reference while we're here, in case of multiple call
    // sites for the same declaration
    fn_call -> getAssociatedFunctionDeclaration() -> set_definingDeclaration( rv );

    return rv;
}

bool isGlobalExternVariable(SgStatement* stmt)
{
  bool                     res = false;
  SgDeclarationStatement*  declstmt = isSgDeclarationStatement(stmt);
  SgFunctionParameterList* funcparam = isSgFunctionParameterList(stmt);

  if (funcparam)
  {
    SgFunctionDeclaration* funcdeclstmt = isSgFunctionDeclaration(funcparam->get_parent());
    ROSE_ASSERT(funcdeclstmt);

    // \pp why can a parameter be extern global?
    // \pp \todo I am not sure that this describes the desired behaviour for parameters...
    res = funcdeclstmt->get_declarationModifier().get_storageModifier().isExtern();
  }
  else if (declstmt)
  {
    res = declstmt->get_declarationModifier().get_storageModifier().isExtern();
  }

  return res;
}


/*************************************************
 * This function adds some very common arguments
 * to instrumented function calls for constructing
 * SourcePosition objects.
 ************************************************/
void RtedTransformation::appendFileInfo( SgExprListExp* arg_list, SgStatement* stmt)
{
    ROSE_ASSERT( arg_list && stmt );
    appendFileInfo( arg_list, stmt->get_scope(), stmt->get_file_info() );
}

void RtedTransformation::appendAllocKind( SgExprListExp* arg_list, AllocKind kind)
{
  ROSE_ASSERT( arg_list );

  appendExpression(arg_list, mkAllocKind(kind));
}

static
SgGlobal* globalScope(SgScopeStatement* scope)
{
  SgGlobal* res = isSgGlobal(scope);

  if (!res) res = sg::ancestor<SgGlobal>(scope);

  ROSE_ASSERT(res);
  return res;
}

void RtedTransformation::appendFileInfo( SgExprListExp* arg_list, SgScopeStatement* scope, Sg_File_Info* file_info)
{
    ROSE_ASSERT(arg_list && scope && file_info);


    SgExpression*           filename = buildStringVal( file_info->get_filename() );
    SgExpression*           linenr = buildIntVal( file_info->get_line() - 1);
    SgExpression*           linenrTransformed = buildOpaqueVarRefExp("__LINE__", globalScope(scope));
    SgExprListExp*          fiArgs = buildExprListExp();

    appendExpression( fiArgs, filename );
    appendExpression( fiArgs, linenr );
    appendExpression( fiArgs, linenrTransformed );

    SgAggregateInitializer* fiCtorArg = genAggregateInitializer(fiArgs, roseFileInfo());

    appendExpression( arg_list, ctorSourceInfo(fiCtorArg) );
}

SgType* skip_ModifierType(SgType* t)
{
  SgModifierType* sgmod = isSgModifierType(t);
  if (sgmod != NULL) return sgmod->get_base_type();

  return t;
}

SgType* skip_PointerType(SgType* t)
{
  SgPointerType* sgptr = isSgPointerType(t);
  if (sgptr != NULL) return sgptr->get_base_type();

  return t;
}


/*
struct PointerLevelSkipper
{
  SgType* res;

  explicit
  PointerLevelSkipper(SgType* t)
  : res(t)
  {}

  template <class SageNode>
  void skip(SageNode& n)
  {
    res = sg::dispatch(PointerLevelSkipper(res), n.get_base_type());
  }

  void handle(SgNode&) { ROSE_ASSERT(false); }
  void handle(SgType&) {}

  void handle(SgModifierType& n) { skip(n); }
  void handle(SgTypedefType& n) { skip(n); }

  void handle(SgPointerType& n)
  {
    res = n.get_base_type();
  }

  operator SgType*() { return res; }
};

SgType* skip_PointerLevel(SgType* t)
{
  return  sg::dispatch(PointerLevelSkipper(t), t);
}
*/

struct PointerDiscoverer
{
  SgPointerType* res;

  PointerDiscoverer()
  : res(NULL)
  {}

  template <class SgNodeType>
  void skip(SgNodeType& n)
  {
    res = sg::dispatch(PointerDiscoverer(), n.get_base_type());
  }

  void handle(SgNode& n)
  {
    std::cerr << "unexpected type: " << typeid(n).name() << std::endl;
    ROSE_ASSERT(false);
  }

  void handle(SgPointerType& n)   { res = &n; }

  void handle(SgModifierType& n)  { skip(n); }
  void handle(SgReferenceType& n) { skip(n); }
  void handle(SgArrayType& n)     { skip(n); }
  void handle(SgTypedefType& n)   { skip(n); }

  operator SgPointerType*() { return res; }
};

SgPointerType* discover_PointerType(SgType* t)
{
  return sg::dispatch(PointerDiscoverer(), t);
}

AllocKind cxxHeapAllocKind(SgType* t)
{
  SgType* allocType = skip_ModifierType(skip_PointerType(t));

  // \pp at least there is a pointer type that needs to be skipped
  ROSE_ASSERT(allocType != t);

  return isSgArrayType(allocType) ? akCxxArrayNew : akCxxNew;
}

SgType* skip_ArrayType(SgType* t)
{
  SgArrayType* sgarr = isSgArrayType(t);
  if (sgarr != NULL) return sgarr->get_base_type();

  return t;
}

SgType* skip_ArrPtrType(SgType* t)
{
  // \pp \todo shouldn't we also skip modifier types?
  SgPointerType* sgptr = isSgPointerType(t);
  if (sgptr != NULL) return sgptr->get_base_type();

  return skip_ArrayType(t);
}


/// \brief   determines the number of indirections to a concrete type
///          and the shared qualifiers on the way.
/// \example int                a      ;  /* indirections, shared_mask: 0, 0 */
///          int*               b      ;  /* indirections, shared_mask: 1, 0 */
///          shared int         c      ;  /* indirections, shared_mask: 0, 1 */
///          shared int*        d      ;  /* indirections, shared_mask: 1, 2 */
///          shared int* shared e      ;  /* indirections, shared_mask: 1, 3 */
///          shared int         f   [1];  /* indirections, shared_mask: 1, 3 */
///          shared int         (*g)[1];  /* indirections, shared_mask: 2, 6 */
///          shared int         *h  [1];  /* indirections, shared_mask: 2, 4 */
struct IndirectionHandler
{
    IndirectionHandler()
    : res()
    {}

    // extends the shared bit from the lower level
    //   e.g., shared int x[1] is stored in shared space
    //         so the shared bit is extended from the base type (int)
    //         to the array type.
    TypeStructureInfo inherit_shared_bit(TypeStructureInfo r)
    {
      ROSE_ASSERT((r.desc.shared_mask & 1) == 0);

      const size_t shared_mask = (r.desc.shared_mask & 2) >> 1;

      r.desc.shared_mask |= shared_mask;
      return r;
    }

    template <class SageNode>
    void add_indirection(SageNode& n)
    {
      res = sg::dispatch(IndirectionHandler(), n.get_base_type());
      res.desc = rted_address_of(res.desc);
    }

    template <class SageNode>
    void skip(SageNode& n)
    {
      res = sg::dispatch(IndirectionHandler(), n.get_base_type());
    }

    void unexpected(SgNode& n)
    {
      sg::unused(n);
      ROSE_ASSERT(false);
    }

    /// unexpected node type
    void handle(SgNode& n) { unexpected(n); }

    /// base case
    void handle(SgType& n) { res.type = &n; }

    /// use base type
    void handle(SgPointerType& n) { add_indirection(n); }

    void handle(SgArrayType& n)
    {
      add_indirection(n);

      res = inherit_shared_bit(res);
    }

    void handle(SgReferenceType& n)
    {
      // references are not counted as indirections
      skip(n);
    }

    void handle(SgTypedefType& n)
    {
      // the shared info might be in the base_type
      skip(n);
    }

    // \pp \todo do we need to handle pointer to members?

    /// extract UPC shared information
    void handle(SgModifierType& n)
    {
      const SgTypeModifier&       modifier = n.get_typeModifier();
      const SgUPC_AccessModifier& upcModifier = modifier.get_upcModifier();

      if (upcModifier.get_isShared())
      {
        skip(n);
        ROSE_ASSERT((res.desc.shared_mask & 1) == 0);

        res.desc.shared_mask |= 1;
      }
      else
      {
        // \pp \note \todo the rted nov10 code does not skip over
        //                 modifier types. That's likely a bug.
        res.type = &n;
      }
    }

    operator TypeStructureInfo() const
    {
      ROSE_ASSERT(res.type != NULL);

      return res;
    }

  private:
    TypeStructureInfo res;
};


/// \brief  counts the number of indirection layers (i.e., Arrays, Pointers)
///         between a type and the first non indirect type.
/// \return the length of indirections and for UPC a description of shared
///         storage properties
TypeStructureInfo indirections(SgType* t)
{
  return sg::dispatch(IndirectionHandler(), t);
}

SgType* skip_References(SgType* t)
{
  if (!isSgReferenceType(t)) return t;

  return isSgReferenceType(t)->get_base_type();
}

SgType* skip_TopLevelTypes(SgType* t)
{
  SgType* res = skip_References(skip_ModifierType(t));

  if (res == t) return res;
  return skip_TopLevelTypes(res);
}


SgAggregateInitializer*
RtedTransformation::mkTypeInformation(SgType* type, bool resolve_class_names, bool array_to_pointer)
{
  ROSE_ASSERT(type);

  TypeStructureInfo indir_desc  = indirections(type);
  SgType*           base_type   = indir_desc.type;
  SgType*           unmod_type  = skip_TopLevelTypes(type);
  AddressDesc       addrDesc    = indir_desc.desc;
  std::string       type_string = unmod_type->class_name();
  std::string       base_type_string;

  if (addrDesc.levels > 0)
    base_type_string = base_type -> class_name();

  // convert SgClassType to the mangled_name, if asked
  if( resolve_class_names ) {
    if( isSgClassType( unmod_type ))
      type_string
        = isSgClassDeclaration(
            isSgClassType( unmod_type ) -> get_declaration()
          ) -> get_mangled_name();

    if( addrDesc.levels > 0 && isSgClassType( base_type ))
      base_type_string
        = isSgClassDeclaration(
            isSgClassType( base_type ) -> get_declaration()
          ) -> get_mangled_name();
  }

  // convert array types to pointer types, if asked
  if (array_to_pointer && type_string == "SgArrayType")
    type_string = "SgPointerType";

  SgExprListExp*          initexpr = buildExprListExp();

  appendExpression( initexpr, buildStringVal(type_string) );
  appendExpression( initexpr, buildStringVal(base_type_string) );
  appendExpression( initexpr, mkAddressDesc(addrDesc) );

  // \note when the typedesc object is added to an argument list, it requires
  //       an explicit type cast. This is not always required (e.g., when we
  //       create a list of typedesc objects, b/c then the type cast is added
  //       through the type cast on top of the typedecl list.
  //
  //       rted_CreateHeapArr((rted_typedesc) { "SgArrayType", "BaseType", { 1, 0 } }, ...
  //     but:
  //       rted_CreateHeapArr((rted_typedesc[]) { /* no typecast */ { "SgArrayType", "BaseType", { 1, 0 } }, { "SgArrayType", "BaseType", { 1, 0 } } }, ...

  return genAggregateInitializer(initexpr, roseTypeDesc());
}



void
RtedTransformation::appendDimensions(SgExprListExp* arg_list, const RtedArray& arr)
{
      ROSE_ASSERT(arg_list);

      SgExprListExp* dimargs = buildExprListExp();
      SgExpression*  noDims = SageBuilder::buildIntVal( arr.getDimension() );

      SageInterface::appendExpression(dimargs, noDims);
      BOOST_FOREACH( SgExpression* expr, arr.getIndices() ) {
          if ( expr == NULL )
          {
              // \pp ???
              expr = SageBuilder::buildIntVal( -1 );
          }

          ROSE_ASSERT( expr );

          // we add a value 0, otherwise the unparser skips the expressions
          // \pp \todo fix the unparser
          expr = SageBuilder::buildAddOp(expr, SageBuilder::buildIntVal(0));

          SageInterface::appendExpression( dimargs, expr );
      }

      SgExpression* dimexpr = ctorDimensionList(genAggregateInitializer(dimargs, roseDimensionType()));

      SageInterface::appendExpression(arg_list, dimexpr);
}

void RtedTransformation::appendDimensionsIfNeeded( SgExprListExp* arg_list, RtedClassElement* rce )
{
  RtedArray* arr = rce->get_array();

  if (arr == NULL)
  {
    ROSE_ASSERT(rce->extraArgSize() == 0);
    return;
  }

  appendDimensions(arg_list, *arr);
}

void RtedTransformation::appendAddressAndSize( SgExprListExp* arg_list,
                                               AppendKind am,
                                               SgScopeStatement* scope,
                                               SgExpression* varRefE,
                                               SgClassDefinition* unionClass
                                             )
{
 //   SgScopeStatement* scope = NULL;

    // FIXME 2: It would be better to explicitly handle dot and arrow
    // expressions here, rather than indirectly through the var's declared scope
#if 0
    if( initName) {
        scope = initName->get_scope();
    }
#endif

    SgExpression* exp = varRefE;

    if ( isSgClassDefinition(scope) ) {
        // member -> &( var.member )
        exp = getUppermostLvalue( varRefE );
    }

    SgType* type = (varRefE ? varRefE -> get_type() : NULL);

    appendAddressAndSize( arg_list, am, exp, type, unionClass );
}


void RtedTransformation::appendAddressAndSize(
                            SgExprListExp* arg_list,
                            AppendKind am,
                            SgExpression* exp,
                            SgType* type,
                            SgClassDefinition* /*unionclass*/)
{
    SgExpression* szop = exp;    // only needed to get an expression where we can take the size
    SgExpression* szexp = NULL;  // sizeof expression

    // for pointer arithmetic variable access in expressions, we want the
    // equivalent expression that computes the new value.
    //
    // e.g., with
    //  int a[] = { 7, 8 };
    //  int *p = &a[ 0 ];
    //  int q = *(p++);
    //
    // we want the last assignment to check that (p + 1) is a valid address to
    // read
    if (isSgPlusPlusOp( szop ) || isSgMinusMinusOp( szop ))
    {
      szop = isSgUnaryOp( szop ) -> get_operand();
    }
    else if( isSgPlusAssignOp( szop ) || isSgMinusAssignOp( szop ))
    {
      szop = isSgBinaryOp( szop ) -> get_lhs_operand();
    }

    // consider, e.g.
    //
    //      char* s1;
    //      char s2[20];
    //
    //      s1 = s2 + 3;
    //
    //  we only want to access s2..s2+sizeof(char), not s2..s2+sizeof(s2)
    //
    //  but we do want to create the array as s2..s2+sizeof(s2)
    if( (am == Elem) && isSgArrayType( type ))
    {
        SgType* clone = deepCopy(isSgArrayType(type)->get_base_type());

        szexp = buildSizeOfOp(clone);
    }
#if 0
      else if (unionclass) {
  cerr <<"isUnionClass" << endl;
  SgType* classtype = unionclass ->get_declaration()->get_type();
  cerr <<" unionclass : " << unionclass->unparseToString()<<"  type: " << classtype->class_name() << endl;

         szexp = buildSizeOfOp( isSgType(classtype-> copy( copy )));
         //        buildSizeOfOp( buildIntVal(8))
      }
#endif
    else if (szop)
    {
        SgExpression* clone = deepCopy(szop);

        szexp = buildSizeOfOp(clone);
    }
    else
    {
        // \pp can this happen?
        szexp = buildSizeOfOp(buildLongIntVal(0));
    }

    ROSE_ASSERT(szexp != NULL);

    appendAddress( arg_list, exp );
    appendExpression( arg_list, szexp );
}

#if OBSOLETE_CODE

// we have to make exp a valid lvalue
// \pp \todo the traversal of the subtree seems not needed
//           why do not we just test whether a node is a cast-expression?
class LValueVisitor : public AstSimpleProcessing
{
public:
  SgExpression* rv;
  bool first_node;

  SgExpression* visit_subtree( SgExpression* n ) {
    first_node = true;
    rv = NULL;

    traverse( n, preorder );

    // if the first node we visit is an SgCastExp, we want to
    // return something else.  Otherwise, if we've only modified
    // proper subtrees of n, n will have been appropriately modified
    // and we can simply return it.
    return rv ? rv : isSgExpression( n );
  }

  /// strip out cast expressions from the subtree
  /// tps (09/18/2009) but only if they come first
  /// dont strip : *((cast)*val) because that leaves only *val
  void visit( SgNode* _n ) {
    SgCastExp* n = isSgCastExp( _n );
    //    cerr << " $$$$$ Traversing nodes : " << _n->class_name() << endl;
    if( n==NULL && first_node ) {
      first_node = false;
      rv = n;
    }
    if( !n )
      return;

    if( first_node ) {
      first_node = false;

      rv = n -> get_operand();
    } else {
      //replaceExpression( n, n -> get_operand() );
    }
  }
};

#endif /* OBSOLETE_CODE */

static
SgExpression* skip_Casts(SgExpression* exp)
{
  SgCastExp* cast = isSgCastExp(exp);

  if (cast == NULL) return exp;
  return skip_Casts(cast->get_operand());
}


struct AddressOfExprBuilder
{
  SgExpression* res;

  AddressOfExprBuilder()
  : res(NULL)
  {}

  void set(SgExpression& v) { res = &v; }
  void address(SgExpression& v) { res = buildAddressOfOp(&v); }

  void handle(SgNode& n) { ROSE_ASSERT(false); }

  void handle(SgExpression& n) { address(n); }

  void handle(SgPointerDerefExp& n) { set(*n.get_operand()); }

  void handle(SgVarRefExp& n)
  {
    SgType* exptype = n.get_type();
    SgType* basetype = skip_ModifierType(exptype);

    if (isSgArrayType(basetype))
    {
      set(n);
    }
    else
    {
      address(n);
    }
  }

  operator SgExpression*()
  {
    ROSE_ASSERT(res);
    return res;
  }
};

static
SgExpression* addressOfExpr(SgExpression* n)
{
  return sg::dispatch(AddressOfExprBuilder(), n);
}


static
SgExpression* genAddressOf(SgExpression* const exp, bool upcShared)
{
  // \pp \todo why can the expression be NULL?
  if (exp == NULL) return buildLongIntVal(0);

  // \pp skip cast expressions b/c they create an r-value
  //     but we need an l-value when we use the address (& (int*) ptr)
  //~ LValueVisitor       make_lvalue_visitor;
  //~ SgExpression*       cloneexp = deepCopy(exp);
  //~ SgExpression* const exp_copy = make_lvalue_visitor.visit_subtree( cloneexp );

  SgExpression* const postcast = skip_Casts(exp);
  SgExpression* const exp_copy = deepCopy(postcast);
  SgType*             char_type = buildCharType();

  // in UPC we must not cast away the shared modifier
  // \pp \todo replace the magic -1 with sth more meaningful
  //           also in Cxx_Grammar and SageBuilder
  if (upcShared) char_type = buildUpcSharedType(char_type, -1);

  SgType*             char_ptr = buildPointerType(char_type);
  SgExpression*       address_exp = addressOfExpr(exp_copy);

  // we return the address as [shared] char*
  return buildCastExp(address_exp, char_ptr);
}

SgFunctionCallExp* RtedTransformation::mkAddress(SgExpression* exp, bool upcShared) const
{
  ROSE_ASSERT( exp );

  SgExprListExp* const args = buildExprListExp();
  SgFunctionSymbol*    ctor = symbols.roseAddr;

  // for upc shared pointers we call rted_AddrSh and cast the
  //   expression to a shared char*
  if (upcShared)
  {
    ROSE_ASSERT(withupc);

    SgType* char_type = buildCharType();

    // \pp \todo replace the magic -1 with sth more meaningful
    SgType* shchar_type = buildUpcSharedType(char_type, -1);

    exp = buildCastExp(exp, buildPointerType(shchar_type));
    ctor = symbols.roseAddrSh;
  }

  ROSE_ASSERT(ctor);
  appendExpression(args, exp);
  return buildFunctionCallExp(ctor, args);
}


SgFunctionCallExp*
RtedTransformation::genAdjustedAddressOf(SgExpression* exp)
{
  if (exp == NULL) return mkAddress(SageBuilder::buildIntVal(0), false /* not shared */);

  SgIntVal* offset = NULL;

  if( isSgPlusPlusOp( exp )) {
      offset = buildIntVal( 1 );
      exp = isSgUnaryOp( exp ) -> get_operand();
  } else if( isSgMinusMinusOp( exp )) {
      offset = buildIntVal( -1 );
      exp = isSgUnaryOp( exp ) -> get_operand();
  } else if( isSgPlusAssignOp( exp )) {
      offset = isSgIntVal( isSgBinaryOp( exp ) ->get_rhs_operand());
      exp = isSgBinaryOp( exp ) -> get_lhs_operand();
  } else if( isSgMinusAssignOp( exp )) {
      offset = buildIntVal(
          -1 *
          isSgIntVal( isSgBinaryOp( exp ) ->get_rhs_operand()) -> get_value()
      );
      exp = isSgBinaryOp( exp ) -> get_lhs_operand();
  }

  ROSE_ASSERT( exp != NULL );
  const bool    upcShared = isUpcShared(exp->get_type());
  SgExpression* arg = genAddressOf(exp, upcShared);

  // add in offset if exp points to a complex data struture
  if ( offset != NULL )
  {
    Sg_File_Info* fi = exp -> get_file_info();
    SgMultiplyOp* mul = new SgMultiplyOp( fi, offset, buildSizeOfOp(exp) );

    arg = new SgAddOp( fi, arg, mul );
  }

  return mkAddress(arg, upcShared);
}


/*
 * Appends the address of exp.
 */
void RtedTransformation::appendAddress(SgExprListExp* arg_list, SgExpression* exp)
{
    SgExpression* arg = genAdjustedAddressOf(exp);

    appendExpression( arg_list, arg );
}


void appendClassName( SgExprListExp* arg_list, SgType* type )
{
    // \pp \todo skip SgModifier types (and typedefs)
    // \was SgType* basetype = skip_ArrPtrType(type);
    SgType* basetype = indirections(type).type;

    if (basetype != type)
    {
      appendClassName( arg_list, basetype );
    }
    else if( isSgClassType( type ))
    {
      SgClassDeclaration* classdecl = isSgClassDeclaration( isSgClassType( type ) -> get_declaration() );

      appendExpression( arg_list, buildStringVal(classdecl->get_mangled_name()) );
    }
    else
    {
      appendExpression( arg_list, buildStringVal( "" ) );
    }
}

void appendBool(SgExprListExp* arglist, bool val)
{
  appendExpression(arglist, buildIntVal(val));
}


void
RtedTransformation::prependPseudoForInitializerExpression(SgExpression* exp, SgStatement* for_stmt)
{
  ROSE_ASSERT (for_stmt!=NULL);

  RtedForStmtProcessed *for_stmt_processed = NULL;

  if( for_stmt -> attributeExists( RtedForStmtProcessed::Key ))
    for_stmt_processed
      = static_cast< RtedForStmtProcessed* >(
        for_stmt -> getAttribute( RtedForStmtProcessed::Key ));

  if( !for_stmt_processed ) {

    // { int RuntimeSystem_eval_once = 1; for( ... ) }
    SgBasicBlock *new_scope = buildBasicBlock();
    SgVariableDeclaration* eval_once_var
      = buildVariableDeclaration(
        "RuntimeSystem_eval_once",
        buildIntType(),
        buildAssignInitializer( buildIntVal( 1 )),
        new_scope );
    new_scope -> prepend_statement( eval_once_var );

    // eval_once = 0
    SgAssignOp* initial_expression
      = buildBinaryExpression< SgAssignOp >(
        buildVarRefExp( "RuntimeSystem_eval_once", new_scope ),
        buildIntVal( 0 ));

    // for( ... ; test; ... )
    // -> for( ... ; test | (eval_once && ((eval_once = 0) | exp1 | exp2 ... )); ... )
    SgExprStatement* test = isSgExprStatement( GeneralizdFor::test(for_stmt) );
    ROSE_ASSERT( test );

    SgExpression* old_test = test -> get_expression();
    replaceExpression(
        test -> get_expression(),
        // test | ...
        buildBinaryExpression< SgBitOrOp >(
            // throw-away expression to avoid null values
            buildIntVal( 0 ),
            // eval_once && ...
            buildBinaryExpression< SgAndOp >(
                buildVarRefExp( "RuntimeSystem_eval_once", new_scope ),
                initial_expression )),
        true);
    isSgBinaryOp( test -> get_expression() ) -> set_lhs_operand( old_test );

    replaceStatement( for_stmt, new_scope );
    new_scope -> append_statement( for_stmt );

    for_stmt_processed = new RtedForStmtProcessed( initial_expression );
    for_stmt
      -> addNewAttribute( RtedForStmtProcessed::Key, for_stmt_processed  );
  }

  ROSE_ASSERT( for_stmt_processed );

  // old_exp -> old_exp | new_exp
  SgBinaryOp* new_exp
    = buildBinaryExpression< SgBitOrOp >(
        exp,
        // This is just a throw-away expression to avoid NULL issues
        buildIntVal( 0 ));

  replaceExpression( for_stmt_processed -> get_exp(), new_exp, true );
  new_exp -> set_rhs_operand( for_stmt_processed -> get_exp() );
  for_stmt_processed -> set_exp( new_exp );
}

SgBasicBlock*
RtedTransformation::buildGlobalConstructor(SgScopeStatement* scope, std::string name)
{
  //SgStatement* global=NULL;
  // build the classdeclaration and definition for RtedGlobal
  SgClassDeclaration* decl = buildClassDeclaration_nfi(
                   "RtedGlobal_"+name,
                   SgClassDeclaration::e_class,
                   scope, NULL);
  ROSE_ASSERT(decl);
  globConstructor = decl;
  SgClassDefinition* def = buildClassDefinition(decl);
  ROSE_ASSERT(def);

  SgFunctionParameterList* param = buildFunctionParameterList (buildFunctionParameterTypeList());
  ROSE_ASSERT(param);
  ROSE_ASSERT(def->get_scope());
  ROSE_ASSERT(def->get_parent()); // should be the declaration
  ROSE_ASSERT(isSgClassDefinition(def));

  SgMemberFunctionDeclaration* mf = buildDefiningMemberFunctionDeclaration("RtedGlobal_"+name,buildVoidType(),param,def);
  ROSE_ASSERT(mf);

  SgSpecialFunctionModifier& fmod = mf->get_specialFunctionModifier();
  fmod.setConstructor();
  SgFunctionDefinition* fdef = mf->get_definition();
  ROSE_ASSERT(fdef);

  SgBasicBlock* block = fdef->get_body();
  ROSE_ASSERT(block);

  // append memberfunc decl to classdef
  prependStatement(mf, def);

  // build the call to RtedGlobal: RtedGlobal rg;
  globalConstructorVariable = buildVariableDeclaration("rg_"+name,decl->get_type(),NULL,scope);
  ROSE_ASSERT(globalConstructorVariable);

  return block;
}

bool traverseAllChildrenAndFind(SgExpression* varRef, SgStatement* stmt)
{
  ROSE_ASSERT(varRef);

  return (stmt!=NULL) && SageInterface::isAncestor(stmt, varRef);

  //~ if (stmt==NULL) return false;
  //~
  //~ const SgNodePtrList&          nodes = NodeQuery::querySubTree(stmt,V_SgExpression);
  //~ SgNodePtrList::const_iterator zz = nodes.end();
  //~ SgNodePtrList::const_iterator pos = std::find(nodes.begin(), zz, varRef);
  //~
  //~ return (pos != zz);
}

bool traverseAllChildrenAndFind(SgInitializedName* varRef, SgStatement* stmt)
{
  ROSE_ASSERT(varRef);

  return (stmt!=NULL) && SageInterface::isAncestor(stmt, varRef);

  // \pp \todo replace this and the previous function with isAncestorOf implementation
  //~ if (stmt == NULL) return false;
  //~
  //~ const SgNodePtrList&          nodes = NodeQuery::querySubTree(stmt,V_SgInitializedName);
  //~ SgNodePtrList::const_iterator zz = nodes.end();
  //~ SgNodePtrList::const_iterator pos = std::find(nodes.begin(), zz, varRef);
  //~
  //~ return (pos != zz);
}

SgBasicBlock*
RtedTransformation::appendToGlobalConstructor(SgScopeStatement* scope, std::string name)
{
  ROSE_ASSERT(scope);

  globalFunction = buildGlobalConstructor(scope,name);
  ROSE_ASSERT(globalFunction);

  return globalFunction;
}

void
RtedTransformation::appendGlobalConstructor(SgScopeStatement* /*scope*/, SgStatement* stmt)
{
  ROSE_ASSERT(globConstructor && stmt);

  insertStatement(stmt, globConstructor, true);
}

void
RtedTransformation::appendGlobalConstructorVariable(SgScopeStatement* /*scope*/, SgStatement* stmt)
{
  // add the global constructor to the top of the file
  ROSE_ASSERT(globalConstructorVariable && stmt);

  insertStatement(stmt, globalConstructorVariable);
}


SgVariableDeclaration*
RtedTransformation::getGlobalVariableForClass(SgGlobal* gl,
                SgClassDeclaration* classStmt) {
  SgVariableDeclaration* var = NULL;
  string classDeclName = classStmt->get_name().str();

  // get all children of global
  SgDeclarationStatementPtrList decls = gl->get_declarations();
  SgDeclarationStatementPtrList::const_iterator it = decls.begin();
  for (;it!=decls.end();++it) {
    SgVariableDeclaration* vard = isSgVariableDeclaration(*it);
    if (vard) {
      // check if declared variable is a class and if it maches with classStmt
      SgInitializedName* initName = *(vard->get_variables().begin());
      ROSE_ASSERT(initName);
      SgClassType* varT = isSgClassType(initName->get_type());
      if (varT) {
  string classname = varT->get_name().str() ;
  //cerr<< "Found global var with type : " << varT->class_name() << "  " <<
  //  initName->unparseToString() << "  class name : " <<
  //  classname<< "         comparing to == " << classDeclName << "   compGen: " <<
  //    classStmt->get_file_info()->isCompilerGenerated() << endl;
  if (classname==classDeclName)
    var=vard;
      }
    }
  }

  return var;

}

#endif
