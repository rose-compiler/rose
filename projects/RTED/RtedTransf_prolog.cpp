#include <rose.h>

// DQ (2/9/2010): Testing use of ROE to compile ROSE.
#ifndef USE_ROSE

#include <string>
#include <iostream>

#include "RtedSymbols.h"
#include "DataStructures.h"
#include "RtedTransformation.h"

using namespace SageInterface;
using namespace SageBuilder;

void RtedTransformation::insertMainCloseCall()
{
  if (!mainBody)
  {
    std::cerr << "# note: file/project without main." << std::endl;
    return;
  }

  SgStatement*      last = mainBody->get_statements().back();
  SgScopeStatement* scope = last->get_scope();
  ROSE_ASSERT(scope);

  ROSE_ASSERT(symbols.roseCheckpoint);
  SgExprListExp*    arg_list = buildExprListExp();
  appendFileInfo( arg_list, scope, mainBody->get_endOfConstruct() );

  SgFunctionRefExp* memRef_r = buildFunctionRefExp(symbols.roseCheckpoint);
  SgExprStatement*  exprStmt = buildFunctionCallStmt(memRef_r, arg_list);
  const bool        insertBefore = isSgReturnStmt(last);

  SageInterface::insertStatement(last, exprStmt, insertBefore);

  std::string comment = "RS : Insert Finalizing Call to Runtime System to check if error was detected (needed for automation)";
  attachComment(exprStmt, comment, PreprocessingInfo::before);
}



void RtedTransformation::transformIfMain(SgFunctionDefinition& mainFunc)
{
    if (!is_main_func(mainFunc)) return;

    renameMain(mainFunc.get_declaration());

    // find the last statement
    SgBasicBlock*         block = mainFunc.get_body();
    ROSE_ASSERT(block);

    SgStatementPtrList&   stmts = block->get_statements();
    ROSE_ASSERT(!stmts.empty());

    mainBody = block;
    mainFirst = stmts.front();
}


void RtedTransformation::renameMain(SgFunctionDeclaration* sg_func)
{
  // grab symbol before any modifications.
  SgGlobal*                global_scope = isSgGlobal(sg_func->get_scope());
  SgName                   mainName = sg_func->get_name();
  SgFunctionSymbol*        symbol = global_scope->lookup_function_symbol(mainName, sg_func->get_type());

  global_scope->remove_symbol(symbol);
  delete (symbol); // avoid dangling symbol!

  // int main( int arc, char** argc)
  SgInitializedName*       arg1 = buildInitializedName("argc", buildIntType());
  SgType*                  type2 = buildPointerType(buildPointerType(buildCharType()));
  SgInitializedName*       arg2 = buildInitializedName("argv", type2);
  SgInitializedName*       arg3 = buildInitializedName("envp", type2);
  SgFunctionParameterList* paraList = buildFunctionParameterList();

  appendArg(paraList, arg1);
  appendArg(paraList, arg2);
  appendArg(paraList, arg3);

  SgFunctionDeclaration*   func = buildDefiningFunctionDeclaration( mainName,
                                                                    buildIntType(),
                                                                    paraList,
                                                                    global_scope
                                                                  );
  appendStatement(func, global_scope);

  // fill main body:
  SgBasicBlock*            body = func->get_definition()->get_body();

  //bupc_init_reentrant(&argc, &argv, &user_main);
  SgExpression*            bupc_arg1 = buildVarRefExp("argc",body);
  SgExpression*            bupc_arg2 = buildVarRefExp("argv",body);
  SgExpression*            bupc_arg3 = buildVarRefExp("envp",body);
  SgExprListExp*           arg_list = buildExprListExp();

  appendExpression(arg_list, bupc_arg1);
  appendExpression(arg_list, bupc_arg2);
  appendExpression(arg_list, bupc_arg3);

  SgExpression*      callmain = buildFunctionCallExp( "RuntimeSystem_original_main",
                                                      buildVoidType(),
                                                      arg_list,
                                                      body
                                                    );
  SgStatement*       st = buildVariableDeclaration( "exit_code",
                                                    buildIntType(),
                                                    buildAssignInitializer(callmain),
                                                    body
                                                  );
  appendStatement(st, body);

  ROSE_ASSERT(globalsInitLoc == NULL);
  globalsInitLoc = insertCheck(ilBefore, st, symbols.roseUpcAllInitialize, buildExprListExp());

  SgExprListExp*     arg_list2 = buildExprListExp();
  appendExpression(arg_list2, buildStringVal("RuntimeSystem.cpp:main"));

  ROSE_ASSERT(symbols.roseClose);
  SgFunctionRefExp*  closeref = buildFunctionRefExp(symbols.roseClose);
  SgExprStatement*   stmt5 = buildFunctionCallStmt(closeref,arg_list2);
  appendStatement(stmt5, body);

  // return 0;
  SgReturnStmt *     stmt2 = buildReturnStmt(buildVarRefExp("exit_code",body));
  appendStatement(stmt2, body);

  // rename it
  SgName             new_name("RuntimeSystem_original_main");
  sg_func->set_name(new_name);
  sg_func->get_declarationModifier().get_storageModifier().setExtern();

  // check if main has argc, argv, envp
  SgInitializedNamePtrList  args = sg_func->get_args();
  SgFunctionParameterList*  parameterList = sg_func->get_parameterList();

  SgPointerType* pType1 = new SgPointerType(SgTypeChar::createType());
  SgPointerType* pType2 = new SgPointerType(pType1);

  if(args.size() < 1) //add argc
      appendArg(parameterList, buildInitializedName(SgName("argc"), SgTypeInt::createType()));


  if(args.size() < 2) //add argc_v
      appendArg(parameterList, buildInitializedName(SgName("argv"), pType2));


  if(args.size() < 3) //add env_p
      appendArg(parameterList, buildInitializedName(SgName("envp"), pType2));


  //handle function symbol:remove the original one, insert a new one
  symbol = new SgFunctionSymbol(sg_func);
  global_scope->insert_symbol(new_name, symbol);
}

#endif
