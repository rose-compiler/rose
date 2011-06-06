#include <rose.h>

// DQ (2/9/2010): Testing use of ROE to compile ROSE.
#ifndef USE_ROSE

#include <string>
#include <boost/algorithm/string/predicate.hpp>

#include "rosez.hpp"

#include "RtedTransformation.h"

#include "RtedSymbols.h"
#include "DataStructures.h"

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

bool isGlobalFunctionOnIgnoreList(const std::string& name) {
   return (  name == "calloc"
          || name == "free"
          || name == "malloc"
          || name == "printf"
          || name == "realloc"
          || name == "upc_all_alloc"
          || name == "upc_local_alloc"
          || name == "upc_alloc"
          || name == "upc_free"
          || name == "upc_global_alloc"
          || isLibFunctionRequiringArgCheck(name)
          );
}


bool isLibFunctionRequiringArgCheck(const std::string& name)
{
  return (  name == "upc_threadof"
         || name == "upc_addrfield"
         );
}

namespace
{
  struct IsIntType
  {
    const SgType* res;

    IsIntType()
    : res(NULL)
    {}

    template <class SageNode>
    void skip(const SageNode& n) { res = n.get_base_type(); }

    void found(const SgType& n) { res = &n; }

    void handle(const SgNode&)                { ROSE_ASSERT(false); }

    void handle(const SgType&)                { /* not an int */ }

    void handle(const SgTypeChar& n)          { found(n); }
    void handle(const SgTypeSignedChar& n)    { found(n); }
    void handle(const SgTypeUnsignedChar& n)  { found(n); }
    void handle(const SgTypeShort& n)         { found(n); }
    void handle(const SgTypeSignedShort& n)   { found(n); }
    void handle(const SgTypeUnsignedShort& n) { found(n); }
    void handle(const SgTypeInt& n)           { found(n); }
    void handle(const SgTypeSignedInt& n)     { found(n); }
    void handle(const SgTypeUnsignedInt& n)   { found(n); }
    void handle(const SgTypeLong& n)          { found(n); }
    void handle(const SgTypeSignedLong& n)    { found(n); }
    void handle(const SgTypeUnsignedLong& n)  { found(n); }

    void handle(const SgModifierType& n)      { skip(n); }
    void handle(const SgTypedefType& n)       { skip(n); }

    operator const SgType*() { return res; }
  };
};

static
bool isIntType(const SgType* n)
{
  const SgType* curr = NULL;
  const SgType* next = n;

  do
  {
    curr = next;
    next = ez::visitSgNode(IsIntType(), curr);
  } while (next && next != curr);

  return (next != NULL);
}

static
bool isFunctionCallReturningInt(SgExpression* exp)
{
  SgFunctionCallExp* funcall = isSgFunctionCallExp(exp);

  return (funcall && isIntType(funcall->get_type()));
}

SgFunctionCallExp* RtedTransformation::convertIntToString(SgExpression* i)
{
  ROSE_ASSERT(symbols.roseConvertIntToString);

  SgFunctionRefExp*  memRef_r = buildFunctionRefExp(symbols.roseConvertIntToString);
  SgExprListExp*     arg_list = buildExprListExp();

  appendExpression(arg_list, i);
  return buildFunctionCallExp(memRef_r, arg_list);
}

void RtedTransformation::insertFuncCall(RtedArguments& args)
{
  // fixed arguments
  // 1 = name
  // 2 = unparsedStmt for Error message
  // 3 = Left Hand side variable, if assignment
  // 4 = SourceInfo
  // 5 = number of the following arguments (\pp ??)

  //  SgStatement* stmt = getSurroundingStatement(args->varRefExp);
  SgStatement*      stmt = args.stmt;
  ROSE_ASSERT(stmt);

  SgScopeStatement* scope = stmt->get_scope();
  ROSE_ASSERT(scope);

  SgExpression*     callNameExp = buildStringVal(args.f_name);
  SgExprListExp*    arg_list = buildExprListExp();

  appendExpression(arg_list, callNameExp);
  appendExpression(arg_list, buildStringVal(removeSpecialChar(stmt->unparseToString())));

  // this one is new, it indicates the variable on the left hand side of the statment,
  // if available
  ROSE_ASSERT(args.leftHandSideAssignmentExprStr);
  appendExpression(arg_list, args.leftHandSideAssignmentExprStr);
  cerr << " ... Left hand side variable : " << args.leftHandSideAssignmentExprStr << endl;

  appendFileInfo(arg_list, stmt);

  // how many additional arguments does this function need?
  const size_t         dimFuncCall = getDimensionForFuncCall(args.f_name);

  // nr of args + 2 (for sepcialFunctions) + 6 =  args+6;
  SgExpressionPtrList& rose_args = args.arguments;
  const size_t         size = dimFuncCall + rose_args.size();
  SgIntVal*            sizeExp = buildIntVal(size);

  appendExpression(arg_list, sizeExp);

  // \pp modifying the dimension count looks strange (in fact when comparing
  //     isStringModifyingFunctionCall to getDimensionForFuncCall one
  //     notices that the only affected call function is strlen)!
  //     Since I do not understand the code below, the refactored/reformmated
  //     code below preserves the original behaviour.
  //     dimIsTwo is newly introduced and is true when the original code had
  //       dimFuncCall = 2.
  //     Subsequently, dimIsTwo replaces all occurances of (dimFuncCall == 2).
  // \pp why do we need dimIsTwo anyways, the arguments seem to be not used in the backend
  const bool                          dimIsTwo = (dimFuncCall == 2) || isStringModifyingFunctionCall(args.f_name);
  SgExprListExp*                      vararg_list = buildExprListExp();

  // iterate over all arguments of the function call, e.g. strcpy(arg1, arg2);
  SgExpressionPtrList::const_iterator it = rose_args.begin();

  for (; it != rose_args.end(); ++it)
  {
     SgExpression* exp = deepCopy(*it);

     // ************ unary operation *******************************
     if (isSgUnaryOp(exp))
     {
        // \pp why are unary operations skipped?
        // \pp \memleak as exp is no longer referenced
        exp = isSgUnaryOp(exp)->get_operand();
     }

     if (isSgVarRefExp(exp))
     {
        SgVarRefExp* var = isSgVarRefExp(exp);
        SgType*      vartype = var->get_type();

        // \pp \todo what about skipping modifier types?
        SgType*      base_type = skip_ArrPtrType(vartype);

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
              appendExpression(vararg_list, var);
              if (dimIsTwo)
              {
                expr = ( expr == 0 ? buildStringVal("00")
                                   : buildStringVal(expr->unparseToString() + "\0")
                       );

                appendExpression(vararg_list, expr);
              }

              cerr << ">>> Found variable : " << name << "  with size : " << expr->unparseToString() << "  and val : " << var << endl;
           }
           else if (isSgPointerType(inittype))
           {
              cerr << "Cant determine the size of the following object : " << var->class_name() << "  with base type : "
                    << base_type->class_name() << endl;

              // this is a pointer to char* but not an array allocation yet
              // We have to pass var so we can check whether the memories overlap!
              appendExpression(vararg_list, var);

              if (dimIsTwo)
              {
                // if this is a pointer to e.g. malloc we cant statically determine the
                // size of the allocated address. We need to pass the variable
                // and check in the runtime system if the size for the variable is known!
                SgExpression* manglName = buildStringVal(var->get_symbol()->get_declaration()->get_mangled_name().str());

                appendExpression(vararg_list, manglName);
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

              appendExpression(vararg_list, charCast);

              if (dimIsTwo)
              {
                SgExpression* numberToString = buildStringVal("001");

                appendExpression(vararg_list, numberToString);
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
                 //       cerr << "\n!!!!!!!!!!parent of initName = " << initName->get_parent()->class_name() << endl;
                 manglName = buildStringVal(initName->get_name().str());
              }
              ROSE_ASSERT(manglName);
              appendExpression(vararg_list, manglName);
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

           appendExpression(vararg_list, convertIntToString(var));
           cerr << " Created Function call  convertToString" << endl;
        }
     }
     // --------- this is not a varRefExp ----
     else if (isSgStringVal(exp))
     {
        string theString = isSgStringVal(exp)->get_value();

        appendExpression(vararg_list, buildStringVal(theString + "\0"));

        if (dimIsTwo)
        {
           size_t        sizeString = theString.size() + 1; // add the '\0' to it
           string        sizeStringStr = RoseBin_support::ToString(sizeString);
           SgExpression* numberToString = buildStringVal(sizeStringStr);

           appendExpression(vararg_list, numberToString);
        }
     }
     else if (isFunctionCallReturningInt(exp))
     {
       appendExpression(vararg_list, convertIntToString(exp));

       if (dimIsTwo)
       {
         appendExpression(vararg_list, buildStringVal("-1"));
       }
     }
     else
     {
        // default create a string
        string        theString = exp->unparseToString();
        SgExpression* stringExp = buildStringVal(theString + "\0");

        appendExpression(vararg_list, stringExp);

        if (dimIsTwo)
        {
          // \pp why is this needed
          size_t sizeString = theString.size();
          string sizeStringStr = RoseBin_support::ToString(sizeString);
          SgExpression* numberToString = buildStringVal(sizeStringStr);

          appendExpression(vararg_list, numberToString);
        }
     }
  }

  appendExpression( arg_list, ctorStringList(genAggregateInitializer(vararg_list, roseConstCharPtrType())) );

  ROSE_ASSERT(symbols.roseFunctionCall);
  insertCheck( ilBefore,
               stmt,
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

  SgStatement*         stmt = getSurroundingStatement(fce);
  ROSE_ASSERT(stmt);

  rt.appendFileInfo( arg_list, stmt->get_scope(), fce->get_file_info() );

  insertCheckOnStmtLevel(ilBefore, fce, rt.symbols.roseAssertFunctionSignature, arg_list);
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

void RtedTransformation::insertFreeCall(SgExpression* freeExp, AllocKind ak)
{
   ROSE_ASSERT( freeExp );

   // stmt wraps a call to free -- most likely an expression statement
   // alert the RTS before the call to detect errors such as double-free
   SgStatement*  stmt = getSurroundingStatement(freeExp);
   SgExpression* address_expression = NULL;

   requiresParentIsBasicBlock(*stmt);

   if ((ak & akCxxHeap) == akCxxHeap)
   {
     SgDeleteExp* delExp = isSgDeleteExp(freeExp);
     ROSE_ASSERT(delExp);

     address_expression = delExp -> get_variable();
   }
   else
   {
     SgFunctionCallExp*   callExp = isSgFunctionCallExp(freeExp);
     ROSE_ASSERT(callExp);

     SgExpressionPtrList& args = callExp->get_args()->get_expressions();
     ROSE_ASSERT( args.size() == 1 );

     address_expression = args.front();
   }

   ROSE_ASSERT( stmt && address_expression );

   const bool     upcShared = ((ak & akUpcSharedHeap) == akUpcSharedHeap);
   SgExprListExp* arg_list = buildExprListExp();

   appendExpression( arg_list, mkAddress(address_expression, upcShared) );
   appendAllocKind( arg_list, ak );
   appendFileInfo( arg_list, stmt );

   // have to check validity of call to free before the call itself
   insertCheck(ilBefore, stmt, symbols.roseFreeMemory, arg_list);

   // upc_free requires the thread to be the only operator on the heap
   if (upcShared)
   {
     ROSE_ASSERT(symbols.roseUpcBeginExclusive && symbols.roseUpcEndExclusive);

     insertCheck(ilBefore, stmt, symbols.roseUpcBeginExclusive, buildExprListExp());
     insertCheck(ilAfter,  stmt, symbols.roseUpcEndExclusive,   buildExprListExp());
   }
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
   insertCheck(ilBefore, stmt, symbols.roseReallocateMemory, arg_list);
}

/***************************************************************
 * Get the variable on the left hand side of an assignment
 * starting on the right hand side below the assignment in the tree
 **************************************************************/
static
SgExpression*
getExpressionLeftOfAssignmentFromChildOnRight(SgFunctionCallExp* n) {
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

static
std::string
getMangledNameOfExpression(SgExpression* expr)
{
  string manglName;
  // look for varRef in the expr and return its mangled name
  const SgNodePtrList& var_refs = NodeQuery::querySubTree(expr,V_SgVarRefExp);

  if (var_refs.size()==0)
  {
    // there should be at least one var ref
    cerr << " getMangledNameOfExpression: varRef on left hand side not found : " << expr->unparseToString() << endl;
  }
  else if (var_refs.size()==1)
  {
    // correct, found on var ref
    SgVarRefExp* varRef = isSgVarRefExp(*(var_refs.begin()));
    ROSE_ASSERT(varRef && varRef->get_symbol()->get_declaration());
    manglName = varRef->get_symbol()->get_declaration()->get_mangled_name();
    cerr << " getMangledNameOfExpression: found varRef: " << manglName << endl;
  }
  else if (var_refs.size()>1)
  {
    // error
    cerr << " getMangledNameOfExpression: Too many varRefs on left hand side : " << var_refs.size() << endl;
  }

  return manglName;
}


void RtedTransformation::addFileIOFunctionCall(SgVarRefExp* n, bool read)
{
   // treat the IO variable as a function call
   SgInitializedName*         name = n -> get_symbol() -> get_declaration();
   std::string                fname = name->get_type()->unparseToString();
   const char*                readstr = (read ? "r" : "w");

   SgExpression*              ex = buildStringVal(readstr);
   SgStatement*               stmt = getSurroundingStatement(n);
   std::vector<SgExpression*> args;

   args.push_back(ex);

   std::cerr << "@@@ my interesting func: " << fname << std::endl;
   function_call.push_back(RtedArguments(stmt, fname, readstr, "", "", n, NULL, NULL, NULL, args));
}

static
bool upcHeapManagementNeeded(const std::string& fnname)
{
  // in order to avoid deadlocks we need to exit UPC reader locks for
  //   the heap when a thread can get blocked.
  //   The general deadlock scenario is:
  //   Thread 1: holds the heap lock (e.g., as reader) and waits at some
  //             synchronized function (e.g., collective call, lock acquisition).
  //   Thread 2: calls upc_free just before calling the collective
  //             function.
  static const std::string upc_prefix = "upc_";
  static const std::string upc_collective_prefix = "upc_all_";

  if (!boost::starts_with(fnname, upc_prefix)) return false;

  return (  fnname == "upc_lock"
         || boost::starts_with(fnname, upc_collective_prefix)
         );
}

struct FunctionCallInfo
{
  RtedTransformation&  trans;
  bool                 memberCall;
  std::string          name;
  std::string          mangled_name;
  SgExpression*        target;

  explicit
  FunctionCallInfo(RtedTransformation& master)
  : trans(master), memberCall(false), name(), mangled_name(), target(NULL)
  {}

  void memberFunctionCall(SgMemberFunctionRefExp& mrefExp);
  void memberFunctionCall(SgBinaryOp& n);
  void ptrToMemberFunctionCall(SgBinaryOp& n);

  void handle(SgNode&) { ROSE_ASSERT(false); }

  void handle(SgExpression& n)
  {
    // this is a computed function (e.g., through a function pointer)
  }

  void handle(SgFunctionRefExp& n)
  {
    SgFunctionDeclaration* decl = n.getAssociatedFunctionDeclaration();

    name = decl->get_name();
    mangled_name = decl->get_mangled_name().str();
    target = &n;
  }

  void handle(SgArrowExp& n) { memberFunctionCall(n); }
  void handle(SgDotExp& n) { memberFunctionCall(n); }

  void handle(SgMemberFunctionRefExp& n)
  {
    // \pp member function calls can occur in context of dot and arrow
    //     expressions, but they can also be "naked" within a member body
    memberFunctionCall(n);
  }

  void handle(SgDotStarOp& n) { ptrToMemberFunctionCall(n); }
  void handle(SgArrowStarOp& n) { ptrToMemberFunctionCall(n); }
};

void FunctionCallInfo::memberFunctionCall(SgMemberFunctionRefExp& mrefExp)
{
   SgMemberFunctionDeclaration* mdecl = mrefExp.getAssociatedMemberFunctionDeclaration();

   memberCall = true;
   name = mdecl->get_name();
   mangled_name = mdecl->get_mangled_name().str();
   target = &mrefExp;
}

void FunctionCallInfo::memberFunctionCall(SgBinaryOp& n)
{
   SgMemberFunctionRefExp*      mrefExp = isSgMemberFunctionRefExp(n.get_rhs_operand());
   ROSE_ASSERT(mrefExp);

   memberFunctionCall(*mrefExp);
}

void FunctionCallInfo::ptrToMemberFunctionCall(SgBinaryOp& n)
{
   // \pp \todo this needs to be handled appropriately
   // e.g. : (testclassA.*testclassAPtr)()
   SgExpression* right = n.get_rhs_operand();
   // we want to make sure that the right hand side is not NULL
   // abort();
   // tps (09/24/2009) : this part is new and needs to be tested
   // testcode breaks at different location right now.
   SgVarRefExp* varrefRight = isSgVarRefExp(right);
   if (varrefRight)
   {
      trans.variable_access_varref.push_back(varrefRight);
   }
}


static
void store_if_needed( RtedTransformation::UpcBlockingOpsContainer& cont, SgFunctionCallExp& call )
{
  SgStatement* stmt = getSurroundingStatement( &call );
  SgStatement* last = cont.empty() ? NULL : cont.back();

  // if the stmt is already going to be protected, then we can skip it
  if (last != stmt) cont.push_back( stmt );
}


/***************************************************************
 * Check if the current node is a "interesting" function call
 **************************************************************/
void RtedTransformation::visit_isFunctionCall(SgFunctionCallExp* const fcexp)
{
  ROSE_ASSERT(fcexp);

  SgExpression* const                 fun = fcexp->get_function();
  const FunctionCallInfo              callee = ez::visitSgNode(FunctionCallInfo(*this), fun);

  // \pp \todo \note The current implementation cannot handle function targets
  //                 determined at runtime (e.g., function pointer).
  if (!callee.target) return;

  // \pp \todo the rest of this function's logic could/should be pushed
  //           into FunctionCallInfo.

  // find out if this is a IO-CALL like : myfile << "something" ;
  // with fstream myfile;
  SgExprListExp* const                exprlist = isSgExprListExp(fcexp->get_args());
  ROSE_ASSERT(exprlist);

  SgExpressionPtrList&                args = exprlist->get_expressions();
  SgExpressionPtrList::const_iterator it = args.begin();

  for (; it!=args.end(); ++it)
  {
    SgExpression* fre = *it;
    SgVarRefExp*  varRef = isSgVarRefExp(fre);

    if (varRef && isFileIOVariable(fre->get_type()))
    {
      // indicate whether this is a stream operator
      addFileIOFunctionCall(varRef, callee.name == "operator>>");
    }
    else if (varRef)
    {
      std::cerr << "@@@ " << varRef->unparseToString() << "not IOVar" << std::endl;
    }
  }

  cerr << "\n@@@@ Found a function call: " << callee.name;
  cerr << "   : fcexp->get_function() : " << fun->class_name()
       << "   parent : " << fcexp->get_parent()->class_name() << "  : " << fcexp->get_parent()->unparseToString()
       << "\n   : type: : " << fun->get_type()->class_name()
       << "   : parent type: : " << isSgExpression(fun->get_parent())->get_type()->class_name()
       << "   unparse: " << fcexp->unparseToString()
       << endl;

  // \pp \todo this should ask whether the function was defined in global scope
  //           or within namespace std (instead of asking for membership).
  //           This is to avoid false positives on functions with the same name
  //           but declared within a namespace.
  bool handled = !callee.memberCall;

  if (handled)
  {
    if (isStringModifyingFunctionCall(callee.name) || isFileIOFunctionCall(callee.name))
    {
       // if this is a function call that has a variable on the left hand size,
       // then we want to push that variable first,
       // this is used, e.g. with  File* fp = fopen("file","r");
       // Therefore we need to go up and see if there is an AssignmentOperator
       // and get the var on the left side
       SgStatement*   stmt = getSurroundingStatement( fcexp );
       SgExpression*  varOnLeft = getExpressionLeftOfAssignmentFromChildOnRight(fcexp);
       SgExpression*  varOnLeftStr=NULL;
       if (varOnLeft) {
          // need to get the mangled_name of the varRefExp on left hand side
          varOnLeftStr = buildStringVal(getMangledNameOfExpression(varOnLeft));
       } else {
          varOnLeftStr = buildStringVal("NoAssignmentVar");
       }

       ROSE_ASSERT(varOnLeftStr && stmt );
       function_call.push_back( RtedArguments( stmt,
                                               callee.name,
                                               callee.mangled_name,
                                               "",
                                               "",
                                               callee.target,
                                               varOnLeftStr,
                                               varOnLeft,
                                               exprlist,
                                               exprlist->get_expressions()
                                             )
                              );
    }
    else if ("free" == callee.name)
    {
       frees.push_back( Deallocations::value_type(fcexp, akCHeap) );
    }
    else if( "realloc" == callee.name )
    {
       reallocs.push_back( fcexp );
    }
    else if( "upc_free" == callee.name )
    {
       frees.push_back( Deallocations::value_type(fcexp, akUpcSharedHeap) );
    }
    else if (upcHeapManagementNeeded(callee.name))
    {
      store_if_needed( upcBlockingOps, *fcexp );
    }
    else if (!isGlobalFunctionOnIgnoreList(callee.name))
    {
      handled = false;
    }
  }

  if (!handled)
  {
     SgStatement* fncallStmt = getSurroundingStatement( fcexp );
     ROSE_ASSERT( fncallStmt );

     // if we're able to, use the function definition's body as the end of
     // scope (for line number complaints).  If not, the callsite is good too.
     SgNode*                end_of_scope = fcexp;
     SgFunctionDeclaration* fndecl = getDefiningDeclaration( fcexp );
     if ( fndecl )
     {
        SgFunctionDefinition* fndef = fndecl->get_definition();

        // \pp the original RTED code does not test whether get_definition
        //     is successful (and it works). Not sure why this is required
        //     now (to test operator<<(std::ostream&, 'std::endl')?
        if (fndef) end_of_scope = fndef->get_body();
     }
     else
     {
        // FIXME 2: We may be adding a lot of unnecessary signature checks
        // If we don't have the definition, we must be doing separate
        // compilation.  We will then have to check the signature at runtime
        // since there's no guarantee our prototype, if any, is accurate.
        function_call_missing_def.push_back( fcexp );
     }

     scopes[ fncallStmt ] = end_of_scope;
  }
}

#endif
