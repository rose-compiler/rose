/*
 * RtedTransf_iofunccall.cpp
 *
 *  Created on: Jul 9, 2009
 *      Author: panas2
 */
#include <rose.h>

// DQ (2/9/2010): Testing use of ROE to compile ROSE.
#ifndef USE_ROSE

#include <string>

#include "RtedTransformation.h"
#include "DataStructures.h"
//#include "RuntimeSystem.h"

namespace SI = SageInterface;
using namespace SageBuilder;



void
RtedTransformation::insertIOFuncCall(RtedArguments* args)
{
  static const std::string comment("RS : Calling Function, parameters: (#args, function name, filename, linenr, linenrTransformed, error message, left hand var, other parameters)");

  SgStatement*                stmt = args->stmt;
  ROSE_ASSERT(stmt);

  SgScopeStatement*           scope = stmt->get_scope();
  ROSE_ASSERT(scope);

  SgExpression*               arg_name = buildStringVal(args->f_name);
  SgExprListExp*              arg_list = buildExprListExp();

  SI::appendExpression(arg_list, arg_name);

  // \pp the two following arguments are currently not used (as of 01/01/11)
  {
    SgExpression*  arg_lhs_exprstr = (args->leftHandSideAssignmentExprStr != NULL)
                                            ? args->leftHandSideAssignmentExprStr
                                            : buildStringVal("NoAssignmentVar")
                                            ;

    std::cerr << " ... Left hand side variable : " <<  args->leftHandSideAssignmentExpr << std::endl;
    SI::appendExpression(arg_list, buildStringVal(removeSpecialChar(stmt->unparseToString())));
    SI::appendExpression(arg_list, arg_lhs_exprstr);
  }

  bool                        insertBefore = true;
  SgExpressionPtrList&        roseArgs = args->arguments;
  SgExpression*               arg_file = NULL;
  SgExpression*               arg_1 = NULL;
  SgExpression*               arg_2 = NULL;

  // generate the arguments based on the actual function being called
  // \pp I do not understand why some expressions are cloned / copied
  //     before being added to the exprlist, and others are not.
  if (args->f_name=="fopen")
  {
    ROSE_ASSERT(roseArgs.size() == 2);

    // file handle
    arg_file = args->leftHandSideAssignmentExpr;
    arg_1 = SI::deepCopy(roseArgs.front());
    arg_2 = SI::deepCopy(roseArgs.back());
    insertBefore = false;
  }
  else if (  args->f_name=="fclose"
          || args->f_name=="fgetc"
          )
  {
    ROSE_ASSERT(roseArgs.size() == 1);

    // file handle
    arg_file = SI::deepCopy(roseArgs.front());
    arg_1 = buildStringVal("NULL");
    arg_2 = buildStringVal("NULL");
  }
  else if (args->f_name=="fputc")
  {
    ROSE_ASSERT(roseArgs.size() == 2);

    // \pp this assert is here b/c the original coding style was
    //     just too funny :(
    ROSE_ASSERT(  isSgTypeChar( roseArgs.front()->get_type() )
               || isSgTypeInt( roseArgs.front()->get_type() )
               );

    // 2 arguments in reverse order
    arg_file = SI::deepCopy(roseArgs.back());

    // \pp not sure why this is needed (and if this even generates
    //     compileable code).
    arg_1 = buildAddressOfOp(SI::deepCopy(roseArgs.front()));
    arg_2 = buildStringVal("NULL");
  }
  else if (args->f_name=="std::fstream")
  {
    // \pp this code seems to be broken ...
    //     not sure what we should test with C++ fstreams
    //     why not also iostream, etc.

    // \pp The original code seems to assume that there is exactly
    //     one argument..., thus adding the following assert.
    ROSE_ASSERT(roseArgs.size() == 1);

    std::cerr << " Detected fstream" << std::endl;

    arg_file = args->varRefExp;
    arg_1 = SI::deepCopy(roseArgs.front());
    arg_2 = buildStringVal("NULL");
  } else {
    std::cerr <<"Unknown io function call " << args->f_name << std::endl;
    abort();
  }

  ROSE_ASSERT(arg_file && arg_1 && arg_2);

  SI::appendExpression( arg_list, arg_file );
  SI::appendExpression( arg_list, arg_1 );
  SI::appendExpression( arg_list, arg_2 );
  appendFileInfo(arg_list, stmt);

  ROSE_ASSERT(symbols.roseIOFunctionCall);
  SgFunctionRefExp* memRef_r = buildFunctionRefExp(symbols.roseIOFunctionCall);
  SgExprStatement*  exprStmt = buildFunctionCallStmt(memRef_r, arg_list);

  // create the function call and its comment
  if (insertBefore)
    SI::insertStatementBefore(stmt, exprStmt);
  else
    SI::insertStatementAfter(stmt, exprStmt);

  SI::attachComment(exprStmt,"",PreprocessingInfo::before);
  SI::attachComment(exprStmt,comment,PreprocessingInfo::before);
}

#endif
