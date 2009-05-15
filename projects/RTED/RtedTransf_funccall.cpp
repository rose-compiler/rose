#include <rose.h>
#include <string>
#include "RtedSymbols.h"
#include "DataStructures.h"
#include "RtedTransformation.h"
//#include "RuntimeSystem.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;


void 
RtedTransformation::insertStackCall(RtedArguments* args  ) {
  insertStackCall(args,true);
  insertFuncCall(args);
  insertStackCall(args,false);
}

void 
RtedTransformation::insertStackCall(RtedArguments* args, bool before  ) {
  //  SgStatement* stmt = getSurroundingStatement(args->varRefExp);
  SgStatement* stmt = args->stmt;
  ROSE_ASSERT(stmt);
  if (isSgStatement(stmt)) {
    SgScopeStatement* scope = stmt->get_scope();
    ROSE_ASSERT(scope);

    SgExpression* callNameExp = buildString(args->name);
    SgExpression* callNameExp2 = buildString(args->mangled_name);
    SgExpression* boolVal = buildString("true");
    if (before==false) {
      delete boolVal;
      boolVal = buildString("false");
    }
    //cerr << " >>>>>>>> Symbol VarRef: " << symbolName << endl;
    ROSE_ASSERT(roseCallStack);

    SgExprListExp* arg_list = buildExprListExp();
    appendExpression(arg_list, callNameExp);
    appendExpression(arg_list, callNameExp2);
    appendExpression(arg_list, boolVal);

    string symbolName2 = roseCallStack->get_name().str();
    //cerr << " >>>>>>>> Symbol Member: " << symbolName2 << endl;
    SgFunctionRefExp* memRef_r = buildFunctionRefExp(
						     roseCallStack);
    SgFunctionCallExp* funcCallExp = buildFunctionCallExp(memRef_r,
							  arg_list);
    SgExprStatement* exprStmt = buildExprStatement(funcCallExp);
    if (before)
      insertStatementBefore(isSgStatement(stmt), exprStmt);
    else
      insertStatementAfter(isSgStatement(stmt), exprStmt);
    string empty_comment = "";
    attachComment(exprStmt,empty_comment,PreprocessingInfo::before);
    string comment = "RS : Putting variables on stack)";
    if (!before)
      comment = "RS : Popping variables from stack)";
    attachComment(exprStmt,comment,PreprocessingInfo::before);
  } else {
    cerr
      << "RuntimeInstrumentation :: Stack : Surrounding Statement could not be found! "
      << endl;
    exit(0);
  }

}

void 
RtedTransformation::insertFuncCall(RtedArguments* args  ) {
  //  SgStatement* stmt = getSurroundingStatement(args->varRefExp);
  SgStatement* stmt = args->stmt;
  ROSE_ASSERT(stmt);
  if (isSgStatement(stmt)) {
    SgScopeStatement* scope = stmt->get_scope();

    ROSE_ASSERT(scope);
    // 4 fixed arguments
    // 1 = name
    // 2 = filename
    // 3 = lineNr
    // 4 = unparsedStmt for Error message

    int extra_params = 6;
    // nr of args + 2 (for sepcialFunctions) + 4 =  args+6;
    int size = extra_params+args->arguments.size();
    SgIntVal* sizeExp = buildIntVal(size);
    SgExpression* callNameExp = buildString(args->name);

    //cerr << " >>>>>>>> Symbol VarRef: " << symbolName << endl;
    ROSE_ASSERT(roseFunctionCall);

    SgExprListExp* arg_list = buildExprListExp();
    appendExpression(arg_list, sizeExp);
    appendExpression(arg_list, callNameExp);
    SgExpression* filename = buildString(stmt->get_file_info()->get_filename());
    SgExpression* linenr = buildString(RoseBin_support::ToString(stmt->get_file_info()->get_line()));
    appendExpression(arg_list, filename);
    appendExpression(arg_list, linenr);
    appendExpression(arg_list, buildString(removeSpecialChar(stmt->unparseToString())));
    
    // iterate over all arguments of the function call, e.g. strcpy(arg1, arg2);
    std::vector<SgExpression*>::const_iterator it = args->arguments.begin();
    for (;it!=args->arguments.end();++it) {
      SgExpression* exp = deepCopy(*it);
      if (isSgUnaryOp(exp))
	exp = isSgUnaryOp(exp)->get_operand();
      //cerr << " exp = " << exp->class_name() << endl;
      // argument is a variable (varRef)
      if (isSgVarRefExp(exp)) {
	SgVarRefExp* var = isSgVarRefExp(exp);
	SgType* type = var->get_type();
	SgType* base_type = NULL;
	if (isSgArrayType(type) )
	  base_type= isSgArrayType(type)->get_base_type();
	if ( isSgPointerType(type))
	  base_type= isSgPointerType(type)->get_base_type();

	cerr << " isSgVarRefExp :: type : " << type->class_name() << endl;
	if (base_type)
	  cerr <<"     base_type: " << base_type->class_name() << endl;
	if (isSgTypeChar(type) || isSgTypeChar(base_type)) {
	  // this is a char* variable that we use
	  // we should represent it by its mangled name of the declaration!
	  // we add variable first and then mangled name
	  appendExpression(arg_list, var);
	  SgExpression* manglName = buildString(var->get_symbol()->get_declaration()->get_mangled_name().str());
	  appendExpression(arg_list, manglName);
      	} else {
	  SgFunctionRefExp* memRef_r2 = NULL;
	  if (isSgTypeInt(type)) {
	    ROSE_ASSERT(roseConvertIntToString);
	    memRef_r2 = buildFunctionRefExp( roseConvertIntToString);
	  } else {
	    cerr << "RtedTransformation - unknown type : " << type->class_name() << endl;
	    exit(1);
	  }
	  ROSE_ASSERT(memRef_r2);
	  string symbolName3 = roseConvertIntToString->get_name().str();
	  cerr << " >>>>>>>> Symbol Member::: " << symbolName3 << endl;

	  SgExprListExp* arg_list2 = buildExprListExp();
	  appendExpression(arg_list2, var);
	  SgFunctionCallExp* funcCallExp2 = buildFunctionCallExp(memRef_r2,
								 arg_list2);
	  ROSE_ASSERT(funcCallExp2);
	  //	  SgCastExp* point= buildCastExp(funcCallExp2,buildPointerType(buildCharType()));
	  appendExpression(arg_list, funcCallExp2);
	  cerr << " Created Function call  convertToString" << endl;
	}
      } else {
	// if it is already a string, dont add extra quates
	cerr << " isNotSgVarRefExp exp : " << exp->class_name() << "   " << exp->unparseToString() << endl;
	if (isSgStringVal(exp))
	  appendExpression(arg_list, buildString(isSgStringVal(exp)->get_value()));
	else {
	  // default create a string
	  SgExpression* stringExp = buildString(exp->unparseToString());
	  appendExpression(arg_list, stringExp);
	}
      }
    }

    string symbolName2 = roseFunctionCall->get_name().str();
    //cerr << " >>>>>>>> Symbol Member: " << symbolName2 << endl;
    SgFunctionRefExp* memRef_r = buildFunctionRefExp(roseFunctionCall);
    SgFunctionCallExp* funcCallExp = buildFunctionCallExp(memRef_r, arg_list);
    SgExprStatement* exprStmt = buildExprStatement(funcCallExp);
    insertStatementBefore(isSgStatement(stmt), exprStmt);
    string empty_comment = "";
    attachComment(exprStmt,empty_comment,PreprocessingInfo::before);
    string comment = "RS : Calling Function, parameters: (#args, function name, filename, linenr, error message, other parameters)";
    attachComment(exprStmt,comment,PreprocessingInfo::before);

  } else {
    cerr
      << "RuntimeInstrumentation :: Surrounding Statement could not be found! "
      << endl;
    exit(0);
  }
  
}

bool 
RtedTransformation::isInterestingFunctionCall(std::string name) {
  bool interesting=false;
  if (name=="memcpy" || 
      name=="memmove" || 
      name=="strcpy" || 
      name=="strncpy" ||
      name=="strcat" ||
      name=="strncat"
      )
    interesting=true;
  return interesting;
}


/***************************************************************
 * Check if the current node is a "interesting" function call
 **************************************************************/
void RtedTransformation::visit_isFunctionCall(SgNode* n) {
  SgFunctionCallExp* fcexp = isSgFunctionCallExp(n);
  if (fcexp) {
    SgExprListExp* exprlist = isSgExprListExp(fcexp->get_args());
    SgFunctionRefExp* refExp = isSgFunctionRefExp(fcexp->get_function());
    ROSE_ASSERT(refExp);
    SgFunctionDeclaration* decl = isSgFunctionDeclaration(refExp->getAssociatedFunctionDeclaration ());
    ROSE_ASSERT(decl);
    string name = decl->get_name();
    string mangled_name = decl->get_mangled_name().str();
    cerr <<"Found a function call " << name;
    cerr << "   : fcexp->get_function() : " << fcexp->get_function()->class_name() << endl;
    if (isInterestingFunctionCall(name)) {
    //if (RuntimeSystem_isInterestingFunctionCall((char*)name.c_str())==1) {
      vector<SgExpression*> args;
      Rose_STL_Container<SgExpression*> expr = exprlist->get_expressions();
      Rose_STL_Container<SgExpression*>::const_iterator it = expr.begin();
      for (;it!=expr.end();++it) {
	SgExpression* ex = *it;
	args.push_back(ex);
      }
      SgStatement* stmt = getSurroundingStatement(refExp);
      ROSE_ASSERT(stmt);
      RtedArguments* funcCall = new RtedArguments(name,
						  mangled_name,
						  refExp,
						  stmt,
						  args						  
						  );
      ROSE_ASSERT(funcCall);
      cerr << " Is a interesting function" << endl;
      function_call.push_back(funcCall);
    }
  }
  
}
