#include <rose.h>

// DQ (2/9/2010): Testing use of ROE to compile ROSE.
#ifndef USE_ROSE

#include <string>
#include <boost/foreach.hpp>
#include "RtedSymbols.h"
#include "DataStructures.h"
#include "RtedTransformation.h"
//#include "RuntimeSystem.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;

/*********************************************************
 * Check if a function call is interesting, i.e. contains a
 * call to a function that we need to check the parameters of
 ********************************************************/
bool isStringModifyingFunctionCall(const std::string& name) {
   return (  name == "memcpy"
          || name == "memmove"
          || name == "strcpy"
          || name == "strncpy"
          || name == "strcat"
          || name == "strchr"
          || name == "strlen"     // \pp actually not string modifying...
                                  //     and I suspect that this gives the wrong
                                  //     size of the argument lists for
                                  //     the (commented out) call in
                                  //     RtedTransformation::insertFuncCall.
          || name == "strncat"
          || name == "strpbrk"
          || name == "strspn"
          || name == "strstr"
          );
}

/***************************************************************
 * This dimension is used to calculate additional parameters
 * necessary for the function call, e.g.
 * if dim = 1 then the parameter is followed by one more
 * element wich is its size
 **************************************************************/
static
size_t getDimensionForFuncCall(const std::string& name)
{
   int dim = 0;

   if (  name == "memcpy"
      || name == "memmove"
      || name == "strcpy"
      || name == "strncpy"
      || name == "strcat"
      || name == "strchr"
      || name == "strncat"
      || name == "strpbrk"
      || name == "strspn"
      || name == "strstr"
      || name == "fopen"
      )
   {
     dim = 2;
   }
   else if (name == "strlen")
   {
     dim = 1;
   }

   return dim;
}

bool isFileIOFunctionCall(const std::string& name) {
   return (  name == "fopen"
          || name == "fgetc"
          || name == "fputc"
          || name == "fclose"
          || name == "std::fstream"
          );
}

bool isFunctionCallOnIgnoreList(const std::string& name) {
   return (  name == "calloc"
          || name == "free"
          || name == "malloc"
          || name == "printf"
          || name == "realloc"
          || name == "upc_all_alloc"
          || name == "upc_alloc"
          || name == "upc_free"
          || name == "upc_global_alloc"
          );
}

void RtedTransformation::insertFuncCall(RtedArguments* args)
{
  // fixed arguments
  // 1 = name
  // 2 = unparsedStmt for Error message
  // 3 = Left Hand side variable, if assignment
  // 4 = SourceInfo
  // 5 = number of the following arguments (\pp ??)

  //  SgStatement* stmt = getSurroundingStatement(args->varRefExp);
  SgStatement*      stmt = args->stmt;
  ROSE_ASSERT(stmt);

  SgScopeStatement* scope = stmt->get_scope();
  ROSE_ASSERT(scope);

  SgExpression*     callNameExp = buildStringVal(args->f_name);
  SgExprListExp*    arg_list = buildExprListExp();

  appendExpression(arg_list, callNameExp);
  appendExpression(arg_list, buildStringVal(removeSpecialChar(stmt->unparseToString())));

  // this one is new, it indicates the variable on the left hand side of the statment,
  // if available
  ROSE_ASSERT(args->leftHandSideAssignmentExprStr);
  appendExpression(arg_list, args->leftHandSideAssignmentExprStr);
  cerr << " ... Left hand side variable : " << args->leftHandSideAssignmentExprStr << endl;

  appendFileInfo(arg_list, stmt);

  // how many additional arguments does this function need?
  const size_t      dimFuncCall = getDimensionForFuncCall(args->f_name);

  // nr of args + 2 (for sepcialFunctions) + 6 =  args+6;
  SgExpressionPtrList& rose_args = args->arguments;
  const size_t         size = dimFuncCall + rose_args.size();
  SgIntVal*            sizeExp = buildIntVal(size);

  appendExpression(arg_list, sizeExp);

  // \pp modifying the dimension count looks strange (in fact when comparing
  //     isStringModifyingFunctionCall to getDimensionForFuncCall one
  //     notices that the only affected call function is strlen)!
  //     Since I do not understand the code below, the refactored/reformmatef
  //     code below preserves the original behaviour.
  //     dimIsTwo is newly introduced and is true when the original code had
  //       dimFuncCall = 2.
  //     Subsequently, dimIsTwo replaces all occurances of (dimFuncCall == 2).
  const bool                          dimIsTwo = (dimFuncCall == 2) || isStringModifyingFunctionCall(args->f_name);

  // iterate over all arguments of the function call, e.g. strcpy(arg1, arg2);
  SgExpressionPtrList::const_iterator it = rose_args.begin();

  for (; it != rose_args.end(); ++it)
  {
     // \pp deep copy... I wonder who frees the memory
     //     if not the complete expression is needed,
     //     as e.g. in the unary branch?
     SgExpression* exp = deepCopy(*it);

     // ************ unary operation *******************************
     if (isSgUnaryOp(exp))
     {
        exp = isSgUnaryOp(exp)->get_operand();
     }
     else if (isSgVarRefExp(exp))
     {
        SgVarRefExp* var = isSgVarRefExp(exp);
        SgType*      vartype = var->get_type();
        SgType*      base_type = get_arrptr_base(vartype);

        cerr << " isSgVarRefExp :: type : " << vartype->class_name() << endl;
        cerr << "     base_type: " << base_type->class_name() << endl;

        // --------- varRefExp is TypeChar -----------------------
        if (isSgTypeChar(vartype) || isSgTypeChar(base_type)) {
           string             name = var->get_symbol()->get_declaration()->get_name().str();
           SgInitializedName* initName = var->get_symbol()->get_declaration();
           ROSE_ASSERT(initName);

           SgType*            inittype = initName->get_typeptr();
           ROSE_ASSERT(inittype);

           SgArrayType*       array = isSgArrayType(inittype);

           if (array)
           {
              SgExpression * expr = array->get_index();

              cerr << " Found 00 - btype : " << array->get_base_type()->class_name() << "  index type: " << expr << endl;

              // this is the variable that we pass
              appendExpression(arg_list, var);
              if (dimIsTwo)
              {
                expr = ( expr == 0 ? buildStringVal("00")
                                   : buildStringVal(expr->unparseToString() + "\0")
                       );

                appendExpression(arg_list, expr);
              }

              cerr << ">>> Found variable : " << name << "  with size : " << expr->unparseToString() << "  and val : " << var << endl;
           }
           else if (isSgPointerType(inittype))
           {
              cerr << "Cant determine the size of the following object : " << var->class_name() << "  with base type : "
                    << base_type->class_name() << endl;

              // this is a pointer to char* but not an array allocation yet
              // We have to pass var so we can check whether the memories overlap!
              appendExpression(arg_list, var);

              if (dimIsTwo)
              {
                // if this is a pointer to e.g. malloc we cant statically determine the
                // size of the allocated address. We need to pass the variable
                // and check in the runtime system if the size for the variable is known!
                SgExpression* manglName = buildStringVal(var->get_symbol()->get_declaration()->get_mangled_name().str());

                appendExpression(arg_list, manglName);
              }
           }
           else
           {
              cerr << "Cant determine the size of the following object : " << var->class_name() << "  with type : "
                   << inittype->class_name() << endl;
              cerr << "   and base type : " << base_type->class_name() << endl;

              // this is most likely a single char, the size of it is 1
              SgExpression* andSign = buildAddressOfOp(var);
              SgExpression* charCast = buildCastExp(andSign, buildPointerType(buildCharType()));

              appendExpression(arg_list, charCast);

              if (dimIsTwo)
              {
                SgExpression* numberToString = buildStringVal("001");

                appendExpression(arg_list, numberToString);
              }
           }
        }
        // --------- varRefExp is PointerType -----------------------
        else if (isSgPointerType(vartype))
        {
           // handle pointers as parameters
           cerr << "RtedTransformation - unknown type : " << vartype->class_name() << endl;
           cerr << "   and base type : " << base_type->class_name() << "  var:" << var->unparseToString() << "   stmt:"
                << stmt->unparseToString() << endl;

           if (var) {
              SgInitializedName* initName = var->get_symbol()->get_declaration();
              ROSE_ASSERT(initName);
              // if it is a initName in a parameterlist than we do not want to send it
              // as a mangled name because it will be something like: L22R__ARG1
              // instead we send it as a normal name that can be found on the stack
              SgExpression* manglName = buildStringVal(initName->get_mangled_name().str());
              if (isSgFunctionParameterList(initName->get_parent())) {
                 //	      cerr << "\n!!!!!!!!!!parent of initName = " << initName->get_parent()->class_name() << endl;
                 manglName = buildStringVal(initName->get_name().str());
              }
              ROSE_ASSERT(manglName);
              appendExpression(arg_list, manglName);
           }
           //ROSE_ASSERT(false);
        }
        // --------- varRefExp is IntegerType -----------------------
        else
        {
           // handle integers as parameters
           if (!isSgTypeInt(vartype)) {
              cerr << "RtedTransformation - unknown type : " << vartype->class_name() << endl;
              ROSE_ASSERT(false);
           }

           ROSE_ASSERT(dimIsTwo);
           ROSE_ASSERT(symbols.roseConvertIntToString);

           SgFunctionRefExp*  memRef_r2 = buildFunctionRefExp(symbols.roseConvertIntToString);
           SgExprListExp*     arg_list2 = buildExprListExp();

           appendExpression(arg_list2, var);
           SgFunctionCallExp* funcCallExp2 = buildFunctionCallExp(memRef_r2, arg_list2);

           appendExpression(arg_list, funcCallExp2);
           cerr << " Created Function call  convertToString" << endl;
        }
     }
     // --------- this is not a varRefExp ----
     else
     {
        // if it is already a string, dont add extra quotes
        cerr << " isNotSgVarRefExp exp : " << exp->class_name() << "   " << exp->unparseToString() << endl;
        if (isSgStringVal(exp))
        {
           string theString = isSgStringVal(exp)->get_value();

           appendExpression(arg_list, buildStringVal(theString + "\0"));

           if (dimIsTwo)
           {
              size_t        sizeString = theString.size() + 1; // add the '\0' to it
              string        sizeStringStr = RoseBin_support::ToString(sizeString);
              SgExpression* numberToString = buildStringVal(sizeStringStr);

              appendExpression(arg_list, numberToString);
           }
        }
        else
        {
           // default create a string
           string        theString = exp->unparseToString();
           SgExpression* stringExp = buildStringVal(theString + "\0");

           appendExpression(arg_list, stringExp);

           if (dimIsTwo)
           {
             size_t sizeString = theString.size();
             string sizeStringStr = RoseBin_support::ToString(sizeString);
             SgExpression* numberToString = buildStringVal(sizeStringStr);

             appendExpression(arg_list, numberToString);
           }
        }
     }
  }

  ROSE_ASSERT(symbols.roseFunctionCall);
  checkBeforeStmt( stmt,
                   symbols.roseFunctionCall,
                   arg_list,
                   "RS : Calling Function, parameters: (#args, function name, filename, linenr, linenrTransformed, error message, left hand var, other parameters)"
                 );
}

static
SgType* select_type(SgExpression* expr)
{
  ROSE_ASSERT(expr);

  return expr->get_type();
}


template <class RoseFunctionRefExp>
static
void insertAssertFunctionSignature(RtedTransformation& rt, SgFunctionCallExp* fce, RoseFunctionRefExp* fnref)
{
  // we aren't able to check the function signature at compile time, so we
  // insert a check to do so at runtime

  using SageInterface::appendExpression;
  using SageBuilder::buildExprListExp;
  using SageBuilder::buildStringVal;
  using SageBuilder::buildIntVal;

  ROSE_ASSERT(fce && fnref);

  SgExprListExp*       arg_list = buildExprListExp();
  SgExpressionPtrList& fnArgs = fce->get_args()->get_expressions();
  SgType*              fnReturn = isSgFunctionType( fnref->get_type() ) -> get_return_type();
  SgTypePtrList        fnArgTypes;

  std::transform(fnArgs.begin(), fnArgs.end(), std::back_inserter(fnArgTypes), select_type);

  appendExpression( arg_list, buildStringVal( fnref->get_symbol()->get_name()) );
  rt.appendSignature( arg_list, fnReturn, fnArgTypes );
  rt.appendFileInfo( arg_list, fce );

  checkBeforeParentStmt(fce, rt.symbols.roseAssertFunctionSignature, arg_list);
}

void RtedTransformation::insertAssertFunctionSignature(SgFunctionCallExp* fncall)
{
   ROSE_ASSERT(fncall);

   SgFunctionRefExp* fn_ref = isSgFunctionRefExp(fncall -> get_function());
   if (fn_ref)
   {
     ::insertAssertFunctionSignature(*this, fncall, fn_ref);
     return;
   }

#if 0
   // \pp not needed according to the code (and comment) below
   SgDotExp* dotExp = isSgDotExp(fncall -> get_function());
   if (dotExp) {
      SgMemberFunctionRefExp* mfn_ref = isSgMemberFunctionRefExp(dotExp->get_rhs_operand());
      // tps: actually, we might not need this for C++
      return;
      // ::insertAssertFunctionSignature(*this, fncall, mfn_ref);
   }
#endif
}

void RtedTransformation::insertFreeCall(SgExpression* free)
{
   SgFunctionCallExp* free_call = isSgFunctionCallExp(free);
   SgDeleteExp*       del_exp = isSgDeleteExp(free);

   ROSE_ASSERT( free_call || del_exp );

   // stmt wraps a call to free -- most likely an expression statement
   // alert the RTS before the call to detect errors such as double-free
   SgStatement*  stmt = getSurroundingStatement(free);
   SgExpression* address_expression = NULL;
   SgExpression* from_malloc_expression = NULL;

   if (free_call) {
      SgExpressionPtrList& args = free_call->get_args()->get_expressions();
      ROSE_ASSERT( args.size() == 1 );

      address_expression = args.front();

      // free should be paired with malloc
      from_malloc_expression = buildIntVal( 1 );
   }
   else
   {
      address_expression = del_exp -> get_variable();

      // delete should be paired with new
      from_malloc_expression = buildIntVal( 0 );
   }

   ROSE_ASSERT( stmt && address_expression && from_malloc_expression );

   SgExprListExp* arg_list = buildExprListExp();

   appendExpression( arg_list, address_expression );
   appendExpression( arg_list, from_malloc_expression );
   appendFileInfo( arg_list, stmt );

   // have to check validity of call to free before the call itself
   checkBeforeStmt(stmt, symbols.roseFreeMemory, arg_list);
}

void RtedTransformation::insertReallocateCall(SgFunctionCallExp* realloc_call)
{
   // stmt wraps a call to realloc -- most likely an expression statement
   // alert the RTS before the call
   SgStatement*         stmt = getSurroundingStatement(realloc_call);
   SgExpressionPtrList& args = realloc_call->get_args()->get_expressions();
   ROSE_ASSERT( args.size() == 2 );

   SgExpression* address_expression = args.front();
   SgExpression* size_expression = args.back();

   ROSE_ASSERT( stmt && address_expression && size_expression );

   // roseReallocate( ptr, source info );
   SgExprListExp* arg_list = buildExprListExp();

   appendExpression( arg_list, address_expression );
   appendExpression( arg_list, size_expression );
   appendFileInfo( arg_list, stmt );

   // have to check validity of call to realloc before the call itself
   checkBeforeStmt(stmt, symbols.roseReallocateMemory, arg_list);
}

/***************************************************************
 * Get the variable on the left hand side of an assignment
 * starting on the right hand side below the assignment in the tree
 **************************************************************/
SgExpression*
RtedTransformation::getVariableLeftOfAssignmentFromChildOnRight(SgNode* n) {
   SgExpression* expr = NULL;
   SgNode* tempNode = n;
   while (!isSgAssignOp(tempNode) && !isSgProject(tempNode)) {
      tempNode = tempNode->get_parent();
      ROSE_ASSERT(tempNode);
   }
   if (isSgAssignOp(tempNode)) {
      expr = isSgAssignOp(tempNode)->get_lhs_operand();
   }
   return expr;
}

void RtedTransformation::addFileIOFunctionCall(SgVarRefExp* n, bool read) {
   // treat the IO variable as a function call
   SgInitializedName *name = n -> get_symbol() -> get_declaration();
   string fname = name->get_type()->unparseToString();
   string readstr = "r";
   if (!read)
      readstr = "w";
   SgExpression* ex = buildStringVal(readstr);
   SgStatement* stmt = getSurroundingStatement(n);
   std::vector<SgExpression*> args;
   args.push_back(ex);
   SgExpression* pexpr = n;
   RtedArguments* funcCall = new RtedArguments(stmt, fname, readstr, "", "", pexpr, NULL, NULL, NULL, args);
   ROSE_ASSERT(funcCall);
   cerr << " Is a interesting function : " << fname << endl;
   function_call.push_back(funcCall);
}

/***************************************************************
 * Check if the current node is a "interesting" function call
 **************************************************************/
void RtedTransformation::visit_isFunctionCall(SgNode* n)
{
  SgFunctionCallExp*      fcexp = isSgFunctionCallExp(n);
  // handle arguments for any function call
  //handle_function_call_arguments.push_back(fcexp->get_args());

  if (fcexp == NULL) return;

  SgExprListExp*          exprlist = isSgExprListExp(fcexp->get_args());
  SgExpression*           callee = fcexp->get_function();
  SgFunctionRefExp*       refExp = isSgFunctionRefExp(callee);
  SgMemberFunctionRefExp* mrefExp = isSgMemberFunctionRefExp(callee);
  SgDotExp*               dotExp = isSgDotExp(callee);
  SgArrowExp*             arrowExp = isSgArrowExp(callee);
  SgBinaryOp*             binop = isSgBinaryOp(callee);
  SgDotStarOp*            dotStar = isSgDotStarOp(callee);
  string                  name;
  string                  mangled_name;

  if (refExp) {
     SgFunctionDeclaration* decl = NULL;
     decl = isSgFunctionDeclaration(refExp->getAssociatedFunctionDeclaration());
     name = decl->get_name();
     mangled_name = decl->get_mangled_name().str();
     //		cerr << " ### Found FuncRefExp: " <<refExp->get_symbol()->get_declaration()->get_name().str() << endl;
  } else if (dotExp || arrowExp) {
     SgMemberFunctionDeclaration* mdecl = NULL;
     mrefExp = isSgMemberFunctionRefExp(binop->get_rhs_operand());
     ROSE_ASSERT(mrefExp);
     mdecl = isSgMemberFunctionDeclaration(mrefExp->getAssociatedMemberFunctionDeclaration());
     name = mdecl->get_name();
     mangled_name = mdecl->get_mangled_name().str();
  #if 0
     cerr << "### DotExp :   left : " <<binop->get_lhs_operand()->class_name() <<
     ":   right : " <<binop->get_rhs_operand()->class_name() << endl;
     cerr << "### DotExp :   left : " <<binop->get_lhs_operand()->unparseToString() <<
     ":   right : " <<binop->get_rhs_operand()->unparseToString() << endl;
  #endif
  } else if (mrefExp) {
     SgMemberFunctionDeclaration* mdecl = NULL;
     ROSE_ASSERT(mrefExp);
     mdecl = isSgMemberFunctionDeclaration(mrefExp->getAssociatedMemberFunctionDeclaration());
     name = mdecl->get_name();
     mangled_name = mdecl->get_mangled_name().str();
  } else if (dotStar) {
     // e.g. : (testclassA.*testclassAPtr)()
     SgExpression* right = dotStar->get_rhs_operand();
     // we want to make sure that the right hand side is not NULL
     //	abort();
     // tps (09/24/2009) : this part is new and needs to be tested
     // testcode breaks at different location right now.
     SgVarRefExp * varrefRight = isSgVarRefExp(right);
     if (varrefRight)
        variable_access_varref.push_back(varrefRight);
     return;
  } else {
     cerr << "This case is not yet handled : " << fcexp->get_function()->class_name() << endl;
     exit(1);
  }

  // find out if this is a IO-CALL like : myfile << "something" ;
  // with fstream myfile;

  // \pp why not ROSE_ASSERT(exprlist) ???
  if (exprlist)
  {
     SgExpressionPtrList&                args = exprlist->get_expressions();
     SgExpressionPtrList::const_iterator it = args.begin();

     for (; it!=args.end(); ++it)
     {
        SgExpression* fre = isSgExpression(*it);
        const bool    isFileIO = isFileIOVariable(fre->get_type());
        SgVarRefExp*  varRef = isSgVarRefExp(fre);

        cerr << "$$ THe first FuncCallExp has expressions: "
             << fre->unparseToString() << "  type: " << fre->get_type()->class_name()
             << "   isFileIO : " << isFileIO << "   class : " << fre->class_name()
             << endl;

        if (isFileIO && varRef)
        {
          // indicate whether this is a stream operator
          addFileIOFunctionCall(varRef, name == "operator>>");
        }
     }
  }

  ROSE_ASSERT(refExp || mrefExp);
  cerr << "\n@@@@ Found a function call: " << name;
  cerr << "   : fcexp->get_function() : " << fcexp->get_function()->class_name()
       << "   parent : " << fcexp->get_parent()->class_name() << "  : " << fcexp->get_parent()->unparseToString()
       << "\n   : type: : " << fcexp->get_function()->get_type()->class_name()
       << "   : parent type: : " << isSgExpression(fcexp->get_function()->get_parent())->get_type()->class_name()
       << "   unparse: " << fcexp->unparseToString()
       << endl;

  if (isStringModifyingFunctionCall(name) || isFileIOFunctionCall(name))
  {
     // if this is a function call that has a variable on the left hand size,
     // then we want to push that variable first,
     // this is used, e.g. with  File* fp = fopen("file","r");
     // Therefore we need to go up and see if there is an AssignmentOperator
     // and get the var on the left side
     SgExpression*  varOnLeft = getVariableLeftOfAssignmentFromChildOnRight(n);
     SgExpression*  varOnLeftStr=NULL;
     if (varOnLeft) {
        // need to get the mangled_name of the varRefExp on left hand side
        varOnLeftStr = buildStringVal(getMangledNameOfExpression(varOnLeft));
     } else {
        varOnLeftStr = buildStringVal("NoAssignmentVar");
     }

     SgExpression*  pexpr = mrefExp ? static_cast<SgExpression*>(mrefExp) : refExp;
     SgStatement*   stmt = getSurroundingStatement(pexpr);

     ROSE_ASSERT(varOnLeftStr && stmt && pexpr);
     RtedArguments* funcCall = new RtedArguments( stmt,
                                                  name, //func_name
                                                  mangled_name,
                                                  "",
                                                  "",
                                                  pexpr, //refExp,
                                                  varOnLeftStr,
                                                  varOnLeft,
                                                  exprlist,
                                                  exprlist->get_expressions()
                                                );
     ROSE_ASSERT(funcCall);
     cerr << " Is a interesting function : " << name << endl;
     function_call.push_back(funcCall);
  }
  else if (!isFunctionCallOnIgnoreList( name))
  {
     SgStatement* fncallStmt = getSurroundingStatement( fcexp );
     ROSE_ASSERT( fncallStmt );

     // if we're able to, use the function definition's body as the end of
     // scope (for line number complaints).  If not, the callsite is good too.
     SgNode* end_of_scope = getDefiningDeclaration( fcexp );
     if( end_of_scope )
     {
        end_of_scope = isSgFunctionDeclaration( end_of_scope ) -> get_definition() -> get_body();
     }
     else
     {
        end_of_scope = fcexp;
        // FIXME 2: We may be adding a lot of unnecessary signature checks
        // If we don't have the definition, we must be doing separate
        // compilation.  We will then have to check the signature at runtime
        // since there's no guarantee our prototype, if any, is accurate.
        function_call_missing_def.push_back( fcexp );
     }

     scopes[ fncallStmt ] = end_of_scope;
  }
  else if( "free" == name )
  {
     frees.push_back( fcexp );
  }
  else if( "realloc" == name )
  {
     reallocs.push_back( fcexp );
  }
}

#endif
