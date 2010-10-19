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
#include "RtedSymbols.h"
#include "DataStructures.h"
#include "RtedTransformation.h"
//#include "RuntimeSystem.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;

void
RtedTransformation::insertIOFuncCall(RtedArguments* args  ) {

  SgStatement* stmt = args->stmt;
  ROSE_ASSERT(stmt);
  if (isSgStatement(stmt)) {
    SgScopeStatement* scope = stmt->get_scope();

    ROSE_ASSERT(scope);
    // fixed arguments
    // 1 = name
    // 2 = filename
    // 3 = lineNr
    // 4 = lineNrTransformed
    // 5 = unparsedStmt for Error message
    // 6 = Left Hand side variable, if assignment

    int extra_params = 6;
    // nr of args + 2 (for sepcialFunctions) + 6 =  args+6;
    int size = extra_params;
    // how many additional arguments does this function need?
    int dimFuncCall = getDimensionForFuncCall(args->f_name);
    size+=dimFuncCall;
    //SgIntVal* sizeExp = buildIntVal(size);
    SgExpression* callNameExp = buildString(args->f_name);

    //cerr << " >>>>>>>> Symbol VarRef: " << symbolName << endl;
    ROSE_ASSERT(symbols->roseIOFunctionCall);

    SgExprListExp* arg_list = buildExprListExp();
    //appendExpression(arg_list, sizeExp);
    appendExpression(arg_list, callNameExp);
    SgExpression* filename = buildString(stmt->get_file_info()->get_filename());
    SgExpression* linenr = buildString(RoseBin_support::ToString(stmt->get_file_info()->get_line()));
    appendExpression(arg_list, filename);
    appendExpression(arg_list, linenr);

    SgExpression* linenrTransformed = buildString("x%%x");
    appendExpression(arg_list, linenrTransformed);

    appendExpression(arg_list, buildString(removeSpecialChar(stmt->unparseToString())));
    // this one is new, it indicates the variable on the left hand side of the statment,
    // if available
    if (args->leftHandSideAssignmentExprStr)
      appendExpression(arg_list, args->leftHandSideAssignmentExprStr);
    else 
      appendExpression(arg_list, buildStringVal("NoAssignmentVar"));
    cerr << " ... Left hand side variable : " <<  args->leftHandSideAssignmentExpr << endl;

    // this is the file handle for fopen
	// for fclose or fget the file handle is in the parameter!
    if (args->f_name=="fopen") {
    	// file handle
    	appendExpression(arg_list, args->leftHandSideAssignmentExpr);
    	// 2 arguments
    	std::vector<SgExpression*>::const_iterator it = args->arguments.begin();
    	for (;it!=args->arguments.end();++it) {
    		SgExpression* exp = deepCopy(*it);
    		appendExpression(arg_list, exp);
    	}
    }
    else if (args->f_name=="fclose") {
		// file handle
    	std::vector<SgExpression*>::const_iterator it = args->arguments.begin();
    	for (;it!=args->arguments.end();++it) {
    		SgExpression* exp = deepCopy(*it);
    		appendExpression(arg_list, exp);
    	}
		// 0 arguments
    	appendExpression(arg_list, buildString("NULL"));
    	appendExpression(arg_list, buildString("NULL"));
    }
    else if (args->f_name=="fgetc") {
		// file handle
    	std::vector<SgExpression*>::const_iterator it = args->arguments.begin();
    	for (;it!=args->arguments.end();++it) {
    		SgExpression* exp = deepCopy(*it);
    		appendExpression(arg_list, exp);
    	}
    	// 0 arguments
    	appendExpression(arg_list, buildString("NULL"));
    	appendExpression(arg_list, buildString("NULL"));
    }
    else if (args->f_name=="fputc") {
		// 2 arguments in reverse order
    	std::vector<SgExpression*>::reverse_iterator it = args->arguments.rbegin();
    	for (;it!=args->arguments.rend();++it) {
    		SgExpression* exp = deepCopy(*it);
			cerr << " ++++++++++++++++++++++++ TYPE :::::: " << exp->get_type()->class_name() <<
				"  " << exp->unparseToString() << endl;
    		if (isSgTypeChar(exp->get_type()) ||
    			isSgTypeInt(exp->get_type()))
    			appendExpression(arg_list, buildAddressOfOp(exp));
    		else
    			appendExpression(arg_list, exp);
    	}
    	appendExpression(arg_list, buildString("NULL"));
    } else if (args->f_name=="std::fstream") {
      cerr << " Detected fstream" << endl;
      appendExpression(arg_list, args->varRefExp); // file handle
      std::vector<SgExpression*>::const_iterator it = args->arguments.begin();
      for (;it!=args->arguments.end();++it) {
          SgExpression* exp = deepCopy(*it);
          appendExpression(arg_list, exp);
      }
      // 0 arguments
      appendExpression(arg_list, buildString("NULL"));
      // appendExpression(arg_list, buildString("NULL"));

    } else {
      cerr <<"Unknown io function call " << args->f_name << endl;
      abort();
    }

    string symbolName2 = symbols->roseIOFunctionCall->get_name().str();
    //cerr << " >>>>>>>> Symbol Member: " << symbolName2 << endl;
    SgFunctionRefExp* memRef_r = buildFunctionRefExp(symbols->roseIOFunctionCall);
    SgFunctionCallExp* funcCallExp = buildFunctionCallExp(memRef_r, arg_list);
    SgExprStatement* exprStmt = buildExprStatement(funcCallExp);
    // create the function call and its comment
    if (args->f_name=="fopen")
        insertStatementAfter(isSgStatement(stmt), exprStmt);
    else
    	insertStatementBefore(isSgStatement(stmt), exprStmt);
    string empty_comment = "";
    attachComment(exprStmt,empty_comment,PreprocessingInfo::before);
    string comment = "RS : Calling Function, parameters: (#args, function name, filename, linenr, linenrTransformed, error message, left hand var, other parameters)";
    attachComment(exprStmt,comment,PreprocessingInfo::before);

  } else {
    cerr
      << "RuntimeInstrumentation :: Surrounding Statement could not be found! "
      << endl;
    exit(0);
  }

}

#endif
