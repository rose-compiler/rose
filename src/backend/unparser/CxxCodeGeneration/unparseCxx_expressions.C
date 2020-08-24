/* unparse_expr.C
 * 
 * This C file contains the general unparse function for expressions and functions 
 * to unparse every kind of expression. Note that there are no definitions for the 
 * following functions (Sage didn't provide this): AssnExpr, ExprRoot, AbstractOp, 
 * ClassInit, DyCast, ForDecl, VConst, and ExprInit. 
 * 
 * NOTE: Look over WCharVal. Sage provides no public function to access p_valueUL, so
 * just use p_value for now. When Sage is rebuilt, we should be able to fix this.
 *
 */
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "unparser.h"
#include <limits>

// DQ (2/21/2019): Added to support remove_substring function.
#include <string>
#include <iostream>

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;
using namespace Rose;

#define OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES 0
#define OUTPUT_HIDDEN_LIST_DATA 0
#define OUTPUT_DEBUGGING_INFORMATION 0

#ifdef _MSC_VER
#include "Cxx_Grammar.h"
#endif

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"



void
Unparse_ExprStmt::unparseLanguageSpecificExpression(SgExpression* expr, SgUnparse_Info& info)
   {
  // This is the C and C++ specific expression code generation

#if 0
     printf ("In C/C++ Unparse_ExprStmt::unparseLanguageSpecificExpression ( expr = %p = %s ) language = %s \n",expr,expr->class_name().c_str(),languageName().c_str());
     curprint(string("\n /*    unparseLanguageSpecificExpression(): class name  = ") + expr->class_name().c_str() + " */ \n");
#endif

#if 0
     printf ("In unparseLanguageSpecificExpression(): info.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
     printf ("In unparseLanguageSpecificExpression(): info.SkipEnumDefinition()  = %s \n",(info.SkipEnumDefinition() == true) ? "true" : "false");
#endif

  // DQ (9/9/2016): These should have been setup to be the same.
     ROSE_ASSERT(info.SkipClassDefinition() == info.SkipEnumDefinition());

     switch (expr->variant())
        {
       // DQ (4/18/2013): I don't think this is ever called this way, IR node resolve to the derived classes not the base classes.
          case UNARY_EXPRESSION:
             {
               printf ("This should never be called: case UNARY_EXPRESSION\n");
               ROSE_ASSERT(false);

               unparseUnaryExpr (expr, info);
               break;
             }

       // DQ (4/18/2013): I don't think this is ever called this way, IR node resolve to the derived classes not the base classes.
          case BINARY_EXPRESSION: 
             {
               printf ("This should never be called: case BINARY_EXPRESSION \n");
               ROSE_ASSERT(false);

               unparseBinaryExpr(expr, info); break; 
             }

       // case EXPRESSION_ROOT:       { unparseExprRoot(expr, info); break; }
       // case EXPR_LIST:             { unparseExprList(expr, info); break; }
          case VAR_REF:               { unparseVarRef(expr, info); break; }
          case CLASSNAME_REF:         { unparseClassRef(expr, info); break; }
          case FUNCTION_REF:          { unparseFuncRef(expr, info); break; }
          case MEMBER_FUNCTION_REF:   { unparseMFuncRef(expr, info); break; }
          case UNSIGNED_INT_VAL:      { unparseUIntVal(expr, info); break; }
          case LONG_INT_VAL:          { unparseLongIntVal(expr, info); break; }
          case LONG_LONG_INT_VAL:     { unparseLongLongIntVal(expr, info); break; }
          case UNSIGNED_LONG_LONG_INT_VAL: { unparseULongLongIntVal(expr, info); break; }
          case UNSIGNED_LONG_INT_VAL: { unparseULongIntVal(expr, info); break; }
          case FLOAT_VAL:             { unparseFloatVal(expr, info); break; }
          case LONG_DOUBLE_VAL:       { unparseLongDoubleVal(expr, info); break; }
       // Liao, 6/18/2008 , UPC identifiers 
          case UPC_THREADS:           { unparseUpcThreads(expr, info); break; }
          case UPC_MYTHREAD:          { unparseUpcMythread(expr, info); break; }
          case FUNC_CALL:             { unparseFuncCall(expr, info); break; }
          case POINTST_OP:            { unparsePointStOp(expr, info); break; }
          case RECORD_REF:            { unparseRecRef(expr, info); break; }
          case DOTSTAR_OP:            { unparseDotStarOp(expr, info); break; }
          case ARROWSTAR_OP:          { unparseArrowStarOp(expr, info); break; }
          case EQ_OP:                 { unparseEqOp(expr, info); break; }
          case LT_OP:                 { unparseLtOp(expr, info); break; }
          case GT_OP:                 { unparseGtOp(expr, info); break; }
          case NE_OP:                 { unparseNeOp(expr, info); break; }
          case LE_OP:                 { unparseLeOp(expr, info); break; }
          case GE_OP:                 { unparseGeOp(expr, info); break; }
          case ADD_OP:                { unparseAddOp(expr, info); break; }
          case SUBT_OP:               { unparseSubtOp(expr, info); break; }
          case MULT_OP:               { unparseMultOp(expr, info); break; }
          case DIV_OP:                { unparseDivOp(expr, info); break; }
          case INTEGER_DIV_OP:        { unparseIntDivOp(expr, info); break; }
          case MOD_OP:                { unparseModOp(expr, info); break; }
          case AND_OP:                { unparseAndOp(expr, info); break; }
          case OR_OP:                 { unparseOrOp(expr, info); break; }
          case BITXOR_OP:             { unparseBitXOrOp(expr, info); break; }
          case BITAND_OP:             { unparseBitAndOp(expr, info); break; }
          case BITOR_OP:              { unparseBitOrOp(expr, info); break; }
          case COMMA_OP:              { unparseCommaOp(expr, info); break; }
          case LSHIFT_OP:             { unparseLShiftOp(expr, info); break; }
          case RSHIFT_OP:             { unparseRShiftOp(expr, info); break; }
          case UNARY_MINUS_OP:        { unparseUnaryMinusOp(expr, info); break; }
          case UNARY_ADD_OP:          { unparseUnaryAddOp(expr, info); break; }

          case SIZEOF_OP:             { unparseSizeOfOp(expr, info); break; }
          case UPC_LOCAL_SIZEOF_EXPR: { unparseUpcLocalSizeOfOp(expr, info); break; }
          case UPC_BLOCK_SIZEOF_EXPR: { unparseUpcBlockSizeOfOp(expr, info); break; }
          case UPC_ELEM_SIZEOF_EXPR:  { unparseUpcElemSizeOfOp(expr, info); break; }

       // DQ (6/20/2013): Added alignof operator to support C/C++ extensions (used in EDG 4.7).
          case ALIGNOF_OP:            { unparseAlignOfOp(expr, info); break; }

       // DQ (2/5/2015): Added missing C++11 support.
          case NOEXCEPT_OP:           { unparseNoexceptOp(expr, info); break; }

          case TYPEID_OP:               { unparseTypeIdOp(expr, info); break; }
          case NOT_OP:                  { unparseNotOp(expr, info); break; }
          case DEREF_OP:                { unparseDerefOp(expr, info); break; }
          case ADDRESS_OP:              { unparseAddrOp(expr, info); break; }
          case MINUSMINUS_OP:           { unparseMinusMinusOp(expr, info); break; }
          case PLUSPLUS_OP:             { unparsePlusPlusOp(expr, info); break; }
          case BIT_COMPLEMENT_OP:       { unparseBitCompOp(expr, info); break; }
          case REAL_PART_OP:            { unparseRealPartOp(expr, info); break; }
          case IMAG_PART_OP:            { unparseImagPartOp(expr, info); break; }
          case CONJUGATE_OP:            { unparseConjugateOp(expr, info); break; }
          case EXPR_CONDITIONAL:        { unparseExprCond(expr, info); break; }
          case CAST_OP:                 { unparseCastOp(expr, info); break; }
          case ARRAY_OP:                { unparseArrayOp(expr, info); break; }
          case NEW_OP:                  { unparseNewOp(expr, info); break; }
          case DELETE_OP:               { unparseDeleteOp(expr, info); break; }
          case THIS_NODE:               { unparseThisNode(expr, info); break; }
          case SCOPE_OP:                { unparseScopeOp(expr, info); break; }
          case ASSIGN_OP:               { unparseAssnOp(expr, info); break; }
          case PLUS_ASSIGN_OP:          { unparsePlusAssnOp(expr, info); break; }
          case MINUS_ASSIGN_OP:         { unparseMinusAssnOp(expr, info); break; }
          case AND_ASSIGN_OP:           { unparseAndAssnOp(expr, info); break; }
          case IOR_ASSIGN_OP:           { unparseIOrAssnOp(expr, info); break; }
          case MULT_ASSIGN_OP:          { unparseMultAssnOp(expr, info); break; }
          case DIV_ASSIGN_OP:           { unparseDivAssnOp(expr, info); break; }
          case MOD_ASSIGN_OP:           { unparseModAssnOp(expr, info); break; }
          case XOR_ASSIGN_OP:           { unparseXorAssnOp(expr, info); break; }
          case LSHIFT_ASSIGN_OP:        { unparseLShiftAssnOp(expr, info); break; }
          case RSHIFT_ASSIGN_OP:        { unparseRShiftAssnOp(expr, info); break; }
          case TYPE_REF:                { unparseTypeRef(expr, info); break; }
          case EXPR_INIT:               { unparseExprInit(expr, info); break; }
          case AGGREGATE_INIT:          { unparseAggrInit(expr, info); break; }
          case COMPOUND_INIT:           { unparseCompInit(expr, info); break; }
          case CONSTRUCTOR_INIT:        { unparseConInit(expr, info); break; }
          case ASSIGN_INIT:             { unparseAssnInit(expr, info); break; }

       // DQ (11/15/2016): Adding support for braced initializer node.
          case BRACED_INIT:             { unparseBracedInit(expr, info); break; }

          case THROW_OP:                { unparseThrowOp(expr, info); break; }
          case VA_START_OP:             { unparseVarArgStartOp(expr, info); break; }
          case VA_START_ONE_OPERAND_OP: { unparseVarArgStartOneOperandOp(expr, info); break; }
          case VA_OP:                   { unparseVarArgOp(expr, info); break; }
          case VA_END_OP:               { unparseVarArgEndOp(expr, info); break; }
          case VA_COPY_OP:              { unparseVarArgCopyOp(expr, info); break; }
          case NULL_EXPR:               { unparseNullExpression(expr, info); break; }
          case STMT_EXPR:               { unparseStatementExpression(expr, info); break; }
          case ASM_OP:                  { unparseAsmOp (expr, info); break; }
          case DESIGNATED_INITIALIZER:  { unparseDesignatedInitializer(expr, info); break; }
          case PSEUDO_DESTRUCTOR_REF:   { unparsePseudoDtorRef(expr, info); break; }
          case KERN_CALL:               { unparseCudaKernelCall(expr, info); break; }

       // DQ (2/26/2012): Added support for template function calls (member and non-member).
          case TEMPLATE_FUNCTION_REF:        { unparseTemplateFuncRef(expr, info);  break; }
          case TEMPLATE_MEMBER_FUNCTION_REF: { unparseTemplateMFuncRef(expr, info); break; }

       // DQ (7/21/2012): This is only called if we process C++ code using the Cxx11 option.
       // This can be demonstrated on test2012_133.C (any maybe many other places too).
          case TEMPLATE_PARAMETER_VAL:  { unparseTemplateParameterValue(expr, info); break; }

       // DQ (7/12/2013): Added support for unparsing type trait builtin expressions (operators).
          case TYPE_TRAIT_BUILTIN_OPERATOR: { unparseTypeTraitBuiltinOperator(expr, info); break; }

       // DQ (9/4/2013): Added support for compund literals.
          case COMPOUND_LITERAL:        { unparseCompoundLiteral(expr, info); break; }

       // DQ (7/24/2014): Added more general support for type expressions (required for C11 generic macro support.
          case TYPE_EXPRESSION:         { unparseTypeExpression(expr, info); break; }

       // DQ (7/24/2014): Added more general support for type expressions (required for C11 generic macro support.
          case FUNCTION_PARAMETER_REF_EXP:  { unparseFunctionParameterRefExpression(expr, info); break; }

       // DQ (4/27/2014): This case appears in a snippet test code (testJava3a) as a result 
       // of something added to support the new shared memory DSL.  Not clear what this is,
       // I will ignore it for the moment as part of debugging this larger issue.
          case JAVA_TYPE_EXPRESSION: 
             {
               printf ("Warning: unparseLanguageSpecificExpression(): case SgJavaTypeExpression ignored \n");
               break;
             }

          case LAMBDA_EXP:  { unparseLambdaExpression(expr, info); break; }

       // DQ (11/21/2017): Adding support for GNU C/C++ extension for computed goto 
       // (and using what was previously only a Fortran IR node to support this).
          case LABEL_REF:   { unparseLabelRefExpression(expr, info); break; }
          case NONREAL_REF: { unparseNonrealRefExpression(expr, info); break; }

       // DQ (2/14/2019): Adding support for C++14 void values.
          case VOID_VAL:    { unparseVoidValue(expr,info); break; }

       // DQ (7/26/2020): Adding support for C++20 spaceship operator.
          case SPACESHIP_OP: { unparseSpaceshipOp(expr,info); break; }

       // DQ (7/26/2020): Adding support for C++17 fold operator.
          case FOLD_EXPR:    { unparseFoldExpression(expr,info); break; }

       // DQ (7/26/2020): Adding support for C++20 fold operator.
          case AWAIT_EXPR:   { unparseAwaitExpression(expr,info); break; }

       // DQ (7/26/2020): Adding support for C++20 fold operator.
          case CHOOSE_EXPR:  { unparseChooseExpression(expr,info); break; }

          default:
             {
            // printf ("Default reached in switch statement for unparsing expressions! expr = %p = %s \n",expr,expr->class_name().c_str());
               printf ("Default reached in switch statement for unparsing expressions! expr = %p = %s \n",expr,expr->class_name().c_str());
               ROSE_ASSERT(false);
               break;
             }
        }

#if 0
     printf ("Leaving unparseLanguageSpecificExpression(): info.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
     printf ("Leaving unparseLanguageSpecificExpression(): info.SkipEnumDefinition()  = %s \n",(info.SkipEnumDefinition() == true) ? "true" : "false");
#endif

  // DQ (9/9/2016): These should have been setup to be the same.
     ROSE_ASSERT(info.SkipClassDefinition() == info.SkipEnumDefinition());

#if 0
     printf ("Leaving C/C++ Unparse_ExprStmt::unparseLanguageSpecificExpression ( expr = %p = %s ) language = %s \n",expr,expr->class_name().c_str(),languageName().c_str());
     curprint(string("\n /* Leaving unparseLanguageSpecificExpression(): class name  = ") + expr->class_name().c_str() + " */ \n");
#endif
   }


void
Unparse_ExprStmt::unparseVoidValue(SgExpression* expr, SgUnparse_Info& info)
   {
  // DQ (2/14/2019): Not clear what to output here.
     curprint(" /* void value unparsed */ ");
   }


void
Unparse_ExprStmt::unparseLabelRefExpression(SgExpression* expr, SgUnparse_Info& info)
   {
  // DQ (11/21/2017): Adding support for GNU C extension for computed goto.

     SgLabelRefExp* labelRefExp = isSgLabelRefExp(expr);
     ASSERT_not_null(labelRefExp);

     ASSERT_not_null(labelRefExp->get_symbol());

     SgName name = labelRefExp->get_symbol()->get_name();

  // curprint("/* Output label reference expression */ ");

     curprint("&&");
     curprint(name);
   }

void
Unparse_ExprStmt::unparseNonrealRefExpression(SgExpression* expr, SgUnparse_Info& info) {
  SgNonrealRefExp * nr_refexp = isSgNonrealRefExp(expr);
  ASSERT_not_null(nr_refexp);

  SgName nameQualifier = nr_refexp->get_qualified_name_prefix();
  curprint(nameQualifier.str());

  SgNonrealSymbol * nrsym = nr_refexp->get_symbol();
  ASSERT_not_null(nrsym);

  SgNonrealDecl * nrdecl = nrsym->get_declaration();
  ASSERT_not_null(nrdecl);

  curprint(nrsym->get_name().str());

  SgTemplateArgumentPtrList & tpl_args = nrdecl->get_tpl_args();
  unparseTemplateArgumentList(tpl_args, info);
}

void
Unparse_ExprStmt::unparseLambdaExpression(SgExpression* expr, SgUnparse_Info& info)
   {
     SgLambdaExp* lambdaExp = isSgLambdaExp(expr);
     ASSERT_not_null(lambdaExp);

#if 0
     printf ("In unparseLambdaExpression(expr = %p = %s) \n",expr,expr->class_name().c_str());
#endif

     // Liao, 7/1/2016
     // To workaround some wrong AST generated from RAJA LULESH code
     // we clear skip base type flag of unparse_info
     if (info.SkipBaseType())
        {
       // DQ (4/7/2018): cleanup output spew (review with Liao).
       // cout<<"Warning in Unparse_ExprStmt::unparseLambdaExpression().  Unparse_Info has skipBaseType() set. Unset it now."<<endl;
       // ROSE_ASSERT(false);

          info.unset_SkipBaseType ();
        }

     curprint(" [");
     // if '=' or '&' exists
     bool hasCaptureCharacter = false;
     int commaCounter = 0;

     // schroder3 (2016-08-23): Do not print "&" AND "=" (because "[&=](){}" is ill-formed):
     if (lambdaExp->get_capture_default() == true) {
       if (lambdaExp->get_default_is_by_reference() == true) {
         curprint("&");
       }
       else {
         curprint("=");
       }
       hasCaptureCharacter = true;
     }
     else {
       // schroder3 (2016-08-23): Consistency check: If there is no capture default then there should be no
       //  by-reference-capture default:
       ROSE_ASSERT(!lambdaExp->get_default_is_by_reference());
     }



     ASSERT_not_null(lambdaExp->get_lambda_capture_list());
     size_t bound = lambdaExp->get_lambda_capture_list()->get_capture_list().size();
     for (size_t i = 0; i < bound; i++)
        {
          SgLambdaCapture* lambdaCapture = lambdaExp->get_lambda_capture_list()->get_capture_list()[i];
          ASSERT_not_null(lambdaCapture);

          // schroder3 (2016-08-23): Do not print implicit captures because this generates ill-formed code if
          //  there is a capture default (C++ standard section [expr.prim.lambda] point 8) (g++ allows this in
          //  non-pedantic mode, clang++ does not). Example: do not transform "int i; [&](){i;};" to ill-formed
          //  "int i; [&, &i](){i;}"). In addition, this change prevents the printing of "&this" (which is
          //  ill-formed too) when "this" is implicitly captured.
          if (!lambdaCapture->get_implicit() && lambdaCapture->get_capture_variable() != NULL)
             {

              // Liao 6/24/2016, we output ",item" when 
              // When not output , : first comma and there is no previous = or & character
              if (commaCounter == 0) // look backwards one identifier
              {
                if (hasCaptureCharacter)
                  curprint(",");
                commaCounter ++; 
              }
              else {
                curprint(",");
              }

              SgExpression * capt_var_expr = lambdaCapture->get_capture_variable();
              ASSERT_not_null(capt_var_expr);

          // TV (11/14/2018): ROSE-1525: Made a separated case when 'this' is captured to properly handle the changes in EDG 4.14
             if (isSgThisExp(capt_var_expr)) {
#if ((ROSE_EDG_MAJOR_VERSION_NUMBER == 4) && (ROSE_EDG_MINOR_VERSION_NUMBER >= 14) ) || (ROSE_EDG_MAJOR_VERSION_NUMBER > 4)
               if (lambdaCapture->get_capture_by_reference() == false) {
                 curprint("*");
               }
#endif
               curprint("this");
             } else {
               if (lambdaCapture->get_capture_by_reference() == true)
                  {
                    curprint("&");
                  }
               unp->u_exprStmt->unparseExpression(capt_var_expr,info);
             }

             }

#if 0
          if (i < bound-1)
             {
               curprint(",");
             }
#endif             
        }
     curprint("] ");

     SgFunctionDeclaration* lambdaFunction =  lambdaExp->get_lambda_function();
     ASSERT_not_null(lambdaFunction);
     ASSERT_not_null(lambdaFunction->get_firstNondefiningDeclaration());
     ASSERT_not_null(lambdaFunction->get_definingDeclaration());

#if 0
     printf ("lambdaFunction                                    = %p = %s \n",lambdaFunction,lambdaFunction->class_name().c_str());
     printf ("lambdaFunction->get_firstNondefiningDeclaration() = %p = %s \n",lambdaFunction->get_firstNondefiningDeclaration(),lambdaFunction->get_firstNondefiningDeclaration()->class_name().c_str());
     printf ("lambdaFunction->get_definingDeclaration()         = %p = %s \n",lambdaFunction->get_definingDeclaration(),lambdaFunction->get_definingDeclaration()->class_name().c_str());
#endif

     if (lambdaFunction->get_functionModifier().isCudaHost()) {
       curprint("__host__ ");
     }
     if (lambdaFunction->get_functionModifier().isCudaKernel()) {
       curprint("__global__ ");
     }
     if (lambdaFunction->get_functionModifier().isCudaDevice()) {
       curprint("__device__ ");
     }

     if (lambdaExp->get_has_parameter_decl() == true)
        {
       // Output the function parameters
          curprint("(");
#if 0
          printf ("In unparseLambdaExpression(): Calling unparseFunctionArgs(lambdaFunction = %p = %s) \n",lambdaFunction,lambdaFunction->class_name().c_str());
#endif
          unparseFunctionArgs(lambdaFunction,info);
#if 0
          printf ("In unparseLambdaExpression(): DONE: Calling unparseFunctionArgs(lambdaFunction = %p = %s) \n",lambdaFunction,lambdaFunction->class_name().c_str());
#endif
          curprint(")");
        }

     if (lambdaExp->get_is_mutable() == true)
        {
          curprint(" mutable ");
        }

#if 0
     if (lambdaFunction->get_is_mutable() == true)
        {
          curprint(" throw() ");
        }
#else
#if 0
     printf ("Lambda function throw keyword not yet supported! \n");
#endif
#endif

     if (lambdaExp->get_explicit_return_type() == true)
        {
#if 1
          curprint(" -> ");
#else
       // DQ (7/5/2018): Debugging test2018_120.C.
          curprint("/* from unparseLambdaExpression() */ -> ");
#endif
          ASSERT_not_null(lambdaFunction);
          ASSERT_not_null(lambdaFunction->get_type());
          SgType* returnType = lambdaFunction->get_type()->get_return_type();
          ASSERT_not_null(returnType);
          unp->u_type->unparseType(returnType,info);
        }



  // Use a new SgUnparse_Info object to support supression of the SgThisExp where compiler generated.
  // This is required because the function is internally a member function but can't explicitly refer 
  // to a "this" expression.
     SgUnparse_Info ninfo(info);
     ninfo.set_supressImplicitThisOperator();

#if 0
     printf ("In unparseLambdaExpression(): BEFORE UNSET: ninfo.SkipEnumDefinition()     = %s \n",ninfo.SkipEnumDefinition() ? "true" : "false");
     printf ("In unparseLambdaExpression(): BEFORE UNSET: ninfo.SkipClassDefinition()    = %s \n",ninfo.SkipClassDefinition() ? "true" : "false");
     printf ("In unparseLambdaExpression(): BEFORE UNSET: ninfo.SkipFunctionDefinition() = %s \n",ninfo.SkipFunctionDefinition() ? "true" : "false");
#endif

  // DQ (2/19/2018): Need to unset the support to skip the function definitions so that the unparsing of the block will allow comments and CPP directives to be output.
     ninfo.unset_SkipEnumDefinition();
     ninfo.unset_SkipClassDefinition();
     ninfo.unset_SkipFunctionDefinition();

#if 0
     printf ("In unparseLambdaExpression(): AFTER UNSET ninfo.SkipFunctionDefinition() = %s \n",ninfo.SkipFunctionDefinition() ? "true" : "false");
#endif

#if 0
     printf ("In unparseLambdaExpression(): calling unparseStatement(lambdaFunction->get_definition()->get_body(), ninfo); \n");
     curprint (" /* In unparseLambdaExpression(): calling unparseStatement(lambdaFunction->get_definition()->get_body(), ninfo); */ ");
#endif

  // Output the function definition
     ASSERT_not_null(lambdaFunction->get_definition());
     unparseStatement(lambdaFunction->get_definition()->get_body(), ninfo);

#if 0
     printf ("In unparseLambdaExpression(): DONE: calling unparseStatement(lambdaFunction->get_definition()->get_body(), ninfo); \n");
     curprint (" /* In unparseLambdaExpression(): DONE: calling unparseStatement(lambdaFunction->get_definition()->get_body(), ninfo); */ ");
#endif

#if 0
     printf ("Leaving unparseLambdaExpression(expr = %p = %s) \n",expr,expr->class_name().c_str());
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }


// DQ (8/11/2014): Added more general support for function parameter expressions (required for C++11 support).
void
Unparse_ExprStmt::unparseFunctionParameterRefExpression (SgExpression* expr, SgUnparse_Info& info)
   {
     ASSERT_not_null(expr);

     SgFunctionParameterRefExp* functionParameterRefExp = isSgFunctionParameterRefExp(expr);
     ASSERT_not_null(functionParameterRefExp);

#if 1
     printf ("In unparseFunctionParameterRefExpression = %p = %s \n",functionParameterRefExp,functionParameterRefExp->class_name().c_str());
     printf ("   --- functionParameterRefExp->get_parameter_number()    = %d \n",functionParameterRefExp->get_parameter_number());
     printf ("   --- functionParameterRefExp->get_parameter_levels_up() = %d \n",functionParameterRefExp->get_parameter_levels_up());
     printf ("   --- functionParameterRefExp->get_parameter_type()      = %p \n",functionParameterRefExp->get_parameter_type());
#endif

#if 0
     if (functionParameterRefExp->get_base_expression() != NULL)
        {
          unp->u_exprStmt->unparseExpression(functionParameterRefExp->get_base_expression(),info);
        }
       else
        {
          ASSERT_not_null(functionParameterRefExp->get_base_type());
          unp->u_type->unparseType(functionParameterRefExp->get_base_type(),info);
        }
#else
  // unp->u_exprStmt->curprint(" /* In unparseFunctionParameterRefExpression() */ ");

  // DQ (2/14/2015): We at least require this sort of funcationality for C++11 test2015_13.C.
  // if (functionParameterRefExp->get_parameter_levels_up() == 0)
     if (functionParameterRefExp->get_parameter_number() == 0 && functionParameterRefExp->get_parameter_levels_up() == 0)
        {
          unp->u_exprStmt->curprint("this ");
        }
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }


// DQ (7/24/2014): Added more general support for type expressions (required for C11 generic macro support).
void
Unparse_ExprStmt::unparseTypeExpression (SgExpression* expr, SgUnparse_Info& info)
   {
     ASSERT_not_null(expr);

     printf ("In unparseTypeExpression(expr = %p = %s) \n",expr,expr->class_name().c_str());

#if 1
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }


// DQ (7/21/2012): Added support for new template IR nodes (only used in C++11 code so far, see test2012_133.C).
void
Unparse_ExprStmt::unparseTemplateParameterValue(SgExpression* expr, SgUnparse_Info& info)
   {
     SgTemplateParameterVal* template_parameter_value = isSgTemplateParameterVal(expr);
     ASSERT_not_null(template_parameter_value);

#if 0
     printf ("In unparseTemplateParameterValue(): template_parameter_value->get_template_parameter_position() = %d \n",template_parameter_value->get_template_parameter_position());
     printf ("In unparseTemplateParameterValue(): template_parameter_value->get_valueString()                 = %s \n",template_parameter_value->get_valueString().c_str());

     printf ("In unparseTemplateParameterValue(): Output the SgTemplateParameterVal valueString = %s \n",template_parameter_value->get_valueString().c_str());
#endif

     curprint(template_parameter_value->get_valueString());

  // printf ("Skip unparsing this IR node until we understand it's connection to C++11 better. \n");
  // DQ (8/23/2012): Uncommented to support debugging.
  // ROSE_ASSERT(false);
   }


#if 1
// DQ (4/25/2012): Added support for new template IR nodes.
void
Unparse_ExprStmt::unparseTemplateFuncRef(SgExpression* expr, SgUnparse_Info& info)
   {
     SgTemplateFunctionRefExp* func_ref = isSgTemplateFunctionRefExp(expr);
     ASSERT_not_null(func_ref);

#if 0
     printf ("Calling the template function unparseFuncRef<SgFunctionRefExp>(func_ref) \n");
#endif

  // Calling the template function unparseFuncRef<SgFunctionRefExp>(func_ref);
  // unparseFuncRefSupport(func_ref,info);
     unparseFuncRefSupport<SgTemplateFunctionRefExp>(expr,info);
   }
#else
void
Unparse_ExprStmt::unparseTemplateFuncRef(SgExpression* expr, SgUnparse_Info& info)
   {
     unp->u_exprStmt->curprint ("Output TemplateFunctionRefExp");
   }
#endif

#if 1
// DQ (4/25/2012): Added support for new template IR nodes.
void
Unparse_ExprStmt::unparseTemplateMFuncRef ( SgExpression* expr, SgUnparse_Info& info )
   {
     unparseMFuncRefSupport<SgTemplateMemberFunctionRefExp>(expr,info);
   }

#else
void
Unparse_ExprStmt::unparseTemplateMFuncRef(SgExpression* expr, SgUnparse_Info& info)
   {
     unp->u_exprStmt->curprint ("Output TemplateMemberFunctionRefExp");
   }
#endif

// DQ (2/16/2005): This function has been moved to this file from unparse_type.C
void
Unparse_ExprStmt::unparseTemplateName(SgTemplateInstantiationDecl* templateInstantiationDeclaration, SgUnparse_Info& info)
   {
     ASSERT_not_null(templateInstantiationDeclaration);

#if 0
     printf ("In unparseTemplateName(): templateInstantiationDeclaration = %p = %s \n",templateInstantiationDeclaration,templateInstantiationDeclaration->class_name().c_str());
     unp->u_exprStmt->curprint ("/* In unparseTemplateName(): output templateInstantiationDeclaration->get_templateName() */ ");
#endif

     unp->u_exprStmt->curprint ( templateInstantiationDeclaration->get_templateName().str());

  // DQ (8/24/2014): Made this a warning instead of an error (see unparseToString/test2004_35.C).
  // DQ (5/7/2013): I think these should be false so that the full type will be output.
     if (info.isTypeFirstPart()  == true)
        {
#if 0
          printf ("WARNING: In unparseTemplateName(): info.isTypeFirstPart() == true \n");
#endif
        }
  // ROSE_ASSERT(info.isTypeFirstPart()  == false);
     ROSE_ASSERT(info.isTypeSecondPart() == false);

#if 0
     printf ("In unparseTemplateName(): templateInstantiationDeclaration = %p = %s \n",templateInstantiationDeclaration,templateInstantiationDeclaration->class_name().c_str());
     unp->u_exprStmt->curprint ("/* In unparseTemplateName(): output unparseTemplateArgumentList() */ ");
#endif

  // DQ (6/21/2011): Refactored this code to generate more than templated class names.
     unparseTemplateArgumentList(templateInstantiationDeclaration->get_templateArguments(),info);

#if 0
     printf ("Leaving unparseTemplateName(): templateInstantiationDeclaration = %p = %s \n",templateInstantiationDeclaration,templateInstantiationDeclaration->class_name().c_str());
     unp->u_exprStmt->curprint ("/* Leaving unparseTemplateName() */ ");
#endif
   }


void
Unparse_ExprStmt::unparseTemplateFunctionName(SgTemplateInstantiationFunctionDecl* templateInstantiationFunctionDeclaration, SgUnparse_Info& info)
   {
  // DQ (6/21/2011): Generated this function from refactored call to unparseTemplateArgumentList
     ASSERT_not_null(templateInstantiationFunctionDeclaration);

     unp->u_exprStmt->curprint(templateInstantiationFunctionDeclaration->get_templateName().str());

     bool unparseTemplateArguments = templateInstantiationFunctionDeclaration->get_template_argument_list_is_explicit();

#if 0
  // DQ (6/29/2013): This controls if the template arguments for the function should be unparsed.
  // printf ("In unparseTemplateFunctionName(): templateInstantiationFunctionDeclaration->get_template_argument_list_is_explicit() = %s \n",
  //      templateInstantiationFunctionDeclaration->get_template_argument_list_is_explicit() ? "true" : "false");
     printf ("In unparseTemplateFunctionName(): name = %s unparse template arguments = %s \n",
          templateInstantiationFunctionDeclaration->get_templateName().str(),unparseTemplateArguments ? "true" : "false");
#endif

  // DQ (6/29/2013): Use the information recorded in the AST as to if this function has been used with 
  // template arguments in the original code.  If so then we always unparse the template arguments, if 
  // not then we never unparse the template arguments.  See test2013_242.C for an example of where this 
  // is significant in the generated code.  Note that this goes a long way toward making the generated
  // code look more like the original input code (where before we have always unparsed template arguments
  // resulting in some very long function calls in the generated code).  Note that if some template
  // arguments are specified and some are not then control over not unparsing template arguments that
  // where not explicit in the original code will be handled seperately in the near future (in the 
  // SgTemplateArgument IR nodes).
  // unparseTemplateArgumentList(templateInstantiationFunctionDeclaration->get_templateArguments(),info);
     if (unparseTemplateArguments == true)
        {
#if 0
          SgTemplateArgumentPtrList & templateArgList = templateInstantiationFunctionDeclaration->get_templateArguments();
          printf ("In unparseTemplateFunctionName(): templateArgList.size() = %zu \n",templateArgList.size());
          for (size_t i = 0; i < templateArgList.size(); i++)
             {
               printf ("--- templateArgList[%zu] = %p \n",i,templateArgList[i]);
             }
#endif
#if 0
          printf ("Calling unparseTemplateArgumentList() \n");
#endif
          unparseTemplateArgumentList(templateInstantiationFunctionDeclaration->get_templateArguments(),info);
        }
   }


void
Unparse_ExprStmt::unparseTemplateMemberFunctionName(SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDeclaration, SgUnparse_Info& info)
   {
  // DQ (5/25/2013): Generated this function to match that of unparseTemplateFunctionName().
     ASSERT_not_null(templateInstantiationMemberFunctionDeclaration);

  // unp->u_exprStmt->curprint(templateInstantiationMemberFunctionDeclaration->get_templateName().str());
     string function_name = templateInstantiationMemberFunctionDeclaration->get_templateName();

#if 0
     printf ("In unparseTemplateMemberFunctionName(): function_name before processing to remove >> references = %s \n",function_name.c_str());
#endif
#if 0
  // printf ("In unparseTemplateMemberFunctionName(): templateInstantiationMemberFunctionDeclaration->get_template_argument_list_is_explicit() = %s \n",
  //      templateInstantiationMemberFunctionDeclaration->get_template_argument_list_is_explicit() ? "true" : "false");
     printf ("In unparseTemplateMemberFunctionName(): name = %s unparse template arguments = %s \n",
          function_name.c_str(),templateInstantiationMemberFunctionDeclaration->get_template_argument_list_is_explicit() ? "true" : "false");
#endif

  // DQ (6/15/2013): Now that we have fixed template handling for member function name output in member 
  // function reference handling, we have to make sure that if handle cases where the operator name
  // (for a conversion operator) has template arguments.  We have to make sure we don't ouput names
  // that contain "<<" or ">>" (since these need an extra space to be unparsed in C++, somthing fixed
  // in C++11, as I recall).
  // This code is translating "s >> len;" to "s > > len;" in test2013_97.C.
     if (function_name != "operator>>")
        {
       // DQ (11/18/2012): Process the function name to remove any cases of ">>" from template names.
          string targetString      = ">>";
          string replacementString = "> >";
          size_t found = function_name.find(targetString);
          while (found != string::npos)
             {
               function_name.replace(found,targetString.length(),replacementString);
               found = function_name.find(targetString);
             }
        }
#if 0
     printf ("In unparseTemplateMemberFunctionName(): func_name after processing to remove >> references = %s \n",function_name.c_str());
#endif

     unp->u_exprStmt->curprint(function_name);

  // DQ (5/26/2013): test2013_194.C demonstrates that we need to drop the template argument list for the case of a constructor (I think).
  // I think that this applies to constructors, destructors, and conversion operators, but I am not sure...
  // unparseTemplateArgumentList(templateInstantiationMemberFunctionDeclaration->get_templateArguments(),info);
     bool isConstructor        = templateInstantiationMemberFunctionDeclaration->get_specialFunctionModifier().isConstructor();
     bool isDestructor         = templateInstantiationMemberFunctionDeclaration->get_specialFunctionModifier().isDestructor();
     bool isConversionOperator = templateInstantiationMemberFunctionDeclaration->get_specialFunctionModifier().isConversion();

  // DQ (5/26/2013): Output output the template argument list when this is not a constructor, destructor, or conversion operator.
     bool skipTemplateArgumentList = (isConstructor == true || isDestructor == true || isConversionOperator == true);

#if 0
     printf ("In unparseTemplateMemberFunctionName(): skipTemplateArgumentList = %s \n",skipTemplateArgumentList ? "true" : "false");
#endif

  // DQ (6/29/2013): Use the information recorded in the AST as to if this function has been used with 
  // template arguments in the original code.  See note in unparseTemplateFunctionName().
     bool unparseTemplateArguments = templateInstantiationMemberFunctionDeclaration->get_template_argument_list_is_explicit();
     if (unparseTemplateArguments == false)
        {
          skipTemplateArgumentList = true;
        }

     if (skipTemplateArgumentList == false)
        {
          unparseTemplateArgumentList(templateInstantiationMemberFunctionDeclaration->get_templateArguments(),info);
        }

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }

#if 0
// DQ (2/11/2019): Localize the logic specific to if a template argument should be unparsed or not.
bool
SgTemplateArgument::outputTemplateArgument()
   {
     bool returnValue = false;

#define DEBUG_OUTPUT_TEMPLATE_ARGUMENT 0

#if DEBUG_OUTPUT_TEMPLATE_ARGUMENT
     printf ("In SgTemplateArgument::outputTemplateArgument() \n");
#endif

     bool isExplicitlySpecified = this->get_explicitlySpecified();
     bool isPackElement    = this->get_is_pack_element();

  // DQ (2/11/2019): If this is a function then we can expect to use the isExplicitlySpecified, else it should not be used.
     SgNode* parentOfTemplateArgument = this->get_parent();
     SgClassDeclaration*    classDeclaration    = isSgClassDeclaration(parentOfTemplateArgument);
     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(parentOfTemplateArgument);
     if (classDeclaration != NULL)
        {
       // In this case the isExplicitlySpecified can NOT be used, so assume it is always true.
#if DEBUG_OUTPUT_TEMPLATE_ARGUMENT
          printf ("In SgTemplateArgument::outputTemplateArgument(): Template Argument is part of class instantiation declaration \n");
#endif
          isExplicitlySpecified = true;

       // DQ (2/11/2019): I think this is required to pass test2019_93.C.
       // isPackElement         = true;
        }
       else
        {
          if (functionDeclaration != NULL)
             {
            // In this case the isExplicitlySpecified CAN be used.
#if DEBUG_OUTPUT_TEMPLATE_ARGUMENT
               printf ("In SgTemplateArgument::outputTemplateArgument(): Template Argument is part of function instantiation declaration \n");
#endif
             }
            else
             {
#if DEBUG_OUTPUT_TEMPLATE_ARGUMENT
               printf ("In SgTemplateArgument::outputTemplateArgument(): Template Argument is neither a function nor a class: parentOfTemplateArgument = %p = %s \n",
                    parentOfTemplateArgument,parentOfTemplateArgument->class_name().c_str());
#endif
             }
        }

#if DEBUG_OUTPUT_TEMPLATE_ARGUMENT
     printf ("In SgTemplateArgument::outputTemplateArgument(): isPackElement         = %s \n",isPackElement ? "true" : "false");
     printf ("In SgTemplateArgument::outputTemplateArgument(): isExplicitlySpecified = %s \n",isExplicitlySpecified ? "true" : "false");
#endif

  // isPackElement = isPackElement && isExplicitlySpecified;
     if (isPackElement && isExplicitlySpecified)
       {
         isPackElement = false;
       }

#if DEBUG_OUTPUT_TEMPLATE_ARGUMENT
     printf ("In SgTemplateArgument::outputTemplateArgument(): (after reset) isPackElement = %s \n",isPackElement ? "true" : "false");
#endif

     bool isAnonymousClass = this->isTemplateArgumentFromAnonymousClass();

#if DEBUG_OUTPUT_TEMPLATE_ARGUMENT
     printf ("In SgTemplateArgument::outputTemplateArgument(): isAnonymousClass = %s \n",isAnonymousClass ? "true" : "false");
#endif

     bool isAssociatedWithLambdaExp = false;
     if (this->get_argumentType() == SgTemplateArgument::type_argument)
        {
#if DEBUG_OUTPUT_TEMPLATE_ARGUMENT
          printf ("In SgTemplateArgument::outputTemplateArgument(): found a SgTemplateArgument::type_argument \n");
#endif
          if (SgClassType * ctype = isSgClassType (this->get_type()))
             {
#if DEBUG_OUTPUT_TEMPLATE_ARGUMENT
               printf ("In SgTemplateArgument::outputTemplateArgument(): ctype != NULL \n");
#endif
               if (SgNode* pnode = ctype->get_declaration()->get_parent())
                  {
#if DEBUG_OUTPUT_TEMPLATE_ARGUMENT
                    printf ("In SgTemplateArgument::outputTemplateArgument(): pnode != NULL \n");
#endif
                    if (isSgLambdaExp(pnode))
                       {
#if DEBUG_OUTPUT_TEMPLATE_ARGUMENT
                         printf ("In SgTemplateArgument::outputTemplateArgument(): Found a SgLambdaExp parent for the class: set hasLambdaFollowed = true  \n");
#endif
                         isAssociatedWithLambdaExp = true;
#if 0
                         printf ("Exiting as a test! \n");
                         ROSE_ASSERT(false);
#endif
                       }
                  }
             }
        }

#if DEBUG_OUTPUT_TEMPLATE_ARGUMENT
     printf ("In SgTemplateArgument::outputTemplateArgument(): isAssociatedWithLambdaExp = %s \n",isAssociatedWithLambdaExp ? "true" : "false");
#endif

  // if ( ((*copy_iter)->get_argumentType() != SgTemplateArgument::start_of_pack_expansion_argument) && (isAnonymousClass == false) && (isPackElement == false) )
  // if ( ( (*copy_iter)->get_argumentType() != SgTemplateArgument::start_of_pack_expansion_argument ) && 
  //      (isAnonymousClass == false) && (isPackElement == false) && (isAssociatedWithLambdaExp == false) )
     if ( ( this->get_argumentType() != SgTemplateArgument::start_of_pack_expansion_argument ) && 
          (isAnonymousClass == false) && (isPackElement == false || isExplicitlySpecified == true) && (isAssociatedWithLambdaExp == false) )
        {
#if DEBUG_OUTPUT_TEMPLATE_ARGUMENT
          printf ("In SgTemplateArgument::outputTemplateArgument(): set returnValue = true \n");
#endif
          returnValue = true;
        }

#if DEBUG_OUTPUT_TEMPLATE_ARGUMENT
     printf ("Leaving SgTemplateArgument::outputTemplateArgument(): returnValue = %s \n",returnValue ? "true" : "false");
#endif

     return returnValue;
   }
#endif

#define DEBUG_OUTPUT_TEMPLATE_ARGUMENT 0

void SgTemplateArgument::outputTemplateArgument(bool & skip_unparsing, bool & stop_unparsing) {

#if DEBUG_OUTPUT_TEMPLATE_ARGUMENT
  printf ("outputTemplateArgument(this = %p (%s)\n", this, this->class_name().c_str());
  printf (" --- this->kind = %s \n", this->template_argument_kind().c_str());
#endif

  ROSE_ASSERT(!skip_unparsing);
  ROSE_ASSERT(!stop_unparsing);

  bool isExplicitlySpecified = this->get_explicitlySpecified();
#if DEBUG_OUTPUT_TEMPLATE_ARGUMENT
  printf (" --- isExplicitlySpecified = %s \n", isExplicitlySpecified ? "true" : "false");
#endif

  bool isPackElement         = this->get_is_pack_element();
#if DEBUG_OUTPUT_TEMPLATE_ARGUMENT
  printf (" --- isPackElement = %s \n", isPackElement ? "true" : "false");
#endif

  SgNode * parentOfTemplateArgument = this->get_parent();
  ASSERT_not_null(parentOfTemplateArgument);
  SgClassDeclaration * xdecl = isSgClassDeclaration(parentOfTemplateArgument);
  if (xdecl != NULL) {
#if DEBUG_OUTPUT_TEMPLATE_ARGUMENT
    printf (" !!! template argument for a class template => isExplicitlySpecified == true\n");
#endif
    isExplicitlySpecified = true;
  }

  if (isPackElement && isExplicitlySpecified) {
#if DEBUG_OUTPUT_TEMPLATE_ARGUMENT
    printf (" !!! isPackElement && isExplicitlySpecified => isPackElement == false\n");
#endif
    isPackElement = false;
  }

  bool isAnonymousClass = this->isTemplateArgumentFromAnonymousClass();
#if DEBUG_OUTPUT_TEMPLATE_ARGUMENT
  printf (" --- isAnonymousClass = %s \n", isAnonymousClass ? "true" : "false");
#endif

  bool isAssociatedWithLambdaExp = false;
  if (this->get_argumentType() == SgTemplateArgument::type_argument) {
    SgClassType * xtype = isSgClassType(this->get_type());
    if (xtype != NULL) {
#if DEBUG_OUTPUT_TEMPLATE_ARGUMENT
      printf ("   - xtype = %p (%s) = %s \n", xtype, xtype->class_name().c_str(), xtype->unparseToString().c_str());
#endif
      SgDeclarationStatement * xdecl = xtype->get_declaration();
      ASSERT_not_null(xdecl);
#if DEBUG_OUTPUT_TEMPLATE_ARGUMENT
      printf ("   - xdecl = %p (%s)\n", xdecl, xdecl->class_name().c_str());
#endif
      SgNode * pnode = xdecl->get_parent();
#if DEBUG_OUTPUT_TEMPLATE_ARGUMENT
      printf ("   - pnode = %p (%s)\n", pnode, pnode ? pnode->class_name().c_str() : "");
#endif
      SgLambdaExp * lambda_exp = isSgLambdaExp(pnode);
      if (lambda_exp != NULL) {
        isAssociatedWithLambdaExp = true;
      }
    }
  }
#if DEBUG_OUTPUT_TEMPLATE_ARGUMENT
  printf (" --- isAssociatedWithLambdaExp = %s \n", isAssociatedWithLambdaExp ? "true" : "false");
#endif

  bool isPackExpansionStart = this->get_argumentType() == SgTemplateArgument::start_of_pack_expansion_argument;
#if DEBUG_OUTPUT_TEMPLATE_ARGUMENT
  printf (" --- isPackExpansionStart = %s \n", isPackExpansionStart ? "true" : "false");
#endif

  if ( isPackExpansionStart || isAnonymousClass || ( isPackElement && !isExplicitlySpecified ) ) {
    skip_unparsing = true;
  }

  if ( isAssociatedWithLambdaExp ) {
    stop_unparsing = true;
  }
#if DEBUG_OUTPUT_TEMPLATE_ARGUMENT
  printf (" >>> skip_unparsing = %s \n", skip_unparsing ? "true" : "false");
  printf (" >>> stop_unparsing = %s \n", stop_unparsing ? "true" : "false");
#endif
}



void
Unparse_ExprStmt::unparseTemplateArgumentList(const SgTemplateArgumentPtrList & input_templateArgListPtr, SgUnparse_Info& info)
   {
  // DQ (7/23/2012): This is one of three locations where the template arguments are assembled and where 
  // the name generated identically (in each case) is critical.  Not clear how to best refactor this code.
  // The other two are:
  //      SgName SageBuilder::appendTemplateArgumentsToName( const SgName & name, const SgTemplateArgumentPtrList & templateArgumentsList)
  // and in:
  //      void SgDeclarationStatement::resetTemplateNameSupport ( bool & nameResetFromMangledForm, SgName & name )
  // It is less clear how to refactor this code.

#define DEBUG_TEMPLATE_ARGUMENT_LIST 0

#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
  // DQ (8/24/2012): Print this out a little less often; but still enough so that we know to fix this later.
     static int counter = 0;
     if (counter++ % 100 == 0)
        {
          printf ("In Unparse_ExprStmt::unparseTemplateArgumentList(): CRITICAL FUNCTION TO BE REFACTORED \n");
        }
#endif

#if DEBUG_TEMPLATE_ARGUMENT_LIST
     printf ("In unparseTemplateArgumentList(): templateArgListPtr.size() = %" PRIuPTR " \n",input_templateArgListPtr.size());
#endif

     SgUnparse_Info ninfo(info);

  // DQ (5/6/2013): This fixes the test2013_153.C test code.
     if (ninfo.isTypeFirstPart() == true)
        {
#if 0
          printf ("In unparseTemplateArgumentList(): resetting isTypeFirstPart() == false \n");
#endif
          ninfo.unset_isTypeFirstPart();
        }

     if (ninfo.isTypeSecondPart() == true)
        {
#if 0
          printf ("In unparseTemplateArgumentList(): resetting isTypeSecondPart() == false \n");
#endif
          ninfo.unset_isTypeSecondPart();
        }

  // DQ (5/6/2013): I think these should be false so that the full type will be output.
     ROSE_ASSERT(ninfo.isTypeFirstPart()  == false);
     ROSE_ASSERT(ninfo.isTypeSecondPart() == false);

  // DQ (2/10/2019): Make a copy to support removing the start_of_pack_expansion_argument which has been complicccated to deal with in unparsing.
  // const SgTemplateArgumentPtrList templateArgListPtr = input_templateArgListPtr;
     SgTemplateArgumentPtrList templateArgListPtr;
     SgTemplateArgumentPtrList::const_iterator copy_iter = input_templateArgListPtr.begin();

#if DEBUG_TEMPLATE_ARGUMENT_LIST
     printf ("In unparseTemplateArgumentList(): iterate over list: \n");
#endif

  // DQ (2/11/2019): Need to control use of empty <> in template argument list handling.
  // Even if we filter out template arguments, it should not be considered an empty list.
     bool isEmptyTemplateArgumentList = true;

     while (copy_iter != input_templateArgListPtr.end())
        {
       // DQ (2/11/2019): Need to control use of empty <> in template argument list handling.
          isEmptyTemplateArgumentList = false;

          SgTemplateArgument * tplarg = *copy_iter;
          ASSERT_not_null(tplarg);

#if DEBUG_TEMPLATE_ARGUMENT_LIST
          printf (" - tplarg = %s\n", tplarg->unparseToString().c_str());
#endif

       // DQ (2/11/2019): Use simpler version of code now that logic has been refactored.
          bool skipTemplateArgument = false;
          bool stopTemplateArgument = false;
          tplarg->outputTemplateArgument(skipTemplateArgument, stopTemplateArgument);

#if DEBUG_TEMPLATE_ARGUMENT_LIST
          printf (" - skipTemplateArgument = %d\n", skipTemplateArgument);
          printf (" - stopTemplateArgument = %d\n", stopTemplateArgument);
#endif

          if (stopTemplateArgument) {
            break;
          } else if (!skipTemplateArgument) {
            templateArgListPtr.push_back(tplarg);
          }

          copy_iter++;
        }

#if 0
     printf ("In unparseTemplateArgumentList(): ninfo.SkipClassDefinition() = %s \n",(ninfo.SkipClassDefinition() == true) ? "true" : "false");
     printf ("In unparseTemplateArgumentList(): ninfo.SkipEnumDefinition()  = %s \n",(ninfo.SkipEnumDefinition() == true) ? "true" : "false");
#endif

  // DQ (2/11/2019): Need to control use of empty <> in template argument list handling.
     if (isEmptyTemplateArgumentList == false)
        {
       // DQ (2/11/2019): Moved to outside of the loop over all template parameters.
          unp->u_exprStmt->curprint ( "< ");
#if 0
          unp->u_exprStmt->curprint ( " /* in template argument list */ ");
#endif
       // DQ (2/22/2019): Added assertion.  This fails for test2019_93.C and test2019_100.C
       // E.g. template<class ... Types> struct Tuple {}; Tuple<> t0;
       // ROSE_ASSERT(templateArgListPtr.empty() == false);
        }
       else
        {
       // DQ (2/22/2019): Added assertion.
          ROSE_ASSERT(templateArgListPtr.empty() == true);
        }

     if (templateArgListPtr.empty() == false)
        {
#if DEBUG_TEMPLATE_ARGUMENT_LIST
          printf ("In unparseTemplateArgumentList(): templateArgListPtr.empty() NOT EMPTY: templateArgListPtr.size() = %" PRIuPTR " \n",templateArgListPtr.size());
#endif
#if DEBUG_TEMPLATE_ARGUMENT_LIST || 0
          printf ("In unparseTemplateArgumentList(): iterate over list: \n");
          SgTemplateArgumentPtrList::const_iterator iter = templateArgListPtr.begin();
          while (iter != templateArgListPtr.end())
             {
               ASSERT_not_null(*iter);

            // printf ("In unparseTemplateArgumentList(): iterate over list: *iter = %p = %s \n",*iter,(*iter)->class_name().c_str());
               printf (" --- *iter = %p = %s \n",*iter,(*iter)->class_name().c_str());
               printf (" --- *iter kind = %s \n",(*iter)->template_argument_kind().c_str());

               iter++;
             }

       // printf ("Calling unparseToStringSupport(): \n");
       // templateArgListPtr.unparseToStringSupport();
#endif
       // DQ (4/18/2005): We would like to avoid output of "<>" if possible so verify that there are template arguments
          ROSE_ASSERT(templateArgListPtr.size() > 0);

       // DQ (5/6/2013): I think these should be false so that the full type will be output.
          ROSE_ASSERT(ninfo.isTypeFirstPart()  == false);
          ROSE_ASSERT(ninfo.isTypeSecondPart() == false);

       // DQ (2/11/2019): Moved to outside of the loop over all template parameters.
       // unp->u_exprStmt->curprint ( "< ");
          SgTemplateArgumentPtrList::const_iterator i = templateArgListPtr.begin();
          while (i != templateArgListPtr.end())
             {
            // DQ (2/10/2019): Add this since we now filter out SgTemplateArgument::start_of_pack_expansion_argument.
               ROSE_ASSERT((*i)->get_argumentType() != SgTemplateArgument::start_of_pack_expansion_argument);

            // skip pack expansion argument, it will be NULL anyway
               if ((*i)->get_argumentType() == SgTemplateArgument::start_of_pack_expansion_argument)
                  {
                    i++;
                    continue;
                  }

#if DEBUG_TEMPLATE_ARGUMENT_LIST
               printf ("In unparseTemplateArgumentList(): templateArgList element *i = %p = %s explicitlySpecified = %s \n",*i,(*i)->class_name().c_str(),((*i)->get_explicitlySpecified() == true) ? "true" : "false");
#endif
#if 0
               unp->u_exprStmt->curprint ( string("/* unparseTemplateArgumentList(): templateArgument is explicitlySpecified = ") + (((*i)->get_explicitlySpecified() == true) ? "true" : "false") + " */");
#endif

#if DEBUG_TEMPLATE_ARGUMENT_LIST
               printf ("In unparseTemplateArgumentList(): Calling unparseTemplateArgument(): *i = %p = %s \n",*i,(*i)->class_name().c_str());
#endif
            // unparseTemplateArgument(*i,info);
               unparseTemplateArgument(*i,ninfo);

#if DEBUG_TEMPLATE_ARGUMENT_LIST
               printf ("In unparseTemplateArgumentList(): DONE: Calling unparseTemplateArgument(): *i = %p = %s \n",*i,(*i)->class_name().c_str());
#endif

               i++;

               // When to output , ?  the argument must not be the last one.
               if (i != templateArgListPtr.end())
                  {
                 // check if this is a class type for C++ 11 lambda function
                 // bool hasLambdaFollowed = false;
                    SgTemplateArgument* arg = *i; 
                    if (arg != NULL) 
                       {
                      // DQ (1/25/2019): Added assertion.
                      // ASSERT_not_null(arg->get_type());
#if 0
                      // printf ("Check if this is a lambda function comming next! arg->get_type() = %p = %s \n",arg->get_type(),arg->get_type()->class_name().c_str());
#endif
                         if (SgClassType * ctype = isSgClassType (arg->get_type()))
                            {
#if DEBUG_TEMPLATE_ARGUMENT_LIST
                              printf ("ctype != NULL \n");
#endif
                              if (SgNode* pnode = ctype->get_declaration()->get_parent())
                                 {
#if DEBUG_TEMPLATE_ARGUMENT_LIST
                                   printf ("pnode != NULL \n");
#endif
                                   if (isSgLambdaExp(pnode))
                                      {
#if DEBUG_TEMPLATE_ARGUMENT_LIST || 0
                                        printf ("isSgLambdaExp(pnode) != NULL (also set hasLambdaFollowed = true) \n");
#endif
                                     // hasLambdaFollowed = true; 
                                      }
                                 }

                           // DQ (1/21/2018): Check if this is an unnamed class (used as a template argument, which is not alloweded, so we should not unparse it).
                              bool isAnonymous = isAnonymousClass(ctype);
                              if (isAnonymous == true)
                                 {
#if DEBUG_TEMPLATE_ARGUMENT_LIST || 0
                                   printf ("isAnonymous == true (also set hasLambdaFollowed = true) \n");
#endif
                                // DQ (1/21/2018): This is mixing logic for explicitlySpecified with something Liao introduced 
                                // which checks for a trailing lambda function.  So we should fix this up later.
                                // hasLambdaFollowed = true; 
                                 }
#if 0
                                else
                                 {
#if DEBUG_TEMPLATE_ARGUMENT_LIST
                                   printf ("isAnonymous == false \n");
#endif
                                   SgClassDeclaration* classDeclaration = isSgClassDeclaration(ctype->get_declaration());
                                   if (classDeclaration != NULL)
                                      {
                                        printf ("In unparseTemplateArgumentList(): last template argument: classDeclaration->get_name() = %s \n",classDeclaration->get_name().str());
                                      }
                                 }
#endif
                            }
#if 0
                           else
                            {
#if DEBUG_TEMPLATE_ARGUMENT_LIST
                              printf ("ctype == NULL \n");
#endif
#if 0
                              ASSERT_not_null(arg->get_type());
                              printf ("arg->get_type() = %p = %s \n",arg->get_type(),arg->get_type()->class_name().c_str());
#endif
                            }
#endif
                       }

#if DEBUG_TEMPLATE_ARGUMENT_LIST
            // printf ("In unparseTemplateArgumentList(): templateArgList element *i = %p = %s hasLambdaFollowed = %s \n",*i,(*i)->class_name().c_str(), hasLambdaFollowed ? "true" : "false");
               printf ("In unparseTemplateArgumentList(): templateArgList element *i = %p = %s \n",*i,(*i)->class_name().c_str());
               printf ("In unparseTemplateArgumentList(): explicitlySpecified = %s \n",(*i)->get_explicitlySpecified() ? "true" : "false");
#endif

#if 0
             // DQ (1/21/2018): I think this needs to be turned off to handle test2014_04.C, but turned on for test2018_04.C.
               if ((*i)->get_explicitlySpecified() == false)
                  {
#if 0
                    printf ("In unparseTemplateArgumentList(): Found (*i)->get_explicitlySpecified() == false: set hasLambdaFollowed = true \n");
#endif
                 // DQ (1/21/2018): This is mixing logic for explicitlySpecified with something Liao introduced 
                 // which checks for a trailing lambda function.  So we should fix this up later.
                    hasLambdaFollowed = true; 
                  }
#endif
#if 0
            // DQ (1/21/2018): Check if this is an unnamed class (used as a template argument, which is not alloweded, so we should not unparse it).
               if (SgClassType * ctype = isSgClassType (arg->get_type()))
                  {
                    bool isAnonymous = isAnonymousClass(ctype);
                    if (isAnonymous == true)
                       {
                      // DQ (1/21/2018): This is mixing logic for explicitlySpecified with something Liao introduced 
                      // which checks for a trailing lambda function.  So we should fix this up later.
                         hasLambdaFollowed = true; 
                       }
                  }
#endif

               // When to skip , ?
               // condition 1: next item is a lambda function
              //  Or condition 2:  next item is an ending parameter pack argument (parameter pack argument in the middle should have , )
              //
              //  Negate this , we get when to output ,
              //
#if 0
               bool isStartOfPragmaPackAtEndOfList = ((*i)->get_argumentType() == SgTemplateArgument::start_of_pack_expansion_argument && ((i+1)== templateArgListPtr.end())  );
               SgTemplateArgumentPtrList::const_iterator next_iter = i;
               if (next_iter != templateArgListPtr.end())
                  {
                    next_iter++;
                  }
               bool isNextArgumentAnonymousClass   = false;
               bool isStartOfPragmaPackAtEndOfList = false;
               if (next_iter != templateArgListPtr.end())
                  {
                    SgClassType * next_classtype = isSgClassType ((*next_iter)->get_type());
                    isNextArgumentAnonymousClass = isAnonymousClass(next_classtype);
                    isStartOfPragmaPack = ((*i)->get_argumentType() == SgTemplateArgument::start_of_pack_expansion_argument && ((next_iter)== templateArgListPtr.end())  );
                  }
#if DEBUG_TEMPLATE_ARGUMENT_LIST
               printf ("isStartOfPragmaPackAtEndOfList = %s \n",isStartOfPragmaPackAtEndOfList ? "true" : "false");
               printf ("isAnonymousClassAtEndOfList    = %s \n",isAnonymousClassAtEndOfList ? "true" : "false");
#endif
#endif

#if 1
            // DQ (2/8/2019): The start_of_pack_expansion_argument can appear anywhere in the list 
            // (see Cxx11_tests/test2019_97.C), so we can't break out of the loop the first time we see it.

            // DQ (1/25/2019): This might be the simpliest way to exit once we see a start_of_pack_expansion_argument.
               if ((*i)->get_argumentType() == SgTemplateArgument::start_of_pack_expansion_argument)
                  {
#if DEBUG_TEMPLATE_ARGUMENT_LIST
                    printf ("Calling break: This might be the simpliest way to exit once we see a start_of_pack_expansion_argument \n");
#endif
                    break;
                  }
#else
            // DQ (2/8/2019): The start_of_pack_expansion_argument can appear anywhere in the list, so skip over it.
            // skip pack expansion argument, it will be NULL anyway
               if ((*i)->get_argumentType() == SgTemplateArgument::start_of_pack_expansion_argument)
                  {
                    i++;
                 // continue;
                  }
#endif

#if DEBUG_TEMPLATE_ARGUMENT_LIST || 0
            // printf ("In unparseTemplateArgumentList(): hasLambdaFollowed = %s \n",hasLambdaFollowed ? "true" : "false");
               printf ("(i+1) == templateArgListPtr.end() = %s \n",(i+1)== templateArgListPtr.end() ? "true" : "false");
#endif
            // DQ (2/11/2019): With the simpler logic we don't have to have this be anything more than true.
            // if (!(hasLambdaFollowed  || ((*i)->get_argumentType() == SgTemplateArgument::start_of_pack_expansion_argument && ((i+1)== templateArgListPtr.end())  )) )
            // if ( !(hasLambdaFollowed  || isStartOfPragmaPackAtEndOfList) )
               if (true)
                  {
                 // unp->u_exprStmt->curprint(" /* output comma: part 1 */ ");
                    unp->u_exprStmt->curprint(" , ");
                  }
                 else
                  {
#if DEBUG_TEMPLATE_ARGUMENT_LIST
                    printf ("In unparseTemplateArgumentList(): Skipping output of a specific template argument \n");
#endif
                  }
#if 0
                 // unp->u_exprStmt->curprint(" , ");
                 // Now the argument is in the middle. It's next argument must not be start_of_pack_expansion_argument
                 // Since the next argument will be unparsed to be NULL
                 // DQ (2/7/2015): See C++11 test2015_01.C for where we have to handle this special case.
                    if ((*i)->get_argumentType() == SgTemplateArgument::start_of_pack_expansion_argument)
                       {
                      // This should be the last template parameter, we I am not verifying this.
#if 0
                         printf ("In unparseTemplateArgumentList(): Need to supress the trailing \",\" \n");
#endif
                       }
                      else
                       {
                         unp->u_exprStmt->curprint(" , ");
                       }
#endif

                  }
             }

       // DQ (2/11/2019): Moved to outside of the loop over all template parameters.
       // unp->u_exprStmt->curprint(" > ");
        }
       else
        {
       // DQ (5/26/2014): In the case of a template instantiation with empty template argument list, output
       // a " " to be consistent with the behavior when there is a non-empty template argument list.
       // This is a better fix for the template issue that Robb pointed out and that was fixed last week.
          unp->u_exprStmt->curprint(" ");
        }

  // DQ (2/11/2019): Need to control use of empty <> in template argument list handling.
     if (isEmptyTemplateArgumentList == false)
        {
       // DQ (2/11/2019): Moved to outside of the loop over all template parameters.
          unp->u_exprStmt->curprint(" > ");
        }

#if DEBUG_TEMPLATE_ARGUMENT_LIST
     printf ("Leaving Unparse_ExprStmt::unparseTemplateArgumentList(): CRITICAL FUNCTION TO BE REFACTORED \n");
#endif
   }


void
Unparse_ExprStmt::unparseTemplateParameterList( const SgTemplateParameterPtrList & templateParameterList, SgUnparse_Info& info, bool is_template_header)
   {
#if 0
     printf ("In unparseTemplateParameterList(): templateParameterList.size() = %zu \n",templateParameterList.size());
#endif

     if (templateParameterList.empty() == false)
        {
          curprint ("< ");
          SgTemplateParameterPtrList::const_iterator i = templateParameterList.begin();
          while (i != templateParameterList.end())
             {
               SgTemplateParameter* templateParameter = *i;
               ASSERT_not_null(templateParameter);
#if 0
               printf ("In unparseTemplateParameterList(): templateParameter = %p \n",templateParameter);
#endif
               unparseTemplateParameter(templateParameter,info,is_template_header);

               i++;

               if (i != templateParameterList.end())
                  {
                 // unp->u_exprStmt->curprint(" /* output comma: part 2 */ ");
                    curprint (",");
                  }
             }

          curprint ("> ");
        }
   }


void
Unparse_ExprStmt::unparseTemplateParameter(SgTemplateParameter* templateParameter, SgUnparse_Info& info, bool is_template_header)
   {
     ASSERT_not_null(templateParameter);

#if 0
     printf ("In unparseTemplateParameter(): templateParameter = %p \n",templateParameter);
#endif

     switch(templateParameter->get_parameterType())
        {
          case SgTemplateParameter::type_parameter:
             {
            // DQ (9/7/2014): Added support for case SgTemplateParameter::type_parameter.
               SgType* type = templateParameter->get_type();
               ASSERT_not_null(type);
#if 0
               printf ("unparseTemplateParameter(): case SgTemplateParameter::type_parameter: type = %p = %s \n",type,type->class_name().c_str());
#endif

            // TV (04/17/2018): Not clear what the use case for other type of type is so let see where it breaks...
               SgNonrealType * nrtype = isSgNonrealType(type);
               ASSERT_not_null(nrtype);

               if (is_template_header)
                 curprint("typename ");
               curprint(nrtype->get_name());

               SgType* default_type = templateParameter->get_defaultTypeParameter();
               if (default_type != NULL)
                  {
                 // Need to add the default type.
#if 0
                    curprint("=");

                    SgUnparse_Info ninfo(info);
                    unp->u_type->unparseType(default_type,ninfo);
#else
                 // See test2014_149.C for an example of where this is mistakenly done in the defining declaration (where it is an error).
#if 0
                    printf ("Skipping default template parameter unparsing \n");
#endif
#endif
                  }
#if 0
               printf ("unparseTemplateParameter(): case SgTemplateParameter::type_parameter: Sorry, not implemented! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case SgTemplateParameter::nontype_parameter:
             {
               if (templateParameter->get_expression() != NULL)
                  {
#if 0
                    printf ("unparseTemplateParameter(): case SgTemplateParameter::nontype_parameter: templateParameter->get_expression() = %p = %s \n",templateParameter->get_expression(),templateParameter->get_expression()->class_name().c_str());
#endif
                    unp->u_exprStmt->unparseExpression(templateParameter->get_expression(),info);
                  }
                 else
                  {
                    ASSERT_not_null(templateParameter->get_initializedName());

                    SgType* type = templateParameter->get_initializedName()->get_type();
                    ASSERT_not_null(type);
#if 0
                    printf ("unparseTemplateParameter(): case SgTemplateParameter::nontype_parameter: templateParameter->get_initializedName()->get_type() = %p = %s \n",type,type->class_name().c_str());
#endif
                 // DQ (9/10/2014): Note that this will unparse "int T" which we want in the template header, but not in the template parameter list.
                 // unp->u_type->outputType<SgInitializedName>(templateParameter->get_initializedName(),type,info);
                    // TV (03/20/2018) only if it is a template header (not a specialization)
                    if (is_template_header) {
                      SgUnparse_Info ninfo(info);
                      unp->u_type->unparseType(type,ninfo);
                    }
                    curprint(templateParameter->get_initializedName()->get_name());
                  }

               break;
             }

          case SgTemplateParameter::template_parameter:
             {
               ASSERT_not_null(templateParameter->get_templateDeclaration());
               SgNonrealDecl* nrdecl = isSgNonrealDecl(templateParameter->get_templateDeclaration());
               ASSERT_not_null(nrdecl);
#if 0
               printf ("unparseTemplateParameter(): case SgTemplateParameter::template_parameter: output name = %s \n",templateDeclaration->get_name().str());
#endif
               curprint("template ");

               SgTemplateParameterPtrList & templateParameterList = nrdecl->get_tpl_params();
               Unparse_ExprStmt::unparseTemplateParameterList (templateParameterList, info, true);

               // TV (03/23/2018): could either be class or typename: where is the info in EDG? where to store it in the AST?
               curprint(" typename ");

               curprint(nrdecl->get_name());
#if 0
               printf ("unparseTemplateParameter(): case SgTemplateParameter::template_parameter: Sorry, not implemented! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          default:
             {
               printf ("Error: default reached \n");
               ROSE_ASSERT(false);
               break;
             }
        }
   }

bool
Unparse_ExprStmt::isAnonymousClass(SgType* templateArgumentType)
   {
     bool returnValue = false;

     SgClassType* classType = isSgClassType(templateArgumentType);
     if (classType != NULL)
        {
          SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
#if 0
          printf ("In isAnonymousClass(): case SgTemplateArgument::type_argument: classDeclaration = %p = %s classDeclaration->get_name() = %s \n",
               classDeclaration,classDeclaration->class_name().c_str(),classDeclaration->get_name().str());
#endif
          bool isUnnamed = (string(classDeclaration->get_name()).substr(0,14) == "__anonymous_0x");
#if 0
          if (isUnnamed == true)
             {
               printf ("In isAnonymousClass(): case SgTemplateArgument::type_argument: isUnnamed = %s \n",isUnnamed ? "true" : "false");
             }
#endif
          returnValue = isUnnamed;
        }

     return returnValue;
   }

#define DEBUG_UNPARSE_TEMPLATE_ARGUMENT 0

void
Unparse_ExprStmt::unparseTemplateArgument(SgTemplateArgument* templateArgument, SgUnparse_Info& info)
   {
     ASSERT_not_null(templateArgument);

#define DEBUG_TEMPLATE_ARGUMENT 0

#if DEBUG_TEMPLATE_ARGUMENT
     printf ("In unparseTemplateArgument() = %p (explicitlySpecified = %s) \n",templateArgument,(templateArgument->get_explicitlySpecified() == true) ? "true" : "false");
#endif

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES || DEBUG_TEMPLATE_ARGUMENT
     printf ("Unparse TemplateArgument (%p) \n",templateArgument);
     unp->u_exprStmt->curprint ( "\n/* Unparse TemplateArgument */ \n");
     unp->u_exprStmt->curprint ( "\n");
#endif

#if DEBUG_TEMPLATE_ARGUMENT
     unp->u_exprStmt->curprint(string("/* unparseTemplateArgument(): templateArgument is explicitlySpecified = ") + ((templateArgument->get_explicitlySpecified() == true) ? "true" : "false") + " */");
#endif
#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

#if 0
     printf ("In unparseTemplateArgument(): info.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
     printf ("In unparseTemplateArgument(): info.SkipEnumDefinition()  = %s \n",(info.SkipEnumDefinition() == true) ? "true" : "false");
#endif

  // DQ (9/9/2016): These should have been setup to be the same.
     ROSE_ASSERT(info.SkipClassDefinition() == info.SkipEnumDefinition());

#if 0
  // DQ (1/21/2018): Using the logic set in EDG/ROSE connection to control output of template arguments.
  // I have tried this previously, and I think it got hung up on details of default template arguments, but
  // since it impacts the support for lambda functions and compiler generated classes holding capture 
  // variables it is worth another try to use this logic.
     if (templateArgument->get_explicitlySpecified() == false)
        {
#if 0
          printf ("In unparseTemplateArgument(): Found templateArgument->get_explicitlySpecified() == false \n");
#endif
          return;
        }
#endif

     SgUnparse_Info newInfo(info);

  // DQ (8/6/2007): Turn this off now that we have a more sophisticated hidden declaration and hidden type list mechanism.
  // DQ (10/13/2006): Force template arguments to be fully qualified! (else they can 
  // now be turned off where the template instantiation appears in a namespace)!
  // DQ (10/14/2006): Since template can appear anywhere types referenced in template instantiation
  // declarations have to be fully qualified.  We can't tell from the template argument if it requires 
  // qualification we would need the type and the function declaration (and then some 
  // analysis).  So fully qualify all function parameter types.  This is a special case
  // (documented in the Unparse_ExprStmt::generateNameQualifier() member function.
  // newInfo.set_forceQualifiedNames();

#if DEBUG_UNPARSE_TEMPLATE_ARGUMENT
     printf (" -- newInfo.forceQualifiedNames()                 = %s \n", newInfo.forceQualifiedNames() ? "true" : "false");
     printf (" -- newInfo.requiresGlobalNameQualification()     = %s \n", newInfo.requiresGlobalNameQualification() ? "true" : "false");
     printf (" -- newInfo.get_name_qualification_length()       = %d \n", newInfo.get_name_qualification_length());
     printf (" -- newInfo.get_global_qualification_required()   = %s \n", newInfo.get_global_qualification_required() ? "true" : "false");
     printf (" -- newInfo.get_type_elaboration_required()       = %s \n", newInfo.get_type_elaboration_required() ? "true" : "false");
     printf (" -- templateArgument->get_name_qualification_length()     = %d \n", templateArgument->get_name_qualification_length());
     printf (" -- templateArgument->get_global_qualification_required() = %s \n", templateArgument->get_global_qualification_required() ? "true" : "false");
     printf (" -- templateArgument->get_type_elaboration_required()     = %s \n", templateArgument->get_type_elaboration_required() ? "true" : "false");
#endif

  // DQ (5/14/2011): Added support for newer name qualification implementation.
  // printf ("In unparseTemplateArgument(): templateArgument->get_name_qualification_length() = %d \n",templateArgument->get_name_qualification_length());
     newInfo.set_name_qualification_length(templateArgument->get_name_qualification_length());
     newInfo.set_global_qualification_required(templateArgument->get_global_qualification_required());
     newInfo.set_type_elaboration_required(templateArgument->get_type_elaboration_required());

  // DQ (5/30/2011): Added support for name qualification.
     newInfo.set_reference_node_for_qualification(templateArgument);
     ASSERT_not_null(newInfo.get_reference_node_for_qualification());

     if (newInfo.requiresGlobalNameQualification()) {
       newInfo.set_global_qualification_required(true);
       newInfo.set_reference_node_for_qualification(NULL);
     }

#if DEBUG_UNPARSE_TEMPLATE_ARGUMENT
     printf (" -- newInfo.get_reference_node_for_qualification() = %p \n",newInfo.get_reference_node_for_qualification());
     if (newInfo.get_reference_node_for_qualification() != NULL)
        {
          printf (" -- newInfo.get_reference_node_for_qualification() = %p = %s \n",newInfo.get_reference_node_for_qualification(),newInfo.get_reference_node_for_qualification()->class_name().c_str());
          unp->u_exprStmt->curprint(string("/* -- newInfo.get_reference_node_for_qualification() = ") + StringUtility::numberToString(newInfo.get_reference_node_for_qualification()) + " */");
        }
#endif

#if 0
     printf ("Exiting in unparseTemplateArgument() to see where this is called \n");
     ROSE_ASSERT(false);
#endif

  // ROSE_ASSERT(newInfo.isTypeFirstPart() == false);
  // ROSE_ASSERT(newInfo.isTypeSecondPart() == false);

#if 0
     printf ("In unparseTemplateArgument(): newInfo.isWithType()       = %s \n",(newInfo.isWithType()       == true) ? "true" : "false");
     printf ("In unparseTemplateArgument(): newInfo.SkipBaseType()     = %s \n",(newInfo.SkipBaseType()     == true) ? "true" : "false");
     printf ("In unparseTemplateArgument(): newInfo.isTypeFirstPart()  = %s \n",(newInfo.isTypeFirstPart()  == true) ? "true" : "false");
     printf ("In unparseTemplateArgument(): newInfo.isTypeSecondPart() = %s \n",(newInfo.isTypeSecondPart() == true) ? "true" : "false");
#endif

     if (newInfo.SkipBaseType() == true)
        {
#if DEBUG_TEMPLATE_ARGUMENT
          printf ("In unparseTemplateArgument(): unset SkipBaseType() (how was this set? Maybe from the function reference expression?) \n");
#endif
          newInfo.unset_SkipBaseType();
        }

#if DEBUG_TEMPLATE_ARGUMENT
     printf ("In unparseTemplateArgument(): templateArgument->get_argumentType() = %d \n",templateArgument->get_argumentType());
#endif

     switch (templateArgument->get_argumentType())
        {
          case SgTemplateArgument::type_argument:
             {
               ASSERT_not_null(templateArgument->get_type());

               SgType* templateArgumentType = templateArgument->get_type();
#if 0
               printf ("In unparseTemplateArgument(): case SgTemplateArgument::type_argument: templateArgument->get_type() = %s \n",templateArgumentType->class_name().c_str());
            // curprint ( "\n /* templateArgument->get_type() */ \n");
               SgNamedType* namedType = isSgNamedType(templateArgumentType);
               if (namedType != NULL) {
                 printf ("--- name = %s \n",namedType->get_name().str());
               }
#endif
            // DQ (1/21/2018): Check if this is an unnamed class (used as a template argument, which is not alloweded, so we should not unparse it).
               bool isAnonymous = isAnonymousClass(templateArgumentType);
               if (isAnonymous == true)
                  {
                 // DQ (2/10/2019): This is now filter and out to simplify template list processing.
                 // DQ (2/11/2019): I think we get this because functions other than the unparseTemplateArgumentList() 
                 // function can call this function.  E.g. unparseToString().
#if 0
                    printf ("In unparseTemplateArgument(): case SgTemplateArgument::type_argument: This is now filtered and out to simplify template list processing \n");
#endif
                 // ROSE_ASSERT(false);
#if 0
                    printf ("In unparseTemplateArgument(): case SgTemplateArgument::type_argument: found anonymous class (returning) \n");
#endif
                    return;
                  }

            // DQ (1/9/2017): If the result of get_type() was identified as containing parts with non public access then we want to use an alternative type alias.
            // The test for this is done on the whole of the AST within the ast post processing.
            // Note that this fix also requires that the name qualification support be computed using the unparsable_type_alias.
               if (templateArgument->get_unparsable_type_alias() != NULL)
                  {
#if 0
                    printf ("In unparseTemplateArgument(): selected an alternative type to unparse to work waround a bug in EDG (this is likely the original type specified in the source code) \n");
                 // DQ (3/30/2018): Can't call this without infinite recursion!
                 // printf ("--- were going to use: %s \n",templateArgument->unparseToString().c_str());
                 // printf ("--- selecing instead : %s \n",templateArgument->get_unparsable_type_alias()->unparseToString().c_str());
#endif
                    templateArgumentType = templateArgument->get_unparsable_type_alias();
                  }

#if OUTPUT_DEBUGGING_INFORMATION
               printf ("In unparseTemplateArgument(): templateArgument->get_type() = %s \n",templateArgumentType->class_name().c_str());
               unp->u_exprStmt->curprint ( "\n /* templateArgument->get_type() */ \n");
#endif
            // curprint ( "\n /* SgTemplateArgument::type_argument */ \n");

            // DQ (7/24/2011): Comment out to test going back to previous version befor unparsing array types correctly.
               newInfo.set_SkipClassDefinition();
               newInfo.set_SkipClassSpecifier();

            // DQ (7/24/2011): Added to prevent output of enum declarations with enum fields in template argument.
               newInfo.set_SkipEnumDefinition();
               
            // DQ (7/23/2011): These are required to unparse the full type directly (e.g. SgArrayType (see test2011_117.C).
            // DQ (11/27/2004): Set these (though I am not sure that they help!)
            // newInfo.unset_isTypeFirstPart();
            // newInfo.unset_isTypeSecondPart();

#if 0
            // DQ (7/24/2011): Output the first part (before the name qualification)
               newInfo.unset_isTypeFirstPart();
               newInfo.unset_isTypeSecondPart();

               newInfo.set_isTypeFirstPart();
               unp->u_type->unparseType(templateArgument->get_type(),newInfo);

            // DQ (7/24/2011): Output the second part (after the name qualification)
               newInfo.unset_isTypeFirstPart();
               newInfo.set_isTypeSecondPart();

            // Debugging...this will fail for unparseToString...
               ASSERT_not_null(newInfo.get_reference_node_for_qualification());
               printf ("newInfo.get_reference_node_for_qualification() = %p = %s \n",newInfo.get_reference_node_for_qualification(),newInfo.get_reference_node_for_qualification()->class_name().c_str());
#endif

#if 0
               printf ("In unparseTemplateArgument(): case SgTemplateArgument::type_argument: templateArgument->get_name_qualification_length() = %d \n",templateArgument->get_name_qualification_length());
#endif

#if 0
            // DQ (5/4/2013): I think we have to separate out the parts of the type so that the name qualificaion will not be output before the "const" for const types.
               newInfo.set_isTypeFirstPart();
               unp->u_type->unparseType(templateArgumentType,newInfo);
            // newInfo.unset_isTypeFirstPart();
               newInfo.set_isTypeSecondPart();
#endif
            // DQ (5/28/2011): We have to handle the name qualification directly since types can be qualified 
            // different and so it depends upon where the type is referenced.  Thus the qualified name is 
            // stored in a map to the IR node that references the type.
               SgName nameQualifier;
               if (templateArgument->get_name_qualification_length() > 0)
                  {
#if 0
                    printf ("In unparseTemplateArgument(): Found a valid name qualification: nameQualifier = %s \n",nameQualifier.str());
#endif

#if 1
                 // DQ (4/7/2013): If this is an enum type then we need to output an enum type specifier sometimes 
                 // (do so unconditionally since that always works).  See test2013_91.C as an example.
                 // if (isSgEnumType(templateArgument->get_type()) != NULL || isSgClassType(templateArgument->get_type()) != NULL)
                    if (isSgNamedType(templateArgument->get_type()) != NULL)
                       {
                      // Note that typedefs are not elaborated in C++.
#if 0
                         printf ("If this is an enum or class/struct/union type then output a type elaboration before any possible qualification: nameQualifier %s \n",nameQualifier.str());
#endif
                         if (isSgEnumType(templateArgument->get_type()) != NULL)
                            {
                              curprint("enum ");
                            }
                           else
                            {
                              if (isSgClassType(templateArgument->get_type()) != NULL)
                                 {
                                   curprint("class ");
                                 }
                            }
                       }
#endif
                 // newInfo.display("In unparseTemplateArgument(): newInfo.display()");

                 // DQ (5/5/2013): Refactored code used here and in the unparseFunctionParameterDeclaration().
                    unp->u_type->outputType<SgTemplateArgument>(templateArgument,templateArgumentType,newInfo);
                  }
                 else
                  {
                 // DQ (7/23/2011): To unparse the type directly we can't have either of these set!
                 // ROSE_ASSERT(newInfo.isTypeFirstPart()  == false);
                 // ROSE_ASSERT(newInfo.isTypeSecondPart() == false);

                 // This will unparse the type will any required name qualification.
#if 0
                    printf ("In unparseTemplateArgument(): Calling unparseType(templateArgument->get_type(),newInfo); (second part) templateArgument->get_type() = %p = %s \n",templateArgument->get_type(),templateArgument->get_type()->class_name().c_str());
                    curprint ( "\n /* second part of type */ \n");
#endif
#if 0
                    printf ("In unparseTemplateArgument(): Before calling unparseType: newInfo.SkipClassDefinition() = %s \n",(newInfo.SkipClassDefinition() == true) ? "true" : "false");
                    printf ("In unparseTemplateArgument(): Before calling unparseType: newInfo.SkipEnumDefinition()  = %s \n",(newInfo.SkipEnumDefinition()  == true) ? "true" : "false");
#endif
                 // unp->u_type->unparseType(templateArgument->get_type(),newInfo);
                    unp->u_type->unparseType(templateArgumentType,newInfo);
#if 0
                    printf ("DONE: In unparseTemplateArgument(): Calling unparseType(templateArgument->get_type(),newInfo); \n");
#endif
#if 0
                    curprint ( "\n /* end of type */ \n");
#endif
#if 0
                    printf ("In unparseTemplateArgument(): After calling unparseType: newInfo.SkipClassDefinition() = %s \n",(newInfo.SkipClassDefinition() == true) ? "true" : "false");
                    printf ("In unparseTemplateArgument(): After calling unparseType: newInfo.SkipEnumDefinition()  = %s \n",(newInfo.SkipEnumDefinition()  == true) ? "true" : "false");
#endif
                  }
               break;
             }

          case SgTemplateArgument::nontype_argument:
             {
            // DQ (8/12/2013): This can be either an SgExpression or SgInitializedName.
            // ASSERT_not_null(templateArgument->get_expression());
               ROSE_ASSERT (templateArgument->get_expression() != NULL || templateArgument->get_initializedName() != NULL);
               ROSE_ASSERT (templateArgument->get_expression() == NULL || templateArgument->get_initializedName() == NULL);
               if (templateArgument->get_expression() != NULL)
                  {
#if 0
                    printf ("In unparseTemplateArgument(): case SgTemplateArgument::nontype_argument: templateArgument->get_expression() = %s \n",templateArgument->get_expression()->class_name().c_str());
#endif
#if OUTPUT_DEBUGGING_INFORMATION
                    printf ("In unparseTemplateArgument(): templateArgument->get_expression() = %s \n",templateArgument->get_expression()->class_name().c_str());
                    unp->u_exprStmt->curprint ( "\n /* templateArgument->get_expression() */ \n");
#endif
                 // curprint ( "\n /* SgTemplateArgument::nontype_argument */ \n");

                 // DQ (8/7/2013): Adding support for template functions overloaded on template parameters.
                 // This should be present, but we don't use it directly in the name generation. We want 
                 // to use the template arguments in the symbol table lookup, but not the name generation.
                    ASSERT_not_null(templateArgument->get_expression()->get_type());
#if 0
                    printf ("Template argument (templateArgument->get_expression()->get_type() (not used in name generation)) = %p = %s \n",templateArgument->get_expression()->get_type(),templateArgument->get_expression()->get_type()->class_name().c_str());
#endif
                 // unp->u_type->unparseType(templateArgument->get_expression()->get_type(),newInfo);

                 // DQ (1/5/2007): test2007_01.C demonstrated where this expression argument requires qualification.
#if 0
                    printf ("Template argument = %p = %s \n",templateArgument->get_expression(),templateArgument->get_expression()->class_name().c_str());
#endif
                    unp->u_exprStmt->unparseExpression(templateArgument->get_expression(),newInfo);
                  }
                 else
                  {
                 // Unparse this case of a SgInitializedName.
#if 0
                    printf ("In unparseTemplateArgument(): templateArgument->get_initializedName() = %s \n",templateArgument->get_initializedName()->get_name().str());
#endif
                    SgType* type = templateArgument->get_initializedName()->get_type();
                    ASSERT_not_null(type);

                 // DQ (9/10/2014): Note that this will unparse "int T" which we want in the template header, but not in the template parameter or argument list.
                 // unp->u_type->outputType<SgInitializedName>(templateArgument->get_initializedName(),type,newInfo);
                 // SgUnparse_Info ninfo(info);
                 // unp->u_type->unparseType(type,ninfo);
                    curprint(templateArgument->get_initializedName()->get_name());
                  }
               break;
             }

          case SgTemplateArgument::template_template_argument:
             {
               SgDeclarationStatement * decl = templateArgument->get_templateDeclaration();
               ASSERT_not_null(decl);

               SgTemplateDeclaration * tpl_decl = isSgTemplateDeclaration(decl);
               ROSE_ASSERT(tpl_decl == NULL);

               SgTemplateClassDeclaration * tpl_cdel = isSgTemplateClassDeclaration(decl);
               SgTemplateTypedefDeclaration * tpl_typedef = isSgTemplateTypedefDeclaration(decl);
               SgNonrealDecl * nrdecl = isSgNonrealDecl(decl);

               SgType * assoc_type = NULL;
               if (tpl_cdel != NULL) {
                 assoc_type = tpl_cdel->get_type();
               } else if (tpl_typedef != NULL) {
                 assoc_type = tpl_typedef->get_type();
               } else if (nrdecl != NULL) {
                 assoc_type = nrdecl->get_type();
               } else {
                 printf("Error: Unexpected declaration %p (%s) for template template argument %p\n", decl, decl->class_name().c_str(), templateArgument);
                 ROSE_ASSERT(false);
               }
               
#if 0
               printf ("In unparseTemplateArgument(): case SgTemplateArgument::template_template_argument:\n");
               printf ("  -- decl = %p (%s) \n", decl, decl->class_name().c_str());
               if (assoc_type != NULL) {
                 printf ("  -- assoc_type = %p (%s) \n", assoc_type, assoc_type->class_name().c_str());
               } else {
                 printf ("  -- decl->get_template_name() = %s \n", decl->get_template_name().str());
               }
               printf ("  -- templateArgument->get_name_qualification_length() = %d\n", templateArgument->get_name_qualification_length());
#endif

               ASSERT_not_null(assoc_type);

               newInfo.set_SkipClassDefinition();
               newInfo.set_SkipClassSpecifier();
               newInfo.set_SkipEnumDefinition();
               unp->u_type->outputType<SgTemplateArgument>(templateArgument,assoc_type,newInfo);

               break;
             }

           case SgTemplateArgument::start_of_pack_expansion_argument:
             {
               printf ("WARNING: start_of_pack_expansion_argument in Unparse_ExprStmt::unparseTemplateArgument (can happen from some debug output)\n");
               break;
             }

          case SgTemplateArgument::argument_undefined:
             {
               printf ("Error argument_undefined in Unparse_ExprStmt::unparseTemplateArgument \n");
               ROSE_ABORT();
               break;
             }

          default:
             {
               printf ("Error default reached in Unparse_ExprStmt::unparseTemplateArgument \n");
               ROSE_ABORT();
             }
        }

#if DEBUG_TEMPLATE_ARGUMENT
     printf ("Leaving unparseTemplateArgument (%p) \n",templateArgument);
#endif
#if DEBUG_TEMPLATE_ARGUMENT
     curprint("\n/* Bottom of unparseTemplateArgument */ \n");
#endif

#if 0
     printf ("Leaving unparseTemplateArgument(): info.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
     printf ("Leaving unparseTemplateArgument(): info.SkipEnumDefinition()  = %s \n",(info.SkipEnumDefinition() == true) ? "true" : "false");
#endif

#if 0
  // Note that newInfo is what was actually used in the case of outputing a type (above).
     printf ("Leaving unparseTemplateArgument(): newInfo.SkipClassDefinition() = %s \n",(newInfo.SkipClassDefinition() == true) ? "true" : "false");
     printf ("Leaving unparseTemplateArgument(): newInfo.SkipEnumDefinition()  = %s \n",(newInfo.SkipEnumDefinition() == true) ? "true" : "false");
#endif

  // DQ (9/9/2016): These should have been setup to be the same.
     ROSE_ASSERT(info.SkipClassDefinition() == info.SkipEnumDefinition());

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES || 0
     printf ("Leaving unparseTemplateArgument (%p) \n",templateArgument);
     unp->u_exprStmt->curprint ( string("\n/* Bottom of unparseTemplateArgument */ \n"));
#endif
   }


string
unparse_operand_constraint (SgAsmOp::asm_operand_constraint_enum constraint)
   {
  // DQ (7/22/2006): filescope array of char
     static char asm_operand_constraint_letters[(int)SgAsmOp::e_last + 1] = 
   {
  /* aoc_invalid */             '@',
  /* aoc_any */                 'X',
  /* aoc_general */             'g',
  /* aoc_match_0 */             '0',
  /* aoc_match_1 */             '1',
  /* aoc_match_2 */             '2',
  /* aoc_match_3 */             '3',
  /* aoc_match_4 */             '4',
  /* aoc_match_5 */             '5',
  /* aoc_match_6 */             '6',
  /* aoc_match_7 */             '7',
  /* aoc_match_8 */             '8',
  /* aoc_match_9 */             '9',
  /* aoc_reg_integer */         'r',
  /* aoc_reg_float */           'f',
  /* aoc_mem_any */             'm',
  /* aoc_mem_offset */          'o',
  /* aoc_mem_nonoffset */       'V',
  /* aoc_mem_autoinc */         '>',
  /* aoc_mem_autodec */         '<',
  /* aoc_imm_int */             'i',
  // DQ (1/10/2009): The code 'n' is not understood by gnu, so use 'r'
  // aoc_imm_number             'n',
  /* aoc_imm_number */          'r',
  /* aoc_imm_symbol */          's',
  /* aoc_imm_float */           'F',
  /* aoc_reg_a */               'a',
  /* aoc_reg_b */               'b',
  /* aoc_reg_c */               'c',
  /* aoc_reg_d */               'd',
  /* aoc_reg_si */              'S',
  /* aoc_reg_di */              'D',
  /* aoc_reg_legacy */          'R',
  // DQ (8/10/2006): Change case of register name, but I'm unclear if
  // this required for any others (OK for GNU, but required for Intel).
  /* aoc_reg_q */               'q',
  /* aoc_reg_ad */              'A',
  /* aoc_reg_float_tos */       't',
  /* aoc_reg_float_second */    'u',
  /* aoc_reg_sse */             'x',
  /* aoc_reg_sse2 */            'Y',
  /* aoc_reg_mmx */             'y',
  /* aoc_imm_short_shift */     'I',
  /* aoc_imm_long_shift */      'J',
  /* aoc_imm_lea_shift */       'M',
  /* aoc_imm_signed8 */         'K',
  /* aoc_imm_unsigned8 */       'N',
  /* aoc_imm_and_zext */        'L',
  /* aoc_imm_80387 */           'G',
  /* aoc_imm_sse */             'H',
  /* aoc_imm_sext32 */          'e',
  /* aoc_imm_zext32 */          'z',
  /* aoc_last */                '~'
   };

  // string returnString = asm_operand_constraint_letters[constraint];
     char shortString [2];
     shortString[0] = asm_operand_constraint_letters[constraint];
     shortString[1] = '\0';
     string returnString = shortString;

     return returnString;
   }

string
Unparse_ExprStmt::unparse_register_name (SgInitializedName::asm_register_name_enum register_name)
   {
  // DQ (7/22/2006): filescope array of char
     static const char* asm_register_names[(int)SgInitializedName::e_last_register + 1] = 
   {
  /* e_invalid_register */ "invalid",
  /* e_memory_register */  "memory",
  /* register_a */       "ax",
  /* register_b */       "bx",
  /* register_c */       "cx",
  /* register_d */       "dx",
  /* register_si */      "si",
  /* register_di */      "di",
  /* register_bp */      "bp",
  /* register_sp */      "sp",
  /* register_r8 */      "r8",
  /* register_r9 */      "r9",
  /* register_r10 */     "r10",
  /* register_r11 */     "r11",
  /* register_r12 */     "r12",
  /* register_r13 */     "r13",
  /* register_r14 */     "r14",
  /* register_r15 */     "r15",
  /* register_st0 */     "st(0)",
  /* register_st1 */     "st(1)",
  /* register_st2 */     "st(2)",
  /* register_st3 */     "st(3)",
  /* register_st4 */     "st(4)",
  /* register_st5 */     "st(5)",
  /* register_st6 */     "st(6)",
  /* register_st7 */     "st(7)",
  /* register_mm0 */     "mm0",
  /* register_mm1 */     "mm1",
  /* register_mm2 */     "mm2",
  /* register_mm3 */     "mm3",
  /* register_mm4 */     "mm4",
  /* register_mm5 */     "mm5",
  /* register_mm6 */     "mm6",
  /* register_mm7 */     "mm7",
  /* register_f0 */      "xmm0",
  /* register_f1 */      "xmm1",
  /* register_f2 */      "xmm2",
  /* register_f3 */      "xmm3",
  /* register_f4 */      "xmm4",
  /* register_f5 */      "xmm5",
  /* register_f6 */      "xmm6",
  /* register_f7 */      "xmm7",
  /* register_f8 */      "xmm8",
  /* register_f9 */      "xmm9",
  /* register_f10 */     "xmm10",
  /* register_f11 */     "xmm11",
  /* register_f12 */     "xmm12",
  /* register_f13 */     "xmm13",
  /* register_f14 */     "xmm14",
  /* register_f15 */     "xmm15",
  /* register_flags */   "flags",
  /* register_fpsr */    "fpsr",
  /* register_dirflag */ "dirflag",
  /* e_unrecognized_register */ "unrecognized",
  /* e_last_register */    "last"
   };

     string returnString = asm_register_names[register_name];

#if (__x86_64 == 1 || __x86_64__ == 1 || __x86_32 == 1 || __x86_32__ == 1)
  // DQ (12/12/2012): Fixup the name of the register so that we are refering to EAX instead of AX (on both 32 bit and 64 bit systems).
     if (register_name >= SgInitializedName::e_register_a && register_name <= SgInitializedName::e_register_sp)
        {
       // Use the extended register name.
          returnString = "e" + returnString;
        }
#endif

  // DQ (12/12/2012): If this is an unrecognized register at least specify a simple register name.
     if (register_name == SgInitializedName::e_unrecognized_register)
        {
          printf ("Error: register names not recognized on non-x86 architectures (putting out reference name: 'ax' \n");
          returnString = "ax";
        }

     return returnString;
   }

void 
Unparse_ExprStmt::unparse_asm_operand_modifier(SgAsmOp::asm_operand_modifier_enum flags)
   {
  // Modifiers to asm operand strings.  These are all machine independent.
  // Many of them do not make sense in asm() but are included anyway for
  // completeness.  Note that these are bitmasks, and that 
  // aom_input + aom_output == aom_modify.

  // e_invalid           = 0x00,  error
  // e_input             = 0x01,  no mod: input operand
  // e_output            = 0x02,  =: output operand
  // e_modify            = 0x03,  +: read-mod-write operand
  // e_earlyclobber      = 0x04,  &: modified early, cannot overlap inputs
  // e_commutative       = 0x08,  %: commutative with next operand
  // e_ignore_next       = 0x10,  *: ignore next letter as a register pref
  // e_ignore_till_comma = 0x20,  #: ignore up to comma as a register pref
  // e_poor_choice       = 0x40,  ?: avoid choosing this
  // e_bad_choice        = 0x80   !: really avoid choosing this

     if (flags & SgAsmOp::e_invalid)           curprint ( "error");

  // DQ (7/23/2006): The coding of these is a bit more complex, get it? :-)
  // if (flags & SgAsmOp::e_input)             curprint ( "";
  // if (flags & SgAsmOp::e_output)            curprint ( "=";
  // if (flags & SgAsmOp::e_modify)            curprint ( "+";

     if ( ((flags & SgAsmOp::e_input) != 0) && ((flags & SgAsmOp::e_output) != 0) )
        {
       // This is how "modify" is coded!
#if PRINT_DEVELOPER_WARNINGS
          printf ("This is how modify is coded \n");
#endif
       // DQ (8/10/2006): Intel compiler can not handle the output of modifier for input operand:
       //      error: an asm input operand may not have the '=' or '+' modifiers
       // curprint ( "+";
       // if ( string(BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH) != string("icpc") && (flags & SgAsmOp::e_output) )
       // if ( string(BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH) != string("icpc") )
               curprint ( "+");
        }
       else
        {
       // Only one of these are true
          if (flags & SgAsmOp::e_output) curprint ( "=");

       // We need an exact match not a partial match!

#if PRINT_DEVELOPER_WARNINGS
          printf ("We need an exact match not a partial match \n");
#endif
       // DQ (8/10/2006): Intel compiler can not handle the output of modifier for input operand:
       //      error: an asm input operand may not have the '=' or '+' modifiers
       // if ( string(BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH) != string("icpc") )
             {
               if ((flags & SgAsmOp::e_modify) == SgAsmOp::e_modify) curprint ( "+");
             }
        }

      if (flags & SgAsmOp::e_earlyclobber)      curprint ( "&");
      if (flags & SgAsmOp::e_commutative)       curprint ( "%");
      if (flags & SgAsmOp::e_ignore_next)       curprint ( "*");
      if (flags & SgAsmOp::e_ignore_till_comma) curprint ( "#");
      if (flags & SgAsmOp::e_poor_choice)       curprint ( "?");
      if (flags & SgAsmOp::e_bad_choice)        curprint ( "!");
   }

void
Unparse_ExprStmt::unparseAsmOp (SgExpression* expr, SgUnparse_Info& info)
   {
  // Just call unparse on the statement.
     SgAsmOp* asmOp = isSgAsmOp(expr);
     ASSERT_not_null(asmOp);

  // printf ("In unparseAsmOp(): asmOp->get_recordRawAsmOperandDescriptions() = %s \n",asmOp->get_recordRawAsmOperandDescriptions() ? "true" : "false");

     SgExpression* expression = asmOp->get_expression();
     ASSERT_not_null(expression);

     if (asmOp->get_name().empty() == false)
        {
       // This is symbolic name indicated for this operand (using the "[ <identifier> ]" syntax, if present).
          curprint ("[" + asmOp->get_name() + "] ");
        }

     curprint ( "\"");
     if (asmOp->get_recordRawAsmOperandDescriptions() == false)
        {
       // This is only set to non-invalid state when RECORD_RAW_ASM_OPERAND_DESCRIPTIONS == FALSE in EDG.
          unparse_asm_operand_modifier(asmOp->get_modifiers());
          curprint ( unparse_operand_constraint(asmOp->get_constraint()));
        }
       else
        {
       // The modifier is part of the constraint, and it is output in the constraintString when recordRawAsmOperandDescriptions() == true.
#if 0
          printf ("asmOp->get_constraintString() = %s \n",asmOp->get_constraintString().c_str());
#endif
          curprint ( asmOp->get_constraintString() );
        }

#if 0
  // DQ (1/8/2009): Added support for case of asm operand handling with EDG RECORD_RAW_ASM_OPERAND_DESCRIPTIONS == TRUE
  // (this case uses the constraintString instead of a constrant code)
  // curprint ( unparse_operand_constraint(asmOp->get_constraint()));
     if (asmOp->get_recordRawAsmOperandDescriptions() == false)
        {
          curprint ( unparse_operand_constraint(asmOp->get_constraint()));
        }
       else
        {
          curprint ( asmOp->get_constraintString() );
        }
#endif

  // This is usually a SgVarRefExp
     curprint ( "\"");
     curprint ( " (");
     unparseExpression(expression,info);
     curprint ( ")");
   }

void
Unparse_ExprStmt::unparseStatementExpression (SgExpression* expr, SgUnparse_Info& info)
   {
  // Just call unparse on the statement.
     SgStatementExpression* statementExpression = isSgStatementExpression(expr);
     ASSERT_not_null(statementExpression);
     SgStatement* statement = statementExpression->get_statement();
     ASSERT_not_null(statement);

  // DQ (10/7/2006): Even if we are in a conditional the statements appearing in the 
  // statement expression must have ";" output (here we have to turn off the flags 
  // to both SkipSemiColon and inConditional).  See test2006_148.C for an example.
     SgUnparse_Info info2(info);
     info2.unset_SkipSemiColon();
     info2.unset_inConditional();

  // Expressions are another place where a class definition should NEVER be unparsed
  // DQ (5/23/2007): Note that statement expressions can have class definition 
  // (so they are exceptions, see test2007_51.C).
     info2.unset_SkipClassDefinition();

  // DQ (1/9/2014): We have to make the handling of enum definitions consistant with that of class definitions.
     info2.unset_SkipEnumDefinition();

     curprint ( "(");
     unparseStatement(statement,info2);
     curprint ( ")");
   }

void
Unparse_ExprStmt::unparseUnaryOperator(SgExpression* expr, const char* op, SgUnparse_Info & info)
   {
  //
  // Flag to keep to original state of the "this" option
  //
     bool orig_this_opt = unp->opt.get_this_opt();
     SgUnparse_Info newinfo(info);
     newinfo.set_operator_name(op);
  //
  // If the "this" option was originally false, then we shouldn't print "this."
  // however, this only applies when the "this" is part of a binary expression.
  // In the unary case, we must print "this," otherwise a syntax error will be
  // produced. (i.e. *this)
  //
     if ( !orig_this_opt )
          unp->opt.set_this_opt(true);

#if 0
     curprint ("\n /* Inside of unparseUnaryOperator(" + expr->class_name() + "," + op + ",SgUnparse_Info) */ \n");
#endif

     unparseUnaryExpr(expr, newinfo);

  //
  // Now set the "this" option back to its original state
  //
     if( !orig_this_opt )
          unp->opt.set_this_opt(false);
   }

void
Unparse_ExprStmt::unparseBinaryOperator(SgExpression* expr, const char* op, SgUnparse_Info& info)
   {
     SgUnparse_Info newinfo(info);
     newinfo.set_operator_name(op);

#if 0
     printf ("In unparseBinaryOperator(): expr = %p op = %s \n",expr,op);
     curprint ( string("\n /* Inside of unparseBinaryOperator(expr = ") +  StringUtility::numberToString(expr) + 
                " = " + expr->sage_class_name() + "," + op + ",SgUnparse_Info) */ \n");
#endif

#if 0
     printf ("In unparseBinaryOperator(): info.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
     printf ("In unparseBinaryOperator(): info.SkipEnumDefinition()  = %s \n",(info.SkipEnumDefinition() == true) ? "true" : "false");
#endif

  // DQ (1/9/2014): These should have been setup to be the same.
     ROSE_ASSERT(info.SkipClassDefinition() == info.SkipEnumDefinition());

#if 0
     printf ("In unparseBinaryOperator(): info.skipCompilerGeneratedSubExpressions()  = %s \n",(info.skipCompilerGeneratedSubExpressions() == true) ? "true" : "false");
     curprint ( string("\n /* Inside of unparseBinaryOperator(expr = ") +  StringUtility::numberToString(expr) + 
              " info.skipCompilerGeneratedSubExpressions() = " + (info.skipCompilerGeneratedSubExpressions() ? "true" : "false") + " */ \n");
#endif

     if (info.skipCompilerGeneratedSubExpressions() == true)
        {
       // Only unparse the rhs operand if it is compiler generated.
          SgBinaryOp* binaryOp = isSgBinaryOp(expr);
          ASSERT_not_null(binaryOp);

          SgExpression* lhs = binaryOp->get_lhs_operand();
          ASSERT_not_null(lhs);
          SgExpression* rhs = binaryOp->get_rhs_operand();
          ASSERT_not_null(rhs);
#if 0
          printf ("In unparseBinaryOperator(): info.skipCompilerGeneratedSubExpressions() == true: only unparsing the rhs operand \n");
#endif
          if (lhs->isCompilerGenerated() == true)
             {
            // Then only unparse the rhs.
#if 0
               curprint( string("\n /* Inside of unparseBinaryOperator(expr = ") +  StringUtility::numberToString(expr) + " = " + expr->sage_class_name() + "," + op + ",SgUnparse_Info) : COMPILER GENERATED: calling unparseExpression(rhs, newinfo) (only unparse the rhs) */ \n");
#endif
               unparseExpression(rhs, newinfo);
             }
            else
             {
#if 0
               curprint( string("\n /* Inside of unparseBinaryOperator(expr = ") +  StringUtility::numberToString(expr) + " = " + expr->sage_class_name() + "," + op + ",SgUnparse_Info) : NOT COMPILER GENERATED: calling unparseBinaryExpr() */ \n");
#endif
               unparseBinaryExpr(expr, newinfo);
#if 0
               curprint( string("\n /* Inside of unparseBinaryOperator(expr = ") +  StringUtility::numberToString(expr) + " = " + expr->sage_class_name() + "," + op + ",SgUnparse_Info) : DONE: NOT COMPILER GENERATED: unparseBinaryExpr() */ \n");
#endif
             }
        }
       else
        {
#if 0
          curprint( string("\n /* Inside of unparseBinaryOperator(expr = ") +  StringUtility::numberToString(expr) + " = " + expr->sage_class_name() + "," + op + ",SgUnparse_Info) : calling unparseBinaryExpr() */ \n");
#endif
          unparseBinaryExpr(expr, newinfo);
#if 0
          curprint( string("\n /* Inside of unparseBinaryOperator(expr = ") +  StringUtility::numberToString(expr) + " = " + expr->sage_class_name() + "," + op + ",SgUnparse_Info) : DONE: unparseBinaryExpr() */ \n");
#endif
        }

#if 0
     printf ("Leaving unparseBinaryOperator(): expr = %p op = %s \n",expr,op);
     curprint ( string("\n /* Leaving unparseBinaryOperator(expr = ") +  StringUtility::numberToString(expr) + " = " + expr->sage_class_name() + "," + op + ",SgUnparse_Info) */ \n");
#endif
   }


void
Unparse_ExprStmt::unparseAssnExpr(SgExpression* expr, SgUnparse_Info& info) {}


void
Unparse_ExprStmt::unparseVarRef(SgExpression* expr, SgUnparse_Info& info)
   {
     SgVarRefExp* var_ref = isSgVarRefExp(expr);
     ASSERT_not_null(var_ref);

#if 0
     printf ("In Unparse_ExprStmt::unparseVarRef() \n");
     curprint(" /* In Unparse_ExprStmt::unparseVarRef() */ \n ");
#endif

#if 0
     var_ref->get_startOfConstruct()->display("In Unparse_ExprStmt::unparseVarRef()");
#endif

  // todo: when get_parent() works for this class we can
  // get back to the lhs of the SgArrowExp or SgDotExp that
  // may be a parent of this expression.  This will let
  // us avoid outputting the class qualifier when its not needed.

  // For now we always output the class qualifier.

     if (var_ref->get_symbol() == NULL)
        {
          printf ("Error in unparseVarRef() at line %d column %d \n",var_ref->get_file_info()->get_line(),var_ref->get_file_info()->get_col());
        }
     ASSERT_not_null(var_ref->get_symbol());

     SgInitializedName* theName = var_ref->get_symbol()->get_declaration();
     ASSERT_not_null(theName);

#if 0
     printf ("In Unparse_ExprStmt::unparseVarRef(): SgInitializedName* theName = %p = %s \n",theName,theName->get_name().str());
     printf ("In Unparse_ExprStmt::unparseVarRef(): SgInitializedName scope = %p = %s \n",theName->get_scope(),theName->get_scope()->class_name().c_str());
     printf ("In Unparse_ExprStmt::unparseVarRef(): SgInitializedName scope = %p qualified name = %s \n",theName->get_scope(),theName->get_scope()->get_qualified_name().str());
#endif

#if 0
  // DQ (2/8/2010): Debugging code.
     var_ref->get_startOfConstruct()->display("Inside of unparseVarRef");
#endif

  // DQ (1/7/2007): Much simpler version of code!
  // SgScopeStatement* declarationScope = theName->get_scope();
  // ASSERT_not_null(declarationScope);
  // SgUnparse_Info ninfo(info);

  // DQ (2/10/2010): Don't search for the name "__assert_fail", this is part of macro expansion of the assert macro and will not be found.
  // SgName nameQualifier = unp->u_name->generateNameQualifier(theName,info);
     SgName nameQualifier;
     if (theName->get_name() != "__assert_fail")
        {
       // nameQualifier = unp->u_name->generateNameQualifier(theName,info);

       // DQ (5/30/2011): Newest refactored support for name qualification.
          nameQualifier = var_ref->get_qualified_name_prefix();

#if 0
          printf ("In Unparse_ExprStmt::unparseVarRef(): nameQualifier = %s \n",nameQualifier.str());
          curprint(" /* In Unparse_ExprStmt::unparseVarRef() */ \n ");
#endif
        }

  // DQ (1/22/2014): Adding support for generated names used in un-named variables.
     bool isAnonymousName = (string(var_ref->get_symbol()->get_name()).substr(0,14) == "__anonymous_0x");
#if 0
     printf ("In unparseVarRef(): isAnonymousName = %s \n",isAnonymousName ? "true" : "false");
#endif

#if 0
     printf ("In Unparse_ExprStmt::unparseVarRef(): output nameQualifier = %s \n",nameQualifier.str());
     curprint(" /* In Unparse_ExprStmt::unparseVarRef(): output nameQualifier */ \n ");
#endif

#if 0
  // DQ (7/31/2012): I don't think we use the name "__unnamed_class" any more (so this maybe be always true).
  // DQ (11/9/2007): Need to ignore these sorts of generated names
     if (nameQualifier.getString().find("__unnamed_class") == string::npos)
        {
#if 0
          printf ("In Unparse_ExprStmt::unparseVarRef(): nameQualifier = %s \n",nameQualifier.str());
#endif
          curprint(nameQualifier.str());
        }
#else
  // DQ (8/19/2014): This causes output such as: "XXX::isValidDomainSize(domain_extents . Extents_s::imin);"
  // with the function parameter's SgVarRefExp qualified un-nessesarily (see test2014_116.C).
     curprint(nameQualifier.str());
#endif

#if 0
     printf ("In Unparse_ExprStmt::unparseVarRef(): DONE output nameQualifier = %s \n",nameQualifier.str());
     curprint(" /* In Unparse_ExprStmt::unparseVarRef(): DONE output nameQualifier */ \n ");
#endif

  // DQ (2/10/2010): This is a strange problem demonstrated only by test2010_07.C.
  // curprint (  var_ref->get_symbol()->get_name().str());
     if (theName->get_name() == "__assert_fail")
        {
       // DQ (2/10/2010): For some reason, "__PRETTY_FUNCTION__" is replaced with "__assert_fail" by EDG?
       // But only when the assert comes from a struct (see test2010_07.C).
       // printf ("Warning: work around substitution of __PRETTY_FUNCTION__ for __assert_fail \n");
          curprint ("__PRETTY_FUNCTION__");
        }
       else
        {
       // curprint (var_ref->get_symbol()->get_name().str());
          if (isAnonymousName == false)
             {
               curprint (var_ref->get_symbol()->get_name().str());
             }
        }

#if 0
     printf ("Leaving Unparse_ExprStmt::unparseVarRef() \n");
     curprint(" /* Leaving Unparse_ExprStmt::unparseVarRef() */ \n ");
#endif
   }


// DQ (9/4/2013): Added support for compound literals.
void
Unparse_ExprStmt::unparseCompoundLiteral (SgExpression* expr, SgUnparse_Info& info)
   {
#if 0
     printf ("In unparseCompoundLiteral() \n");
     curprint ("/* In unparseCompoundLiteral() */ \n");
#endif

     SgCompoundLiteralExp* compoundLiteral = isSgCompoundLiteralExp(expr);
     ASSERT_not_null(compoundLiteral);

     SgVariableSymbol* variableSymbol = compoundLiteral->get_symbol();
     ASSERT_not_null(variableSymbol);

     SgInitializedName* initializedName = variableSymbol->get_declaration();
     ASSERT_not_null(initializedName);

     if (initializedName->get_initptr() == NULL)
        {
          printf ("Error: In unparseCompoundLiteral(): initializedName->get_initptr() == NULL: initializedName = %p name = %s \n",initializedName,initializedName->get_name().str());
        }

     ASSERT_not_null(initializedName->get_initptr());

     SgAggregateInitializer* aggregateInitializer = isSgAggregateInitializer(initializedName->get_initptr());
     ASSERT_not_null(aggregateInitializer);
     ROSE_ASSERT(aggregateInitializer->get_uses_compound_literal() == true);

#if 0
     printf ("Calling unparseAggrInit() \n");
     curprint ("/* Calling unparseAggrInit() */ \n");
#endif

     unparseAggrInit(aggregateInitializer,info);

#if 0
     printf ("DONE: Calling unparseAggrInit() \n");
     curprint ("/* DONE: Calling unparseAggrInit() */ \n");
#endif

#if 0
     printf ("unparseCompoundLiteral not implemented yet! \n");
#endif
#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }


void
Unparse_ExprStmt::unparseClassRef(SgExpression* expr, SgUnparse_Info& info)
   {
     SgClassNameRefExp* classname_ref = isSgClassNameRefExp(expr);
     ASSERT_not_null(classname_ref);

     curprint (  classname_ref->get_symbol()->get_declaration()->get_name().str());
   }


void
Unparse_ExprStmt::unparseFuncRef(SgExpression* expr, SgUnparse_Info& info)
   {
     SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
     ASSERT_not_null(func_ref);

  // Calling the template function unparseFuncRef<SgFunctionRefExp>(func_ref);
     unparseFuncRefSupport<SgFunctionRefExp>(expr,info);
   }


template <class T>
void
Unparse_ExprStmt::unparseFuncRefSupport(SgExpression* expr, SgUnparse_Info& info)
   {
  // DQ (4/25/2012): since these IR nodes have the same API, we can use a templated function to avoid the dublication of code.

#define DEBUG_FUNCTION_REFERENCE_SUPPORT 0

  // SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
     T* func_ref = dynamic_cast<T*>(expr);
     ASSERT_not_null(func_ref);

  // DQ (4/14/2013): Added support for unparsing "operator+(x,y)" in place of "x+y".  This is 
  // required in places even though we have historically defaulted to the generation of the 
  // operator syntax (e.g. "x+y"), see test2013_100.C for an example of where this is required.
     ASSERT_not_null(func_ref->get_parent());
  // SgNode* possibleFunctionCall = func_ref->get_parent()->get_parent();
     SgNode* possibleFunctionCall = func_ref->get_parent();
     ASSERT_not_null(possibleFunctionCall);
     SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(possibleFunctionCall);

  // This fails for test2005_112.C.
  // ASSERT_not_null(functionCallExp);

     bool uses_operator_syntax = false;
     if (functionCallExp != NULL)
        {
          uses_operator_syntax = functionCallExp->get_uses_operator_syntax();
#if 0
       // DQ (8/28/2014): It is a bug in GNU 4.4.7 to use the non-operator syntax of a user-defined conversion operator.
       // So we have to detect such operators and then detect if they are implicit then mark them to use the operator 
       // syntax plus supress them from being output.  We might alternatively go directly to supressing them from being
       // output, except that this is more complex for the non-operator syntax unparsing (I think).

          SgFunctionSymbol* functionSymbol = func_ref->get_symbol();
          ASSERT_not_null(functionSymbol);
          SgFunctionDeclaration* functionDeclaration = functionSymbol->get_declaration();
          ASSERT_not_null(functionDeclaration);
#endif
#if 0
          printf ("Exiting as a tesxt! \n");
          ROSE_ASSERT(false);
#endif
        }

#if DEBUG_FUNCTION_REFERENCE_SUPPORT
     printf ("In unparseFuncRefSupport(): uses_operator_syntax = %s \n",uses_operator_syntax ? "true" : "false");
     curprint (string("\n /* Inside of unparseFuncRefSupport: uses_operator_syntax = ") + (uses_operator_syntax ? "true" : "false") + " */ \n");
#endif

  // If we have previously computed a name for this function (because it was a templated function 
  // with template arguments that required name qualification) then output the name directly.

  // DQ (6/21/2011): This controls if we output the generated name of the type (required to 
  // support name qualification of subtypes) or if we unparse the type from the AST (where 
  // name qualification of subtypes is not required).
     bool usingGeneratedNameQualifiedFunctionNameString = false;
     string functionNameString;

  // DQ (6/4/2011): Support for output of generated string for type (used where name 
  // qualification is required for subtypes (e.g. template arguments)).
     SgNode* nodeReferenceToFunction = info.get_reference_node_for_qualification();

#if DEBUG_FUNCTION_REFERENCE_SUPPORT
     printf ("In unparseFuncRefSupport(): nodeReferenceToFunction = %p \n",nodeReferenceToFunction);
#endif

  // DQ (8/24/2014): test2014_156.C demonstrates where we need to sometime distinquish between when a 
     if (functionCallExp == NULL)
        {
#if DEBUG_FUNCTION_REFERENCE_SUPPORT
          printf ("This SgFunctionRefExp is not a part of a SgFunctionCallExp, so just using the associated function name. \n");
#endif
       // reset the nodeReferenceToFunction to avoid the wrong logic from being used.
          nodeReferenceToFunction = NULL;
        }

     if (nodeReferenceToFunction != NULL)
        {
       // See test2005_02.C for an example of where this logic is required fro constructors.
#if DEBUG_FUNCTION_REFERENCE_SUPPORT
          printf ("rrrrrrrrrrrr In unparseFuncRefSupport() output type generated name: nodeReferenceToFunction = %p = %s SgNode::get_globalTypeNameMap().size() = %" PRIuPTR " \n",
               nodeReferenceToFunction,nodeReferenceToFunction->class_name().c_str(),SgNode::get_globalTypeNameMap().size());
#endif
          std::map<SgNode*,std::string>::iterator i = SgNode::get_globalTypeNameMap().find(nodeReferenceToFunction);
          if (i != SgNode::get_globalTypeNameMap().end())
             {
               usingGeneratedNameQualifiedFunctionNameString = true;

               functionNameString = i->second.c_str();
#if DEBUG_FUNCTION_REFERENCE_SUPPORT
               printf ("ssssssssssssssss Found type name in SgNode::get_globalTypeNameMap() typeNameString = %s for nodeReferenceToType = %p = %s \n",functionNameString.c_str(),nodeReferenceToFunction,nodeReferenceToFunction->class_name().c_str());
#endif
#if 0
            // DQ (8/24/2014): reset the string to generate an error so that I can better understand where this name qualification feature is required.
               functionNameString = "TEST_TEST_TEST";
#endif
             }
            else
             {
#if DEBUG_FUNCTION_REFERENCE_SUPPORT
               printf ("Could not find saved name qualified function name in globalTypeNameMap: nodeReferenceToFunction = %p \n",nodeReferenceToFunction);
#endif
             }
        }
       else
        {
       // DQ (6/23/2011): Make this a warning since the tests/nonsmoke/functional/CompileTests/OpenMP_tests/alignment.c fails in the tests/nonsmoke/functional/roseTests/ompLoweringTests directory.
       // This also happens for the tests/nonsmoke/functional/roseTests/programAnalysisTests/testPtr1.C when run by the tests/nonsmoke/functional/roseTests/programAnalysisTests/PtrAnalTest tool.

       // printf ("ERROR: In unparseType(): nodeReferenceToFunction = NULL \n");
       // printf ("WARNING: In unparseType(): nodeReferenceToFunction = NULL \n");
       // ROSE_ASSERT(false);
        }

#if DEBUG_FUNCTION_REFERENCE_SUPPORT || 0
     printf ("In unparseFuncRef(): usingGeneratedNameQualifiedFunctionNameString = %s \n",usingGeneratedNameQualifiedFunctionNameString ? "true" : "false");
#endif

     if (usingGeneratedNameQualifiedFunctionNameString == true)
        {
       // Output the previously generated type name contianing the correct name qualification of subtypes (e.g. template arguments).
       // curprint ("/* output the function in unparseFuncRef() */");

          curprint(functionNameString);
       // curprint ("/* DONE: output the function in unparseFuncRef() */");
        }
       else
        {
       // This is the code that was always used before the addition of type names generated from where name qualification of subtypes are required.

       // Start of old code (not yet intended properly).

  // DQ: This acceses the string pointed to by the pointer in the SgName object 
  // directly ans is thus UNSAFE! A copy of the string should be made.
  // char* func_name = func_ref->get_symbol()->get_name();
  // char* func_name = strdup (func_ref->get_symbol()->get_name().str());
     ASSERT_not_null(func_ref->get_symbol());
     string func_name = func_ref->get_symbol()->get_name().str();
     int diff = 0; // the length difference between "operator" and function

#if DEBUG_FUNCTION_REFERENCE_SUPPORT || 0
     printf ("Inside of Unparse_ExprStmt::unparseFuncRef(): func_name = %s \n",func_name.c_str());
#endif

     ASSERT_not_null(func_ref->get_symbol());
     ASSERT_not_null(func_ref->get_symbol()->get_declaration());

     SgDeclarationStatement* declaration = func_ref->get_symbol()->get_declaration();

#if DEBUG_FUNCTION_REFERENCE_SUPPORT
  // DQ (7/26/2012): Test the function name (debuging test2009_31.C: "operator<<" output as "operator")
     printf ("declaration = %p = %s \n",declaration,declaration->class_name().c_str());
     SgTemplateInstantiationFunctionDecl* templateInstantiationFunctionDecl = isSgTemplateInstantiationFunctionDecl(declaration);
     if (templateInstantiationFunctionDecl != NULL)
        {
          printf ("templateInstantiationFunctionDecl->get_name() = %p = %s \n",templateInstantiationFunctionDecl,templateInstantiationFunctionDecl->get_name().str());
        }
       else
        {
          SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDecl = isSgTemplateInstantiationMemberFunctionDecl(declaration);
          if (templateInstantiationMemberFunctionDecl != NULL)
             {
               printf ("templateInstantiationMemberFunctionDecl->get_name() = %p = %s \n",templateInstantiationMemberFunctionDecl,templateInstantiationMemberFunctionDecl->get_name().str());
             }
            else
             {
               printf ("This is not a template function instantation (member nor non-member function) \n");
             }
        }
#endif

  // DQ (2/12/2019): Adding support for C++11 user-defined literal operators.
     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(declaration);
     ASSERT_not_null(functionDeclaration);

     bool is_literal_operator = false;
     if (functionDeclaration->get_specialFunctionModifier().isUldOperator() == true)
        {
#if 0
          printf ("Detected a literal operator! \n");
#endif
          is_literal_operator = true;
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

  // check that this an operator overloading function
  // if (!unp->opt.get_overload_opt() && !strncmp(func_name.c_str(), "operator", 8))
     if (!unp->opt.get_overload_opt() && !strncmp(func_name.c_str(), "operator", 8))
        {
       // set the length difference between "operator" and function
       // diff = strlen(func_name.c_str()) - strlen("operator");
          diff = (uses_operator_syntax == true) ? strlen(func_name.c_str()) - strlen("operator") : 0;
#if 0
          printf ("Found an operator: func_name = %s \n",func_name.c_str());
#endif
       // DQ (1/6/2006): trap out cases of global new and delete functions called 
       // using ("::operator new" or "::operator delete" syntax).  In these cases 
       // the function are treated as normal function calls and not classified in 
       // the AST as SgNewExp and SgDeleteExp.  See test2006_04.C.
          bool isNewOperator    =  (strncmp(func_name.c_str(), "operator new", 12) == 0)    ? true : false;
          bool isDeleteOperator =  (strncmp(func_name.c_str(), "operator delete", 15) == 0) ? true : false;

#if DEBUG_FUNCTION_REFERENCE_SUPPORT
          printf ("isNewOperator    = %s \n",isNewOperator    ? "true" : "false");
          printf ("isDeleteOperator = %s \n",isDeleteOperator ? "true" : "false");
#endif
       // DQ (1/6/2006): Only do this if not the "operator new" or "operator delete" functions.
       // now we check if the difference is larger than 0. If it is, that means that
       // there is something following "operator". Then we can get the substring after
       // "operator." If the diff is not larger than 0, then don't get the substring.
          if ( (isNewOperator == false) && (isDeleteOperator == false) && (diff > 0) )
             {
            // get the substring after "operator." If you are confused with how strchr 
            // works, look up the man page for it.
            // func_name = strchr(func_name.c_str(), func_name[8]);
               if (uses_operator_syntax == true)
                  {
#if 0
                    printf ("In unparseFuncRef(): before modification: func_name = |%s| \n",func_name.c_str());
#endif
                    func_name = strchr(func_name.c_str(), func_name[8]);
#if 0
                    printf ("In unparseFuncRef(): using operator syntax: func_name = |%s| \n",func_name.c_str());
#endif
                    if (is_literal_operator == true)
                       {
                      // func_name = strchr(func_name.c_str(), func_name[8]);
                      // func_name = strchr(func_name.c_str(), "\"\"");
                         func_name = strchr(func_name.c_str(), func_name[4]);
#if 0
                         printf ("In unparseFuncRef(): using operator syntax for literal operator: func_name = |%s| \n",func_name.c_str());
#endif

#if 0
                         printf ("Exiting as a test! \n");
                         ROSE_ASSERT(false);
#endif
                       }

#if DEBUG_FUNCTION_REFERENCE_SUPPORT
                    printf ("In unparseFuncRef(): using operator syntax: func_name = %s \n",func_name.c_str());
#endif
                  }
                 else
                  {
#if DEBUG_FUNCTION_REFERENCE_SUPPORT
                    printf ("In unparseFuncRef(): using full operator name: func_name = %s \n",func_name.c_str());
#endif
                  }
             }
        }

  // if func_name is not "()", print it. Otherwise, we don't print it because we want
  // to print out, for example, A(0) = 5, not A()(0) = 5.

#if DEBUG_FUNCTION_REFERENCE_SUPPORT
     printf ("func_name = %s uses_operator_syntax = %s \n",func_name.c_str(),uses_operator_syntax ? "true" : "false");
     printf ("   --- strcmp(func_name.c_str(), \"()\") = %s \n",strcmp(func_name.c_str(), "()") ? "true" : "false");
#endif

  // DQ (4/14/2013): Modified to handle conditional use of uses_operator_syntax.
  // if (strcmp(func_name.c_str(), "()"))
     if ( ( strcmp(func_name.c_str(), "()") && (uses_operator_syntax == true) ) || ( strcmp(func_name.c_str(), "operator()") && (uses_operator_syntax == false) ) )
        {
       // DQ (10/21/2006): Only do name qualification of function names for C++
          if (SageInterface::is_Cxx_language() == true)
             {
#if DEBUG_FUNCTION_REFERENCE_SUPPORT
               printf ("declaration->get_declarationModifier().isFriend() = %s \n",declaration->get_declarationModifier().isFriend() ? "true" : "false");
#endif
            // DQ (12/2/2004): Added diff == 0 to avoid qualification of operators (avoids "i__gnu_cxx::!=0") 
            // added some extra spaces to make it more clear if it is ever wrong again (i.e. "i __gnu_cxx:: != 0")
            // DQ (11/13/2004) Modified to avoid qualified name for friend functions
            // DQ (11/12/2004) Added support for qualification of function names output as function calls
            // if ( (declaration->get_declarationModifier().isFriend() == false) && (diff == 0) )
               bool useNameQualification = ( (declaration->get_declarationModifier().isFriend() == false) && (diff == 0) );

            // DQ (4/1/2014): Force name qualification where it was computed to be required (see test2014_28.C).
            // Even friends can need name qualification.  However, this causes other test codes to fail.
               useNameQualification = true;
               useNameQualification = useNameQualification && (uses_operator_syntax == false);
          
               if ( useNameQualification == true )
                  {
                 // DQ (8/6/2007): Now that we have a more sophisticated name qualifiation mechanism using 
                 // hidden declaration lists, we don't have to force the qualification of function names.
                 // DQ (10/15/2006): Force output of any qualified names for function calls.
                 // info.set_forceQualifiedNames();

                 // curprint ( "/* unparseFuncRef calling info.set_forceQualifiedNames() */ ";

                 // DQ (5/12/2011): Support for new name qualification.
                    SgUnparse_Info tmp_info(info);
                    tmp_info.set_name_qualification_length(func_ref->get_name_qualification_length());
                    tmp_info.set_global_qualification_required(func_ref->get_global_qualification_required());

                 // SgName nameQualifier = unp->u_name->generateNameQualifier( declaration, info );
                 // SgName nameQualifier = unp->u_name->generateNameQualifier( declaration, tmp_info );

                 // DQ (5/29/2011): Newest refactored support for name qualification.
                 // printf ("In unparseFuncRef(): Looking for name qualification for SgFunctionRefExp = %p \n",func_ref);
                    SgName nameQualifier = func_ref->get_qualified_name_prefix();
#if DEBUG_FUNCTION_REFERENCE_SUPPORT
                    printf ("In unparseFuncRef(): nameQualifier = %s \n",nameQualifier.str());
                    printf ("SgNode::get_globalQualifiedNameMapForNames().size() = %" PRIuPTR " \n",SgNode::get_globalQualifiedNameMapForNames().size());
                    printf ("In unparseFuncRef(): Testing name in map: for SgFunctionRefExp = %p qualified name = %s \n",func_ref,func_ref->get_qualified_name_prefix().str());
                 // curprint ( "\n /* unparseFuncRef using nameQualifier = " + nameQualifier.str() + " */ \n";
#endif
#if 0
                    SgFunctionCallExp* functionCallExpression = isSgFunctionCallExp(expr->get_parent());
                    if (functionCallExpression != NULL)
                       {
                         printf ("Found the function call, global qualification is defined here functionCallExpression->get_global_qualified_name() = %s \n",
                              functionCallExpression->get_global_qualified_name() == true ? "true" : "false");
                         if (functionCallExpression->get_global_qualified_name() == true)
                            {
                              curprint ("::");
                            }
                       }
#endif
                    curprint (nameQualifier.str());
                 // curprint (nameQualifier.str() + " ";
                  }
                 else
                  {
#if DEBUG_FUNCTION_REFERENCE_SUPPORT
                    printf ("In unparseFuncRef(): No name qualification permitted in this case! \n");
#endif
                  }
             }

       // curprint ("\n /* unparseFuncRef func_name = " + func_name + " */ \n");
       // DQ (6/21/2011): Support for new name qualification (output of generated function name).
          ASSERT_not_null(declaration);
       // printf ("Inside of Unparse_ExprStmt::unparseFuncRef(): declaration = %p = %s \n",declaration,declaration->class_name().c_str());
#if 0
       // DQ (4/15/2013): If there is other debug output turned on then nesting of comments inside of comments can occur in this output (see test2007_17.C).
          curprint (string("\n /* In unparseFuncRef(): put out func_name = ") + func_name + " */ \n ");
#endif
       // If this is a template then the name will include template arguments which require name qualification and the name 
       // qualification will depend on where the name is referenced in the code.  So we have generate the non-canonical name 
       // with all possible qualifications and save it to be reused by the unparser when it unparses the tempated function name.
          SgTemplateInstantiationFunctionDecl* templateInstantiationFunctionDecl = isSgTemplateInstantiationFunctionDecl(declaration);
          if (templateInstantiationFunctionDecl != NULL)
             {
#if DEBUG_FUNCTION_REFERENCE_SUPPORT
               printf ("In unparseFuncRef(): declaration->get_declarationModifier().isFriend() = %s \n",declaration->get_declarationModifier().isFriend() ? "true" : "false");
               printf ("In unparseFuncRef(): diff = %d \n",diff);
#endif
#if DEBUG_FUNCTION_REFERENCE_SUPPORT
               printf ("In unparseFuncRef(): templateInstantiationFunctionDecl = %p \n",templateInstantiationFunctionDecl);
#endif
            // SgTemplateFunctionDeclaration* templateFunctionDeclaration = templateInstantiationFunctionDecl->get_templateDeclaration();
            // ASSERT_not_null(templateFunctionDeclaration);
#if DEBUG_FUNCTION_REFERENCE_SUPPORT
            // printf ("In unparseFuncRef(): templateFunctionDeclaration->get_template_argument_list_is_explicit() = %s \n",templateFunctionDeclaration->get_template_argument_list_is_explicit() ? "true" : "false");
               printf ("In unparseFuncRef(): templateInstantiationFunctionDecl->get_template_argument_list_is_explicit() = %s \n",templateInstantiationFunctionDecl->get_template_argument_list_is_explicit() ? "true" : "false");
#endif
               if ( (declaration->get_declarationModifier().isFriend() == false) && (diff == 0) )
                  {
#if DEBUG_FUNCTION_REFERENCE_SUPPORT
                    printf ("Regenerate the name func_name = %s \n",func_name.c_str());
                    printf ("templateInstantiationFunctionDecl->get_templateName() = %s \n",templateInstantiationFunctionDecl->get_templateName().str());
#endif
                    unparseTemplateFunctionName(templateInstantiationFunctionDecl,info);
                  }
                 else
                  {
                 // This case supports test2004_77.C
#if DEBUG_FUNCTION_REFERENCE_SUPPORT
                    printf ("In unparseFuncRef(): No name qualification permitted in this case! \n");
#endif
                    curprint (func_name);
                  }
             }
            else
             {
               curprint (func_name);
             }
        }

       // End of old code (not yet intended properly).
        }

  // printDebugInfo("unparseFuncRef, Function Name: ", false); printDebugInfo(func_name.c_str(), true);

#if DEBUG_FUNCTION_REFERENCE_SUPPORT
     printf ("Leaving unparseFuncRefSupport() \n");
#endif
   }


void
Unparse_ExprStmt::unparseMFuncRef ( SgExpression* expr, SgUnparse_Info& info )
   {
     unparseMFuncRefSupport<SgMemberFunctionRefExp>(expr,info);
   }




#if 1
// DQ (7/6/2014): A different version of this is in the unparseCxx_expressions.C file.
bool
partOfArrowOperatorChain(SgExpression* expr)
   {
#define DEBUG_ARROW_OPERATOR_CHAIN 0

     SgBinaryOp* binary_op = isSgBinaryOp(expr);
  // ASSERT_not_null(binary_op);

     bool result = false;

  // DQ (7/6/2014): We need this test to avoid more general cases where this function can be called.
     if (binary_op != NULL)
        {
#if DEBUG_ARROW_OPERATOR_CHAIN
          printf ("Inside of partOfArrowOperatorChain(): binary_op = %p = %s \n",binary_op,binary_op->class_name().c_str());
#endif

  // DQ (4/9/2013): Added support for unparsing "operator+(x,y)" in place of "x+y".  This is 
  // required in places even though we have historically defaulted to the generation of the 
  // operator syntax (e.g. "x+y"), see test2013_100.C for an example of where this is required.
     SgNode* possibleParentFunctionCall = binary_op->get_parent();

  // DQ (4/9/2013): This fails for test2006_92.C.
  // ASSERT_not_null(possibleFunctionCall);
//   bool parent_is_a_function_call                    = false;
//   bool parent_function_call_uses_operator_syntax    = false;
     bool parent_function_is_overloaded_arrow_operator = false;
//   bool parent_function_call_is_compiler_generated   = false;
     if (possibleParentFunctionCall != NULL)
        {
          SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(possibleParentFunctionCall);
          if (functionCallExp != NULL)
             {
//             parent_is_a_function_call                  = true;
//             parent_function_call_uses_operator_syntax  = functionCallExp->get_uses_operator_syntax();
//             parent_function_call_is_compiler_generated = functionCallExp->isCompilerGenerated();
#if 1
            // DQ (7/5/2014): Add code to detect use of overloaded "operator->" as a special case.
               SgExpression* rhs = binary_op->get_rhs_operand();
            // bool isRelevantOverloadedOperator = false;
               SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(rhs);
               if (memberFunctionRefExp != NULL)
                  {
                    string functionName = memberFunctionRefExp->get_symbol()->get_name();
#if DEBUG_ARROW_OPERATOR_CHAIN
                    printf ("--- parent function is: functionName = %s \n",functionName.c_str());
#endif
                    if (functionName == "operator->")
                       {
                         parent_function_is_overloaded_arrow_operator = true;
                       }
                  }
#endif
               if (parent_function_is_overloaded_arrow_operator == true)
                  {
                    SgExpression* expression = isSgExpression(functionCallExp->get_parent());
                    if (expression != NULL)
                       {
                         SgCastExp* castExp = isSgCastExp(expression);
                         if (castExp != NULL)
                            {
                           // Skip over an SgCastExp IR nodes (see test2014_72.C).
                              expression = isSgExpression(castExp->get_parent());
                            }

                         SgArrowExp* arrowExp = isSgArrowExp(expression);
                         if (arrowExp != NULL)
                            {
                              result = true;
                            }
                           else
                            {
                              result = partOfArrowOperatorChain(expression);
                            }
                       }
                      else
                       {
                         result = false;
                       }
                  }
                 else
                  {
                    result = false;
                  }
             }
        }
        }

#if DEBUG_ARROW_OPERATOR_CHAIN
     printf ("Leaving partOfArrowOperatorChain(SgExpression* expr = %p = %s): result = %s \n",expr,expr->class_name().c_str(),result ? "true" : "false");
#endif

     return result;
   }
#endif


template <class T>
void
Unparse_ExprStmt::unparseMFuncRefSupport ( SgExpression* expr, SgUnparse_Info& info )
   {
  // CH (4/7/2010): This issue is because of using a MSVC keyword 'cdecl' as a variable name

#define MFuncRefSupport_DEBUG 0

     T* mfunc_ref = dynamic_cast<T*>(expr);
     ASSERT_not_null(mfunc_ref);

#if MFuncRefSupport_DEBUG
     printf ("In unparseMFuncRefSupport(): expr = %p = %s \n",expr,expr->class_name().c_str());
#endif
#if MFuncRefSupport_DEBUG
     curprint ("\n /* Inside of unparseMFuncRef " + StringUtility::numberToString(expr) + " */ \n");
#endif

  // info.display("Inside of unparseMFuncRef");

     SgMemberFunctionDeclaration* mfd  = mfunc_ref->get_symbol()->get_declaration();
     ASSERT_not_null(mfd);

#if MFuncRefSupport_DEBUG
     printf ("mfunc_ref->get_symbol()->get_name() = %s \n",mfunc_ref->get_symbol()->get_name().str());
     printf ("mfunc_ref->get_symbol()->get_declaration()->get_name() = %s \n",mfunc_ref->get_symbol()->get_declaration()->get_name().str());
#endif

  // DQ (4/8/2013): Added support for unparsing "operator+(x,y)" in place of "x+y".  This is 
  // required in places even though we have historically defaulted to the generation of the 
  // operator syntax (e.g. "x+y"), see test2013_100.C for an example of where this is required.
     ASSERT_not_null(mfunc_ref->get_parent());
     SgNode* possibleFunctionCall = mfunc_ref->get_parent()->get_parent();

     if (possibleFunctionCall == NULL)
        {
       // DQ (3/5/2017): Converted to use message logging.
          mprintf ("In unparseMFuncRefSupport(): possibleFunctionCall == NULL: mfunc_ref = %p = %s \n",mfunc_ref,mfunc_ref->class_name().c_str());
          SgNode* parent = mfunc_ref->get_parent();
          mprintf ("  ---  parent = %p = %s \n",parent,parent->class_name().c_str());
          ROSE_ASSERT(parent->get_parent() == NULL);
        }

  // DQ (10/16/2016): Fix for test2016_84.C and test2016_85.C (simpler code) specific to EDG 4.11 use.
  // ASSERT_not_null(possibleFunctionCall);
     bool uses_operator_syntax = false;
     if (possibleFunctionCall != NULL)
        {
          SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(possibleFunctionCall);
       // bool is_compiler_generated = false;
          if (functionCallExp != NULL)
             {
               uses_operator_syntax  = functionCallExp->get_uses_operator_syntax();
            // is_compiler_generated = functionCallExp->isCompilerGenerated();
#if 0
            // DQ (8/28/2014): It is a bug in GNU 4.4.7 to use the non-operator syntax of a user-defined conversion operator.
            // So we have to detect such operators and then detect if they are implicit then mark them to use the operator 
            // syntax plus supress them from being output.  We might alternatively go directly to supressing them from being
            // output, except that this is more complex for the non-operator syntax unparsing (I think).

               SgFunctionSymbol* functionSymbol = mfunc_ref->get_symbol();
               ASSERT_not_null(functionSymbol);
               SgFunctionDeclaration* functionDeclaration = functionSymbol->get_declaration();
               ASSERT_not_null(functionDeclaration);
               SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(functionDeclaration);
               ASSERT_not_null(memberFunctionDeclaration);

               if (functionDeclaration->get_specialFunctionModifier().isConversion() == true)
                  {
#if 0
                    printf ("Detected a conversion operator! \n");
#endif
                 // Force output of generated code using the operator syntax, plus supress the output if is_compiler_generated == true.
                    uses_operator_syntax = true;
                    if (is_compiler_generated == true)
                       {
#if 0
                         printf ("Detected is_compiler_generated == true for conversion operator! \n");
#endif
#if 0
                         printf ("Exiting as a test! \n");
                         ROSE_ASSERT(false);
#endif
                       }
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }
#endif
             }
        }

     SgExpression* binary_op = isSgExpression(mfunc_ref->get_parent());
  // TV (11/15/2018): With EDG 5.0, it happens inside some STL include (originating from <string>).
  // ASSERT_not_null(binary_op);
     bool isPartOfArrowOperatorChain = binary_op != NULL ? partOfArrowOperatorChain(binary_op) : false;

#if MFuncRefSupport_DEBUG
     printf ("In unparseMFuncRefSupport(): isPartOfArrowOperatorChain                   = %s \n",isPartOfArrowOperatorChain ? "true" : "false");
     printf ("In unparseMFuncRefSupport(): uses_operator_syntax  = %s \n",uses_operator_syntax ? "true" : "false");
  // printf ("In unparseMFuncRefSupport(): is_compiler_generated = %s \n",is_compiler_generated ? "true" : "false");
#endif
#if MFuncRefSupport_DEBUG
     curprint (string("\n /* Inside of unparseMFuncRef: uses_operator_syntax  = ") + (uses_operator_syntax ? "true" : "false") + " */ \n");
  // curprint (string("\n /* Inside of unparseMFuncRef: is_compiler_generated = ") + (is_compiler_generated ? "true" : "false") + " */ \n");
#endif

  // DQ (11/17/2004): Interface modified, use get_class_scope() if we want a
  // SgClassDefinition, else use get_scope() if we want a SgScopeStatement.
  // SgClassDefinition*           cdef = mfd->get_scope();
     SgDeclarationStatement* decl = mfd->get_associatedClassDeclaration();
     SgClassDeclaration* xdecl = isSgClassDeclaration(decl);
     SgNonrealDecl* nrdecl = isSgNonrealDecl(decl);

#if MFuncRefSupport_DEBUG
     printf ("In unparseMFuncRefSupport(): expr = %p (name = %s::%s) \n",expr,xdecl? xdecl->get_name().str() : ( nrdecl ? nrdecl->get_name().str() : "" ),mfd->get_name().str());
#endif
#if 0
     curprint ("\n /* Inside of unparseMFuncRef */ \n");
#endif
#if 0
     mfd->get_functionModifier().display("In unparseMFuncRef: functionModifier");
     mfd->get_specialFunctionModifier().display("In unparseMFuncRef: specialFunctionModifier");
#endif

  // If we have previously computed a name for this function (because it was a templated function 
  // with template arguments that required name qualification) then output the name directly.

  // DQ (6/21/2011): This controls if we output the generated name of the type (required to 
  // support name qualification of subtypes) or if we unparse the type from the AST (where 
  // name qualification of subtypes is not required).
     bool usingGeneratedNameQualifiedFunctionNameString = false;
     string functionNameString;

  // DQ (6/4/2011): Support for output of generated string for type (used where name 
  // qualification is required for subtypes (e.g. template arguments)).
     SgNode* nodeReferenceToFunction = info.get_reference_node_for_qualification();
#if MFuncRefSupport_DEBUG
     printf ("In unparseMFuncRefSupport(): nodeReferenceToFunction = %p \n",nodeReferenceToFunction);
#endif
     if (nodeReferenceToFunction != NULL)
        {
#if MFuncRefSupport_DEBUG
          printf ("rrrrrrrrrrrr In unparseMFuncRefSupport() output type generated name: nodeReferenceToFunction = %p = %s SgNode::get_globalTypeNameMap().size() = %" PRIuPTR " \n",
               nodeReferenceToFunction,nodeReferenceToFunction->class_name().c_str(),SgNode::get_globalTypeNameMap().size());
#endif
#if 0
       // DQ (7/9/2019): This will cause the class specifier to be output.
          std::map<SgNode*,std::string>::iterator i = SgNode::get_globalTypeNameMap().find(nodeReferenceToFunction);
          if (i != SgNode::get_globalTypeNameMap().end())
             {
            // I think this branch supports non-template member functions in template classes (called with explicit template arguments).
               usingGeneratedNameQualifiedFunctionNameString = true;

               functionNameString = i->second.c_str();
#if MFuncRefSupport_DEBUG
               printf ("ssssssssssssssss Found type name in SgNode::get_globalTypeNameMap() typeNameString = %s for nodeReferenceToType = %p = %s \n",
                    functionNameString.c_str(),nodeReferenceToFunction,nodeReferenceToFunction->class_name().c_str());
#endif
             }
          else
#else
         // DQ (7/9/2019): Debugging test2019_493.C.
         // printf ("In unparseMFuncRefSupport(): Don't use the globalTypeNameMap since it uses the class specifier! \n");
#endif
             {
#if MFuncRefSupport_DEBUG
               printf ("Could not find saved name qualified function name in globalTypeNameMap: using key: nodeReferenceToFunction = %p = %s \n",nodeReferenceToFunction,nodeReferenceToFunction->class_name().c_str());
#endif
#if 0
            // DQ (6/23/2013): If it was not present in the globalTypeNameMap, then look in the globalQualifiedNameMapForNames.
            // However, this is the qualified name for the member function ref, not the generated name of the member function.
               std::map<SgNode*,std::string>::iterator i = SgNode::get_globalQualifiedNameMapForNames().find(mfunc_ref);
               if (i != SgNode::get_globalQualifiedNameMapForNames().end())
                  {
                 // I think this branch supports template member functions (called with explicit template arguments) (see test2013_221.C).

#error "DEAD CODE!"

                    printf ("Commented out usingGeneratedNameQualifiedFunctionNameString: Not using the saved generated name from globalQualifiedNameMapForNames() \n");
                 // usingGeneratedNameQualifiedFunctionNameString = true;

                    functionNameString = i->second.c_str();
#if 0
                    printf ("tttttttttttttt Found type name in SgNode::get_globalQualifiedNameMapForNames() typeNameString = %s for nodeReferenceToType = %p = %s \n",
                         functionNameString.c_str(),mfunc_ref,mfunc_ref->class_name().c_str());
#endif
                  }
                 else
                  {
#if 0
                    printf ("Could not find saved name qualified function name in globalNameMap: nodeReferenceToFunction = %p \n",nodeReferenceToFunction);
#endif
                  }
#endif
#if 1
            // DQ (6/23/2013): This will get any generated name for the member function (typically only generated if template argument name qualification was required).
               std::map<SgNode*,std::string>::iterator j = SgNode::get_globalTypeNameMap().find(mfunc_ref);
               if (j != SgNode::get_globalTypeNameMap().end())
                  {
                 // I think this branch supports non-template member functions in template classes (called with explicit template arguments).

                 // printf ("Commented out usingGeneratedNameQualifiedFunctionNameString: Not using the saved generated name from globalTypeNameMap() \n");
                    usingGeneratedNameQualifiedFunctionNameString = true;

                    functionNameString = j->second.c_str();
#if MFuncRefSupport_DEBUG
                    printf ("uuuuuuuuuuuuuuuuuuuu Found type name in SgNode::get_globalTypeNameMap() typeNameString = %s for nodeReferenceToType = %p = %s \n",
                         functionNameString.c_str(),mfunc_ref,mfunc_ref->class_name().c_str());
#endif
                  }
                 else
                  {
#if MFuncRefSupport_DEBUG
                    printf ("Could not find saved name qualified function name in globalTypeNameMap: using key: mfunc_ref = %p = %s \n",mfunc_ref,mfunc_ref->class_name().c_str());
#endif
                  }
#endif
             }
        }
       else
        {
       // DQ (6/23/2011): Make this a warning since the tests/nonsmoke/functional/CompileTests/OpenMP_tests/alignment.c fails in the tests/nonsmoke/functional/roseTests/ompLoweringTests directory.
       // This also happens for the tests/nonsmoke/functional/roseTests/programAnalysisTests/testPtr1.C when run by the tests/nonsmoke/functional/roseTests/programAnalysisTests/PtrAnalTest tool.

       // printf ("ERROR: In unparseType(): nodeReferenceToFunction = NULL \n");
       // printf ("WARNING: In unparseType(): nodeReferenceToFunction = NULL \n");
       // ROSE_ASSERT(false);
        }

#if 0
     printf ("In unparseMFuncRef(): usingGeneratedNameQualifiedFunctionNameString = %s \n",usingGeneratedNameQualifiedFunctionNameString ? "true" : "false");
#endif
#if MFuncRefSupport_DEBUG
     printf ("In unparseMFuncRefSupport(): functionNameString = %s \n",functionNameString.c_str());
#endif

     if (usingGeneratedNameQualifiedFunctionNameString == true)
        {
       // Output the previously generated type name contianing the correct name qualification of subtypes (e.g. template arguments).
       // curprint ("/* output the function in unparseFuncRef() */");

          curprint(functionNameString);
       // curprint ("/* DONE: output the function in unparseFuncRef() */");
        }
       else
        {
       // This is the code that was always used before the addition of type names generated from where name qualification of subtypes are required.

       // Start of old code (not yet intended properly).

  // qualified name is always outputed except when the p_need_qualifier is
  // set to 0 (when the naming class is identical to the selection class, and
  // and when we aren't suppressing the virtual function mechanism).

  // if (!get_is_virtual_call()) -- take off because this is not properly set

  // DQ (9/17/2004): Added assertion
     ASSERT_not_null(decl);
     if (decl->get_parent() == NULL)
        {
       // DQ (3/5/2017): Converted to use message logging.
          mprintf ("Note: decl->get_parent() == NULL for decl = %p = %s (name = %s::%s) (OK for index expresion in array type) \n",
               decl,decl->class_name().c_str(),xdecl? xdecl->get_name().str() : ( nrdecl ? nrdecl->get_name().str() : "" ),mfd->get_name().str());
        }
  // DQ (5/30/2016): This need not have a parent if it is an expression in index for an array type (see test2016_33.C).
  // ASSERT_not_null(decl->get_parent());

     bool print_colons = false;

#if MFuncRefSupport_DEBUG
     printf ("mfunc_ref->get_need_qualifier() = %s \n",(mfunc_ref->get_need_qualifier() == true) ? "true" : "false");
#endif

  // DQ (11/7/2012): This is important for Elsa test code t0051.cc and now also test2012_240.C (putting it back).
  // DQ (3/28/2012): I think this is a bug left over from the previous implementation of support for name qualification.
  // if (mfunc_ref->get_need_qualifier() == true)
  // SgFunctionCallExp* functionCall = isSgFunctionCallExp(mfunc_ref->get_parent());
  // if (functionCall != NULL)
     if (mfunc_ref->get_need_qualifier() == true)
        {
       // check if this is a iostream operator function and the value of the overload opt is false
#if 0
          printf ("unp->opt.get_overload_opt()        = %s \n",unp->opt.get_overload_opt() ? "true" : "false");
          printf ("unp->u_sage->isOperator(mfunc_ref) = %s \n",unp->u_sage->isOperator(mfunc_ref) ? "true" : "false");
#endif
       // DQ (12/28/2005): Changed to check for more general overloaded operators (e.g. operator[])
       // if (!unp->opt.get_overload_opt() && isIOStreamOperator(mfunc_ref));
       // if (unp->opt.get_overload_opt() == false && unp->u_sage->isOperator(mfunc_ref) == true)
          if (unp->opt.get_overload_opt() == false && (uses_operator_syntax == true) && unp->u_sage->isOperator(mfunc_ref) == true)
             {
            // ... nothing to do here
             }
            else
             {
#if 0
               curprint("\n /* Output the qualified class name: mfunc_ref->get_need_qualifier() == true */ \n");
#endif
            // printf ("In unparseMFuncRef(): Qualified names of member function reference expressions are not handled yet! \n");
            // DQ (6/1/2011): Use the newly generated qualified names.
               SgName nameQualifier = mfunc_ref->get_qualified_name_prefix();
               curprint (nameQualifier);
#if 0
               printf ("Output name qualification for SgMemberFunctionDeclaration: nameQualifier = %s \n",nameQualifier.str());
#endif
               print_colons = true;
             }
        }
       else
        {
       // See test2012_51.C for an example of this.

       // printf ("In unparseMFuncRefSupport(): mfunc_ref->get_parent() = %p = %s \n",mfunc_ref->get_parent(),mfunc_ref->get_parent()->class_name().c_str());
          SgAddressOfOp* addressOperator = isSgAddressOfOp(mfunc_ref->get_parent());
          if (addressOperator != NULL)
             {
            // DQ (5/19/2012): This case also happens for test2005_112.C. This case is now supported.
            // When the address of a member function is take it must use the qualified name.
               SgName nameQualifier = mfunc_ref->get_qualified_name_prefix();
#if 0
               curprint("\n /* Output the qualified class name for SgAddressOfOp */ \n");
#endif
               curprint (nameQualifier);
            // printf ("Output name qualification for SgMemberFunctionDeclaration: nameQualifier = %s \n",nameQualifier.str());
               print_colons = true;
             }
        }

  // comments about the logic below can be found above in the unparseFuncRef function.

  // char* func_name = mfunc_ref->get_symbol()->get_name();
     string func_name = mfunc_ref->get_symbol()->get_name().str();

     string full_function_name = func_name;

#if MFuncRefSupport_DEBUG
 // DQ (2/8/2014): This is a problem when we output comments in the func_name and comments will not nest.
 // curprint ( "\n /* Inside of unparseMFuncRef (after name qualification) func_name = " + func_name + " */ \n");
#endif
#if MFuncRefSupport_DEBUG
     printf ("func_name before processing to extract operator substring = %s \n",func_name.c_str());

     printf ("unp->opt.get_overload_opt()                            = %s \n",(unp->opt.get_overload_opt() == true) ? "true" : "false");
     printf ("strncmp(func_name, \"operator\", 8)                 = %d \n",strncmp(func_name.c_str(), "operator", 8));
     printf ("print_colons                                      = %s \n",(print_colons == true) ? "true" : "false");
     printf ("mfd->get_specialFunctionModifier().isConversion() = %s \n",(mfd->get_specialFunctionModifier().isConversion() == true) ? "true" : "false");
#endif

  // DQ (2/21/2019): Need to avoid processing operator>>=().
  // DQ (4/7/2013): This code is translating "s >> len;" to "s > > len;" in test2013_97.C.
  // if (mfunc_ref->get_symbol()->get_name() != "operator>>")
  // if ( !( (mfunc_ref->get_symbol()->get_name() == "operator>>") || (mfunc_ref->get_symbol()->get_name() == "operator>>=") ) )
     if ( (mfunc_ref->get_symbol()->get_name() != "operator>>") && (mfunc_ref->get_symbol()->get_name() != "operator>>=") )
        {
       // DQ (11/18/2012): Process the function name to remove any cases of ">>" from template names.
          string targetString      = ">>";
          string replacementString = "> >";
          size_t found = func_name.find(targetString);
          while (found != string::npos)
             {
               func_name.replace( found, targetString.length(), replacementString );
               found = func_name.find( targetString );
             }
        }

#if MFuncRefSupport_DEBUG
     printf("In unparseMFuncRefSupport(): func_name after processing to remove >> references = %s \n",func_name.c_str());
     curprint("\n /* Inside of unparseMFuncRef (after name qualification and before output of function name) func_name = " + func_name + " */ \n");
#endif

  // DQ (7/6/2014): Added support for if the operator is compiler generated (undid this change 
  // since overloaded operators using operator syntax will always be marked as compiler generated).
  // DQ (11/24/2004): unparse conversion operators ("operator X&();") as "result.operator X&()"
  // instead of "(X&) result" (which appears as a cast instead of a function call.
  // check that this an operator overloading function and that colons were not printed
  // if (!unp->opt.get_overload_opt() && !strncmp(func_name, "operator", 8) && !print_colons)
  // if (!unp->opt.get_overload_opt() && func_name.size() >= 8 && func_name.substr(0, 8) == "operator" &&  !print_colons && !mfd->get_specialFunctionModifier().isConversion())
  // if (!unp->opt.get_overload_opt() && (uses_operator_syntax == true) && func_name.size() >= 8 && func_name.substr(0, 8) == "operator" &&  !print_colons && !mfd->get_specialFunctionModifier().isConversion())
  // if (!unp->opt.get_overload_opt() && (uses_operator_syntax == true && is_compiler_generated == true) && func_name.size() >= 8 && func_name.substr(0, 8) == "operator" &&  !print_colons && !mfd->get_specialFunctionModifier().isConversion())
     if (!unp->opt.get_overload_opt() && (uses_operator_syntax == true) && func_name.size() >= 8 && func_name.substr(0, 8) == "operator" &&  !print_colons && !mfd->get_specialFunctionModifier().isConversion())
        {
          func_name = func_name.substr(8);
        }
#if MFuncRefSupport_DEBUG
     printf ("func_name after processing to extract operator substring = %s \n",func_name.c_str());
#endif

     if (func_name == "[]")
        {
       // DQ (12/28/2005): This case now appears to be just dead code!

       //
       // [DT] 3/30/2000 -- Don't unparse anything here.  The square brackets will
       //      be handled from unparseFuncCall().
       //
       //      May want to handle overloaded operator() the same way.
#if 0
          printf ("Case of unparsing \"operator[]\" \n");
          curprint ("\n /* Case of unparsing \"operator[]\" */ \n");
#endif
       // This is a special case, while the input code may be either expressed as "a[i]" or "a.operator[i]"
       // (we can't tell which from the EDG AST, I think).
       // often we want to unparse the code as "a[i]" but there is a case were this is not possible
       // ("a->operator[](i)" is valid as is "(*a)[i]", but only if the operator-> is not defined for 
       // the type of which "a" is a variable).  So here we check the lhs of the parent of the curprintrent
       // expression so that we can detect this special case!

       // DQ (12/11/2004): We need to unparse the keyword "operator" in this special cases (see test2004_159.C)
          SgExpression* parentExpression = isSgExpression(expr->get_parent());
          ASSERT_not_null(parentExpression);
       // printf ("parentExpression = %p = %s \n",parentExpression,parentExpression->sage_class_name());
          SgDotExp* dotExpression = isSgDotExp(parentExpression);
          if (dotExpression != NULL)
             {
               SgExpression* lhs = dotExpression->get_lhs_operand();
               ASSERT_not_null(lhs);
#if 0
               printf ("lhs = %p = %s \n",lhs,lhs->class_name().c_str());
               curprint ( "\n /* lhs = " + StringUtility::numberToString(lhs) + " = " + lhs->class_name() + " */ \n");
#endif
             }
        }
       else
        {
#if MFuncRefSupport_DEBUG
          printf ("Case of unparsing a member function which is NOT short form of \"operator[]\" (i.e. \"[]\") funct_name = %s \n",func_name.c_str());
#endif
       // Make sure that the member function name does not include "()" (this prevents "operator()()" from being output)
          if (func_name != "()")
             {
#if MFuncRefSupport_DEBUG
               printf ("Case of unparsing a member function which is NOT \"operator()\" \n");
               curprint ("/* Case of unparsing a member function which is NOT \"operator()\" */ \n");
#endif
            // DQ (12/11/2004): Catch special case of "a.operator->();" and avoid unparsing it as "a->;" (illegal C++ code)
            // Get the parent SgFunctionCall so that we can check if it's parent was a SgDotExp with a valid rhs_operand!
            // if not then we have the case of "a.operator->();"

            // It might be that this could be a "->" instead of a "."
               ASSERT_not_null(mfunc_ref);
               SgDotExp   *dotExpression   = isSgDotExp  (mfunc_ref->get_parent());
               SgArrowExp *arrowExpression = isSgArrowExp(mfunc_ref->get_parent());
            // ROSE_ASSERT(dotExpression != NULL || arrowExpression != NULL);

            // printf ("dotExpression = %p arrowExpression = %p \n",dotExpression,arrowExpression);

            // Note that not all references to a member function are a function call.
               SgFunctionCallExp* functionCall = NULL;
               if (dotExpression != NULL)
                  {
                 // printf ("Check for X.operator->(); then check for X->operator->() \n");
                    functionCall = isSgFunctionCallExp(dotExpression->get_parent());
                  }
               if (arrowExpression != NULL)
                  {
                 // printf ("Not a X.operator->(); check for X->operator->() \n");
                    functionCall = isSgFunctionCallExp(arrowExpression->get_parent());
                  }

#if MFuncRefSupport_DEBUG
               curprint (string("/* In unparseMFuncRefSupport(): (functionCall != NULL) && (uses_operator_syntax == false) = ") + (((functionCall != NULL) && (uses_operator_syntax == false)) ? "true" : "false") + " */ \n");
               curprint (string("/* In unparseMFuncRefSupport(): (functionCall != NULL) = ") + ((functionCall != NULL) ? "true" : "false") + " */ \n");
               curprint (string("/* In unparseMFuncRefSupport(): uses_operator_syntax   = ") + (uses_operator_syntax   ? "true" : "false") + " */ \n");
#endif
#if MFuncRefSupport_DEBUG
               printf ("In unparseMFuncRefSupport(): functionCall = %p uses_operator_syntax = %s \n",functionCall,uses_operator_syntax ? "true" : "false");
#endif
            // if (functionCall != NULL)
               if ( (functionCall != NULL) && (uses_operator_syntax == false) )
                  {
                 // SgExpressionRoot* expressionRoot = isSgExpressionRoot(functionCall->get_parent());
                 // if ( (expressionRoot != NULL) && (isUnaryOperatorArrowSubtree(functionCall) == true) )
                    if ( unp->u_sage->isUnaryOperatorArrowSubtree(functionCall) == true )
                       {
                      // DQ (Dec, 2004): special (rare) case of .operator->() or ->operator->()
                      // decided to handle these cases because they are amusing (C++ Trivia) :-).
#if 0
                         printf ("Output special case of .operator->() or ->operator->() \n");
                         curprint("\n /* Output special case of .operator->() or ->operator->() */ \n");
#endif
                         if (dotExpression != NULL)
                            {
                           // curprint(".operator->()");
                           // curprint(".operator->");
                              curprint("operator->");
                            }
                           else
                            {
                           // curprint("->operator->()");
                           // curprint("->operator->");
                              curprint("operator->");
                            }
                       }
                      else
                       {
                      // DQ (2/9/2010): Fix for test2010_03.C
#if 0
                         printf ("In unparseMFuncRefSupport(): output function name: func_name = %s \n",func_name.c_str());
#endif
#if 0
                      // DQ (6/15/2013): This is the older version of the code (which si a problem for test2013_206.C).
                      // curprint(func_name);
#error "DEAD CODE!"
                         curprint(" " + func_name + " ");
#else
#if 1
                      // DQ (6/15/2013): The code for processing the function name when it contains template arguments that requires name qualification.

                      // DQ (5/25/2013): Added support to unparse the template arguments seperately from the member function name (which should NOT 
                      // include the template arguments when unparsing). Note the the template arguments in the name are important for the generation
                      // of mangled names for use in symbol tabls, but that we need to output the member function name and it's template arguments 
                      // seperately so that they name qulification can be computed and saved in the name qualification name maps.

                      // Note that this code below is a copy of that from the support for unpasing the SgTemplateInstantiationFunctionDecl (in function above).

                         SgDeclarationStatement* declaration = mfd;

                      // DQ (6/21/2011): Support for new name qualification (output of generated function name).
                         ASSERT_not_null(declaration);
#if 0
                         printf ("Inside of Unparse_ExprStmt::unparseFuncRef(): declaration = %p = %s \n",declaration,declaration->class_name().c_str());
#endif
#if 0
                      // DQ (4/15/2013): If there is other debug output turned on then nesting of comments inside of comments can occur in this output (see test2007_17.C).
                         curprint (string("\n /* In unparseMFuncRef(): put out func_name = ") + func_name + " */ \n ");
#endif
                      // If this is a template then the name will include template arguments which require name qualification and the name 
                      // qualification will depend on where the name is referenced in the code.  So we have generate the non-canonical name 
                      // with all possible qualifications and save it to be reused by the unparser when it unparses the tempated function name.
                         SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDecl = isSgTemplateInstantiationMemberFunctionDecl(declaration);
                         if (templateInstantiationMemberFunctionDecl != NULL)
                            {
#if 0
                              printf ("In unparseMFuncRefSupport(): templateInstantiationMemberFunctionDecl->get_template_argument_list_is_explicit() = %s \n",templateInstantiationMemberFunctionDecl->get_template_argument_list_is_explicit() ? "true" : "false");
#endif
#if 0
                              printf ("In unparseMFuncRefSupport(): declaration->get_declarationModifier().isFriend() = %s \n",declaration->get_declarationModifier().isFriend() ? "true" : "false");
                           // printf ("In unparseMFuncRefSupport(): diff = %d \n",diff);
#endif
                           // if ( (declaration->get_declarationModifier().isFriend() == false) && (diff == 0) )
                              if (declaration->get_declarationModifier().isFriend() == false)
                                 {
#if 0
                                   printf ("Regenerate the name func_name = %s for case (functionCall != NULL) && (uses_operator_syntax == false)) \n",func_name.c_str());
                                   printf ("templateInstantiationMemberFunctionDecl->get_templateName() = %s \n",templateInstantiationMemberFunctionDecl->get_templateName().str());
#endif
                                   unparseTemplateMemberFunctionName(templateInstantiationMemberFunctionDecl,info);

#if 0
                                // DQ (4/1/2018): Added debbuging for test2018_69.C.
                                   printf ("Exiting as a test! \n");
                                   ROSE_ASSERT(false);
#endif
                                 }
                                else
                                 {
                                // This case supports test2004_77.C

                                   printf ("WARNING: In unparseMFuncRef(): No name qualification permitted in this case! (not clear if this case if important for unparseMFuncRef(), as it was for unparseFuncRef()) \n");

                                // DQ (6/15/2013): I think this mod is required for test2010_03.C.
                                // curprint (func_name);
                                   curprint(" " + func_name + " ");
                                 }
                            }
                           else
                            {
                           // DQ (6/15/2013): I think this mod is required for test2010_03.C.
                           // curprint (func_name);
                              curprint(" " + func_name + " ");
                            }
#else
                 // DQ (10/21/2006): Only do name qualification of function names for C++
                    if (SageInterface::is_Cxx_language() == true)
                       {
#error "DEAD CODE!"
                      // DQ (12/2/2004): Added diff == 0 to avoid qualification of operators (avoids "i__gnu_cxx::!=0") 
                      // added some extra spaces to make it more clear if it is ever wrong again (i.e. "i __gnu_cxx:: != 0")
                      // DQ (11/13/2004) Modified to avoid qualified name for friend functions
                      // DQ (11/12/2004) Added support for qualification of function names output as function calls
                         if ( (declaration->get_declarationModifier().isFriend() == false) && (diff == 0) )
                            {
                           // DQ (8/6/2007): Now that we have a more sophisticated name qualifiation mechanism using 
                           // hidden declaration lists, we don't have to force the qualification of function names.
                           // DQ (10/15/2006): Force output of any qualified names for function calls.
                           // info.set_forceQualifiedNames();

                           // curprint ( "/* unparseFuncRef calling info.set_forceQualifiedNames() */ ";

                           // DQ (5/12/2011): Support for new name qualification.
                              SgUnparse_Info tmp_info(info);
                              tmp_info.set_name_qualification_length(func_ref->get_name_qualification_length());
                              tmp_info.set_global_qualification_required(func_ref->get_global_qualification_required());

#error "DEAD CODE!"
                           // SgName nameQualifier = unp->u_name->generateNameQualifier( declaration, info );
                           // SgName nameQualifier = unp->u_name->generateNameQualifier( declaration, tmp_info );

                           // DQ (5/29/2011): Newest refactored support for name qualification.
                           // printf ("In unparseFuncRef(): Looking for name qualification for SgFunctionRefExp = %p \n",func_ref);
                              SgName nameQualifier = func_ref->get_qualified_name_prefix();
#if 0
                              printf ("In unparseMFuncRefSupport(): nameQualifier = %s \n",nameQualifier.str());
                              printf ("SgNode::get_globalQualifiedNameMapForNames().size() = %" PRIuPTR " \n",SgNode::get_globalQualifiedNameMapForNames().size());
                              printf ("In unparseMFuncRefSupport(): Testing name in map: for SgFunctionRefExp = %p qualified name = %s \n",func_ref,func_ref->get_qualified_name_prefix().str());
                           // curprint ( "\n /* unparseFuncRef using nameQualifier = " + nameQualifier.str() + " */ \n";
#endif
#if 0
                              SgFunctionCallExp* functionCallExpression = isSgFunctionCallExp(expr->get_parent());
                              if (functionCallExpression != NULL)
                                 {
                                   printf ("Found the function call, global qualification is defined here functionCallExpression->get_global_qualified_name() = %s \n",
                                        functionCallExpression->get_global_qualified_name() == true ? "true" : "false");
                                   if (functionCallExpression->get_global_qualified_name() == true)
                                      {
                                        curprint ("::");
                                      }
                                 }
#endif
                              curprint (nameQualifier.str());
                           // curprint (nameQualifier.str() + " ";
#error "DEAD CODE!"
                            }
                           else
                            {
#if 0
                              printf ("In unparseFuncRef(): No name qualification permitted in this case! \n");
#endif
                            }
                       }
#endif
#endif
                       }
                  }
                 else
                  {
                 // DQ (2/9/2010): This does not fix test2010_03.C, but defines a uniform handling as in the fix above.
                 // curprint (func_name);
                 // curprint(" " + func_name + " ");

                 // If uses_operator_syntax == true, then we want to have the unparseMFuncRefSupport() NOT output the 
                 // operator name since it is best done by the binary operator handling (e.g. unparseBinaryExpr()).
                    if ( uses_operator_syntax == false )
                       {
#if MFuncRefSupport_DEBUG
                         printf ("In unparseMFuncRefSupport(): function name IS output \n");
                         curprint("/* In unparseMFuncRefSupport(): function name IS output */ \n");
#endif
#if 0
#error "DEAD CODE!"
                      // DQ (5/25/2013): This is the older version of the code.
                         curprint(" " + func_name + " ");
#else
                      // DQ (5/25/2013): Added support to unparse the template arguments seperately from the member function name (which should NOT 
                      // include the template arguments when unparsing). Note the the template arguments in the name are important for the generation
                      // of mangled names for use in symbol tabls, but that we need to output the member function name and it's template arguments 
                      // seperately so that they name qulification can be computed and saved in the name qualification name maps.

                      // Note that this code below is a copy of that from the support for unpasing the SgTemplateInstantiationFunctionDecl (in function above).
                         SgDeclarationStatement* declaration = mfd;

                      // DQ (6/21/2011): Support for new name qualification (output of generated function name).
                         ASSERT_not_null(declaration);
                      // printf ("Inside of Unparse_ExprStmt::unparseFuncRef(): declaration = %p = %s \n",declaration,declaration->class_name().c_str());
#if 0
                      // DQ (4/15/2013): If there is other debug output turned on then nesting of comments inside of comments can occur in this output (see test2007_17.C).
                         curprint (string("\n /* In unparseMFuncRefSupport(): put out func_name = ") + func_name + " */ \n ");
#endif
                      // If this is a template then the name will include template arguments which require name qualification and the name 
                      // qualification will depend on where the name is referenced in the code.  So we have generate the non-canonical name 
                      // with all possible qualifications and save it to be reused by the unparser when it unparses the tempated function name.
                         SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDecl = isSgTemplateInstantiationMemberFunctionDecl(declaration);
                         if (templateInstantiationMemberFunctionDecl != NULL)
                            {
#if 0
                              printf ("In unparseMFuncRefSupport(): declaration->get_declarationModifier().isFriend() = %s \n",declaration->get_declarationModifier().isFriend() ? "true" : "false");
                           // printf ("In unparseMFuncRef(): diff = %d \n",diff);
#endif
                           // if ( (declaration->get_declarationModifier().isFriend() == false) && (diff == 0) )
                              if (declaration->get_declarationModifier().isFriend() == false)
                                 {
#if 0
                                   printf ("Regenerate the name func_name = %s for case (uses_operator_syntax == false) \n",func_name.c_str());
                                   printf ("templateInstantiationMemberFunctionDecl->get_templateName() = %s \n",templateInstantiationMemberFunctionDecl->get_templateName().str());
#endif
                                   unparseTemplateMemberFunctionName(templateInstantiationMemberFunctionDecl,info);
                                 }
                                else
                                 {
                                // This case supports test2004_77.C
                                   printf ("WARNING: In unparseMFuncRefSupport(): No name qualification permitted in this case! (not clear if this case if important for unparseMFuncRef(), as it was for unparseFuncRef()) \n");

                                   curprint (func_name);
                                 }
                            }
                           else
                            {
                              curprint (func_name);
                            }
#endif
                       }
                      else
                       {
#if MFuncRefSupport_DEBUG
                         printf ("In unparseMFuncRefSupport(): function name is NOT output: full_function_name = %s \n",full_function_name.c_str());
                         curprint("/* In unparseMFuncRefSupport(): function name is NOT output */ \n");
#endif
#if MFuncRefSupport_DEBUG
                         printf ("In unparseMFuncRefSupport(): mfd->get_args().size() = %" PRIuPTR " \n",mfd->get_args().size());
#endif
                      // DQ (11/17/2013): We need to distinguish between unary and binary overloaded operators (for member 
                      // functions a unary operator has zero arguments, and a binary operator has a single argument).
                         bool is_unary_operator = (mfd->get_args().size() == 0);
#if MFuncRefSupport_DEBUG
                         printf ("In unparseMFuncRefSupport(): is_unary_operator     = %s \n",is_unary_operator     ? "true" : "false");
                      // printf ("In unparseMFuncRefSupport(): is_compiler_generated = %s \n",is_compiler_generated ? "true" : "false");
#endif
#if 1
                      // DQ (7/6/2014): If this is compiler generated then supress the output of the operator name.
                      // if (is_compiler_generated == false)
                         if (isPartOfArrowOperatorChain == false)
                            {
#endif
                      // DQ (7/5/2014): Adding operator-> as an additional special case.
                      // These operators require special handling since they are prefix operators when unparsed using operator syntax.
                      // if (full_function_name != "operator*" && full_function_name != "operator&")
                         if ( (is_unary_operator == false) || (is_unary_operator == true && full_function_name != "operator*" && full_function_name != "operator&"))
                      // if ( (is_unary_operator == false) || (is_unary_operator == true && full_function_name != "operator*" && full_function_name != "operator&" && full_function_name != "operator->"))
                            {
#if MFuncRefSupport_DEBUG
                              printf ("In unparseMFuncRefSupport(): not overloaded reference or dereference operator: function name IS output: func_name = %s \n",func_name.c_str());
                              curprint("/* In unparseMFuncRefSupport(): not overloaded reference or dereference operator: function name = " + func_name + " IS output */ \n");
#endif
                              curprint(" " + func_name + " ");
                           // curprint(" /* In unparseMFuncRefSupport(): function name is NOT output (not overloaded reference or dereference operator) */ " + func_name + " ");
                           // curprint(" /* In unparseMFuncRefSupport(): after output of function_name */ ");
                            }
                           else
                            {
#if MFuncRefSupport_DEBUG
                              printf ("info.isPrefixOperator() = %s \n",info.isPrefixOperator() ? "true" : "false");
#endif
                              if (info.isPrefixOperator() == true)
                                 {
                                   curprint(" " + func_name + " ");
                                 }
                                else
                                 {
#if MFuncRefSupport_DEBUG
                                   printf ("In unparseMFuncRefSupport(): function name is NOT output for this operator: func_name = %s \n",func_name.c_str());
                                   curprint("/* In unparseMFuncRefSupport(): function name is NOT output for this operator:  func_name = " + func_name + " */ \n");
#endif
                                 }
                            }
#if 1
                            }
                           else
                            {
#if MFuncRefSupport_DEBUG
                           // printf ("In unparseMFuncRefSupport(): case of compiler generated function: function name is NOT output for this operator: func_name = %s \n",func_name.c_str());
                           // curprint("/* In unparseMFuncRefSupport(): case of compiler generated function: function name is NOT output for this operator:  func_name = " + func_name + " */ \n");
                              printf ("In unparseMFuncRefSupport(): case of isPartOfArrowOperatorChain == true: function name is NOT output for this operator: func_name = %s \n",func_name.c_str());
                              curprint("/* In unparseMFuncRefSupport(): case of isPartOfArrowOperatorChain == true: function name is NOT output for this operator:  func_name = " + func_name + " */ \n");
#endif
                            }
#endif
                       }
                  }
#if 0
               if (arrowExpression != NULL)
                  {
#error "DEAD CODE!"
                 // DQ (2/17/2005): If the parent is an arrow expression then output the dereference operator!
                    curprint(" /* output a derference of the pointer implied by SgArrowExp */ ");
                    curprint(" * ");
                  }
#endif
             }
            else
             {
#if MFuncRefSupport_DEBUG
               printf ("Case of unparsing a member function which is \"operator()\" \n");
#endif
             }
#if 0
       // curprint ( mfunc_ref->get_symbol()->get_name();
          unp->u_debug->printDebugInfo("unparseMFuncRef, Function Name: ", false);
          unp->u_debug->printDebugInfo(func_name.c_str(), true);
#endif
        }

       // End of old code (not yet intended properly).
        }

#if MFuncRefSupport_DEBUG
     printf ("Leaving unparseMFuncRefSupport \n");
#endif
#if MFuncRefSupport_DEBUG
     curprint ("\n/* leaving unparseMFuncRefSupport */ \n");
#endif
   }


// DQ (2/21/2019): Adding support to remove "\000" substrings from output strings in SgStringVal unparsing.
template<typename T>
void remove_substrings(basic_string<T>& s, const basic_string<T>& p)
   {
     typename basic_string<T>::size_type n = p.length();

#if 0
     size_t string_size = s.length();
     for (size_t j = 0; j < string_size; j++)
        {
          int character_value = s[j];
          printf ("character_value = %d \n",character_value);
        }
#endif

     for (typename basic_string<T>::size_type i = s.find(p); i != basic_string<T>::npos; i = s.find(p))
        {
#if 0
          printf ("In remove_substrings: loop: i = %zu s = %s \n",i,s.c_str());
#endif
          s.erase(i, n);
        }

#if 0
     printf ("Leaving remove_substrings() \n");
#endif
   }


void
Unparse_ExprStmt::unparseStringVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgStringVal* str_val = isSgStringVal(expr);
     ASSERT_not_null(str_val);

#if 0
     printf ("In unparseStringVal(): expr = %p = %s \n",expr,expr->class_name().c_str());
#endif

  // Handle special case of macro specification (this is a temporary hack to permit us to
  // specify macros within transformations)

     int wrap = unp->u_sage->cur_get_linewrap();
     unp->u_sage->cur_get_linewrap();

#ifndef CXX_IS_ROSE_CODE_GENERATION
#if 0
  // const char* targetString = "ROSE-TRANSFORMATION-MACRO:";
     const char* targetString = "ROSE-MACRO-EXPRESSION:";
     int targetStringLength = strlen(targetString);
  // if (str_val->get_value() == NULL)
     if (str_val->get_value().empty() == true)
        {
          printf ("Found an pointer in SgStringVal = %p for value of string! \n",str_val);
          str_val->get_file_info()->display("Called from unparseStringVal: debug");
        }
     ASSERT_not_null(str_val->get_value());
     if (strncmp(str_val->get_value(),targetString,targetStringLength) == 0)
        {
       // unparse the string without the surrounding quotes and with a new line at the end
          char* remainingString = str_val->get_value()+targetStringLength;
          printf ("Specify a MACRO: remainingString = %s \n",remainingString);
       // Put in a leading CR so that the macro will always be unparsed onto its own line
       // Put in a trailing CR so that the trailing ";" will be unparsed onto its own line too!
          curprint ( "\n" + remainingString + "\n");
        }
       else
        {
          curprint ( "\"" + str_val->get_value() + "\"");
        }
     ASSERT_not_null(str_val->get_value());
#else
  // DQ (3/25/2006): Finally we can use the C++ string class
     string targetString = "ROSE-MACRO-CALL:";
     int targetStringLength = targetString.size();
     string stringValue = str_val->get_value();
     string::size_type location = stringValue.find(targetString);
     if (location != string::npos)
        {
       // unparse the string without the surrounding quotes and with a new line at the end
          string remainingString = stringValue.replace(location,targetStringLength,"");
       // printf ("Specify a MACRO: remainingString = %s \n",remainingString.c_str());
          remainingString.replace(remainingString.find("\\\""),4,"\"");
          curprint("\n" + remainingString + "\n");
        }
       else
        {
          SgFile* file = TransformationSupport::getFile(str_val);
#if 0
          printf ("In unparseStringVal(): resolving file to be %p \n",file);
#endif
       // bool is_Cxx_Compiler = file->get_Cxx_only();
          bool is_Cxx_Compiler = false;
          if (file != NULL)
             {
               is_Cxx_Compiler = file->get_Cxx_only();
             }
            else
             {
               printf ("Warning: TransformationSupport::getFile(str_val) == NULL \n");
             }
       // bool is_C_Compiler   = file->get_C_only();

       // curprint ( "\"" + str_val->get_value() + "\"";
          if (str_val->get_wcharString() == true)
             {
               curprint("L");
             }
            else
             {
               if (str_val->get_is16bitString() == true)
                  {
                    curprint("u");
                  }
                 else
                  {
                    if (str_val->get_is32bitString() == true)
                       {
                      // curprint("U");
                         if (is_Cxx_Compiler == true)
                            {
                              curprint("U");
                            }
                           else
                            {
                           // For C (C11) code.
                              curprint("L");
                            }
                       }
                      else
                       {
                      // This is the default, but "u8" would be a more explicit prefix.
                       }
                  }
             }

#if 1
       // DQ (8/13/2014): Added support for C++11 raw string prefix values.
          string s;
          if (str_val->get_isRawString() == true)
             {
               curprint("R");

            // DQ (2/21/2019): This is a problem for Cxx11_tets/test2019_180.C
            // Note added delimiters.
            // s = string("\"(") + str_val->get_raw_string_value() + string(")\"");
               s = string("\"(") + str_val->get_value() + string(")\"");
#if 0
               printf ("Before remove substrings: s = %s \n",s.c_str());
#endif
               string p = "\\000";
               remove_substrings(s, p);
#if 0
               printf ("After remove substrings: s = %s \n",s.c_str());
#endif
             }
            else
             {
               s = string("\"") + str_val->get_value() + string("\"");
             }
#else
       // curprint("\"" + str_val->get_value() + "\"");
          string s = string("\"") + str_val->get_value() + string("\"");
#endif
#if 0
          printf ("In unparseStringVal(): str_val         = %p \n",str_val);
          printf ("   --- str_val->get_value()            = %s \n",str_val->get_value().c_str());
          printf ("   --- str_val->get_value().length()   = %" PRIuPTR " \n",str_val->get_value().length());
          printf ("   --- output string: s                = %s \n",s.c_str());
          printf ("   --- str_val->get_raw_string_value() = %s \n",str_val->get_raw_string_value().c_str());
       // printf ("   --- str_val->get_valueString()      = %s \n",str_val->get_valueString().c_str());
#endif
          curprint(s);
        }
#endif
#endif

     unp->u_sage->cur_set_linewrap(wrap);
   }


void
Unparse_ExprStmt::unparseUIntVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgUnsignedIntVal* uint_val = isSgUnsignedIntVal(expr);
     ASSERT_not_null(uint_val);

  // curprint ( uint_val->get_value();
  // DQ (7/20/2006): Bug reported by Yarden, see test2006_94.C for where this is important (e.g. evaluation of "if (INT_MAX + 1U > 0)").
  // curprint ( "U";

  // DQ (8/30/2006): Make change suggested by Rama (patch)
     if (uint_val->get_valueString() == "")
        {
          curprint ( tostring(uint_val->get_value()) + "U");
        }
       else
        {
          curprint ( uint_val->get_valueString());
        }
   }

void
Unparse_ExprStmt::unparseLongIntVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgLongIntVal* longint_val = isSgLongIntVal(expr);
     ASSERT_not_null(longint_val);

  // curprint ( longint_val->get_value();
  // DQ (7/20/2006): Bug reported by Yarden, see test2006_94.C for where this is important (e.g. evaluation of "if (INT_MAX + 1U > 0)").
  // curprint ( "L";

  // DQ (8/30/2006): Make change suggested by Rama (patch)
     if (longint_val->get_valueString() == "")
        {
          curprint ( tostring(longint_val->get_value()) + "L");
        }
       else
        {
          curprint ( longint_val->get_valueString());
        }
   }

void
Unparse_ExprStmt::unparseLongLongIntVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgLongLongIntVal* longlongint_val = isSgLongLongIntVal(expr);
     ASSERT_not_null(longlongint_val);

  // curprint ( longlongint_val->get_value();
  // DQ (7/20/2006): Bug reported by Yarden, see test2006_94.C for where this is important (e.g. evaluation of "if (INT_MAX + 1U > 0)").
  // curprint ( "LL";

  // DQ (8/30/2006): Make change suggested by Rama (patch)
     if (longlongint_val->get_valueString() == "")
        {
          curprint ( tostring(longlongint_val->get_value()) + "LL");
        }
       else
        {
          curprint ( longlongint_val->get_valueString());
        }
   }

void
Unparse_ExprStmt::unparseULongLongIntVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgUnsignedLongLongIntVal* ulonglongint_val = isSgUnsignedLongLongIntVal(expr);
     ASSERT_not_null(ulonglongint_val);

  // curprint ( ulonglongint_val->get_value();
  // DQ (7/20/2006): Bug reported by Yarden, see test2006_94.C for where this is important (e.g. evaluation of "if (INT_MAX + 1U > 0)").
  // curprint ( "ULL";

  // DQ (8/30/2006): Make change suggested by Rama (patch)
     if (ulonglongint_val->get_valueString() == "")
        {
          curprint ( tostring(ulonglongint_val->get_value()) + "ULL");
        }
       else
        {
          curprint ( ulonglongint_val->get_valueString());
        }
   }

void
Unparse_ExprStmt::unparseULongIntVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgUnsignedLongVal* ulongint_val = isSgUnsignedLongVal(expr);
     ASSERT_not_null(ulongint_val);

  // curprint ( ulongint_val->get_value();
  // DQ (7/20/2006): Bug reported by Yarden, see test2006_94.C for where this is important (e.g. evaluation of "if (INT_MAX + 1U > 0)").
  // curprint ( "UL";

  // DQ (8/30/2006): Make change suggested by Rama (patch)
     if (ulongint_val->get_valueString() == "")
        {
          curprint ( tostring(ulongint_val->get_value()) + "UL");
        }
       else
        {
          curprint ( ulongint_val->get_valueString());
        }
   }

void
Unparse_ExprStmt::unparseFloatVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgFloatVal* float_val = isSgFloatVal(expr);
     ASSERT_not_null(float_val);

#if 0
     printf ("Inside of unparseFloatVal = %p \n",float_val);
     float_val->get_file_info()->display("unparseFloatVal");
#endif

  // DQ (10/18/2005): Need to handle C code which cannot use C++ mechanism to specify 
  // infinity, quiet NaN, and signaling NaN values.  Note that we can't use the C++
  // interface since the input program, and thus the generated code, might not have 
  // included the "limits" header file.
     float float_value = float_val->get_value();
#if 0
     if (SageInterface::is_C_language() == true)
        {
          if (float_val->get_valueString() == "")
             {
               curprint ( tostring(float_val->get_value()));
             }
            else
             {
               curprint ( float_val->get_valueString());
             }
        }
       else
        {
        }
#endif

     if (float_value == std::numeric_limits<float>::infinity())
        {
       // printf ("Infinite value found as value in unparseFloatVal() \n");
       // curprint ( "std::numeric_limits<float>::infinity()";
          curprint ( "__builtin_huge_valf()");
        }
       else
        {
       // Test for NaN value (famous test of to check for equality) or check for C++ definition of NaN.
       // We detect C99 and C "__NAN__" in EDG, but translate to backend specific builtin function.
          if ((float_value != float_value) || (float_value == std::numeric_limits<float>::quiet_NaN()) )
             {
            // curprint ( "std::numeric_limits<float>::quiet_NaN()";
               curprint ( "__builtin_nanf (\"\")");
             }
            else
             {
               if (float_value == std::numeric_limits<float>::signaling_NaN())
                  {
                 // curprint ( "std::numeric_limits<float>::signaling_NaN()";
                    curprint ( "__builtin_nansf (\"\")");
                  }
                 else
                  {
                 // typical case!
                 // curprint ( float_val->get_value();
                 // AS (11/08/2005) add support for values as string
                    if (float_val->get_valueString() == "") {
                      curprint ( tostring(float_value) );
                      if (!info.get_user_defined_literal()) {
                        curprint ( "L" );
                      }
                    } else {
                      std::string strVal = float_val->get_valueString();
                      if (info.get_user_defined_literal() && ( strVal.at(strVal.length() - 1) == 'f' || strVal.at(strVal.length() - 1) == 'F' ) ) {
                        strVal = strVal.substr(0, strVal.length() - 1);
                      }
                      curprint (strVal);
                    }
                  }
             }
        }
   }

#if 0
// DQ: This is no longer used (we use properties of the iostream to formate floating point now).
bool
Unparse_ExprStmt::zeroRemainder( long double doubleValue )
   {
     int integerValue = (int)doubleValue;
     double remainder = doubleValue - (double)integerValue;

     return (remainder == 0) ? true : false;
   }
#endif

#if 0
void
Unparse_ExprStmt::unparseDoubleVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgDoubleVal* dbl_val = isSgDoubleVal(expr);
     ASSERT_not_null(dbl_val);

#if 0
     printf ("Inside of unparseDblVal = %p \n",dbl_val);
     dbl_val->get_file_info()->display("unparseDblVal");
#endif

  // os->setf(ios::showpoint);
  // curprint ( dbl_val->get_value();
  // curprint ( setiosflags(ios::showpoint) + setprecision(4) + dbl_val->get_value();

     setiosflags(ios::showpoint);

  // DQ (10/16/2004): Not sure what 4 implies, but we get 16 digits after the decimal 
  // point so it should be fine (see test2004_114.C)!
     setprecision(4);

  // curprint ( dbl_val->get_value();
  // os->unsetf(ios::showpoint);

  // DQ (10/18/2005): Need to handle C code which cannot use C++ mechanism to specify 
  // infinity, quiet NaN, and signaling NaN values.
     double double_value = dbl_val->get_value();
     if (double_value == std::numeric_limits<double>::infinity())
        {
       // printf ("Infinite value found as value in unparseFloatVal() \n");
       // curprint ( "std::numeric_limits<double>::infinity()";
          curprint ( "__builtin_huge_val()");
        }
       else
        {
       // Test for NaN value (famous test of to check for equality) or check for C++ definition of NaN.
       // We detect C99 and C "__NAN__" in EDG, but translate to backend specific builtin function.
          if ( (double_value != double_value) || (dbl_val->get_value() == std::numeric_limits<double>::quiet_NaN()) )
             {
            // curprint ( "std::numeric_limits<double>::quiet_NaN()";
               curprint ( "__builtin_nan (\"\")");
             }
            else
             {
               if (double_value == std::numeric_limits<double>::signaling_NaN())
                  {
                 // curprint ( "std::numeric_limits<double>::signaling_NaN()";
                    curprint ( "__builtin_nans (\"\")");
                  }
                 else
                  {
                 // typical case!
                 // curprint ( dbl_val->get_value();
                 // AS (11/08/2005) add support for values as string
                    if (dbl_val->get_valueString() == "")
                         curprint ( tostring(double_value));
                      else
                         curprint ( dbl_val->get_valueString());
                  }
             }
        }
   }

#endif

void
Unparse_ExprStmt::unparseLongDoubleVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgLongDoubleVal* longdbl_val = isSgLongDoubleVal(expr);
     ASSERT_not_null(longdbl_val);

  // curprint ( longdbl_val->get_value();

  // DQ (10/18/2005): Need to handle C code which cannot use C++ mechanism to specify 
  // infinity, quiet NaN, and signaling NaN values.
     long double longDouble_value = longdbl_val->get_value();
     if (longDouble_value == std::numeric_limits<long double>::infinity())
        {
       // printf ("Infinite value found as value in unparseFloatVal() \n");
       // curprint ( "std::numeric_limits<long double>::infinity()";
          curprint ( "__builtin_huge_vall()");
        }
       else
        {
       // Test for NaN value (famous test of to check for equality) or check for C++ definition of NaN.
       // We detect C99 and C "__NAN__" in EDG, but translate to backend specific builtin function.
          if ( (longDouble_value != longDouble_value) || (longDouble_value == std::numeric_limits<long double>::quiet_NaN()) )
             {
            // curprint ( "std::numeric_limits<long double>::quiet_NaN()";
               curprint ( "__builtin_nanl (\"\")");
             }
            else
             {
               if (longDouble_value == std::numeric_limits<long double>::signaling_NaN())
                  {
                 // curprint ( "std::numeric_limits<long double>::signaling_NaN()";
                    curprint ( "__builtin_nansl (\"\")");
                  }
                 else
                  {
                 // typical case!
                 // curprint ( longdbl_val->get_value();
                 // AS (11/08/2005) add support for values as string
                    if (longdbl_val->get_valueString() == "") {
                      curprint ( tostring(longDouble_value) );
                      if (!info.get_user_defined_literal()) {
                        curprint ( "L" );
                      }
                    } else {
                      std::string strVal = longdbl_val->get_valueString();
                      if (info.get_user_defined_literal() && ( strVal.at(strVal.length() - 1) == 'l' || strVal.at(strVal.length() - 1) == 'L' ) ) {
                        strVal = strVal.substr(0, strVal.length() - 1);
                      }
                      curprint (strVal);
                    }
                  }
             }
        }
   }

void
Unparse_ExprStmt::unparseComplexVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgComplexVal* complex_val = isSgComplexVal(expr);
     ASSERT_not_null(complex_val);

     if (complex_val->get_valueString() != "") { // Has string
       curprint (complex_val->get_valueString());
     } else if (complex_val->get_real_value() == NULL) { // Pure imaginary
       curprint ("(");
       unparseValue(complex_val->get_imaginary_value(), info);
       curprint (" * 1.0i)");
     } else { // Complex number
       curprint ("(");
       unparseValue(complex_val->get_real_value(), info);
       curprint (" + ");
       unparseValue(complex_val->get_imaginary_value(), info);
       curprint (" * 1.0i)");
     }
   }

void 
Unparse_ExprStmt::unparseUpcThreads(SgExpression* expr, SgUnparse_Info& info)
   {
     SgUpcThreads* upc_threads = isSgUpcThreads(expr);
     ASSERT_not_null(upc_threads);

     curprint ("THREADS ");
   }

void 
Unparse_ExprStmt::unparseUpcMythread(SgExpression* expr, SgUnparse_Info& info)
   {
     SgUpcMythread* upc_mythread = isSgUpcMythread(expr);
     ASSERT_not_null(upc_mythread);

     curprint ("MYTHREAD ");
   }


void
Unparse_ExprStmt::unparseTypeTraitBuiltinOperator(SgExpression* expr, SgUnparse_Info& info)
   {
     SgTypeTraitBuiltinOperator* operatorExp = isSgTypeTraitBuiltinOperator(expr);
     ASSERT_not_null(operatorExp);

     string functionNameString = operatorExp->get_name();
     curprint(functionNameString);

#if 0
     printf ("In unparseTypeTraitBuiltinExp(): functionNameString = %s expr = %p = %s \n",functionNameString.c_str(),expr,expr->class_name().c_str());
#endif

     ROSE_ASSERT(operatorExp->get_args().empty() == false);

     SgNodePtrList& list = operatorExp->get_args();
     SgNodePtrList::iterator operand = list.begin();
     curprint("(");
     while (operand != list.end())
        {
#if 0
          printf ("   --- TOP operand = %p = %s \n",*operand,(*operand)->class_name().c_str());
#endif
#if 0
          (*operand)->get_file_info()->display("opertor argument");
#endif
       // DQ (4/24/2013): Moved this to be ahead so that the unparseArg value would be associated with the current argument.
          if (operand != list.begin())
             {
               curprint(","); 
             }

          SgType*       type       = isSgType(*operand);
          SgExpression* expression = isSgExpression(*operand);
#if 0
          printf ("   --- operand = %p = %s \n",*operand,(*operand)->class_name().c_str());
#endif
       // DQ (7/13/2013): Build a new SgUnparse_Info so that we can skip passing on any existing referenceNode for name qualification.
       // We need to debug name qualification seperately, if it is required, likely it could be fore any referenced types.
          SgUnparse_Info newinfo(info);
          newinfo.set_reference_node_for_qualification(operatorExp);
          ASSERT_not_null(newinfo.get_reference_node_for_qualification());

          if (type != NULL)
             {
            // unp->u_type->unparseType(type,newinfo);

               newinfo.set_SkipClassDefinition();
            // newinfo.set_SkipClassSpecifier();
               newinfo.set_SkipEnumDefinition();

               newinfo.set_isTypeFirstPart();
#if 0
               printf ("In unparseTypeTraitBuiltinOperator(): isTypeFirstPart: sizeof_op->get_operand_type() = %p = %s \n",type,type->class_name().c_str());
               curprint ("/* In unparseTypeTraitBuiltinOperator(): isTypeFirstPart \n */ ");
#endif
               unp->u_type->unparseType(type, newinfo);
               newinfo.set_isTypeSecondPart();
#if 0
               printf ("In unparseTypeTraitBuiltinOperator(): isTypeSecondPart: type = %p = %s \n",type,type->class_name().c_str());
               curprint ("/* In unparseTypeTraitBuiltinOperator(): isTypeSecondPart \n */ ");
#endif
               unp->u_type->unparseType(type, newinfo);

               newinfo.unset_isTypeFirstPart();
               newinfo.unset_isTypeSecondPart();
             }
            else
             {
#if 1
            // DQ (3/24/2015): Added case of "__builtin_offsetof" to make it consistant with the change in the EDG/ROSE translation.
            // DQ (3/19/2015): For the case of the __offsetof() builtin function we have to avoid output of the structure (e.g. "(0*).field").
            // unparseExpression(expression,info);
            // if (functionNameString == "__offsetof")
               if (functionNameString == "__offsetof" || functionNameString == "__builtin_offsetof")
                  {
#if 1
                 // DQ (3/25/2015): Develop new way to supress output of "(0*)" in "(0*).field".
                 // This is the more general form required for test2013_104.c "offsetof(zip_header_t, formatted.extra_len)".
                    SgUnparse_Info info2(info);
                    info2.set_skipCompilerGeneratedSubExpressions();
#if 0
                    printf ("In unparseTypeTraitBuiltinExp(): set skipCompilerGeneratedSubExpressions flag \n");
#endif
                    ROSE_ASSERT(info2.skipCompilerGeneratedSubExpressions() == true);

                    unparseExpression(expression,info2);
#else
                 // This should be the 2nd operand.
                    ROSE_ASSERT(operand != list.begin());
#error "DEAD CODE!"
                    SgDotExp* dotExp = isSgDotExp(expression);
                    ASSERT_not_null(dotExp);
                    SgExpression* field = dotExp->get_rhs_operand();

                 // Output the data member field only.
                    unparseExpression(field,info);
#endif
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }
                 else
                  {
                 // Original code.
                    unparseExpression(expression,info);
                  }
#else
               printf ("In unparseTypeTraitBuiltinExp(): Skipping special conversion for functionNameString == __offsetof \n");
#error "DEAD CODE!"
               unparseExpression(expression,info);
#endif
             }

          operand++;
        }

     curprint(")");
   }




// DQ (8/13/2007): Moved to common (language independent) base class
// JJW Added a lot of these back in

//-----------------------------------------------------------------------------------
//  void Unparse_ExprStmt::unparseFuncCall 
//  
//  This function is called whenever we unparse a function call. It is divided up 
//  into two parts. The first part unparses the function call and its arguments 
//  using an "in-order" tree traversal method. This is done when we have a binary 
//  operator overloading function and the operator overloading option is turned off.
//  The second part unparses the function call directly in a list-like manner. This 
//  is done for non-operator function calls, or when the operator overloading option
//  is turned on. 
//-----------------------------------------------------------------------------------
void
Unparse_ExprStmt::unparseFuncCall(SgExpression* expr, SgUnparse_Info& info)
   {
#define DEBUG_FUNCTION_CALL 0

#if DEBUG_FUNCTION_CALL
     printf ("In Unparse_ExprStmt::unparseFuncCall(): expr = %p unp->opt.get_overload_opt() = %s \n",expr,(unp->opt.get_overload_opt() == true) ? "true" : "false");
     curprint ( "\n/* In Unparse_ExprStmt::unparseFuncCall " + StringUtility::numberToString(expr) + " */ \n");
#endif

     SgFunctionCallExp* func_call = isSgFunctionCallExp(expr);
     ASSERT_not_null(func_call);
     SgUnparse_Info newinfo(info);
     bool needSquareBrackets = false;

#if DEBUG_FUNCTION_CALL
     curprint       ("/* func_call->get_function()                   = " + func_call->get_function()->class_name() + " */\n");
     curprint(string("/* func_call->get_uses_operator_syntax()       = ") + ((func_call->get_uses_operator_syntax() == true) ? "true" : "false") + " */\n");
     curprint(string("/* unp->opt.get_overload_opt()                 = ") + ((unp->opt.get_overload_opt() == true) ? "true" : "false") + " */\n");
  // curprint("/* isBinaryOperator(func_call->get_function()) = " + ((unp->u_sage->isBinaryOperator(func_call->get_function()) == true) ? "true" : "false") + " */\n");
#endif

  // DQ (4/8/2013): Added support for unparsing "operator+(x,y)" in place of "x+y".  This is 
  // required in places even though we have historically defaulted to the generation of the 
  // operator syntax (e.g. "x+y"), see test2013_100.C for an example of where this is required.
     bool uses_operator_syntax = func_call->get_uses_operator_syntax();

#if DEBUG_FUNCTION_CALL
     printf ("In Unparse_ExprStmt::unparseFuncCall(): (before test for conversion operator) uses_operator_syntax = %s \n",uses_operator_syntax == true ? "true" : "false");
     curprint(string("/* In unparseFuncCall(): (before test for conversion operator) uses_operator_syntax     = ") + (uses_operator_syntax ? "true" : "false") + " */\n");
#endif

#if DEBUG_FUNCTION_CALL
  // DQ (4/8/2013): Test to make sure this is not presently required in our regression tests.
     if (uses_operator_syntax == true)
        {
          printf ("In Unparse_ExprStmt::unparseFuncCall(): Detected uses_operator_syntax == true \n");
       // ROSE_ASSERT(false);
        }
#endif

#if DEBUG_FUNCTION_CALL
     printf ("func_call->get_function() = %p = %s \n",func_call->get_function(),func_call->get_function()->class_name().c_str());
#endif

     bool suppress_implicit_conversion_operator = false;

     SgDotExp* dotExp = isSgDotExp(func_call->get_function());
     if (dotExp != NULL)
        {
          SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(dotExp->get_rhs_operand());
          if (memberFunctionRefExp != NULL)
             {
            // Operator syntax inplies output of generated code as of "B b; b.A::operator+(b);" instead of "B b; b+b;"
            // For conversion operators the form would be "B b; return b.operator A();" instead of "B b; return A(b);"

            // DQ (8/28/2014): It is a bug in GNU 4.4.7 to use the operator syntax of a user-defined conversion operator.
            // So we have to detect such operators and then detect if they are implicit then mark them to use the operator 
            // syntax plus supress them from being output.  We might alternatively go directly to supressing them from being
            // output, except that this is might be more complex for the operator syntax unparsing (I think).

               SgFunctionSymbol* functionSymbol = memberFunctionRefExp->get_symbol();
               ASSERT_not_null(functionSymbol);
               SgFunctionDeclaration* functionDeclaration = functionSymbol->get_declaration();
               ASSERT_not_null(functionDeclaration);
            // SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(functionDeclaration);
            // ASSERT_not_null(memberFunctionDeclaration);

               bool is_compiler_generated = func_call->isCompilerGenerated();

            // If operator form is specified then turn it off.
            // if (uses_operator_syntax == true)
                  {
                    if (functionDeclaration->get_specialFunctionModifier().isConversion() == true)
                       {
#if DEBUG_FUNCTION_CALL
                         printf ("In Unparse_ExprStmt::unparseFuncCall(): Detected a conversion operator! \n");
#endif
                      // DQ (8/28/2014): Force output of generated code using the operator syntax, plus supress the output if is_compiler_generated == true.
                      // uses_operator_syntax = false;

                         if (is_compiler_generated == true)
                            {
#if DEBUG_FUNCTION_CALL
                              printf ("In Unparse_ExprStmt::unparseFuncCall(): Detected is_compiler_generated == true for conversion operator! \n");
#endif
                              suppress_implicit_conversion_operator = true;
#if 0
                              printf ("Exiting as a test! \n");
                              ROSE_ASSERT(false);
#endif
                            }
#if 0
                         printf ("Exiting as a test! \n");
                         ROSE_ASSERT(false);
#endif
                       }
                  }
             }
        }

#if DEBUG_FUNCTION_CALL
     printf ("In Unparse_ExprStmt::unparseFuncCall(): (after test for conversion operator) uses_operator_syntax = %s \n",uses_operator_syntax == true ? "true" : "false");
     curprint(string("/* In unparseFuncCall(): (after test for conversion operator) uses_operator_syntax     = ") + (uses_operator_syntax ? "true" : "false") + " */\n");
#endif

#if DEBUG_FUNCTION_CALL
  // DQ (11/16/2013): This need not be a SgFunctionRefExp.
     SgFunctionRefExp* func_ref = isSgFunctionRefExp(func_call->get_function());
  // ASSERT_not_null(func_ref);
  // ASSERT_not_null(func_ref->get_symbol());
  // printf ("Function name = %s \n",func_ref->get_symbol()->get_name().str());
     if (func_ref != NULL)
        {
          ASSERT_not_null(func_ref->get_symbol());
          printf ("Function name = %s \n",func_ref->get_symbol()->get_name().str());
        }
       else
        {
       // If this is not a SgFunctionRefExp, then look for a member function reference via a SgDotExp or SgArrowExp.
        }
#endif
#if DEBUG_FUNCTION_CALL
     printf ("isBinaryOperator(func_call->get_function())       = %s \n",unp->u_sage->isBinaryOperator(func_call->get_function()) ? "true" : "false");
     printf ("isSgDotExp(func_call->get_function())             = %s \n",isSgDotExp(func_call->get_function()) ? "true" : "false");
     printf ("isSgArrowExp(func_call->get_function())           = %s \n",isSgArrowExp(func_call->get_function()) ? "true" : "false");

     printf ("isUnaryOperatorPlus(func_call->get_function())    = %s \n",unp->u_sage->isUnaryOperatorPlus(func_call->get_function()) ? "true" : "false");
     printf ("isUnaryOperatorMinus(func_call->get_function())   = %s \n",unp->u_sage->isUnaryOperatorMinus(func_call->get_function()) ? "true" : "false");

     printf ("isUnaryOperator(func_call->get_function())        = %s \n",unp->u_sage->isUnaryOperator(func_call->get_function()) ? "true" : "false");
     printf ("isUnaryPostfixOperator(func_call->get_function()) = %s \n",unp->u_sage->isUnaryPostfixOperator(func_call->get_function()) ? "true" : "false");
#endif

#if DEBUG_FUNCTION_CALL
     printf ("WARNING: unparseOperatorSyntax and uses_operator_syntax are functionally redundant declarations \n");
#endif

  // DQ (6/17/2007): Turn off the generation of "B b; b+b" in favor of "B b; b.A::operator+(b)
  // when A::operator+(A) is called instead of B::operator+(A).  See test2007_73.C for an example.
  // bool unparseOperatorSyntax = false;

  // if ( !unp->opt.get_overload_opt() && isBinaryOperator(func_call->get_function()) && (isSgDotExp(func_call->get_function()) != NULL) || (isSgArrowExp(func_call->get_function()) != NULL) )
  // if ( (unp->opt.get_overload_opt() == false) && ( (isSgDotExp(func_call->get_function()) != NULL) || (isSgArrowExp(func_call->get_function()) != NULL) ) )
  // if ( ((unp->opt.get_overload_opt() == false) && (uses_operator_syntax == false)) && ( (isSgDotExp(func_call->get_function()) != NULL) || (isSgArrowExp(func_call->get_function()) != NULL) ) )
     if ( ((unp->opt.get_overload_opt() == false) && (uses_operator_syntax == true)) && ( (isSgDotExp(func_call->get_function()) != NULL) || (isSgArrowExp(func_call->get_function()) != NULL) ) )
        {
#if DEBUG_FUNCTION_CALL
          printf ("Found case to investigate for generation of \"B b; b.A::operator+(b)\" instead of \"B b; b+b\" \n");
#endif
          SgBinaryOp* binaryOperator = isSgBinaryOp(func_call->get_function());
          ASSERT_not_null(binaryOperator);

          SgExpression* lhs = binaryOperator->get_lhs_operand();
          SgExpression* rhs = binaryOperator->get_rhs_operand();
#if DEBUG_FUNCTION_CALL
          printf ("lhs = %p = %s \n",lhs,lhs->class_name().c_str());
          printf ("rhs = %p = %s \n",rhs,rhs->class_name().c_str());
#endif
          SgMemberFunctionRefExp* memberFunctionRef = isSgMemberFunctionRefExp(rhs);
          if (memberFunctionRef != NULL)
             {
               SgSymbol* memberFunctionSymbol = memberFunctionRef->get_symbol();
               ASSERT_not_null(memberFunctionSymbol);
#if DEBUG_FUNCTION_CALL
               printf ("member function symbol = %p name = %s \n",memberFunctionRef->get_symbol(),memberFunctionRef->get_symbol()->get_name().str());
               printf ("lhs->get_type() = %s \n",lhs->get_type()->class_name().c_str());
#endif
               SgClassType* classType = isSgClassType(lhs->get_type());
               SgClassDeclaration* lhsClassDeclaration = NULL;
               SgClassDefinition*  lhsClassDefinition  = NULL;
               if (classType != NULL)
                  {
#if DEBUG_FUNCTION_CALL
                    printf ("classType->get_declaration() = %p = %s \n",classType->get_declaration(),classType->get_declaration()->class_name().c_str());
#endif
                    lhsClassDeclaration = isSgClassDeclaration(classType->get_declaration());
                    ASSERT_not_null(lhsClassDeclaration);
#if DEBUG_FUNCTION_CALL
                    printf ("lhs classDeclaration = %p = %s \n",lhsClassDeclaration,lhsClassDeclaration->get_name().str());
#endif
                    lhsClassDefinition = lhsClassDeclaration->get_definition();
#if OUTPUT_HIDDEN_LIST_DATA
                    outputHiddenListData (lhsClassDefinition);
#endif
                  }
                 else
                  {
#if PRINT_DEVELOPER_WARNINGS
                 // DQ (10/22/2007): This is part of incomplete debugging of a famous detail (name qualification 
                 // for operators). Only output this messag for developers.

                 // This is the case of a member function call off of the "this" pointer, see test2007_124.C.
                    printf ("lhs is not a classType lhs->get_type() = %p = %s \n",lhs->get_type(),lhs->get_type()->class_name().c_str());
#endif
                  }

               SgClassDefinition* functionClassDefinition = isSgClassDefinition(memberFunctionRef->get_symbol()->get_scope());
               ASSERT_not_null(functionClassDefinition);
#if DEBUG_FUNCTION_CALL
               printf ("member function scope (class = %p = %s) \n",functionClassDefinition,functionClassDefinition->class_name().c_str());
#endif
#if OUTPUT_HIDDEN_LIST_DATA
               outputHiddenListData (functionClassDefinition);
#endif

               SgClassDeclaration* functionClassDeclaration = isSgClassDeclaration(functionClassDefinition->get_declaration());
#if DEBUG_FUNCTION_CALL
               printf ("functionClassDeclaration = %p = %s \n",functionClassDeclaration,functionClassDeclaration->get_name().str());
#endif
               if (lhsClassDeclaration == NULL)
                  {
                 // printf ("lhsClassDeclaration = %p = %s \n",lhsClassDeclaration,lhsClassDeclaration->get_name().str());
                 // lhsClassDeclaration->get_startOfConstruct()->display("lhsClassDeclaration");
#if PRINT_DEVELOPER_WARNINGS
                    printf ("Error: lhsClassDeclaration == NULL, lhs = %p \n",lhs);
                    lhs->get_startOfConstruct()->display("lhs");
#endif
                  }
            // ASSERT_not_null(lhsClassDeclaration);
            // ASSERT_not_null(functionClassDeclaration);

               if (lhsClassDeclaration != NULL && functionClassDeclaration != NULL)
                  {
                    if (lhsClassDeclaration->get_firstNondefiningDeclaration() != functionClassDeclaration->get_firstNondefiningDeclaration())
                       {
#if 0
                         if (SgProject::get_verbose() > 0)
                            {
                              printf ("Warning: lhs and member function from different classes (linked though class derivation) \n");
                            }
                         curprint ( "/* Warning: lhs and member function from different classes (linked though class derivation) */\n");
#endif

                         if (SgProject::get_verbose() > 0)
                            {
                              printf ("lhsClassDefinition = %p functionClassDefinition = %p \n",lhsClassDefinition,functionClassDefinition);
                            }
                         ROSE_ASSERT(lhsClassDefinition != NULL || functionClassDefinition != NULL);

                         set<SgSymbol*> & hiddenList = (lhsClassDefinition != NULL) ? lhsClassDefinition->get_hidden_declaration_list() : functionClassDefinition->get_hidden_declaration_list();
                         if (SgProject::get_verbose() > 0)
                            {
                              printf ("Looking for symbol = %p \n",memberFunctionSymbol);
                            }
                         set<SgSymbol*>::iterator hiddenDeclaration = hiddenList.find(memberFunctionSymbol);
                         if ( hiddenDeclaration != hiddenList.end() )
                            {
#if DEBUG_FUNCTION_CALL
                              printf ("Warning: lhs class hidding derived class member function call (skip setting uses_operator_syntax == true) \n");
#endif
#if DEBUG_FUNCTION_CALL
                              curprint ( "/* Warning: lhs class hidding derived class member function call */\n ");
#endif
                           // unparseOperatorSyntax = true;
                           // uses_operator_syntax = true;
                            }
                       }
                  }
                 else
                  {
#if PRINT_DEVELOPER_WARNINGS
                    printf ("Warning: either lhsClassDeclaration == NULL || functionClassDeclaration == NULL, so we need more work to compute if the operator syntax is required \n");
#endif
                  }
#if DEBUG_FUNCTION_CALL
            // printf ("Warning: name qualification required = %s \n",unparseOperatorSyntax ? "true" : "false");
               printf ("Warning: name qualification required = %s \n",uses_operator_syntax ? "true" : "false");
#endif
             }
            else
             {
#if DEBUG_FUNCTION_CALL
               printf ("rhs was not a SgMemberFunctionRefExp \n");
            // ROSE_ASSERT(false);
#endif
             }

       // printf ("Exiting as part of testing \n");
       // ROSE_ASSERT(false);
        }

#if DEBUG_FUNCTION_CALL
     printf ("In unparseFuncCall(): unp->u_sage->isBinaryOperator(func_call->get_function() = %p = %s ) = %s \n",
          func_call->get_function(),func_call->get_function()->class_name().c_str(),unp->u_sage->isBinaryOperator(func_call->get_function()) ? "true" : "false");
#endif

  // FIRST PART
  // check if this is an binary operator overloading function and if the overloading 
  // option is off. If so, we traverse using "in-order" tree traversal. However, do 
  // not enter this segment if we have a dot expression. Dot expressions are handled 
  // by the second part. 
  // if (!unp->opt.get_overload_opt() && unp->u_sage->isBinaryOperator(func_call->get_function()) && !(isSgDotExp(func_call->get_function())) && !(isSgArrowExp(func_call->get_function())))
  // if (!unp->opt.get_overload_opt() && (uses_operator_syntax == false) && unp->u_sage->isBinaryOperator(func_call->get_function()) && !(isSgDotExp(func_call->get_function())) && !(isSgArrowExp(func_call->get_function())))
     if (!unp->opt.get_overload_opt() && (uses_operator_syntax == true) && unp->u_sage->isBinaryOperator(func_call->get_function()) && !(isSgDotExp(func_call->get_function())) && !(isSgArrowExp(func_call->get_function())))
        {
          unp->u_debug->printDebugInfo("in FIRST PART of unparseFuncCall", true);
#if DEBUG_FUNCTION_CALL
          printf ("output 1st part (without syntax sugar) \n");
          curprint ( " /* output 1st part (without syntax sugar) */ ");
#endif
          ASSERT_not_null(func_call->get_args());
          SgExpressionPtrList& list = func_call->get_args()->get_expressions();
#if DEBUG_FUNCTION_CALL
          printf ("In unparseFuncCall(): argument list size = %ld \n",list.size());
#endif
          SgExpressionPtrList::iterator arg = list.begin();
          if (arg != list.end())
             {
               newinfo.set_nested_expression();

            // printf ("output function argument (left) \n");

            // unparse the lhs operand
               unp->u_debug->printDebugInfo("left arg: ", false);
               unparseExpression((*arg), newinfo);
            // unparse the operator

            // DQ (6/21/2011): Added support for name qualification.
               info.set_reference_node_for_qualification(func_call->get_function());
               ASSERT_not_null(info.get_reference_node_for_qualification());
#if DEBUG_FUNCTION_CALL
               curprint ( "\n/* In unparseFuncCall(): 1st part BEFORE: unparseExpression(func_call->get_function(), info); */ \n");
#endif
               unparseExpression(func_call->get_function(), info);
#if DEBUG_FUNCTION_CALL
               curprint ( "\n/* In unparseFuncCall(): 1st part AFTER: unparseExpression(func_call->get_function(), info); */ \n");
#endif
               info.set_reference_node_for_qualification(NULL);

               arg++;

            // unparse the rhs operand
               unp->u_debug->printDebugInfo("right arg: ", false);
#if DEBUG_FUNCTION_CALL
               curprint ( "\n/* In unparseFuncCall(): 1st part BEFORE: right arg: unparseExpression(*arg, info); */ \n");
#endif
            // DQ (5/6/2007): Added assert, though this was only a problem when handling unary minus implemented as a non-member function
               ROSE_ASSERT (arg != list.end());
               unparseExpression((*arg), newinfo);
#if 0
            // DQ (8/29/2014): This was a mistake.
            // DQ (8/29/2014): This fails for test2014_172.C.
            // ROSE_ASSERT (arg != list.end());
               if (arg != list.end())
                  {
                    unparseExpression((*arg), newinfo);
                  }
                 else
                  {
                    printf ("WARNING: arg == list.end() in unparseFuncCall() \n");
                  }
#endif
#if DEBUG_FUNCTION_CALL
               curprint ("\n/* In unparseFuncCall(): 1st part AFTER: right arg: unparseExpression(*arg, info); */ \n");
#endif
               newinfo.unset_nested_expression();

            // printf ("DONE: output function argument (right) \n");
             }
#if DEBUG_FUNCTION_CALL
          curprint ("\n/* Leaving processing first part in unparseFuncCall */ \n");
#endif
        }
       else
        {
       // SECOND PART
       // this means that we have an unary operator overloading function, a non-operator
       // overloading function, or that the overloading option was turned on. 
          unp->u_debug->printDebugInfo("in SECOND PART of unparseFuncCall", true);
          bool print_paren = true;

       // DQ (2/20/2005): By default always output the function arguments (only in the case of 
       // the overloaded prefix/postfix increment/decrement operators do we supress their output).
          bool printFunctionArguments = true;

       // if (unp->opt.get_overload_opt())
          if (unp->opt.get_overload_opt() || (uses_operator_syntax == false))
             {
               info.set_nested_expression();
             }
#if DEBUG_FUNCTION_CALL
          printf ("output 2nd part func_call->get_function() = %s \n",func_call->get_function()->class_name().c_str());
          printf ("suppress_implicit_conversion_operator = %s \n",suppress_implicit_conversion_operator ? "true" : "false");
          curprint ("/* output 2nd part  func_call->get_function() = " + func_call->get_function()->class_name() + " */ \n");
          curprint ( string("/* suppress_implicit_conversion_operator = ") + (uses_operator_syntax == true ? "true" : "false") + " */ \n");
#endif

       // DQ (8/29/2014): Adding support to supress output of implicit user-defined conversion operators.
          if (suppress_implicit_conversion_operator == false)
             {
       //
       // Unparse the function first.
       //
          SgUnparse_Info alt_info(info);
       // unparseExpression(func_call->get_function(), info);

       // DQ (6/13/2007): First set to NULL then to the correct value (this allows us to have checking which 
       // detects the overwriting of pointer values generally, but it is not relavant in this case).
          alt_info.set_current_function_call(NULL);
          alt_info.set_current_function_call(func_call);

       // DQ (6/21/2011): Added support for name qualification.
          alt_info.set_reference_node_for_qualification(func_call->get_function());
          ASSERT_not_null(alt_info.get_reference_node_for_qualification());
#if DEBUG_FUNCTION_CALL
          curprint ( "\n/* In unparseFuncCall(): 2nd part BEFORE: unparseExpression(func_call->get_function(), info); */ \n");
#endif
#if DEBUG_FUNCTION_CALL
          printf ("uses_operator_syntax                                           = %s \n",uses_operator_syntax ? "true" : "false");
          printf ("unp->u_sage->isUnaryOperator(func_call->get_function())        = %s \n",unp->u_sage->isUnaryOperator(func_call->get_function()) ? "true" : "false");
          printf ("unp->u_sage->isUnaryPostfixOperator(func_call->get_function()) = %s \n",unp->u_sage->isUnaryPostfixOperator(func_call->get_function()) ? "true" : "false");
#endif

       // DQ (2/2/2018): Handle the case of a non-postfix operator.
       // unparseExpression(func_call->get_function(), alt_info);
          if ( ! ( (uses_operator_syntax == true) &&
                   (unp->u_sage->isUnaryOperator(func_call->get_function()) == true) && 
                   (unp->u_sage->isUnaryPostfixOperator(func_call->get_function()) == true) ))
             {
#if DEBUG_FUNCTION_CALL
            // printf ("func_call->get_function()->get_name()                          = %s \n",func_call->get_function()->get_name().str());
               printf ("uses_operator_syntax                                           = %s \n",uses_operator_syntax ? "true" : "false");
               printf ("unp->u_sage->isUnaryOperator(func_call->get_function())        = %s \n",unp->u_sage->isUnaryOperator(func_call->get_function()) ? "true" : "false");
               printf ("unp->u_sage->isUnaryPostfixOperator(func_call->get_function()) = %s \n",unp->u_sage->isUnaryPostfixOperator(func_call->get_function()) ? "true" : "false");
               printf ("func_call->get_function()                                      = %p = %s \n",func_call->get_function(),func_call->get_function()->class_name().c_str());
               printf ("###################### Calling unparseExpression(func_call->get_function(), alt_info); \n");
#endif
               unparseExpression(func_call->get_function(), alt_info);

#if DEBUG_FUNCTION_CALL
               printf ("###################### DONE: Calling unparseExpression(func_call->get_function(), alt_info); \n");
#endif
             }
#if DEBUG_FUNCTION_CALL
          curprint ( "\n/* In unparseFuncCall(): 2nd part AFTER: unparseExpression(func_call->get_function(), info); */ \n");
#endif
#if 0
          func_call->get_function()->get_file_info()->display("In unparse function call: debug");
#endif
          alt_info.set_reference_node_for_qualification(NULL);

#if DEBUG_FUNCTION_CALL
          curprint ( " /* after output func_call->get_function() */ ");
#endif

       // if (unp->opt.get_overload_opt())
          if (unp->opt.get_overload_opt() || (uses_operator_syntax == false))
               info.unset_nested_expression();

#if 0
       // check if we have an unary operator or an overloaded arrow operator and the 
       // operator overloading option is turned off. If so, then we can return right 
       // away because there are no arguments to unparse. Sage provides default arguments
       // for unary operators, such as (), or (0). We want to avoid printing this if
       // the conditional is true.
          if (!unp->opt.get_overload_opt())
             {
               if (RemoveArgs(func_call->get_function()))
                  {
                    printf ("$$$$$ Returning from Unparse_ExprStmt::unparseFuncCall prematurely (detected \"->\" operator, but not \"operator->()\") $$$$$ \n");
                    curprint ( " /* Returning from Unparse_ExprStmt::unparseFuncCall prematurely */ ");
                    return;
                  }
             }
#endif

          SgUnparse_Info newinfo(info);

       // now check if the overload option is off and that the function is dot binary
       // expression. If so, check if the rhs is an operator= overloading function (and 
       // that the function isn't preceded by a class name). If the operator= is preceded
       // by a class name ("<class>::operator=") then do not set print_paren to false.
       // If so, set print_paren to false, otherwise, set print_paren to true for
       // all other functions.
       //
       // [DT] 4/6/2000 -- Need to check for operator==, also, as well
       //      any other potential overloaded operator that having
       //      this paren would cause a problem.  e.g. in the case
       //      of operator==, we would get something like (x==)(y)
       //      where the paren at ==) comes from unparseBinaryExpr()
       //      and the paren at (y comes from here.
       //
       //      NOTE:  I went ahead and created isBinaryEqualityOperator()
       //      and put the check here.  But there needs to be a more
       //      thorough fix that handles operator<, operator>=, etc...
       //
       //      4/10/2000 -- Created isBinaryInequalityOperator() and
       //      isBinaryArithmeticOperator().  Thinking about simply
       //      creating an isBinaryOverloadedOperator().
       //
          SgBinaryOp* binary_op = isSgBinaryOp(func_call->get_function());
#if DEBUG_FUNCTION_CALL
          curprint ( string(" /* !unp->opt.get_overload_opt() && (uses_operator_syntax == true) = ") + ((!unp->opt.get_overload_opt() && (uses_operator_syntax == true)) ? "true" : "false") + " */ \n ");
          printf ("In unparseFuncCall(): binary_op = %p \n",binary_op);
          printf (" --- func_call->get_function() = %p = %s \n",func_call->get_function(),func_call->get_function()->class_name().c_str());
#endif
       // if (!unp->opt.get_overload_opt())
          if (!unp->opt.get_overload_opt() && (uses_operator_syntax == true))
             {
            // curprint ( "\n /* Unparse so as to suppress overloaded operator function names (generate short syntax) */ \n";
            // DQ (2/19/2005): Rewrote this case to be more general than just specific to a few operators
               SgExpression* rhs = NULL;
               if (binary_op != NULL) 
                  {
                    rhs = binary_op->get_rhs_operand();
                    ASSERT_not_null(rhs);
                  }
            // if ( binary_op != NULL && rhs->get_specialFunctionModifier().isOperator() && unp->u_sage->noQualifiedName(rhs) )

#if DEBUG_FUNCTION_CALL
               printf ("binary_op = %p rhs = %p \n",binary_op,rhs);
               if (rhs != NULL)
                  {
                    printf ("rhs       = %s \n",rhs->class_name().c_str());
                    printf ("binary_op = %s \n",binary_op->class_name().c_str());
                  }
               printf ("unp->u_sage->noQualifiedName(rhs) = %s \n",unp->u_sage->noQualifiedName(rhs) ? "true" : "false");
#endif

            // DQ (12/28/2005): I don't think this need be qualified to permit us to use the "[]" syntax, see test2005_193.C
            // if ( binary_op != NULL && unp->u_sage->noQualifiedName(rhs) )
               if ( binary_op != NULL )
                  {
                 // printf ("Found a binary operator without qualification \n");
                 // curprint ( "\n /* found a binary operator without qualification */ \n";
                    SgFunctionRefExp*       func_ref  = isSgFunctionRefExp(rhs);
                    SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(rhs);

                    if ( (func_ref != NULL) && func_ref->get_symbol()->get_declaration()->get_specialFunctionModifier().isOperator() )
                         print_paren = false;

                    if ( (mfunc_ref != NULL) && mfunc_ref->get_symbol()->get_declaration()->get_specialFunctionModifier().isOperator() )
                         print_paren = false;

                 // Liao, work around for bug 320, operator flag for *i is not set properly, 2/18/2009
                 // Please turn this code off when the bug is fixed!
                    if (mfunc_ref != NULL)
                       {
                         string name = mfunc_ref->get_symbol()->get_name().getString();
                         if (name=="operator*")
                            {
                              print_paren = false;
                              if (mfunc_ref->get_symbol()->get_declaration()->get_specialFunctionModifier().isOperator() ==false)
                                   cerr<<"unparseCxx_expresssions.C error: found a function named as operator* which is not set as isOperator! \n Fixed its unparsing here temporarily but please consult bug 320!"<<endl;
                            }
                       }

                 // DQ (2/20/2005) The operator()() is the parenthesis operator and for this case we do want to output "(" and ")"
                    if (unp->u_sage->isBinaryParenOperator(rhs) == true)
                         print_paren = true;

                 // DQ (2/20/2005): Merged code below with this case to simplify operator handling!
                 // printf ("isBinaryBracketOperator(rhs) = %s \n",isBinaryBracketOperator(rhs) ? "true" : "false");
                    if (unp->u_sage->isBinaryBracketOperator(rhs) == true)
                       {
                      // DQ (2/20/2005): Just as for operator()(), operator[]() needs the parens
                         print_paren = true;

                      // DQ (12/28/2005): This has to reproduce the same logic as in the unparseMFuncRef() function
                      // curprint ( " /* Newly handled case in unparser unparseFuncCall() */ ";
                         needSquareBrackets = true;
                      // Turn off parens in order to output [i] instead of [(i)].
                         print_paren = false;
                       }

                 // DQ (2/20/2005): This operator is special in C++ in that it take an integer parameter when called 
                 // using the explicit operator function form (e.g. "x.operator++()").  As decribed in C++:
                 //      "x.operator++(0)"  --> x++ (the postfix increment operator)
                 //      "x.operator++(1)"  --> ++x (the prefix increment operator)
                 // an analigious syntax controls the use of the prefix and postfix decrement operator.
                    if ( unp->u_sage->isUnaryIncrementOperator(rhs) || unp->u_sage->isUnaryDecrementOperator(rhs) )
                       {
                         printFunctionArguments = false;
                       }
                      else
                       {
                      // DQ (2/12/2019): We may have to explicitly detect the literal operators here!
#if 0
                         printf ("For literal operators we may have to set printFunctionArguments = false \n");
#endif
#if 0
                         ASSERT_not_null(func_call->get_function());
                         printf ("func_call->get_function() = %p = %s \n",func_call->get_function(),func_call->get_function()->class_name().c_str());
                         if ( unp->u_sage->isUnaryLiteralOperator(func_call->get_function()) == true )
                            {
#if 1
                              printf ("Found a literal operators: set printFunctionArguments == false \n");
#endif
                              printFunctionArguments = false;
                            }
#endif
                       }
                  }
                 else
                  {
                 // DQ (2/12/2019): Added this branch for when binary_op == NULL.
                    ROSE_ASSERT(binary_op == NULL);

                 // ASSERT_not_null(rhs);
                 // SgFunctionRefExp*       func_ref  = isSgFunctionRefExp(rhs);
                 // SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(rhs);

                    ASSERT_not_null(func_call->get_function());
                    SgFunctionRefExp*       func_ref  = isSgFunctionRefExp(func_call->get_function());
                    SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(func_call->get_function());

                 // DQ (2/12/2019): Adding support for C++11 uld operators.
                    if ( (func_ref != NULL) && func_ref->get_symbol()->get_declaration()->get_specialFunctionModifier().isUldOperator() )
                       {
                         print_paren = false;
                         newinfo.set_user_defined_literal(true);
                      // printFunctionArguments = false;
                       }

                 // DQ (2/12/2019): Adding support for C++11 uld operators.
                    if ( (mfunc_ref != NULL) && mfunc_ref->get_symbol()->get_declaration()->get_specialFunctionModifier().isUldOperator() )
                       {
                         print_paren = false;
                         newinfo.set_user_defined_literal(true);
                      // printFunctionArguments = false;
                       }
                  }
             }

       //
       // [DT] 3/30/2000 -- In the case of overloaded [] operators, 
       //      set a flag indicating that square brackets should be
       //      wrapped around the argument below.  This will
       //      result in the desired syntax in the unparsed code 
       //      as long as the unparseMFuncExpr() function knows better 
       //      than to output any of ".operator[]".
       //
       //      Q: Need to check unp->opt.get_overload_opt()?
       //
       // MK: Yes! We only need square brackets if
       //     1. unp->opt.get_overload_opt() is false (= keyword "operator" not required in the output), and
       //     2. we do not have to specify a qualifier; i.e., <classname>::<funcname>
       //     Otherwise, we print "operator[]" and need parenthesis "()" around the
       //     function argument.
       // DQ (12/10/2004): Skip this simplification if the lhs is a SgPointerDerefExp 
       // (i.e. "x->operator[](i)" should not be simplified to "x->[i]")
#if 0
          if (binary_op != NULL)
             {
               printf ("binary_op = %p = %s \n",binary_op,binary_op->sage_class_name());
               curprint ( "\n /* binary_op = " + StringUtility::numberToString(binary_op) + " = " + binary_op->sage_class_name() + " */ \n");
               printf ("binary_op->get_lhs_operand() = %p = %s \n",binary_op->get_lhs_operand(),binary_op->get_lhs_operand()->sage_class_name());
               curprint ( "\n /* binary_op->get_lhs_operand() = " + StringUtility::numberToString(binary_op->get_lhs_operand()) + " = " + binary_op->get_lhs_operand()->sage_class_name() + " */ \n");
               printf ("binary_op->get_rhs_operand() = %p = %s \n",binary_op->get_rhs_operand(),binary_op->get_rhs_operand()->sage_class_name());
               curprint ( "\n /* binary_op->get_rhs_operand() = " + StringUtility::numberToString(binary_op->get_rhs_operand()) + " = " + binary_op->get_rhs_operand()->sage_class_name() + " */ \n");
             }
#endif
#if 0
       // DQ (4/9/2013): need to tune output of redundant "()".
          if (uses_operator_syntax == false && (unp->u_sage->isOperator(binary_op->get_rhs_operand()) == true))
             {
               print_paren = false;
             }
#endif
          if ( needSquareBrackets)
             {
               curprint ( "[");
             }

       // now unparse the function's arguments
       // if (func_call->get_args() != NULL)
       //      printDebugInfo("unparsing arguments of function call", true);

#if DEBUG_FUNCTION_CALL
          curprint (string("\n /* Before preint paren in unparseFuncCall: print_paren = ") + (print_paren ? "true" : "false") + " */ \n");
#endif
          if (print_paren)
             {
#if DEBUG_FUNCTION_CALL
               curprint ("\n/* Unparse args in unparseFuncCall: opening */ \n"); 
#endif
               curprint ("(");
            // printDebugInfo("( from FuncCall", true);
             }

       // DQ (2/20/2005): Added case of (printFunctionArguments == true) to handle prefix/postfix increment/decrement 
       // overloaded operators (which take an argument to control prefix/postfix, but which should never be output
       // unless we are trying to reproduce the operator function call syntax e.g. "x.operator++(0)" or "x.operator++(1)").
          if ( (printFunctionArguments == true) && (func_call->get_args() != NULL) )
             {
               SgExpressionPtrList& list = func_call->get_args()->get_expressions();
               SgExpressionPtrList::iterator arg = list.begin();
               while (arg != list.end())
                  {
#if 0
                    (*arg)->get_file_info()->display("function call argument");
#endif

                 // DQ (4/24/2013): We want to avoid unparsing arguments present as a result of automatically instered 
                 // default arguments.  This improves the quality of the source-to-source translation.  However, it 
                 // might be that we can't just test for the argument marked as compiler generated and we might have 
                 // to explicitly makr it as being associated with a default argument, else a compiler generated
                 // cast might trigger the argument to not be output.  Need to test this.
                 // bool unparseArg = ((*arg)->get_file_info()->isCompilerGenerated() == false);
                    bool unparseArg = ((*arg)->get_file_info()->isDefaultArgument() == false);
#if DEBUG_FUNCTION_CALL
                    printf ("func_call->get_args() = %p = %s arg = %p = %s unparseArg = %s \n",func_call->get_args(),
                         func_call->get_args()->class_name().c_str(),*arg,(*arg)->class_name().c_str(),unparseArg ? "true" : "false");
#endif
                 // DQ (4/24/2013): Moved this to be ahead so that the unparseArg value would be associated with the current argument.
                    if (arg != list.begin() && unparseArg == true)
                       {
                         curprint(","); 
                       }

                    if (unparseArg == true)
                       {
                         SgConstructorInitializer* con_init = isSgConstructorInitializer(*arg);
                         if (con_init != NULL && unp->u_sage->isOneElementList(con_init))
                            {
                              unp->u_debug->printDebugInfo("entering unp->u_sage->unparseOneElemConInit", true);
#if DEBUG_FUNCTION_CALL
                              curprint("\n/* unp->u_sage->unparseOneElemConInit in unparseFuncCall */ \n"); 
#endif
                              unparseOneElemConInit(con_init, newinfo);
                           // curprint ( "\n/* DONE: unp->u_sage->unparseOneElemConInit in unparseFuncCall */ \n"); 
                            }
                           else
                            {
#if DEBUG_FUNCTION_CALL
                              curprint("\n/* unparseExpression in args processing in unparseFuncCall */ \n");
#endif
                           // printf ("unparseExpression in args processing in unparseFuncCall \n");
                           // newinfo.display("newinfo in unparseFuncCall()");
                              unparseExpression((*arg), newinfo);
                           // curprint("\n/* DONE: unparseExpression in args processing in unparseFuncCall */ \n");
                            }
                       }

                    arg++;
#if 0
                 // if (arg != list.end())
                    if (arg != list.end() && unparseArg == true)
                       {
                         curprint(","); 
                       }
#endif
                 // DQ (1/2/2018): Supress the trailing function argument in the case of a postfix non-member function using operator syntax.
                    if ( (uses_operator_syntax == true) &&
                         (unp->u_sage->isUnaryOperator(func_call->get_function()) == true) && 
                         (unp->u_sage->isUnaryPostfixOperator(func_call->get_function()) == true))
                       {
#if DEBUG_FUNCTION_CALL
                         printf ("Suppress the trailing argument of the unary postfix operator \n");
                         curprint("\n/* Suppress the trailing argument of the unary postfix operator in unparseFuncCall */ \n"); 
#endif
                      // DQ (2/12/2019): Debugging C++11 literal operators.
                      // ROSE_ASSERT(arg != list.end());
                      // arg++;
                         if (arg != list.end())
                            {
                              arg++;
                            }
                           else
                            {
#if 0
                              printf ("WARNING: arg == list.end(): could be literal operator \n");
#endif
                            }
                       }
                  }
             }

          if (print_paren)
             {
#if DEBUG_FUNCTION_CALL
               curprint ("\n/* Unparse args in unparseFuncCall: closing */ \n"); 
#endif
               curprint (")");
            // printDebugInfo(") from FuncCall", true);
             }

          if ( needSquareBrackets)
             {
               curprint("]");
            // curprint(" /* needSquareBrackets == true */ ]");
             }

       // DQ (8/29/2014): Adding support to supress output of implicit user-defined conversion operators.
             }
            else
             {
#if DEBUG_FUNCTION_CALL
               printf ("Skipping due to suppressed implicit user-defined conversion operator \n");
               curprint("/* Skipping due to suppressed implicit user-defined conversion operator */ \n ");
#endif
               SgUnparse_Info newinfo(info);
               SgBinaryOp* binary_op = isSgBinaryOp(func_call->get_function());
               if (binary_op != NULL)
                  {
                    SgDotExp* dotExp = isSgDotExp(binary_op);
                    if (dotExp != NULL)
                       {
#if DEBUG_FUNCTION_CALL
                         printf ("Unparse the lhs of the SgDotExp (as part of skipping conversion operator) \n");
                         curprint("/* Unparse the lhs of the SgDotExp (as part of skipping conversion operator) */ \n ");
#endif
                         unparseExpression(dotExp->get_lhs_operand(),newinfo);
#if DEBUG_FUNCTION_CALL
                         printf ("DONE: Unparse the lhs of the SgDotExp (as part of skipping conversion operator) \n");
                         curprint("/* DONE: Unparse the lhs of the SgDotExp (as part of skipping conversion operator) */ \n ");
#endif
                       }
                  }
             }

       // DQ (2/2/2018): Handle the case of a postfix operator.
          if ( (uses_operator_syntax == true) &&
               (unp->u_sage->isUnaryOperator(func_call->get_function()) == true) && 
               (unp->u_sage->isUnaryPostfixOperator(func_call->get_function()) == true))
             {
               SgUnparse_Info alt_info(info);
               unparseExpression(func_call->get_function(), alt_info);
             }
#if DEBUG_FUNCTION_CALL
          curprint("\n/* Leaving processing second part in unparseFuncCall */ \n");
#endif
        }

#if DEBUG_FUNCTION_CALL
     printf ("Leaving Unparse_ExprStmt::unparseFuncCall = %p \n",expr);
     curprint ( "\n/* Leaving Unparse_ExprStmt::unparseFuncCall " + StringUtility::numberToString(expr) + " */ \n");
#endif
   }

#if 1
void Unparse_ExprStmt::unparsePointStOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "->", info); }
#else
void Unparse_ExprStmt::unparsePointStOp(SgExpression* expr, SgUnparse_Info& info) 
   {
     printf ("In Unparse_ExprStmt::unparsePointStOp = %p \n",expr);
     curprint ( "\n/* In Unparse_ExprStmt::unparsePointStOp " + StringUtility::numberToString(expr) + " */ \n");
 
     unparseBinaryOperator(expr, "->", info);

     printf ("Leaving Unparse_ExprStmt::unparsePointStOp = %p \n",expr);
     curprint ( "\n/* Leaving Unparse_ExprStmt::unparsePointStOp " + StringUtility::numberToString(expr) + " */ \n");
   }
#endif

void Unparse_ExprStmt::unparseRecRef(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, ".",info); }
void Unparse_ExprStmt::unparseDotStarOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, ".*", info); }
void Unparse_ExprStmt::unparseArrowStarOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "->*", info); }
void Unparse_ExprStmt::unparseEqOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "==", info); }
void Unparse_ExprStmt::unparseLtOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "<", info); }
void Unparse_ExprStmt::unparseGtOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, ">", info); }
void Unparse_ExprStmt::unparseNeOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "!=", info); }
void Unparse_ExprStmt::unparseLeOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "<=", info); }
void Unparse_ExprStmt::unparseGeOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, ">=", info); }
void Unparse_ExprStmt::unparseAddOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "+", info); }
void Unparse_ExprStmt::unparseSubtOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "-", info); }
void Unparse_ExprStmt::unparseMultOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "*", info); }
void Unparse_ExprStmt::unparseDivOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "/", info); }
void Unparse_ExprStmt::unparseIntDivOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "/", info); }
void Unparse_ExprStmt::unparseModOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "%", info); }
void Unparse_ExprStmt::unparseAndOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "&&", info); }
void Unparse_ExprStmt::unparseOrOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "||", info); }
void Unparse_ExprStmt::unparseBitXOrOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "^", info); }
void Unparse_ExprStmt::unparseBitAndOp(SgExpression* expr, SgUnparse_Info& info) {unparseBinaryOperator(expr, "&", info); }
void Unparse_ExprStmt::unparseBitOrOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "|", info); }
void Unparse_ExprStmt::unparseCommaOp(SgExpression* expr, SgUnparse_Info& info) { curprint("("); unparseBinaryOperator(expr, ",", info); curprint(")"); }
void Unparse_ExprStmt::unparseLShiftOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "<<", info); }
void Unparse_ExprStmt::unparseRShiftOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, ">>", info); }
void Unparse_ExprStmt::unparseUnaryMinusOp(SgExpression* expr, SgUnparse_Info& info) { unparseUnaryOperator(expr, "-", info); }
void Unparse_ExprStmt::unparseUnaryAddOp(SgExpression* expr, SgUnparse_Info& info) { unparseUnaryOperator(expr, "+", info); }

// DQ (7/26/2020): Adding support for C++20 spaceship operator.
void Unparse_ExprStmt::unparseSpaceshipOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "<=>", info); }


// DQ (7/26/2020): Adding support for C++20 await expression.
void 
Unparse_ExprStmt::unparseAwaitExpression(SgExpression* expr, SgUnparse_Info& info)
   {
     printf ("C++20 await expression unparse support not implemented \n");
     ROSE_ASSERT(false);
   }


// DQ (7/26/2020): Adding support for C++20 choose expression.
void
Unparse_ExprStmt::unparseChooseExpression(SgExpression* expr, SgUnparse_Info& info)
   {
     printf ("C++20 choose expression unparse support not implemented \n");
     ROSE_ASSERT(false);
   }


// DQ (7/26/2020): Adding support for C++20 expression folding expression.
void
Unparse_ExprStmt::unparseFoldExpression(SgExpression* expr, SgUnparse_Info& info)
   {
  // printf ("C++20 fold expression unparse support not implemented (selected an alternative operator +) \n");

     SgFoldExpression* foldExpression = isSgFoldExpression(expr);
     SgExpression* operands = foldExpression->get_operands();
     unparseExpression(operands,info);
     string operator_token = foldExpression->get_operator_token();

     curprint(operator_token.c_str());
     curprint(" ... ");

  // ROSE_ASSERT(false);
  // unparseAddOp(expr,info);
   }


void
Unparse_ExprStmt::unparseSizeOfOp(SgExpression* expr, SgUnparse_Info & info)
   {
     SgSizeOfOp* sizeof_op = isSgSizeOfOp(expr);
     ASSERT_not_null(sizeof_op);

  // DQ (10/19/2012): This is the explicitly set boolean value which indicates that a class declaration is buried inside
  // the current cast expression's reference to a type (e.g. "(((union ABC { int __in; int __i; }) { .__in = 42 }).__i);").
  // In this case we have to output the base type with its definition.
     bool outputTypeDefinition = sizeof_op->get_sizeOfContainsBaseTypeDefiningDeclaration();

#if 0
     printf ("In unparseSizeOfOp(expr = %p): outputTypeDefinition = %s \n",expr,(outputTypeDefinition == true) ? "true" : "false");
     printf ("sizeof_op->get_operand_expr() = %p \n",sizeof_op->get_operand_expr());
     printf ("sizeof_op->get_operand_type() = %p \n",sizeof_op->get_operand_type());     
#endif

     curprint("sizeof(");

     SgExpression* sizeofExpression = sizeof_op->get_operand_expr();
  // if (sizeof_op->get_operand_expr() != NULL)
     if (sizeofExpression != NULL)
        {
          ASSERT_not_null(sizeofExpression);

       // DQ (1/12/2019): Adding support for C++11 feature (see test2019_10.C).
          if (sizeof_op->get_is_objectless_nonstatic_data_member_reference() == true)
             {
            // Output the name of the class (but don't conside this to be name qualification).
#if 0
               printf ("sizeofExpression = %p = %s = %s \n",sizeofExpression,sizeofExpression->class_name().c_str(),SageInterface::get_name(sizeofExpression).c_str());
#endif
            // Need to find the member reference.
               SgArrowExp* arrowExp = isSgArrowExp(sizeofExpression);
               ASSERT_not_null(arrowExp);

            // SgExpression* lhs = arrowExp->get_lhs_operand();
            // ASSERT_not_null(lhs);
            // printf ("lhs = %p = %s \n",lhs,lhs->class_name().c_str());

               SgExpression* rhs = arrowExp->get_rhs_operand();
               ASSERT_not_null(rhs);
#if 0
               printf ("rhs = %p = %s \n",rhs,rhs->class_name().c_str());
#endif
               SgVarRefExp* varRef = isSgVarRefExp(rhs);
               ASSERT_not_null(varRef);

               unparseExpression(varRef, info);
             }
            else
             {
            // DQ (1/12/2019): Previous code before supporting C++11 objectless non-static data member references.
               unparseExpression(sizeofExpression, info);
             }
        }
       else
        {
          ASSERT_not_null(sizeof_op->get_operand_type());

          SgUnparse_Info info2(info);
          info2.unset_SkipBaseType();

       // DQ (11/3/2015): We might have to use the "struct" class elaboration if this is a type.
       // We have to turn this back on in the case where we are in a for loop test (condition) 
       // where it would be turned off as a result of a fix to make handling of the test expression 
       // more unifor between token-based unparsing and the AST unparsing.
          info2.unset_SkipClassSpecifier();

       // DQ (3/15/2015): test2015_11.c demonstrates a case where I think this should be not be set (un-named struct type).
       // info2.set_SkipClassDefinition();

          info2.unset_isTypeFirstPart();
          info2.unset_isTypeSecondPart();

       // DQ (6/2/2011): Added support for name qualification of types reference via sizeof operator.
          info2.set_reference_node_for_qualification(sizeof_op);

       // DQ (10/19/2012): Modified to support output of the type's defining declaration (see test2012_57.c).
       // unp->u_type->unparseType(sizeof_op->get_operand_type(), info2);

          SgUnparse_Info newinfo(info2);

          if (outputTypeDefinition == true)
             {
            // DQ (10/11/2006): As part of new implementation of qualified names we now default to the generation of all qualified names unless they are skipped.
            // newinfo.set_SkipQualifiedNames();
#if 1
            // DQ (3/15/2015): test2015_11.c demonstrates a case where I think this should be not be set (un-named struct type).
            // DQ (10/17/2012): Added new code not present where this is handled for SgVariableDeclaration IR nodes.
               newinfo.unset_SkipDefinition();

            // DQ (5/23/2007): Commented these out since they are not applicable for statement expressions (see test2007_51.C).
            // DQ (10/5/2004): If this is a defining declaration then make sure that we don't skip the definition
               ROSE_ASSERT(newinfo.SkipClassDefinition() == false);
               ROSE_ASSERT(newinfo.SkipEnumDefinition()  == false);
               ROSE_ASSERT(newinfo.SkipDefinition()      == false);
#endif
             }
            else
             {
               newinfo.set_SkipDefinition();
               ROSE_ASSERT(newinfo.SkipClassDefinition() == true);
               ROSE_ASSERT(newinfo.SkipEnumDefinition() == true);
             }

#if 0
          printf ("In unparseSizeOfOp(): calling newinfo.unset_SkipSemiColon() \n");
          curprint ("/* In unparseSizeOfOp(): calling newinfo.unset_SkipSemiColon() \n */ ");
#endif
       // DQ (10/18/2012): Added to unset ";" usage in defining declaration.
          newinfo.unset_SkipSemiColon();

       // DQ (10/17/2012): We have to separate these out if we want to output the defining declarations.
          newinfo.set_isTypeFirstPart();

       // DQ (1/6/2020): The type will be an argument to the sizeof operator (see Cxx11_tests/test2020_14.C).
          newinfo.set_inArgList();
#if 0
          newinfo.display("In unparseSizeOfOp(): newinfo");
#endif
#if 0
          printf ("In unparseSizeOfOp(): isTypeFirstPart:  sizeof_op->get_operand_type() = %p = %s \n",sizeof_op->get_operand_type(),sizeof_op->get_operand_type()->class_name().c_str());
          curprint ("/* In unparseSizeOfOp(): isTypeFirstPart \n */ ");
#endif
          unp->u_type->unparseType(sizeof_op->get_operand_type(), newinfo);
          newinfo.set_isTypeSecondPart();
#if 0
          printf ("In unparseSizeOfOp(): isTypeSecondPart: sizeof_op->get_operand_type() = %p = %s \n",sizeof_op->get_operand_type(),sizeof_op->get_operand_type()->class_name().c_str());
          curprint ("/* In unparseSizeOfOp(): isTypeSecondPart \n */ ");
#endif
          unp->u_type->unparseType(sizeof_op->get_operand_type(), newinfo);
        }

     curprint(")");

#if 0
     printf ("Leaving unparseSizeOfOp(expr = %p): outputTypeDefinition = %s \n",expr,(outputTypeDefinition == true) ? "true" : "false");
#endif
   }


void
Unparse_ExprStmt::unparseAlignOfOp(SgExpression* expr, SgUnparse_Info & info)
   {
     SgAlignOfOp* sizeof_op = isSgAlignOfOp(expr);
     ASSERT_not_null(sizeof_op);

  // DQ (10/19/2012): This is the explicitly set boolean value which indicates that a class declaration is buried inside
  // the current cast expression's reference to a type (e.g. "(((union ABC { int __in; int __i; }) { .__in = 42 }).__i);").
  // In this case we have to output the base type with its definition.
     bool outputTypeDefinition = sizeof_op->get_alignOfContainsBaseTypeDefiningDeclaration();

#if 0
     printf ("In unparseAlignOfOp(expr = %p): outputTypeDefinition = %s \n",expr,(outputTypeDefinition == true) ? "true" : "false");
#endif

  // curprint ( "alignof(");
     curprint ( "__alignof__(");

     if (sizeof_op->get_operand_expr() != NULL)
        {
          ASSERT_not_null(sizeof_op->get_operand_expr());
          unparseExpression(sizeof_op->get_operand_expr(), info);
        }
       else
        {
          ASSERT_not_null(sizeof_op->get_operand_type());
          SgUnparse_Info info2(info);
          info2.unset_SkipBaseType();

          info2.set_SkipClassDefinition();
#if 0
          printf ("In unparseAlignOfOp(expr = %p): Added call to set_SkipEnumDefinition() for symetry with call to set_SkipClassDefinition() \n",expr);
#endif
       // DQ (9/9/2016): Added call to set_SkipEnumDefinition().
          info2.set_SkipEnumDefinition();

          info2.unset_isTypeFirstPart();
          info2.unset_isTypeSecondPart();

       // DQ (6/2/2011): Added support for name qualification of types reference via sizeof operator.
          info2.set_reference_node_for_qualification(sizeof_op);

       // DQ (10/19/2012): Modified to support output of the type's defining declaration (see test2012_57.c).
       // unp->u_type->unparseType(sizeof_op->get_operand_type(), info2);

          SgUnparse_Info newinfo(info2);

          if (outputTypeDefinition == true)
             {
            // DQ (10/11/2006): As part of new implementation of qualified names we now default to the generation of all qualified names unless they are skipped.
            // newinfo.set_SkipQualifiedNames();

            // DQ (10/17/2012): Added new code not present where this is handled for SgVariableDeclaration IR nodes.
               newinfo.unset_SkipDefinition();

            // DQ (5/23/2007): Commented these out since they are not applicable for statement expressions (see test2007_51.C).
            // DQ (10/5/2004): If this is a defining declaration then make sure that we don't skip the definition
               ROSE_ASSERT(newinfo.SkipClassDefinition() == false);
               ROSE_ASSERT(newinfo.SkipEnumDefinition()  == false);
               ROSE_ASSERT(newinfo.SkipDefinition()      == false);
             }
            else
             {
               newinfo.set_SkipDefinition();
               ROSE_ASSERT(newinfo.SkipClassDefinition() == true);
               ROSE_ASSERT(newinfo.SkipEnumDefinition() == true);
             }

#if 0
          printf ("In unparseSizeOfOp(): calling newinfo.unset_SkipSemiColon() \n");
#endif
       // DQ (10/18/2012): Added to unset ";" usage in defining declaration.
          newinfo.unset_SkipSemiColon();
#if 1
       // DQ (10/17/2012): We have to separate these out if we want to output the defining declarations.
          newinfo.set_isTypeFirstPart();
          unp->u_type->unparseType(sizeof_op->get_operand_type(), newinfo);
          newinfo.set_isTypeSecondPart();
          unp->u_type->unparseType(sizeof_op->get_operand_type(), newinfo);
#else
       // DQ (1/14/2006): p_expression_type is no longer stored (type is computed instead)
       // unp->u_type->unparseType(cast_op->get_expression_type(), newinfo);
       // unp->u_type->unparseType(cast_op->get_type(), newinfo);
          unp->u_type->unparseType(sizeof_op->get_operand_type(), newinfo);
#endif
        }
     curprint ( ")");
   }


void
Unparse_ExprStmt::unparseNoexceptOp(SgExpression* expr, SgUnparse_Info & info)
   {
     SgNoexceptOp* noexcept_op = isSgNoexceptOp(expr);
     ASSERT_not_null(noexcept_op);

#if 0
     printf ("In unparseNoexceptOp(expr = %p): \n",expr);
#endif

     curprint("noexcept(");

     ASSERT_not_null(noexcept_op->get_operand_expr());
     unparseExpression(noexcept_op->get_operand_expr(), info);

     curprint(")");
   }


void
Unparse_ExprStmt::unparseUpcLocalSizeOfOp(SgExpression* expr, SgUnparse_Info & info)
   {
     SgUpcLocalsizeofExpression* sizeof_op = isSgUpcLocalsizeofExpression(expr);
     ASSERT_not_null(sizeof_op);

     curprint ( "upc_localsizeof(");
     if (sizeof_op->get_expression() != NULL)
        {
          ASSERT_not_null(sizeof_op->get_expression());
          unparseExpression(sizeof_op->get_expression(), info);
        }
#if 1
    // DQ (2/12/2011): Leave this here until I'm sure that we don't need to handle types.
       else
        {
          ASSERT_not_null(sizeof_op->get_operand_type());
          SgUnparse_Info info2(info);
          info2.unset_SkipBaseType();

          info2.set_SkipClassDefinition();

#if 0
          printf ("In unparseUpcLocalSizeOfOp(expr = %p): Added call to set_SkipEnumDefinition() for symetry with call to set_SkipClassDefinition() \n",expr);
#endif
       // DQ (9/9/2016): Added call to set_SkipEnumDefinition().
          info2.set_SkipEnumDefinition();

          info2.unset_isTypeFirstPart();
          info2.unset_isTypeSecondPart();
          unp->u_type->unparseType(sizeof_op->get_operand_type(), info2);
        }
#endif
     curprint ( ")");
   }

void
Unparse_ExprStmt::unparseUpcBlockSizeOfOp(SgExpression* expr, SgUnparse_Info & info)
   {
     SgUpcBlocksizeofExpression* sizeof_op = isSgUpcBlocksizeofExpression(expr);
     ASSERT_not_null(sizeof_op);

     curprint ( "upc_blocksizeof(");
     if (sizeof_op->get_expression() != NULL)
        {
          ASSERT_not_null(sizeof_op->get_expression());
          unparseExpression(sizeof_op->get_expression(), info);
        }
#if 1
    // DQ (2/12/2011): Leave this here until I'm sure that we don't need to handle types.
       else
        {
          ASSERT_not_null(sizeof_op->get_operand_type());
          SgUnparse_Info info2(info);
          info2.unset_SkipBaseType();

          info2.set_SkipClassDefinition();

#if 0
          printf ("In unparseUpcBlockSizeOfOp(expr = %p): Added call to set_SkipEnumDefinition() for symetry with call to set_SkipClassDefinition() \n",expr);
#endif
       // DQ (9/9/2016): Added call to set_SkipEnumDefinition().
          info2.set_SkipEnumDefinition();

          info2.unset_isTypeFirstPart();
          info2.unset_isTypeSecondPart();
          unp->u_type->unparseType(sizeof_op->get_operand_type(), info2);
        }
#endif
     curprint ( ")");
   }

void
Unparse_ExprStmt::unparseUpcElemSizeOfOp(SgExpression* expr, SgUnparse_Info & info)
   {
     SgUpcElemsizeofExpression* sizeof_op = isSgUpcElemsizeofExpression(expr);
     ASSERT_not_null(sizeof_op);

     curprint ( "upc_elemsizeof(");
     if (sizeof_op->get_expression() != NULL)
        {
          ASSERT_not_null(sizeof_op->get_expression());
          unparseExpression(sizeof_op->get_expression(), info);
        }
#if 1
    // DQ (2/12/2011): Leave this here until I'm sure that we don't need to handle types.
       else
        {
          ASSERT_not_null(sizeof_op->get_operand_type());
          SgUnparse_Info info2(info);
          info2.unset_SkipBaseType();

          info2.set_SkipClassDefinition();
#if 0
          printf ("In unparseUpcElemSizeOfOp(expr = %p): Added call to set_SkipEnumDefinition() for symetry with call to set_SkipClassDefinition() \n",expr);
#endif
       // DQ (9/9/2016): Added call to set_SkipEnumDefinition().
          info2.set_SkipEnumDefinition();

          info2.unset_isTypeFirstPart();
          info2.unset_isTypeSecondPart();
          unp->u_type->unparseType(sizeof_op->get_operand_type(), info2);
        }
#endif
     curprint ( ")");
   }

void Unparse_ExprStmt::unparseTypeIdOp(SgExpression* expr, SgUnparse_Info& info)
   {
     SgTypeIdOp* typeid_op = isSgTypeIdOp(expr);
     ASSERT_not_null(typeid_op);

     curprint ( "typeid(");
     if (typeid_op->get_operand_expr() != NULL)
        {
          ASSERT_not_null(typeid_op->get_operand_expr());
#if 0
          printf ("In unparseTypeIdOp(): typeid_op->get_operand_expr() = %p = %s \n",typeid_op->get_operand_expr(),typeid_op->get_operand_expr()->class_name().c_str());
#endif
          unparseExpression(typeid_op->get_operand_expr(), info);
        }
       else
        {
          ASSERT_not_null(typeid_op->get_operand_type());
          SgUnparse_Info info2(info);
          info2.unset_SkipBaseType();
          info2.set_SkipClassDefinition();

       // DQ (10/28/2015): This will be enforced uniformally with SkipClassDefinition() in the unparseType() function below.
          info2.set_SkipEnumDefinition();

       // DQ (6/2/2011): Added support for name qualification of types reference via sizeof operator.
          info2.set_reference_node_for_qualification(typeid_op);

       // DQ (10/28/2015): This will be enforced in the unparseType() function
       // (so detect it here where it is more clear how to fix it, above).
          ROSE_ASSERT(info2.SkipClassDefinition() == info2.SkipEnumDefinition());

          unp->u_type->unparseType(typeid_op->get_operand_type(), info2);
        }

     curprint ( ")");
   }

void Unparse_ExprStmt::unparseNotOp(SgExpression* expr, SgUnparse_Info& info) { unparseUnaryOperator(expr, "!", info); }
void Unparse_ExprStmt::unparseDerefOp(SgExpression* expr, SgUnparse_Info& info) { unparseUnaryOperator(expr, "*", info); }
void Unparse_ExprStmt::unparseAddrOp(SgExpression* expr, SgUnparse_Info& info) { unparseUnaryOperator(expr, "&", info); }
void Unparse_ExprStmt::unparseMinusMinusOp(SgExpression* expr, SgUnparse_Info& info) { unparseUnaryOperator(expr, "--", info); }
void Unparse_ExprStmt::unparsePlusPlusOp(SgExpression* expr, SgUnparse_Info& info) { unparseUnaryOperator(expr, "++", info); }
void Unparse_ExprStmt::unparseAbstractOp(SgExpression* expr, SgUnparse_Info& info) {}
void Unparse_ExprStmt::unparseBitCompOp(SgExpression* expr, SgUnparse_Info& info) { unparseUnaryOperator(expr, "~", info); }
void Unparse_ExprStmt::unparseRealPartOp(SgExpression* expr, SgUnparse_Info& info) { unparseUnaryOperator(expr, "__real__ ", info); }
void Unparse_ExprStmt::unparseImagPartOp(SgExpression* expr, SgUnparse_Info& info) { unparseUnaryOperator(expr, "__imag__ ", info); }
void Unparse_ExprStmt::unparseConjugateOp(SgExpression* expr, SgUnparse_Info& info) { unparseUnaryOperator(expr, "~", info); }

void Unparse_ExprStmt::unparseExprCond(SgExpression* expr, SgUnparse_Info& info)
   {
     SgConditionalExp* expr_cond = isSgConditionalExp(expr);
     ASSERT_not_null(expr_cond);

#if 0
     printf ("In unparseExprCond(): info.get_nested_expression() = %d \n",info.get_nested_expression());
#endif

  // int toplevel_expression = !info.get_nested_expression();
     bool toplevel_expression = (info.get_nested_expression() == 0);

  // DQ (2/9/2010): Added code to reset if we are in a top level expression (see test2010_04.C).
  // Detecting the nesting level is not enough since the SgDotExp does not set this.  So check the parents.
     SgNode* parentNode = expr->get_parent();
  // printf ("parentNode = %p = %s \n",parentNode,parentNode->class_name().c_str());
     if (isSgExprListExp(parentNode) != NULL && toplevel_expression == true)
        {
       // printf ("Resetting toplevel_expression to false \n");
          toplevel_expression = false;
        }

     info.set_nested_expression();

#if 0
     printf ("In unparseExprCond(): toplevel_expression = %d \n",toplevel_expression);
#endif

#if 0
  // DQ (10/25/2004): Not clear what this is about???
  // For now make sure this generates output so that we can debug this
     if ( SgProject::get_verbose() >= 1 )
          printf ("In unparseExprCond(): Fixed lvalue handling - expr_cond->get_is_lvalue() \n");
#endif

  // if (! toplevel_expression || expr_cond->get_is_lvalue())
  // if (!toplevel_expression)
     if (! toplevel_expression || expr_cond->get_lvalue())
        {
          curprint ( "(");
       // curprint ( "/* unparseExprCond */ (";
        }
     unparseExpression(expr_cond->get_conditional_exp(), info);

  // DQ (1/26/2009): Added spaces to make the formatting nicer (but it breaks the diff tests in the loop processor, so fix this later).
  // curprint (" ? "); 
     curprint ("?"); 

     unparseExpression(expr_cond->get_true_exp(), info);

   // Liao, 2/16/2009. We have to have space to avoid first?x:::std::string("") Three colons in a row!
  // DQ (1/26/2009): Added spaces to make the formatting nicer (but it breaks the diff tests in the loop processor, so fix this later).
   curprint (" : "); 
  //   curprint (":"); 

     unparseExpression(expr_cond->get_false_exp(), info);
  // if (! toplevel_expression || expr_cond->get_is_lvalue())
  // if (!toplevel_expression)
     if (! toplevel_expression || expr_cond->get_lvalue())
        {
          curprint ( ")");
        }
     info.unset_nested_expression();
   }

void
Unparse_ExprStmt::unparseClassInitOp(SgExpression* expr, SgUnparse_Info& info)
   {
   } 

void
Unparse_ExprStmt::unparseDyCastOp(SgExpression* expr, SgUnparse_Info& info)
   {
   }

void
Unparse_ExprStmt::unparseCastOp(SgExpression* expr, SgUnparse_Info& info)
   {
     SgCastExp* cast_op = isSgCastExp(expr);
     ASSERT_not_null(cast_op);

#if 0
     printf ("In unparseCastOp(): expr = %p \n",expr);
     curprint("/* In unparseCastOp() */ \n ");
#endif

#if 0
     cast_op->get_file_info()->display("In unparseCastOp(): debug");
#endif

#if 0
     printf ("In unparseCastOp(): info.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
     printf ("In unparseCastOp(): info.SkipEnumDefinition()  = %s \n",(info.SkipEnumDefinition() == true) ? "true" : "false");
#endif

  // DQ (1/9/2014): These should have been setup to be the same.
     ROSE_ASSERT(info.SkipClassDefinition() == info.SkipEnumDefinition());

     SgUnparse_Info newinfo(info);
     newinfo.unset_PrintName();
     newinfo.unset_isTypeFirstPart();
     newinfo.unset_isTypeSecondPart();

  // DQ (5/30/2011): Added support for name qualification.
     newinfo.set_reference_node_for_qualification(cast_op);
     ASSERT_not_null(newinfo.get_reference_node_for_qualification());

  // DQ (10/8/2004): Never unparse the declaration from within a cast expression (see testcode2001_28.C)!
     newinfo.set_SkipDefinition();

     newinfo.unset_SkipBaseType();

  // printf ("In unparseCastOp(): cast_op->cast_type() = %d \n",cast_op->cast_type());
  // curprint ( "/* In unparseCastOp(): cast_op->cast_type() = " + cast_op->cast_type() + " */";

  // DQ (6/19/2006): Constant folding happens within casts and we have to address this.
  // more info can be found in the documentation for the addition of the 
  // SgCastExp::p_originalExpressionTree data member in ROSE/src/ROSETTA/expressions.C
     SgExpression* expressionTree = cast_op->get_originalExpressionTree();
     if (expressionTree != NULL && info.SkipConstantFoldedExpressions() == false)
        {
#if 0
          printf ("Found and expression tree representing a cast expression (unfolded constant expression requiring a cast) expressionTree = %p = %s \n",
               expressionTree,expressionTree->class_name().c_str());
#endif

       // Use the saved alternative (original) cast expression (should always be a cast 
       // expression as well). Note that we still have to deal with where this is a cast 
       // to an un-named type (e.g. un-named enum: test2006_75.C).
          cast_op = isSgCastExp(expressionTree);
#if 0
       // ASSERT_not_null(cast_op);
       // if (cast_op == NULL)
          if (cast_op != NULL) // Liao, 11/2/2010, we should use the original expression tree here!!
             {
            // Jeremiah has submitted the following example: int x[2]; char* y = (char*)x + 1; and the expressionTree is just "x+1".
               unparseExpression(expressionTree,info);

            // Don't continue processing this as a cast!
               return;
             }
            else 
             {
               cast_op = isSgCastExp(expr); // restore to the original non-null value otherwise
             }
#else
       // Liao, 11/8/2010, we should now always unparse the original expression tree, regardless its Variant_T value
          unparseExpression(expressionTree,info);
          return;
#endif
        }

#if 0
  // DQ (6/2/2011): Output the previously generated name qualification (empty is not required).
  // SgName nameQualifier = cast_op->get_qualified_name_prefix();
     SgName nameQualifier = cast_op->get_qualified_name_prefix_for_referenced_type();
     curprint ("/* nameQualifier = " + nameQualifier + " */ \n");
     curprint (nameQualifier);
#else
  // DQ (6/2/2011): I think this is all that is required.
  // SgName nameQualifier = cast_op->get_qualified_name_prefix_for_referenced_type();
  // curprint ("/* nameQualifier = " + nameQualifier + " */ \n");
     newinfo.set_reference_node_for_qualification(cast_op);
#endif

  // DQ (10/17/2012): This is the explicitly set boolean value which indicates that a class declaration is buried inside
  // the current cast expression's reference to a type (e.g. "(((union ABC { int __in; int __i; }) { .__in = 42 }).__i);").
  // In this case we have to output the base type with its definition.
     bool outputTypeDefinition = cast_op->get_castContainsBaseTypeDefiningDeclaration();

#if 0
     printf ("In unparseCastOp(expr = %p): outputTypeDefinition = %s \n",expr,(outputTypeDefinition == true) ? "true" : "false");
#endif

     if (outputTypeDefinition == true)
        {
       // DQ (10/11/2006): As part of new implementation of qualified names we now default to the generation of all qualified names unless they are skipped.
       // newinfo.set_SkipQualifiedNames();

       // DQ (10/17/2012): Added new code not present where this is handled for SgVariableDeclaration IR nodes.
          newinfo.unset_SkipDefinition();

       // DQ (5/23/2007): Commented these out since they are not applicable for statement expressions (see test2007_51.C).
       // DQ (10/5/2004): If this is a defining declaration then make sure that we don't skip the definition
          ROSE_ASSERT(newinfo.SkipClassDefinition() == false);
          ROSE_ASSERT(newinfo.SkipEnumDefinition()  == false);
          ROSE_ASSERT(newinfo.SkipDefinition()      == false);
        }
       else
        {
          newinfo.set_SkipDefinition();
          ROSE_ASSERT(newinfo.SkipClassDefinition() == true);
          ROSE_ASSERT(newinfo.SkipEnumDefinition() == true);
        }

     bool addParens = false;
     switch(cast_op->cast_type())
        {
          case SgCastExp::e_unknown:
             {
               printf ("SgCastExp::e_unknown found \n");
               ROSE_ASSERT(false);
               break; 
             }

          case SgCastExp::e_default:
             {
               printf ("SgCastExp::e_default found \n");
               ROSE_ASSERT(false);
               break; 
             }

          case SgCastExp::e_dynamic_cast:
             {
            // dynamic_cast <P *> (expr)
               curprint ( "dynamic_cast < ");

            // DQ (1/14/2006): p_expression_type is no longer stored (type is computed instead)
            // unp->u_type->unparseType(cast_op->get_expression_type(), newinfo); // first/second part
               unp->u_type->unparseType(cast_op->get_type(), newinfo); // first/second part

               curprint ( " > "); // paren are in operand_i
               addParens = true;
               break; 
             }

          case SgCastExp::e_reinterpret_cast:
             {
            // reinterpret_cast <P *> (expr)
               curprint ( "reinterpret_cast < ");

            // DQ (1/14/2006): p_expression_type is no longer stored (type is computed instead)
            // unp->u_type->unparseType(cast_op->get_expression_type(), newinfo);
               unp->u_type->unparseType(cast_op->get_type(), newinfo);

               curprint ( " > ");
               addParens = true;
               break;
             }

          case SgCastExp::e_const_cast:
             {
            // const_cast <P *> (expr)
               curprint ( "const_cast < ");

            // DQ (1/14/2006): p_expression_type is no longer stored (type is computed instead)
            // unp->u_type->unparseType(cast_op->get_expression_type(), newinfo);
               unp->u_type->unparseType(cast_op->get_type(), newinfo);

               curprint ( " > ");
               addParens = true;
               break;
             }

          case SgCastExp::e_static_cast:
             {
            // static_cast <P *> (expr)
               curprint ( "static_cast < ");

            // DQ (1/14/2006): p_expression_type is no longer stored (type is computed instead)
            // unp->u_type->unparseType(cast_op->get_expression_type(), newinfo);
               unp->u_type->unparseType(cast_op->get_type(), newinfo);

               curprint ( " > ");
               addParens = true;
               break;
             }

       // case SgCastExp::e_const_cast:
          case SgCastExp::e_C_style_cast:
             {
#if 0
               if (cast_op->get_file_info()->isCompilerGenerated() == true)
                  {
                     curprint ( "\n /* unparseCastOp compiler generated cast = " 
                         + StringUtility::numberToString(cast_op) 
                         + ") file info = " 
                         + StringUtility::numberToString((void*)(cast_op->get_file_info())) 
                         + " */ \n");
                  }
                 else
                  {
                    curprint ( "\n /* unparseCastOp explicitly specified cast */ \n"); 
                  }
#endif
#if 0
            // This error checking code is in unparseExpression() already.
               if (expr->get_file_info()->isCompilerGenerated() != expr->get_startOfConstruct()->isCompilerGenerated())
                  {
                    printf ("In unparseCastOp(): Detected error expr->get_file_info()->isCompilerGenerated() != expr->get_startOfConstruct()->isCompilerGenerated() \n");
                    expr->get_file_info()->display("expr->get_file_info(): debug");
                    expr->get_startOfConstruct()->display("expr->get_startOfConstruct(): debug");
                  }
#endif
#if 0
               printf ("case SgCastExp::e_C_style_cast: cast_op->get_startOfConstruct()->isCompilerGenerated() = %s \n",cast_op->get_startOfConstruct()->isCompilerGenerated() ? "true" : "false");
#endif
            // DQ (2/28/2005): Only output the cast if it is NOT compiler generated (implicit in the source code)
            // this avoids redundant casts in the output code and avoid errors in the generated code caused by an 
            // implicit cast to a private type (see test2005_12.C).
            // if (cast_op->get_file_info()->isCompilerGenerated() == false)
               if (cast_op->get_startOfConstruct()->isCompilerGenerated() == false)
                  {
                 // (P *) expr
                 // check if the expression that we are casting is not a string

                 // DQ (7/26/2013): This should also be true (all of the source position info should be consistant).
                    if (cast_op->get_file_info()->isCompilerGenerated()) {
                      printf("[Unparse_ExprStmt::unparseCastOp] Fatal: cast_op->get_file_info()->isCompilerGenerated() but !cast_op->get_startOfConstruct()->isCompilerGenerated().\n");
                    }
                    ROSE_ASSERT(cast_op->get_file_info()->isCompilerGenerated() == false);

                 // DQ (7/31/2013): This appears to happen for at least one test in projects/arrayOptimization.
                 // I can't fix that project presently, so make this an error message for the moment.
                 // ROSE_ASSERT(cast_op->get_endOfConstruct()->isCompilerGenerated() == false);
                    if (cast_op->get_endOfConstruct() == NULL || cast_op->get_endOfConstruct()->isCompilerGenerated() == true)
                       {
                         printf ("Error: In unparseCastOp(): cast_op->get_endOfConstruct() is inconsistant with cast_op->get_file_info() \n");
                       }
#if 0
                    curprint ("\n /* explicit cast: cast_op->get_operand_i() = " + cast_op->get_operand_i()->class_name() + " */ \n");
#endif
                    if (cast_op->get_operand_i()->variant() != STRING_VAL)
                       {
                      // It is not a string, so we always cast
#if 0
                         curprint("/* unparseCastOp SgCastExp::c_cast_e nonstring */ ");
#endif
                         curprint("(");
#if 0
                         printf ("In unparseCastOp(): output cast to associated type \n");
#endif
                      // DQ (10/18/2012): Added to unset ";" usage in defining declaration.
                         newinfo.unset_SkipSemiColon();

                      // DQ (10/17/2012): We have to separate these out if we want to output the defining declarations.
                         newinfo.set_isTypeFirstPart();
                         unp->u_type->unparseType(cast_op->get_type(), newinfo);
                         newinfo.set_isTypeSecondPart();
                         unp->u_type->unparseType(cast_op->get_type(), newinfo);
                         curprint(")");
                       }
                 // cast_op->get_operand_i()->variant() == STRING_VAL
                 // it is a string, so now check if the cast is not a "const char* "
                 // or if the caststring option is on. If any of these are true,
                 // then unparse the cast. Both must be false to not unparse the cast.
                      else
                       {
                      // DQ (1/14/2006): p_expression_type is no longer stored (type is computed instead)
                      // if (!unp->u_sage->isCast_ConstCharStar(cast_op->get_expression_type()) || unp->opt.get_caststring_opt())
                         if (!unp->u_sage->isCast_ConstCharStar(cast_op->get_type()) || unp->opt.get_caststring_opt())
                            {
#if 0
                              curprint ("/* unparseCastOp SgCastExp::c_cast_e case string */ ");
#endif
                              curprint ("(");

                           // DQ (1/14/2006): p_expression_type is no longer stored (type is computed instead)
                           // unp->u_type->unparseType(cast_op->get_expression_type(), newinfo);
                              unp->u_type->unparseType(cast_op->get_type(), newinfo);

                              curprint (")");
                            }
                       }
                  }
               break; 
             }

          default:
             {
               printf ("Default reached in cast_op->cast_type() = %d \n",cast_op->cast_type());
               ROSE_ASSERT(false);
               break; 
             }
        }

#if 0
     printf ("In unparseCastOp(): case SgCastExp::e_C_style_cast: addParens = %s \n",addParens ? "true" : "false");

     curprint (string("/* unparse the cast's operand: get_operand() = ") + cast_op->get_operand()->class_name() + " */");
     curprint (string("/* unparse the cast's operand: get_need_paren() = ") + (cast_op->get_operand()->get_need_paren() ? "true" : "false") + " */");
     curprint (string("/* unparse the cast's operand: addParens = ") + (addParens ? "true" : "false") + " */");
#endif

  // DQ (6/15/2005): reinterpret_cast always needs parens
     if (addParens == true)
        {
          curprint(" (");
        }

  // DQ (6/21/2011): Added support for name qualification.
     info.set_reference_node_for_qualification(cast_op->get_operand());
     ASSERT_not_null(info.get_reference_node_for_qualification());

#if 0
     printf ("In unparseCastOp(): info.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
     printf ("In unparseCastOp(): info.SkipEnumDefinition()  = %s \n",(info.SkipEnumDefinition() == true) ? "true" : "false");
#endif

  // DQ (1/9/2014): These should have been setup to be the same.
     ROSE_ASSERT(info.SkipClassDefinition() == info.SkipEnumDefinition());

#if 0
     printf("In unparseCastOp(): case SgCastExp::e_C_style_cast: cast_op->get_operand() = %p = %s \n",cast_op->get_operand(),cast_op->get_operand()->class_name().c_str());
#endif

     unparseExpression(cast_op->get_operand(), info); 

     if (addParens == true)
        {
          curprint(")");
        }

#if 0
     printf ("Leaving unparseCastOp(): expr = %p \n",expr);
     curprint("/* Leaving unparseCastOp() */ \n ");
#endif
   }


void
Unparse_ExprStmt::unparseArrayOp(SgExpression* expr, SgUnparse_Info& info)
   { 
     unparseBinaryOperator(expr, "[]", info); 
   }

void
Unparse_ExprStmt::unparseNewOp(SgExpression* expr, SgUnparse_Info& info)
   {
  // printf ("In Unparse_ExprStmt::unparseNewOp \n");
  // curprint ( "\n /* In Unparse_ExprStmt::unparseNewOp */ \n";

#define DEBUG_NEW_OPERATOR 0

#ifndef CXX_IS_ROSE_CODE_GENERATION
     SgNewExp* new_op = isSgNewExp(expr);
     ASSERT_not_null(new_op);

     if (new_op->get_need_global_specifier())
        {
       // DQ (1/5/2006): I don't think that we want the extra space after the "::".
       // curprint ( ":: ";
          curprint ( "::");
        }

     curprint ("new ");

#if DEBUG_NEW_OPERATOR
     curprint ("\n /* Output any placement arguments */ \n");
#endif

     SgUnparse_Info newinfo(info);
     newinfo.unset_inVarDecl();
     if (new_op->get_placement_args() != NULL)
        {
#if 0
       // printf ("Output placement arguments for new operator \n");
          curprint ( "\n/* Output placement arguments for new operator */\n");
#endif
       // DQ (1/5/2006): The placement arguments require "() " (add a space to make it look nice)
          curprint ( "(");
          unparseExpression(new_op->get_placement_args(), newinfo);
          curprint ( ") ");
        }

     newinfo.unset_PrintName();
     newinfo.unset_isTypeFirstPart();
     newinfo.unset_isTypeSecondPart();
     newinfo.set_SkipClassSpecifier();

  // DQ (11/26/2004): In KULL this is set to true when I think it should not be (could not reproduce error in smaller code!)
  // the problem is that the type name is not being output after the new keyword.  It should unparse to "new typename (args)" and 
  // instead just unparses to "new (args)".  Error occurprints in generated code (rose_polygonalaleremapswig.C).
     newinfo.unset_SkipBaseType();

  // This fixes a bug having to do with the unparsing of the type name of constructors in return statements.

  // DQ (5/30/2011): Added support for name qualification.
     newinfo.set_reference_node_for_qualification(new_op);
     ASSERT_not_null(newinfo.get_reference_node_for_qualification());

#if DEBUG_NEW_OPERATOR
     curprint ("\n /* Output type name for new operator */ \n");
#endif

  // printf ("In Unparse_ExprStmt::unparseNewOp: new_op->get_type()->class_name() = %s \n",new_op->get_type()->class_name().c_str());

  // DQ (3/26/2012): I think this is required because the type might be the only public way refer to the 
  // class (via a public typedef to a private class, so we can't use the constructor; except for it's args)

#if DEBUG_NEW_OPERATOR
     printf ("In unparseNewOp(): Calling unparseType(): new_op->get_specified_type() = %p = %s \n",new_op->get_specified_type(),new_op->get_specified_type()->class_name().c_str());
#endif

  // DQ (1/8/2020): When arguments are provided, we need to add parenthesis around the type.
  // See Cxx11_tests/test2020_29.C for an example of where this is required.
     bool add_parenthesis_around_type = false;
     if (new_op->get_constructor_args() != NULL)
        {
#if DEBUG_NEW_OPERATOR
          printf ("In unparseNewOp(): Calling unparseType(): new_op->get_constructor_args() = %p = %s \n",new_op->get_constructor_args(),new_op->get_constructor_args()->class_name().c_str());
#endif
          SgType* newOperatorSpecifiedType = new_op->get_specified_type();
          ASSERT_not_null(newOperatorSpecifiedType);

#if DEBUG_NEW_OPERATOR || 0
          printf ("In unparseNewOp(): newOperatorType = %p = %s \n",newOperatorSpecifiedType,newOperatorSpecifiedType->class_name().c_str());
#endif
          SgArrayType* newOperatorArrayType = isSgArrayType(newOperatorSpecifiedType);

       // DQ (1/8/2020): If this is not an array type, then we will need additional parenthesizes.
          if (newOperatorArrayType == NULL)
             {
               add_parenthesis_around_type = true;
             }
        }

     if (add_parenthesis_around_type == true)
        {
          curprint ("( ");
        }

  // DQ (4/16/2019): Added support for name qualification implementation.
  // newinfo.set_name_qualification_length(new_op->get_name_qualification_length());
  // newinfo.set_global_qualification_required(new_op->get_global_qualification_required());
  // newinfo.set_type_elaboration_required(new_op->get_type_elaboration_required());

  // DQ (4/16/2019): Added support for name qualification.
     newinfo.set_reference_node_for_qualification(new_op);
     ASSERT_not_null(newinfo.get_reference_node_for_qualification());

  // DQ (3/26/2012): Turn this OFF to avoid output fo the class name twice (if the constructor is available).
  // DQ (1/17/2006): The the type specified explicitly in the new expressions syntax, 
  // get_type() has been modified to return a pointer to new_op->get_specified_type().
  // unp->u_type->unparseType(new_op->get_type(), newinfo);
     unp->u_type->unparseType(new_op->get_specified_type(), newinfo);

  // printf ("DONE: new_op->get_type()->class_name() = %s \n",new_op->get_type()->class_name().c_str());

     if (add_parenthesis_around_type == true)
        {
          curprint (") ");
        }

#if DEBUG_NEW_OPERATOR
     curprint ("\n /* Output constructor args */ \n");
#endif

     if (new_op->get_constructor_args() != NULL)
        {
       // printf ("In Unparse_ExprStmt::unparseNewOp: Now unparse new_op->get_constructor_args() \n");
          unparseExpression(new_op->get_constructor_args(), newinfo);
        }
#if 0
    // DQ (3/26/2012): See not above about why we can't use the constructor's class name.
    // DQ (3/26/2012): Turn this ON to avoid skipping output for the class name (if the constructor is available).
       else
        {
       // printf ("In Unparse_ExprStmt::unparseNewOp: Call unparse type \n");
       // unp->u_type->unparseType(new_op->get_type(), newinfo);
          unp->u_type->unparseType(new_op->get_specified_type(), newinfo);
        }
#endif

#if DEBUG_NEW_OPERATOR
     curprint ("\n /* Output builtin args */ \n");
#endif

     if (new_op->get_builtin_args() != NULL)
        {
       // printf ("In Unparse_ExprStmt::unparseNewOp: Now unparse new_op->get_builtin_args() \n");
          unparseExpression(new_op->get_builtin_args(), newinfo);
        }

#if DEBUG_NEW_OPERATOR
     curprint ("\n /* Leaving Unparse_ExprStmt::unparseNewOp */ \n");
     printf ("Leaving Unparse_ExprStmt::unparseNewOp \n");
#endif

// #endif for CXX_IS_ROSE_CODE_GENERATION
#endif
   }


void
Unparse_ExprStmt::unparseDeleteOp(SgExpression* expr, SgUnparse_Info& info)
   {
     SgDeleteExp* delete_op = isSgDeleteExp(expr);
     ASSERT_not_null(delete_op);

     if (delete_op->get_need_global_specifier())
        {
          curprint ( ":: ");
        }
     curprint ( "delete ");
     SgUnparse_Info newinfo(info);
     if (delete_op->get_is_array())
        {
          curprint ( "[]");
        }
     unparseExpression(delete_op->get_variable(), newinfo);
   }


void
Unparse_ExprStmt::unparseThisNode(SgExpression* expr, SgUnparse_Info& info) 
   {
     SgThisExp* this_node = isSgThisExp(expr);

     ASSERT_not_null(this_node);

  // printf ("In Unparse_ExprStmt::unparseThisNode: unp->opt.get_this_opt() = %s \n", (unp->opt.get_this_opt()) ? "true" : "false");

     if (unp->opt.get_this_opt()) // Checks options to determine whether to print "this"  
        {
          curprint ( "this"); 
        }
   }

void
Unparse_ExprStmt::unparseScopeOp(SgExpression* expr, SgUnparse_Info& info)
   {
     SgScopeOp* scope_op = isSgScopeOp(expr);
     ASSERT_not_null(scope_op);

     if (scope_op->get_lhs_operand())
          unparseExpression(scope_op->get_lhs_operand(), info);
     curprint ( "::");
     unparseExpression(scope_op->get_rhs_operand(), info);
   }

void Unparse_ExprStmt::unparseAssnOp(SgExpression* expr, SgUnparse_Info& info)       { unparseBinaryOperator(expr, "=",   info); }
void Unparse_ExprStmt::unparsePlusAssnOp(SgExpression* expr, SgUnparse_Info& info)   { unparseBinaryOperator(expr, "+=",  info); } 
void Unparse_ExprStmt::unparseMinusAssnOp(SgExpression* expr, SgUnparse_Info& info)  { unparseBinaryOperator(expr, "-=",  info); }
void Unparse_ExprStmt::unparseAndAssnOp(SgExpression* expr, SgUnparse_Info& info)    { unparseBinaryOperator(expr, "&=",  info); }
void Unparse_ExprStmt::unparseIOrAssnOp(SgExpression* expr, SgUnparse_Info& info)    { unparseBinaryOperator(expr, "|=",  info); }
void Unparse_ExprStmt::unparseMultAssnOp(SgExpression* expr, SgUnparse_Info& info)   { unparseBinaryOperator(expr, "*=",  info); }
void Unparse_ExprStmt::unparseDivAssnOp(SgExpression* expr, SgUnparse_Info& info)    { unparseBinaryOperator(expr, "/=",  info); }
void Unparse_ExprStmt::unparseModAssnOp(SgExpression* expr, SgUnparse_Info& info)    { unparseBinaryOperator(expr, "%=",  info); }
void Unparse_ExprStmt::unparseXorAssnOp(SgExpression* expr, SgUnparse_Info& info)    { unparseBinaryOperator(expr, "^=",  info); }

void Unparse_ExprStmt::unparseLShiftAssnOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "<<=",  info); }
void Unparse_ExprStmt::unparseRShiftAssnOp(SgExpression* expr, SgUnparse_Info& info) 
   {
#if 0
     printf ("In unparseRShiftAssnOp() \n");
#endif
     unparseBinaryOperator(expr, ">>=", info); 
   }

void Unparse_ExprStmt::unparseForDeclOp(SgExpression* expr, SgUnparse_Info& info) {}

void
Unparse_ExprStmt::unparseTypeRef(SgExpression* expr, SgUnparse_Info& info)
   {
     SgRefExp* type_ref = isSgRefExp(expr);
     ASSERT_not_null(type_ref);

     SgUnparse_Info newinfo(info);
     newinfo.unset_PrintName();
     newinfo.unset_isTypeFirstPart();
     newinfo.unset_isTypeSecondPart();
  
     unp->u_type->unparseType(type_ref->get_type_name(), newinfo);
   }

void Unparse_ExprStmt::unparseVConst(SgExpression* expr, SgUnparse_Info& info) {}
void Unparse_ExprStmt::unparseExprInit(SgExpression* expr, SgUnparse_Info& info) {}


// Liao 11/3/2010
// Sometimes initializers can from an included file
//    SgAssignInitializer -> SgCastExp ->SgCastExp ->SgIntVal
// We should not unparse them
// This function will check if the nth initializer is from a different file from the aggregate initializer
static bool isFromAnotherFile (SgLocatedNode* lnode)
   {
     bool result = false;
     ASSERT_not_null(lnode);

#if 0
     printf ("TOP of isFromAnotherFile(SgLocatedNode* lnode = %p = %s): result = %s \n",lnode,lnode->class_name().c_str(),result ? "true" : "false");
#endif

  // Liao 11/22/2010, a workaround for enum value constant assign initializer
  // EDG passes the source location information of the original declaration of the enum value, not the location for the value's reference
  // So SgAssignInitializer has wrong file info.
  // In this case, we look down to the actual SgEnumVal for the file info instead of looking at its ancestor SgAssignInitializer  
     SgAssignInitializer *a_initor = isSgAssignInitializer (lnode);
     if (a_initor != NULL)
        {
          result = false;
          SgExpression * leaf_child = a_initor->get_operand_i();
          while (SgCastExp * cast_op = isSgCastExp(leaf_child))
             {
            // redirect to original expression tree if possible
               if (cast_op->get_originalExpressionTree() != NULL)
                    leaf_child = cast_op->get_originalExpressionTree();
                 else
                    leaf_child = cast_op->get_operand_i();
             }

       // if (isSgEnumVal(leaf_child))
          lnode = leaf_child;
        }

  // DQ (11/2/2012): This seems like it could be kind of expensive for each expression...
     SgStatement* cur_stmt = SageInterface::getEnclosingStatement(lnode);
     ASSERT_not_null(cur_stmt);

  // SgFile* cur_file = SageInterface::getEnclosingFileNode(lnode);
     SgFile* cur_file = SageInterface::getEnclosingFileNode(cur_stmt);

     if (cur_file != NULL)
        {
       // DQ (9/1/2013): Changed predicate to test for isCompilerGenerated() instead of isOutputInCodeGeneration().
       // The proper source position is now set for the children of SgAssignInitializer.

       // normal file info 
       // if (lnode->get_file_info()->isTransformation() == false && lnode->get_file_info()->isCompilerGenerated() == false)
       // if (lnode->get_file_info()->isTransformation() == false && lnode->get_file_info()->isOutputInCodeGeneration() == false)
          if (lnode->get_file_info()->isTransformation() == false && lnode->get_file_info()->isCompilerGenerated() == false)
             {
            // DQ (9/5/2013): We need to use the Sg_File_Info::get_physical_filename() instead of Sg_File_Info::get_filenameString() 
            // because the file might have #line directives (see test2013_52.c).  Note that this much match the cur_file's get_physical_filename().
            // if (lnode->get_file_info()->get_filenameString() != "" && cur_file->get_file_info()->get_filenameString() != lnode->get_file_info()->get_filenameString())
            // if (lnode->get_file_info()->get_filenameString() != "" && lnode->get_file_info()->get_filenameString() != "NULL_FILE" && cur_file->get_file_info()->get_filenameString() != lnode->get_file_info()->get_filenameString())
            // if (lnode->get_file_info()->get_physical_filename() != "" && lnode->get_file_info()->get_physical_filename() != "NULL_FILE" && cur_file->get_file_info()->get_filenameString() != lnode->get_file_info()->get_physical_filename())
               if (lnode->get_file_info()->get_physical_filename() != "" && lnode->get_file_info()->get_physical_filename() != "NULL_FILE" && cur_file->get_file_info()->get_physical_filename() != lnode->get_file_info()->get_physical_filename())
                  {
                    result = true;
#if 0
                    printf ("In isFromAnotherFile(SgLocatedNode* lnode): lnode->get_file_info()        = %p \n",lnode->get_file_info());
                    printf ("In isFromAnotherFile(SgLocatedNode* lnode): lnode->get_startOfConstruct() = %p \n",lnode->get_startOfConstruct());
                    printf ("In isFromAnotherFile(SgLocatedNode* lnode): lnode->get_endOfConstruct()   = %p \n",lnode->get_endOfConstruct());
                    ASSERT_not_null(cur_file->get_file_info());
                    ASSERT_not_null(lnode->get_file_info());
                    printf ("In isFromAnotherFile(SgLocatedNode* lnode): cur_file->get_file_info()->get_filenameString() = %s \n",cur_file->get_file_info()->get_filenameString().c_str());
                    printf ("In isFromAnotherFile(SgLocatedNode* lnode): lnode->get_file_info()->get_filenameString()    = %s \n",lnode->get_file_info()->get_filenameString().c_str());
                    lnode->get_file_info()->display("In isFromAnotherFile(): get_file_info(): returning TRUE: debug");
                    lnode->get_startOfConstruct()->display("In isFromAnotherFile(): get_startOfConstruct(): returning TRUE: debug");
#endif
                  }
             }
        }

#if 0
  // DQ (9/1/2013): Now that we have modified EDG to supported the source position information in constants in the 
  // SgAssignInitializer (in the case of SgAggregateInitializers), we can use the code above (finally) to suppress 
  // the unparsing of the aggregate initializers when they were from a different include file (see test2013_05.c 
  // as an example).

  // DQ (11/4/2012): This is a bit too much trouble to support at the moment. I'm am getting tired of fighting it.
  // This is causing too many things to not be unparsed and needs to be iterated on when we are a bit further along
  // and under less pressure.  this support for expression level selection of what to unparse is only addrressing
  // a narrow part of the unparsing of expressions.  I understand that this is an attempt to support the case where
  // an initializer list comes from an include file, but that is a bit more of an advanced concept that I don't 
  // want to be forced to support presently (it also have many details and only general cases of this problem can
  // be addressed since we don't have the parse tree (and I don't want to query the tokens)).
     if (result == true)
        {
       // Let this warning be output, but infrequently...
          static int counter = 1;

          if (counter++ % 100 == 0)
             {
               printf ("Note: In unparseCxx_expressions.C: isFromAnotherFile(): forcing this function to return false for initializer list unparsing \n");
#if 0
               printf ("Leaving isFromAnotherFile(SgLocatedNode* lnode = %p = %s): result = %s \n",lnode,lnode->class_name().c_str(),result ? "true" : "false");
#endif
             }
          result = false;
        }
#endif

#if 0
     printf ("In isFromAnotherFile(SgLocatedNode* lnode = %p = %s): result = %s \n",lnode,lnode->class_name().c_str(),result ? "true" : "false");
#endif

     return result;
   }


#if 0
static bool isFromAnotherFile(SgAggregateInitializer * aggr_init, size_t n)
{
  bool result = false; 
  ASSERT_not_null(aggr_init);
  size_t m_size = (aggr_init->get_initializers()->get_expressions()).size();
  ROSE_ASSERT (n <= m_size);

  SgExpression* initializer = (aggr_init->get_initializers()->get_expressions())[n];
  ASSERT_not_null(initializer);

   // Try to get the bottom leaf child
  SgAssignInitializer * a_initor = isSgAssignInitializer (initializer);
  if (a_initor)
  {
    SgExpression * leaf_child = a_initor->get_operand_i();
    while (SgCastExp * cast_op = isSgCastExp(leaf_child))
    { 
      // redirect to original expression tree if possible
      if (cast_op->get_originalExpressionTree() != NULL)
      leaf_child = cast_op->get_originalExpressionTree();
      else
        leaf_child = cast_op->get_operand_i();
    }
    // compare file names
     SgFile* cur_file = SageInterface::getEnclosingFileNode(aggr_init);
     if (cur_file != NULL)
     {
       // normal file info 
       if (leaf_child->get_file_info()->isTransformation() == false &&  leaf_child->get_file_info()->isCompilerGenerated() ==false)
       {
         if (cur_file->get_file_info()->get_filename() != leaf_child->get_file_info()->get_filename())
           result = true;
       }
     } //
  } // end if assign initializer 
  return result; 
}
#endif


static bool
sharesSameStatement(SgExpression* expr, SgType* expressionType)
   {
  // DQ (7/29/2013): This function supports the structural analysis to determine when we have to output the type definition 
  // or just the type name for a compound literal.

     bool result = false;
     SgNamedType* namedType = isSgNamedType(expressionType);
#if 1
  // DQ (9/14/2013): I think this is a better implementation (test2012_46.c was failing before).
  // This permits both test2013_70.c and test2012_46.c to unparse properly, where the two were 
  // previously mutually exclusive.
     if (namedType != NULL)
        {
          ASSERT_not_null(namedType->get_declaration());
          SgDeclarationStatement* declarationStatementDefiningType = namedType->get_declaration();
          SgDeclarationStatement* definingDeclarationStatementDefiningType = declarationStatementDefiningType->get_definingDeclaration();
          SgClassDeclaration* classDeclaration = isSgClassDeclaration(definingDeclarationStatementDefiningType);

       // printf ("classDeclaration = %p \n",classDeclaration);

       // DQ (3/26/2015): Need to handle case where isAutonomousDeclaration() == true, but we still don't want to unparse the definition.
       // bool isDeclarationPartOfVariableDeclaration = isSgVariableDeclaration(classDeclaration->get_parent());
          bool isDeclarationPartOfTypedefDeclaration  = false;
          bool isDeclarationPartOfVariableDeclaration = false;
          if (classDeclaration != NULL)
             {
               isDeclarationPartOfVariableDeclaration = isSgVariableDeclaration(classDeclaration->get_parent());
               isDeclarationPartOfTypedefDeclaration  = isSgTypedefDeclaration(classDeclaration->get_parent());
             }

       // if (classDeclaration != NULL && classDeclaration->get_isAutonomousDeclaration() == false)
       // if (classDeclaration != NULL && classDeclaration->get_isAutonomousDeclaration() == false && isDeclarationPartOfVariableDeclaration == false)
          if (classDeclaration != NULL && classDeclaration->get_isAutonomousDeclaration() == false && isDeclarationPartOfVariableDeclaration == false && isDeclarationPartOfTypedefDeclaration == false)
             {
            // This declaration IS defined imbedded in another statement.
#if 0
               printf ("This class declaration IS defined embedded in another statement. \n");
#endif
               result = true;
             }
            else
             {
            // This declaration is NOT defined imbedded in another statement.
#if 0
               printf ("This is not a class declaration OR the declaration is NOT defined embedded in another statement. \n");
#endif
               result = false;
             }
        }
#else
     SgStatement* statementDefiningType         = NULL;
     if (namedType != NULL)
        {
          ASSERT_not_null(namedType->get_declaration());

#error "DEAD CODE!"

       // DQ (9/14/2013): If this is a scope statement then we want to use the declaration directly (else everything 
       // will be an ancestor in the case of this being a global scope).  See test2013_70.c for what we need this.
       // statementDefiningType = TransformationSupport::getStatement(namedType->get_declaration()->get_parent());
          if (isSgScopeStatement(namedType->get_declaration()->get_parent()) != NULL)
             {
            // The declaration for the type is declared in a scope and not as part of being embedded in another statement.
            // statementDefiningType = TransformationSupport::getStatement(namedType->get_declaration());
               statementDefiningType = namedType->get_declaration();
             }
            else
             {
            // This is the case of the type embedded in another statement (e.g. for a GNU statement expression).
               statementDefiningType = TransformationSupport::getStatement(namedType->get_declaration()->get_parent());
             }
        }

     if (statementDefiningType != NULL)
        {
#if 0
          printf ("Calling SageInterface::isAncestor(): statementDefiningType = %p = %s \n",statementDefiningType,statementDefiningType->class_name().c_str());
          printf ("Calling SageInterface::isAncestor(): expr                  = %p = %s \n",expr,expr->class_name().c_str());
#endif
          result = SageInterface::isAncestor(statementDefiningType,expr);
        }
#endif

#if 0
     printf ("In sharesSameStatement(SgExpression* expr, SgType* expressionType): result = %s \n",result ? "true" : "false");
     printf ("   --- statementDefiningType = %p = %s \n",statementDefiningType,(statementDefiningType == NULL) ? "null" : statementDefiningType->class_name().c_str());
#endif

     return result;
   }


static bool
containsIncludeDirective(SgLocatedNode* locatedNode)
   {
     bool returnResult = false;
     AttachedPreprocessingInfoType* comments = locatedNode->getAttachedPreprocessingInfo();

     if (comments != NULL)
        {
#if 0
          printf ("Found attached comments (at %p of type: %s): \n",locatedNode,locatedNode->class_name().c_str());
#endif
          AttachedPreprocessingInfoType::iterator i;
          for (i = comments->begin(); i != comments->end(); i++)
             {
               ASSERT_not_null((*i));
#if 0
               printf ("          Attached Comment (relativePosition=%s): %s\n",
                    ((*i)->getRelativePosition() == PreprocessingInfo::before) ? "before" : "after",
                    (*i)->getString().c_str());
               printf ("Comment/Directive getNumberOfLines = %d getColumnNumberOfEndOfString = %d \n",(*i)->getNumberOfLines(),(*i)->getColumnNumberOfEndOfString());
               (*i)->get_file_info()->display("comment/directive location");
#endif
               if (locatedNode->get_startOfConstruct()->isSameFile((*i)->get_file_info()) == true)
                  {
                 // This should also be true.
                    ROSE_ASSERT(locatedNode->get_endOfConstruct()->isSameFile((*i)->get_file_info()) == true);
                  }

               if ( *(locatedNode->get_startOfConstruct()) <= *((*i)->get_file_info()) && *(locatedNode->get_endOfConstruct()) >= *((*i)->get_file_info()) )
                  {
                 // Then the comment is in between the start of the construct and the end of the construct.
                    if ((*i)->getTypeOfDirective() == PreprocessingInfo::CpreprocessorIncludeDeclaration)
                       {
                         returnResult = true;
                       }
                  }
             }
        }
       else
        {
#if 0
          printf ("In containsIncludeDirective(): No attached comments (at %p of type: %s): \n",locatedNode,locatedNode->sage_class_name());
#endif
        }

#if 0
     printf ("In containsIncludeDirective(): returnResult = %s locatedNode = %p = %s \n",returnResult ? "true" : "false",locatedNode,locatedNode->sage_class_name());
#endif

     return returnResult;
   }


static void
removeIncludeDirective(SgLocatedNode* locatedNode)
   {
//   bool returnResult = false;
     AttachedPreprocessingInfoType* comments = locatedNode->getAttachedPreprocessingInfo();
     AttachedPreprocessingInfoType includeDirectiveList;

     if (comments != NULL)
        {
#if 0
          printf ("Found attached comments (at %p of type: %s): \n",locatedNode,locatedNode->class_name().c_str());
#endif
          for (AttachedPreprocessingInfoType::iterator i = comments->begin(); i != comments->end(); i++)
             {
               ASSERT_not_null((*i));
#if 0
               printf ("          Attached Comment (relativePosition=%s): %s\n",
                    ((*i)->getRelativePosition() == PreprocessingInfo::before) ? "before" : "after",
                    (*i)->getString().c_str());
               printf ("Comment/Directive getNumberOfLines = %d getColumnNumberOfEndOfString = %d \n",(*i)->getNumberOfLines(),(*i)->getColumnNumberOfEndOfString());
               (*i)->get_file_info()->display("comment/directive location");
#endif
               if (locatedNode->get_startOfConstruct()->isSameFile((*i)->get_file_info()) == true)
                  {
                 // This should also be true.
                    ROSE_ASSERT(locatedNode->get_endOfConstruct()->isSameFile((*i)->get_file_info()) == true);
                  }

               if ( *(locatedNode->get_startOfConstruct()) <= *((*i)->get_file_info()) && *(locatedNode->get_endOfConstruct()) >= *((*i)->get_file_info()) )
                  {
                 // Then the comment is in between the start of the construct and the end of the construct.
                    if ((*i)->getTypeOfDirective() == PreprocessingInfo::CpreprocessorIncludeDeclaration)
                       {
#if 0
                         printf ("Found cpp include directive \n");
#endif
//                       returnResult = true;
                         includeDirectiveList.push_back(*i);
                       }
                  }
             }

       // Remove the list of include directives.
          for (AttachedPreprocessingInfoType::iterator i = includeDirectiveList.begin(); i != includeDirectiveList.end(); i++)
             {
#if 0
               printf ("In removeIncludeDirective(): Removing cpp include directive \n");

               printf ("     Remove Comment (relativePosition=%s): %s\n",((*i)->getRelativePosition() == PreprocessingInfo::before) ? "before" : "after",(*i)->getString().c_str());
               printf ("     Remove Comment/Directive getNumberOfLines = %d getColumnNumberOfEndOfString = %d \n",(*i)->getNumberOfLines(),(*i)->getColumnNumberOfEndOfString());
               (*i)->get_file_info()->display("remove comment/directive location");
#endif
               comments->erase(find(comments->begin(),comments->end(),*i));
             }
        }
       else
        {
#if 0
          printf ("In removeIncludeDirective(): No attached comments (at %p of type: %s): \n",locatedNode,locatedNode->sage_class_name());
#endif
        }
   }


bool uses_cxx11_initialization (SgNode* n)
   {
  // See what the structure of this initialization is to see if it is using the C++11 initialization features for structs.

  // SgInitializer* initializerChain[3] = { NULL, NULL, NULL };
     std::vector<SgInitializer*> initializerChain;
     bool returnValue = false;

#if 0
     printf ("In uses_cxx11_initialization(): n = %p = %s \n",n,n->class_name().c_str());
#endif

#if 0
     class InitializerTraversal : public AstSimpleProcessing
        {
          private:
               std::vector<SgInitializer*> & initializerChain;
               int counter;

          public:
               InitializerTraversal(std::vector<SgInitializer*> & x) : initializerChain(x), counter(0) {}
               void visit (SgNode* node)
                  {
                    ASSERT_not_null(node);
#if 0
                    printf ("In InitializerTraversal::visit(): node = %p = %s \n",node,node->class_name().c_str());
#endif
                    SgInitializer* initializer = isSgInitializer(node);
                    if (initializer != NULL && counter < 3)
                       {
                      // initializerChain[counter] = initializer;
                         initializerChain.push_back(initializer);
                         counter++;
                       }
                  }
        };

  // Now buid the traveral object and call the traversal (preorder) on the AST subtree.
     InitializerTraversal traversal(initializerChain);
     traversal.traverse(n, preorder);
#else
  // This version starts at the second in the chain of three SgAggregateInitializer IR nodes.
  // It initializes the first intry in the chain through accessing the parents and the last two 
  // from the current node and it's children.

     class InitializerTraversal : public AstSimpleProcessing
        {
          private:
               std::vector<SgInitializer*> & initializerChain;
               int counter;

          public:
               InitializerTraversal(std::vector<SgInitializer*> & x) : initializerChain(x), counter(1) {}
               void visit (SgNode* node)
                  {
                    ASSERT_not_null(node);
#if 0
                    printf ("In InitializerTraversal::visit(): node = %p = %s \n",node,node->class_name().c_str());
#endif
                    SgInitializer* initializer = isSgInitializer(node);
                    if (initializer != NULL && counter < 3)
                       {
                      // initializerChain[counter] = initializer;
                         initializerChain.push_back(initializer);
                         counter++;
                       }
                  }
        };


     SgExprListExp* expressonList = isSgExprListExp(n->get_parent());
     if (expressonList != NULL)
        {
          SgAggregateInitializer* aggregateInitializer = isSgAggregateInitializer(expressonList->get_parent());
          if (aggregateInitializer != NULL)
             {
               initializerChain.push_back(aggregateInitializer);

               ROSE_ASSERT(initializerChain.size() == 1);

            // Now buid the traveral object and call the traversal (preorder) on the AST subtree.
               InitializerTraversal traversal(initializerChain);
               traversal.traverse(n, preorder);

             }
        }
#endif

#if 0
     printf ("In uses_cxx11_initialization(): initializerChain.size() = %lu \n",initializerChain.size());
#endif

     ROSE_ASSERT(initializerChain.size() <= 3);

     if (initializerChain.size() == 3)
        {
       // Check the order of the initializers.
          SgAggregateInitializer* agregateInitializer_0 = isSgAggregateInitializer(initializerChain[0]);
          SgAggregateInitializer* agregateInitializer_1 = isSgAggregateInitializer(initializerChain[1]);
          SgAggregateInitializer* agregateInitializer_2 = isSgAggregateInitializer(initializerChain[2]);
#if 0
          printf ("agregateInitializer_0 = %p = %s \n",agregateInitializer_0,initializerChain[0]->class_name().c_str());
          printf ("agregateInitializer_1 = %p = %s \n",agregateInitializer_1,initializerChain[1]->class_name().c_str());
          printf ("agregateInitializer_2 = %p = %s \n",agregateInitializer_2,initializerChain[2]->class_name().c_str());
#endif
          if (agregateInitializer_0 != NULL && agregateInitializer_1 != NULL && agregateInitializer_2 != NULL)
             {
               SgType* agregateInitializer_type_0 = isSgClassType(agregateInitializer_0->get_type());
               SgType* agregateInitializer_type_1 = isSgArrayType(agregateInitializer_1->get_type());
               SgType* agregateInitializer_type_2 = isSgClassType(agregateInitializer_2->get_type());
#if 0
               printf ("agregateInitializer_type_0 = %p = %s \n",agregateInitializer_type_0,agregateInitializer_0->get_type()->class_name().c_str());
               printf ("agregateInitializer_type_1 = %p = %s \n",agregateInitializer_type_1,agregateInitializer_1->get_type()->class_name().c_str());
               printf ("agregateInitializer_type_2 = %p = %s \n",agregateInitializer_type_2,agregateInitializer_2->get_type()->class_name().c_str());
#endif
               SgClassType* classType_0 = isSgClassType(agregateInitializer_type_0);
               SgArrayType* arrayType_1 = isSgArrayType(agregateInitializer_type_1);
               SgClassType* classType_2 = isSgClassType(agregateInitializer_type_2);

               if (classType_0 != NULL && arrayType_1 != NULL && classType_2 != NULL)
                  {
#if 0
                    printf ("Found C++11 specific initializer chain that requires use of class specifier \n");
#endif
                    returnValue = true;
                  }
                 else
                  {
#if 0
                    printf ("No C++11 class specification required for this initializer chain \n");
#endif
                  }
             }
        }


#if 0
  // DQ (6/27/2018): Debugging test2018_113.C.
     if (initializerChain.size() == 0)
        {
#if 0
          printf ("Found C++11 specific initializer chain size 0: that requires use of class specifier \n");
#endif
       // This might be overly general.
          returnValue = true;
        }
#endif

  // DQ (6/27/2018): check if this is part of a template instantiation that would require the class name in the initializer to trigger the instantiation.
     SgAggregateInitializer* aggregateInitializer = isSgAggregateInitializer(n);
     if (aggregateInitializer != NULL)
        {
          SgExprListExp* expListExp = isSgExprListExp(aggregateInitializer->get_parent());
          if (expListExp != NULL)
             {
               SgFunctionCallExp* functionCall = isSgFunctionCallExp(expListExp->get_parent());

               if (functionCall != NULL)
                  {
                    SgExpression* functionExpression = functionCall->get_function();
#if 0
                    printf ("functionExpression = %p = %s \n",functionExpression,functionExpression->class_name().c_str());
#endif
                    SgFunctionRefExp*       functionRefExp       = isSgFunctionRefExp(functionCall->get_function());
                    SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(functionCall->get_function());

                    SgArrowExp* arrowExpression = isSgArrowExp(functionExpression);
                    SgDotExp*   dotExpression   = isSgDotExp(functionExpression);

                    SgExpression* rhs = NULL;
                    if (arrowExpression != NULL)
                       {
                         rhs = arrowExpression->get_rhs_operand();
                       }
                    if (dotExpression != NULL)
                       {
                         rhs = dotExpression->get_rhs_operand();
                       }

                    if (functionRefExp == NULL && memberFunctionRefExp == NULL)
                       {
                      // Get the pointer from the rhs.
                         functionRefExp       = isSgFunctionRefExp(rhs);
                         memberFunctionRefExp = isSgMemberFunctionRefExp(rhs);
                       }

                 // DQ (6/27/2018): This assertion fails for test2018_111.C.
                 // ASSERT_not_null(functionRefExp);

                    SgFunctionDeclaration* functionDeclaration = NULL;
                    if (functionRefExp != NULL)
                       {
                         SgFunctionSymbol* functionSymbol = functionRefExp->get_symbol();
#if 0
                         printf ("functionSymbol = %p = %s \n",functionSymbol,functionSymbol->class_name().c_str());
#endif
                      // SgFunctionDeclaration* functionDeclaration = functionSymbol->get_declaration();
                         functionDeclaration = functionSymbol->get_declaration();
                       }

                    if (memberFunctionRefExp != NULL)
                       {
                         SgMemberFunctionSymbol* functionSymbol = memberFunctionRefExp->get_symbol();
#if 0
                         printf ("member functionSymbol = %p = %s \n",functionSymbol,functionSymbol->class_name().c_str());
#endif
                      // SgFunctionDeclaration* functionDeclaration = functionSymbol->get_declaration();
                         functionDeclaration = functionSymbol->get_declaration();
                       }

                    if (functionDeclaration != NULL)
                       {

                         bool isTemplateInstantiation = SageInterface::isTemplateInstantiationNode(functionDeclaration);
#if 0
                         printf ("isTemplateInstantiation = %s \n",isTemplateInstantiation ? "true" : "false");
#endif
                         if (isTemplateInstantiation == true)
                            {
#if 0
                              printf ("Found C++11 specific initializer function (%s) argument to template instantiation: that requires use of class specifier \n",functionDeclaration->class_name().c_str());
#endif
                           // This might be overly general.
                              returnValue = true;
#if 0
                              printf ("Exiting as a test! \n");
                              ROSE_ASSERT(false);
#endif
                            }
                       }
                  }
             }
        }

#if 0
     printf ("Leaving uses_cxx11_initialization(): n = %p = %s returnValue = %s \n",n,n->class_name().c_str(),returnValue ? "true" : "false");
#endif

     return returnValue;
   }


void
Unparse_ExprStmt::unparseAggrInit(SgExpression* expr, SgUnparse_Info& info)
   {
     SgAggregateInitializer* aggr_init = isSgAggregateInitializer(expr);
     ASSERT_not_null(aggr_init);

#if 0
  // DQ (1/26/2014): This would not be as portabile of a solution, but it is more robus in other ways. So this is a compromize.
  // DQ (9/11/2013): It is a better solution to always unparse the ggregate attributes and to remove the #include that would be a problem.
  // Skip the entire thing if the initializer is from an included file
     if (isFromAnotherFile (expr))
        {
          printf ("In unparseAggrInit(): This SgAggregateInitializer (aggr_init = %p) is from another file so its subtree will not be output in the generated code \n",aggr_init);

          return;
        }
#endif

#if 0
  // DQ (5/17/2019): Moved from the top of the function to be closer to where it is used.
     SgUnparse_Info newinfo(info);

  // DQ (5/17/2019): Set this so that we can know to skip the output of type elaboration.
     newinfo.set_inAggregateInitializer();
#endif

     static int depth = 0;

     depth++;

#define DEBUG_AGGREGATE_INITIALIZER 0

#if DEBUG_AGGREGATE_INITIALIZER || 0
     printf ("\nIn unparseAggrInit(): aggr_init = %p = %s aggr_init->get_uses_compound_literal() = %s \n",aggr_init,aggr_init->class_name().c_str(),aggr_init->get_uses_compound_literal() ? "true" : "false");
     curprint ("/* In unparseAggrInit() */ ");
#endif

#if 1
  // See what the structure of this initialization is to see if it is using the C++11 initialization features for structs.
     bool need_cxx11_class_specifier = uses_cxx11_initialization (expr);

  // DQ (4/12/2018): Check if this is a C++11 file (just to make sure), see C_tests/test2018_35.c).
     SgSourceFile* sourceFile = info.get_current_source_file();
  // ASSERT_not_null(sourceFile);

#if 0
     printf("In Unparse_ExprStmt::unparseAggrInit\n");
     printf("  -- sourceFile = %p\n", sourceFile);
     printf("     -- Cxx11_only = %s\n", sourceFile->get_Cxx11_only() ? "true" : "false");
     printf("     -- Cxx14_only = %s\n", sourceFile->get_Cxx14_only() ? "true" : "false");
     printf("  -- need_cxx11_class_specifier = %s\n", need_cxx11_class_specifier ? "true" : "false");
#endif

  // TV (08/17/2018): sourceFile is NULL when called from unparseToString
  //                  FIXME will it be needed with C++ 14 and 17 ???
     if ( (sourceFile != NULL) && !( sourceFile->get_Cxx11_only() || sourceFile->get_Cxx14_only() ) )
        {
          need_cxx11_class_specifier = false;
        }

#if 0
     printf("  -- need_cxx11_class_specifier = %s\n", need_cxx11_class_specifier ? "true" : "false");
#endif

#if 0
     printf ("DONE: Calling uses_cxx11_initialization: expr = %p type = %p = %s need_cxx11_class_specifier = %s \n",
          expr,expr->get_type(),expr->get_type()->class_name().c_str(),need_cxx11_class_specifier ? "true" : "false");
#endif
#if 0
     printf ("&&&&&&&&&&&&&&&&&&& In unparseAggrInit(): depth = %d need_cxx11_class_specifier = %s \n",depth,need_cxx11_class_specifier ? "true" : "false");
#endif
#endif

  // DQ (3/21/2018): For testing this should work on test2018_59.C for the first case.
  // ROSE_ASSERT (need_cxx11_class_specifier == true);

  // DQ  (3/12/2018): Moved to outer functions scope so that we can reuse it in the loop over initializers.
     SgUnparse_Info newinfo2(info);

  // DQ (5/17/2019): Set this so that we can know to skip the output of type elaboration.
     newinfo2.set_inAggregateInitializer();

  // DQ (7/27/2013): Added support for aggregate initializers.
     if (aggr_init->get_uses_compound_literal() == true)
        {
       // This aggregate initializer is using a compound literal and so we need to output the type.
       // This looks like an explict cast, but is not a cast internally in the language, just that
       // this is how compound literals are supposed to be handled.
#if 0
          printf ("NOTE: In unparseAggrInit(): compound literal detected: Need to output the name of the type: aggr_init->get_type() = %p = %s \n",aggr_init->get_type(),aggr_init->get_type()->class_name().c_str());
          curprint ("/* output type in unparseAggrInit() */ ");
#endif
       // DQ (7/29/2013): If we have this code then we will pass test2012_47.c but fail test2013_27.c, else we fail test2013_27.c, but fail test2012_47.c.
       // To resolve if we want to unparse the type definition or not, we have to check if the type's definition is defined in the same statement as the 
       // SgAggregateInitializer (shares the same parent statement).
       // SgUnparse_Info newinfo(info);
       // newinfo.unset_SkipClassDefinition();

       // DQ  (3/12/2018): Moved to outer functions scope so that we can reuse it in the loop over initializers.
       // SgUnparse_Info newinfo2(info);

       // DQ (3/12/2018): Rewrite this so that we can output the calue of "shared".
       // if (sharesSameStatement(aggr_init,aggr_init->get_type()) == true)
          bool shares = (sharesSameStatement(aggr_init,aggr_init->get_type()) == true);
#if 0
          printf ("In unparseAggrInit(): shares = %s \n",shares ? "true" : "false");
#endif
          if (shares == true)
             {
#if 0
               printf ("sharesSameStatement(aggr_init,aggr_init->get_type()) == true) \n");
#endif
               newinfo2.unset_SkipClassDefinition();

            // DQ (1/9/2014): We have to make the handling of enum definitions consistant with that of class definitions.
               newinfo2.unset_SkipEnumDefinition();
#if 0
               printf ("In unparseAggrInit(): newinfo2.SkipClassDefinition() = %s \n",(newinfo2.SkipClassDefinition() == true) ? "true" : "false");
               printf ("In unparseAggrInit(): newinfo2.SkipEnumDefinition()  = %s \n",(newinfo2.SkipEnumDefinition() == true) ? "true" : "false");
#endif
            // DQ (1/9/2014): These should have been setup to be the same.
               ROSE_ASSERT(newinfo2.SkipClassDefinition() == newinfo2.SkipEnumDefinition());
             }
            else
             {
#if 0
               printf ("sharesSameStatement(aggr_init,aggr_init->get_type()) == false) \n");
#endif
#if 0
               printf ("In unparseAggrInit(): aggr_init->get_uses_compound_literal() == true: Skipping the class definition if it is not in the expression. \n");
#endif
            // DQ (3/26/2015): Skip the class definition if it is not in the expression.
               newinfo2.set_SkipClassDefinition();
               newinfo2.set_SkipEnumDefinition();
#if 0
               printf ("In unparseAggrInit(): newinfo2.SkipClassDefinition() = %s \n",(newinfo2.SkipClassDefinition() == true) ? "true" : "false");
               printf ("In unparseAggrInit(): newinfo2.SkipEnumDefinition()  = %s \n",(newinfo2.SkipEnumDefinition() == true) ? "true" : "false");
#endif
             }
#if 0
          newinfo2.display("In unparseAggrInit(): (aggr_init->get_uses_compound_literal() == true): newinfo");
#endif
          curprint ("(");

       // DQ (3/21/2018): Added assertion.
          ASSERT_not_null(aggr_init->get_type());

#if 0
          printf ("In unparseAggrInit(): aggr_init->get_type() = %p = %s \n",aggr_init->get_type(),aggr_init->get_type()->class_name().c_str());
          curprint ("/* output type in unparseAggrInit() */ ");
#endif
       // DQ (9/4/2013): We need to unparse the full type (both the first and second parts of the type).
       // unp->u_type->unparseType(aggr_init->get_type(),newinfo);
          unp->u_type->outputType<SgAggregateInitializer>(aggr_init,aggr_init->get_type(),newinfo2);

          curprint (")");
#if 0
          curprint ("/* DONE: output type in unparseAggrInit() */ ");
#endif
        }

  // DQ (9/29/2012): We don't want to use the explicit "{}" inside of function argument lists (see C test code: test2012_10.c).
     bool need_explicit_braces = aggr_init->get_need_explicit_braces();

#if DEBUG_AGGREGATE_INITIALIZER
     printf ("In unparseAggrInit(): after output of type: need_explicit_braces           = %s \n",need_explicit_braces ? "true" : "false");
     printf ("In unparseAggrInit(): after output of type: newinfo2.SkipEnumDefinition()  = %s \n",newinfo2.SkipEnumDefinition() ? "true" : "false");
     printf ("In unparseAggrInit(): after output of type: newinfo2.SkipClassDefinition() = %s \n",newinfo2.SkipClassDefinition() ? "true" : "false");

  // DQ (5/18/2019): Adding test for inAggregateInitializer.
     printf ("In unparseAggrInit(): after output of type: newinfo2.inAggregateInitializer() = %s \n",newinfo2.inAggregateInitializer() ? "true" : "false");
#endif

#if 0
  // DQ (7/27/2013): Commented this out since we do need it now that we support the compound literals.
     if (aggr_init->get_need_explicit_braces())
        {
          SgExprListExp* expressionList = isSgExprListExp(aggr_init->get_parent());
          if (expressionList != NULL)
             {
               SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(expressionList->get_parent());
               if (functionCallExp != NULL)
                  {
                    need_explicit_braces = false;

                    printf ("reset in SgFunctionCallExp: need_explicit_braces = %s \n",need_explicit_braces ? "true" : "false");
                  }
             }
        }
#endif

     SgExpressionPtrList& list = aggr_init->get_initializers()->get_expressions();

  // DQ (6/27/2018): This is to output the class name for test2018_113.C.
  // if (need_cxx11_class_specifier == true)
  // if (list.size() == 0)
  // Check if this is a part of the function argument list to a template (function or class), 
  // since we need this to work independent of the list size (see test2018_116.C).


  // if (need_cxx11_class_specifier == true && list.size() == 0 && need_explicit_braces == true)
     if (need_cxx11_class_specifier == true && need_explicit_braces == true)
        {
       // Identified list size == 0

          ASSERT_not_null(aggr_init);
          SgClassType* classType = isSgClassType(aggr_init->get_type());
#if 0
          printf ("classType = %p \n",classType);
#endif
          if (classType != NULL)
             {
#if 0
               printf ("Output the class name classType = %p = %s \n",classType,classType->class_name().c_str());
#endif
               newinfo2.set_SkipClassSpecifier();
               unp->u_type->outputType<SgAggregateInitializer>(aggr_init,classType,newinfo2);
             }
        }

  // if (aggr_init->get_need_explicit_braces())
     if (need_explicit_braces == true)
        {
       // DQ (3/12/2018): Could this be what should drive the introduction of the class name?
          if (info.inAggregateInitializer() == true)
             {
               curprint("/* Need explicit braces: is this where we insert the class name? */ ");
             }

          curprint("{");
        }

#if 0
  // DQ (3/12/2018): Set this so that we can only add class names inside of the first AggregateInitializer.
#if 0
     printf ("In unparseAggrInit(): Calling newinfo2.set_inAggregateInitializer() \n");
#endif
     newinfo2.set_inAggregateInitializer();
#endif


#if 0
     SgExpressionPtrList::iterator p = list.begin();
     if (p != list.end())
        {
          while (1)
             {
               bool skipUnparsing = isFromAnotherFile(aggr_init,index);
               if (!skipUnparsing)
                 unparseExpression((*p), newinfo);
               p++;
               index ++;
               if (p != list.end())
                  { 
                    if (!skipUnparsing)
                       curprint ( ", ");  
                  }
               else
                  break;
             }
        }
#endif

#if 0
     printf ("In unparseAggrInit(): printOutComments(aggr_init = %p = %s) \n",aggr_init,aggr_init->class_name().c_str());
     printOutComments(aggr_init);

     for (size_t index = 0; index < list.size(); index ++)
        {
          printf ("In unparseAggrInit(): loop: printOutComments(list[index=%" PRIuPTR "] = %p = %s) \n",index,list[index],list[index]->class_name().c_str());
          printOutComments(list[index]);
        }
#endif

  // DQ (9/11/2013): Detect the use of an #include directive and remove the #include directive.
     bool skipTestForSourcePosition = containsIncludeDirective(aggr_init);
     if (skipTestForSourcePosition == true)
        {
#if DEBUG_AGGREGATE_INITIALIZER
          printf ("In unparseAggrInit(): Found an include directive to be removed \n");
#endif
          removeIncludeDirective(aggr_init);
        }

#if DEBUG_AGGREGATE_INITIALIZER
     printf ("In unparseAggrInit(): list.size() = %zu \n",list.size());
     curprint ("/* output list elements in unparseAggrInit() */ ");
#endif

  // SgArrayType* arrayType = isSgArrayType(aggr_init->get_type());
#if 0
     printf ("Looking for an array initializer: arrayType = %p \n",arrayType);
#endif

     size_t last_index = list.size() -1;

     for (size_t index = 0; index < list.size(); index++)
        {
#if DEBUG_AGGREGATE_INITIALIZER || 0
          printf ("In unparseAggrInit(): list index = %zu \n",index);
          curprint ("/* In unparseAggrInit(): output list element */ ");
#endif

#if 0
       // printf ("Top of loop: depth = %d need_cxx11_class_specifier = %s \n",depth,need_cxx11_class_specifier ? "true" : "false");
          printf ("Top of loop: depth = %d \n",depth);
#endif

       // **********************************************************************************************
       // DQ (3/12/2018): Best thoughts as I am leaving to head home! Pick this up again in the morning.
       // Sometimes we need to output the class name if the list elements are class types.
       // However, it might be that we don't do this if we are initializing an array of class types.
       // In contrast we should output the class type when we are initializing a structure containing 
       // multiple data members that are of class types.
       // **********************************************************************************************

          SgAggregateInitializer* aggregateInitializer = isSgAggregateInitializer(list[index]);
       // ASSERT_not_null(aggregateInitializer);

#if 0
          printf ("In loop: depth = %d need_cxx11_class_specifier = %s \n",depth,need_cxx11_class_specifier ? "true" : "false");
#endif
       // DQ (6/27/2018): Make this more restrictive since need_cxx11_class_specifier is set more often to be true.
       // if (need_cxx11_class_specifier == true)
          if (need_cxx11_class_specifier == true && aggregateInitializer != NULL)
             {
               ASSERT_not_null(aggregateInitializer);
#if 0
               printf ("aggregateInitializer->get_type() = %p = %s \n",aggregateInitializer->get_type(),aggregateInitializer->get_type()->class_name().c_str());
#endif
            // Might need to strip modifiers.
            // SgClassType* classType = isSgClassType(arrayType->get_base_type());
               SgClassType* classType = isSgClassType(aggregateInitializer->get_type());

            // DQ (6/27/2018): Make this more restrictive since need_cxx11_class_specifier is set more often to be true.
            // ASSERT_not_null(classType);
               if (classType != NULL)
                  {
#if 0
               printf ("In unparseAggrInit(): need_cxx11_class_specifier == true: aggregateInitializer = %p \n",aggregateInitializer);
               printf ("In unparseAggrInit(): need_cxx11_class_specifier == true: classType            = %p \n",classType);
#endif
            // DQ (3/12/2018): Trying something different.
            // unp->u_type->outputType<SgAggregateInitializer>(aggr_init,aggr_init->get_type(),newinfo2);
               if (aggregateInitializer != NULL && classType != NULL)
                  {
                    newinfo2.set_SkipClassSpecifier();

                    unp->u_type->outputType<SgAggregateInitializer>(aggregateInitializer,classType,newinfo2);
                  }
                  }
             }

#if DEBUG_AGGREGATE_INITIALIZER
          printf ("In unparseAggrInit(): between class name and expression: index = %zu \n",index);
          curprint ("/* output class name between array elements */ ");
#endif

#if 0
       // DQ (2/20/2019): Check if this is a compiler generated SgConstructorInitializer (see Cxx11_tests/test2019_171.C).
          if (list[index]->isCompilerGenerated() == true)
             {
               break;
             }
#endif

#if 1
       // DQ (5/17/2019): Moved from the top of the function to be closer to where it is used.
          SgUnparse_Info newinfo(info);

       // DQ (5/17/2019): Set this so that we can know to skip the output of type elaboration.
          newinfo.set_inAggregateInitializer();
#endif

#if DEBUG_AGGREGATE_INITIALIZER
       // printf ("In unparseAggrInit(): before output of SgConstructorInitializer: need_explicit_braces          = %s \n",need_explicit_braces ? "true" : "false");
          printf ("In unparseAggrInit(): before output of SgConstructorInitializer: newinfo.SkipEnumDefinition()  = %s \n",newinfo.SkipEnumDefinition() ? "true" : "false");
          printf ("In unparseAggrInit(): before output of SgConstructorInitializer: newinfo.SkipClassDefinition() = %s \n",newinfo.SkipClassDefinition() ? "true" : "false");
#endif

       // If there was an include then unparse everything (because we removed the include (above)).
       // If there was not an include then still unparse everything!
          unparseExpression(list[index], newinfo);

          if (index != last_index)
             {
            // DQ (2/20/2019): Check if this is a compiler generated SgConstructorInitializer (see Cxx11_tests/test2019_171.C).

            // DQ (2/20/2019): Check if this is a compiler generated SgConstructorInitializer
            // (see Cxx11_tests/test2019_171.C).  I think this may be the wrong place for this.
               SgConstructorInitializer* constructorInitializer = isSgConstructorInitializer(list[index+1]);
               if (constructorInitializer != NULL && constructorInitializer->isCompilerGenerated() == true)
                  {
#if 0
                    printf ("Detected compiler generated expression so break out of loop index = %zu \n",index);
#endif
                    break;
                  }
               curprint (", ");
             }

#if 0
          printf ("Bottom of loop: depth = %d need_cxx11_class_specifier = %s \n",depth,need_cxx11_class_specifier ? "true" : "false");
#endif
        }
     unparseAttachedPreprocessingInfo(aggr_init, info, PreprocessingInfo::inside);

  // if (aggr_init->get_need_explicit_braces())
     if (need_explicit_braces == true)
        {
          curprint("}");
        }

#if DEBUG_AGGREGATE_INITIALIZER 
     printf ("Leaving unparseAggrInit() \n");
     curprint ("/* Leaving unparseAggrInit() */ ");
#endif

#if 0
     printf ("Leaving unparseAggrInit(): depth = %d \n",depth);
#endif

#if 0
     if (need_cxx11_class_specifier == true)
        {
          printf ("Found case of need_cxx11_class_specifier == true \n");
          ROSE_ASSERT(false);
        }
#endif

     depth--;
   }


void
Unparse_ExprStmt::unparseCompInit(SgExpression* expr, SgUnparse_Info& info)
   {
     SgCompoundInitializer* comp_init = isSgCompoundInitializer(expr);
     ASSERT_not_null(comp_init);

#if 0
  // Skip the entire thing if the initializer is from an included file
     if (isFromAnotherFile (expr))
        {
#if 0
          printf ("In unparseCompInit(): This SgCompoundInitializer (comp_init = %p) is from another file so its subtree will not be output in the generated code \n",comp_init);
#endif
          return;
        }
#endif

     SgUnparse_Info newinfo(info);

     curprint("(");

     SgExpressionPtrList& list = comp_init->get_initializers()->get_expressions();
     size_t last_index = list.size() -1;
     for (size_t index =0; index < list.size(); index ++)
        {
       // bool skipUnparsing = isFromAnotherFile(aggr_init,index);
          bool skipUnparsing = isFromAnotherFile(list[index]);
          if (!skipUnparsing)
             {
               unparseExpression(list[index], newinfo);
               if (index!= last_index)
                    curprint ( ", ");
             }
            else
             {
#if 0
               printf ("In unparseCompInit(): (comp_init = %p) list[index = %" PRIuPTR "] = %p = %s is from another file so its subtree will not be output in the generated code \n",comp_init,index,list[index],list[index]->class_name().c_str());
#endif
             }
        }

     unparseAttachedPreprocessingInfo(comp_init, info, PreprocessingInfo::inside);
     curprint(")");
   }


SgName
Unparse_ExprStmt::trimOutputOfFunctionNameForGNU_4_5_VersionAndLater(SgName nameQualifier, bool & skipOutputOfFunctionName)
   {
  // This approach might not work since more complex qualified names will include template with 
  // template parameters that have qualified names.  So we will have to support building the 
  // name qualification string differently for this special case of a SgConstructorInitializer.
  // Alternatively we can truncate the output of "::<class name>".

  // This function is used in both the unparseOneElemConInit() and unparseConInit() functions.

  // Note that the g++ compiler might not be named "g++", it is not clear how to handle this case.
  // string backEndCompiler = BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH;

     bool usingGxx = false;
     bool usingClang = false;
     #ifdef USE_CMAKE
       #ifdef CMAKE_COMPILER_IS_GNUCXX
         usingGxx = true;
       #endif
       #ifdef CMAKE_COMPILER_IS_CLANG
         usingClang = true;
       #endif
     #else
    // DQ (2/1/2016): Make the behavior of ROSE independent of the exact name of the backend compiler (problem when packages name compilers such as "g++-4.8").
    // usingGxx = (backEndCompiler == "g++");
       #if BACKEND_CXX_IS_GNU_COMPILER
          usingGxx = true;
       #endif
       #if BACKEND_CXX_IS_CLANG_COMPILER
          usingClang = true;
       #endif
     #endif
/*Pei-Hung (03/25/2020) Adding Clang support and tested with Clang 8.x.  We might lower the clang version checking */
     //if (usingGxx)
        {
       // Now check the version of the identified GNU g++ compiler.
          if ((usingGxx && ((BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER == 4 && BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER >= 5) || (BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER > 4)))
          // DQ (3/31/2020): bug fix for use of ROSE on older clang version 6 (on my Mac)
          // || (usingClang && (BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER > 7)))
             || (usingClang && (BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER >= 6)))
             {
            // If this is the GNU g++ 4.5 version compiler (or greater) then we have to use "X::A()"
            // as a constructor name instead of "X::A::A()" which was previously accepted by GNU g++.
               string temp_nameQualifier = nameQualifier.str();
#if 0
               printf ("In trimOutputOfFunctionNameForGNU_4_5_VersionAndLater(): g++ version 4.5 or greater: temp_nameQualifier = %s \n",temp_nameQualifier.c_str());
#endif
            // trim the trailing "A::" from the end of the nameQualifier.
               size_t temp_nameQualifier_last  = temp_nameQualifier.find_last_not_of("::");
#if 0
               printf ("In trimOutputOfFunctionNameForGNU_4_5_VersionAndLater(): temp_nameQualifier_last = %" PRIuPTR " \n",temp_nameQualifier_last);
#endif
               if (temp_nameQualifier_last != string::npos)
                  {
                    string temp_nameQualifier_substring = temp_nameQualifier.substr(0,temp_nameQualifier_last+1);
#if 0
                    printf ("In trimOutputOfFunctionNameForGNU_4_5_VersionAndLater(): temp_nameQualifier_substring = %s \n",temp_nameQualifier_substring.c_str());
#endif
                    nameQualifier = temp_nameQualifier_substring.c_str();
                    skipOutputOfFunctionName = true;
                  }
             }
        }

     return nameQualifier;
   }



// DQ (1/30/2019): Add this to the Unparse_ExprStmt namespace so that we can call it from unparseCxxStatement().
bool
Unparse_ExprStmt::isAssociatedWithCxx11_initializationList( SgConstructorInitializer* con_init, SgUnparse_Info& info )
   {
  // This is a suppoting function to detect the use of the C++11 signature which will 
  // trigger the unparsing of C++11 initializer list syntax.

     bool is_cxx11_initialization_list = false;

#define DEBUG_CXX11_INITIALIZATION_LIST 0

     if (con_init != NULL)
        {
          SgMemberFunctionDeclaration* memberFunctionDeclaration = con_init->get_declaration();
          if (memberFunctionDeclaration != NULL)
             {
               string name = memberFunctionDeclaration->get_name();
#if DEBUG_CXX11_INITIALIZATION_LIST
               printf ("In isAssociatedWithCxx11_initializationList(): memberFunctionDeclaration name = %s \n",name.c_str());
#endif
            // I don't feel comfortable with detecting the name of a specific class and having behavior depend upon this, 
            // but this is actually the way in works in C++ (at least in EDG specifically).
               if (name == "initializer_list")
                  {
                 // Found special type used in C++ to indicate special syntax for C++11 initiazation list support.
#if DEBUG_CXX11_INITIALIZATION_LIST
                    printf ("In isAssociatedWithCxx11_initializationList(): Found special type used in C++ to indicate special syntax for C++11 initiazation list support \n");
#endif
#if 0
                 // TV (07/18/18): happens in C++ 14 . With Kripke, EDG auto-detect C++14 (forcing C++11 causes C++14 related errors)
                 // Check if this is a C++11 file (just to make sure).
                    SgSourceFile* sourceFile = info.get_current_source_file();
                    ASSERT_not_null(sourceFile);
                    bool isCxx11 = sourceFile->get_Cxx11_only();
                    ROSE_ASSERT(isCxx11 == true);
#endif
                    is_cxx11_initialization_list = true;
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }
             }
            else
             {
            // DQ (1/14/2019): Added debugging code.
            // ASSERT_not_null(con_init->get_declaration());
               if (con_init->get_declaration() != NULL)
                 {
#if DEBUG_CXX11_INITIALIZATION_LIST
                    printf ("In isAssociatedWithCxx11_initializationList(): con_init->get_declaration() = %s \n",con_init->get_declaration()->class_name().c_str());
#endif
                 }
               else
                 {
#if DEBUG_CXX11_INITIALIZATION_LIST
                   printf ("In isAssociatedWithCxx11_initializationList(): con_init->get_declaration() = NULL \n");
#endif
                 }
             }
        }

#if DEBUG_CXX11_INITIALIZATION_LIST
     printf ("Leaving isAssociatedWithCxx11_initializationList(): is_cxx11_initialization_list = %s \n",is_cxx11_initialization_list ? "true" : "false");
#endif

     return is_cxx11_initialization_list;
   }


void
Unparse_ExprStmt::unparseConInit(SgExpression* expr, SgUnparse_Info& info)
   {
#define DEBUG_CONSTRUCTOR_INITIALIZER 0

#if DEBUG_CONSTRUCTOR_INITIALIZER
     printf ("\n\n**************************************** \n");
     printf ("In Unparse_ExprStmt::unparseConInit(): expr = %p \n",expr);
     printf ("**************************************** \n");
     printf ("WARNING: This is redundent with the Unparse_ExprStmt::unp->u_sage->unparseOneElemConInit (This function does not handle qualified names!) \n");
     curprint ("\n /* In unparseConInit(): TOP */ \n");
#endif

     SgConstructorInitializer* con_init = isSgConstructorInitializer(expr);
     ASSERT_not_null(con_init);

#if 0
  // DQ (2/20/2019): Check if this is a compiler generated SgConstructorInitializer (see Cxx11_tests/test2019_171.C).
     if (con_init->isCompilerGenerated() == true)
        {
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
          return;
        }
#endif

     SgUnparse_Info newinfo(info);
     bool outputParenthisis = false;

  // DQ (7/9/2019): Supress the class specifier.
  // newinfo.set_SkipClassSpecifier();

#if 0
     printf ("In unparseConInit(): set SkipClassSpecifier() \n");
#endif
#if DEBUG_CONSTRUCTOR_INITIALIZER
     printf ("In unparseConInit(): con_init->get_type()                        = %p = %s \n",con_init->get_type(),con_init->get_type()->class_name().c_str());
     printf ("In unparseConInit(): con_init->get_need_name()                   = %s \n",(con_init->get_need_name() == true) ? "true" : "false");
     printf ("In unparseConInit(): con_init->get_is_explicit_cast()            = %s \n",(con_init->get_is_explicit_cast() == true) ? "true" : "false");
     printf ("In unparseConInit(): con_init->get_is_braced_initialized()       = %s \n",(con_init->get_is_braced_initialized() == true) ? "true" : "false");
     printf ("In unparseConInit(): con_init->get_associated_class_unknown()    = %s \n",(con_init->get_associated_class_unknown() == true) ? "true" : "false");
     printf ("In unparseConInit(): con_init->get_need_parenthesis_after_name() = %s \n",(con_init->get_need_parenthesis_after_name() == true) ? "true" : "false");
  // curprint ( string("\n /* con_init->get_need_name()        = ") + (con_init->get_need_name() ? "true" : "false") + " */ \n");
  // curprint ( string("\n /* con_init->get_is_explicit_cast() = ") + (con_init->get_is_explicit_cast() ? "true" : "false") + " */ \n");
#endif

  // DQ (1/18/2019): Test the current SgConstructorInitializer.
     bool this_constructor_initializer_is_using_Cxx11_initializer_list = isAssociatedWithCxx11_initializationList(con_init,info);

#if DEBUG_CONSTRUCTOR_INITIALIZER
     printf ("In unparseConInit(): this_constructor_initializer_is_using_Cxx11_initializer_list = %s \n",(this_constructor_initializer_is_using_Cxx11_initializer_list == true) ? "true" : "false");
#endif

  // DQ (2/7/2016): Adding support for C++11 specific initialization lists (different from non-C++11 specific initialization lists).
  // C++11 rules for parsing give special attention to the use of "std::initialization_list<>" template.
  // As a result of this we have to detect this and treat it special.  For now we will do this by recognizing the name.
  // To make this worse, it appears that the code generation requires us to look ahead in the AST for this case of a constructor 
  // initializer refering to a class named "initialization_list".  This is even more ridiculous!
#if DEBUG_CONSTRUCTOR_INITIALIZER
     bool process_using_cxx11_initialization_list_syntax = false;
#endif
     bool current_constructor_initializer_is_for_initialization_list_member_function = false;

  // This is the signature of C++11 using the "std::initialization_list<>" template and in this case the syntax we generate has 
  // to be different (incredible, but true).  These guys should be ashamed of themselves.
     SgExprListExp* argumentList = con_init->get_args();

#if DEBUG_CONSTRUCTOR_INITIALIZER
     printf ("In unparseConInit(): argumentList = %zu \n",argumentList->get_expressions().size());
#endif

     if (argumentList->get_expressions().size() == 1)
        {
       // Look ahead to the use of the "std::initialization_list<>" template class.
          SgConstructorInitializer* nested_con_init = isSgConstructorInitializer(argumentList->get_expressions()[0]);

#if DEBUG_CONSTRUCTOR_INITIALIZER
          printf ("In unparseConInit(): nested_con_init = %p \n",nested_con_init);
#endif

          if (nested_con_init != NULL)
             {
            // Test the nested SgConstructorInitializer, if it was found.
#if DEBUG_CONSTRUCTOR_INITIALIZER
               printf ("In unparseConInit(): nested_con_init = %p = %s \n",nested_con_init,nested_con_init->class_name().c_str());
#endif
#if DEBUG_CONSTRUCTOR_INITIALIZER
               process_using_cxx11_initialization_list_syntax = isAssociatedWithCxx11_initializationList(nested_con_init,info);
#endif
             }
        }
       else
        {
       // We also need to know if the current SgConstructorInitializer is associated with the C++11 initialization list.
          if (argumentList->get_expressions().size() == 2)
             {
               current_constructor_initializer_is_for_initialization_list_member_function = isAssociatedWithCxx11_initializationList(con_init,info);
             }
        }

#if DEBUG_CONSTRUCTOR_INITIALIZER
     printf ("newinfo.get_cxx11_initialization_list() = %s \n",newinfo.get_cxx11_initialization_list() ? "true" : "false");
     curprint ("\n /* In unparseConInit(): after call to get_cxx11_initialization_list() */ \n");

     printf ("process_using_cxx11_initialization_list_syntax                             = %s \n",process_using_cxx11_initialization_list_syntax ? "true" : "false");
     printf ("current_constructor_initializer_is_for_initialization_list_member_function = %s \n",current_constructor_initializer_is_for_initialization_list_member_function ? "true" : "false");
#endif



     bool use_braces_instead_of_parenthisis = false;
#if 0
  // DQ (1/18/2019): Set use_braces_instead_of_parenthisis directly from the con_init->get_is_braced_initialized() data member.
     if (process_using_cxx11_initialization_list_syntax == true || current_constructor_initializer_is_for_initialization_list_member_function == true)
        {
          use_braces_instead_of_parenthisis = true;

          newinfo.set_cxx11_initialization_list();
        }
       else
        {
          if (newinfo.get_cxx11_initialization_list() == true)
             {
            // use "{ }" instead of "( )"
               use_braces_instead_of_parenthisis = true;
             }
        }
#endif

  // DQ (1/15/2019): Added support for braced initialization.
     if (con_init->get_is_braced_initialized() == true)
        {
          use_braces_instead_of_parenthisis = true;
        }

#if DEBUG_CONSTRUCTOR_INITIALIZER
     printf ("In unparseConInit(): use_braces_instead_of_parenthisis = %s \n",use_braces_instead_of_parenthisis ? "true" : "false");
#endif

     SgNode* nodeReferenceToType = newinfo.get_reference_node_for_qualification();

#if DEBUG_CONSTRUCTOR_INITIALIZER
     printf ("In unparseConInit(): nodeReferenceToType = %p \n",nodeReferenceToType);
     curprint ("\n /* In unparseConInit(): nodeReferenceToType */ \n");
#endif

     if (nodeReferenceToType != NULL)
        {
#if DEBUG_CONSTRUCTOR_INITIALIZER
          printf ("In unparseConInit(): nodeReferenceToType = %p = %s \n",nodeReferenceToType,nodeReferenceToType->class_name().c_str());
          curprint ("\n /* In unparseConInit(): nodeReferenceToType = " + nodeReferenceToType->class_name() + " */ \n");
#endif
        }
       else
        {
#if DEBUG_CONSTRUCTOR_INITIALIZER
          curprint ("\n /* In unparseConInit(): nodeReferenceToType = NULL */ \n");
#endif
       // DQ (6/4/2011): If it is not set then set it to the SgConstructorInitializer expression.
       // We can't enforce that it be non-null since that would have far reaching effects on the 
       // unparser implementation.
       // newinfo.set_reference_node_for_qualification(con_init);
        }

  // This should have been set from wherever it is called.
  // ASSERT_not_null(newinfo.get_reference_node_for_qualification());

  // DQ (5/26/2013): Alternative form of SgConstructorInitializer unparsing.

#if DEBUG_CONSTRUCTOR_INITIALIZER || 0
     printf ("con_init->get_need_name()                      = %s \n",con_init->get_need_name() ? "true" : "false");
     printf ("con_init->get_is_explicit_cast()               = %s \n",con_init->get_is_explicit_cast() ? "true" : "false");
  // printf ("con_init->get_is_braced_initializer()          = %s \n",con_init->get_is_braced_initializer() ? "true" : "false");
     printf ("con_init->get_is_braced_initialized()          = %s \n",con_init->get_is_braced_initialized() ? "true" : "false");
     printf ("unp->u_sage->printConstructorName(con_init)    = %s \n",unp->u_sage->printConstructorName(con_init) ? "true" : "false");
     printf ("process_using_cxx11_initialization_list_syntax = %s \n",process_using_cxx11_initialization_list_syntax ? "true" : "false");
     printf ("con_init->get_is_braced_initialized()          = %s \n",con_init->get_is_braced_initialized() ? "true" : "false");
     printf ("con_init->get_declaration()                    = %p \n",con_init->get_declaration());
     if (con_init->get_declaration() != NULL)
        {
          printf ("con_init->get_declaration()                    = %p = %s \n",con_init->get_declaration(),con_init->get_declaration()->class_name().c_str());
          SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunction = isSgTemplateInstantiationMemberFunctionDecl(con_init->get_declaration());
          if (templateInstantiationMemberFunction != NULL)
             {
               printf ("templateInstantiationMemberFunction->get_name() = %s \n",templateInstantiationMemberFunction->get_name().str());
             }
        }
#endif

  // DQ (5/26/2013): Combined these predicates so that we could rewrite this to not generate a SgName and ouput it later.  
  // This is required to allow us to call the function that output the member function name (if used), so that we can 
  // under specific cases drop the output of the template argument list where it is redundant with the class.
  // if ( unp->u_sage->printConstructorName(con_init) && !nm.is_null() )
  // if ((con_init->get_need_name() == true) && (con_init->get_is_explicit_cast() == true) )
     if ((con_init->get_need_name() == true) && (con_init->get_is_explicit_cast() == true) && unp->u_sage->printConstructorName(con_init) == true)
        {
       // for foo(B())
          ROSE_ASSERT(con_init->get_associated_class_unknown() == true || con_init->get_declaration() != NULL || con_init->get_class_decl() != NULL);

       // DQ (1/17/2019): Test by turning this off.
       // DQ (2/7/2016): I think that if we need the name then this can't be true.
       // ROSE_ASSERT(process_using_cxx11_initialization_list_syntax == false);

       // DQ (4/27/2006): Maybe we can finally assert this!
       // ROSE_ASSERT ( con_init->get_associated_class_unknown() == true);

       // DQ (8/5/2005): Now this logic is greatly simplified! Unforntunately not!
       // DQ (6/1/2011): It can't be this simple since con_init->get_declaration() can be NULL where in a struct there is no constructor defined.
#if DEBUG_CONSTRUCTOR_INITIALIZER
          printf ("con_init->get_declaration() = %p \n",con_init->get_declaration());
#endif
#if DEBUG_CONSTRUCTOR_INITIALIZER
          curprint ( "\n /* con_init->get_declaration() = " + string(con_init->get_declaration() ? "valid" : "null") + " pointer */ \n");
#endif
          if (con_init->get_declaration() != NULL)
             {
            // DQ (6/1/2011): Newest refactored support for name qualification.
            // nm = con_init->get_declaration()->get_qualified_name();
               SgName nameQualifier = con_init->get_qualified_name_prefix();
#if DEBUG_CONSTRUCTOR_INITIALIZER
               printf ("In unparseConInit(): nameQualifier = %s \n",nameQualifier.str());
               printf ("In unparseConInit(): con_init->get_declaration()->get_name() = %s \n",con_init->get_declaration()->get_name().str());
#endif
            // DQ (5/26/2013): This is the newer version of the code.
#if DEBUG_CONSTRUCTOR_INITIALIZER
               printf("In unparseConInit(): nameQualifier = %s (must be modified for GNU g++ 4.5 compilers) \n",nameQualifier.str());
#endif
            // DQ (2/8/2014): Added support to generate names that are suitable for the later versions of the GNU g++ compiler.
               bool skipOutputOfFunctionName = false;
               nameQualifier = trimOutputOfFunctionNameForGNU_4_5_VersionAndLater(nameQualifier,skipOutputOfFunctionName);

            // DQ (5/26/2013): Output the name qualification for the class.
               curprint(nameQualifier.str());

            // DQ (5/25/2013): Added support to unparse the template arguments seperately from the member function name (which should NOT 
            // include the template arguments when unparsing). Note the the template arguments in the name are important for the generation
            // of mangled names for use in symbol tabls, but that we need to output the member function name and it's template arguments 
            // seperately so that they name qulification can be computed and saved in the name qualification name maps.

            // Note that this code below is a copy of that from the support for unpasing the SgTemplateInstantiationFunctionDecl (in function above).

               SgDeclarationStatement* declaration = con_init->get_declaration();

               SgName func_name = con_init->get_declaration()->get_name();

            // DQ (6/21/2011): Support for new name qualification (output of generated function name).
               ASSERT_not_null(declaration);
            // printf ("Inside of Unparse_ExprStmt::unparseFuncRef(): declaration = %p = %s \n",declaration,declaration->class_name().c_str());
#if DEBUG_CONSTRUCTOR_INITIALIZER
            // DQ (4/15/2013): If there is other debug output turned on then nesting of comments inside of comments can occur in this output (see test2007_17.C).
               printf ("In unparseConInit(): put out func_name = %s \n",func_name.str());
#endif
#if DEBUG_CONSTRUCTOR_INITIALIZER
               curprint (string("\n /* In unparseConInit(): put out func_name = ") + func_name + " */ \n ");
#endif

               if (skipOutputOfFunctionName == false)
                  {
                 // If this is a template then the name will include template arguments which require name qualification and the name 
                 // qualification will depend on where the name is referenced in the code.  So we have generate the non-canonical name 
                 // with all possible qualifications and save it to be reused by the unparser when it unparses the tempated function name.
                    SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDecl = isSgTemplateInstantiationMemberFunctionDecl(declaration);
                    if (templateInstantiationMemberFunctionDecl != NULL)
                       {
#if 0
                         printf ("In unparseConInit(): declaration->get_declarationModifier().isFriend() = %s \n",declaration->get_declarationModifier().isFriend() ? "true" : "false");
                      // printf ("In unparseConInit(): diff = %d \n",diff);
#endif
                      // if ( (declaration->get_declarationModifier().isFriend() == false) && (diff == 0) )
                         if (declaration->get_declarationModifier().isFriend() == false)
                            {
#if 0
                              printf ("Regenerate the name func_name = %s \n",func_name.str());
                              printf ("templateInstantiationMemberFunctionDecl->get_templateName() = %s \n",templateInstantiationMemberFunctionDecl->get_templateName().str());
#endif
                           // DQ (2/8/2016): Should this be using newinfo instead of info?
                              unparseTemplateMemberFunctionName(templateInstantiationMemberFunctionDecl,info);
                            }
                           else
                            {
                           // This case supports test2004_77.C

                              printf ("WARNING: In unparseConInit(): No name qualification permitted in this case! (not clear if this case if important for unparseMFuncRef(), as it was for unparseFuncRef()) \n");

                              curprint(func_name);
                            }
                       }
                      else
                       {
                         curprint(func_name);
                       }
                  }
             }
            else
             {
            // In this case there is no constructor member function to name, and so there is only the name of the class to use as a default constructor call.
#if DEBUG_CONSTRUCTOR_INITIALIZER
               printf ("con_init->get_class_decl() = %s \n",con_init->get_class_decl() ? "true" : "false");
               curprint ("\n /* In unparseConInit(): con_init->get_class_decl() */ \n");
#endif
               if (con_init->get_class_decl() != NULL)
                  {
                 // DQ (6/1/2011): Newest refactored support for name qualification.
                 // nm = con_init->get_class_decl()->get_qualified_name();

                    SgName nameQualifier = con_init->get_qualified_name_prefix();
#if DEBUG_CONSTRUCTOR_INITIALIZER
                    printf ("In Unparse_ExprStmt::unparseConInit(): con_init->get_declaration() == NULL -- nameQualifier = %s \n",nameQualifier.str());
                    curprint ("\n /* In unparseConInit(): con_init->get_class_decl() != NULL */ \n");
#endif
                 // DQ (2/8/2014): I think this process of trimming the generated name is not required where there is not 
                 // associated member function and we are using the class name directly.
                    bool skipOutputOfFunctionName = false;
                 // nameQualifier = trimOutputOfFunctionNameForGNU_4_5_VersionAndLater(nameQualifier,skipOutputOfFunctionName);

                 // nm = nameQualifier + con_init->get_class_decl()->get_name();
                    curprint(nameQualifier.str());

                 // DQ (8/19/2013): This handles the case represented by test2013_306.C.
                    newinfo.set_reference_node_for_qualification(con_init);

                 // DQ (8/19/2013): This is required to handle references to classes in the throw expression (at least), see test2004_150.C.
                 // DQ (8/19/2013): This has no effect if the type string is taken from the type name map and so the "class" needs
                 // to be eliminated in the generation of the intial string as part of the generation of the name qualification.
                 // printf ("In unparseOneElemConInit(): calling set_SkipClassSpecifier() \n");
                    newinfo.set_SkipClassSpecifier();
#if DEBUG_CONSTRUCTOR_INITIALIZER
                    printf ("In unparseConInit(): Unparse the type = %p = %s \n",con_init->get_type(),con_init->get_type()->class_name().c_str());
#endif
                 // unp->u_type->unparseType(con_init->get_type(),newinfo);
                    if (skipOutputOfFunctionName == false)
                       {
                         unp->u_type->unparseType(con_init->get_type(),newinfo);
                       }
#if DEBUG_CONSTRUCTOR_INITIALIZER
                    printf ("DONE: In unparseConInit(): unparseType() \n");
#endif
                  }
              // DQ (8/4/2012): We need this case to handle tests such as test2012_162.C.
              // DQ (3/29/2012): For EDG 4.x it appear we need a bit more since both con_init->get_declaration() and con_init->get_class_decl() can be NULL (see test2012_52.C).
                 else
                  {
#if DEBUG_CONSTRUCTOR_INITIALIZER
                    printf ("In unparseConInit(): Need to handle new case for where both con_init->get_declaration() and con_init->get_class_decl() can be NULL \n");
                    printf ("In unparseConInit(): Get name of type = %p = %s name = %s \n",con_init->get_type(),con_init->get_type()->class_name().c_str(),"NOT EVALUATED YET");
#endif
                    unp->u_type->unparseType(con_init->get_type(),newinfo);

                 // ROSE_ASSERT ( nm.is_null() == false );
                  }
             }

#if DEBUG_CONSTRUCTOR_INITIALIZER
       // printf ("In Unparse_ExprStmt::unparseConInit(): nm = %s \n",nm.str());
          curprint ("\n /* In unparseConInit(): nm != NULL */ \n");
#endif

       // DQ (8/4/2012): Commented out this test since we output the type name using unparseType() for the case of a primative type.
       // ROSE_ASSERT ( nm.is_null() == false );

       // printf ("In Unparse_ExprStmt::unparseConInit: info.PrintName() = %s nm = %s \n",info.PrintName() ? "true" : "false",nm.str());
       // curprint ( "\n /* Debugging In Unparse_ExprStmt::unparseConInit: nm = " + nm.str() + " */ \n";

       // purify error: nm.str() could be a NULL string
       // if (unp->u_sage->printConstructorName(con_init) && info.PrintName())
       // if ( unp->u_sage->printConstructorName(con_init) )
       // if ( unp->u_sage->printConstructorName(con_init) && !nm.is_null() )
          if (unp->u_sage->printConstructorName(con_init) == true)
             {
            // printf ("unp->u_sage->printConstructorName(con_init) == true \n");
#if DEBUG_CONSTRUCTOR_INITIALIZER
               curprint ("\n /* In unparseConInit(): set outputParenthisis = true */ \n");
#endif

            // DQ (5/26/2013): In this rewritten part of the function we no longer output the name as a SgName (it was unparsed directly as needed).
            // curprint(nm.str());
               outputParenthisis = true;
             }
        }

     if (con_init->get_is_used_in_conditional() == true)
        {
#if DEBUG_CONSTRUCTOR_INITIALIZER
          printf ("In Unparse_ExprStmt::unparseConInit: output for use in conditional \n");
#endif
          curprint(" = ");
#if DEBUG_CONSTRUCTOR_INITIALIZER
          curprint(" /* output for use in conditional */ ");
#endif
#if 0
          printf ("Case of con_init->get_is_used_in_conditional() == true: exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

#if 0
  // DQ (1/18/2019): This does not appear to be needed (fails for Cxx11_tests/test2019_15.C).
  // DQ (2/7/2016): Adding support for C++11 specific initialization list syntax.
     if (process_using_cxx11_initialization_list_syntax == true)
        {
#if DEBUG_CONSTRUCTOR_INITIALIZER
          printf ("In Unparse_ExprStmt::unparseConInit: output C++11 initialization list syntax \n");
#endif

          curprint(" = ");

#if DEBUG_CONSTRUCTOR_INITIALIZER
          curprint(" /* output C++11 initialization list syntax */ ");
#endif
#if 0
          printf ("Case of process_using_cxx11_initialization_list_syntax == true: exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }
#endif

  // printf ("Now unparse the constructor arguments \n");
  // newinfo.display("Unparse_ExprStmt::unparseConInit");

     if ( con_init->get_args() == NULL )
        {
          printf ("Error: con_init->get_args() == NULL \n");
          con_init->get_file_info()->display("Error: con_init->get_args() == NULL");
        }
     ASSERT_not_null(con_init->get_args());

#if 0
#if 0
  // DQ (1/13/2019): Bug fix for C++11_tests/test2014_85.C
  // if ((con_init->get_need_name() == true) && (con_init->get_is_explicit_cast() == true) && unp->u_sage->printConstructorName(con_init) == true)
  // Note that this might not be the correct condition upon which to use the "{ }" syntax for the C++11 initialization list support.
  // if ( con_init->get_need_parenthesis_after_name() == true )
  // if ( (con_init->get_need_name() == true) && (con_init->get_need_parenthesis_after_name() == true) )
     if ( ( (con_init->get_need_name() == true) || 
            (con_init->get_is_explicit_cast() == true) || 
            (unp->u_sage->printConstructorName(con_init) == true) ) && 
          ( (con_init->get_need_parenthesis_after_name() == true) ) && 
            (process_using_cxx11_initialization_list_syntax == true))
#else
  // DQ (1/16/2019): Since the explicit caset data member is now set properly in the EDG/ROSE translation, we can't depend on it to always be true.
  // if ((con_init->get_is_explicit_cast() == true) && unp->u_sage->printConstructorName(con_init) == true)
  // if ((con_init->get_need_name() == true) && (con_init->get_is_explicit_cast() == true) && unp->u_sage->printConstructorName(con_init) == true)
     if ((con_init->get_need_name() == true) && unp->u_sage->printConstructorName(con_init) == true)
#endif
        {
#if DEBUG_CONSTRUCTOR_INITIALIZER
          printf ("Output the parenthisis after the class name \n");
          curprint(" /* set outputParenthisis == true */ ");
#endif
          outputParenthisis = true;
        }
#endif

  // DQ (1/15/2019): If we have use_braces_instead_of_parenthisis then set outputParenthisis since in 
  // that logic is the code to alternatively output the braces.
     if (use_braces_instead_of_parenthisis == true)
        {
          outputParenthisis = true;
        }

#if DEBUG_CONSTRUCTOR_INITIALIZER
     printf ("In unparseConInit(): con_init->get_need_parenthesis_after_name() = %s \n",(con_init->get_need_parenthesis_after_name() == true) ? "true" : "false");
#endif

  // DQ (1/16/2019): This appears to be required to handle the preinitializaztion lists (see test2019_13.C).
     if (con_init->get_need_parenthesis_after_name() == true)
        {
          outputParenthisis = true;
        }

  // DQ (1/17/2019): If there are arguments, then output parenthesis to contain them.
     if (unp->u_sage->printConstructorName(con_init) == true)
        {
          SgExprListExp* expressionList = isSgExprListExp(con_init->get_args());
          ASSERT_not_null(expressionList);
          if (expressionList->get_expressions().empty() == false)
             {
#if DEBUG_CONSTRUCTOR_INITIALIZER
               printf ("In unparseConInit(): Found non-empty expression list in the SgConstructorInitializer \n");
#endif

            // Check if this is an aggregate initializer, or just a SgInitializer, if so then suppress the parenthesis.
            // outputParenthisis = true;
               SgInitializer* initializer = isSgInitializer(expressionList->get_expressions()[0]);
#if DEBUG_CONSTRUCTOR_INITIALIZER
               printf ("In unparseConInit(): Found non-empty expression list expression is: initializer = %p = %s \n",
                    initializer,(initializer != NULL) ? initializer->class_name().c_str() : "null");
#endif
               if (initializer == NULL)
                  {
                    outputParenthisis = true;
                  }
                 else
                  {
#if DEBUG_CONSTRUCTOR_INITIALIZER
                    printf ("Found a SgInitializer, so don't output parenthesis due to unp->u_sage->printConstructorName(con_init) == true \n");
                    printf ("expressionList->get_expressions().size() = %zu \n",expressionList->get_expressions().size());
#endif

                 // DQ (12/7/2019): If there are multiple arguments, then we need the parenthesis (see test2019_489.C).
                    if (expressionList->get_expressions().size() >= 2)
                       {
#if DEBUG_CONSTRUCTOR_INITIALIZER
                         printf ("Set outputParenthisis = true \n");
#endif
                         outputParenthisis = true;
                       }
                  }
             }
        }

  // DQ (1/18/2019): this indicates a silent mode and no output should be generated from where
  // the constructor initializer is associated with the C++11 initialization_list class.
     if (this_constructor_initializer_is_using_Cxx11_initializer_list == true)
        {
#if DEBUG_CONSTRUCTOR_INITIALIZER
          printf ("Suppress output for constructor initializer from the C++11 initializer_list class: this_constructor_initializer_is_using_Cxx11_initializer_list == true: reset outputParenthisis = false \n");
#endif
          outputParenthisis = false;
        }

#if DEBUG_CONSTRUCTOR_INITIALIZER
     printf ("Output the opening parenthisis: con_init = %p \n",con_init);
     curprint(string(" /* output the opening parenthisis: con_init = ") + StringUtility::numberToString(con_init) + " */ ");
#endif

  // DQ (4/1/2005): sometimes con_init->get_args() is NULL (as in test2005_42.C)
     if (outputParenthisis == true)
        {
#if DEBUG_CONSTRUCTOR_INITIALIZER
       // curprint(" /* output the opening parenthisis */ ");
          curprint(string(" /* output the opening parenthisis: process_using_cxx11_initialization_list_syntax = ") + (process_using_cxx11_initialization_list_syntax ? "true" : "false") + " */ ");
          curprint(string(" /* output the opening parenthisis: current_constructor_initializer_is_for_initialization_list_member_function = ") + (current_constructor_initializer_is_for_initialization_list_member_function ? "true" : "false") + " */ ");
#endif

       // DQ (2/10/2016): Prefer "()" instead of "{}" where possible.
       // if (current_constructor_initializer_is_for_initialization_list_member_function == true)
          if (use_braces_instead_of_parenthisis == true)
             {
               if (con_init->get_args()->get_expressions().empty() == true)
                  {
#if DEBUG_CONSTRUCTOR_INITIALIZER
                    printf ("RESETTING use_braces_instead_of_parenthisis to FALSE \n");
#endif
                    use_braces_instead_of_parenthisis = false;
                  }
             }

       // DQ (2/7/2016): When using the C++11 initializer list syntax, we have to use "{" instead of "(".
       // curprint("(");
          if (use_braces_instead_of_parenthisis == true)
             {
            // Supress the inner set of "{"
            // curprint("{");
               if (current_constructor_initializer_is_for_initialization_list_member_function == false)
            // if (con_init->get_is_used_in_conditional() == false)
                  {
#if DEBUG_CONSTRUCTOR_INITIALIZER
                    printf ("use_braces_instead_of_parenthisis == true: current_constructor_initializer_is_for_initialization_list_member_function == false: output { \n");
#endif
                    curprint("{");
                  }
             }
            else
             {
#if DEBUG_CONSTRUCTOR_INITIALIZER
               curprint ("\n /* In unparseConInit(): output open parenthisis */ \n");
#endif
#if DEBUG_CONSTRUCTOR_INITIALIZER
               printf ("use_braces_instead_of_parenthisis == false: output ( \n");
#endif
               curprint("(");
             }
        }
       else
        {
#if DEBUG_CONSTRUCTOR_INITIALIZER
          printf ("Skip the output of the opening parenthisis: con_init = %p \n",con_init);
          curprint(string(" /* Skip the output of the opening parenthisis: con_init = ") + StringUtility::numberToString(con_init) + " */ ");
#endif
        }

#if DEBUG_CONSTRUCTOR_INITIALIZER
     printf ("output constructor arguments \n");
#endif

     if (con_init->get_args() != NULL)
        {
       // DQ (11/13/2004): Remove the parenthesis if we don't output the constructor name
       // this would only work if there was a single argument to the constructor!

       // DQ (3/17/2005): Remove the parenthesis if we don't output the constructor name
       // DQ (3/17/2005): Put the parenthesis BACK!
       // We need this to avoid: doubleArray *arrayPtr1 = (new doubleArray 42); (where it should have been "(42)")
       // if (con_init->get_is_explicit_cast() == true)
       //      curprint ( "(";

#if DEBUG_CONSTRUCTOR_INITIALIZER
          curprint(" /* output each arg */ ");
#endif
#if DEBUG_CONSTRUCTOR_INITIALIZER
          printf ("output each arg \n");
#endif

#if DEBUG_CONSTRUCTOR_INITIALIZER || 0
          printf ("### current_constructor_initializer_is_for_initialization_list_member_function = %s \n",current_constructor_initializer_is_for_initialization_list_member_function ? "true" : "false");
#endif
#if 0
          curprint(string(" /* current_constructor_initializer_is_for_initialization_list_member_function = ") + (current_constructor_initializer_is_for_initialization_list_member_function ? "true" : "false") + " */ ");
#endif

       // DQ (8/24/2020): debugging Cxx_tests/test2020_44.C need to communicate when to suppress extra parenthesis use around SgFunctionType arguments.
       // newinfo.set_current_constructor_initializer_is_for_initialization_list_member_function(current_constructor_initializer_is_for_initialization_list_member_function);
       // printf ("In unparseConInit(): con_init->get_is_braced_initialized()       = %s \n",(con_init->get_is_braced_initialized() == true) ? "true" : "false");
       // bool newinfo_input = con_init->get_is_braced_initialized();
       // bool newinfo_input = con_init->get_is_braced_initialized() || con_init->get_is_explicit_cast();
       // bool newinfo_input = con_init->get_is_braced_initialized() || con_init->get_is_explicit_cast();
          bool newinfo_input = con_init->get_is_braced_initialized() || con_init->get_is_explicit_cast() || (use_braces_instead_of_parenthisis == false);
#if 0
          printf ("con_init->get_is_braced_initialized() = %s \n",con_init->get_is_braced_initialized() ? "true" : "false");
          printf ("use_braces_instead_of_parenthisis = %s \n",use_braces_instead_of_parenthisis ? "true" : "false");
          printf ("newinfo_input = %s \n",newinfo_input ? "true" : "false");
#endif
       // newinfo.set_current_constructor_initializer_is_for_initialization_list_member_function(newinfo_input);
          newinfo.set_context_for_added_parentheses(newinfo_input);

       // DQ (2/7/2016): The output of the arguments is also special when using the C++11 initializer list syntax.
       // unparseExpression(con_init->get_args(), newinfo);
          if (current_constructor_initializer_is_for_initialization_list_member_function == true)
             {
            // In this case we onlut output the first argument (the second is a value representing the number of arguments).
               ROSE_ASSERT(con_init->get_args()->get_expressions().size() == 2);

               ASSERT_not_null(con_init->get_args()->get_expressions()[0]);

               unparseExpression(con_init->get_args()->get_expressions()[0], newinfo);
             }
            else
             {
#if DEBUG_CONSTRUCTOR_INITIALIZER
               printf ("Calling unparseExpression(): con_init = %p number of args = %zu \n",con_init,con_init->get_args()->get_expressions().size());
#endif

            // DQ (8/24/2020): Adding an extra parenthesis (see Cxx_tests/test2020_44.C).
            // curprint("/* extra opening parenthesis */ (");

               unparseExpression(con_init->get_args(), newinfo);

            // DQ (8/24/2020): Adding an extra parenthesis (see Cxx_tests/test2020_44.C).
            // curprint(") /* extra closing parenthesis */");

#if DEBUG_CONSTRUCTOR_INITIALIZER
               printf ("DONE: Calling unparseExpression(): con_init = %p number of args = %zu \n",con_init,con_init->get_args()->get_expressions().size());
#endif
             }

       // DQ (3/17/2005): Remove the parenthesis if we don't output the constructor name
       // if (con_init->get_is_explicit_cast() == true)
       //      curprint ( ")";
        }

#if DEBUG_CONSTRUCTOR_INITIALIZER
     printf ("Output the closing parenthisis: con_init = %p \n",con_init);
     curprint(string(" /* output the closing parenthisis: con_init = ") + StringUtility::numberToString(con_init) + " */ ");
#endif

     if (outputParenthisis == true)
        {
#if 0
       // curprint(" /* output the closing parenthisis */ ");
          curprint(string(" /* output the closing parenthisis: process_using_cxx11_initialization_list_syntax = ") + (process_using_cxx11_initialization_list_syntax ? "true" : "false") + " */ ");
          curprint(string(" /* output the closing parenthisis: current_constructor_initializer_is_for_initialization_list_member_function = ") + (current_constructor_initializer_is_for_initialization_list_member_function ? "true" : "false") + " */ ");
#endif
       // DQ (2/7/2016): When using the C++11 initializer list syntax, we have to use "{" instead of "(".
       // curprint(")");
          if (use_braces_instead_of_parenthisis == true)
             {
            // Supress the inner set of "{"
            // curprint("}");
               if (current_constructor_initializer_is_for_initialization_list_member_function == false)
                  {
                    curprint("}");
                  }
             }
            else
             {
               curprint(")");
             }
        }
       else
        {
#if DEBUG_CONSTRUCTOR_INITIALIZER
          printf ("Skip the output of the closing parenthisis: con_init = %p \n",con_init);
          curprint(string(" /* Skip the output of the closing parenthisis: con_init = ") + StringUtility::numberToString(con_init) + " */ ");
#endif
        }

#if 0
       else
        {
       // DQ (5/29/2004) Skip this so that we can avoid unparsing "B b;" as "B b();" since
       // this is a problem for g++ if a reference is taken to "b" (see test2004_44.C).
       // Verify that P::P():B() {} will still unparse correctly, ... it does!
       // for P::P():B() {}
       // if (con_init->get_need_name() || con_init->get_need_paren())
          if (con_init->get_need_paren() == true)
             {
               printf ("Handle case of: P::P():B() {} (where B is a data member in the preinitialization list) \n");
               curprint ( "()");
             }
        }
#endif

#if DEBUG_CONSTRUCTOR_INITIALIZER
     printf ("**************************************** \n");
     printf ("Leaving Unparse_ExprStmt::unparseConInit \n");
     printf ("**************************************** \n");
#endif
#if DEBUG_CONSTRUCTOR_INITIALIZER
     curprint ("\n /* Leaving Unparse_ExprStmt::unparseConInit() */ \n");
#endif
   }


void
Unparse_ExprStmt::unparseAssnInit(SgExpression* expr, SgUnparse_Info& info)
   {
     SgAssignInitializer* assn_init = isSgAssignInitializer(expr);
     ASSERT_not_null(assn_init);

#if 0
     printf ("In unparseAssnInit(): assn_init->get_is_explicit_cast() = %s \n",(assn_init->get_is_explicit_cast() == true) ? "true" : "false");
     printf ("In unparseAssnInit(): assn_init->get_operand() = %p = %s \n",assn_init->get_operand(),assn_init->get_operand()->class_name().c_str());
#endif
#if 0
     curprint("/* In unparseAssnInit() */ "); 
#endif
      if (assn_init->get_is_explicit_cast() == true)
        {
       // TV (11/15/2018): fixing weird behavior introduced with EDG 5.0 on Cxx_tests/test2006_70.C
       //                  happens when unparsing the "parameterList_syntax" for a parameter that has a default value
          if (assn_init->get_operand()->get_originalExpressionTree() != NULL) {
            unparseExpression(assn_init->get_operand()->get_originalExpressionTree(), info);
          } else {
            unparseExpression(assn_init->get_operand(), info);
          }
        }
       else
        {
          SgCastExp* castExp = isSgCastExp(assn_init->get_operand());
          if ( castExp != NULL)
             {
            // curprint ( "\n/* Skip the CAST in unparsing SgAssignInitializer */ \n";
               unparseExpression(castExp->get_operand(), info);
             }
            else
             {
            // temp backup in case operand is not a cast!
               printf ("Warning: unparseAssnInit operand was marked as implicit cast but didn't contain a SgCastExp object \n");
            // ROSE_ASSERT(false);

               unparseExpression(assn_init->get_operand(), info);
             }
        }

#if 0
     curprint("/* Leaving unparseAssnInit() */ "); 
#endif
   }


void
Unparse_ExprStmt::unparseBracedInit(SgExpression* expr, SgUnparse_Info& info)
   {
     SgBracedInitializer* braced_init = isSgBracedInitializer(expr);
     ASSERT_not_null(braced_init);

     SgUnparse_Info newinfo(info);

#define DEBUG_BRACED_INITIALIZER 0

#if DEBUG_BRACED_INITIALIZER
     printf ("In unparseBracedInit(): braced_init = %p = %s \n",braced_init,braced_init->class_name().c_str());
     curprint ("/* In unparseBracedInit() */ ");
#endif

#if DEBUG_BRACED_INITIALIZER
     printf ("In unparseBracedInit(): after output of type: newinfo.SkipEnumDefinition()  = %s \n",newinfo.SkipEnumDefinition() ? "true" : "false");
     printf ("In unparseBracedInit(): after output of type: newinfo.SkipClassDefinition() = %s \n",newinfo.SkipClassDefinition() ? "true" : "false");
#endif

     curprint("{");

     SgExpressionPtrList& list = braced_init->get_initializers()->get_expressions();
     size_t last_index = list.size()-1;

#if DEBUG_BRACED_INITIALIZER
     printf ("In unparseBracedInit(): list.size() = %zu \n",list.size());
     curprint ("/* output list elements in unparseBracedInit() */ ");
#endif

     for (size_t index = 0; index < list.size(); index ++)
        {
          unparseExpression(list[index], newinfo);
          if (index != last_index)
               curprint ( ", ");
        }

     unparseAttachedPreprocessingInfo(braced_init, info, PreprocessingInfo::inside);

     curprint("}");

#if DEBUG_AGGREGATE_INITIALIZER
     printf ("Leaving unparseBracedInit() \n");
     curprint ("/* Leaving unparseBracedInit() */ ");
#endif
   }


void
Unparse_ExprStmt::unparseThrowOp(SgExpression* expr, SgUnparse_Info& info)
   {
     SgThrowOp* throw_op = isSgThrowOp(expr);
     ASSERT_not_null(throw_op);

  // printf ("In unparseThrowOp(%s) \n",expr->sage_class_name());
  // curprint ( "\n/* In unparseThrowOp(" + expr->sage_class_name() + ") */ \n";

  // DQ (9/19/2004): Added support for different types of throw expressions!
     switch ( throw_op->get_throwKind() )
        {
          case SgThrowOp::unknown_throw:
             {
               printf ("Error: case of SgThrowOp::unknown_throw in unparseThrowOp() \n");
               ROSE_ASSERT(false);
               break;
             }

          case SgThrowOp::throw_expression:
             {
            // printf ("Case of SgThrowOp::throw_expression in unparseThrowOp() \n");
               curprint ( "throw ");

            // DQ not sure if we want to have the extra parentheses, EDG accepts them, but g++ does not (so skip them)
            // Nice example of where parenthesis are not meaningless.
            // curprint ( "(";

               ASSERT_not_null(throw_op->get_operand());
               unparseExpression(throw_op->get_operand(), info);

            // DQ skip  closing ")" see comment above 
            // curprint ( ")";

            // DQ (12/3/2004): Case of constructor initializer will require "()" 
            // if the constructor is not explicitly implemented in the class (see test2004_150.C)
               SgConstructorInitializer* constructorInitializer = isSgConstructorInitializer(throw_op->get_operand());
            // printf ("Handle extra () for constructorInitializer = %p \n",constructorInitializer);
            // printf ("constructorInitializer->get_declaration()  = %p \n",constructorInitializer->get_declaration());

               if (constructorInitializer != NULL)
                  {
                    if (constructorInitializer->get_declaration() != NULL)
                       {
                      // printf ("(Valid Declaration) constructorInitializer->get_declaration()->get_name() = %s \n",
                      //      constructorInitializer->get_declaration()->get_name().str());
                      // printf ("(Valid Declaration) constructorInitializer->get_args() = %p \n",constructorInitializer->get_args());
                       }
                      else
                       {
                         //Liao, 5/23/2009
                         // This is not necessarily true.
                         // A derived class's constructor initializer list acutally 
                         // can call its base class's constructor, even there is no
                         // user-defined default constructor for the base class.
                         // In this case, the parenthesis of the superclass is still needed.
                         // e.g: baseclass::baselcass( ):superclass() {};
                         // See bug 351
                      // DQ (8/5/2005): Now we force the get_args() pointer to always be a valid pointer and explicitly store a
                      // bool value to control when we output the "()" after the class name.
                         //ROSE_ASSERT(constructorInitializer->get_need_parenthesis_after_name() == false);
                      // DQ (4/1/2005) This is now fixed by forcing the unparsing of the constructor initializer to output the "()"
                      // DQ (12/3/2004): If there is no member function then the constructor is not explicit within the class.
                      // In this case we need to output the "()" after the constructor explicitly since it will not be done as 
                      // part of the output of the constructor initializer.  This details is quite important (see ChangeLog) 
                      // since the out put of the empty "() within the output of a constructor has a way of implying that the 
                      // constructor is explicit.
                      // printf ("(No Declaration) constructorInitializer->get_args() = %p \n",constructorInitializer->get_args());
                      // ROSE_ASSERT(constructorInitializer->get_args() == NULL);
                      // if (constructorInitializer->get_args() == NULL)
                      // curprint ( "()";
                       }
                  }
               break;
             }

          case SgThrowOp::rethrow:
             {
            // printf ("Case of SgThrowOp::rethrow in unparseThrowOp() \n");
               curprint ( "throw");
               break;
             }

          default:
               printf ("Error: default reached in unparseThrowOp() \n");
               ROSE_ASSERT(false);
               break;
        }
   }


void
Unparse_ExprStmt::unparseVarArgStartOp(SgExpression* expr, SgUnparse_Info& info)
   {
  // printf ("Inside of Unparse_ExprStmt::unparseVarArgStartOp \n");

     SgVarArgStartOp* varArgStart = isSgVarArgStartOp(expr);
     ASSERT_not_null(varArgStart);
     SgExpression* lhsOperand = varArgStart->get_lhs_operand();
     SgExpression* rhsOperand = varArgStart->get_rhs_operand();

     ASSERT_not_null(lhsOperand);
     ASSERT_not_null(rhsOperand);

  // DQ (9/16/2013): This was a problem pointed out by Phil Miller, it only has to be correct to make the resulting code link properly.
  // curprint ( "va_start(");
     curprint ( "__builtin_va_start(");
     unparseExpression(lhsOperand,info);
     curprint ( ",");
     unparseExpression(rhsOperand,info);
     curprint ( ")");
   }

void
Unparse_ExprStmt::unparseVarArgStartOneOperandOp(SgExpression* expr, SgUnparse_Info& info)
   {
  // printf ("Inside of Unparse_ExprStmt::unparseVarArgStartOneOperandOp \n");

     SgVarArgStartOneOperandOp* varArgStart = isSgVarArgStartOneOperandOp(expr);
     ASSERT_not_null(varArgStart);
     SgExpression* operand = varArgStart->get_operand_expr();
     ASSERT_not_null(operand);

  // DQ (9/16/2013): This was a problem pointed out by Phil Miller, it only has to be correct to make the resulting code link properly.
  // curprint ( "va_start(");
     curprint ( "__builtin_va_start(");
     unparseExpression(operand,info);
     curprint ( ")");
   }

void
Unparse_ExprStmt::unparseVarArgOp(SgExpression* expr, SgUnparse_Info& info)
   {
  // printf ("Inside of Unparse_ExprStmt::unparseVarArgOp \n");

     SgVarArgOp* varArg = isSgVarArgOp(expr);
     ASSERT_not_null(varArg);
     SgExpression* operand = varArg->get_operand_expr();

  // DQ (1/14/2006): p_expression_type is no longer stored (type is computed instead)
  // SgType* type = varArg->get_expression_type();
     SgType* type = varArg->get_type();

     ASSERT_not_null(operand);
     ASSERT_not_null(type);

#if 0
     printf ("In unparseVarArgOp(): info.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
     printf ("In unparseVarArgOp(): info.SkipEnumDefinition()  = %s \n",(info.SkipEnumDefinition() == true) ? "true" : "false");
#endif

  // DQ (1/7/2014): These should have been setup to be the same.
     ROSE_ASSERT(info.SkipClassDefinition() == info.SkipEnumDefinition());

     curprint ( "va_arg(");
     unparseExpression(operand,info);
     curprint ( ",");
     unp->u_type->unparseType(type,info);
     curprint ( ")");
   }

void
Unparse_ExprStmt::unparseVarArgEndOp(SgExpression* expr, SgUnparse_Info& info)
   {
  // printf ("Inside of Unparse_ExprStmt::unparseVarArgEndOp \n");

     SgVarArgEndOp* varArgEnd = isSgVarArgEndOp(expr);
     ASSERT_not_null(varArgEnd);
     SgExpression* operand = varArgEnd->get_operand_expr();
     ASSERT_not_null(operand);

  // DQ (9/16/2013): This was a problem pointed out by Phil Miller, it only has to be correct to make the resulting code link properly.
  // curprint("va_end(");
     curprint("__builtin_va_end(");
     unparseExpression(operand,info);
     curprint ( ")");
   }

void
Unparse_ExprStmt::unparseVarArgCopyOp(SgExpression* expr, SgUnparse_Info& info)
   {
  // printf ("Inside of Unparse_ExprStmt::unparseVarArgCopyOp \n");

     SgVarArgCopyOp* varArgCopy = isSgVarArgCopyOp(expr);

     SgExpression* lhsOperand = varArgCopy->get_lhs_operand();
     SgExpression* rhsOperand = varArgCopy->get_rhs_operand();

     ASSERT_not_null(lhsOperand);
     ASSERT_not_null(rhsOperand);

  // DQ (9/16/2013): This was a problem pointed out by Phil Miller, it only has to be correct to make the resulting code link properly.
  // curprint("va_copy(");
     curprint("__builtin_va_copy(");
     unparseExpression(lhsOperand,info);
     curprint ( ",");
     unparseExpression(rhsOperand,info);
     curprint ( ")" );
   }


#if 0
static bool 
subTreeContainsDesignatedInitializer ( SgExpression* exp )
   {
  // DQ (7/22/2013): This function traverses the AST and detects any SgDesignatedInitializer IR node.
  // The goal more specifically is to detect the use of array initializers that don't require the "=" 
  // in ther unparsed syntax.

     class ContainsDesignatedInitializer : public AstSimpleProcessing
        {
          public:
              bool hasDesignatedInitializer;

              ContainsDesignatedInitializer() : hasDesignatedInitializer(false) {}

              void visit ( SgNode* astNode )
                 {
                   SgDesignatedInitializer* designatedInitializer = isSgDesignatedInitializer(astNode);
                   if (designatedInitializer != NULL)
                      {
                     // This depends on the implemantation to have a SgExprListExp (which I would like to eliminate in the IR node design).
                        if ( isSgUnsignedLongVal(designatedInitializer->get_designatorList()->get_expressions()[0]) != NULL)
                           {
                             hasDesignatedInitializer = true;
                           }
                      }
                 }
        };

      ContainsDesignatedInitializer traversal;

      traversal.traverse(exp,preorder);

#if 0
      printf ("traversal.hasDesignatedInitializer = %s \n",traversal.hasDesignatedInitializer ? "true" : "false");
#endif

      return traversal.hasDesignatedInitializer;
   }
#endif


void
Unparse_ExprStmt::unparseDesignatedInitializer(SgExpression* expr, SgUnparse_Info & info)
   {

#define DEBUG_DESIGNATED_INITIALIZER 0

#if DEBUG_DESIGNATED_INITIALIZER
     printf ("TOP of unparseDesignatedInitializer: expr = %p \n",expr);
     curprint (string("\n/* Top of unparseDesignatedInitializer(") + expr->class_name().c_str() + ") */ \n");
#endif
#if 0
     expr->get_startOfConstruct()->display("In unparseDesignatedInitializer: debug");
#endif

#if 1
  // DQ (7/22/2013): New version of unparser code for this IR node. I think this is now
  // organized differently (required for C support of more complex designator cases).
     SgDesignatedInitializer* di = isSgDesignatedInitializer(expr);

     ROSE_ASSERT(di->get_designatorList()->get_expressions().empty() == false);

     SgExpression*  designator  = di->get_designatorList()->get_expressions()[0];
     SgInitializer* initializer = di->get_memberInit();

     SgVarRefExp* varRefExp = isSgVarRefExp(designator);
  // SgValueExp*  valueExp  = isSgValueExp(designator);

     bool isDataMemberDesignator   = (varRefExp != NULL);
     bool isArrayElementDesignator = (isSgUnsignedLongVal(designator) != NULL);

  // DQ (7/23/2013): These are relatively rare cases that we want to detect and allow.
     bool isCastDesignator         = (isSgCastExp(designator) != NULL);
     bool isAggregateInitializer   = (isSgAggregateInitializer(designator) != NULL);

     bool isAssignInitializer      = (isSgAssignInitializer(initializer) != NULL);

  // DQ (3/15/2015): Look for nested SgDesignatedInitializer (so we can supress the unparsed "=" syntax) (this case is demonstrated in test2015_03.c).
  // bool isInitializer_AggregateInitializer   = (isSgAggregateInitializer(initializer) != NULL);

  // bool outputDesignatedInitializer                   = (isDataMemberDesignator == true && varRefExp->get_symbol() != NULL);
  // bool outputDesignatedInitializerAssignmentOperator = (subTreeContainsDesignatedInitializer(initializer) == false && isCastDesignator == false);
  // bool outputDesignatedInitializerAssignmentOperator = (subTreeContainsDesignatedInitializer(initializer) == false && isCastDesignator == false && isAggregateInitializer == false);
  // bool outputDesignatedInitializerAssignmentOperator = true;
  // bool outputDesignatedInitializerAssignmentOperator = (subTreeContainsDesignatedInitializer(initializer) == false);
  // bool outputDesignatedInitializerAssignmentOperator = (isArrayElementDesignator == false);
  // bool outputDesignatedInitializerAssignmentOperator = (subTreeContainsDesignatedInitializer(initializer) == false) && (isArrayElementDesignator == false);

  // DQ (4/11/2015): Aggregate initializers should also have an unparsed "=" syntax.
  // bool outputDesignatedInitializerAssignmentOperator = (isArrayElementDesignator == false) || (isAssignInitializer == true);
  // bool outputDesignatedInitializerAssignmentOperator = ( (isArrayElementDesignator == false) || (isAssignInitializer == true) || (isInitializer_AggregateInitializer == true) );

#if DEBUG_DESIGNATED_INITIALIZER
     printf ("--- isInitializer_AggregateInitializer = %s \n",isInitializer_AggregateInitializer ? "true" : "false");
#endif

#if 0
  // DQ (5/11/2015): This is part of an overly sophisticated approach to control the use of "={}" syntax.
  // It is easier to just always normalize to the use of "={}" and then restrict specific cases where it is not allowed.

     if (isInitializer_AggregateInitializer == true)
        {
#if DEBUG_DESIGNATED_INITIALIZER
          printf ("--- Found memberInit to be a SgAggregateInitializer \n");
#endif
          SgAggregateInitializer* aggregateInitializer = isSgAggregateInitializer(initializer);
          ASSERT_not_null(aggregateInitializer);
          SgExprListExp* exprListExp = aggregateInitializer->get_initializers();
          ASSERT_not_null(exprListExp);
          if (exprListExp->get_expressions().size() == 1)
             {
               SgExpression* tmp_expr = exprListExp->get_expressions()[0];
               ASSERT_not_null(tmp_expr);

               SgDesignatedInitializer* designatedInitializer = isSgDesignatedInitializer(tmp_expr);
               if (designatedInitializer != NULL)
                  {
#if 1
                 // DQ (5/10/2015): This fails for test2015_85.c.
#if DEBUG_DESIGNATED_INITIALIZER || 0
                    printf ("--- Before possible reset: outputDesignatedInitializerAssignmentOperator = %s \n",outputDesignatedInitializerAssignmentOperator ? "true" : "false");
                    printf ("--- Mark outputDesignatedInitializerAssignmentOperator as false since there is a nested SgDesignatedInitializer \n");
#endif
#if 0
                 // DQ (5/10/2015): This code works for test2015_119.c, but fails for test2015_117.c.
                    outputDesignatedInitializerAssignmentOperator = false;
#else
                 // DQ (5/10/2015): Handle the different between test2015_119.c and test2015_117.c.
                    SgAggregateInitializer* nested_aggregateInitializer = isSgAggregateInitializer(designatedInitializer->get_memberInit());
                    SgAssignInitializer*    nested_assignInitializer    = isSgAssignInitializer(designatedInitializer->get_memberInit());

                 // Check the lhs to be a SgVarRef expression and not an array index (see test2013_37.c for case of array index).
                    SgExpression* nested_designator = designatedInitializer->get_designatorList()->get_expressions()[0];
                    SgVarRefExp*  nested_varRefExp  = isSgVarRefExp(nested_designator);

                    bool isArrayReference = (nested_aggregateInitializer != NULL && isSgArrayType(nested_aggregateInitializer->get_type()) != NULL);
                 // bool isClassReference = (nested_aggregateInitializer != NULL && isSgClassType(nested_aggregateInitializer->get_type()) != NULL);
                    bool isClassReference = (aggregateInitializer != NULL && isSgClassType(aggregateInitializer->get_type()) != NULL);
                    bool isArrayReference_outer = (aggregateInitializer != NULL && isSgArrayType(aggregateInitializer->get_type()) != NULL);
#if DEBUG_DESIGNATED_INITIALIZER
                    printf ("varRefExp                   = %p \n",varRefExp);
                    printf ("valueExp                    = %p \n",valueExp);
                    printf ("nested_aggregateInitializer = %p \n",nested_aggregateInitializer);
                    printf ("nested_assignInitializer    = %p \n",nested_assignInitializer);
                    printf ("nested_varRefExp            = %p \n",nested_varRefExp);
                    printf ("isArrayReference            = %s \n",isArrayReference ? "true" : "false");
                    printf ("isArrayReference_outer      = %s \n",isArrayReference_outer ? "true" : "false");
                    printf ("isClassReference            = %s \n",isClassReference ? "true" : "false");
#endif
                 // For test2015_119.c: (nested_aggregateInitializer != NULL && nested_varRefExp != NULL)
                 // For test2013_37.c: (nested_assignInitializer != NULL && nested_varRefExp == NULL)

                 // if (nested_assignInitializer != NULL)
                 // if (nested_aggregateInitializer != NULL)
                 // if (nested_aggregateInitializer != NULL && nested_varRefExp != NULL)
                 // if (nested_assignInitializer == NULL && nested_varRefExp != NULL)
                 // if (nested_aggregateInitializer != NULL && nested_varRefExp == NULL)
                 // if (nested_assignInitializer == NULL && nested_varRefExp != NULL)
#if 1
                 // DQ (5/10/2015): This is an overly complex rule, so we need a better one.  However, this will pass all regression tests.
                 // if ( (nested_assignInitializer != NULL && nested_varRefExp == NULL) || (nested_aggregateInitializer != NULL && nested_varRefExp != NULL) || isArrayReference == true)
                    if ( (nested_assignInitializer != NULL && nested_varRefExp == NULL) || (nested_aggregateInitializer != NULL && nested_varRefExp != NULL) || isArrayReference == true || 
                         (nested_assignInitializer != NULL && nested_varRefExp != NULL && isClassReference == true && varRefExp == NULL) )
#else
#error "DEAD CODE!"
                    if ( (nested_assignInitializer != NULL && nested_varRefExp == NULL) || (nested_aggregateInitializer != NULL && nested_varRefExp != NULL) || isArrayReference == true || isClassReference == true)
#endif
                       {
#if DEBUG_DESIGNATED_INITIALIZER
                         printf ("In unparseDesignatedInitializer: RESET outputDesignatedInitializerAssignmentOperator \n");
#endif
                         outputDesignatedInitializerAssignmentOperator = false;
                       }
#if DEBUG_DESIGNATED_INITIALIZER
                    printf ("In unparseDesignatedInitializer: outputDesignatedInitializerAssignmentOperator = %s \n",outputDesignatedInitializerAssignmentOperator ? "true" : "false");
#endif
#endif
#else
                 // DQ (4/11/2015): Testing for test2015_85.c.
                    printf ("--- Skip the reset of outputDesignatedInitializerAssignmentOperator as false for detected nesting of SgDesignatedInitializer \n");
#endif
                  }
             }
        }
#endif

#if DEBUG_DESIGNATED_INITIALIZER
     printf ("In unparseDesignatedInitializer: designator  = %p = %s \n",designator,designator->class_name().c_str());
     printf ("In unparseDesignatedInitializer: initializer = %p = %s \n",initializer,initializer->class_name().c_str());

     printf ("In unparseDesignatedInitializer: isArrayElementDesignator                      = %s \n",isArrayElementDesignator ? "true" : "false");
     printf ("In unparseDesignatedInitializer: isAggregateInitializer                        = %s \n",isAggregateInitializer ? "true" : "false");
     printf ("In unparseDesignatedInitializer: isAssignInitializer                           = %s \n",isAssignInitializer ? "true" : "false");
     printf ("In unparseDesignatedInitializer: isInitializer_AggregateInitializer            = %s \n",isInitializer_AggregateInitializer ? "true" : "false");
     printf ("In unparseDesignatedInitializer: isDataMemberDesignator                        = %s \n",isDataMemberDesignator ? "true" : "false");
     printf ("In unparseDesignatedInitializer: outputDesignatedInitializerAssignmentOperator = %s \n",outputDesignatedInitializerAssignmentOperator ? "true" : "false");
#endif

#if 0
     info.display("In unparseDesignatedInitializer()");
#endif

  // DQ (5/11/2015): This needs to be defined outside of this conditional case so that it can be used to supress the output of the "={}" syntax.
     bool isInUnion = false;
     if (isDataMemberDesignator == true)
        {
       // We need to check if this is a designator that is associated with a union (does it have to be an un-named union).
          SgVariableSymbol*   variableSymbol   = isSgVariableSymbol(varRefExp->get_symbol());
          SgClassDefinition*  classDefinition  = isSgClassDefinition(variableSymbol->get_declaration()->get_scope());
          SgClassDeclaration* classDeclaration = NULL;
          if (classDefinition != NULL)
             {
               classDeclaration = classDefinition->get_declaration();
             }

       // bool isInUnion = (classDeclaration != NULL && classDeclaration->get_class_type() == SgClassDeclaration::e_union);
          isInUnion = (classDeclaration != NULL && classDeclaration->get_class_type() == SgClassDeclaration::e_union);
#if DEBUG_DESIGNATED_INITIALIZER
          printf ("In unparseDesignatedInitializer: isInUnion = %s info.SkipClassDefinition() = %s \n",isInUnion ? "true" : "false",info.SkipClassDefinition() ? "true" : "false");
#endif
       // DQ (7/24/2013): Force isInUnion to false so that we can handle test2012_46.c:
       // int x = (((union ABC { int __in; int __i; }) { .__in = 42 }).__i);
       // isInUnion = false;
#if 0

#error "DEAD CODE!"

       // DQ (7/25/2013): We need to detect if this is in a function argument list.
          if (info.SkipClassDefinition() == false)
             {
               isInUnion = false;
             }
#else
       // Comment out to process test2013_32.c
       // isInUnion = false;

       // DQ (7/27/2013): Don't output designated initialized in function call arguments (appears to not be allowed).
          if (isInUnion == true)
             {
               bool isInFunctionCallArgument = SageInterface::getEnclosingNode<SgFunctionCallExp>(di);
#if DEBUG_DESIGNATED_INITIALIZER
               printf ("isInFunctionCallArgument = %s \n",isInFunctionCallArgument ? "true" : "false");
#endif
               if (isInFunctionCallArgument == false)
                  {
                    isInUnion = false;
#if DEBUG_DESIGNATED_INITIALIZER
                    printf ("reset isInUnion: isInUnion = %s \n",isInUnion ? "true" : "false");
#endif
                  }
             }
#endif

          if (isInUnion == false)
             {
            // A struct field
               curprint (".");
               unparseVarRef(designator, info);
             }
            else
             {
#if DEBUG_DESIGNATED_INITIALIZER
               printf ("Reset outputDesignatedInitializerAssignmentOperator = false \n");
#endif
            // DQ (10/22/2016): This variable is not used.
            // outputDesignatedInitializerAssignmentOperator = false;
             }
        }
       else
        {
          if (isArrayElementDesignator == true)
             {
               curprint ("[");
               unparseValue(designator, info);
               curprint ("]");
               isArrayElementDesignator = true;
             }
            else
             {
            // if (isCastDesignator == true)
               if (isCastDesignator == true || isAggregateInitializer == true)
                  {
                    printf ("WARNING: designator might be an inappropriate expression (expected SgVarRefExp or SgUnsignedLongVal, but this case might be OK): designator = %p = %s \n",designator,designator->class_name().c_str());
                  }
                 else
                  {
                    printf ("ERROR: designator is an inappropriate expression (should be SgVarRefExp or SgUnsignedLongVal): designator = %p = %s \n",designator,designator->class_name().c_str());
                    ROSE_ASSERT(false);
                  }
             }
        }

#if DEBUG_DESIGNATED_INITIALIZER
     printf ("In unparseDesignatedInitializer: outputDesignatedInitializerAssignmentOperator = %s \n",outputDesignatedInitializerAssignmentOperator ? "true" : "false");
     printf ("In unparseDesignatedInitializer: di->get_memberInit()                          = %p = %s \n",di->get_memberInit(),di->get_memberInit()->class_name().c_str());
#endif

#if 0
  // DQ (5/11/2015): Older code.
  // Only unparse the "=" if this is not another in a chain of SgAggregateInitializer IR nodes.
  // if (isSgAggregateInitializer(di->get_memberInit()) == NULL)
  // if (subTreeContainsDesignatedInitializer(initializer) == false)
  // if (subTreeContainsDesignatedInitializer(initializer) == false && isCastDesignator == false)
     if (outputDesignatedInitializerAssignmentOperator == true)
        {
          curprint (" = ");
        }

     unparseExpression(initializer, info);
#else

  // DQ (5/11/2015): Use this normalized form for the designated initializer.  It is becoming too difficult to
  // figure out when to use and not use the "=" operator and it is not available as information explicitly in 
  // the AST.

  // DQ (5/11/2015): We need to look at the SgAggregateInitializer and see if it will be outputing "{}".
     SgAggregateInitializer* aggregateInitializer = isSgAggregateInitializer(initializer);
     bool need_explicit_braces_in_aggregateInitializer = (aggregateInitializer != NULL && aggregateInitializer->get_need_explicit_braces());

#if 0
     printf ("In unparseDesignatedInitializer: Changed default value of need_explicit_braces in unparser (must be set correctly in AST) \n");

  // Variable used to control output of normalized syntax "={}".
  // bool need_explicit_braces = (need_explicit_braces_in_aggregateInitializer == false);
     bool need_explicit_braces = (need_explicit_braces_in_aggregateInitializer == true);
#else
  // Variable used to control output of normalized syntax "={}".
     bool need_explicit_braces = (need_explicit_braces_in_aggregateInitializer == false);
#endif

#if DEBUG_DESIGNATED_INITIALIZER
     printf ("In unparseDesignatedInitializer: initial value from AST: need_explicit_braces = %s \n",need_explicit_braces ? "true" : "false");
#endif

  // DQ (5/11/2015): Supress output of normalized syntax "={}" for specific kinds of initializers (to avoid warnings in generated code).
     if (need_explicit_braces == true && isAssignInitializer == true)
        {
          SgAssignInitializer* assignInitializer = isSgAssignInitializer(initializer);
          ASSERT_not_null(assignInitializer);
          SgValueExp*       valueExp       = isSgValueExp(assignInitializer->get_operand());
          SgCastExp*        castExp        = isSgCastExp(assignInitializer->get_operand());
          SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(assignInitializer->get_operand());

       // DQ (5/12/2015): We need to supress "={}" when the types are classes (so that the class assignment operator will be called).
          ASSERT_not_null(assignInitializer->get_operand());
          ASSERT_not_null(assignInitializer->get_operand()->get_type());

          SgClassType* classType = isSgClassType( assignInitializer->get_operand()->get_type()->stripType(SgType::STRIP_MODIFIER_TYPE | SgType::STRIP_REFERENCE_TYPE | SgType::STRIP_RVALUE_REFERENCE_TYPE | SgType::STRIP_TYPEDEF_TYPE) );
          bool isClassType = (classType != NULL);

#if DEBUG_DESIGNATED_INITIALIZER
          printf ("assignInitializer->get_operand()->get_type() = %p = %s \n",assignInitializer->get_operand()->get_type(),assignInitializer->get_operand()->get_type()->class_name().c_str());
          printf ("   --- isClassType = %s \n",isClassType ? "true" : "false");
#endif

       // if (valueExp != NULL || castExp != NULL || functionRefExp != NULL)
          if (valueExp != NULL || castExp != NULL || functionRefExp != NULL || isClassType == true)
             {
#if DEBUG_DESIGNATED_INITIALIZER
               printf ("In unparseDesignatedInitializer: reset need_explicit_braces to false \n");
#endif
               need_explicit_braces = false;

#if DEBUG_DESIGNATED_INITIALIZER
               printf ("In unparseDesignatedInitializer: reset based on kind of expression: need_explicit_braces = %s \n",need_explicit_braces ? "true" : "false");
#endif
             }
        }

#if DEBUG_DESIGNATED_INITIALIZER
     printf ("In unparseDesignatedInitializer: need_explicit_braces = %s \n",need_explicit_braces ? "true" : "false");
     printf ("In unparseDesignatedInitializer: isInUnion = %s \n",isInUnion ? "true" : "false");
#endif

  // if (outputDesignatedInitializerAssignmentOperator == true)
     if (isInUnion == false)
        {
          curprint (" = ");
#if DEBUG_DESIGNATED_INITIALIZER
          printf ("In unparseDesignatedInitializer: isInitializer_AggregateInitializer = %s \n",isInitializer_AggregateInitializer ? "true" : "false");
          printf ("In unparseDesignatedInitializer: initializer = %p = %s \n",initializer,initializer->class_name().c_str());
#endif
       // if (isInitializer_AggregateInitializer == false)
       // if (need_explicit_braces == false)
          if (need_explicit_braces == true)
             {
#if DEBUG_DESIGNATED_INITIALIZER
               curprint (" /* designated initializer */ ");
#endif
               curprint ("{");
             }
        }

     unparseExpression(initializer, info);

  // if (outputDesignatedInitializerAssignmentOperator == true)
     if (isInUnion == false)
        {
       // if (isInitializer_AggregateInitializer == false)
       // if (need_explicit_braces == false)
          if (need_explicit_braces == true)
             {
               curprint ("}");
#if DEBUG_DESIGNATED_INITIALIZER
               curprint (" /* designated initializer */ ");
#endif
             }
        }
#endif

#else
  // Liao, fixing bug 355, 6/16/2009
  // for multidimensional array's designated initializer, don't emit '=' until it reaches the last dimension
  // TODO this is not the ultimate fix: EDG uses nested tree for multidimensional array's designated initializer
  // while ROSE's SgDesignatedInitializer is designed to have a flat list for designators 
  // But the EDG_SAGE_connect part generated nested ROSE AST tree following EDG's IR tree.
     bool lastDesignator           = true; 
     bool isArrayElementDesignator = false;

#error "DEAD CODE!"

  // DQ (10/22/2012): Only output the SgDesignatedInitializer if it is not compiler generated or if it is compiler generated, only if it is marked to be output.
     bool outputDesignatedInitializer = (expr->get_startOfConstruct()->isCompilerGenerated() == false);
     if (expr->get_startOfConstruct()->isCompilerGenerated() == true && expr->get_startOfConstruct()->isOutputInCodeGeneration() == false)
        {
          outputDesignatedInitializer = false;
        }

  // DQ (7/20/2013): Reset this.
     printf ("Always output the designated initializer: outputDesignatedInitializer = %s (will be reset) \n",outputDesignatedInitializer ? "true" : "false");
     outputDesignatedInitializer = true;

#if 0
     printf ("In unparseDesignatedInitializer: outputDesignatedInitializer = %s \n",outputDesignatedInitializer ? "true" : "false");
#endif

     if (outputDesignatedInitializer == true)
        {
       // The SgDesignatedInitializer is generally compiler generated, but not always wanted as output (see test2012_74.c).
          SgDesignatedInitializer* di = isSgDesignatedInitializer(expr);
          const SgExpressionPtrList& designators = di->get_designatorList()->get_expressions();
          for (size_t i = 0; i < designators.size(); ++i)
             {
               SgExpression* designator = designators[i];

               printf ("In loop: designator = %p = %s \n",designator,designator->class_name().c_str());

            // DQ (7/20/2013): Make up for incorrect handling of SgDesignatedInitializer in AST (as a test, then fix it properly if this works).
               if (isSgExprListExp(designator) != NULL)
                  {
                    SgExprListExp* designatorList = isSgExprListExp(designator);
                    ROSE_ASSERT(designatorList->get_expressions().size() == 1);
                    designator = designatorList->get_expressions()[0];

                    printf ("In loop (reset): designator = %p = %s \n",designator,designator->class_name().c_str());
                  }

               if (isSgVarRefExp(designator))
                  {
                 // A struct field
                    curprint ( "." );
                    unparseVarRef(designator, info);
                  }
                 else
                  {
                    if (isSgValueExp(designator))
                       {
                         curprint ( "[" );
                         unparseValue(designator, info);
                         curprint ( "]" );
                         isArrayElementDesignator = true;
                       }
                  }
             }

       // check if the current designator is the last one within an aggregate initializer
       // e.g. double grid[3] [4] = { [0][1]=8};
       // [0] is not the last one, [1] is.  Only emit '=' after [1].
       // The reference code is gen_designator() in cp_gen_be.c
          if (isSgAggregateInitializer(expr->get_parent()->get_parent()))
             {
               SgInitializer* child_init = di->get_memberInit();
               if (isSgAggregateInitializer(child_init))
                  {
                 // grab the first one
                    SgExpression* grand_child = (isSgAggregateInitializer(child_init)->get_initializers()->get_expressions())[0];
                    if (isSgDesignatedInitializer(grand_child))
                         lastDesignator = false;
                  }
             }

       // Don't emit '=' if it is an array element and is not the last designator
          if ( !(isArrayElementDesignator && !lastDesignator) )
             {
               curprint (" = ");
             }

          unparseExpression(di->get_memberInit(), info);
        }
       else
        {
       // This is the case taken for the test2012_74.c test code.
#if 0
          printf ("This SgDesignatedInitializer is compiler generated and should not be output \n");
#endif
          SgDesignatedInitializer* di = isSgDesignatedInitializer(expr);
          unparseExpression(di->get_memberInit(), info);
        }
#endif
   }


void
Unparse_ExprStmt::unparsePseudoDtorRef(SgExpression* expr, SgUnparse_Info & info)
   {
     SgPseudoDestructorRefExp* pdre = isSgPseudoDestructorRefExp(expr);
     ASSERT_not_null(pdre);

     SgType *objt = pdre->get_object_type();

#if 0
     printf ("In unparsePseudoDtorRef(): pdre->get_object_type() = %p = %s \n",objt,objt->class_name().c_str());
#endif

  // curprint("~");

  // if (SgNamedType *nt = isSgNamedType(objt))
     SgNamedType* namedType = isSgNamedType(objt);
     if (namedType != NULL)
        {

       // DQ (1/18/2020): Adding support for name qualification (see Cxx11_tests/test2020_56.C).
          SgName nameQualifier = pdre->get_qualified_name_prefix();
#if 0
          printf ("In unparsePseudoDtorRef(): nameQualifier = %s \n",nameQualifier.str());
#endif
          if (nameQualifier.is_null() == false)
             {
               SgName nameOfType = namedType->get_name();

               SgName name = nameQualifier + nameOfType + "::";
#if 0
               printf ("In unparsePseudoDtorRef(): name = %s \n",name.str());
#endif
            // curprint(nameQualifier.str());
               curprint(name.str());
             }
#if 0
          printf ("Unparser will output SgPseudoDestructorRefExp using the class name only \n");
#endif
          curprint("~");

          curprint(namedType->get_name().str());

       // DQ (3/14/2012): Note that I had to add this for the case of EDG 4.3, but it was not required previously for EDG 3.3, something in ROSE has likely changed.
          curprint("()");
        }
       else
        {
          curprint("~");

       // DQ (3/14/2012): This is the case of of a primative type (e.g. "~int"), which is allowed.
       // PC: I do not think this case will ever occur in practice.  If it does, the resulting
       // code will be invalid.  It may, however, appear in an implicit template instantiation.
       // printf ("WARNING: This case of unparsing in unparsePseudoDtorRef() using unparseType() may not work \n");
          unp->u_type->unparseType(objt, info);
        }

#if 0
     printf ("Leaving unparsePseudoDtorRef(): pdre->get_object_type() = %p = %s \n",objt,objt->class_name().c_str());
#endif
   }


// TV (05/06/2010): CUDA, Kernel call unparse
void Unparse_ExprStmt::unparseCudaKernelCall(SgExpression* expr, SgUnparse_Info& info) {

     SgCudaKernelCallExp* kernel_call = isSgCudaKernelCallExp(expr);
     ASSERT_not_null(kernel_call);
     
     unparseExpression(kernel_call->get_function(), info);
     
     SgCudaKernelExecConfig * exec_config = isSgCudaKernelExecConfig(kernel_call->get_exec_config());
     ASSERT_not_null(exec_config);

     curprint ("<<<");
     
     SgExpression * grid_exp = exec_config->get_grid();
     ASSERT_not_null(grid_exp);
     SgConstructorInitializer * con_init = isSgConstructorInitializer(grid_exp);
     if (con_init != NULL && unp->u_sage->isOneElementList(con_init))
          unparseOneElemConInit(con_init, info);
     else
          unparseExpression(grid_exp, info);
     curprint (",");
     
     SgExpression * blocks_exp = exec_config->get_blocks();
     ASSERT_not_null(blocks_exp);
     con_init = isSgConstructorInitializer(blocks_exp);
     if (con_init != NULL && unp->u_sage->isOneElementList(con_init))
          unparseOneElemConInit(con_init, info);
     else
          unparseExpression(blocks_exp, info);
     
     SgExpression * shared_exp = exec_config->get_shared();
     if (shared_exp != NULL) {
          curprint (",");
          con_init = isSgConstructorInitializer(shared_exp);
          if (con_init != NULL && unp->u_sage->isOneElementList(con_init))
               unparseOneElemConInit(con_init, info);
          else
               unparseExpression(shared_exp, info);
          
          SgExpression * stream_exp = exec_config->get_stream();
          if (stream_exp != NULL) {
               curprint (",");
               con_init = isSgConstructorInitializer(stream_exp);
               if (con_init != NULL && unp->u_sage->isOneElementList(con_init))
                    unparseOneElemConInit(con_init, info);
               else
                    unparseExpression(stream_exp, info);
          }
     }
          
     curprint (">>>");
     
     curprint ( "(");
     if ( kernel_call->get_args() != NULL) {
          SgExpressionPtrList& list = kernel_call->get_args()->get_expressions();
          SgExpressionPtrList::iterator arg = list.begin();
          while (arg != list.end()) {
               con_init = isSgConstructorInitializer(*arg);
               if (con_init != NULL && unp->u_sage->isOneElementList(con_init))
                    unparseOneElemConInit(con_init, info);
               else
                    unparseExpression((*arg), info);
               arg++;
               if (arg != list.end())
                    curprint ( ",");
          }
     }
     curprint ( ")");
}
