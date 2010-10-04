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



void 
RtedTransformation::insertMainCloseCall(SgStatement* stmt) {
  if (isSgStatement(stmt)) {
    SgScopeStatement* scope = stmt->get_scope();
    ROSE_ASSERT(scope);
    if (isSgBasicBlock(scope)) {
#if 0
      SgVarRefExp* varRef_l =
	buildVarRefExp("runtimeSystem", globalScope);
      string symbolName = varRef_l->get_symbol()->get_name().str();
#endif
      ROSE_ASSERT(symbols->roseCheckpoint);

      SgExprListExp* arg_list = buildExprListExp();
      appendFileInfo( mainEnd , arg_list );

      string symbolName2 = symbols->roseCheckpoint->get_name().str();
      //cerr << " >>>>>>>> Symbol Member: " << symbolName2 << endl;
      SgFunctionRefExp* memRef_r = buildFunctionRefExp(
    		  symbols->roseCheckpoint);
      //      SgArrowExp* sgArrowExp = buildArrowExp(varRef_l, memRef_r);
      SgFunctionCallExp* funcCallExp = buildFunctionCallExp(memRef_r,
							    arg_list);
      SgExprStatement* exprStmt = buildExprStatement(funcCallExp);
      //cerr << " Last statement in main : " << stmt->class_name() << "  insertBefore : " << mainEndsWithReturn << endl;
      if (mainEndsWithReturn) {
#if 0
    	  if (mainHasBeenChanged==false) {
          // consider e.g.
          //    int main() {
          //        return foo();
          //    }
          //
          //  we must translate this to:
          //
          //    int main() {
          //        int rv = foo();
          //        RuntimeSystem_roseCheckpoint();
          //        return rv;
          //    }

        SgScopeStatement* scope = stmt->get_scope();
        // FIXME 2: better to create a guaranteed unique name
        SgName returnValueName = SgName( "RuntimeSystem_return_value" );

        // build the new function call
        //      int RuntimeSystem_return_value = <exp>;
        SgStatement* newFnCallStmt = buildVariableDeclaration(
            returnValueName,
            SgTypeInt::createType(),
            buildAssignInitializer( mainReturnStmt->get_expression() ),
            scope
        );

        // build the new return stmt
        //      return RuntimeSystem_return_value;
        SgStatement* newRtnStmt = buildReturnStmt(
            buildVarRefExp( returnValueName, scope )
        );


        insertStatementBefore( stmt, newFnCallStmt );
        insertStatementBefore(isSgStatement(stmt), exprStmt);
        replaceStatement( stmt, newRtnStmt );
    	  }
#endif
    	   insertStatementBefore(isSgStatement(stmt), exprStmt);
      } else
	insertStatementAfter(isSgStatement(stmt), exprStmt);
      string comment = "RS : Insert Finalizing Call to Runtime System to check if error was detected (needed for automation)";
      attachComment(exprStmt,comment,PreprocessingInfo::before);

    }

  } else {
    cerr
      << "RuntimeInstrumentation :: Prolog Surrounding Statement could not be found! "
      << stmt->class_name() << endl;
    ROSE_ASSERT(false);
  }
}



/* -----------------------------------------------------------
 * Insert the header files (Step 1)
 * -----------------------------------------------------------*/
void RtedTransformation::insertProlog(SgProject* proj) {
  cout << "Inserting headers ... " << endl;
  // grep all source (.c) files and insert headers
  Rose_STL_Container<SgNode*> vec =
    NodeQuery::querySubTree(proj,V_SgSourceFile);
  cerr << "Found source files : " << vec.size() << endl;
  Rose_STL_Container<SgNode*>::iterator it = vec.begin();
  for (;it!=vec.end();++it) {
    SgSourceFile* source = isSgSourceFile(*it);
    ROSE_ASSERT(source);
    cerr << "Creating pdf..." << endl;
    AstPDFGeneration pdf;
    pdf.generateWithinFile(source);
    globalScope = source->get_globalScope();
    pushScopeStack (isSgScopeStatement (globalScope));
    // this needs to be fixed
    //buildCpreprocessorDefineDeclaration(globalScope, "#define EXITCODE_OK 0");

#if 0
    // currently doesnt work -- crashes somewhere in wave
    insertHeader("rose.h",PreprocessingInfo::before,false,globalScope);
#else
    insertHeader("RuntimeSystem.h",PreprocessingInfo::before,false,globalScope);
    //insertHeader("iostream",PreprocessingInfo::before,true,globalScope);
    //insertHeader("map",PreprocessingInfo::before,true,globalScope);
    //insertHeader("string",PreprocessingInfo::before,true,globalScope);
#endif

    popScopeStack ();
  }
}

/* -----------------------------------------------------------
 * Insert
 * RuntimeSystem* runtimeSystem = new RuntimeSystem();
 * -----------------------------------------------------------*/
void RtedTransformation::insertRuntimeSystemClass() {
#if 0

  Sg_File_Info* fileInfo = globalScope->get_file_info();
  ROSE_ASSERT(runtimeClassSymbol);
  ROSE_ASSERT(runtimeClassSymbol->get_type());
  SgType* type = runtimeClassSymbol->get_type();
  //SgType* type  = new SgClassType();
  cerr << "Found type : " << type->class_name() << endl;

  SgExprListExp* exprList = buildExprListExp();
  ROSE_ASSERT(roseCreateArray->get_declaration());

  SgConstructorInitializer* constr = buildConstructorInitializer(roseCreateArray->get_declaration(),
								 exprList,type,false,false,false,false);
  SgExprListExp* exprList2 = buildExprListExp();
  //  SgNewExp* newexp = new SgNewExp(fileInfo,type,exprList2,constr,NULL,0,NULL);
  SgNewExp* newexp = buildNewExp(type,NULL,constr,NULL,0,NULL);
  SgAssignInitializer* init = buildAssignInitializer(newexp);

  SgVariableDeclaration* variable =
    buildVariableDeclaration("runtimeSystem",buildPointerType(type),init);
  SgStatement* st = isSgStatement(rememberTopNode->get_parent());
  insertStatement(st,variable,true);
#endif
}



void RtedTransformation::visit_checkIsMain(SgNode* n)
{
    SgFunctionDefinition* mainFunc = isSgFunctionDefinition(n);
    ROSE_ASSERT(mainFunc);
    string funcname = mainFunc->get_declaration()->get_name().str();
    if (funcname == "main")
    {
        renameMain(mainFunc->get_declaration());

        // find the last statement
        SgBasicBlock* block = mainFunc->get_body();
        ROSE_ASSERT(block);
        Rose_STL_Container<SgStatement*> stmts = block->get_statements();
        SgStatement* first = stmts.front();
        SgStatement* last = stmts.back();
        if (isSgReturnStmt(last))
        {
            mainEndsWithReturn = true;
            mainReturnStmt = isSgReturnStmt( last );
        }
        else
        mainEndsWithReturn=false;
        //cerr << " Last statement in main : " << last->class_name() << "  insertBefore : " <<
        //	RoseBin_support::resBool(mainEndsWithReturn) << endl;
        ROSE_ASSERT(last);
        // insert call to close before last statement (could be return)
	cerr << "\n\n@@@@@@@@@@@@@@@@@@@@@@ Changing mainLast in prolog" << endl;
        mainLast = last;
        mainFirst = first;
        mainBody = block;
        mainEnd = block->get_endOfConstruct();
    }
}


void RtedTransformation::renameMain(SgFunctionDeclaration* sg_func)
{
    ROSE_ASSERT(isMain(sg_func));

    // grab symbol before any modifications.
    SgGlobal* global_scope = isSgGlobal(sg_func->get_scope());
    ROSE_ASSERT(global_scope);

	SgFunctionSymbol * symbol = global_scope->lookup_function_symbol(SgName("main"), sg_func->get_type());
	global_scope->remove_symbol(symbol);
	delete (symbol); // avoid dangling symbol!

    // add another main!!
    if (global_scope) {
        // int main( int arc, char** argc)
        SgInitializedName* arg1 = buildInitializedName("argc", buildIntType());
        SgType* type2= buildPointerType(buildPointerType(buildCharType()));
        SgInitializedName* arg2 = buildInitializedName("argv", type2);
        SgInitializedName* arg3 = buildInitializedName("envp", type2);
        SgFunctionParameterList * paraList = buildFunctionParameterList();
        appendArg(paraList, arg1);
        appendArg(paraList, arg2);
        appendArg(paraList, arg3);

        SgFunctionDeclaration * func = buildDefiningFunctionDeclaration
                ("main",buildIntType(),paraList,global_scope);
        appendStatement(func,global_scope);

        // fill main body:
        SgBasicBlock* body = func->get_definition()->get_body();

         //bupc_init_reentrant(&argc, &argv, &user_main);
         SgExpression * bupc_arg1 = (buildVarRefExp("argc",body));
         SgExpression * bupc_arg2 = (buildVarRefExp("argv",body));
         SgExpression * bupc_arg3 = (buildVarRefExp("envp",body));
         SgExprListExp* arg_list = buildExprListExp();
         appendExpression(arg_list,bupc_arg1);
         appendExpression(arg_list,bupc_arg2);
         appendExpression(arg_list,bupc_arg3);

         SgExpression* stmt1 = buildFunctionCallExp
             ("RuntimeSystem_original_main",buildVoidType(),arg_list,body);

		 SgStatement* st = buildVariableDeclaration("exit_code",buildIntType(),
					 buildAssignInitializer(stmt1),body);
         appendStatement(st, body);

         SgExprListExp* arg_list2 = buildExprListExp();
         appendExpression(arg_list2,buildStringVal("RuntimeSystem.cpp:main"));
         SgExprStatement* stmt5 = buildFunctionCallStmt
             ("RuntimeSystem_roseRtedClose",buildVoidType(),arg_list2,body);
         appendStatement(stmt5, body);


         //return 0;
        SgReturnStmt * stmt2 = buildReturnStmt(buildVarRefExp("exit_code",body));
        appendStatement(stmt2, body);

    }

    // rename it
    SgName new_name = SgName("RuntimeSystem_original_main");
    sg_func->set_name(new_name);
    sg_func->get_declarationModifier().get_storageModifier().setExtern();

    // check if main has argc, argv, envp
    SgInitializedNamePtrList args = sg_func->get_args();
    SgFunctionParameterList * parameterList = sg_func->get_parameterList();

    SgPointerType *pType1= new SgPointerType(SgTypeChar::createType());
    SgPointerType *pType2= new SgPointerType(pType1);

    if(args.size() < 1) //add argc
        appendArg(parameterList, buildInitializedName(SgName("argc"),SgTypeInt::createType()));


    if(args.size() < 2) //add argc_v
        appendArg(parameterList, buildInitializedName(SgName("argv"),pType2));


    if(args.size() < 3) //add env_p
        appendArg(parameterList, buildInitializedName(SgName("envp"),pType2));


    //handle function symbol:remove the original one, insert a new one
    symbol = new SgFunctionSymbol(sg_func);
    global_scope->insert_symbol(new_name, symbol);
}

#endif
