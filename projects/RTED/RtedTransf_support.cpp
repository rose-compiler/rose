// vim:et sta sw=4 ts=4:
#include <rose.h>

// DQ (2/9/2010): Testing use of ROE to compile ROSE.
#ifndef USE_ROSE

#include <string>
#include <algorithm>

#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

#include "rosez.hpp"

#include "RtedSymbols.h"
#include "DataStructures.h"
#include "RtedTransformation.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;

// This really belongs in the non-existent DataStructures.cpp
const std::string RtedForStmtProcessed::Key( "Rted::ForStmtProcessed" );


/**
 * Helper Functions
 *
 *
 * This function returns the statement that
 * surrounds a given Node or Expression
 ****************************************/

bool is_main_func(const SgFunctionDefinition* fndef)
{
  ROSE_ASSERT(fndef);

  const SgFunctionDeclaration* fndecl = fndef->get_declaration();
  ROSE_ASSERT(fndecl);

  const std::string fnname = fndecl->get_name().str();

  return (  isSgGlobal(fndef->get_scope())
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

SgStatement* getSurroundingStatement(SgExpression* n)
{
  return ez::ancestor<SgStatement>(n);
}

SgStatement* getSurroundingStatement(SgInitializedName* n)
{
  return ez::ancestor<SgStatement>(n);
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
  SgExprStatement* exprStmt = insertCheck(ilBefore, stmt, checker, args);

  attachComment(exprStmt, "", PreprocessingInfo::before);
  attachComment(exprStmt, comment, PreprocessingInfo::before);

  return exprStmt;
}

SgExprStatement*
insertCheckOnStmtLevel(InsertLoc iloc, SgExpression* checked_node, SgFunctionSymbol* checker, SgExprListExp* args)
{
  return insertCheck(iloc, getSurroundingStatement(checked_node), checker, args);
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
static
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

static
bool upcSharedFlag(const SgModifierType& n)
{
  const SgTypeModifier&       modifier = n.get_typeModifier();
  const SgUPC_AccessModifier& upcModifier = modifier.get_upcModifier();

  return upcModifier.get_isShared();
}

static
bool isUpcShared(const SgType* n)
{
  const SgModifierType* t = isSgModifierType(n);
  bool                  res = false;

  // \pp \note can ModifierTypes be stacked?
  while (t != NULL && !res)
  {
    res = upcSharedFlag(*t);
    t = isSgModifierType(t->get_base_type());
  }

  return res;
}

/// \brief adds an AddressDesc for a single ptr/arr indirection
/// \param the base type (already after the indirection)
AddressDesc baseAddressDesc(const SgType* t)
{
  ROSE_ASSERT( t );

  AddressDesc desc;

  desc.levels = 0;
  desc.shared_mask = isUpcShared(t);

  return desc;
}


SgAggregateInitializer*
RtedTransformation::mkAddressDesc(SgExpression* expr) const
{
  ROSE_ASSERT( expr );

  return mkAddressDesc( baseAddressDesc(expr->get_type()) );
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

SgArrayType* RtedTransformation::isUsableAsSgArrayType( SgType* type ) {
    return isSgArrayType( skip_ReferencesAndTypedefs( type ));
}

SgReferenceType* RtedTransformation::isUsableAsSgReferenceType( SgType* type ) {
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


bool isConstructor( SgDeclarationStatement* decl )
{
    // \pp \todo consider replacing with SageInterface::isCtor

    // \note \tmp \pp the next line was moved in, might not be needed
    //                for calls from ExecuteTransformations
    SgMemberFunctionDeclaration* mfun = isSgMemberFunctionDeclaration( decl->get_definingDeclaration() );

    if( !mfun )
        // non member functions certainly aren't constructors
        return false;

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

static
bool isNonConstructor(SgDeclarationStatement* decl)
{
  return !isConstructor(decl);
}

void RtedTransformation::appendConstructors(SgClassDefinition* cdef, SgDeclarationStatementPtrList& constructors)
{
  SgDeclarationStatementPtrList& members = cdef -> get_members();

  std::remove_copy_if(members.begin(), members.end(), std::back_inserter(constructors), isNonConstructor);
}

bool isNormalScope( SgScopeStatement* n ) {
	return	(  isSgBasicBlock( n )
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

  return rtedfiles -> find( file_name ) != rtedfiles -> end();
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

/****************************************
 * This function returns InitializedName
 * for a DotExpr
 ****************************************/
std::pair<SgInitializedName*,SgVarRefExp*>
RtedTransformation::getRightOfDot(SgDotExp* dot, std::string str,
				  SgVarRefExp* varRef) {
  varRef=NULL;
  SgInitializedName* initName = NULL;
  SgExpression* rightDot = dot->get_rhs_operand();
  ROSE_ASSERT(rightDot);
  varRef = isSgVarRefExp(rightDot);
  if (varRef) {
    initName = (varRef)->get_symbol()->get_declaration();
  } else {
    cerr << "RtedTransformation : " << str << " - Unknown : "
	 << rightDot->class_name() << endl;
    ROSE_ASSERT(false);
  }
  return make_pair(initName,varRef);
}

std::pair<SgInitializedName*,SgVarRefExp*>
RtedTransformation::getRightOfDotStar(SgDotStarOp* dot, std::string str,
				      SgVarRefExp* varRef) {
  varRef=NULL;
  SgInitializedName* initName = NULL;
  SgExpression* rightDot = dot->get_rhs_operand();
  ROSE_ASSERT(rightDot);
  varRef = isSgVarRefExp(rightDot);
  if (varRef) {
    initName = (varRef)->get_symbol()->get_declaration();
  } else {
    cerr << "RtedTransformation : " << str << " - Unknown : "
	 << rightDot->class_name() << endl;
    ROSE_ASSERT(false);
  }
  return make_pair(initName,varRef);
}

/****************************************
 * This function returns InitializedName
 * for a DotExpr
 ****************************************/
std::pair<SgInitializedName*,SgVarRefExp*>
RtedTransformation::getRightOfPointerDeref(SgPointerDerefExp* dot, std::string str,
					   SgVarRefExp* varRef) {
  varRef=NULL;
  SgInitializedName* initName = NULL;
  SgExpression* rightDot = dot->get_operand();
  ROSE_ASSERT(rightDot);
  varRef = isSgVarRefExp(rightDot);
  if (varRef) {
    initName = (varRef)->get_symbol()->get_declaration();
  } else {
    cerr << "RtedTransformation : " << str << " - Unknown : "
	 << rightDot->class_name() << endl;
    ROSE_ASSERT(false);
  }
  return make_pair(initName,varRef);
}

/****************************************
 * This function returns InitializedName
 * for a ArrowExp
 ****************************************/
std::pair<SgInitializedName*,SgVarRefExp*>
RtedTransformation::getRightOfArrow(SgArrowExp* arrow, std::string str,
				    SgVarRefExp* varRef) {
  varRef=NULL;
  SgInitializedName* initName = NULL;
  SgExpression* rightArrow = arrow->get_rhs_operand();
  ROSE_ASSERT(rightArrow);
  varRef = isSgVarRefExp(rightArrow);
  if (varRef) {
    initName = varRef->get_symbol()->get_declaration();
  } else {
    cerr << "RtedTransformation : " << str << " - Unknown : "
	 << rightArrow->class_name() << endl;
    ROSE_ASSERT(false);
  }
  return make_pair(initName,varRef);
}

std::pair<SgInitializedName*,SgVarRefExp*>
RtedTransformation::getRightOfArrowStar(SgArrowStarOp* arrowstar, std::string str,
					SgVarRefExp* varRef) {
  varRef=NULL;
  SgInitializedName* initName = NULL;
  SgExpression* rightArrow = arrowstar->get_rhs_operand();
  ROSE_ASSERT(rightArrow);
  varRef = isSgVarRefExp(rightArrow);
  if (varRef) {
    initName = varRef->get_symbol()->get_declaration();
  } else {
    cerr << "RtedTransformation : " << str << " - Unknown : "
	 << rightArrow->class_name() << endl;
    ROSE_ASSERT(false);
  }
  return make_pair(initName,varRef);
}

/****************************************
 * This function returns InitializedName
 * for a PlusPlusOp
 ****************************************/
std::pair<SgInitializedName*,SgVarRefExp*>
RtedTransformation::getPlusPlusOp(SgPlusPlusOp* plus, std::string str,
				  SgVarRefExp* varRef) {
  varRef=NULL;
  SgInitializedName* initName = NULL;
  SgExpression* expPl = plus->get_operand();
  ROSE_ASSERT(expPl);
  varRef = isSgVarRefExp(expPl);
  if (varRef) {
    initName = varRef->get_symbol()->get_declaration();
  } else {
    cerr << "RtedTransformation : " << str << " - Unknown : "
	 << expPl->class_name() << endl;
    ROSE_ASSERT(false);
  }
  return make_pair(initName,varRef);
}

/****************************************
 * This function returns InitializedName
 * for a MinusMinusOp
 ****************************************/
std::pair<SgInitializedName*,SgVarRefExp*>
RtedTransformation::getMinusMinusOp(SgMinusMinusOp* minus, std::string str
				    , SgVarRefExp* varRef) {
  varRef=NULL;
  SgInitializedName* initName = NULL;
  SgExpression* expPl = minus->get_operand();
  ROSE_ASSERT(expPl);
  varRef = isSgVarRefExp(expPl);
  if (varRef) {
    initName = varRef->get_symbol()->get_declaration();
  } else {
    cerr << "RtedTransformation : " << str << " - Unknown : "
	 << expPl->class_name() << endl;
    ROSE_ASSERT(false);
  }
  return make_pair(initName,varRef);
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

  SgType* const orig_type = exp->get_type();
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
  ROSE_ASSERT(exp->get_type() == orig_type);

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

SgVarRefExp*
RtedTransformation::resolveToVarRefRight(SgExpression* expr) {
  SgVarRefExp* result = NULL;
  SgExpression* newexpr = NULL;
  ROSE_ASSERT(expr);
  if (	isSgDotExp( expr )
		|| isSgArrowExp( expr )) {

    newexpr= isSgBinaryOp(expr)->get_rhs_operand();
    result = isSgVarRefExp(newexpr);
  } else if( isSgPointerDerefExp( expr )) {
	  newexpr = isSgUnaryOp( expr ) -> get_operand();
	  result = isSgVarRefExp( newexpr );
  } else {
    cerr <<" >> resolveToVarRefRight : unknown expression " << expr->class_name() <<endl;
    ROSE_ASSERT( false );
  }
  if (!result) {
      cerr <<"  >> resolveToVarRefRight : right : " << newexpr->class_name() << endl;
      ROSE_ASSERT( false );
  }

  return result;
}

SgVarRefExp*
RtedTransformation::resolveToVarRefLeft(SgExpression* expr) {
  SgVarRefExp* result = NULL;
  SgExpression* newexpr = NULL;
  ROSE_ASSERT(expr);
  if (isSgDotExp(expr)) {
    newexpr= isSgDotExp(expr)->get_lhs_operand();
    result = isSgVarRefExp(newexpr);
    if (!result) {
      cerr <<"  >> resolveToVarRefRight : right : " << newexpr->class_name() << endl;
      ROSE_ASSERT( false );
    }
  } else {
    cerr <<" >> resolveToVarRefRight : unknown expression " << expr->class_name() <<endl;
    ROSE_ASSERT( false );
  }

  return result;
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

void RtedTransformation::appendFileInfo( SgExprListExp* arg_list, SgScopeStatement* scope, Sg_File_Info* file_info)
{
    ROSE_ASSERT(arg_list && scope && file_info);

    SgExpression*           filename = buildStringVal( file_info->get_filename() );
    SgExpression*           linenr = buildIntVal( file_info->get_line() );
    SgExpression*           linenrTransformed = buildOpaqueVarRefExp("__LINE__", scope);
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

SgType* skip_ArrPtrType(SgType* t)
{
  SgPointerType* sgptr = isSgPointerType(t);
  if (sgptr != NULL) return sgptr->get_base_type();

  SgArrayType*   sgarr = isSgArrayType(t);
  if (sgarr != NULL) return sgarr->get_base_type();

  // \pp \todo shouldn't we also skip modifier types?
  return t;
}


struct IndirectionHandler
{
    typedef std::pair<SgType*, AddressDesc> ResultType;

    IndirectionHandler()
    : res(NULL, rted_obj())
    {}

    ~IndirectionHandler()
    {
      // the type must be set
      ROSE_ASSERT(res.first);
    }

    template <class SgPtr>
    void add_indirection(SgPtr& n)
    {
      std::cerr << "*" << std::endl;
      res.first = n.get_base_type();
      ++res.second.levels;
    }

    void unexpected(SgNode& n)
    {
      ez::unused(n);
      ROSE_ASSERT(false);
    }

    /// unexpected node type
    void handle(SgNode& n) { unexpected(n); }

    /// base case
    void handle(SgType& n)
    {
      res.first = &n;
      std::cerr << "!" << std::endl;
    }

    /// use base type
    void handle(SgPointerType& n) { add_indirection(n); }
    void handle(SgArrayType& n)   { add_indirection(n); }

    void handle(SgReferenceType& n)
    {
      // references are not counted as indirections
      res.first = n.get_base_type();
    }

    /// extract UPC shared information
    void handle(SgModifierType& n)
    {
      std::cerr << "." << std::endl;
      bool sharedFlag = upcSharedFlag(n);

      if (sharedFlag)
      {
        const size_t mask_bit = (1 << res.second.levels);

        ROSE_ASSERT((res.second.shared_mask & mask_bit) == 0);
        res.second.shared_mask |= mask_bit;
      }

      // \pp \note \todo
      // the following conditional should (probably) be removed It is here b/c.
      // the Nov 2010 RTED code did not skip modified types                   .
      //   to be: res.first = n.get_base_type();
      res.first = sharedFlag ? n.get_base_type() : &n;
    }

    SgType& type() const { return *res.first; }
    ResultType result() const { return res; }

  private:
    std::pair<SgType*, AddressDesc> res;
};


/// \brief  counts the number of indirection layers (i.e., Arrays, Pointers)
///         between a type and the first non indirect type.
/// \return A type and the length of indirections
static
IndirectionHandler::ResultType
indirections(SgType* base_type)
{
  std::cerr << "$[$" << std::endl;
  IndirectionHandler ih = ez::visitSgNode(IndirectionHandler(), base_type);

  while (base_type != &ih.type())
  {
    base_type = &ih.type();
    ih = ez::visitSgNode(ih, base_type);
  }

  std::cerr << "$]$" << std::endl;
  return ih.result();
}


SgAggregateInitializer* RtedTransformation::mkTypeInformation(SgInitializedName* initName)
{
    // \pp was: if ( !initName ) return;
    ROSE_ASSERT(initName != NULL);

    return mkTypeInformation( initName, initName -> get_type() );
}

SgAggregateInitializer* RtedTransformation::mkTypeInformation(SgInitializedName* initName, SgType* type)
{
  // arrays in parameters decay to pointers, so we shouldn't
  // treat them as stack arrays
	bool array_decay = (  initName
                     && isSgFunctionParameterList( getSurroundingStatement( initName ))
                     && type->class_name() == "SgArrayType"
                     );

	return mkTypeInformation( type, false, array_decay );
}

SgAggregateInitializer* RtedTransformation::mkTypeInformation( SgType* type,
                                                               bool resolve_class_names,
                                                               bool array_to_pointer
                                                             )
{
  ROSE_ASSERT(type);

  IndirectionHandler::ResultType indir_desc = indirections(type);
  SgType*                        base_type = indir_desc.first;
  AddressDesc                    addrDesc = indir_desc.second;
	std::string                    type_string = type->class_name();
	std::string                    base_type_string;

	if (addrDesc.levels > 0)
		base_type_string = base_type -> class_name();

	// convert SgClassType to the mangled_name, if asked
	if( resolve_class_names ) {
		if( isSgClassType( type ))
			type_string
				=	isSgClassDeclaration(
						isSgClassType( type ) -> get_declaration()
					) -> get_mangled_name();

    if( addrDesc.levels > 0 && isSgClassType( base_type ))
			base_type_string
				=	isSgClassDeclaration(
						isSgClassType( base_type ) -> get_declaration()
					) -> get_mangled_name();
	}

	// convert array types to pointer types, if asked
  if (array_to_pointer && type_string == "SgArrayType")
    type_string = "SgPointerType";

  SgExprListExp*          initexpr = buildExprListExp();

  appendExpression( initexpr, buildStringVal( type_string ));
  appendExpression( initexpr, buildStringVal( base_type_string ));
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
RtedTransformation::appendDimensions(SgExprListExp* arg_list, RtedArray* arr)
{
      ROSE_ASSERT(arg_list && arr);

      SgExprListExp* dimargs = buildExprListExp();
		  SgExpression*  noDims = SageBuilder::buildIntVal( arr->getDimension() );

			SageInterface::appendExpression(dimargs, noDims);
			BOOST_FOREACH( SgExpression* expr, arr->getIndices() ) {
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

      std::cerr << dimexpr->unparseToString() << std::endl;
      SageInterface::appendExpression(arg_list, dimexpr);
}

void RtedTransformation::appendDimensionsIfNeeded( SgExprListExp* arg_list, RtedClassElement* rce )
{
  RtedArray* arr = rce->get_array();

  if (arr == NULL) return;
  appendDimensions(arg_list, arr);
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
			   //			   buildSizeOfOp( buildIntVal(8))
      }
#endif
    else if (szop)
    {
        szexp = buildSizeOfOp(deepCopy(szop));
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
    //		cerr << " $$$$$ Traversing nodes : " << _n->class_name() << endl;
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


static
SgExpression* genAddressOf(SgExpression* const exp, bool upcShared)
{
  // \pp \todo why can the expression be NULL?
  if (exp == NULL) return buildLongIntVal(0);

  // \pp I am not convinced that we need to strip cast expressions ...
  //     what's so bad about casting a cast?
  LValueVisitor       make_lvalue_visitor;
  SgExpression* const exp_copy = make_lvalue_visitor.visit_subtree( deepCopy(exp) );
  SgType*             char_type = buildCharType();

  // in UPC we must not cast away the shared modifer
  // \pp \todo replace the magic -1 with sth more meaningful
  //           also in Cxx_Grammar and SageBuilder
  if (upcShared) char_type = buildUpcSharedType(char_type, -1);

  SgType*             char_ptr = buildPointerType(char_type);

  // we return the address as [shared] char*
  return buildCastExp(buildAddressOfOp(exp_copy), char_ptr);
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


/*
 * Appends the address of exp.  If exp is ++, +=, -- or -=, the address of the
 * pointer after the assignment is appended to arg_list.
 */
void RtedTransformation::appendAddress(SgExprListExp* arg_list, SgExpression* exp)
{
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

    // get the address of exp
    std::cerr << "**** " << typeid(*exp->get_type()).name() << std::endl;

    const bool    upcShared = isUpcShared(exp->get_type());
    SgExpression* arg = genAddressOf(exp, upcShared);

    // add in offset if exp points to a complex data struture
    if( offset != NULL )
    {
        ROSE_ASSERT( exp != NULL );
        arg = new SgAddOp( exp -> get_file_info(),
                           arg,
                           new SgMultiplyOp( exp -> get_file_info(),
                                             offset,
                                             buildSizeOfOp( exp )
                                           )
                         );
    }

    appendExpression( arg_list, mkAddress(arg, upcShared) );
}


void RtedTransformation::appendClassName( SgExprListExp* arg_list, SgType* type )
{
    SgType* basetype = skip_ArrPtrType(type);

    if (basetype != type)
    {
      appendClassName( arg_list, basetype );
    }
    else if( isSgClassType( type ))
    {
      SgClassDeclaration* classdecl = isSgClassDeclaration( isSgClassType( type ) -> get_declaration() );

      appendExpression( arg_list, buildStringVal(classdecl->get_mangled_name() ));
    }
    else
    {
      appendExpression( arg_list, buildStringVal( "" ));
    }
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
RtedTransformation::buildGlobalConstructor(SgScopeStatement* scope, std::string name) {
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

  SgFunctionParameterList* param =
    buildFunctionParameterList (buildFunctionParameterTypeList());
  ROSE_ASSERT(param);
  ROSE_ASSERT(def->get_scope());
  ROSE_ASSERT(def->get_parent()); // should be the declaration
  ROSE_ASSERT(isSgClassDefinition(def));
  SgMemberFunctionDeclaration* mf =
    buildDefiningMemberFunctionDeclaration("RtedGlobal_"+name,buildVoidType(),param,def);
  ROSE_ASSERT(mf);
  SgSpecialFunctionModifier& fmod = mf->get_specialFunctionModifier();
  fmod.setConstructor();
  SgFunctionDefinition* fdef = mf->get_definition();
  ROSE_ASSERT(fdef);
  SgBasicBlock* block = fdef->get_body();
  ROSE_ASSERT(block);

  // append memberfunc decl to classdef
  prependStatement(mf,def);

  // build the call to RtedGlobal: RtedGlobal rg;
  globalConstructorVariable = buildVariableDeclaration("rg_"+name,decl->get_type(),NULL,scope);
  ROSE_ASSERT(globalConstructorVariable);


  return block;
}

bool traverseAllChildrenAndFind(SgExpression* varRef, SgStatement* stmt)
{
  if (stmt==NULL) return false;

  const SgNodePtrList           nodes = NodeQuery::querySubTree(stmt,V_SgExpression);
  SgNodePtrList::const_iterator zz = nodes.end();
  SgNodePtrList::const_iterator pos = std::find(nodes.begin(), zz, varRef);

  return (pos != zz);
}

bool traverseAllChildrenAndFind(SgInitializedName* varRef, SgStatement* stmt)
{
  // \pp \todo replace this and the previous function with isAncestorOf implementation
  if (stmt == NULL) return false;

  const SgNodePtrList           nodes = NodeQuery::querySubTree(stmt,V_SgInitializedName);
  SgNodePtrList::const_iterator zz = nodes.end();
  SgNodePtrList::const_iterator pos = std::find(nodes.begin(), zz, varRef);

  return (pos != zz);
}

SgBasicBlock*
RtedTransformation::appendToGlobalConstructor(SgScopeStatement* scope, std::string name) {
  // if we do not have a globalConstructor yet, build one
  ROSE_ASSERT(scope);
  // if (globalFunction==NULL) {
  globalFunction = buildGlobalConstructor(scope,name);
    //  }
  ROSE_ASSERT(globalFunction);
  // return the last statement in the globalConstructor
  // so that the new statement can be appended
  SgBasicBlock* gl_scope = isSgBasicBlock(globalFunction);
  //cerr << " gl_scope = " << globalFunction->get_scope()->class_name()<<endl;
  ROSE_ASSERT(gl_scope);
  return gl_scope;
}

void
RtedTransformation::appendGlobalConstructor(SgScopeStatement* /*scope*/,
					    SgStatement* stmt) {
  // add the global constructor to the top of the file
  //ROSE_ASSERT(globalConstructorVariable);
  ROSE_ASSERT(globConstructor);
  //prependStatement(globalConstructorVariable, scope);
  // prependStatement(globConstructor, scope);
  ROSE_ASSERT(stmt);
  insertStatement(stmt, globConstructor, true);
  //insertStatement(stmt, globConstructor, false);

}

void
RtedTransformation::appendGlobalConstructorVariable(SgScopeStatement* /*scope*/,
					    SgStatement* stmt) {
  // add the global constructor to the top of the file
  ROSE_ASSERT(globalConstructorVariable);
  ROSE_ASSERT(stmt);
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
	//	  classStmt->get_file_info()->isCompilerGenerated() << endl;
	if (classname==classDeclName)
	  var=vard;
      }
    }
  }

  return var;

}

#endif
