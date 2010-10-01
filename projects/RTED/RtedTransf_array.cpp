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



/* -----------------------------------------------------------
 * Is the Initialized Name already known as an array element ?
 * -----------------------------------------------------------*/
bool
RtedTransformation::isVarRefInCreateArray(SgInitializedName* search) {
  bool found=false;
  std::map<SgVarRefExp*, RTedArray*>::const_iterator it= 
    create_array_define_varRef_multiArray.begin();
  for (;it!=create_array_define_varRef_multiArray.end();++it) {
    //SgVarRefExp* varRef = it->first;
    RTedArray* array = it->second;
    SgInitializedName* initName = array->initName;
    if (initName==search) {
      found=true;
    }
  }
  std::map<SgInitializedName*, RTedArray*>::const_iterator it2 =  
    create_array_define_varRef_multiArray_stack.begin();
  for (;it2!=create_array_define_varRef_multiArray_stack.end();++it2) {
    SgInitializedName* initName = it2->first;
    if (initName==search) {
      found=true;
    }
  }
  return found;
}


/* -----------------------------------------------------------
 * Perform Transformation: insertArrayCreateCall
 * -----------------------------------------------------------*/
void RtedTransformation::insertArrayCreateCall(SgVarRefExp* n, RTedArray* value) {
  ROSE_ASSERT(value);
  SgInitializedName* initName = n->get_symbol()->get_declaration();
  ROSE_ASSERT(initName);
  SgStatement* stmt = value->surroundingStatement;
  if( !stmt )
      stmt = getSurroundingStatement(n);
  insertArrayCreateCall(stmt, initName, n, value);
}

void RtedTransformation::insertArrayCreateCall(SgInitializedName* initName,
                                               RTedArray* value) {
  ROSE_ASSERT(value);
  ROSE_ASSERT(initName);
  SgStatement* stmt = value->surroundingStatement;
  if( !stmt )
      stmt = getSurroundingStatement(initName);

  SgVarRefExp* var_ref = buildVarRef( initName );
  insertArrayCreateCall(stmt, initName, var_ref, value);
}

SgStatement* 
RtedTransformation::buildArrayCreateCall(SgInitializedName* initName, 
                                         SgVarRefExp* varRef,
					 RTedArray* array,
					 SgStatement* stmt) {
      // build the function call : runtimeSystem-->createArray(params); ---------------------------
  string name = initName->get_mangled_name().str();
      SgExpression* plainname = buildString(initName->get_name());
      SgExpression* callNameExp = buildString(name);

      SgExprListExp* arg_list = buildExprListExp();
      appendExpression(arg_list, plainname);
      appendExpression(arg_list, callNameExp);

      if (varRef->get_parent()!=NULL)
    	  appendTypeInformation( NULL, getExprBelowAssignment( varRef ) -> get_type(), arg_list );
      else
    	  appendTypeInformation( NULL, varRef -> get_type(), arg_list );

		SgScopeStatement* scope = NULL;
		if (initName) scope = initName->get_scope();
#if 0
	bool isUnionClass=false;
	SgClassDefinition* unionclass = isSgClassDefinition(initName->get_scope());
	if (unionclass && 
	    unionclass->get_declaration()->get_class_type()==SgClassDeclaration::e_union)
	  isUnionClass=true;
	//if (!isUnionClass) 
#endif
	SgClassDefinition* unionclass=NULL;

	    if (varRef->get_parent()!=NULL)
              appendAddressAndSize(//initName,
                                   scope, getExprBelowAssignment( varRef ), arg_list,0,unionclass);
            else
	      appendAddressAndSize(//initName,
                                   scope, varRef , arg_list,0,unionclass);


      //SgIntVal* ismalloc = buildIntVal( 0 );
      SgExpression* size = buildIntVal( 0 );
      SgExpression* fromMalloc = buildIntVal( 0 );
      if ( array -> onHeap ) {
        ROSE_ASSERT( array -> size );
        size = buildCastExp( array -> size, buildLongType() );
        // track whether heap memory was allocated via malloc or new, to ensure
        // that free/delete matches
        if( array -> fromMalloc )
            fromMalloc = buildIntVal( 1 );
      }
      appendExpression( arg_list, size );
      appendExpression( arg_list, fromMalloc );


      appendClassName( arg_list, initName -> get_type() );


      SgExpression* filename = buildString(stmt->get_file_info()->get_filename());
      SgExpression* linenr = buildString(RoseBin_support::ToString(stmt->get_file_info()->get_line()));
      appendExpression(arg_list, filename);
      appendExpression(arg_list, linenr);

      SgExpression* linenrTransformed = buildString("x%%x");
      appendExpression(arg_list, linenrTransformed);

      array -> appendDimensionInformation( arg_list );

      //      appendExpression(arg_list, buildString(stmt->unparseToString()));
      ROSE_ASSERT(symbols->roseCreateHeap);
      string symbolName2 = symbols->roseCreateHeap->get_name().str();
      //cerr << " >>>>>>>> Symbol Member: " << symbolName2 << endl;
      SgFunctionRefExp* memRef_r = buildFunctionRefExp(  symbols->roseCreateHeap);
      //SgArrowExp* sgArrowExp = buildArrowExp(varRef_l, memRef_r);

      SgFunctionCallExp* funcCallExp = buildFunctionCallExp(memRef_r,
							    arg_list);
      SgExprStatement* exprStmt = buildExprStatement(funcCallExp);
      return exprStmt;
}

void RtedTransformation::insertArrayCreateCall(SgStatement* stmt,
                                               SgInitializedName* initName, 
                                               SgVarRefExp* varRef,
					       RTedArray* array) {
  // make sure there is no extern in front of stmt
  bool externQual = isGlobalExternVariable(stmt);
  if (externQual) {
    cerr << "Skipping this insertArrayCreate because it probably occurs multiple times (with and without extern)." << endl;
    return;
  }

  ROSE_ASSERT(initName && initName->get_parent());
  ROSE_ASSERT(varRef);
  //cerr <<" varref is unparsed? : " << varRef->get_file_info()->isOutputInCodeGeneration() << endl;
 //ROSE_ASSERT(varRef->get_parent() || varRef->get_file_info()->isOutputInCodeGeneration()==false);

  bool global_stmt = false;
  std::vector<SgExpression*> value = array -> getIndices();
  //bool stack = array->stack;
  if (isSgStatement(stmt)) {
    SgScopeStatement* scope = stmt->get_scope();
    string name = initName->get_mangled_name().str();

    ROSE_ASSERT(scope);
    // what if there is an array creation within a ClassDefinition
    if ( isSgClassDefinition(scope)) {
      // new stmt = the classdef scope
      SgClassDeclaration* decl = isSgClassDeclaration(scope->get_parent());
      ROSE_ASSERT(decl);
      stmt = isSgVariableDeclaration(decl->get_parent());
      if (!stmt) {
	cerr << " Error . stmt is unknown : " << decl->get_parent()->class_name() << endl;
	ROSE_ASSERT( false );
      } 
      scope = scope->get_scope();
      // We want to insert the stmt before this classdefinition, if its still in a valid block
      cerr <<" ....... Found ClassDefinition Scope. New Scope is : " << scope->class_name() << "  stmt:" << stmt->class_name() <<endl;
    }
    // what is there is an array creation in a global scope
    else if (isSgGlobal(scope)) {
      scope = mainBody;
	  global_stmt = true;
    }
    if (isSgIfStmt(scope)) {
      SgStatement* exprStmt = buildArrayCreateCall(initName,varRef,array,stmt);      
      ROSE_ASSERT(exprStmt);
      // get the two bodies of the ifstmt and prepend to them
      cerr <<"If Statment : inserting createHeap" << endl;
      SgStatement* trueb = isSgIfStmt(scope)->get_true_body();
      SgStatement* falseb = isSgIfStmt(scope)->get_false_body();
      bool partOfTrue = traverseAllChildrenAndFind(varRef,trueb);
      bool partOfFalse = traverseAllChildrenAndFind(varRef,falseb);
      bool partOfCondition = false;
      if (partOfTrue==false && partOfFalse==false)
	partOfCondition=true;
      if (trueb && ( partOfTrue || partOfCondition)) {
	if (!isSgBasicBlock(trueb)) {
	  removeStatement(trueb);
	  SgBasicBlock* bb = buildBasicBlock();
	  bb->set_parent(isSgIfStmt(scope));
	  isSgIfStmt(scope)->set_true_body(bb);
	  bb->prepend_statement(trueb);
	  trueb=bb;
	}
	prependStatement( exprStmt,isSgScopeStatement(trueb));
      }
      if (falseb && ( partOfFalse || partOfCondition)) {
	if (!isSgBasicBlock(falseb)) {
	  removeStatement(falseb);
	  SgBasicBlock* bb = buildBasicBlock();
	  bb->set_parent(isSgIfStmt(scope));
	  isSgIfStmt(scope)->set_false_body(bb);
	  bb->prepend_statement(falseb);
	  falseb=bb;
	}
	prependStatement(exprStmt,isSgScopeStatement(falseb));
      }else if (partOfCondition){
	// create false statement, this is sometimes needed
	SgBasicBlock* bb = buildBasicBlock();
	bb->set_parent(isSgIfStmt(scope));
	isSgIfStmt(scope)->set_false_body(bb);
	falseb=bb;
	prependStatement(exprStmt,isSgScopeStatement(falseb));
      }
    } else if (isSgBasicBlock(scope)) {
      SgStatement* exprStmt = buildArrayCreateCall(initName,varRef,array,stmt);      
#if 1
      cerr << "++++++++++++ stmt :"<<stmt << " mainFirst:"<<mainFirst<<
      "   initName->get_scope():"<<initName->get_scope() <<
      "   mainFirst->get_scope():"<<mainFirst->get_scope()<<endl;
      if( global_stmt && initName->get_scope()!=mainFirst->get_scope()) {
		  mainBody -> prepend_statement( exprStmt );
        cerr << "+++++++ insert Before... "<<endl;
      } else {
        // insert new stmt (exprStmt) after (old) stmt
        insertStatementAfter(isSgStatement(stmt), exprStmt);
        cerr << "+++++++ insert After... "<<endl;
      }
#endif
      string empty_comment = "";
      attachComment(exprStmt,empty_comment,PreprocessingInfo::before);
      string comment = "RS : Create Array Variable, paramaters : (name, manglname, typr, basetype, address, sizeof(type), array size, fromMalloc, filename, linenr, linenrTransformed, dimension info ...)";
      attachComment(exprStmt,comment,PreprocessingInfo::before);
    } 
    else if (isSgNamespaceDefinitionStatement(scope)) {
      cerr <<"RuntimeInstrumentation :: WARNING - Scope not handled!!! : " << name << " : " << scope->class_name() << endl;
    } else {
      cerr
	<< "RuntimeInstrumentation :: Surrounding Block is not Block! : "
	<< name << " : " << scope->class_name() << endl;
      ROSE_ASSERT(false);
    }
  } else {
    cerr
      << "RuntimeInstrumentation :: Surrounding Statement could not be found! "
      << stmt->class_name() << endl;
    ROSE_ASSERT(false);
  }

  bool ismalloc = array->onHeap;
  // unfortunately the arrays are filled with '\0' which is a problem
  // for detecting other bugs such as not null terminated strings
  // therefore we call a function that appends code to the 
  // original program to add padding different from '\0'
  if (ismalloc)
    addPaddingToAllocatedMemory(stmt, array);

}

/* -----------------------------------------------------------
 * Perform Transformation: insertArrayCreateAccessCall
 * -----------------------------------------------------------*/
void RtedTransformation::insertArrayAccessCall(SgExpression* arrayExp,
                                               RTedArray* value) {
  SgStatement* stmt = value->surroundingStatement;
  if( !stmt )
      stmt = getSurroundingStatement( arrayExp );
  ROSE_ASSERT( arrayExp );
  ROSE_ASSERT( stmt );
  insertArrayAccessCall(stmt, arrayExp, value);
}


void RtedTransformation::insertArrayAccessCall(SgStatement* stmt,
                                               SgExpression* arrayExp, RTedArray* array) {

  if (isSgStatement(stmt)) {
    SgScopeStatement* scope = stmt->get_scope();
    ROSE_ASSERT(scope);

    SgExprListExp* arg_list = buildExprListExp();




    SgExpression* filename = buildString(stmt->get_file_info()->get_filename());
    SgExpression* linenr = buildString(RoseBin_support::ToString(stmt->get_file_info()->get_line()));
    appendExpression(arg_list, filename);


    SgPntrArrRefExp* arrRefExp = isSgPntrArrRefExp( arrayExp );
    ROSE_ASSERT( arrRefExp );

    // Recursively check each dimension of a multidimensional array access.
    // This doesn't matter for stack arrays, since they're contiguous and can
    // therefore be conceptually flattened, but it does matter for double
    // pointer array access.
    if( isSgPntrArrRefExp( arrRefExp -> get_lhs_operand() )) {
        //      a[ i ][ j ] = x;
        //      x = a[ i ][ j ];
        // in either case, a[ i ] is read, and read before a[ i ][ j ].
        insertArrayAccessCall( stmt, arrRefExp -> get_lhs_operand(), array );
    }


    int read_write_mask = 0;
	// determine whether this array access is a read or write
	SgNode* iter = arrayExp;
	do {
		SgNode* child = iter;
		iter = iter->get_parent();
		SgBinaryOp* binop = isSgBinaryOp( iter );

		if( isSgAssignOp( iter )) {
			ROSE_ASSERT( binop );

			// lhs write only, rhs read only
			if( binop->get_lhs_operand() == child )
				read_write_mask |= Write;
			else
				read_write_mask |= Read;
			// regardless of which side arrayExp was on, we can stop now
			break;
		} else if(  isSgAndAssignOp( iter ) 
					|| isSgDivAssignOp( iter ) 
					|| isSgIorAssignOp( iter )
					|| isSgLshiftAssignOp( iter )
					|| isSgMinusAssignOp( iter )
					|| isSgModAssignOp( iter )
					|| isSgMultAssignOp( iter )
					|| isSgPlusAssignOp( iter )
					|| isSgPointerAssignOp( iter )
					|| isSgRshiftAssignOp( iter )
					|| isSgXorAssignOp( iter)) {
		
			ROSE_ASSERT( binop );
			// lhs read & write, rhs read only
			read_write_mask |= Read;
			if( binop->get_lhs_operand() == child )
				read_write_mask |= Write;
			// regardless of which side arrayExp was on, we can stop now
			break;
		} else if( isSgPntrArrRefExp( iter )) {
			// outer[ inner[ ix ]] = val;
			//  inner[ ix ]  is only a read
			break;
		} else if( isSgDotExp( iter )) {
			ROSE_ASSERT( binop );
			if( child == binop -> get_lhs_operand() ) {
				// arr[ ix ].member is neither a read nor write of the array
				// itself.
				break;
			} // else foo.arr[ ix ] depends on parent context, so keep going
		}
	} while( iter );
    // always do a bounds check
    read_write_mask |= BoundsCheck;

	// for contiguous array, base is at &array[0] whether on heap or on stack
	SgTreeCopy tree_copy;
	SgPntrArrRefExp* array_base = isSgPntrArrRefExp( arrRefExp -> copy( tree_copy ));
	array_base -> set_rhs_operand( buildIntVal( 0 ));
	appendAddress( arg_list, array_base ); 
    appendAddressAndSize(NULL, arrRefExp, arg_list,0); 
    appendExpression( arg_list, buildIntVal( read_write_mask ));

    appendExpression(arg_list, linenr);

    SgExpression* linenrTransformed = buildString("x%%x");
    appendExpression(arg_list, linenrTransformed);



    ROSE_ASSERT(symbols->roseAccessHeap);
    string symbolName2 = symbols->roseAccessHeap->get_name().str();
    //cerr << " >>>>>>>> Symbol Member: " << symbolName2 << endl;
    SgFunctionRefExp* memRef_r = buildFunctionRefExp(
    		symbols->roseAccessHeap);
    //SgArrowExp* sgArrowExp = buildArrowExp(varRef_l, memRef_r);

    SgFunctionCallExp* funcCallExp = buildFunctionCallExp(memRef_r,
							  arg_list);
    SgExprStatement* exprStmt = buildExprStatement(funcCallExp);
    insertStatementBefore(isSgStatement(stmt), exprStmt);
    string empty_comment = "";
    attachComment(exprStmt,empty_comment,PreprocessingInfo::before);
    string comment = "RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message)";
    attachComment(exprStmt,comment,PreprocessingInfo::before);

  } else {
    cerr
      << "RuntimeInstrumentation :: Surrounding Statement could not be found! "
      << endl;
    exit(0);
  }
}


void RtedTransformation::populateDimensions( RTedArray* array, SgInitializedName* init, SgArrayType* type_ ) {
    std::vector< SgExpression* >& indices = array -> getIndices();

    bool implicit_index = false;
    SgType* type = type_;
    while( isSgArrayType( type )) {
        SgExpression* index = isSgArrayType( type ) -> get_index();
        if( index )
            indices.push_back( index );
        else
            implicit_index = true;
        type = isSgArrayType( type ) -> get_base_type();
    }

    // handle implicit first dimension for array initializers
    // for something like
    //      int p[][2][3] = {{{ 1, 2, 3 }, { 4, 5, 6 }}}
    //  we can calculate the first dimension as
    //      sizeof( p ) / ( sizeof( int ) * 2 * 3 )
    if( implicit_index ) {
        SgType* uint = buildUnsignedIntType();
        Sg_File_Info* file_info = init -> get_file_info();

        std::vector< SgExpression* >::iterator i = indices.begin();
        SgExpression* denominator = buildSizeOfOp( type );
        while( i != indices.end() ) {
            denominator = new SgMultiplyOp( file_info, denominator, *i, uint );
            ++i;
        }
        assert( denominator != NULL );

        indices.insert(
            indices.begin(),
            new SgDivideOp(
                file_info,
                buildSizeOfOp(
                    buildVarRefExp(
                        init,
                        getSurroundingStatement( init ) -> get_scope()
                    )
                ),
                denominator,
                uint
            )
        );
    }
}


void RtedTransformation::visit_isArraySgInitializedName(SgNode* n) {
  SgInitializedName* initName = isSgInitializedName(n);
  ROSE_ASSERT(initName);
  int dimension = 0;
  dimension = getDimension(initName);
  // STACK ARRAY : lets see if we assign an array here
  SgType* type = initName->get_typeptr();
  ROSE_ASSERT(type);
  SgArrayType* array = isSgArrayType(type);


  SgNode* gp = n -> get_parent() -> get_parent();
  SgFunctionDeclaration* fndec = isSgFunctionDeclaration( gp );
  // something like:
  // 	struct type { int before; char c[ 10 ]; int after; }
  // does not need a createarray call, as the type is registered and any array
  // information will be tracked when variables of that type are created
  //
  // ignore arrays in parameter lists as they're actually pointers, not stack
  // arrays
  if (  array 
        && !( isSgClassDefinition( gp ))
        && !( fndec )) {

      RTedArray* arrayRted = new RTedArray(true, initName, NULL, false);
      populateDimensions( arrayRted, initName, array );
      create_array_define_varRef_multiArray_stack[initName] = arrayRted;
  }
}


void
RtedTransformation::visit_isSgPointerDerefExp(SgPointerDerefExp* n) {
  cerr <<"\n$$$$$ visit_isSgPointerDerefExp : " << n->unparseToString() <<
    "  in line : " << n->get_file_info()->get_line() << " -------------------------------------" <<endl;

#if 1
  SgExpression* right = isSgExpression(n->get_operand());
  // right hand side should contain some VarRefExp
  std::vector< SgNode* > vars = NodeQuery::querySubTree( right, V_SgVarRefExp );
  std::vector< SgNode* >::const_iterator it = vars.begin();
  bool abortme=false;
  for (;it!=vars.end();++it) {
    SgVarRefExp* varRef = isSgVarRefExp(*it);
    ROSE_ASSERT(varRef);
    SgExpression* parent = isSgExpression(varRef->get_parent());
    ROSE_ASSERT(parent);
    SgDotExp* dotExp = isSgDotExp(parent);
    SgArrowExp* arrowExp = isSgArrowExp(parent);
    SgExpression* left = NULL;
    if (dotExp) left = dotExp->get_rhs_operand();
    if (arrowExp) left = arrowExp->get_lhs_operand();
    if ((left && left==varRef) || left==NULL) {
      variable_access_pointerderef[n]=varRef;
      cerr << "$$$ DotExp: " << dotExp << "   arrowExp: " << arrowExp << 	endl;
      cerr << "  &&& Adding : " << varRef->unparseToString() << endl;
    } else {
    	cerr << "$$$ Found a SgPointerDerefExp  but not adding to list. " << endl;
    	cerr << "  $$$ DotExp: " << dotExp << "   arrowExp: " << arrowExp << 	endl;
    	cerr << "  $$$ left: " << left->unparseToString() << "   varRef: " << varRef->unparseToString()<< 	endl;
    	abortme=true;
    }
  }
//  if (abortme) abort();
  if (vars.size()>1) {
    cerr << "Warning : We added more than one SgVarRefExp to this map for SgPointerDerefExp. This might be a problem" << endl;
    //exit(1);
  }
#if 1
  std::vector< SgNode* > vars2 = NodeQuery::querySubTree( right, V_SgThisExp );
  std::vector< SgNode* >::const_iterator it2 = vars2.begin();
  for (;it2!=vars2.end();++it2) {
    SgThisExp* varRef = isSgThisExp(*it2);
    ROSE_ASSERT(varRef);
    variable_access_arrowthisexp[ n] = varRef;
    cerr << " &&& Adding : " << varRef->unparseToString() << endl;
  }
  if (vars2.size()>1) {
    cerr << "Warning : We added more than one SgThisExp to this map for SgArrowExp. This might be a problem" << endl;
    //exit(1);
  }
#endif
#endif
}

void
RtedTransformation::visit_isSgArrowExp(SgArrowExp* n) {
  cerr <<"\n$$$$$ visit_isSgArrowExp : " << n->unparseToString() <<
    "  in line : " << n->get_file_info()->get_line() << " -------------------------------------" <<endl;

  SgExpression* left = isSgExpression(n->get_lhs_operand());
  ROSE_ASSERT(left);
  // left hand side should be a varrefexp or a thisOp
  std::vector< SgNode* > vars = NodeQuery::querySubTree( left, V_SgVarRefExp );
  std::vector< SgNode* >::const_iterator it = vars.begin();
  bool abortme=false;
  for (;it!=vars.end();++it) {
    SgVarRefExp* varRef = isSgVarRefExp(*it);
    ROSE_ASSERT(varRef);
    SgExpression* left = n->get_lhs_operand();
    if (left==varRef) {
    	variable_access_arrowexp[ n] = varRef;
    	cerr << " &&& Adding : " << varRef->unparseToString() << endl;
    } else {
    	cerr << " &&& Not adding varRef because on right hand side of -> :" << varRef->unparseToString() <<endl;
    	cerr << "   &&& left : " << left->unparseToString() << "  varRef: " << varRef << "  left:" <<left<<endl;
    	abortme = true;
    }
  }
//  if (abortme) abort();
  if (vars.size()>1) {
    cerr << "Warning : We added more than one SgVarRefExp to this map for SgArrowExp. This might be a problem" << endl;
    //exit(1);
  }
#if 1
  std::vector< SgNode* > vars2 = NodeQuery::querySubTree( left, V_SgThisExp );
  std::vector< SgNode* >::const_iterator it2 = vars2.begin();
  for (;it2!=vars2.end();++it2) {
    SgThisExp* varRef = isSgThisExp(*it2);
    ROSE_ASSERT(varRef);
    variable_access_arrowthisexp[ n] = varRef;
    cerr << " &&& Adding : " << varRef->unparseToString() << endl;
  }
  if (vars2.size()>1) {
    cerr << "Warning : We added more than one SgThisExp to this map for SgArrowExp. This might be a problem" << endl;
    //exit(1);
  }
#endif
}




// TODO 2 djh:  rewrite this function to be more robust
//  i.e. handle general cases
//  consider whether getting the initname is important
void RtedTransformation::visit_isArraySgAssignOp(SgNode* n) {
  SgAssignOp* assign = isSgAssignOp(n);
  
  SgInitializedName* initName = NULL;
  // left hand side of assign
  SgExpression* expr_l = assign->get_lhs_operand();
  // right hand side of assign
  SgExpression* expr_r = assign->get_rhs_operand();

  // varRef ([indx1][indx2]) = malloc (size); // total array alloc
  // varRef [indx1]([]) = malloc (size); // indx2 array alloc
  SgExpression* indx1 = NULL;
  SgExpression* indx2 = NULL;

  cerr <<"   ::: Checking assignment : " << n->unparseToString()<<endl;

  // FIXME 2: This probably does not handle n-dimensional arrays
  //
  // left side contains SgInitializedName somewhere ... search
  SgVarRefExp* varRef = isSgVarRefExp(expr_l);
  SgPntrArrRefExp* pntrArr = isSgPntrArrRefExp(expr_l);
  SgPointerDerefExp* pointerDeref = isSgPointerDerefExp(expr_l);
  if (varRef) {
    // is variable on left side
    // could be something like int** pntr; pntr = malloc ... (double array)
    // assume in this case explicitly pntr[indx1]=...
    initName = varRef->get_symbol()->get_declaration();
  } // ------------------------------------------------------------
  else if (pntrArr) {
    // is array on left side
    // could be pntr[indx1]  or pntr[indx1][indx2]
    SgExpression* expr_ll = pntrArr->get_lhs_operand();
    indx1 = pntrArr->get_rhs_operand();
    ROSE_ASSERT(expr_ll);
    varRef = isSgVarRefExp(expr_ll);
    if (varRef) {
      // we assume pntr[indx1] = malloc
      initName = varRef->get_symbol()->get_declaration();
    } else if (isSgPntrArrRefExp(expr_ll)) {
      SgPntrArrRefExp* pntrArr2 = isSgPntrArrRefExp(expr_ll);
      ROSE_ASSERT(pntrArr2);
      SgExpression* expr_lll = pntrArr2->get_lhs_operand();
      indx2 = pntrArr2->get_rhs_operand();
      varRef = isSgVarRefExp(expr_lll);
      if (varRef) {
	// we assume pntr[indx1][indx2] = malloc
	initName = varRef->get_symbol()->get_declaration();
      } else if (isSgDotExp(expr_lll)) {
	std::pair<SgInitializedName*,SgVarRefExp*> mypair = getRightOfDot(isSgDotExp(expr_lll),
									  "Left of pntrArr2 - Right of Dot  - line: "
									  + expr_lll->unparseToString() + " ", varRef);
	initName = mypair.first;
	varRef = mypair.second;
	if (initName)
	  ROSE_ASSERT(varRef);
      } else if (isSgArrowExp(expr_lll)) {
	std::pair<SgInitializedName*,SgVarRefExp*> mypair = getRightOfArrow(isSgArrowExp(expr_lll),
									    "Left of pntrArr2 - Right of Arrow  - line: "
									    + expr_lll->unparseToString() + " ", varRef);
	initName = mypair.first;
	varRef = mypair.second;
	if (initName)
	  ROSE_ASSERT(varRef);
      } else if( isSgPointerDerefExp( expr_lll )) {
        varRef = isSgVarRefExp( isSgPointerDerefExp( expr_lll ) -> get_operand() );
        ROSE_ASSERT( varRef );
        initName = varRef -> get_symbol() -> get_declaration();
      } else {
	cerr
	  << "RtedTransformation : Left of pntrArr2 - Unknown : "
	  << expr_lll->class_name() << endl;
	ROSE_ASSERT(false);
      }
    } else if (isSgDotExp(expr_ll)) {
      cerr << "RtedTransformation : isSgDotExp : " << endl;
      
      std::pair<SgInitializedName*,SgVarRefExp*> mypair = getRightOfDot(isSgDotExp(expr_ll),
									"Left of pntrArr - Right of Dot  - line: "
									+ expr_ll->unparseToString() + " ", varRef);
      initName = mypair.first;
      varRef = mypair.second;
      if (initName)
	ROSE_ASSERT(varRef);
    } else if (isSgPointerDerefExp(expr_ll)) {
      cerr << "RtedTransformation : isSgPointerDerefExp : " << endl;
      
      std::pair<SgInitializedName*,SgVarRefExp*> mypair = getRightOfPointerDeref(isSgPointerDerefExp(expr_ll),
                                                                                 "Left of pntrArr - Right of PointerDeref  - line: "
										 + expr_ll->unparseToString() + " ", varRef);
      initName = mypair.first;
      varRef = mypair.second;
      if (initName)
	ROSE_ASSERT(varRef);
    }
    else if (isSgArrowExp(expr_ll)) {
      cerr << "RtedTransformation : isSgArrowExp : " << endl;
      std::pair<SgInitializedName*,SgVarRefExp*> mypair  = getRightOfArrow(isSgArrowExp(expr_ll),
                                                                           "Left of pntrArr - Right of Arrow  - line: "
									   + expr_ll->unparseToString() + " ", varRef);
      initName = mypair.first;
      varRef = mypair.second;
      if (initName)
	ROSE_ASSERT(varRef);
    } else {
      cerr << "RtedTransformation : Left of pntrArr - Unknown : "
           << expr_ll->class_name() << "  line:"
	   << expr_ll->unparseToString() << endl;
      ROSE_ASSERT(false);
    }
  } // ------------------------------------------------------------
  else if (isSgDotExp(expr_l)) {
    std::pair<SgInitializedName*,SgVarRefExp*> mypair = getRightOfDot(isSgDotExp(expr_l),
								      "Right of Dot  - line: " + expr_l->unparseToString() + " ", varRef);
    initName = mypair.first;
    varRef = mypair.second;
    if (initName)
      ROSE_ASSERT(varRef);
  }// ------------------------------------------------------------
  else if (isSgArrowExp(expr_l)) {
    std::pair<SgInitializedName*,SgVarRefExp*> mypair  = getRightOfArrow(isSgArrowExp(expr_l),
                                                                         "Right of Arrow  - line: " + expr_l->unparseToString()
									 + " ", varRef);
    initName = mypair.first;
    varRef = mypair.second;
    if (initName)
      ROSE_ASSERT(varRef);
  } // ------------------------------------------------------------
  else if (pointerDeref) {
    SgExpression* exp = pointerDeref->get_operand();
    ROSE_ASSERT(exp);
    if (isSgPlusPlusOp(exp)) {
      std::pair<SgInitializedName*,SgVarRefExp*> mypair  = getPlusPlusOp(isSgPlusPlusOp(exp),
                                                                         "PointerDerefExp PlusPlus  - line: "
									 + pointerDeref->unparseToString() + " ", varRef);
      initName = mypair.first;
      varRef = mypair.second;
      ROSE_ASSERT(varRef);
    } else if (isSgMinusMinusOp(exp)) {
      std::pair<SgInitializedName*,SgVarRefExp*> mypair  = getMinusMinusOp(isSgMinusMinusOp(exp),
                                                                           "PointerDerefExp MinusMinus  - line: "
									   + pointerDeref->unparseToString() + " ", varRef);
      initName = mypair.first;
      varRef = mypair.second;
      ROSE_ASSERT(varRef);
    } 
    else if (isSgVarRefExp(exp)) {
      initName = isSgVarRefExp(exp)->get_symbol()->get_declaration();
      varRef = isSgVarRefExp(exp);
      ROSE_ASSERT(varRef);
    }
    else if (isSgDotExp(exp)) {
      std::pair<SgInitializedName*,SgVarRefExp*> mypair = getRightOfDot(isSgDotExp(exp),
									"Right of Dot  - line: " + exp->unparseToString() + " ", varRef);
      initName = mypair.first;
      varRef = mypair.second;
      if (initName)
	ROSE_ASSERT(varRef);
    }// ------------------------------------------------------------
    else if (isSgPointerDerefExp(exp)) {
      std::pair<SgInitializedName*,SgVarRefExp*> mypair = getRightOfPointerDeref(isSgPointerDerefExp(exp),
                                                                                 "Right of PointerDeref  - line: " + exp->unparseToString() + " ", varRef);
      initName = mypair.first;
      varRef = mypair.second;
      if (initName)
        ROSE_ASSERT(varRef);
    }// ------------------------------------------------------------
    else if( isSgArrowExp( exp )) {
      std::pair<SgInitializedName*,SgVarRefExp*> mypair = getRightOfArrow(isSgArrowExp(exp),
                                                                          "Right of PointerDeref  - line: " + exp->unparseToString() + " ", varRef);
      initName = mypair.first;
      varRef = mypair.second;
      if (initName)
        ROSE_ASSERT(varRef);
    }// ------------------------------------------------------------
    else if( isSgCastExp( exp )) {
        std::vector< SgNode* > vars = NodeQuery::querySubTree( exp, V_SgVarRefExp );
        ROSE_ASSERT( vars.size() > 0 );

        varRef = isSgVarRefExp( vars[ 0 ]);
        ROSE_ASSERT( varRef );

        initName = varRef -> get_symbol() -> get_declaration();
        ROSE_ASSERT(varRef);
    }// ------------------------------------------------------------
    else {
      cerr << "RtedTransformation : PointerDerefExp - Unknown : "
           << exp->class_name() << "  line:"
	   << pointerDeref->unparseToString() << endl;

      //      ROSE_ASSERT(false);
    }
  } // ------------------------------------------------------------
  else if (isSgFunctionCallExp(expr_l)) {
    cerr
      << "RtedTransformation: UNHANDLED BUT ACCEPTED FOR NOW - Left of assign - Unknown : "
      << expr_l->class_name() << "  line:"
      << expr_l->unparseToString() << endl;
#if 1
    SgFunctionCallExp* expcall = isSgFunctionCallExp(expr_l);
      SgExpression* exp = expcall->get_function();
      if (exp && isSgDotExp(exp)) {
	SgExpression* rightDot = isSgDotExp(exp)->get_rhs_operand();
	SgExpression* leftDot =  isSgDotExp(exp)->get_lhs_operand();
	ROSE_ASSERT(rightDot);
	ROSE_ASSERT(leftDot);
	SgVarRefExp* varRefL = isSgVarRefExp(leftDot);
	SgMemberFunctionRefExp* varRefR = isSgMemberFunctionRefExp(rightDot);
	if (varRefL && varRefR) {
	  // variable is on the left hand side
	  varRef=varRefL;
	  initName = (varRef)->get_symbol()->get_declaration();
	  if (initName)
	    ROSE_ASSERT(varRef);
	  ROSE_ASSERT(initName);
	}
    }// ------------------------------------------------------------
#endif
  }
  else if (isSgArrowStarOp(expr_l)) {
    std::pair<SgInitializedName*,SgVarRefExp*> mypair  = 
      getRightOfArrowStar(isSgArrowStarOp(expr_l),
		      "Right of Arrow  - line: " + expr_l->unparseToString()
		      + " ", varRef);
    ROSE_ASSERT(mypair.first);
    ROSE_ASSERT(mypair.second);
    initName = mypair.first;
    varRef = mypair.second;
    if (initName)
      ROSE_ASSERT(varRef);
  }// ------------------------------------------------------------
  else if (isSgDotStarOp(expr_l)) {
    std::pair<SgInitializedName*,SgVarRefExp*> mypair = 
      getRightOfDotStar(isSgDotStarOp(expr_l),
		    "Right of Dot  - line: " + expr_l->unparseToString() + " ", varRef);
    initName = mypair.first;
    varRef = mypair.second;
    if (initName)
      ROSE_ASSERT(varRef);
  }// ------------------------------------------------------------
  else {
    cerr << "RtedTransformation : Left of assign - Unknown : "
         << expr_l->class_name() << "  line:"
	 << expr_l->unparseToString() << endl;
    ROSE_ASSERT(false);
  }
  cerr << " expr_l : " << expr_l->class_name() << endl;
  ROSE_ASSERT(initName);

  // handle MALLOC
  bool ismalloc=false;
  bool iscalloc=false;
  vector<SgNode*> calls = NodeQuery::querySubTree(expr_r,
						  V_SgFunctionCallExp);
  vector<SgNode*>::const_iterator it = calls.begin();
  for (; it != calls.end(); ++it) {
    SgFunctionCallExp* funcc = isSgFunctionCallExp(*it);
    if (funcc) {
      // MALLOC : function call
      SgTreeCopy treeCopy;
      SgExprListExp* size = isSgExprListExp(funcc->get_args()->copy(treeCopy));
      ROSE_ASSERT(size);

      // find if sizeof present in size operator
      vector<SgNode*> results = NodeQuery::querySubTree(size,V_SgSizeOfOp);
      SgSizeOfOp* sizeofOp = NULL;
      if (results.size()==1) {
	sizeofOp = isSgSizeOfOp(*(results.begin()));
	ROSE_ASSERT(sizeofOp);
      } else if (results.size()>1) {
	cerr << "More than 1 sizeof operand. Abort." << endl;
	ROSE_ASSERT( false );
      }
      SgExpression* func = funcc->get_function();
      if (func && size) {
	ismalloc = false;
	SgFunctionRefExp* funcr = isSgFunctionRefExp(func);
	if (funcr) {
	  SgFunctionDeclaration* funcd =
	    funcr->getAssociatedFunctionDeclaration();
	  ROSE_ASSERT(funcd);
	  string funcname = funcd->get_name().str();
	  if (funcname == "malloc" )  {
	    ismalloc = true;
	    cerr
	      << "... Detecting func call on right hand side : "
	      << funcname << "     and size : "
	      << size->unparseToString() << "   idx1 : "
	      << indx1 << "  idx2 : " << indx2 << endl;
	  } else if( funcname == "calloc" ) {
          iscalloc = true;
          cerr
              << "... Detecting func call on right hand side : "
              << funcname;
      }
	  ROSE_ASSERT(varRef);
	} else {
	  // right hand side of assign should only contain call to malloc somewhere
	  cerr
	    << "RtedTransformation: UNHANDLED AND ACCEPTED FOR NOW. Right of Assign : Unknown (Array creation) : "
	    << func->class_name() << "  line:"
	    << funcc->unparseToString() << endl;
	  //	    ROSE_ASSERT(false);
	}
	if (ismalloc) {
	  ROSE_ASSERT(initName);
	  ROSE_ASSERT(varRef);

      // copy the argument to malloc so we know how much memory was allocated
      ROSE_ASSERT( size->get_expressions().size() > 0 );
      SgExpression* size_orig = 
          isSgExprListExp( size ) -> get_expressions()[ 0 ];

	  RTedArray* array = new RTedArray(false, initName, getSurroundingStatement( varRef ),
									  ismalloc, true, size_orig);
	  // varRef can not be a array access, its only an array Create
	  variablesUsedForArray.push_back(varRef);
	  create_array_define_varRef_multiArray[varRef] = array;
	} else if( iscalloc ) {
	  ROSE_ASSERT(initName);
	  ROSE_ASSERT(varRef);

      // copy the arguments to calloc so we know how much memory was allocated
      ROSE_ASSERT( size->get_expressions().size() > 1 );
      SgExpression* size_to_use 
          = new SgMultiplyOp(
            size -> get_file_info(),
              isSgExprListExp( size ) -> get_expressions()[ 0 ],
              isSgExprListExp( size ) -> get_expressions()[ 1 ]
          );

	  RTedArray* array 
          = new RTedArray(
                false, initName, getSurroundingStatement( varRef ),
                true,   // array is on heap
                true,   // array came form [mc]alloc
                size_to_use
            );

	  // varRef can not be a array access, its only an array Create
	  variablesUsedForArray.push_back(varRef);
	  create_array_define_varRef_multiArray[varRef] = array;
    }
      }
    }
  }

  // FIXME 3: This won't handle weird cases with, e.g. multiple news on the rhs,
  //    but for those cases this entire function is probably broken.  Consider,
  //    e.g.
  //        int *a, *b;
  //        a = ( b = new int, new int );
  //
  // handle new (implicit C++ malloc)
  BOOST_FOREACH(
        SgNode* exp,
        NodeQuery::querySubTree( expr_r, V_SgNewExp )) {

    // FIXME 2: this is a false positive if operator new is overloaded
    SgNewExp* new_op = isSgNewExp( exp );

    ROSE_ASSERT( new_op );
    ROSE_ASSERT( varRef );

    RTedArray *array = new RTedArray(
        false,                              // not on stack
        initName,
        getSurroundingStatement( varRef ),
        true,                              // memory on heap
        false,                             // but not from call to malloc
        buildSizeOfOp( new_op -> get_specified_type() )
    );

    variablesUsedForArray.push_back( varRef );
    create_array_define_varRef_multiArray[ varRef ] = array;
  }

  // ---------------------------------------------
  // handle variables ..............................
  // here we should know the initName of the variable on the left hand side
  ROSE_ASSERT(initName);
  ROSE_ASSERT(varRef);
  if (initName && varRef) {
    // we now know that this variable must be initialized
    // if we have not set this variable to be initialized yet,
    // we do so
    cerr  << ">> Setting this var to be initialized : " << initName->unparseToString() << endl;
    variableIsInitialized[varRef]=std::pair<SgInitializedName*,bool>(initName,ismalloc);
  }
  // ---------------------------------------------


}



void RtedTransformation::addPaddingToAllocatedMemory(SgStatement* stmt,  RTedArray* array) {
  printf(">>> Padding allocated memory with blank space\n");
  //SgStatement* stmt = getSurroundingStatement(varRef);
  ROSE_ASSERT(stmt);
  // if you find this:
  //   str1 = ((char *)(malloc(((((4 * n)) * (sizeof(char )))))));
  // add the following lines:
  //   int i;
  //   for (i = 0; (i) < malloc(((((4 * n)) * (sizeof(char )); i++)
  //     str1[i] = ' ';

  // we do this only for char*
  bool cont=false;
  SgInitializedName* initName = array->initName;
  SgType* type = initName->get_type();
  ROSE_ASSERT(type);
  cerr << " Padding type : " << type->class_name() << endl;
  if (isSgPointerType(type)) {
    SgType* basetype = isSgPointerType(type)->get_base_type();
    cerr << " Base type : " << basetype->class_name() << endl;
    if (basetype && isSgTypeChar(basetype))
      cont=true;
  }
  
  // since this is mainly to handle char* correctly, we only deal with one dim array for now
  if (cont && array->getDimension()==1) {
    // allocated size
    SgScopeStatement* scope = stmt->get_scope();
    SgExpression* size = array->getIndices()[ 0 ];
    pushScopeStack (scope);
    // int i;
    SgVariableDeclaration* stmt1 = buildVariableDeclaration("i",buildIntType(),NULL); 
    //for(i=0;..)
    SgStatement* init_stmt= buildAssignStatement(buildVarRefExp("i"),buildIntVal(0));

    // for(..,i<size,...) It is an expression, not a statement!
    SgExprStatement* cond_stmt=NULL;
    cond_stmt= buildExprStatement(buildLessThanOp(buildVarRefExp("i"),size)); 
 
    // for (..,;...;i++); not ++i;
    SgExpression* incr_exp = NULL;
    incr_exp=buildPlusPlusOp(buildVarRefExp("i"),SgUnaryOp::postfix);
    // loop body statement
    SgStatement* loop_body= NULL; 
    SgExpression* lhs = buildPntrArrRefExp(buildVarRefExp(array->initName->get_name()),buildVarRefExp("i"));
    SgExpression* rhs = buildCharVal(' ');
    loop_body = buildAssignStatement(lhs,rhs);
    //loop_body = buildExprStatement(stmt2); 


    SgForStatement* forloop = buildForStatement (init_stmt,cond_stmt,incr_exp,loop_body);

    SgBasicBlock* bb = buildBasicBlock(stmt1,
                                       forloop);
    insertStatementAfter(isSgStatement(stmt), bb);
    string comment = "RS: Padding this newly generated array with empty space.";
    attachComment(bb,comment,PreprocessingInfo::before);
    popScopeStack();
  } 
}


void RtedTransformation::visit_isArrayPntrArrRefExp(SgNode* n) {
  SgPntrArrRefExp* arrRefExp = isSgPntrArrRefExp(n);
  // make sure the parent is not another pntr array (pntr->pntr), we only want the top one
  // also, ensure we don't count arr[ix].member as an array access, e.g. in the
  // following:
  //    arr[ix].member = 2;
  // we need only checkwrite &( arr[ix].member ), which is handled by init var.
  if ( !isSgPntrArrRefExp( arrRefExp->get_parent() )) {

    int dimension = 1;
    SgExpression* left = arrRefExp->get_lhs_operand();
    // right hand side can be any expression!
    SgExpression* right1 = arrRefExp->get_rhs_operand();
    SgExpression* right2 = NULL;
    ROSE_ASSERT(right1);
    SgVarRefExp* varRef = isSgVarRefExp(left);
    if (varRef == NULL) {
      SgArrowExp* arrow = isSgArrowExp(left);
      SgDotExp* dot = isSgDotExp(left);
      SgPointerDerefExp* pointerDeref = isSgPointerDerefExp(left);
      SgPntrArrRefExp* arrRefExp2 = isSgPntrArrRefExp(left);
      if (arrow) {
	varRef = isSgVarRefExp(arrow->get_rhs_operand());
	ROSE_ASSERT(varRef);
      } else if (dot) {
        varRef = isSgVarRefExp(dot->get_rhs_operand());
        ROSE_ASSERT(varRef);
      } else if (pointerDeref) {
	varRef = isSgVarRefExp(pointerDeref->get_operand());
	ROSE_ASSERT(varRef);
      } else if (arrRefExp2) {
	dimension = 2;
	SgExpression* expr2 = arrRefExp2->get_lhs_operand();
	right2 = arrRefExp2->get_rhs_operand();
	varRef = isSgVarRefExp(expr2);
	if (varRef) {
	  // do nothing
	} else if ((varRef=resolveToVarRefRight(expr2))) {
	  ROSE_ASSERT(varRef);
	} else {
	  cerr
	    << ">> RtedTransformation::ACCESS::SgPntrArrRefExp:: unknown left of SgPntrArrRefExp2: "
	    << expr2->class_name() << " --"
	    << arrRefExp->unparseToString() << "-- "
	    << endl;
	  ROSE_ASSERT(false);
	}
	ROSE_ASSERT(varRef);
      } else {
	cerr
	  << ">> RtedTransformation::ACCESS::SgPntrArrRefExp:: unknown left of SgArrowExp: "
	  << left->class_name() << " --"
	  << arrRefExp->unparseToString() << "-- " << endl;
	ROSE_ASSERT(false);
      }
    }
    ROSE_ASSERT(varRef);
    bool create_access_call=true;
    vector<SgVarRefExp*>::const_iterator cv = variablesUsedForArray.begin();
    for (;cv!=variablesUsedForArray.end();++cv) {
      SgVarRefExp* stored = *cv;
      if (stored == varRef)
	create_access_call=false;
    }
    if (create_access_call) {
      SgInitializedName* initName =
	  varRef->get_symbol()->get_declaration();
      ROSE_ASSERT(initName);
	  RTedArray* array = new RTedArray(false, initName, getSurroundingStatement( n ), false);
      cerr << "!! CALL : " << varRef << " - "
           << varRef->unparseToString() << "    size : "
	   << create_array_access_call.size() << "  -- "
	   << array->unparseToString() << " : "
	   << arrRefExp->unparseToString() << endl;
      create_array_access_call[ arrRefExp ] = array;
    }
  }

}

// deprecated - will be removed
void RtedTransformation::visit_isArrayExprListExp(SgNode* n) {
  // there could be a cast between SgExprListExp and SgVarRefExp
  SgExprListExp* exprlist = isSgExprListExp(isSgVarRefExp(n)->get_parent());
#if 1
  cerr << " >> checking node : " << n->class_name() << endl;
  if (isSgVarRefExp(n)) {
    cerr << ">>>>>>>>>>> checkign func : " << isSgVarRefExp(n)->unparseToString() <<
      "    parent : " <<  isSgVarRefExp(n)->get_parent()->class_name() <<
      "    parent : " <<  isSgVarRefExp(n)->get_parent()->get_parent()->class_name() << endl;
  }
#endif
  SgNode* parent = isSgVarRefExp(n)->get_parent();
  while (!isSgExprListExp(parent) && !isSgProject(parent)) {
    parent=parent->get_parent();
  }
  if (isSgExprListExp(parent))
    exprlist = isSgExprListExp(parent);
  //  if (isSgCastExp(isSgVarRefExp(n)->get_parent()))
  //  exprlist = isSgExprListExp(isSgVarRefExp(n)->get_parent()->get_parent());
  // check if this is a function call with array as parameter
  if (exprlist) {
    SgFunctionCallExp* fcexp = isSgFunctionCallExp(exprlist->get_parent());
    if (fcexp) {
      cerr <<"      ... Found a function call with varRef as parameter: " << fcexp->unparseToString() <<endl;
      // check if parameter is array - then check function name
      // call func(array_name) to runtime system for runtime inspection 
      SgInitializedName* initName =
	isSgVarRefExp(n)->get_symbol()->get_declaration();
      if (isVarRefInCreateArray(initName) ||
	  isVarInCreatedVariables(initName)) {
	// create this function call only, if it is not one of the 
	// interesting function calls, such as strcpy, strcmp ...
	// because for those we do not need the parameters and do not 
	// need to put them on the stack
	SgFunctionRefExp* refExp = isSgFunctionRefExp(fcexp->get_function());
	ROSE_ASSERT(refExp);
	SgFunctionDeclaration* decl = isSgFunctionDeclaration(refExp->getAssociatedFunctionDeclaration ());
	ROSE_ASSERT(decl);
	string name = decl->get_name();
	string mangled_name = decl->get_mangled_name().str();
	cerr <<">>Found a function call " << name << endl;
	if (isStringModifyingFunctionCall(name)==false &&
	    isFileIOFunctionCall(name)==false ) {
	  vector<SgExpression*> args;
	  SgExpression* varOnLeft = buildString("NoAssignmentVar2");
	  SgStatement* stmt = getSurroundingStatement(isSgVarRefExp(n));
	  ROSE_ASSERT(stmt);
	  RtedArguments* funcCall = new RtedArguments(name, // function name
						      mangled_name,
						      // we need this for the function as well
						      initName->get_name(), // variable
						      initName->get_mangled_name().str(),
						      isSgVarRefExp(n),
						      stmt,
						      args,
						      varOnLeft,
						      varOnLeft,
						      NULL
						      );
	  ROSE_ASSERT(funcCall);	
	  cerr << " !!!!!!!!!! Adding function call." << name << endl;
	  function_call.push_back(funcCall);
	}
      } else {
	cerr << " This is a function call but its not passing an array element " << endl;
      }
    }
  }
}

#endif





