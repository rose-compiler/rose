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
          || name == "strlen"
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
int RtedTransformation::getDimensionForFuncCall(std::string name) {
   int dim = 0;
   if (name == "memcpy" || name == "memmove" || name == "strcpy" || name == "strncpy" || name == "strcat" || name == "strncat"
         || name == "strchr" || name == "strpbrk" || name == "strspn" || name == "strstr" || name == "fopen") {
      dim = 2;
   } else if (name == "strlen") {
      dim = 1;
   }
   return dim;
}

/*********************************************************
 * Check if a function call is a call to a function
 * on our ignore list. We do not want to check those
 * functions right now.
 * This check makes sure that we dont push variables
 * on the stack for functions that we dont check
 * and hence the generated code is cleaner
 ********************************************************/
bool RtedTransformation::isFileIOFunctionCall(std::string name) {
   bool interesting = false;
   if (name == "fopen" || name == "fgetc" || name == "fputc" || name == "fclose" || name == "std::fstream"

   )
      interesting = true;
   return interesting;
}

/*********************************************************
 * Check if a function call is a call to a function
 * on our ignore list. We do not want to check those
 * functions right now.
 * This check makes sure that we dont push variables
 * on the stack for functions that we dont check
 * and hence the generated code is cleaner
 ********************************************************/
bool RtedTransformation::isFunctionCallOnIgnoreList(std::string name) {
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
  //  SgStatement* stmt = getSurroundingStatement(args->varRefExp);
  SgStatement* stmt = args->stmt;

  ROSE_ASSERT(stmt);
  SgScopeStatement* scope = stmt->get_scope();

  ROSE_ASSERT(scope);
  // fixed arguments
  // 1 = name
  // 2 = unparsedStmt for Error message
  // 3 = Left Hand side variable, if assignment
  // 4 = filename
  // 5 = lineNr
  // 6 = lineNrTransformed

  int extra_params = 6;
  int size = extra_params + args->arguments.size(); // nr of args + 2 (for sepcialFunctions) + 6 =  args+6;
  // how many additional arguments does this function need?
  int dimFuncCall = getDimensionForFuncCall(args->f_name);
  size += dimFuncCall;
  SgIntVal* sizeExp = buildIntVal(size);
  SgExpression* callNameExp = buildStringVal(args->f_name);

  //cerr << " >>>>>>>> Symbol VarRef: " << symbolName << endl;
  ROSE_ASSERT(symbols.roseFunctionCall);

  SgExprListExp* arg_list = buildExprListExp();

  appendExpression(arg_list, sizeExp);
  appendExpression(arg_list, callNameExp);

  appendExpression(arg_list, buildStringVal(removeSpecialChar(stmt->unparseToString())));

  // this one is new, it indicates the variable on the left hand side of the statment,
  // if available
  ROSE_ASSERT(args->leftHandSideAssignmentExprStr);
  appendExpression(arg_list, args->leftHandSideAssignmentExprStr);
  cerr << " ... Left hand side variable : " << args->leftHandSideAssignmentExprStr << endl;

  appendFileInfo(arg_list, stmt);

  // if we need 2 additional parameters, we need to add them when necessary
  if (isStringModifyingFunctionCall(args->f_name))
     dimFuncCall = 2;
  // iterate over all arguments of the function call, e.g. strcpy(arg1, arg2);
  std::vector<SgExpression*>::const_iterator it = args->arguments.begin();
  for (; it != args->arguments.end(); ++it) {
     SgExpression* exp = deepCopy(*it);

     // ************ unary operation *******************************
     if (isSgUnaryOp(exp))
        exp = isSgUnaryOp(exp)->get_operand();
     //cerr << " exp = " << exp->class_name() << endl;
     // argument is a variable (varRef)
     if (isSgVarRefExp(exp)) {
        SgVarRefExp* var = isSgVarRefExp(exp);
        SgType* type = var->get_type();
        SgType* base_type = NULL;
        if (isSgArrayType(type))
           base_type = isSgArrayType(type)->get_base_type();
        if (isSgPointerType(type))
           base_type = isSgPointerType(type)->get_base_type();

        cerr << " isSgVarRefExp :: type : " << type->class_name() << endl;
        if (base_type)
           cerr << "     base_type: " << base_type->class_name() << endl;

        // --------- varRefExp is TypeChar -----------------------
        if (isSgTypeChar(type) || isSgTypeChar(base_type)) {
           string name = var->get_symbol()->get_declaration()->get_name().str();
           //appendExpression(arg_list, manglName);
           // in addition we want the allocated size for this variable
           SgInitializedName* initName = var->get_symbol()->get_declaration();
           ROSE_ASSERT(initName);
           SgType* type = initName->get_typeptr();
           ROSE_ASSERT(type);
           SgArrayType* array = isSgArrayType(type);
           if (array) {
              SgExpression * expr = array->get_index();
              cerr << " Found 00 - btype : " << array->get_base_type()->class_name() << "  index type: " << expr << endl;
              if (expr == NULL) {
                 expr = buildStringVal("00");
              } else
                 expr = buildStringVal(expr->unparseToString() + "\0");
              // this is the variable that we pass
              appendExpression(arg_list, var);
              if (dimFuncCall == 2)
                 appendExpression(arg_list, expr);
              cerr << ">>> Found variable : " << name << "  with size : " << expr->unparseToString() << "  and val : " << var
                    << endl;
           } else {
              cerr << "Cant determine the size of the following object : " << var->class_name() << "  with type : "
                    << type->class_name() << endl;
              if (base_type)
                 cerr << "   and base type : " << base_type->class_name() << endl;
              if (isSgPointerType(type)) {
                 // this is a pointer to char* but not an array allocation yet
                 // We have to pass var so we can check whether the memories overlap!
                 //SgExpression* emptyString = buildStringVal("");
                 appendExpression(arg_list, var);
                 // if this is a pointer to e.g. malloc we cant statically determine the
                 // size of the allocated address. We need to pass the variable
                 // and check in the runtime system if the size for the variable is known!
                 //	      SgExpression* numberToString = buildStringVal("000");
                 SgExpression* manglName = buildStringVal(var->get_symbol()->get_declaration()->get_mangled_name().str());
                 if (dimFuncCall == 2)
                    appendExpression(arg_list, manglName);
              } else {
                 // this is most likely a single char, the size of it is 1
                 SgExpression* andSign = buildAddressOfOp(var);
                 SgExpression* charCast = buildCastExp(andSign, buildPointerType(buildCharType()));
                 appendExpression(arg_list, charCast);
                 SgExpression* numberToString = buildStringVal("001");
                 if (dimFuncCall == 2)
                    appendExpression(arg_list, numberToString);

              }
              //ROSE_ASSERT(false);
           }
        }

        // --------- varRefExp is PointerType -----------------------
        else if (isSgPointerType(type)) {
           // handle pointers as parameters
           cerr << "RtedTransformation - unknown type : " << type->class_name() << endl;
           if (base_type)
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
        else {
           // handle integers as parameters
           SgFunctionRefExp* memRef_r2 = NULL;
           if (isSgTypeInt(type)) {
              ROSE_ASSERT(symbols.roseConvertIntToString);
              memRef_r2 = buildFunctionRefExp(symbols.roseConvertIntToString);
           } else {
              cerr << "RtedTransformation - unknown type : " << type->class_name() << endl;
              ROSE_ASSERT(false);
           }
           ROSE_ASSERT(memRef_r2);
           string symbolName3 = symbols.roseConvertIntToString->get_name().str();
           cerr << " >>>>>>>> Symbol Member::: " << symbolName3 << endl;

           SgExprListExp* arg_list2 = buildExprListExp();
           appendExpression(arg_list2, var);
           SgFunctionCallExp* funcCallExp2 = buildFunctionCallExp(memRef_r2, arg_list2);
           ROSE_ASSERT(funcCallExp2);
           if (dimFuncCall == 2)
              appendExpression(arg_list, funcCallExp2);
           cerr << " Created Function call  convertToString" << endl;
        }
     }

     // --------- this is not a varRefExp ----
     else {
        // if it is already a string, dont add extra quates
        cerr << " isNotSgVarRefExp exp : " << exp->class_name() << "   " << exp->unparseToString() << endl;
        if (isSgStringVal(exp)) {
           string theString = isSgStringVal(exp)->get_value();
           appendExpression(arg_list, buildStringVal(theString + "\0"));
           int sizeString = theString.size() + 1; // add the '\0' to it
           string sizeStringStr = RoseBin_support::ToString(sizeString);
           ROSE_ASSERT(sizeStringStr!="");
           SgExpression* numberToString = buildStringVal(sizeStringStr);
           if (dimFuncCall == 2)
              appendExpression(arg_list, numberToString);
        } else {
           // default create a string
           string theString = exp->unparseToString();
           SgExpression* stringExp = buildStringVal(theString + "\0");
           appendExpression(arg_list, stringExp);
           int sizeString = theString.size();
           string sizeStringStr = RoseBin_support::ToString(sizeString);
           ROSE_ASSERT(sizeStringStr!="");
           SgExpression* numberToString = buildStringVal(sizeStringStr);
           if (dimFuncCall == 2)
              appendExpression(arg_list, numberToString);
        }
     }
  }

  checkBeforeStmt( stmt,
                   symbols.roseFunctionCall,
                   arg_list,
                   "RS : Calling Function, parameters: (#args, function name, filename, linenr, linenrTransformed, error message, left hand var, other parameters)"
                 );
}

template <class RoseFunctionRefExp>
static
void insertAssertFunctionSignature(RtedTransformation& rt, SgFunctionCallExp* fce, RoseFunctionRefExp* funref)
{
  // we aren't able to check the function signature at compile time, so we
  // insert a check to do so at runtime

  using SageInterface::appendExpression;
  using SageBuilder::buildExprListExp;
  using SageBuilder::buildStringVal;
  using SageBuilder::buildIntVal;

  ROSE_ASSERT(fce && funref);

  SgExprListExp*                      arg_list = buildExprListExp();
  Rose_STL_Container< SgExpression* > args = fce->get_args()->get_expressions();

  appendExpression( arg_list, buildStringVal( funref->get_symbol()->get_name()) );
  appendExpression( arg_list, buildIntVal( args.size() + 1 ));
  rt.appendTypeInformation( arg_list,
                            isSgFunctionType( funref->get_type() ) -> get_return_type(),
                            true,
                            true
                          );

   BOOST_FOREACH( SgExpression* arg, args ) {
      rt.appendTypeInformation( arg_list, arg -> get_type(), true, true );
   }

   // checkpoint information
   rt.appendFileInfo(arg_list, fce);

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

void RtedTransformation::insertFreeCall(SgExpression* free) {

   SgFunctionCallExp* free_call = isSgFunctionCallExp(free);
   SgDeleteExp* del_exp = isSgDeleteExp(free);

   ROSE_ASSERT( free_call || del_exp );

   // stmt wraps a call to free -- most likely an expression statement
   // alert the RTS before the call to detect errors such as double-free
   SgStatement* stmt = getSurroundingStatement(free);
   SgExpression* address_expression;
   SgExpression* from_malloc_expression;

   if (free_call) {
      Rose_STL_Container< SgExpression* > args
      = free_call->get_args()->get_expressions();
      ROSE_ASSERT( args.size() > 0 );

      address_expression = args[ 0 ];

      // free should be paired with malloc
      from_malloc_expression = buildIntVal( 1 );
   } else if( del_exp ) {

      address_expression = del_exp -> get_variable();

      // delete should be paired with new
      from_malloc_expression = buildIntVal( 0 );
   } else {ROSE_ASSERT( false );}

   ROSE_ASSERT( stmt );
   ROSE_ASSERT( address_expression );
   ROSE_ASSERT( from_malloc_expression );

   // roseFree( ptr, source info );
   SgExprListExp* arg_list = buildExprListExp();
   appendExpression( arg_list, address_expression );
   appendExpression( arg_list, from_malloc_expression );
   appendFileInfo( arg_list, stmt );

   // have to check validity of call to free before the call itself
   checkBeforeStmt(stmt, symbols.roseFreeMemory, arg_list);
}

void RtedTransformation::insertReallocateCall(SgFunctionCallExp* realloc_call) {

   // stmt wraps a call to realloc -- most likely an expression statement
   // alert the RTS before the call
   SgStatement* stmt = getSurroundingStatement(realloc_call);
   Rose_STL_Container< SgExpression* > args = realloc_call->get_args()->get_expressions();
   ROSE_ASSERT( args.size() > 1 );

   SgExpression* address_expression = args[ 0 ];
   SgExpression* size_expression = args[ 1 ];

   ROSE_ASSERT( stmt );
   ROSE_ASSERT( address_expression );
   ROSE_ASSERT( size_expression );

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
   RtedArguments* funcCall = new RtedArguments(fname, readstr, "", "", pexpr, stmt, args, NULL, NULL, NULL);
   ROSE_ASSERT(funcCall);
   cerr << " Is a interesting function : " << fname << endl;
   function_call.push_back(funcCall);
}

/***************************************************************
 * Check if the current node is a "interesting" function call
 **************************************************************/
void RtedTransformation::visit_isFunctionCall(SgNode* n) {
   SgFunctionCallExp* fcexp = isSgFunctionCallExp(n);
   // handle arguments for any function call
   //handle_function_call_arguments.push_back(fcexp->get_args());

   if (fcexp) {
      SgExprListExp* exprlist = isSgExprListExp(fcexp->get_args());
      SgFunctionRefExp* refExp = isSgFunctionRefExp(fcexp->get_function());
      SgMemberFunctionRefExp* mrefExp = isSgMemberFunctionRefExp(fcexp->get_function());
      SgDotExp* dotExp = isSgDotExp(fcexp->get_function());
      SgArrowExp* arrowExp = isSgArrowExp(fcexp -> get_function());
      SgBinaryOp* binop = isSgBinaryOp(fcexp -> get_function());
      //	    cerr << "       refExp : " << refExp << "  mrefExp : " << mrefExp
      //	 << "  dotExp : " << dotExp << "   arrowExp: " << arrowExp << endl;

      SgDotStarOp* dotStar = isSgDotStarOp(fcexp->get_function());

      string name = "";
      string mangled_name = "";
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
      if (exprlist) {
         Rose_STL_Container<SgExpression*> expr = exprlist->get_expressions();
         Rose_STL_Container<SgExpression*>::const_iterator it = expr.begin();
         for (;it!=expr.end();++it) {
            SgExpression* fre = isSgExpression(*it);
            bool isFileIO = isFileIOVariable(fre->get_type());
            SgVarRefExp* varRef = isSgVarRefExp(fre);
            cerr << "$$ THe first FuncCallExp has expressions: " <<
            fre->unparseToString() << "  type: " << fre->get_type()->class_name() <<
            "   isFileIO : " << isFileIO << "   class : " << fre->class_name()<<endl;
            if (isFileIO && varRef) {
               if (name.compare("operator>>")==0)
               addFileIOFunctionCall(varRef,true); //read
               else
               addFileIOFunctionCall(varRef,false); //write
            }
         }
      }

      ROSE_ASSERT(refExp || mrefExp);
      cerr <<"\n@@@@ Found a function call: " << name;
      cerr << "   : fcexp->get_function() : " << fcexp->get_function()->class_name() <<
      "   parent : " << fcexp->get_parent()->class_name() << "  : " << fcexp->get_parent()->unparseToString() <<
      "\n   : type: : " << fcexp->get_function()->get_type()->class_name() <<
      "   : parent type: : " << isSgExpression(fcexp->get_function()->get_parent())->get_type()->class_name() <<
      "   unparse: " << fcexp->unparseToString() << endl;
      if (isStringModifyingFunctionCall(name) ||
            isFileIOFunctionCall(name)
      ) {
         vector<SgExpression*> args;
         // if this is a function call that has a variable on the left hand size,
         // then we want to push that variable first,
         // this is used, e.g. with  File* fp = fopen("file","r");
         // Therefore we need to go up and see if there is an AssignmentOperator
         // and get the var on the left side
         SgExpression* varOnLeft = getVariableLeftOfAssignmentFromChildOnRight(n);
         SgExpression* varOnLeftStr=NULL;
         if (varOnLeft) {
            // need to get the mangled_name of the varRefExp on left hand side
            varOnLeftStr = buildStringVal(getMangledNameOfExpression(varOnLeft));
         } else {
            varOnLeftStr = buildStringVal("NoAssignmentVar");
         }

         Rose_STL_Container<SgExpression*> expr = exprlist->get_expressions();
         Rose_STL_Container<SgExpression*>::const_iterator it = expr.begin();
         for (;it!=expr.end();++it) {
            SgExpression* ex = *it;
            args.push_back(ex);
         }
         SgStatement* stmt = NULL;
         if (refExp ) stmt = getSurroundingStatement(refExp);
         else if (mrefExp) stmt = getSurroundingStatement(mrefExp);
         SgExpression* pexpr = refExp;
         if (mrefExp) pexpr = mrefExp;
         ROSE_ASSERT(stmt);
         RtedArguments* funcCall = new RtedArguments(name, //func_name
               mangled_name,
               "",
               "",
               pexpr, //refExp,
               stmt,
               args,
               varOnLeftStr,
               varOnLeft,
               exprlist
         );
         ROSE_ASSERT(funcCall);
         cerr << " Is a interesting function : " << name << endl;
         function_call.push_back(funcCall);
      } else if(!isFunctionCallOnIgnoreList( name)) {
         SgStatement* fncallStmt = getSurroundingStatement( fcexp );
         ROSE_ASSERT( fncallStmt );

         // if we're able to, use the function definition's body as the end of
         // scope (for line number complaints).  If not, the callsite is good too.
         SgNode* end_of_scope = getDefiningDeclaration( fcexp );
         if( end_of_scope ) {
            end_of_scope =
            isSgFunctionDeclaration( end_of_scope )
            -> get_definition() -> get_body();
         } else {
            end_of_scope = fcexp;
            // FIXME 2: We may be adding a lot of unnecessary signature checks
            // If we don't have the definition, we must be doing separate
            // compilation.  We will then have to check the signature at runtime
            // since there's no guarantee our prototype, if any, is accurate.
            function_call_missing_def.push_back( fcexp );
         }
         scopes[ fncallStmt ] = end_of_scope;
      } else if( "free" == name ) {
         frees.push_back( fcexp );
      } else if( "realloc" == name ) {
         reallocs.push_back( fcexp );
      }
   }
}

#endif
