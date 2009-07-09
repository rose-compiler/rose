/*
 * RtedTransf_variable.cpp
 *
 *  Created on: Jul 7, 2009
 *      Author: panas2
 */


#include <rose.h>
#include <string>
#include "RtedSymbols.h"
#include "DataStructures.h"
#include "RtedTransformation.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;


// ------------------------ VARIABLE SPECIFIC CODE --------------------------
// SHOULD BE MOVED TO SEPARATE FILE LATER

bool RtedTransformation::isVarInCreatedVariables(SgInitializedName* n) {
  bool ret=false;
  ROSE_ASSERT(n);
  Rose_STL_Container<SgInitializedName*>::const_iterator it=variable_declarations.begin();
  for (;it!=variable_declarations.end();++it) {
    SgInitializedName* initName = *it;
    if (initName==n) {
      ret=true;
      break;
    }
  }
  return ret;
}

void RtedTransformation::visit_isSgVariableDeclaration(SgNode* n) {
  SgVariableDeclaration* varDecl = isSgVariableDeclaration(n);
  Rose_STL_Container<SgInitializedName*> vars = varDecl->get_variables();
  Rose_STL_Container<SgInitializedName*>::const_iterator it = vars.begin();
  cerr << " ...... CHECKING Variable Declaration " << endl;
  for (;it!=vars.end();++it) {
    SgInitializedName* initName = *it;
    ROSE_ASSERT(initName);
    // need to get the type and the possible value that it is initialized with
    cerr << "      Detected initName : " << initName->unparseToString() ;
    cerr <<"  type : " << initName->get_type()->unparseToString() << endl;
    variable_declarations.push_back(initName);
  }
}


void RtedTransformation::insertVariableCreateCall(SgInitializedName* initName
						  ) {
  SgStatement* stmt = getSurroundingStatement(initName);
  // make sure there is no extern in front of stmt
#if 1
  SgDeclarationStatement* declstmt = isSgDeclarationStatement(stmt);
  SgFunctionParameterList* funcparam = isSgFunctionParameterList(stmt);
  bool externQual =false;
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
  if (externQual) {
    cerr << "Skipping this insertVariableCreateCall because it probably occurs multiple times (with and without extern)." << endl;
    return;
  }
#endif


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
	exit(1);
      }
      scope = scope->get_scope();
      // We want to insert the stmt before this classdefinition, if its still in a valid block
      cerr <<" ....... Found ClassDefinition Scope. New Scope is : " << scope->class_name() << "  stmt:" << stmt->class_name() <<endl;
    }
    // what is there is an array creation in a global scope
    else if (isSgGlobal(scope)) {
      cerr <<"RuntimeInstrumentation :: WARNING - Scope not handled!!! : " << name << " : " << scope->class_name() << endl;
      // We need to add this new statement to the beginning of main
      // get the first statement in main as stmt
      stmt = mainFirst;
      scope = stmt->get_scope();

      SgExprStatement* exprStmt = buildVariableCreateCallStmt( initName, stmt);
      // kind of hackey.  Really we should be prepending into main's body, not
      // inserting relative its first statement.
      //insertStatementBefore(isSgStatement(stmt), exprStmt);
    }
    if (isSgBasicBlock(scope)) {
      // insert new stmt (exprStmt) after (old) stmt
      SgExprStatement* exprStmt = buildVariableCreateCallStmt( initName, stmt);
#if 1
      cerr << "++++++++++++ stmt :"<<stmt << " mainFirst:"<<mainFirst<<
      "   initName->get_scope():"<<initName->get_scope() <<
      "   mainFirst->get_scope():"<<mainFirst->get_scope()<<endl;
      if( stmt == mainFirst && initName->get_scope()!=mainFirst->get_scope()) {
        insertStatementBefore(isSgStatement(stmt), exprStmt);
        cerr << "+++++++ insert Before... "<<endl;
      } else {
        // insert new stmt (exprStmt) after (old) stmt
        insertStatementAfter(isSgStatement(stmt), exprStmt);
        cerr << "+++++++ insert After... "<<endl;
      }
#endif
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


}


SgExprStatement*
RtedTransformation::buildVariableCreateCallStmt( SgInitializedName* initName, SgStatement* stmt, bool forceinit) {
    string name = initName->get_mangled_name().str();
    SgScopeStatement* scope = stmt->get_scope();


    ROSE_ASSERT( initName);
    ROSE_ASSERT( stmt);
    ROSE_ASSERT( scope);

    // build the function call : runtimeSystem-->createArray(params); ---------------------------
    SgExprListExp* arg_list = buildExprListExp();
    SgExpression* callName = buildString(initName->get_name().str());
    //SgExpression* callName = buildStringVal(initName->get_name().str());
    SgExpression* callNameExp = buildString(name);
    SgExpression* typeName = buildString(initName->get_type()->class_name());
    SgInitializer* initializer = initName->get_initializer();
    SgExpression* fileOpen = buildString("no");
    bool initb = false;
    if (initializer) initb=true;
    SgExpression* initBool = buildIntVal(0);
    if (initb || forceinit)
      initBool = buildIntVal(1);

    SgExpression* var_ref = buildVarRefExp( initName, scope);
    // TODO 2: Remove this if statement once the RTS handles user types
    // Note: This if statement is a hack put in place to pass the array out of
    // bounds tests
    if( isSgVariableDeclaration( stmt)) {
      SgInitializedName* first_var_name
        = isSgVariableDeclaration( stmt)->get_variables()[0];

      if(
          isSgClassDeclaration(
              isSgVariableDeclaration(stmt)->get_baseTypeDefiningDeclaration())
            && first_var_name != initName
      ) {
        var_ref = new SgDotExp(
          // file info
          initName->get_file_info(),
          // lhs
          buildVarRefExp( first_var_name, scope),
          // rhs
          var_ref,
          // type
          initName->get_type()
        );
      }
    }


    appendExpression(arg_list, callName);
    appendExpression(arg_list, callNameExp);
    //appendExpression(arg_list, typeName);
    appendAddressAndSize(initName, isSgVarRefExp(var_ref), stmt, arg_list,1);


    appendExpression(arg_list, initBool);
    appendExpression(arg_list, fileOpen);

    SgExpression* filename = buildString(stmt->get_file_info()->get_filename());
    SgExpression* linenr = buildString(RoseBin_support::ToString(stmt->get_file_info()->get_line()));
    appendExpression(arg_list, filename);
    appendExpression(arg_list, linenr);

    SgExpression* linenrTransformed = buildString("x%%x");
    appendExpression(arg_list, linenrTransformed);

    ROSE_ASSERT(roseCreateVariable);
    string symbolName2 = roseCreateVariable->get_name().str();
    SgFunctionRefExp* memRef_r = buildFunctionRefExp(  roseCreateVariable);
    SgFunctionCallExp* funcCallExp = buildFunctionCallExp(memRef_r,
                arg_list);
    SgExprStatement* exprStmt = buildExprStatement(funcCallExp);
    string empty_comment = "";
    attachComment(exprStmt,empty_comment,PreprocessingInfo::before);
    string comment = "RS : Create Variable, paramaters : (name, mangl_name, type, address, sizeof, initialized, fileOpen, filename, linenr, linenrTransformed)";
    attachComment(exprStmt,comment,PreprocessingInfo::before);

    return exprStmt;
}


void RtedTransformation::appendAddressAndSize(SgInitializedName* initName,
					      SgExpression* varRefE,
					      SgStatement* stmt,
					      SgExprListExp* arg_list,
					      int appendType
					      ) {
#if 1
    SgType* type = initName->get_type();
    ROSE_ASSERT(type);
    SgType* basetype = NULL;
    SgExpression* basetypeStr = buildString("no base");
    if (isSgPointerType(type)) {
	//	typestr="pointer";
	basetype = isSgPointerType(type)->get_base_type();
	if (basetype)
	  basetypeStr = buildString(basetype->class_name());
    }
    SgExpression* ctypeStr = buildString(type->class_name());
    if (appendType==1) {
    	appendExpression(arg_list, ctypeStr);
	  	appendExpression(arg_list, basetypeStr);
    }

    SgScopeStatement* scope = initName->get_scope();
    if (isSgClassType(basetype) ||
	  isSgTypedefType(basetype) ||
	  isSgClassDefinition(scope)
	  ) 	      {
			// cant handle struct yet
	     appendExpression(arg_list, buildIntVal(66));
	     appendExpression(arg_list, buildIntVal(66));
    } else {
	//int derefCounter=0;//getTypeOfPointer(initName);
	//SgExpression* fillExp = getExprBelowAssignment(varRefE, derefCounter);
	//ROSE_ASSERT(fillExp);
    appendExpression(
      arg_list,
      buildCastExp(
        buildAddressOfOp( varRefE),
        buildUnsignedLongLongType()
      )
    );
	appendExpression(
	    arg_list,
	    buildSizeOfOp( varRefE)
	  );
    }
#else
      // if the variable assignment : var = ...
      // is a pointer then we store the memory location
      // otherwise we store the value
      SgScopeStatement* scope = initName->get_scope();
      SgType* type = initName->get_type();
      ROSE_ASSERT(type);
      // string typestr = "notype";
      SgType* basetype = NULL;
      SgExpression* basetypeStr = buildString("no base");
      if (isSgPointerType(type)) {
	//	typestr="pointer";
	basetype = isSgPointerType(type)->get_base_type();
	if (basetype)
	  basetypeStr = buildString(basetype->class_name());

      }
      SgExpression* ctypeStr = buildString(type->class_name());
      if (appendType==1) {
	appendExpression(arg_list, ctypeStr);
	  	appendExpression(arg_list, basetypeStr);
	//appendExpression(arg_list, buildString(scope->class_name()));
      }

      if (isSgClassType(basetype) ||
	  isSgTypedefType(basetype) ||
	  isSgClassDefinition(scope)
	  ) 	      {
	appendExpression(arg_list, buildIntVal(66));
      } else {

      //appendExpression(arg_list, buildString(varRefE->get_parent()->class_name()));
      cerr << " Checking type : " << type->class_name() << endl;
      if (basetype)
	cerr << "  base type : " << basetype->class_name() << endl;
      if (isSgPointerType(type) && isSgPointerType(type)->get_parent())
	cerr << "  parent type : " << (isSgPointerType(type)->get_parent())->class_name() << endl;
      // append address and value
      if (isSgPointerType(type) &&
	  isSgPointerType(isSgPointerType(type)->get_parent())==NULL
	  ) {

	//char* res = (char*)malloc(long int);
	//fprintf(res,"%p",varRefE);
	int derefCounter=0;//getTypeOfPointer(initName);
	SgExpression* fillExp = getExprBelowAssignment(varRefE, derefCounter);

	//	ROSE_ASSERT(fillExp);
	if (fillExp) {
	  bool isDoNotHandleType=false;
	  // handle Exceptions. dont init value if class or pointer to pointer
	  if (isSgClassType(basetype) ||
	      isSgPointerType(basetype) ||
	      isSgTypedefType(basetype)
	      )
	    isDoNotHandleType=true;


	  if (isSgPointerType(type)) {
	    bool pointer=true;
	    if (isSgPointerType(basetype)) {
	      // pointer to pointer
	      pointer=false;
	    } else {
	      // pointer
	    }
	    if (pointer && derefCounter==0) {
	      // int *ptr=0; ptr = 0:  pass address : ptr    value: *ptr
	      appendExpression(arg_list, buildCastExp(fillExp,buildUnsignedLongLongType()));
	    } else if (pointer && derefCounter==1) {
	      // if it is a pointer then we should have derefCounter == 1
	      // int *ptr=0; *ptr = 0:  pass address : &*ptr    value: *ptr
	      appendExpression(arg_list, buildCastExp(buildAddressOfOp(fillExp),buildUnsignedLongLongType()));
	    } else if (!pointer && derefCounter==0) {
	      // int **ptr=0; ptr = 0:  pass address : ptr    value: **ptr
	      appendExpression(arg_list, buildCastExp(fillExp,buildUnsignedLongLongType()));
	    } else if (!pointer && derefCounter==1) {
	      // int **ptr=0; *ptr = 0:  pass address : *ptr    value: **ptr
	      appendExpression(arg_list, buildCastExp(fillExp,buildUnsignedLongLongType()));
	    } else if (!pointer && derefCounter==2) {
	      // int **ptr=0; **ptr = 0:  pass address : &*ptr    value: **ptr
	      appendExpression(arg_list, buildCastExp(buildAddressOfOp(fillExp),buildUnsignedLongLongType()));
	    } else {
	      // no such case
	      cerr << "pointer = " << pointer << "  derefCounter = " << derefCounter << "   " << stmt->unparseToString() << endl;
	      ROSE_ASSERT(false);
	    }

	  } else {
	    appendExpression(arg_list, buildCastExp(fillExp,buildUnsignedLongLongType()));
	  }
	} else {
	  cerr << " Pointer that is not handled: " << varRefE->unparseToString() << endl;
	  // assume its an array - dont handle (PntrArrRefExp)
	  //fixme
	  appendExpression(arg_list, buildIntVal(-5));
	  //return;
	}

      } else if (isSgTypeInt(type) ||
		 isSgTypeUnsignedInt(type) ||
		 isSgTypeFloat(type) ||
		 isSgTypeDouble(type) ||
		 isSgTypeLong(type) ||
		 isSgTypeLongLong(type)) {
	int derefCounter=0;
	SgExpression* fillExp = getExprBelowAssignment(varRefE, derefCounter);
	if (fillExp) {
	  appendExpression(arg_list, buildCastExp(buildAddressOfOp(fillExp),buildUnsignedLongLongType())); //buildLongIntVal(-1));
	} else {
	  cerr << "Unhandled for variables : " << varRefE->unparseToString() << endl;
	  //ROSE_ASSERT(fillExp);
	}
      }  else if (isSgTypeChar(type)) {
	int derefCounter=0;
	SgExpression* fillExp = getExprBelowAssignment(varRefE, derefCounter);
	if (fillExp) {
	  //appendExpression(arg_list, buildCastExp(buildAddressOfOp(varRefE),buildUnsignedLongLongType())); //buildLongIntVal(-1));
	  //appendExpression(arg_list, varRefE);
	  appendExpression(arg_list, buildCastExp(buildAddressOfOp(fillExp),buildUnsignedLongLongType())); //buildLongIntVal(-1));
	} else {
	  cerr << "Unhandled for variables : " << varRefE->unparseToString() << endl;
	  //ROSE_ASSERT(fillExp);
	}
      }
      else if (isSgArrayType(type) ||
	       isSgClassType(type) ||
	       isSgTypedefType(type)
	       ) {
	// we don't do this for arrays, because the initialization of each
	// element is handled at runtime
	// fixme
	appendExpression(arg_list,buildIntVal(-1));
	//return;
      } else {
	cerr << "Variable Init: Condition unknown :" << type->class_name() << endl;
	exit(1);
      }

	  }



      // append sizeof(type);
      // this won't work for struct, class ,etc.
      if (isSgClassType(basetype) ||
	  isSgPointerType(basetype) ||
	  isSgTypedefType(basetype)
	  )
	// fixme: Need to handle the size of these
	appendExpression(arg_list,buildSizeOfOp(basetype));
      else {
	if (isSgArrayType(type)) {
	  	appendExpression(
	    arg_list,
	    //buildSizeOfOp( initName->get_type()) //basetype?
	    buildIntVal( -2) //fixme
	  		 );
	} else {
	appendExpression(
	    arg_list,
	    buildSizeOfOp( initName->get_type())
			 );
	}
      }
#endif
}


void RtedTransformation::insertInitializeVariable(SgInitializedName* initName,
						  SgVarRefExp* varRefE,
						  bool ismalloc
						  ) {
  SgStatement* stmt = getSurroundingStatement(varRefE);
  // make sure there is no extern in front of stmt

  if (isSgStatement(stmt)) {
    SgScopeStatement* scope = stmt->get_scope();
    string name = initName->get_mangled_name().str();
    cerr << "          ... running insertInitializeVariable :  "<<name<< "   scope: " << scope->class_name() << endl;

    ROSE_ASSERT(scope);
    // what if there is an array creation within a ClassDefinition
    if ( isSgClassDefinition(scope)) {
      // new stmt = the classdef scope
      SgClassDeclaration* decl = isSgClassDeclaration(scope->get_parent());
      ROSE_ASSERT(decl);
      stmt = isSgVariableDeclaration(decl->get_parent());
      if (!stmt) {
	cerr << " Error . stmt is unknown : " << decl->get_parent()->class_name() << endl;
	exit(1);
      }
      scope = scope->get_scope();
      // We want to insert the stmt before this classdefinition, if its still in a valid block
      cerr <<" ------->....... Found ClassDefinition Scope. New Scope is : " << scope->class_name() << "  stmt:" << stmt->class_name() <<"\n\n\n\n"<<endl;
    }
    // what is there is an array creation in a global scope
    else if (isSgGlobal(scope)) {
      cerr <<" ------->....... RuntimeInstrumentation :: WARNING - Scope not handled!!! : " << name << " : " << scope->class_name() << "\n\n\n\n" <<endl;
      // We need to add this new statement to the beginning of main
      // get the first statement in main as stmt
      stmt = mainFirst;
      scope=stmt->get_scope();
    }
    if (isSgBasicBlock(scope) ||
	isSgIfStmt(scope) ||
	isSgForStatement(scope)) {
      // build the function call : runtimeSystem-->createArray(params); ---------------------------
      SgExprListExp* arg_list = buildExprListExp();
      SgExpression* simplename = buildString(initName->get_name().str());
      appendExpression(arg_list, simplename);
      SgExpression* callName = buildString(initName->get_mangled_name().str());
      appendExpression(arg_list, callName);


      appendAddressAndSize(initName, varRefE, stmt, arg_list,1);


      SgIntVal* ismallocV = buildIntVal(0);
      if (ismalloc)
        ismallocV = buildIntVal(1);
      appendExpression(arg_list, ismallocV);

      SgExpression* filename = buildString(stmt->get_file_info()->get_filename());
      SgExpression* linenr = buildString(RoseBin_support::ToString(stmt->get_file_info()->get_line()));
      appendExpression(arg_list, filename);
      appendExpression(arg_list, linenr);
      SgExpression* linenrTransformed = buildString("x%%x");
      appendExpression(arg_list, linenrTransformed);

      appendExpression(arg_list, buildString(removeSpecialChar(stmt->unparseToString())));

      ROSE_ASSERT(roseInitVariable);
      string symbolName2 = roseInitVariable->get_name().str();
      SgFunctionRefExp* memRef_r = buildFunctionRefExp(  roseInitVariable);
      SgFunctionCallExp* funcCallExp = buildFunctionCallExp(memRef_r,
                  arg_list);

      SgExprStatement* exprStmt = buildExprStatement(funcCallExp);
      string empty_comment = "";
      attachComment(exprStmt,empty_comment,PreprocessingInfo::before);
      string comment = "RS : Init Variable, paramaters : (name, mangl_name, tpye, basetype, address, size, ismalloc, filename, line, linenrTransformed, error line)";
      attachComment(exprStmt,comment,PreprocessingInfo::before);

      // insert new stmt (exprStmt) before (old) stmt
      insertStatementAfter(isSgStatement(stmt), exprStmt);
    } // basic block
    else if (isSgNamespaceDefinitionStatement(scope)) {
      cerr <<" ------------> RuntimeInstrumentation :: WARNING - Scope not handled!!! : " << name << " : " << scope->class_name() << "\n\n\n\n"<<endl;
    } else {
      cerr
	<< " -----------> RuntimeInstrumentation :: Surrounding Block is not Block! : "
	<< name << " : " << scope->class_name() << "  - " << stmt->unparseToString() << endl;
      ROSE_ASSERT(false);
    }
  } else {
    cerr
      << "RuntimeInstrumentation :: Surrounding Statement could not be found! "
      << stmt->class_name() << "  " << stmt->unparseToString() << endl;
    ROSE_ASSERT(false);
  }


}



void RtedTransformation::insertAccessVariable(SgVarRefExp* varRefE,
			SgPointerDerefExp* derefExp
						  ) {
	SgStatement* stmt = getSurroundingStatement(varRefE);
  // make sure there is no extern in front of stmt
  SgInitializedName* initName = varRefE->get_symbol()->get_declaration();

  if (isSgStatement(stmt)) {
    SgScopeStatement* scope = stmt->get_scope();
    string name = initName->get_mangled_name().str();
    cerr << "          ... running insertAccessVariable :  "<<name<< "   scope: " << scope->class_name() << endl;

    ROSE_ASSERT(scope);
    // what if there is an array creation within a ClassDefinition
    if ( isSgClassDefinition(scope)) {
      // new stmt = the classdef scope
      SgClassDeclaration* decl = isSgClassDeclaration(scope->get_parent());
      ROSE_ASSERT(decl);
      stmt = isSgVariableDeclaration(decl->get_parent());
      if (!stmt) {
	cerr << " Error . stmt is unknown : " << decl->get_parent()->class_name() << endl;
	exit(1);
      }
      scope = scope->get_scope();
      // We want to insert the stmt before this classdefinition, if its still in a valid block
      cerr <<" ------->....... Found ClassDefinition Scope. New Scope is : " << scope->class_name() << "  stmt:" << stmt->class_name() <<"\n\n\n\n"<<endl;
    }
    // what is there is an array creation in a global scope
    else if (isSgGlobal(scope)) {
      cerr <<" ------->....... RuntimeInstrumentation :: WARNING - Scope not handled!!! : " << name << " : " << scope->class_name() << "\n\n\n\n" <<endl;
      // We need to add this new statement to the beginning of main
      // get the first statement in main as stmt
      stmt = mainFirst;
      scope=stmt->get_scope();
    }
    if (isSgBasicBlock(scope) ||
	isSgIfStmt(scope) ||
	isSgForStatement(scope)) {
      // build the function call : runtimeSystem-->createArray(params); ---------------------------
      SgExprListExp* arg_list = buildExprListExp();
      SgExpression* simplename = buildString(initName->get_name().str());
      appendExpression(arg_list, simplename);
      SgExpression* callName = buildString(initName->get_mangled_name().str());
      appendExpression(arg_list, callName);
#if 1
      if (derefExp)
    	  appendAddressAndSize(initName, derefExp, stmt, arg_list,0);
      else
#endif
    	  appendAddressAndSize(initName, varRefE, stmt, arg_list,0);

      SgExpression* filename = buildString(stmt->get_file_info()->get_filename());
      SgExpression* linenr = buildString(RoseBin_support::ToString(stmt->get_file_info()->get_line()));
      appendExpression(arg_list, filename);
      appendExpression(arg_list, linenr);

      SgExpression* linenrTransformed = buildString("x%%x");
      appendExpression(arg_list, linenrTransformed);

      appendExpression(arg_list, buildString(removeSpecialChar(stmt->unparseToString())));

      ROSE_ASSERT(roseAccessVariable);
      string symbolName2 = roseAccessVariable->get_name().str();
      //cerr << " >>>>>>>> Symbol Member: " << symbolName2 << endl;
      SgFunctionRefExp* memRef_r = buildFunctionRefExp(  roseAccessVariable);
      SgFunctionCallExp* funcCallExp = buildFunctionCallExp(memRef_r,
							    arg_list);
      SgExprStatement* exprStmt = buildExprStatement(funcCallExp);
      // insert new stmt (exprStmt) before (old) stmt
      insertStatementBefore(isSgStatement(stmt), exprStmt);
      string empty_comment = "";
      attachComment(exprStmt,empty_comment,PreprocessingInfo::before);
      string comment = "RS : Access Variable, paramaters : (name, mangl_name, address, sizeof(type), filename, line, line transformed, error Str)";
      attachComment(exprStmt,comment,PreprocessingInfo::before);
    } // basic block
    else if (isSgNamespaceDefinitionStatement(scope)) {
      cerr <<" ------------> RuntimeInstrumentation :: WARNING - Scope not handled!!! : " << name << " : " << scope->class_name() << "\n\n\n\n"<<endl;
    } else {
      cerr
	<< " -----------> RuntimeInstrumentation :: Surrounding Block is not Block! : "
	<< name << " : " << scope->class_name() << "  - " << stmt->unparseToString() << endl;
      ROSE_ASSERT(false);
    }
  } else {
    cerr
      << "RuntimeInstrumentation :: Surrounding Statement could not be found! "
      << stmt->class_name() << "  " << stmt->unparseToString() << endl;
    ROSE_ASSERT(false);
  }


}

