// vim:et sta sw=4 ts=4:
#include <rose.h>

// DQ (2/9/2010): Testing use of ROE to compile ROSE.
#ifndef USE_ROSE

#include <string>
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
SgStatement* getSurroundingStatement(SgNode* n)
{
  ROSE_ASSERT(n);

  while (!isSgStatement(n) && !isSgProject(n))
  {
    //~ if (stat->get_parent() == NULL)
    //~ {
    //~   cerr << " No parent possible for : " << n->unparseToString()
    //~        <<"  :" << stat->unparseToString() << endl;
    //~ }

    n = n->get_parent();
    ROSE_ASSERT(n);
  }

  return isSgStatement(n);
}

SgAggregateInitializer* genAggregateInitializer(SgExprListExp* initexpr, SgType* type)
{
  SgAggregateInitializer* res = buildAggregateInitializer(initexpr, type);

  // not sure whether the explicit braces flag needs to be set
  res->set_need_explicit_braces(true);

  return res;
}


SgExprStatement* checkBeforeStmt(SgStatement* stmt, SgFunctionSymbol* checker, SgExprListExp* args)
{
  ROSE_ASSERT(stmt && checker && args);

  SgFunctionRefExp*  funRef = buildFunctionRefExp(checker);
  SgFunctionCallExp* funcCallExp = buildFunctionCallExp(funRef, args);
  SgExprStatement*   exprStmt = buildExprStatement(funcCallExp);

  insertStatementBefore(stmt, exprStmt);

  return exprStmt;
}

SgExprStatement* checkBeforeStmt(SgStatement* stmt, SgFunctionSymbol* checker, SgExprListExp* args, const std::string& comment)
{
  SgExprStatement* exprStmt = checkBeforeStmt(stmt, checker, args);

  attachComment(exprStmt, "", PreprocessingInfo::before);
  attachComment(exprStmt, comment, PreprocessingInfo::before);

  return exprStmt;
}

SgExprStatement*
checkBeforeParentStmt(SgExpression* checked_node, SgFunctionSymbol* checker, SgExprListExp* args)
{
  return checkBeforeStmt(getSurroundingStatement(checked_node), checker, args);
}


//
//

SgCastExp*
RtedTransformation::ctorTypeDesc(SgAggregateInitializer* exp) const
{
  ROSE_ASSERT(exp->get_initializers()->get_expressions().size() == 3);

  return buildCastExp(exp, roseTypeDesc(), SgCastExp::e_C_style_cast);
}

SgCastExp*
RtedTransformation::ctorTypeDescList(SgAggregateInitializer* exp) const
{
  SgType* type = buildArrayType(roseTypeDesc(), NULL);

  return buildCastExp(exp, type, SgCastExp::e_C_style_cast);
}

SgCastExp*
RtedTransformation::ctorSourceInfo(SgAggregateInitializer* exp) const
{
  ROSE_ASSERT(exp->get_initializers()->get_expressions().size() == 3);

  return buildCastExp(exp, roseFileInfo(), SgCastExp::e_C_style_cast);
}


SgAggregateInitializer*
RtedTransformation::mkAddressDesc(size_t indirection_level) const
{
  SgExprListExp* initexpr = buildExprListExp();

  appendExpression(initexpr, buildIntVal(indirection_level));
  appendExpression(initexpr, buildIntVal(0 /* upc_shared_mask */));

  return genAggregateInitializer(initexpr, roseAddressDesc());
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

SgPointerType* RtedTransformation::isUsableAsSgPointerType( SgType* type ) {
    return isSgPointerType( resolveReferencesAndTypedefs( type ));
}

SgArrayType* RtedTransformation::isUsableAsSgArrayType( SgType* type ) {
    return isSgArrayType( resolveReferencesAndTypedefs( type ));
}

SgReferenceType* RtedTransformation::isUsableAsSgReferenceType( SgType* type ) {
    return isSgReferenceType( resolveTypedefs( type ));
}

/**
 * Follow the base type of @c type until we reach a non-typedef.
 */
SgType* RtedTransformation::resolveTypedefs( SgType* type ) {
     if( isSgTypedefType( type ))
        return resolveTypedefs(
            isSgTypedefType( type ) -> get_base_type() );

     return type;
}

/**
 * Follow the base type of @c type until we reach a non-typedef, non-reference.
 */
SgType* RtedTransformation::resolveReferencesAndTypedefs( SgType* type ) {
     if( isSgTypedefType( type )) {
        return resolveReferencesAndTypedefs(
            isSgTypedefType( type ) -> get_base_type() );
    } else if( isSgReferenceType( type )) {
        // resolve reference to reference ... to pointer
        return resolveReferencesAndTypedefs(
            isSgReferenceType( type ) -> get_base_type() );
    }

    return type;
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
    // \note \tmp \pp the next linw was moved in, might not be needed
    //                for calls from ExecuteTRansformations
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

bool RtedTransformation::isNormalScope( SgNode* n ) {
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

SgVarRefExp*
RtedTransformation::buildVarRef( SgInitializedName *&initName ) {
    return buildVarRefExp(
        isSgVariableSymbol(
            initName -> get_scope()
                -> lookup_symbol( initName -> get_name() )));
}


std::string
RtedTransformation::removeSpecialChar(std::string str) {
  string searchString="\"";
  string replaceString="'";
  string::size_type pos = 0;
  while ( (pos = str.find(searchString, pos)) != string::npos ) {
    str.replace( pos, searchString.size(), replaceString );
    pos++;
  }
  return str;
}


std::string
RtedTransformation::getMangledNameOfExpression(SgExpression* expr) {
  string manglName="";
  // look for varRef in the expr and return its mangled name
  Rose_STL_Container< SgNode * > var_refs = NodeQuery::querySubTree(expr,V_SgVarRefExp);
  if (var_refs.size()==0) {
    // there should be at least one var ref
    cerr << " getMangledNameOfExpression: varRef on left hand side not found : " << expr->unparseToString() << endl;
  } else if (var_refs.size()==1) {
    // correct, found on var ref
    SgVarRefExp* varRef = isSgVarRefExp(*(var_refs.begin()));
    ROSE_ASSERT(varRef && varRef->get_symbol()->get_declaration());
    manglName = varRef->get_symbol()->get_declaration()->get_mangled_name();
    cerr << " getMangledNameOfExpression: found varRef: " << manglName << endl;
  } else if (var_refs.size()>1) {
    // error
    cerr << " getMangledNameOfExpression: Too many varRefs on left hand side : " << var_refs.size() << endl;
  }
  return manglName;
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


// we have some expression, e.g. a var ref to m, but we want as much of the
// expression as necessary to properly refer to m
//
//  e.g. &foo.bar.m, instead of the erroneous &m
SgExpression* RtedTransformation::getUppermostLvalue( SgExpression* exp ) {
	if (exp)
	cerr << "Checking if exp has parent: " << exp->unparseToString() << "  :" << endl;
	if(exp==NULL || exp->get_parent()==NULL) return exp;
	SgExpression* parent = isSgExpression( exp->get_parent() );

    while(  parent
            && (isSgDotExp( parent ))) {

        // in c++, the rhs of the dot may be a member function, which we'd want
        // to ignore
        SgExpression* rhs = isSgDotExp( parent ) -> get_rhs_operand();
        if( isSgMemberFunctionRefExp( rhs ))
            // in, e.g.
            //  foo.bar = 9001
            //  foo.bar.=( 9001 )
            // we want to stop at the dot exp
            //  foo.bar
            break;

        exp = parent;
        parent = isSgExpression( parent->get_parent() );
    }

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

int RtedTransformation::getDimension(SgInitializedName* initName) {
  ROSE_ASSERT(initName);
  int dimension = 0;
  SgType* type = initName->get_type();
  ROSE_ASSERT(type);
  if (isSgArrayType(type)) {
    dimension++;
  } else {
    while (isSgPointerType(type) && !isSgPointerMemberType(type)) {
      SgPointerType* pointer = isSgPointerType(type);
      ROSE_ASSERT(pointer);
      type = pointer->dereference();
      ROSE_ASSERT(type);
      dimension++;
      ROSE_ASSERT(dimension<10);
      //cerr << "Dimension : " << dimension << "  : " << type->class_name() << endl;
    }
  }
  return dimension;
}

int
RtedTransformation::getDimension(SgInitializedName* initName, SgVarRefExp* varRef) {
  int dim =-1;
  std::map<SgVarRefExp*, RTedArray*>::const_iterator it = create_array_define_varRef_multiArray.begin();
  for (;it!=create_array_define_varRef_multiArray.end();++it) {
    RTedArray* array = it->second;
    SgInitializedName* init = array->initName;
    if (init==initName) {
      dim=array->getDimension();
      cerr << "Found init : " << init->unparseToString() << " dim : " << dim << "  compare to : " << initName->unparseToString()<<endl;
    }
  }

  std::map<SgInitializedName*, RTedArray*>::const_iterator it2= create_array_define_varRef_multiArray_stack.find(initName);
  for (;it2!=create_array_define_varRef_multiArray_stack.end();++it2) {
    RTedArray* array = it2->second;
    SgInitializedName* init = array->initName;
    if (init==initName) {
      dim=array->getDimension();
    }
  }
  cerr << " -------------------------- resizing dimension to : " << dim << "  for : " << varRef->unparseToString() << endl;
  return dim;
}



bool RtedTransformation::isGlobalExternVariable(SgStatement* stmt) {
  bool externQual =false;
#if 1
  SgDeclarationStatement* declstmt = isSgDeclarationStatement(stmt);
  SgFunctionParameterList* funcparam = isSgFunctionParameterList(stmt);
  if (funcparam) {
    SgFunctionDeclaration* funcdeclstmt = isSgFunctionDeclaration(funcparam->get_parent());
    ROSE_ASSERT(funcdeclstmt);
    externQual = funcdeclstmt->get_declarationModifier().get_storageModifier().isExtern();
    cerr << ">>>>>>>>>>>>>>>> stmt-param : " << funcdeclstmt->unparseToString() << "  " << funcdeclstmt->class_name() <<
      "  " << externQual << endl;
  } else if (declstmt) {
    externQual = declstmt->get_declarationModifier().get_storageModifier().isExtern();
  }
  cerr << ">>>>>>>>>>>>>>>> stmt : " << stmt->unparseToString() << "  " << stmt->class_name() << endl;
#endif
  return externQual;
}


/*************************************************
 * This function adds some very common arguments
 * to instrumented function calls for constructing
 * SourcePosition objects.
 ************************************************/
void RtedTransformation::appendFileInfo( SgExprListExp* arg_list, SgNode* node)
{
    ROSE_ASSERT( node );
    appendFileInfo( arg_list, node->get_file_info() );
}

void RtedTransformation::appendFileInfo( SgExprListExp* arg_list, Sg_File_Info* file_info)
{
    SgExpression*           filename = buildStringVal( file_info->get_filename() );
    SgExpression*           linenr = buildIntVal( file_info->get_line() );
    SgExpression*           linenrTransformed = buildOpaqueVarRefExp("__LINE__");
    SgExprListExp*          fiArgs = buildExprListExp();

    appendExpression( fiArgs, filename );
    appendExpression( fiArgs, linenr );
    appendExpression( fiArgs, linenrTransformed );

    SgAggregateInitializer* fiCtorArg = genAggregateInitializer(fiArgs, roseFileInfo());

    appendExpression( arg_list, ctorSourceInfo(fiCtorArg) );
}


SgType* get_arrptr_base(SgType* t)
{
  SgPointerType* sgptr = isSgPointerType(t);
  if (sgptr != NULL) return sgptr;

  SgArrayType*   sgarr = isSgArrayType(t);
  if (sgarr != NULL) return sgarr;

  return t;
}


/// \brief  counts the number of indirection layers (i.e., Arrays, Pointers)
///         between a type and the first non indirect type.
/// \return A type and the length of indirections
static
std::pair<SgType*, size_t>
indirections(SgType* base_type)
{
  std::pair<SgType*, size_t> res(base_type, 0);

  base_type = get_arrptr_base(base_type);
  while (base_type != res.first)
  {
    res.first = base_type;
    ++res.second;

    base_type = get_arrptr_base(base_type);
  }

  return res;
}


SgAggregateInitializer* RtedTransformation::mkTypeInformation(SgInitializedName* initName) {
    // \pp was: if ( !initName ) return;
    ROSE_ASSERT(initName != NULL);

    return mkTypeInformation( initName, initName -> get_type() );
}

SgAggregateInitializer* RtedTransformation::mkTypeInformation(SgInitializedName* initName, SgType* type ) {
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

  // we always resolve reference type information
  if (isSgReferenceType( type )) {
      return mkTypeInformation( isSgReferenceType( type ) -> get_base_type(),
                                resolve_class_names,
                                array_to_pointer
                              );
  }

  std::pair<SgType*, size_t> indir_desc = indirections(type);
  SgType*                    base_type = indir_desc.first;
  size_t                     indirection_level = indir_desc.second;
	std::string                type_string = type->class_name();
	std::string                base_type_string;

	if (indirection_level > 0)
		base_type_string = base_type -> class_name();

	// convert SgClassType to the mangled_name, if asked
	if( resolve_class_names ) {
		if( isSgClassType( type ))
			type_string
				=	isSgClassDeclaration(
						isSgClassType( type ) -> get_declaration()
					) -> get_mangled_name();
		if( indirection_level > 0 && isSgClassType( base_type ))
			base_type_string
				=	isSgClassDeclaration(
						isSgClassType( base_type ) -> get_declaration()
					) -> get_mangled_name();
	}

	// convert array types to pointer types, if asked
  if (array_to_pointer && type_string == "SgArrayType")
    type_string = "SgPointerType";

  SgExprListExp*             initexpr = buildExprListExp();

  appendExpression( initexpr, buildStringVal( type_string ));
  appendExpression( initexpr, buildStringVal( base_type_string ));
  appendExpression( initexpr, mkAddressDesc(indirection_level) );

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
RtedTransformation::appendDimensions(SgExprListExp* arg_list, RTedArray* arr)
{
      ROSE_ASSERT(arg_list && arr);

		  SgExpression* noDims = SageBuilder::buildIntVal( arr->getDimension() );

			SageInterface::appendExpression(arg_list, noDims);
			BOOST_FOREACH( SgExpression* expr, arr->getIndices() ) {
          if ( expr == NULL )
          {
              // \pp ???
              expr = SageBuilder::buildIntVal( -1 );
          }

          ROSE_ASSERT( expr );
          SageInterface::appendExpression( arg_list, expr );
      }
}

void RtedTransformation::appendDimensionsIfNeeded( SgExprListExp* arg_list,
                                                   RtedClassElement* rce
                                                 )
{
  RTedArray* arr = rce->get_array();

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
    SgExpression* op = exp;
    SgExpression* szexp = NULL;

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
    if (isSgPlusPlusOp( op ) || isSgMinusMinusOp( op ))
    {
      op = isSgUnaryOp( op ) -> get_operand();
    }
    else if( isSgPlusAssignOp( op ) || isSgMinusAssignOp( op ))
    {
      op = isSgBinaryOp( op ) -> get_lhs_operand();
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
    if( (am == Complex) && isSgArrayType( type ))
    {
        SgTreeCopy copy;
        SgNode*    clone = isSgArrayType(type)->get_base_type()->copy( copy );

        szexp = buildSizeOfOp(isSgType(clone));
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
    else if (op)
    {
        SgTreeCopy copy;

        szexp = buildSizeOfOp(isSgExpression( op -> copy( copy )));
    }
    else
    {
        szexp = buildSizeOfOp(buildLongIntVal(0));
    }

    ROSE_ASSERT(szexp != NULL);

    appendAddress( arg_list, exp );
    appendExpression( arg_list, szexp );
}

// we have to make exp a valid lvalue
class LValueVisitor : public AstSimpleProcessing
{
public:
  SgExpression* rv;
  bool first_node;

  SgExpression* visit_subtree( SgNode* n ) {
    first_node = true;
    rv = NULL;

    traverse( n, preorder );

    // if the first node we visit is an SgCastExp, we want to
    // return something else.  Otherwise, if we've only modified
    // proper subtrees of n, n will have been appropriately modified
    // and we can simply return it.
    return rv
           ? rv
           : isSgExpression( n );
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
SgExpression* rted_AddrOf(SgExpression* const exp)
{
  // \why can the expression be NULL? (PP)
  if (exp == NULL) return buildLongIntVal(0);

  LValueVisitor       make_lvalue_visitor;
  SgTreeCopy          copy;
  SgExpression* const exp_copy = make_lvalue_visitor.visit_subtree( exp -> copy( copy ));

  // needed for UPC, where we cannot cast a shared pointer directly to
  // an integer value.
  SgExpression* const cast_void_star = buildCastExp( buildAddressOfOp(exp_copy),
                                                     buildPointerType(buildVoidType())
                                                   );

  // \note I believe that the type of the following cast expression should be
  //       size_t instead of unsigned long (PP).
  return buildCastExp(cast_void_star, buildUnsignedLongType());
}


/*
 * Appends the address of exp.  If exp is ++, +=, -- or -=, the address of the
 * pointer after the assignment is appended to arg_list.
 */
void RtedTransformation::appendAddress(SgExprListExp* arg_list, SgExpression* exp) {
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

    //    if (exp)
    //cerr << " exp to be added as address : " << exp->unparseToString() << endl;

    // get the address of exp
    SgExpression* arg = rted_AddrOf(exp);

    // add in offset if exp points to a complex data struture
    if( offset != NULL ) {
        ROSE_ASSERT( exp != NULL );
        arg = new SgAddOp(
            exp -> get_file_info(), arg,
            new SgMultiplyOp(
                exp -> get_file_info(),
                offset,
                buildSizeOfOp( exp )
            )
        );
    }

    appendExpression( arg_list, arg );
}


void RtedTransformation::appendBaseType( SgExprListExp* arg_list, SgType* type ) {
	SgType* base_type = NULL;

	SgArrayType* arr = isSgArrayType( type );
	SgPointerType* ptr = isSgPointerType( type );

	if( arr )
		base_type = arr -> get_base_type();
	else if ( ptr )
		base_type = ptr -> get_base_type();

	if( base_type )
        appendExpression(arg_list, buildStringVal(
			base_type -> class_name()
		));
	else
        appendExpression(arg_list, buildStringVal(""));
}


void RtedTransformation::appendClassName( SgExprListExp* arg_list, SgType* type ) {

    if( isSgClassType( type )) {
        appendExpression(arg_list, buildStringVal(
			isSgClassDeclaration( isSgClassType( type ) -> get_declaration() )
				-> get_mangled_name() )
		);
    } else if( isSgArrayType( type )) {

        appendClassName( arg_list, isSgArrayType( type ) -> get_base_type() );

    } else if( isSgPointerType( type )) {

        appendClassName( arg_list, isSgPointerType( type ) -> get_base_type() );

    } else {

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
        SgExprStatement* test = isSgExprStatement( gfor_loop_test(for_stmt) );
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

bool
RtedTransformation::traverseAllChildrenAndFind(SgExpression* varRef,
					       SgStatement* stmt) {
  bool found =false;
  if (stmt==NULL)
    return found;
  Rose_STL_Container< SgNode * > nodes =
    NodeQuery::querySubTree(stmt,V_SgExpression);
  Rose_STL_Container< SgNode * >::const_iterator it = nodes.begin();
  for (;it!=nodes.end();++it) {
    SgExpression* var = isSgExpression(*it);
    ROSE_ASSERT(var);
    if (var==varRef) {
      found=true;
      break;
    }
  }
  return found;
}

bool
RtedTransformation::traverseAllChildrenAndFind(SgInitializedName* varRef,
					       SgStatement* stmt) {
  bool found =false;
  if (stmt==NULL)
    return found;
  Rose_STL_Container< SgNode * > nodes =
    NodeQuery::querySubTree(stmt,V_SgInitializedName);
  Rose_STL_Container< SgNode * >::const_iterator it = nodes.begin();
  for (;it!=nodes.end();++it) {
    SgInitializedName* var = isSgInitializedName(*it);
    ROSE_ASSERT(var);
    if (var==varRef) {
      found=true;
      break;
    }
  }
  return found;
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
  Rose_STL_Container<SgDeclarationStatement*> decls = gl->get_declarations();
  Rose_STL_Container<SgDeclarationStatement*>::const_iterator it = decls.begin();
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
