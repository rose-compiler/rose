#include <rose.h>

// DQ (2/9/2010): Testing use of ROE to compile ROSE.
#ifndef USE_ROSE

#include <string>
#include <iostream>

#include "sageFunctors.h"

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

    renameMain(mainFunc);

    // find the last statement
    SgBasicBlock*         block = mainFunc.get_body();
    ROSE_ASSERT(block);

    SgStatementPtrList&   stmts = block->get_statements();
    ROSE_ASSERT(!stmts.empty());

    mainBody = block;
    mainFirst = stmts.front();
}


static
void appendMainParamList(SgFunctionParameterList& plist, size_t szargs)
{
  if (szargs == 0) return;

  SgInitializedName* arg1 = buildInitializedName("argc", buildIntType());
  appendArg(&plist, arg1);
  if (szargs == 1) return;

  SgType*            charArrPtr = buildPointerType(buildPointerType(buildCharType()));
  SgInitializedName* arg2 = buildInitializedName("argv", charArrPtr);
  appendArg(&plist, arg2);
  if (szargs == 2) return;

  SgInitializedName* arg3 = buildInitializedName("envp", charArrPtr);
  appendArg(&plist, arg3);
}


void RtedTransformation::renameMain(SgFunctionDefinition& maindef)
{
  // \todo reimplement in terms of wrapFunction

  // grab symbol before any modifications.
  SgFunctionDeclaration*    sg_func = maindef.get_declaration();
  SgGlobal*                global_scope = isSgGlobal(sg_func->get_scope());
  SgName                   mainName = sg_func->get_name();
  SgFunctionSymbol*        symbol = global_scope->lookup_function_symbol(mainName, sg_func->get_type());
  SgInitializedNamePtrList& args = sg_func->get_args();
  const size_t              argssize = args.size();

  global_scope->remove_symbol(symbol);
  delete (symbol); // avoid dangling symbol!

  SgFunctionParameterList* paraList = buildFunctionParameterList();

  appendMainParamList(*paraList, argssize);

  SgFunctionDeclaration*   func = buildDefiningFunctionDeclaration( mainName,
                                                                    buildIntType(),
                                                                    paraList,
                                                                    global_scope
                                                                  );
  appendStatement(func, global_scope);

  // fill main body:
  SgFunctionDefinition*     newFundef = func->get_definition();
  SgBasicBlock*             body = newFundef->get_body();
  SgInitializedNamePtrList& param_decls = paraList->get_args();
  SgExprListExp*           arg_list = buildExprListExp();

  std::transform( param_decls.begin(), param_decls.end(), sg::sage_inserter(*arg_list), sg::VarRefBuilder(*newFundef) );

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

  //handle function symbol:remove the original one, insert a new one
  symbol = new SgFunctionSymbol(sg_func);
  global_scope->insert_symbol(new_name, symbol);
}

#endif
