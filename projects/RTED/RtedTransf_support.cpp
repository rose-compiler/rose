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
const std::string &RtedForStmtProcessed::Key = std::string( "Rted::ForStmtProcessed" );


/* -----------------------------------------------------------
 * Helper Function
 * -----------------------------------------------------------*/
SgStatement*
RtedTransformation::getStatement(SgExpression* exp) {
  SgStatement* stmt = NULL;
  SgNode* expr = exp;
  while (!isSgStatement(expr) && !isSgProject(expr))
    expr = expr->get_parent();
  if (isSgStatement(expr))
    stmt = isSgStatement(expr);
  return stmt;
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


SgDeclarationStatementPtrList& RtedTransformation::appendConstructors(
        SgClassType* type,  SgDeclarationStatementPtrList& constructors) {

    SgClassDeclaration* decl
        = isSgClassDeclaration(
                type -> get_declaration() -> get_definingDeclaration() );
    ROSE_ASSERT( decl );

    SgClassDefinition* cdef = decl -> get_definition();
    ROSE_ASSERT( cdef );

    return appendConstructors( cdef, constructors );
}

SgDeclarationStatementPtrList& RtedTransformation::appendConstructors(
        SgClassDefinition* cdef,  SgDeclarationStatementPtrList& constructors) {


    SgDeclarationStatementPtrList& members = cdef -> get_members();
    BOOST_FOREACH( SgDeclarationStatement* member, members ) {
        SgMemberFunctionDeclaration* mfun 
            = isSgMemberFunctionDeclaration(
                member -> get_definingDeclaration() );

        if( isConstructor( mfun ))
            constructors.push_back( mfun );
    }

    return constructors;
}

bool RtedTransformation::isConstructor( SgFunctionDeclaration* fndecl ) {
    SgMemberFunctionDeclaration* mfun = isSgMemberFunctionDeclaration( fndecl );
    if( !mfun )
        // non member functions certainly aren't constructors
        return false;

    SgClassDeclaration* cdef = mfun -> get_associatedClassDeclaration();

    // a constructor is a member function whose name matches that 
    return (
        mfun -> get_mangled_name() == cdef -> get_mangled_name()
        // SgClassDefinition.get_mangled_name can return the
        // non-mangled name!
        || mfun -> get_name() == cdef -> get_mangled_name()
    );
}

bool RtedTransformation::hasNonEmptyConstructor( SgClassType* type ) {
    SgDeclarationStatementPtrList constructors;
    appendConstructors( type, constructors );

    return constructors.size() > 0;
}

bool RtedTransformation::isNormalScope( SgNode* n ) {
	return	isSgBasicBlock( n )
			|| isSgIfStmt( n )
			|| isSgWhileStmt( n )
			|| isSgDoWhileStmt( n )
			|| isSgForStatement( n );
}

bool
RtedTransformation::isInInstrumentedFile( SgNode* n ) {
	ROSE_ASSERT( n );
	std::string file_name = n -> get_file_info() -> get_filename();
	return !(
		rtedfiles -> find( file_name ) == rtedfiles -> end() 
	);
}

SgVarRefExp*
RtedTransformation::buildVarRef( SgInitializedName *&initName ) {
    return buildVarRefExp(
        isSgVariableSymbol(
            initName -> get_scope() 
                -> lookup_symbol( initName -> get_name() )));
}

SgExpression*
RtedTransformation::buildString(std::string name) {
  //  SgExpression* exp = buildCastExp(buildStringVal(name),buildPointerType(buildCharType()));
  SgExpression* exp = buildStringVal(name);
  return exp;
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

/****************************************
 * This function returns the statement that
 * surrounds a given Node or Expression
 ****************************************/
SgStatement*
RtedTransformation::getSurroundingStatement(SgNode* n) {
  SgNode* stat = n;
  while (!isSgStatement(stat) && !isSgProject(stat)) {
    if (stat->get_parent()==NULL) {
      cerr << " No parent possible for : " << n->unparseToString()
	   <<"  :" << stat->unparseToString() << endl;
    }
    ROSE_ASSERT(stat->get_parent());
    stat = stat->get_parent();
  }
  return isSgStatement(stat);
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
void RtedTransformation::appendFileInfo( SgNode* node, SgExprListExp* arg_list) {
    ROSE_ASSERT( node );
    appendFileInfo( node->get_file_info(), arg_list );
}
void RtedTransformation::appendFileInfo( Sg_File_Info* file_info, SgExprListExp* arg_list) {

    SgExpression* filename = buildString( file_info->get_filename() );
    SgExpression* linenr = buildString( RoseBin_support::ToString( file_info->get_line() ));
    SgExpression* linenrTransformed = buildString("x%%x");

    appendExpression( arg_list, filename );
    appendExpression( arg_list, linenr );
    appendExpression( arg_list, linenrTransformed );
}


void RtedTransformation::appendTypeInformation( SgInitializedName* initName, SgExprListExp* arg_list ) {
    if( !initName ) return;

    appendTypeInformation( initName, initName -> get_type(), arg_list );
}
void RtedTransformation::appendTypeInformation( SgInitializedName* initName, SgType* type, SgExprListExp* arg_list ) {
    // arrays in parameters are actually passed as pointers, so we shouldn't
    // treat them as stack arrays
	bool array_to_pointer 
		=	initName
            && isSgFunctionParameterList( getSurroundingStatement( initName ))
            &&  type->class_name() == "SgArrayType";

	appendTypeInformation( type, arg_list, false, array_to_pointer );
}

void RtedTransformation::appendTypeInformation( SgType* type, SgExprListExp* arg_list, bool resolve_class_names, bool array_to_pointer ) {
    ROSE_ASSERT(type);

    // we always resolve reference type information
    if( isSgReferenceType( type )) {
        appendTypeInformation(
            isSgReferenceType( type ) -> get_base_type(),
            arg_list,
            resolve_class_names,
            array_to_pointer );
        return;
    }

    SgType* base_type = NULL;
    size_t indirection_level = 0;

    base_type = type;
    while( true ) {
        if (isSgPointerType( base_type )) {
            base_type = isSgPointerType( base_type )->get_base_type();
            ++indirection_level;
        } else if( isSgArrayType( base_type )) {
            base_type = isSgArrayType( base_type )->get_base_type();
            ++indirection_level;
        } else break;
	}


	std::string type_string = type -> class_name();
	std::string base_type_string = "";
	if( indirection_level > 0 )
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
    if(	array_to_pointer &&  type_string == "SgArrayType" )
        type_string =  "SgPointerType";

    appendExpression( arg_list, buildString( type_string ));
    appendExpression( arg_list, buildString( base_type_string ));
    appendExpression( arg_list, buildIntVal( indirection_level ));
}

void RtedTransformation::appendAddressAndSize(
                         //   SgInitializedName* initName,
					      SgScopeStatement* scope,
                            SgExpression* varRefE,
                            SgExprListExp* arg_list,
					      int appendType,
					      SgClassDefinition* UnionClass) {

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
    SgType* type  = NULL;
    if (varRefE) type=varRefE -> get_type();
    appendAddressAndSize( exp, type, arg_list, appendType,UnionClass );
}

void RtedTransformation::appendAddressAndSize(
                            SgExpression* exp,
                            SgType* type,
                            SgExprListExp* arg_list,
                            int appendType,
					      SgClassDefinition* unionclass) {

    appendAddress( arg_list, exp );

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
    if(     isSgPlusPlusOp( exp )
            || isSgMinusMinusOp( exp )) {

        exp = isSgUnaryOp( exp ) -> get_operand();
    } else if( isSgPlusAssignOp( exp )
                || isSgMinusAssignOp( exp )) {

        exp = isSgBinaryOp( exp ) -> get_lhs_operand();
    }

    SgTreeCopy copy;
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
    if( appendType & 2 && isSgArrayType( type )) {
        appendExpression(
            arg_list,
            buildSizeOfOp(
                isSgType(
                    isSgArrayType( type ) 
                        -> get_base_type() -> copy( copy )))
        );
    } else {
#if 0
      if (unionclass) {
	cerr <<"isUnionClass" << endl;
	SgType* classtype = unionclass ->get_declaration()->get_type();
	cerr <<" unionclass : " << unionclass->unparseToString()<<"  type: " << classtype->class_name() << endl;

          appendExpression(
			   arg_list,
			   buildSizeOfOp( isSgType(classtype-> copy( copy )))
			   //			   buildSizeOfOp( buildIntVal(8))
			   );
      } else
#endif

    	if (exp)
          appendExpression(
            arg_list,
            buildSizeOfOp( isSgExpression( exp -> copy( copy )))
        );
    	else
          appendExpression(
                arg_list,
                buildSizeOfOp( isSgExpression( buildLongLongIntVal(0)))
            );

    }
}

/*
 * Appends the address of exp.  If exp is ++, +=, -- or -=, the address of the
 * pointer after the assignment is appended to arg_list.
 */
void RtedTransformation::appendAddress(SgExprListExp* arg_list, SgExpression* exp) {
    SgTreeCopy copy;


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


    // we have to make exp a valid lvalue
    class anon : public AstSimpleProcessing {
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
                } 
		else{
		  //replaceExpression( n, n -> get_operand() );
                }
            }
    } make_lvalue_visitor;

    SgExpression* copy_exp = NULL;
    //    if (exp)
    //cerr << " exp to be added as address : " << exp->unparseToString() << endl;
    if (exp)
      copy_exp = make_lvalue_visitor.visit_subtree(  exp -> copy( copy ));

    SgExpression *arg;
    SgCastExp *cast_op = NULL;
    if (exp)
     cast_op=   buildCastExp(
            buildAddressOfOp( copy_exp ),
            buildUnsignedLongLongType()
        );
    else
        cast_op=   buildCastExp(
               buildLongLongIntVal( 0),
               buildUnsignedLongLongType()
           );

    if( offset != NULL ) {
        ROSE_ASSERT( exp != NULL );
        arg = new SgAddOp(
            exp -> get_file_info(), cast_op,
            new SgMultiplyOp(
                exp -> get_file_info(),
                offset,
                buildSizeOfOp( exp )
            )
        );
    } else
        arg = cast_op;

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
        appendExpression(arg_list, buildString(
			base_type -> class_name()
		));
	else
        appendExpression(arg_list, buildString(""));
}

void RtedTransformation::appendClassName( SgExprListExp* arg_list, SgType* type ) {

    if( isSgClassType( type )) {
        appendExpression(arg_list, buildString(
			isSgClassDeclaration( isSgClassType( type ) -> get_declaration() )
				-> get_mangled_name() )
		);
    } else if( isSgArrayType( type )) {

        appendClassName( arg_list, isSgArrayType( type ) -> get_base_type() );

    } else if( isSgPointerType( type )) {

        appendClassName( arg_list, isSgPointerType( type ) -> get_base_type() );

    } else {

        appendExpression( arg_list, buildString( "" ));
    }
}

void RtedTransformation::prependPseudoForInitializerExpression(
		SgExpression* exp, SgForStatement* for_stmt ) {

  if (for_stmt==NULL) {
    // something went wrong!
     return;
  }

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
        SgExprStatement* test = isSgExprStatement( for_stmt -> get_test() );
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
RtedTransformation::appendGlobalConstructor(SgScopeStatement* scope,
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
RtedTransformation::appendGlobalConstructorVariable(SgScopeStatement* scope,
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
