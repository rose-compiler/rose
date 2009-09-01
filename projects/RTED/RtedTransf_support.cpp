// vim:et sta sw=4 ts=4:
#include <rose.h>
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
RtedTransformation::isUsedAsLvalue( SgExpression* exp ) {
    if( NULL == exp ) return false;

    SgExpression* ancestor = getExprBelowAssignment( exp );
    SgBinaryOp* assign = isSgBinaryOp( ancestor -> get_parent() );
    return(
        assign && assign -> get_lhs_operand() == ancestor
    );
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

SgExpression* RtedTransformation::getUppermostLvalue( SgExpression* exp ) {
    SgExpression* parent = isSgExpression( exp->get_parent() );
    
    while( parent
            && (isSgDotExp( parent )
                || isSgArrowExp( parent )
                || isSgPointerDerefExp( parent )
                || isSgPntrArrRefExp( parent ))) {
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
    SgType* base_type = NULL;
    ROSE_ASSERT(type);

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
                            SgInitializedName* initName,
                            SgExpression* varRefE,
                            SgExprListExp* arg_list,
                            int appendType ) {

    SgScopeStatement* scope = NULL;

    // FIXME 2: It would be better to explicitly handle dot and arrow
    // expressions here, rather than indirectly through the var's declared scope
    if( initName) {
        scope = initName->get_scope();
    }

    SgExpression* exp = varRefE;
    if ( isSgClassDefinition(scope) ) {

        // member -> &( var.member )
        exp = getUppermostLvalue( varRefE );
    }

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
    if( appendType & 2 && isSgArrayType( varRefE->get_type() )) {
        appendExpression(
            arg_list,
            buildSizeOfOp(
                isSgType(
                    isSgArrayType( varRefE -> get_type() ) 
                        -> get_base_type() -> copy( copy )))
        );
    } else {
        appendExpression(
            arg_list,
            buildSizeOfOp( isSgExpression( exp -> copy( copy )))
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

    SgExpression *arg;
    SgCastExp *cast_op = 
        buildCastExp(
            buildAddressOfOp( isSgExpression( exp -> copy( copy ))),
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
		for_stmt -> set_test_expr(
			// test | ...
			buildBinaryExpression< SgBitOrOp >(
				for_stmt -> get_test_expr(),
				// eval_once && ...
				buildBinaryExpression< SgAndOp >(
					buildVarRefExp( "RuntimeSystem_eval_once", new_scope ),
					initial_expression )));

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

