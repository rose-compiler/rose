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

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

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

          case LAMBDA_EXP:              { unparseLambdaExpression(expr, info); break; }

          default:
             {
            // printf ("Default reached in switch statement for unparsing expressions! expr = %p = %s \n",expr,expr->class_name().c_str());
               printf ("Default reached in switch statement for unparsing expressions! expr = %p = %s \n",expr,expr->class_name().c_str());
               ROSE_ASSERT(false);
               break;
             }
        }
   }


void
Unparse_ExprStmt::unparseLambdaExpression(SgExpression* expr, SgUnparse_Info& info)
   {
     SgLambdaExp* lambdaExp = isSgLambdaExp(expr);
     ROSE_ASSERT(lambdaExp != NULL);

     curprint(" [");

     if (lambdaExp->get_capture_default() == true)
        {
          curprint("=,");
        }

     if (lambdaExp->get_default_is_by_reference() == true)
        {
          curprint("&,");
        }

     ROSE_ASSERT(lambdaExp->get_lambda_capture_list() != NULL);
     size_t bound = lambdaExp->get_lambda_capture_list()->get_capture_list().size();
     for (size_t i = 0; i < bound; i++)
        {
          SgLambdaCapture* lambdaCapture = lambdaExp->get_lambda_capture_list()->get_capture_list()[i];
          ROSE_ASSERT(lambdaCapture != NULL);

          if (lambdaCapture->get_capture_variable() != NULL)
             {
               if (lambdaCapture->get_capture_by_reference() == true)
                  {
                    curprint("&");
                  }

               unp->u_exprStmt->unparseExpression(lambdaCapture->get_capture_variable(),info);
             }

          if (i < bound-1)
             {
               curprint(",");
             }
        }
     curprint("] ");

     SgFunctionDeclaration* lambdaFunction =  lambdaExp->get_lambda_function();
     ROSE_ASSERT(lambdaFunction != NULL);
     ROSE_ASSERT(lambdaFunction->get_firstNondefiningDeclaration() != NULL);
     ROSE_ASSERT(lambdaFunction->get_definingDeclaration() != NULL);

#if 0
     printf ("lambdaFunction                                    = %p = %s \n",lambdaFunction,lambdaFunction->class_name().c_str());
     printf ("lambdaFunction->get_firstNondefiningDeclaration() = %p = %s \n",lambdaFunction->get_firstNondefiningDeclaration(),lambdaFunction->get_firstNondefiningDeclaration()->class_name().c_str());
     printf ("lambdaFunction->get_definingDeclaration()         = %p = %s \n",lambdaFunction->get_definingDeclaration(),lambdaFunction->get_definingDeclaration()->class_name().c_str());
#endif

     if (lambdaExp->get_has_parameter_decl() == true)
        {
       // Output the function parameters
          curprint("(");
          unparseFunctionArgs(lambdaFunction,info);
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
          curprint(" -> ");
          ROSE_ASSERT(lambdaFunction != NULL);
          ROSE_ASSERT(lambdaFunction->get_type() != NULL);
          SgType* returnType = lambdaFunction->get_type()->get_return_type();
          ROSE_ASSERT(returnType != NULL);
          unp->u_type->unparseType(returnType,info);
        }



  // Use a new SgUnparse_Info object to support supression of the SgThisExp where compiler generated.
  // This is required because the function is internally a member function but can't explicitly refer 
  // to a "this" expression.
     SgUnparse_Info ninfo(info);
     ninfo.set_supressImplicitThisOperator();

  // Output the function definition
     ROSE_ASSERT(lambdaFunction->get_definition() != NULL);
     unparseStatement(lambdaFunction->get_definition()->get_body(), ninfo);

#if 0
     printf ("Exitng as a test! \n");
     ROSE_ASSERT(false);
#endif
   }


// DQ (8/11/2014): Added more general support for function parameter expressions (required for C++11 support).
void
Unparse_ExprStmt::unparseFunctionParameterRefExpression (SgExpression* expr, SgUnparse_Info& info)
   {
     ROSE_ASSERT(expr != NULL);

     SgFunctionParameterRefExp* functionParameterRefExp = isSgFunctionParameterRefExp(expr);
     ROSE_ASSERT(functionParameterRefExp != NULL);

#if 0
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
          ROSE_ASSERT(functionParameterRefExp->get_base_type() != NULL);
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
     ROSE_ASSERT(expr != NULL);

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
     ROSE_ASSERT(template_parameter_value != NULL);

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
     ROSE_ASSERT(func_ref != NULL);

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
     ROSE_ASSERT (templateInstantiationDeclaration != NULL);

#if 0
     printf ("In unparseTemplateName(): templateInstantiationDeclaration = %p \n",templateInstantiationDeclaration);
#endif

     unp->u_exprStmt->curprint ( templateInstantiationDeclaration->get_templateName().str());

  // DQ (8/24/2014): Made this a warning instead of an error (see unparseToString/test2004_35.C).
  // DQ (5/7/2013): I think these should be false so that the full type will be output.
     if (info.isTypeFirstPart()  == true)
        {
          printf ("WARNING: In unparseTemplateName(): info.isTypeFirstPart() == true \n");
        }
  // ROSE_ASSERT(info.isTypeFirstPart()  == false);
     ROSE_ASSERT(info.isTypeSecondPart() == false);

#if 1
  // DQ (6/21/2011): Refactored this code to generate more then templated class names.
     unparseTemplateArgumentList(templateInstantiationDeclaration->get_templateArguments(),info);
#else
     const SgTemplateArgumentPtrList& templateArgListPtr = templateInstantiationDeclaration->get_templateArguments();
     if (!templateArgListPtr.empty())
        {
       // printf ("templateArgListPtr->size() = %" PRIuPTR " \n",templateArgListPtr->size());

       // DQ (4/18/2005): We would like to avoid output of "<>" if possible so verify that there are template arguments
          ROSE_ASSERT(templateArgListPtr.size() > 0);

#error "DEAD CODE!"

          unp->u_exprStmt->curprint ( "< ");
          SgTemplateArgumentPtrList::const_iterator i = templateArgListPtr.begin();
          while (i != templateArgListPtr.end())
             {
            // printf ("templateArgList element *i = %s \n",(*i)->sage_class_name());
#if 0
               unp->u_exprStmt->curprint ( string("/* templateArgument is explicitlySpecified = ") + 
                      (((*i)->get_explicitlySpecified() == true) ? "true" : "false") + " */");
#endif

               unparseTemplateArgument(*i,info);
               i++;
               if (i != templateArgListPtr.end())
                    unp->u_exprStmt->curprint ( " , ");
             }
          unp->u_exprStmt->curprint ( " > ");
        }
#endif
   }


void
Unparse_ExprStmt::unparseTemplateFunctionName(SgTemplateInstantiationFunctionDecl* templateInstantiationFunctionDeclaration, SgUnparse_Info& info)
   {
  // DQ (6/21/2011): Generated this function from refactored call to unparseTemplateArgumentList
     ROSE_ASSERT (templateInstantiationFunctionDeclaration != NULL);

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
          unparseTemplateArgumentList(templateInstantiationFunctionDeclaration->get_templateArguments(),info);
        }
   }


void
Unparse_ExprStmt::unparseTemplateMemberFunctionName(SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDeclaration, SgUnparse_Info& info)
   {
  // DQ (5/25/2013): Generated this function to match that of unparseTemplateFunctionName().
     ROSE_ASSERT (templateInstantiationMemberFunctionDeclaration != NULL);

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
   }



void
Unparse_ExprStmt::unparseTemplateArgumentList(const SgTemplateArgumentPtrList & templateArgListPtr, SgUnparse_Info& info)
   {
  // DQ (7/23/2012): This is one of three locations where the template arguments are assembled and where 
  // the name generated identically (in each case) is critical.  Not clear how to best refactor this code.
  // The other two are:
  //      SgName SageBuilder::appendTemplateArgumentsToName( const SgName & name, const SgTemplateArgumentPtrList & templateArgumentsList)
  // and in:
  //      void SgDeclarationStatement::resetTemplateNameSupport ( bool & nameResetFromMangledForm, SgName & name )
  // It is less clear how to refactor this code.

#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
  // DQ (8/24/2012): Print this out a little less often; but still enough so that we know to fix this later.
     static int counter = 0;
     if (counter++ % 100 == 0)
        {
          printf ("In Unparse_ExprStmt::unparseTemplateArgumentList(): CRITICAL FUNCTION TO BE REFACTORED \n");
        }
#endif

#if 0
     printf ("In unparseTemplateArgumentList(): templateArgListPtr.size() = %" PRIuPTR " \n",templateArgListPtr.size());
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

     if (!templateArgListPtr.empty())
        {
#if 0
          printf ("In unparseTemplateArgumentList(): templateArgListPtr.size() = %" PRIuPTR " \n",templateArgListPtr.size());
#endif
       // DQ (4/18/2005): We would like to avoid output of "<>" if possible so verify that there are template arguments
          ROSE_ASSERT(templateArgListPtr.size() > 0);

       // DQ (5/6/2013): I think these should be false so that the full type will be output.
          ROSE_ASSERT(ninfo.isTypeFirstPart()  == false);
          ROSE_ASSERT(ninfo.isTypeSecondPart() == false);

          unp->u_exprStmt->curprint ( "< ");
          SgTemplateArgumentPtrList::const_iterator i = templateArgListPtr.begin();
          while (i != templateArgListPtr.end())
             {
#if 0
               printf ("In unparseTemplateArgumentList(): templateArgList element *i = %s explicitlySpecified = %s \n",(*i)->class_name().c_str(),((*i)->get_explicitlySpecified() == true) ? "true" : "false");
#endif
#if 0
               unp->u_exprStmt->curprint ( string("/* templateArgument is explicitlySpecified = ") + (((*i)->get_explicitlySpecified() == true) ? "true" : "false") + " */");
#endif
            // unparseTemplateArgument(*i,info);
               unparseTemplateArgument(*i,ninfo);
               i++;

               if (i != templateArgListPtr.end())
                  {
                 // unp->u_exprStmt->curprint(" , ");

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

                  }
             }

          unp->u_exprStmt->curprint(" > ");
        }
       else
        {
       // DQ (5/26/2014): In the case of a template instantiation with empty template argument list, output
       // a " " to be consistant with the behavior when there is a non-empty template argument list.
       // This is a better fix for the template issue that Robb pointed out and that was fixed last week.
          unp->u_exprStmt->curprint(" ");
        }

#if 0
     printf ("Leaving Unparse_ExprStmt::unparseTemplateArgumentList(): CRITICAL FUNCTION TO BE REFACTORED \n");
#endif
   }


void
Unparse_ExprStmt::unparseTemplateParameterList( const SgTemplateParameterPtrList & templateParameterList, SgUnparse_Info& info)
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
               ROSE_ASSERT(templateParameter != NULL);
#if 0
               printf ("In unparseTemplateParameterList(): templateParameter = %p \n",templateParameter);
#endif
               unparseTemplateParameter(templateParameter,info);

               i++;

               if (i != templateParameterList.end())
                  {
                    curprint (",");
                  }
             }

          curprint ("> ");
        }
   }


void
Unparse_ExprStmt::unparseTemplateParameter(SgTemplateParameter* templateParameter, SgUnparse_Info& info)
   {
     ROSE_ASSERT(templateParameter != NULL);

#if 0
     printf ("In unparseTemplateParameter(): templateParameter = %p \n",templateParameter);
#endif

     switch(templateParameter->get_parameterType())
        {
          case SgTemplateParameter::type_parameter:
             {
            // DQ (9/7/2014): Added support for case SgTemplateParameter::type_parameter.
               SgType* type = templateParameter->get_type();
               ROSE_ASSERT(type != NULL);
#if 0
               printf ("unparseTemplateParameter(): case SgTemplateParameter::type_parameter: type = %p = %s \n",type,type->class_name().c_str());
#endif

            // DQ (9/9/2014): Note that this is now constrcuted to be a SgClassType were ever possible.
            // This allows the name qualification to be resolved when unparsing template declarations.
               SgTemplateType* templateType = isSgTemplateType(type);
            // ROSE_ASSERT(templateType != NULL);
               if (templateType == NULL)
                  {
#if 0
                    printf ("unparseTemplateParameter(): case SgTemplateParameter::type_parameter: (templateType == NULL): type = %p = %s \n",type,type->class_name().c_str());
#endif
                 // When unparsing a template paramter we only want to use the name.
                    SgClassType* classType = isSgClassType(type);
                    if (classType != NULL)
                       {
                         string name = classType->get_name();
                         curprint(name);
                       }
                      else
                       {
                         SgUnparse_Info ninfo(info);
                         unp->u_type->unparseType(type,ninfo);
                       }
                  }
                 else
                  {
                    string name = templateType->get_name();
#if 0
                    printf ("unparseTemplateParameter(): case SgTemplateParameter::type_parameter: type->get_name() = %s \n",name.c_str());
#endif
                 // unp->u_exprStmt->curprint(" typename ");
                    curprint(name);
                  }

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
                    ROSE_ASSERT(templateParameter->get_initializedName() != NULL);

                    SgType* type = templateParameter->get_initializedName()->get_type();
                    ROSE_ASSERT(type != NULL);
#if 0
                    printf ("unparseTemplateParameter(): case SgTemplateParameter::nontype_parameter: templateParameter->get_initializedName()->get_type() = %p = %s \n",type,type->class_name().c_str());
#endif
                 // DQ (9/10/2014): Note that this will unparse "int T" which we want in the template header, but not in the template parameter list.
                 // unp->u_type->outputType<SgInitializedName>(templateParameter->get_initializedName(),type,info);
                 // SgUnparse_Info ninfo(info);
                 // unp->u_type->unparseType(type,ninfo);
                    curprint(templateParameter->get_initializedName()->get_name());
                  }

               break;
             }

          case SgTemplateParameter::template_parameter:
             {
               ROSE_ASSERT(templateParameter->get_templateDeclaration() != NULL);
               SgTemplateDeclaration* templateDeclaration = isSgTemplateDeclaration(templateParameter->get_templateDeclaration());
               ROSE_ASSERT(templateDeclaration != NULL);
#if 0
               printf ("unparseTemplateParameter(): case SgTemplateParameter::template_parameter: output name = %s \n",templateDeclaration->get_name().str());
#endif
#if 0
            // This is a part of the template header that is not output here.
               SgTemplateParameterPtrList & templateParameterList = templateDeclaration->get_templateParameters();
               SgTemplateParameterPtrList::iterator i = templateParameterList.begin();
               curprint(" template < ");
               while (i != templateParameterList.end())
                  {
                    SgUnparse_Info newInfo(info);
                    curprint(" typename ");
                    unparseTemplateParameter(*i,newInfo);
                 // curprint(" SgTemplateDeclaration_name ");

                    i++;

                    if (i != templateParameterList.end())
                         curprint(",");
                  }

               curprint(" > ");
#endif
               curprint(templateDeclaration->get_name());
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

#if 0
     printf ("unparseTemplateParameter(): Sorry, not implemented! \n");
     ROSE_ASSERT(false);
#endif
   }


#if 0
// Saved version of this function.  We want to build a verison that will output a string seperately
// and then use this function to output the string.  Then the front-end can use the same function to generate 
// the string.
void
Unparse_ExprStmt::unparseTemplateArgument(SgTemplateArgument* templateArgument, SgUnparse_Info& info)
   {
     ROSE_ASSERT(templateArgument != NULL);

#error "DEAD CODE!"

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES
     printf ("Unparse TemplateArgument (%p) \n",templateArgument);
     unp->u_exprStmt->curprint ( "\n/* Unparse TemplateArgument */ \n");
  // unp->u_exprStmt->curprint ( std::endl;
     unp->u_exprStmt->curprint ( "\n");
#endif

#if 0
     unp->u_exprStmt->curprint ( string("/* templateArgument is explicitlySpecified = ") + 
            ((templateArgument->get_explicitlySpecified() == true) ? "true" : "false") +
            " */");
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

#error "DEAD CODE!"

  // DQ (5/14/2011): Added support for newer name qualification implementation.
  // printf ("In unparseTemplateArgument(): templateArgument->get_name_qualification_length() = %d \n",templateArgument->get_name_qualification_length());
     newInfo.set_name_qualification_length(templateArgument->get_name_qualification_length());
     newInfo.set_global_qualification_required(templateArgument->get_global_qualification_required());
     newInfo.set_type_elaboration_required(templateArgument->get_type_elaboration_required());

  // DQ (5/30/2011): Added support for name qualification.
     newInfo.set_reference_node_for_qualification(templateArgument);
     ROSE_ASSERT(newInfo.get_reference_node_for_qualification() != NULL);

#if 0
     printf ("Exiting in unparseTemplateArgument() to see whate this is called \n");
     ROSE_ASSERT(false);
#endif

  // ROSE_ASSERT(newInfo.isTypeFirstPart() == false);
  // ROSE_ASSERT(newInfo.isTypeSecondPart() == false);

#error "DEAD CODE!"

     switch (templateArgument->get_argumentType())
        {
          case SgTemplateArgument::type_argument:
             {
               ROSE_ASSERT (templateArgument->get_type() != NULL);

            // printf ("In unparseTemplateArgument(): templateArgument->get_type() = %s \n",templateArgument->get_type()->class_name().c_str());

#if OUTPUT_DEBUGGING_INFORMATION
               printf ("In unparseTemplateArgument(): templateArgument->get_type() = %s \n",templateArgument->get_type()->sage_class_name());
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
               ROSE_ASSERT(newInfo.get_reference_node_for_qualification() != NULL);
               printf ("newInfo.get_reference_node_for_qualification() = %p = %s \n",newInfo.get_reference_node_for_qualification(),newInfo.get_reference_node_for_qualification()->class_name().c_str());
#endif

#error "DEAD CODE!"

#if 1
            // DQ (5/28/2011): We have to handle the name qualification directly since types can be qualified 
            // different and so it depends upon where the type is referenced.  Thus the qualified name is 
            // stored in a map to the IR node that references the type.
               SgName nameQualifier;
               if (templateArgument->get_name_qualification_length() > 0)
                  {
                    std::map<SgNode*,std::string>::iterator i = SgNode::get_globalQualifiedNameMapForTypes().find(templateArgument);
                    ROSE_ASSERT(i != SgNode::get_globalQualifiedNameMapForTypes().end());
                    if (i != SgNode::get_globalQualifiedNameMapForTypes().end())
                       {
                         nameQualifier = i->second;
#if 0
                         printf ("In unparseTemplateArgument(): Found a valid name qualification: nameQualifier %s \n",nameQualifier.str());
#endif
                         curprint(nameQualifier);
                       }
                  }
#endif
            // DQ (7/23/2011): To unparse the type directly we can't have either of these set!
            // ROSE_ASSERT(newInfo.isTypeFirstPart()  == false);
            // ROSE_ASSERT(newInfo.isTypeSecondPart() == false);

            // This will unparse the type will any required name qualification.
               unp->u_type->unparseType(templateArgument->get_type(),newInfo);
               break;
             }

#error "DEAD CODE!"

          case SgTemplateArgument::nontype_argument:
             {
               ROSE_ASSERT (templateArgument->get_expression() != NULL);
#if OUTPUT_DEBUGGING_INFORMATION
               printf ("In unparseTemplateArgument(): templateArgument->get_expression() = %s \n",templateArgument->get_expression()->sage_class_name());
               unp->u_exprStmt->curprint ( "\n /* templateArgument->get_expression() */ \n");
#endif
            // curprint ( "\n /* SgTemplateArgument::nontype_argument */ \n");

            // DQ (1/5/2007): test2007_01.C demonstrated where this expression argument requires qualification.
            // printf ("Template argument = %p = %s \n",templateArgument->get_expression(),templateArgument->get_expression()->class_name().c_str());

               unp->u_exprStmt->unparseExpression(templateArgument->get_expression(),newInfo);
            // printf ("Error: nontype_argument case not implemented in Unparse_ExprStmt::unparseTemplateArgument \n");
            // ROSE_ABORT();
               break;
             }

#error "DEAD CODE!"

          case SgTemplateArgument::template_template_argument:
             {
            // unparseTemplateName(templateArgument->xxx,newInfo);
               ROSE_ASSERT(templateArgument->get_templateDeclaration() != NULL);

            // curprint ( "\n /* SgTemplateArgument::template_template_argument */ \n");

            // DQ (8/24/2006): Skip output of the extra space.
            // unp->u_exprStmt->curprint ( templateArgument->get_templateDeclaration()->get_name().str() << " ";
#ifdef TEMPLATE_DECLARATIONS_DERIVED_FROM_NON_TEMPLATE_DECLARATIONS
               unp->u_exprStmt->curprint ( templateArgument->get_templateDeclaration()->get_template_name().str());
#else
               unp->u_exprStmt->curprint ( templateArgument->get_templateDeclaration()->get_name().str());
#endif
            // printf ("Error: template_argument case not implemented in Unparse_ExprStmt::unparseTemplateArgument \n");
            // ROSE_ABORT();
               break;
             }

          case SgTemplateArgument::argument_undefined:
             {
               printf ("Error argument_undefined in Unparse_ExprStmt::unparseTemplateArgument \n");
               ROSE_ABORT();
               break;
             }

#error "DEAD CODE!"

          default:
             {
               printf ("Error default reached in Unparse_ExprStmt::unparseTemplateArgument \n");
               ROSE_ABORT();
             }
        }

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES
     printf ("Leaving unparseTemplateArgument (%p) \n",templateArgument);
     unp->u_exprStmt->curprint ( string("\n/* Bottom of unparseTemplateArgument */ \n"));
#endif
   }
#endif



void
Unparse_ExprStmt::unparseTemplateArgument(SgTemplateArgument* templateArgument, SgUnparse_Info& info)
   {
     ROSE_ASSERT(templateArgument != NULL);

#if 0
     printf ("In unparseTemplateArgument() = %p \n",templateArgument);
#endif

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES
     printf ("Unparse TemplateArgument (%p) \n",templateArgument);
     unp->u_exprStmt->curprint ( "\n/* Unparse TemplateArgument */ \n");
     unp->u_exprStmt->curprint ( "\n");
#endif

#if 0
     unp->u_exprStmt->curprint(string("/* templateArgument is explicitlySpecified = ") + ((templateArgument->get_explicitlySpecified() == true) ? "true" : "false") + " */");
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

  // DQ (5/14/2011): Added support for newer name qualification implementation.
  // printf ("In unparseTemplateArgument(): templateArgument->get_name_qualification_length() = %d \n",templateArgument->get_name_qualification_length());
     newInfo.set_name_qualification_length(templateArgument->get_name_qualification_length());
     newInfo.set_global_qualification_required(templateArgument->get_global_qualification_required());
     newInfo.set_type_elaboration_required(templateArgument->get_type_elaboration_required());

  // DQ (5/30/2011): Added support for name qualification.
     newInfo.set_reference_node_for_qualification(templateArgument);
     ROSE_ASSERT(newInfo.get_reference_node_for_qualification() != NULL);

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
#if 0
          printf ("In unparseTemplateArgument(): unset SkipBaseType() (how was this set? Maybe from the function reference expression?) \n");
#endif
          newInfo.unset_SkipBaseType();
        }

#if 0
     printf ("In unparseTemplateArgument(): templateArgument->get_argumentType() = %d \n",templateArgument->get_argumentType());
#endif

     switch (templateArgument->get_argumentType())
        {
          case SgTemplateArgument::type_argument:
             {
               ROSE_ASSERT (templateArgument->get_type() != NULL);

               SgType* templateArgumentType = templateArgument->get_type();
#if 0
               printf ("In unparseTemplateArgument(): templateArgument->get_type() = %s \n",templateArgumentType->class_name().c_str());
            // curprint ( "\n /* templateArgument->get_type() */ \n");
#endif
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
               ROSE_ASSERT(newInfo.get_reference_node_for_qualification() != NULL);
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
                    printf ("In unparseTemplateArgument(): Found a valid name qualification: nameQualifier %s \n",nameQualifier.str());
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

#if 0
#error "DEAD CODE!"
                 // Previous code to output the qualified name (but it happens too early).
                    curprint(nameQualifier);
#else
#if 1
            // newInfo.display("In unparseTemplateArgument(): newInfo.display()");

            // DQ (5/5/2013): Refactored code used here and in the unparseFunctionParameterDeclaration().
               unp->u_type->outputType<SgTemplateArgument>(templateArgument,templateArgumentType,newInfo);
#else
            // DQ (5/4/2013): This code was copied from the function argument processing which does handle the types properly.
            // So this code needs to be refactored.

#error "DEAD CODE!"

               printf ("REFACTOR THIS TEMPLATE ARGUMENT TYPE HANDLING! \n");

            // info.set_isTypeFirstPart();
               newInfo.set_isTypeFirstPart();
            // curprint( "\n/* unparse_helper(): output the 1st part of the type */ \n");

            // DQ (8/6/2007): Skip forcing the output of qualified names now that we have a hidden list mechanism.
            // DQ (10/14/2006): Since function can appear anywhere types referenced in function 
            // declarations have to be fully qualified.  We can't tell from the type if it requires 
            // qualification we would need the type and the function declaration (and then some 
            // analysis).  So fully qualify all function parameter types.  This is a special case
            // (documented in the Unparse_ExprStmt::unp->u_name->generateNameQualifier() member function.
            // info.set_forceQualifiedNames();

#error "DEAD CODE!"

            // SgUnparse_Info ninfo_for_type(info);
               SgUnparse_Info ninfo_for_type(newInfo);

#if 1
            // DQ (12/20/2006): This is used to specify global qualification separately from the more general name 
            // qualification mechanism.  Note that SgVariableDeclarations don't use the requiresGlobalNameQualificationOnType
            // on the SgInitializedNames in their list since the SgVariableDeclaration IR nodes is marked directly.
            // if (initializedName->get_requiresGlobalNameQualificationOnType() == true)
               if (templateArgument->get_requiresGlobalNameQualificationOnType() == true)
                  {
                 // Output the name qualification for the type in the variable declaration.
                 // But we have to do so after any modifiers are output, so in unparseType().
                 // printf ("In Unparse_ExprStmt::unparseFunctionParameterDeclaration(): This function parameter type requires a global qualifier \n");

                 // Note that general qualification of types is separated from the use of globl qualification.
                 // ninfo2.set_forceQualifiedNames();
                    ninfo_for_type.set_requiresGlobalNameQualification();
                  }
#endif

#error "DEAD CODE!"

            // DQ (5/12/2011): Added support for newer name qualification implementation.
            // ninfo_for_type.set_name_qualification_length(initializedName->get_name_qualification_length_for_type());
            // ninfo_for_type.set_global_qualification_required(initializedName->get_global_qualification_required_for_type());
            // ninfo_for_type.set_type_elaboration_required(initializedName->get_type_elaboration_required_for_type());
#if 0
               printf ("In unparseTemplateArgument(): BEFORE: templateArgument->get_name_qualification_length_for_type()     = %d \n",templateArgument->get_name_qualification_length_for_type());
               printf ("In unparseTemplateArgument(): BEFORE: templateArgument->get_global_qualification_required_for_type() = %s \n",templateArgument->get_global_qualification_required_for_type() ? "true" : "false");
               printf ("In unparseTemplateArgument(): BEFORE: templateArgument->get_type_elaboration_required_for_type()     = %s \n",templateArgument->get_type_elaboration_required_for_type() ? "true" : "false");
#endif
            // Transfer values from old variables to the newly added variables (which will be required to support the refactoring into a template of common code.
               templateArgument->set_name_qualification_length_for_type(templateArgument->get_name_qualification_length());
               templateArgument->set_global_qualification_required_for_type(templateArgument->get_global_qualification_required());
               templateArgument->set_type_elaboration_required_for_type(templateArgument->get_type_elaboration_required());

            // ninfo_for_type.set_name_qualification_length(templateArgument->get_name_qualification_length_for_type());
               ninfo_for_type.set_name_qualification_length(templateArgument->get_name_qualification_length());

#error "DEAD CODE!"

            // ninfo_for_type.set_global_qualification_required(templateArgument->get_global_qualification_required_for_type());
               ninfo_for_type.set_global_qualification_required(templateArgument->get_global_qualification_required());

            // ninfo_for_type.set_type_elaboration_required(templateArgument->get_type_elaboration_required_for_type());
               ninfo_for_type.set_type_elaboration_required(templateArgument->get_type_elaboration_required());
#if 0
               printf ("In unparseTemplateArgument(): AFTER: templateArgument->get_name_qualification_length_for_type()     = %d \n",templateArgument->get_name_qualification_length_for_type());
               printf ("In unparseTemplateArgument(): AFTER: templateArgument->get_global_qualification_required_for_type() = %s \n",templateArgument->get_global_qualification_required_for_type() ? "true" : "false");
               printf ("In unparseTemplateArgument(): AFTER: templateArgument->get_type_elaboration_required_for_type()     = %s \n",templateArgument->get_type_elaboration_required_for_type() ? "true" : "false");
#endif
            // DQ (5/29/2011): We have to set the associated reference node so that the type unparser can get the name qualification if required.
            // ninfo_for_type.set_reference_node_for_qualification(initializedName);
               ninfo_for_type.set_reference_node_for_qualification(templateArgument);

#error "DEAD CODE!"

            // unparseType(tmp_type, info);
            // unp->u_type->unparseType(tmp_type, ninfo_for_type);
               unp->u_type->unparseType(templateArgumentType, ninfo_for_type);

            // curprint( "\n/* DONE - unparse_helper(): output the 1st part of the type */ \n");

#if 0
            // DQ (5/4/2013): This would be the name of the variable of the specific type in the function 
            // parameter list, not wanted for the case of template arguments.

            // forward declarations don't necessarily need the name of the argument
            // so we must check if not NULL before adding to chars_on_line
            // This is a more consistant way to handle the NULL string case
            // curprint( "\n/* unparse_helper(): output the name of the type */ \n");
            // curprint(tmp_name.str());
               curprint( "\n/* In unparseTemplateArgument(): <<< name of type >>> */ \n");
#endif
            // output the rest of the type
            // info.set_isTypeSecondPart();
               newInfo.set_isTypeSecondPart();

            // info.display("unparse_helper(): output the 2nd part of the type");

#error "DEAD CODE!"

            // printf ("unparse_helper(): output the 2nd part of the type \n");
            // curprint( "\n/* unparse_helper(): output the 2nd part of the type */ \n");
            // unp->u_type->unparseType(tmp_type, info);
            // unp->u_type->unparseType(templateArgumentType, info);
               unp->u_type->unparseType(templateArgumentType, newInfo);

            // printf ("DONE: unparse_helper(): output the 2nd part of the type \n");
            // curprint( "\n/* DONE: unparse_helper(): output the 2nd part of the type */ \n");
#endif
#endif
                  }
                 else
                  {
                 // DQ (7/23/2011): To unparse the type directly we can't have either of these set!
                 // ROSE_ASSERT(newInfo.isTypeFirstPart()  == false);
                 // ROSE_ASSERT(newInfo.isTypeSecondPart() == false);

#if 0
                 // DQ (6/19/2013): If we are not using name qualification we at least need to unparse the whold type (both of the two parts).
                 // DQ (5/4/2013): I think we have to separate out the parts of the type so that the name qualificaion will not be output before the "const" for const types.
                    newInfo.set_isTypeFirstPart();
#if 0
                    printf ("In unparseTemplateArgument(): Calling unparseType(templateArgument->get_type(),newInfo); (first part) templateArgument->get_type() = %p = %s \n",templateArgument->get_type(),templateArgument->get_type()->class_name().c_str());
                    curprint ( "\n /* first part of type */ \n");
#endif
                    unp->u_type->unparseType(templateArgumentType,newInfo);
                 // newInfo.unset_isTypeFirstPart();
                    newInfo.set_isTypeSecondPart();
#endif
                 // This will unparse the type will any required name qualification.
#if 0
                    printf ("In unparseTemplateArgument(): Calling unparseType(templateArgument->get_type(),newInfo); (second part) templateArgument->get_type() = %p = %s \n",templateArgument->get_type(),templateArgument->get_type()->class_name().c_str());
                    curprint ( "\n /* second part of type */ \n");
#endif
                 // unp->u_type->unparseType(templateArgument->get_type(),newInfo);
                    unp->u_type->unparseType(templateArgumentType,newInfo);
#if 0
                    printf ("DONE: In unparseTemplateArgument(): Calling unparseType(templateArgument->get_type(),newInfo); \n");
                    curprint ( "\n /* end of type */ \n");
#endif
                  }

#if 0
            // DQ (5/4/2013): This is the old code.

            // DQ (7/23/2011): To unparse the type directly we can't have either of these set!
            // ROSE_ASSERT(newInfo.isTypeFirstPart()  == false);
            // ROSE_ASSERT(newInfo.isTypeSecondPart() == false);

            // This will unparse the type will any required name qualification.
#if 0
               printf ("In unparseTemplateArgument(): Calling unparseType(templateArgument->get_type(),newInfo); templateArgument->get_type() = %p = %s \n",templateArgument->get_type(),templateArgument->get_type()->class_name().c_str());
#endif
            // unp->u_type->unparseType(templateArgument->get_type(),newInfo);
               unp->u_type->unparseType(templateArgumentType,newInfo);
#if 0
               printf ("DONE: In unparseTemplateArgument(): Calling unparseType(templateArgument->get_type(),newInfo); \n");
#endif
#endif
               break;
             }

          case SgTemplateArgument::nontype_argument:
             {
            // DQ (8/12/2013): This can be either an SgExpression or SgInitializedName.
            // ROSE_ASSERT (templateArgument->get_expression() != NULL);
               ROSE_ASSERT (templateArgument->get_expression() != NULL || templateArgument->get_initializedName() != NULL);
               ROSE_ASSERT (templateArgument->get_expression() == NULL || templateArgument->get_initializedName() == NULL);
               if (templateArgument->get_expression() != NULL)
                  {
#if 0
                    printf ("In unparseTemplateArgument(): templateArgument->get_expression() = %s \n",templateArgument->get_expression()->class_name().c_str());
#endif
#if OUTPUT_DEBUGGING_INFORMATION
                    printf ("In unparseTemplateArgument(): templateArgument->get_expression() = %s \n",templateArgument->get_expression()->class_name().c_str());
                    unp->u_exprStmt->curprint ( "\n /* templateArgument->get_expression() */ \n");
#endif
                 // curprint ( "\n /* SgTemplateArgument::nontype_argument */ \n");

                 // DQ (8/7/2013): Adding support for template functions overloaded on template parameters.
                 // This should be present, but we don't use it directly in the name generation. We want 
                 // to use the template arguments in the symbol table lookup, but not the name generation.
                    ROSE_ASSERT(templateArgument->get_expression()->get_type() != NULL);
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
                    ROSE_ASSERT(type != NULL);

                 // DQ (9/10/2014): Note that this will unparse "int T" which we want in the template header, but not in the template parameter or argument list.
                 // unp->u_type->outputType<SgInitializedName>(templateArgument->get_initializedName(),type,newInfo);
                 // SgUnparse_Info ninfo(info);
                 // unp->u_type->unparseType(type,ninfo);
                    curprint(templateArgument->get_initializedName()->get_name());
                  }

            // printf ("Error: nontype_argument case not implemented in Unparse_ExprStmt::unparseTemplateArgument \n");
            // ROSE_ABORT();
               break;
             }

          case SgTemplateArgument::template_template_argument:
             {
            // unparseTemplateName(templateArgument->xxx,newInfo);
               ROSE_ASSERT(templateArgument->get_templateDeclaration() != NULL);
#if 0
               printf ("In unparseTemplateArgument(): template_template_argument: templateArgument->get_templateDeclaration()->get_template_name() = %s \n",templateArgument->get_templateDeclaration()->get_template_name().str());
#endif
            // curprint ( "\n /* SgTemplateArgument::template_template_argument */ \n");

            // DQ (8/24/2006): Skip output of the extra space.
            // unp->u_exprStmt->curprint ( templateArgument->get_templateDeclaration()->get_name().str() << " ";
#ifdef TEMPLATE_DECLARATIONS_DERIVED_FROM_NON_TEMPLATE_DECLARATIONS
               unp->u_exprStmt->curprint ( templateArgument->get_templateDeclaration()->get_template_name().str());
#else
// DQ (8/29/2012): this is OK to compile now (using the older EDG 3.3 work for backward compatability).
// #error "Older version of pre-EDG 4.x code!"
               unp->u_exprStmt->curprint ( templateArgument->get_templateDeclaration()->get_name().str());
#endif
            // printf ("Error: template_argument case not implemented in Unparse_ExprStmt::unparseTemplateArgument \n");
            // ROSE_ABORT();
               break;
             }

           case SgTemplateArgument::start_of_pack_expansion_argument:
             {
            // DQ (7/3/2013): Added initial support for varadic template arguments.
            // Using an expression for now, but we might need something else.
               ROSE_ASSERT (templateArgument->get_expression() != NULL);
#if 0
               printf ("In unparseTemplateArgument(): Template argument = %p = %s \n",templateArgument->get_expression(),templateArgument->get_expression()->class_name().c_str());
#endif
            // unp->u_exprStmt->unparseExpression(templateArgument->get_expression(),newInfo);

            // DQ (11/6/2014): C++11 test: test2014_84.C demonstrates that we don't want to output the "...".
            // DQ (7/4/2013): I am not sure if this is correct.
#if 0
               curprint("/* varadic template argument */ ...");
#else
            // DQ (2/7/2015): Supress the comment from being output.
            // curprint("/* varadic template argument */ ");
#endif
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

#if 0
     printf ("Leaving unparseTemplateArgument (%p) \n",templateArgument);
  // curprint("\n/* Bottom of unparseTemplateArgument */ \n");
#endif
#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES
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
     ROSE_ASSERT(asmOp != NULL);

  // printf ("In unparseAsmOp(): asmOp->get_recordRawAsmOperandDescriptions() = %s \n",asmOp->get_recordRawAsmOperandDescriptions() ? "true" : "false");

     SgExpression* expression = asmOp->get_expression();
     ROSE_ASSERT(expression != NULL);

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
     ROSE_ASSERT(statementExpression != NULL);
     SgStatement* statement = statementExpression->get_statement();
     ROSE_ASSERT(statement != NULL);

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
     curprint ( string("\n /* Inside of unparseBinaryOperator(expr = ") +  StringUtility::numberToString(expr) + " = " + expr->sage_class_name() + "," + op + ",SgUnparse_Info) */ \n");
#endif

#if 0
     printf ("In unparseBinaryOperator(): info.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
     printf ("In unparseBinaryOperator(): info.SkipEnumDefinition()  = %s \n",(info.SkipEnumDefinition() == true) ? "true" : "false");
#endif

  // DQ (1/9/2014): These should have been setup to be the same.
     ROSE_ASSERT(info.SkipClassDefinition() == info.SkipEnumDefinition());

     if (info.skipCompilerGeneratedSubExpressions() == true)
        {
       // Only unparse the rhs operand if it is compiler generated.
          SgBinaryOp* binaryOp = isSgBinaryOp(expr);
          ROSE_ASSERT(binaryOp != NULL);

          SgExpression* lhs = binaryOp->get_lhs_operand();
          ROSE_ASSERT(lhs != NULL);
          SgExpression* rhs = binaryOp->get_rhs_operand();
          ROSE_ASSERT(rhs != NULL);
#if 1
          printf ("In unparseBinaryOperator(): info.skipCompilerGeneratedSubExpressions() == true: only unparsing the rhs operand \n");
#endif
          if (lhs->isCompilerGenerated() == true)
             {
            // Then only unparse the rhs.
               unparseExpression(rhs, newinfo);
             }
            else
             {
               unparseBinaryExpr(expr, newinfo);
             }
        }
       else
        {
          unparseBinaryExpr(expr, newinfo);
        }

#if 0
     curprint ( string("\n /* Leaving unparseBinaryOperator(expr = ") +  StringUtility::numberToString(expr) + " = " + expr->sage_class_name() + "," + op + ",SgUnparse_Info) */ \n");
#endif
   }


void
Unparse_ExprStmt::unparseAssnExpr(SgExpression* expr, SgUnparse_Info& info) {}


void
Unparse_ExprStmt::unparseVarRef(SgExpression* expr, SgUnparse_Info& info)
   {
     SgVarRefExp* var_ref = isSgVarRefExp(expr);
     ROSE_ASSERT(var_ref != NULL);

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
     ROSE_ASSERT(var_ref->get_symbol() != NULL);

     SgInitializedName* theName = var_ref->get_symbol()->get_declaration();
     ROSE_ASSERT(theName != NULL);

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
  // ROSE_ASSERT(declarationScope != NULL);
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
     SgCompoundLiteralExp* compoundLiteral = isSgCompoundLiteralExp(expr);
     ROSE_ASSERT(compoundLiteral != NULL);

     SgVariableSymbol* variableSymbol = compoundLiteral->get_symbol();
     ROSE_ASSERT(variableSymbol != NULL);

     SgInitializedName* initializedName = variableSymbol->get_declaration();
     ROSE_ASSERT(initializedName != NULL);
     ROSE_ASSERT(initializedName->get_initptr() != NULL);

     SgAggregateInitializer* aggregateInitializer = isSgAggregateInitializer(initializedName->get_initptr());
     ROSE_ASSERT(aggregateInitializer != NULL);
     ROSE_ASSERT(aggregateInitializer->get_uses_compound_literal() == true);

     unparseAggrInit(aggregateInitializer,info);

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
     ROSE_ASSERT(classname_ref != NULL);

     curprint (  classname_ref->get_symbol()->get_declaration()->get_name().str());
   }


void
Unparse_ExprStmt::unparseFuncRef(SgExpression* expr, SgUnparse_Info& info)
   {
     SgFunctionRefExp* func_ref = isSgFunctionRefExp(expr);
     ROSE_ASSERT(func_ref != NULL);

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
     ROSE_ASSERT(func_ref != NULL);

  // DQ (4/14/2013): Added support for unparsing "operator+(x,y)" in place of "x+y".  This is 
  // required in places even though we have historically defaulted to the generation of the 
  // operator syntax (e.g. "x+y"), see test2013_100.C for an example of where this is required.
     ROSE_ASSERT(func_ref->get_parent() != NULL);
  // SgNode* possibleFunctionCall = func_ref->get_parent()->get_parent();
     SgNode* possibleFunctionCall = func_ref->get_parent();
     ROSE_ASSERT(possibleFunctionCall != NULL);
     SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(possibleFunctionCall);

  // This fails for test2005_112.C.
  // ROSE_ASSERT(functionCallExp != NULL);

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
          ROSE_ASSERT(functionSymbol != NULL);
          SgFunctionDeclaration* functionDeclaration = functionSymbol->get_declaration();
          ROSE_ASSERT(functionDeclaration != NULL);
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
       // DQ (6/23/2011): Make this a warning since the tests/CompileTests/OpenMP_tests/alignment.c fails in the tests/roseTests/ompLoweringTests directory.
       // This also happens for the tests/roseTests/programAnalysisTests/testPtr1.C when run by the tests/roseTests/programAnalysisTests/PtrAnalTest tool.

       // printf ("ERROR: In unparseType(): nodeReferenceToFunction = NULL \n");
       // printf ("WARNING: In unparseType(): nodeReferenceToFunction = NULL \n");
       // ROSE_ASSERT(false);
        }

#if DEBUG_FUNCTION_REFERENCE_SUPPORT
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
     ROSE_ASSERT(func_ref->get_symbol() != NULL);
     string func_name = func_ref->get_symbol()->get_name().str();
     int diff = 0; // the length difference between "operator" and function

#if DEBUG_FUNCTION_REFERENCE_SUPPORT
     printf ("Inside of Unparse_ExprStmt::unparseFuncRef(): func_name = %s \n",func_name.c_str());
#endif

     ROSE_ASSERT(func_ref->get_symbol() != NULL);
     ROSE_ASSERT(func_ref->get_symbol()->get_declaration() != NULL);

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

  // check that this an operator overloading function
  // if (!unp->opt.get_overload_opt() && !strncmp(func_name.c_str(), "operator", 8))
     if (!unp->opt.get_overload_opt() && !strncmp(func_name.c_str(), "operator", 8))
        {
       // set the length difference between "operator" and function
       // diff = strlen(func_name.c_str()) - strlen("operator");
          diff = (uses_operator_syntax == true) ? strlen(func_name.c_str()) - strlen("operator") : 0;
#if 0
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
          printf ("Found an operator: func_name = %s \n",func_name.c_str());
#endif
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
                    func_name = strchr(func_name.c_str(), func_name[8]);

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
          ROSE_ASSERT(declaration != NULL);
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
            // ROSE_ASSERT(templateFunctionDeclaration != NULL);
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
  // ROSE_ASSERT(binary_op != NULL);

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
  // ROSE_ASSERT(possibleFunctionCall != NULL);
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
     ROSE_ASSERT(mfunc_ref != NULL);

#if MFuncRefSupport_DEBUG
     printf ("In unparseMFuncRefSupport(): expr = %p = %s \n",expr,expr->class_name().c_str());
#endif
#if MFuncRefSupport_DEBUG
     curprint ("\n /* Inside of unparseMFuncRef " + StringUtility::numberToString(expr) + " */ \n");
#endif

  // info.display("Inside of unparseMFuncRef");

     SgMemberFunctionDeclaration* mfd  = mfunc_ref->get_symbol()->get_declaration();
     ROSE_ASSERT (mfd != NULL);

#if MFuncRefSupport_DEBUG
     printf ("mfunc_ref->get_symbol()->get_name() = %s \n",mfunc_ref->get_symbol()->get_name().str());
     printf ("mfunc_ref->get_symbol()->get_declaration()->get_name() = %s \n",mfunc_ref->get_symbol()->get_declaration()->get_name().str());
#endif

  // DQ (4/8/2013): Added support for unparsing "operator+(x,y)" in place of "x+y".  This is 
  // required in places even though we have historically defaulted to the generation of the 
  // operator syntax (e.g. "x+y"), see test2013_100.C for an example of where this is required.
     ROSE_ASSERT(mfunc_ref->get_parent() != NULL);
     SgNode* possibleFunctionCall = mfunc_ref->get_parent()->get_parent();
     ROSE_ASSERT(possibleFunctionCall != NULL);
     SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(possibleFunctionCall);
     bool uses_operator_syntax = false;
//   bool is_compiler_generated = false;
     if (functionCallExp != NULL)
        {
          uses_operator_syntax  = functionCallExp->get_uses_operator_syntax();
//        is_compiler_generated = functionCallExp->isCompilerGenerated();

#if 0
       // DQ (8/28/2014): It is a bug in GNU 4.4.7 to use the non-operator syntax of a user-defined conversion operator.
       // So we have to detect such operators and then detect if they are implicit then mark them to use the operator 
       // syntax plus supress them from being output.  We might alternatively go directly to supressing them from being
       // output, except that this is more complex for the non-operator syntax unparsing (I think).

          SgFunctionSymbol* functionSymbol = mfunc_ref->get_symbol();
          ROSE_ASSERT(functionSymbol != NULL);
          SgFunctionDeclaration* functionDeclaration = functionSymbol->get_declaration();
          ROSE_ASSERT(functionDeclaration != NULL);
          SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(functionDeclaration);
          ROSE_ASSERT(memberFunctionDeclaration != NULL);

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

     SgExpression* binary_op = isSgExpression(mfunc_ref->get_parent());
     ROSE_ASSERT(binary_op != NULL);
     bool isPartOfArrowOperatorChain = partOfArrowOperatorChain(binary_op);

#if MFuncRefSupport_DEBUG
     printf ("In unparseMFuncRefSupport(): isPartOfArrowOperatorChain                   = %s \n",isPartOfArrowOperatorChain ? "true" : "false");
     printf ("In unparseMFuncRefSupport(): uses_operator_syntax  = %s \n",uses_operator_syntax ? "true" : "false");
     printf ("In unparseMFuncRefSupport(): is_compiler_generated = %s \n",is_compiler_generated ? "true" : "false");
#endif
#if MFuncRefSupport_DEBUG
     curprint (string("\n /* Inside of unparseMFuncRef: uses_operator_syntax  = ") + (uses_operator_syntax ? "true" : "false") + " */ \n");
     curprint (string("\n /* Inside of unparseMFuncRef: is_compiler_generated = ") + (is_compiler_generated ? "true" : "false") + " */ \n");
#endif

  // DQ (11/17/2004): Interface modified, use get_class_scope() if we want a
  // SgClassDefinition, else use get_scope() if we want a SgScopeStatement.
  // SgClassDefinition*           cdef = mfd->get_scope();
     SgClassDefinition*           cdef = mfd->get_class_scope();

  // DQ (2/16/2004): error in templates (test2004_18.C)
     ROSE_ASSERT (cdef != NULL);
     SgClassDeclaration* decl;
     decl = cdef->get_declaration();

#if MFuncRefSupport_DEBUG
     printf ("In unparseMFuncRefSupport(): expr = %p (name = %s::%s) \n",expr,decl->get_name().str(),mfd->get_name().str());
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
#if 0
          printf ("rrrrrrrrrrrr In unparseMFuncRefSupport() output type generated name: nodeReferenceToFunction = %p = %s SgNode::get_globalTypeNameMap().size() = %" PRIuPTR " \n",
               nodeReferenceToFunction,nodeReferenceToFunction->class_name().c_str(),SgNode::get_globalTypeNameMap().size());
#endif
          std::map<SgNode*,std::string>::iterator i = SgNode::get_globalTypeNameMap().find(nodeReferenceToFunction);
          if (i != SgNode::get_globalTypeNameMap().end())
             {
            // I think this branch supports non-template member functions in template classes (called with explicit template arguments).
               usingGeneratedNameQualifiedFunctionNameString = true;

               functionNameString = i->second.c_str();
#if 0
               printf ("ssssssssssssssss Found type name in SgNode::get_globalTypeNameMap() typeNameString = %s for nodeReferenceToType = %p = %s \n",
                    functionNameString.c_str(),nodeReferenceToFunction,nodeReferenceToFunction->class_name().c_str());
#endif
             }
            else
             {
#if 0
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
#if 0
                    printf ("uuuuuuuuuuuuuuuuuuuu Found type name in SgNode::get_globalTypeNameMap() typeNameString = %s for nodeReferenceToType = %p = %s \n",
                         functionNameString.c_str(),mfunc_ref,mfunc_ref->class_name().c_str());
#endif
                  }
                 else
                  {
#if 0
                    printf ("Could not find saved name qualified function name in globalTypeNameMap: using key: mfunc_ref = %p = %s \n",mfunc_ref,mfunc_ref->class_name().c_str());
#endif
                  }
#endif
             }
        }
       else
        {
       // DQ (6/23/2011): Make this a warning since the tests/CompileTests/OpenMP_tests/alignment.c fails in the tests/roseTests/ompLoweringTests directory.
       // This also happens for the tests/roseTests/programAnalysisTests/testPtr1.C when run by the tests/roseTests/programAnalysisTests/PtrAnalTest tool.

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
     ROSE_ASSERT(decl != NULL);
     if (decl->get_parent() == NULL)
        {
          printf ("Error: decl->get_parent() == NULL for decl = %p = %s (name = %s::%s) \n",decl,decl->class_name().c_str(),decl->get_name().str(),mfd->get_name().str());
        }
     ROSE_ASSERT(decl->get_parent() != NULL);

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

  // DQ (4/7/2013): This code is translating "s >> len;" to "s > > len;" in test2013_97.C.
     if (mfunc_ref->get_symbol()->get_name() != "operator>>")
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
          ROSE_ASSERT(parentExpression != NULL);
       // printf ("parentExpression = %p = %s \n",parentExpression,parentExpression->sage_class_name());
          SgDotExp* dotExpression = isSgDotExp(parentExpression);
          if (dotExpression != NULL)
             {
               SgExpression* lhs = dotExpression->get_lhs_operand();
               ROSE_ASSERT(lhs != NULL);
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
               ROSE_ASSERT(mfunc_ref != NULL);
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
                         ROSE_ASSERT(declaration != NULL);
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
                         ROSE_ASSERT(declaration != NULL);
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
                         printf ("In unparseMFuncRefSupport(): is_compiler_generated = %s \n",is_compiler_generated ? "true" : "false");
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


void
Unparse_ExprStmt::unparseStringVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgStringVal* str_val = isSgStringVal(expr);
     ROSE_ASSERT(str_val != NULL);

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
     ROSE_ASSERT(str_val->get_value() != NULL);
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
     ROSE_ASSERT(str_val->get_value() != NULL);
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

            // Note added delimiters.
               s = string("\"(") + str_val->get_raw_string_value() + string(")\"");
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
          printf ("In unparseStringVal(): str_val->get_value()          = %s \n",str_val->get_value().c_str());
          printf ("In unparseStringVal(): str_val->get_value().length() = %" PRIuPTR " \n",str_val->get_value().length());
          printf ("In unparseStringVal(): output string: s              = %s \n",s.c_str());
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
     ROSE_ASSERT(uint_val != NULL);

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
     ROSE_ASSERT(longint_val != NULL);

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
     ROSE_ASSERT(longlongint_val != NULL);

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
     ROSE_ASSERT(ulonglongint_val != NULL);

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
     ROSE_ASSERT(ulongint_val != NULL);

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
     ROSE_ASSERT(float_val != NULL);

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
                    if (float_val->get_valueString() == "")
                       {
                         curprint ( tostring(float_val->get_value()) + "F");
                       }
                      else
                       {
                         curprint ( float_val->get_valueString());
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
     ROSE_ASSERT(dbl_val != NULL);

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
     ROSE_ASSERT(longdbl_val != NULL);

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
                    if (longdbl_val->get_valueString() == "")
                         curprint ( tostring(longDouble_value));
                      else
                         curprint ( longdbl_val->get_valueString());
                  }
             }
        }
   }

void
Unparse_ExprStmt::unparseComplexVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgComplexVal* complex_val = isSgComplexVal(expr);
     ROSE_ASSERT(complex_val != NULL);

     if (complex_val->get_valueString() != "") { // Has string
       curprint (complex_val->get_valueString());
     } else if (complex_val->get_real_value() == NULL) { // Pure imaginary
       curprint ("(");
       unparseValue(complex_val->get_imaginary_value(), info);
#ifdef ROSE_USE_NEW_EDG_INTERFACE 
    // curprint (" * __I__)");
       curprint (" * 1.0i)");
#else
       curprint (" * _Complex_I)");
#endif
     } else { // Complex number
       curprint ("(");
       unparseValue(complex_val->get_real_value(), info);
       curprint (" + ");
       unparseValue(complex_val->get_imaginary_value(), info);
#ifdef ROSE_USE_NEW_EDG_INTERFACE 
    // curprint (" * __I__)");
       curprint (" * 1.0i)");
#else
       curprint (" * _Complex_I)");
#endif
     }
   }

void 
Unparse_ExprStmt::unparseUpcThreads(SgExpression* expr, SgUnparse_Info& info)
   {
     SgUpcThreads* upc_threads = isSgUpcThreads(expr);
     ROSE_ASSERT(upc_threads != NULL);

     curprint ("THREADS ");
   }

void 
Unparse_ExprStmt::unparseUpcMythread(SgExpression* expr, SgUnparse_Info& info)
   {
     SgUpcMythread* upc_mythread = isSgUpcMythread(expr);
     ROSE_ASSERT(upc_mythread != NULL);

     curprint ("MYTHREAD ");
   }


void
Unparse_ExprStmt::unparseTypeTraitBuiltinOperator(SgExpression* expr, SgUnparse_Info& info)
   {
     SgTypeTraitBuiltinOperator* operatorExp = isSgTypeTraitBuiltinOperator(expr);
     ROSE_ASSERT(operatorExp != NULL);

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
          ROSE_ASSERT(newinfo.get_reference_node_for_qualification() != NULL);

          if (type != NULL)
             {
               unp->u_type->unparseType(type,newinfo);
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
#if 1
                    printf ("In unparseTypeTraitBuiltinExp(): set skipCompilerGeneratedSubExpressions flag \n");
#endif
                    ROSE_ASSERT(info2.skipCompilerGeneratedSubExpressions() == true);

                    unparseExpression(expression,info2);
#else
                 // This should be the 2nd operand.
                    ROSE_ASSERT(operand != list.begin());
#error "DEAD CODE!"
                    SgDotExp* dotExp = isSgDotExp(expression);
                    ROSE_ASSERT(dotExp != NULL);
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
#if 0
     printf ("In Unparse_ExprStmt::unparseFuncCall(): expr = %p unp->opt.get_overload_opt() = %s \n",expr,(unp->opt.get_overload_opt() == true) ? "true" : "false");
     curprint ( "\n/* In Unparse_ExprStmt::unparseFuncCall " + StringUtility::numberToString(expr) + " */ \n");
#endif

     SgFunctionCallExp* func_call = isSgFunctionCallExp(expr);
     ROSE_ASSERT(func_call != NULL);
     SgUnparse_Info newinfo(info);
     bool needSquareBrackets = false;

#if 0
     curprint       ("/* func_call->get_function()                   = " + func_call->get_function()->class_name() + " */\n");
     curprint(string("/* func_call->get_uses_operator_syntax()       = ") + ((func_call->get_uses_operator_syntax() == true) ? "true" : "false") + " */\n");
     curprint(string("/* unp->opt.get_overload_opt()                 = ") + ((unp->opt.get_overload_opt() == true) ? "true" : "false") + " */\n");
  // curprint("/* isBinaryOperator(func_call->get_function()) = " + ((unp->u_sage->isBinaryOperator(func_call->get_function()) == true) ? "true" : "false") + " */\n");
#endif

  // DQ (4/8/2013): Added support for unparsing "operator+(x,y)" in place of "x+y".  This is 
  // required in places even though we have historically defaulted to the generation of the 
  // operator syntax (e.g. "x+y"), see test2013_100.C for an example of where this is required.
     bool uses_operator_syntax = func_call->get_uses_operator_syntax();

#if 0
     printf ("In Unparse_ExprStmt::unparseFuncCall(): (before test for conversion operator) uses_operator_syntax = %s \n",uses_operator_syntax == true ? "true" : "false");
     curprint(string("/* In unparseFuncCall(): (before test for conversion operator) uses_operator_syntax     = ") + (uses_operator_syntax ? "true" : "false") + " */\n");
#endif

#if 0
  // DQ (4/8/2013): Test to make sure this is not presently required in our regression tests.
     if (uses_operator_syntax == true)
        {
          printf ("In Unparse_ExprStmt::unparseFuncCall(): Detected uses_operator_syntax == true \n");
       // ROSE_ASSERT(false);
        }
#endif

#if 0
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
               ROSE_ASSERT(functionSymbol != NULL);
               SgFunctionDeclaration* functionDeclaration = functionSymbol->get_declaration();
               ROSE_ASSERT(functionDeclaration != NULL);
            // SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(functionDeclaration);
            // ROSE_ASSERT(memberFunctionDeclaration != NULL);

               bool is_compiler_generated = func_call->isCompilerGenerated();

            // If operator form is specified then turn it off.
            // if (uses_operator_syntax == true)
                  {
                    if (functionDeclaration->get_specialFunctionModifier().isConversion() == true)
                       {
#if 0
                         printf ("In Unparse_ExprStmt::unparseFuncCall(): Detected a conversion operator! \n");
#endif
                      // DQ (8/28/2014): Force output of generated code using the operator syntax, plus supress the output if is_compiler_generated == true.
                      // uses_operator_syntax = false;

                         if (is_compiler_generated == true)
                            {
#if 0
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

#if 0
     printf ("In Unparse_ExprStmt::unparseFuncCall(): (after test for conversion operator) uses_operator_syntax = %s \n",uses_operator_syntax == true ? "true" : "false");
     curprint(string("/* In unparseFuncCall(): (after test for conversion operator) uses_operator_syntax     = ") + (uses_operator_syntax ? "true" : "false") + " */\n");
#endif

#if 0
  // DQ (11/16/2013): This need not be a SgFunctionRefExp.
     SgFunctionRefExp* func_ref = isSgFunctionRefExp(func_call->get_function());
  // ROSE_ASSERT(func_ref != NULL);
  // ROSE_ASSERT(func_ref->get_symbol() != NULL);
  // printf ("Function name = %s \n",func_ref->get_symbol()->get_name().str());
     if (func_ref != NULL)
        {
          ROSE_ASSERT(func_ref->get_symbol() != NULL);
          printf ("Function name = %s \n",func_ref->get_symbol()->get_name().str());
        }
       else
        {
       // If this is not a SgFunctionRefExp, then look for a member function reference via a SgDotExp or SgArrowExp.
        }
#endif
#if 0
     printf ("isBinaryOperator(func_call->get_function())     = %s \n",unp->u_sage->isBinaryOperator(func_call->get_function()) ? "true" : "false");
     printf ("isSgDotExp(func_call->get_function())           = %s \n",isSgDotExp(func_call->get_function()) ? "true" : "false");
     printf ("isSgArrowExp(func_call->get_function())         = %s \n",isSgArrowExp(func_call->get_function()) ? "true" : "false");

     printf ("isUnaryOperatorPlus(func_call->get_function())  = %s \n",unp->u_sage->isUnaryOperatorPlus(func_call->get_function()) ? "true" : "false");
     printf ("isUnaryOperatorMinus(func_call->get_function()) = %s \n",unp->u_sage->isUnaryOperatorMinus(func_call->get_function()) ? "true" : "false");
#endif
#if 0
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
#if 0
          printf ("Found case to investigate for generation of \"B b; b.A::operator+(b)\" instead of \"B b; b+b\" \n");
#endif
          SgBinaryOp* binaryOperator = isSgBinaryOp(func_call->get_function());
          ROSE_ASSERT(binaryOperator != NULL);

          SgExpression* lhs = binaryOperator->get_lhs_operand();
          SgExpression* rhs = binaryOperator->get_rhs_operand();
#if 0
          printf ("lhs = %p = %s \n",lhs,lhs->class_name().c_str());
          printf ("rhs = %p = %s \n",rhs,rhs->class_name().c_str());
#endif
          SgMemberFunctionRefExp* memberFunctionRef = isSgMemberFunctionRefExp(rhs);
          if (memberFunctionRef != NULL)
             {
               SgSymbol* memberFunctionSymbol = memberFunctionRef->get_symbol();
               ROSE_ASSERT(memberFunctionSymbol != NULL);
#if 0
               printf ("member function symbol = %p name = %s \n",memberFunctionRef->get_symbol(),memberFunctionRef->get_symbol()->get_name().str());
               printf ("lhs->get_type() = %s \n",lhs->get_type()->class_name().c_str());
#endif
               SgClassType* classType = isSgClassType(lhs->get_type());
               SgClassDeclaration* lhsClassDeclaration = NULL;
               SgClassDefinition*  lhsClassDefinition  = NULL;
               if (classType != NULL)
                  {
#if 0
                    printf ("classType->get_declaration() = %p = %s \n",classType->get_declaration(),classType->get_declaration()->class_name().c_str());
#endif
                    lhsClassDeclaration = isSgClassDeclaration(classType->get_declaration());
                    ROSE_ASSERT(lhsClassDeclaration != NULL);
#if 0
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
               ROSE_ASSERT(functionClassDefinition != NULL);
#if 0
               printf ("member function scope (class = %p = %s) \n",functionClassDefinition,functionClassDefinition->class_name().c_str());
#endif
#if OUTPUT_HIDDEN_LIST_DATA
               outputHiddenListData (functionClassDefinition);
#endif

               SgClassDeclaration* functionClassDeclaration = isSgClassDeclaration(functionClassDefinition->get_declaration());
#if 0
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
            // ROSE_ASSERT(lhsClassDeclaration      != NULL);
            // ROSE_ASSERT(functionClassDeclaration != NULL);

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
#if 1
                              printf ("Warning: lhs class hidding derived class member function call (skip setting uses_operator_syntax == true) \n");
#endif
#if 0
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
#if 0
            // printf ("Warning: name qualification required = %s \n",unparseOperatorSyntax ? "true" : "false");
               printf ("Warning: name qualification required = %s \n",uses_operator_syntax ? "true" : "false");
#endif
             }
            else
             {
#if 0
               printf ("rhs was not a SgMemberFunctionRefExp \n");
            // ROSE_ASSERT(false);
#endif
             }

       // printf ("Exiting as part of testing \n");
       // ROSE_ASSERT(false);
        }

#if 0
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
#if 0
          printf ("output 1st part (without syntax sugar) \n");
          curprint ( " /* output 1st part (without syntax sugar) */ ");
#endif
          ROSE_ASSERT(func_call->get_args() != NULL);
          SgExpressionPtrList& list = func_call->get_args()->get_expressions();
#if 0
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
               ROSE_ASSERT(info.get_reference_node_for_qualification() != NULL);
#if 0
               curprint ( "\n/* In unparseFuncCall(): 1st part BEFORE: unparseExpression(func_call->get_function(), info); */ \n");
#endif
               unparseExpression(func_call->get_function(), info);
#if 0
               curprint ( "\n/* In unparseFuncCall(): 1st part AFTER: unparseExpression(func_call->get_function(), info); */ \n");
#endif
               info.set_reference_node_for_qualification(NULL);

               arg++;

            // unparse the rhs operand
               unp->u_debug->printDebugInfo("right arg: ", false);
#if 0
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
#if 0
               curprint ("\n/* In unparseFuncCall(): 1st part AFTER: right arg: unparseExpression(*arg, info); */ \n");
#endif
               newinfo.unset_nested_expression();

            // printf ("DONE: output function argument (right) \n");
             }
#if 0
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
#if 0
          printf ("output 2nd part func_call->get_function() = %s \n",func_call->get_function()->class_name().c_str());
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
          ROSE_ASSERT(alt_info.get_reference_node_for_qualification() != NULL);
#if 0
          curprint ( "\n/* In unparseFuncCall(): 2nd part BEFORE: unparseExpression(func_call->get_function(), info); */ \n");
#endif
          unparseExpression(func_call->get_function(), alt_info);
#if 0
          curprint ( "\n/* In unparseFuncCall(): 2nd part AFTER: unparseExpression(func_call->get_function(), info); */ \n");
#endif
#if 0
          func_call->get_function()->get_file_info()->display("In unparse function call: debug");
#endif
          alt_info.set_reference_node_for_qualification(NULL);

#if 0
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
#if 0
          curprint ( string(" /* !unp->opt.get_overload_opt() && (uses_operator_syntax == true) = ") + ((!unp->opt.get_overload_opt() && (uses_operator_syntax == true)) ? "true" : "false") + " */ \n ");
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
                    ROSE_ASSERT(rhs != NULL);
                  }
            // if ( binary_op != NULL && rhs->get_specialFunctionModifier().isOperator() && unp->u_sage->noQualifiedName(rhs) )

#if 0
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

#if 0
          curprint (string("\n /* Before preint paren in unparseFuncCall: print_paren = ") + (print_paren ? "true" : "false") + " */ \n");
#endif
          if (print_paren)
             {
#if 0
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
#if 0
                    printf ("func_call->get_args() = %p = %s arg = %p = %s unparseArg = %s \n",func_call->get_args(),func_call->get_args()->class_name().c_str(),*arg,(*arg)->class_name().c_str(),unparseArg ? "true" : "false");
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
#if 0
                              curprint("\n/* unp->u_sage->unparseOneElemConInit in unparseFuncCall */ \n"); 
#endif
                              unparseOneElemConInit(con_init, newinfo);
                           // curprint ( "\n/* DONE: unp->u_sage->unparseOneElemConInit in unparseFuncCall */ \n"); 
                            }
                           else
                            {
#if 0
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
                  }
             }

          if (print_paren)
             {
#if 0
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
#if 0
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
#if 0
                         printf ("Unparse the lhs of the SgDotExp (as part of skipping conversion operator) \n");
                         curprint("/* Unparse the lhs of the SgDotExp (as part of skipping conversion operator) */ \n ");
#endif
                         unparseExpression(dotExp->get_lhs_operand(),newinfo);
#if 0
                         printf ("DONE: Unparse the lhs of the SgDotExp (as part of skipping conversion operator) \n");
                         curprint("/* DONE: Unparse the lhs of the SgDotExp (as part of skipping conversion operator) */ \n ");
#endif
                       }
                  }
             }
#if 0
          curprint("\n/* Leaving processing second part in unparseFuncCall */ \n");
#endif
        }

#if 0
     printf ("Leaving Unparse_ExprStmt::unparseFuncCall = %p \n",expr);
     curprint ( "\n/* Leaving Unparse_ExprStmt::unparseFuncCall " + StringUtility::numberToString(expr) + " */ \n");
#endif
   }


void Unparse_ExprStmt::unparsePointStOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, "->", info); }
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


void
Unparse_ExprStmt::unparseSizeOfOp(SgExpression* expr, SgUnparse_Info & info)
   {
     SgSizeOfOp* sizeof_op = isSgSizeOfOp(expr);
     ROSE_ASSERT(sizeof_op != NULL);

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
     if (sizeof_op->get_operand_expr() != NULL)
        {
          ROSE_ASSERT(sizeof_op->get_operand_expr() != NULL);
          unparseExpression(sizeof_op->get_operand_expr(), info);
        }
       else
        {
          ROSE_ASSERT(sizeof_op->get_operand_type() != NULL);

          SgUnparse_Info info2(info);
          info2.unset_SkipBaseType();

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
#if 0
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
#if 1
       // DQ (10/17/2012): We have to separate these out if we want to output the defining declarations.
          newinfo.set_isTypeFirstPart();
#if 0
          printf ("In unparseSizeOfOp(): isTypeFirstPart: sizeof_op->get_operand_type() = %p = %s \n",sizeof_op->get_operand_type(),sizeof_op->get_operand_type()->class_name().c_str());
          curprint ("/* In unparseSizeOfOp(): isTypeFirstPart \n */ ");
#endif
          unp->u_type->unparseType(sizeof_op->get_operand_type(), newinfo);
          newinfo.set_isTypeSecondPart();
#if 0
          printf ("In unparseSizeOfOp(): isTypeSecondPart: sizeof_op->get_operand_type() = %p = %s \n",sizeof_op->get_operand_type(),sizeof_op->get_operand_type()->class_name().c_str());
          curprint ("/* In unparseSizeOfOp(): isTypeSecondPart \n */ ");
#endif
          unp->u_type->unparseType(sizeof_op->get_operand_type(), newinfo);
#else

#error "DEAD CODE!"

       // DQ (1/14/2006): p_expression_type is no longer stored (type is computed instead)
       // unp->u_type->unparseType(cast_op->get_expression_type(), newinfo);
       // unp->u_type->unparseType(cast_op->get_type(), newinfo);
          unp->u_type->unparseType(sizeof_op->get_operand_type(), newinfo);
#endif
        }

     curprint(")");
   }


void
Unparse_ExprStmt::unparseAlignOfOp(SgExpression* expr, SgUnparse_Info & info)
   {
     SgAlignOfOp* sizeof_op = isSgAlignOfOp(expr);
     ROSE_ASSERT(sizeof_op != NULL);

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
          ROSE_ASSERT(sizeof_op->get_operand_expr() != NULL);
          unparseExpression(sizeof_op->get_operand_expr(), info);
        }
       else
        {
          ROSE_ASSERT(sizeof_op->get_operand_type() != NULL);
          SgUnparse_Info info2(info);
          info2.unset_SkipBaseType();
          info2.set_SkipClassDefinition();
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
     ROSE_ASSERT(noexcept_op != NULL);

#if 0
     printf ("In unparseNoexceptOp(expr = %p): \n",expr);
#endif

     curprint("noexcept(");

     ROSE_ASSERT(noexcept_op->get_operand_expr() != NULL);
     unparseExpression(noexcept_op->get_operand_expr(), info);

     curprint(")");
   }


void
Unparse_ExprStmt::unparseUpcLocalSizeOfOp(SgExpression* expr, SgUnparse_Info & info)
   {
     SgUpcLocalsizeofExpression* sizeof_op = isSgUpcLocalsizeofExpression(expr);
     ROSE_ASSERT(sizeof_op != NULL);

     curprint ( "upc_localsizeof(");
     if (sizeof_op->get_expression() != NULL)
        {
          ROSE_ASSERT(sizeof_op->get_expression() != NULL);
          unparseExpression(sizeof_op->get_expression(), info);
        }
#if 1
    // DQ (2/12/2011): Leave this here until I'm sure that we don't need to handle types.
       else
        {
          ROSE_ASSERT(sizeof_op->get_operand_type() != NULL);
          SgUnparse_Info info2(info);
          info2.unset_SkipBaseType();
          info2.set_SkipClassDefinition();
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
     ROSE_ASSERT(sizeof_op != NULL);

     curprint ( "upc_blocksizeof(");
     if (sizeof_op->get_expression() != NULL)
        {
          ROSE_ASSERT(sizeof_op->get_expression() != NULL);
          unparseExpression(sizeof_op->get_expression(), info);
        }
#if 1
    // DQ (2/12/2011): Leave this here until I'm sure that we don't need to handle types.
       else
        {
          ROSE_ASSERT(sizeof_op->get_operand_type() != NULL);
          SgUnparse_Info info2(info);
          info2.unset_SkipBaseType();
          info2.set_SkipClassDefinition();
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
     ROSE_ASSERT(sizeof_op != NULL);

     curprint ( "upc_elemsizeof(");
     if (sizeof_op->get_expression() != NULL)
        {
          ROSE_ASSERT(sizeof_op->get_expression() != NULL);
          unparseExpression(sizeof_op->get_expression(), info);
        }
#if 1
    // DQ (2/12/2011): Leave this here until I'm sure that we don't need to handle types.
       else
        {
          ROSE_ASSERT(sizeof_op->get_operand_type() != NULL);
          SgUnparse_Info info2(info);
          info2.unset_SkipBaseType();
          info2.set_SkipClassDefinition();
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
     ROSE_ASSERT(typeid_op != NULL);

     curprint ( "typeid(");
     if (typeid_op->get_operand_expr() != NULL)
        {
          ROSE_ASSERT(typeid_op->get_operand_expr() != NULL);
          unparseExpression(typeid_op->get_operand_expr(), info);
        }
       else
        {
          ROSE_ASSERT(typeid_op->get_operand_type() != NULL);
          SgUnparse_Info info2(info);
          info2.unset_SkipBaseType();
          info2.set_SkipClassDefinition();

       // DQ (6/2/2011): Added support for name qualification of types reference via sizeof operator.
          info2.set_reference_node_for_qualification(typeid_op);

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
     ROSE_ASSERT(expr_cond != NULL);

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
     ROSE_ASSERT(cast_op != NULL);

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
     ROSE_ASSERT(newinfo.get_reference_node_for_qualification() != NULL);

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
       // ROSE_ASSERT(cast_op != NULL);
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
                 else
                  {
#if 0
                    printf ("case SgCastExp::e_C_style_cast: compiler generated cast not output \n");
                    curprint("/* case SgCastExp::e_C_style_cast: compiler generated cast not output */");
#endif
                 // DQ (7/26/2013): This should also be true (all of the source position info should be consistant).
                    ROSE_ASSERT(cast_op->get_file_info()->isCompilerGenerated() == true);
                    ROSE_ASSERT(cast_op->get_endOfConstruct()->isCompilerGenerated() == true);
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
     ROSE_ASSERT(info.get_reference_node_for_qualification() != NULL);

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

#ifndef CXX_IS_ROSE_CODE_GENERATION
     SgNewExp* new_op = isSgNewExp(expr);
     ROSE_ASSERT(new_op != NULL);

     if (new_op->get_need_global_specifier())
        {
       // DQ (1/5/2006): I don't think that we want the extra space after the "::".
       // curprint ( ":: ";
          curprint ( "::");
        }

     curprint ("new ");

#if 0
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
     ROSE_ASSERT(newinfo.get_reference_node_for_qualification() != NULL);

#if 0
     curprint ("\n /* Output type name for new operator */ \n");
#endif

  // printf ("In Unparse_ExprStmt::unparseNewOp: new_op->get_type()->class_name() = %s \n",new_op->get_type()->class_name().c_str());

  // DQ (3/26/2012): I think this is required because the type might be the only public way refer to the 
  // class (via a public typedef to a private class, so we can't use the constructor; except for it's args)

  // DQ (3/26/2012): Turn this OFF to avoid output fo the class name twice (if the constructor is available).
  // DQ (1/17/2006): The the type specified explicitly in the new expressions syntax, 
  // get_type() has been modified to return a pointer to new_op->get_specified_type().
  // unp->u_type->unparseType(new_op->get_type(), newinfo);
     unp->u_type->unparseType(new_op->get_specified_type(), newinfo);

  // printf ("DONE: new_op->get_type()->class_name() = %s \n",new_op->get_type()->class_name().c_str());

#if 0
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
#if 0
     curprint ("\n /* Output builtin args */ \n");
#endif

     if (new_op->get_builtin_args() != NULL)
        {
       // printf ("In Unparse_ExprStmt::unparseNewOp: Now unparse new_op->get_builtin_args() \n");
          unparseExpression(new_op->get_builtin_args(), newinfo);
        }
#if 0
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
     ROSE_ASSERT(delete_op != NULL);

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

     ROSE_ASSERT(this_node != NULL);

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
     ROSE_ASSERT(scope_op != NULL);

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
void Unparse_ExprStmt::unparseRShiftAssnOp(SgExpression* expr, SgUnparse_Info& info) { unparseBinaryOperator(expr, ">>=", info); }

void Unparse_ExprStmt::unparseForDeclOp(SgExpression* expr, SgUnparse_Info& info) {}

void
Unparse_ExprStmt::unparseTypeRef(SgExpression* expr, SgUnparse_Info& info)
   {
     SgRefExp* type_ref = isSgRefExp(expr);
     ROSE_ASSERT(type_ref != NULL);

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
     ROSE_ASSERT (lnode != NULL);

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
     ROSE_ASSERT(cur_stmt != NULL);

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
                    ROSE_ASSERT(cur_file->get_file_info() != NULL);
                    ROSE_ASSERT(lnode->get_file_info() != NULL);
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
  ROSE_ASSERT (aggr_init != NULL);
  size_t m_size = (aggr_init->get_initializers()->get_expressions()).size();
  ROSE_ASSERT (n <= m_size);

  SgExpression* initializer = (aggr_init->get_initializers()->get_expressions())[n];
  ROSE_ASSERT (initializer != NULL);

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
          ROSE_ASSERT(namedType->get_declaration() != NULL);
          SgDeclarationStatement* declarationStatementDefiningType = namedType->get_declaration();
          SgDeclarationStatement* definingDeclarationStatementDefiningType = declarationStatementDefiningType->get_definingDeclaration();
          SgClassDeclaration* classDeclaration = isSgClassDeclaration(definingDeclarationStatementDefiningType);
          if (classDeclaration != NULL && classDeclaration->get_isAutonomousDeclaration() == false)
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
          ROSE_ASSERT(namedType->get_declaration() != NULL);

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
               ROSE_ASSERT ( (*i) != NULL );
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
               ROSE_ASSERT ( (*i) != NULL );
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


void
Unparse_ExprStmt::unparseAggrInit(SgExpression* expr, SgUnparse_Info& info)
   {
     SgAggregateInitializer* aggr_init = isSgAggregateInitializer(expr);
     ROSE_ASSERT(aggr_init != NULL);

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

     SgUnparse_Info newinfo(info);

#if 0
     printf ("In unparseAggrInit(): aggr_init->get_uses_compound_literal() = %s \n",aggr_init->get_uses_compound_literal() ? "true" : "false");
#endif

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
          SgUnparse_Info newinfo(info);
          if (sharesSameStatement(aggr_init,aggr_init->get_type()) == true)
             {
#if 0
               printf ("sharesSameStatement(aggr_init,aggr_init->get_type()) == true) \n");
#endif
               newinfo.unset_SkipClassDefinition();

            // DQ (1/9/2014): We have to make the handling of enum definitions consistant with that of class definitions.
               newinfo.unset_SkipEnumDefinition();
#if 0
               printf ("In unparseAggrInit(): newinfo.SkipClassDefinition() = %s \n",(newinfo.SkipClassDefinition() == true) ? "true" : "false");
               printf ("In unparseAggrInit(): newinfo.SkipEnumDefinition()  = %s \n",(newinfo.SkipEnumDefinition() == true) ? "true" : "false");
#endif
            // DQ (1/9/2014): These should have been setup to be the same.
               ROSE_ASSERT(newinfo.SkipClassDefinition() == newinfo.SkipEnumDefinition());
             }
            else
             {
#if 0
               printf ("sharesSameStatement(aggr_init,aggr_init->get_type()) == false) \n");
#endif
             }
#if 0
          newinfo.display("In unparseAggrInit(): (aggr_init->get_uses_compound_literal() == true): newinfo");
#endif
          curprint ("(");

       // DQ (9/4/2013): We need to unparse the full type (both the first and second parts of the type).
       // unp->u_type->unparseType(aggr_init->get_type(),newinfo);
          unp->u_type->outputType<SgAggregateInitializer>(aggr_init,aggr_init->get_type(),newinfo);

          curprint (")");
#if 0
          curprint ("/* DONE: output type in unparseAggrInit() */ ");
#endif
        }

  // DQ (9/29/2012): We don't want to use the explicit "{}" inside of function argument lists (see C test code: test2012_10.c).
     bool need_explicit_braces = aggr_init->get_need_explicit_braces();

#if 0
     printf ("In unparseAggrInit(): need_explicit_braces = %s \n",need_explicit_braces ? "true" : "false");
     printf ("In unparseAggrInit(): newinfo.SkipEnumDefinition() = %s \n",newinfo.SkipEnumDefinition() ? "true" : "false");
     printf ("In unparseAggrInit(): newinfo.SkipClassDefinition() = %s \n",newinfo.SkipClassDefinition() ? "true" : "false");
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

  // if (aggr_init->get_need_explicit_braces())
     if (need_explicit_braces == true)
        {
          curprint("{");
        }

     SgExpressionPtrList& list = aggr_init->get_initializers()->get_expressions();
     size_t last_index = list.size() -1;

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
#if 0
          printf ("In unparseAggrInit(): Found an include directive to be removed \n");
#endif
          removeIncludeDirective(aggr_init);
        }

     for (size_t index = 0; index < list.size(); index ++)
        {
#if 1
       // If there was an include then unparse everything (because we removed the include (above)).
       // If there was not an include then still unparse everything!
          unparseExpression(list[index], newinfo);
          if (index!= last_index)
               curprint ( ", ");
#else
       // DQ (9/11/2013): Older code that attempted to use the source position, but it is sensitive  
       // to errors in the source position information in EDG (or maybe in the translation to ROSE).
       // bool skipUnparsing = isFromAnotherFile(aggr_init,index);
          bool skipUnparsing = isFromAnotherFile(list[index]);
          if (!skipUnparsing)
             {

#error "DEAD CODE!"

               unparseExpression(list[index], newinfo);
               if (index!= last_index)
                    curprint(", ");
             }
            else
             {
#if 0
               printf ("In unparseAggrInit(): (aggr_init = %p) list[index = %" PRIuPTR "] = %p = %s is from another file so its subtree will not be output in the generated code \n",aggr_init,index,list[index],list[index]->class_name().c_str());
#endif
             }
#endif
        }
     unparseAttachedPreprocessingInfo(aggr_init, info, PreprocessingInfo::inside);

  // if (aggr_init->get_need_explicit_braces())
     if (need_explicit_braces == true)
        {
          curprint("}");
        }
   }


void
Unparse_ExprStmt::unparseCompInit(SgExpression* expr, SgUnparse_Info& info)
   {
     SgCompoundInitializer* comp_init = isSgCompoundInitializer(expr);
     ROSE_ASSERT(comp_init != NULL);

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
     string backEndCompiler = BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH;

     bool usingGxx = false;
     #ifdef USE_CMAKE
       #ifdef CMAKE_COMPILER_IS_GNUCXX
         usingGxx = true;
       #endif
     #else
       usingGxx = (backEndCompiler == "g++");
     #endif

     if (usingGxx)
        {
       // Now check the version of the identified GNU g++ compiler.
          if ((BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER == 4 && BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER >= 5) || (BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER > 4))
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


void
Unparse_ExprStmt::unparseConInit(SgExpression* expr, SgUnparse_Info& info)
   {
#if 0
     printf ("In Unparse_ExprStmt::unparseConInit expr = %p \n",expr);
     printf ("WARNING: This is redundent with the Unparse_ExprStmt::unp->u_sage->unparseOneElemConInit (This function does not handle qualified names!) \n");
#endif

     SgConstructorInitializer* con_init = isSgConstructorInitializer(expr);
     ROSE_ASSERT(con_init != NULL);

     SgUnparse_Info newinfo(info);
     bool outputParenthisis = false;

#if 0
     printf ("In unparseConInit(): con_init->get_need_name()        = %s \n",(con_init->get_need_name() == true) ? "true" : "false");
     printf ("In unparseConInit(): con_init->get_is_explicit_cast() = %s \n",(con_init->get_is_explicit_cast() == true) ? "true" : "false");
  // curprint ( string("\n /* con_init->get_need_name()        = ") + (con_init->get_need_name() ? "true" : "false") + " */ \n");
  // curprint ( string("\n /* con_init->get_is_explicit_cast() = ") + (con_init->get_is_explicit_cast() ? "true" : "false") + " */ \n");
#endif

     SgNode* nodeReferenceToType = newinfo.get_reference_node_for_qualification();
#if 0
     printf ("In unparseConInit(): nodeReferenceToType = %p \n",nodeReferenceToType);
#endif
     if (nodeReferenceToType != NULL)
        {
#if 0
          printf ("In unparseConInit(): nodeReferenceToType = %p = %s \n",nodeReferenceToType,nodeReferenceToType->class_name().c_str());
          curprint ("\n /* In unparseConInit(): nodeReferenceToType = " + nodeReferenceToType->class_name() + " */ \n");
#endif
        }
       else
        {
#if 0
          curprint ("\n /* In unparseConInit(): nodeReferenceToType = NULL */ \n");
#endif
       // DQ (6/4/2011): If it is not set then set it to the SgConstructorInitializer expression.
       // We can't enforce that it be non-null since that would have far reaching effects on the 
       // unparser implementation.
       // newinfo.set_reference_node_for_qualification(con_init);
        }

  // This should have been set from wherever it is called.
  // ROSE_ASSERT(newinfo.get_reference_node_for_qualification() != NULL);

#if 0
  // DQ (3/17/2005): Ignoring ned_name in favor of is_explicit_cast!
  // if (con_init->get_need_name() == true)
     if ((con_init->get_need_name() == true) && (con_init->get_is_explicit_cast() == true) )
        {
       // for foo(B())
          SgName nm;

#error "DEAD CODE!"

       // DQ (12/4/2003): Added assertion (one of these is required!)
       // ROSE_ASSERT (con_init->get_declaration() != NULL || con_init->get_class_decl() != NULL);

       // DQ (8/5/2005): Both are now required (under a policy of not having NULL pointers)
       // Well actually the case of a cast introduced by initialization from the return of 
       // a function does not have the information about the class declaration or the constructor, 
       // so we can't assert this. The value of the field bool p_associated_class_unknown 
       // now indicates when both pointers are NULL (else they should be valid pointers).
       // ROSE_ASSERT (con_init->get_declaration() != NULL && con_init->get_class_decl() != NULL);

#error "DEAD CODE!"

          ROSE_ASSERT ( con_init->get_associated_class_unknown() == true || 
                        con_init->get_declaration() != NULL || 
                        con_init->get_class_decl() != NULL);

       // DQ (4/27/2006): Maybe we can finally assert this!
       // ROSE_ASSERT ( con_init->get_associated_class_unknown() == true);

#error "DEAD CODE!"

       // DQ (8/5/2005): Now this logic is greatly simplified! Unforntunately not!
       // DQ (6/1/2011): It can't be this simple since con_init->get_declaration() 
       // can be NULL where in a struct there is no constructor defined.
#if 0
          printf ("con_init->get_declaration() = %p \n",con_init->get_declaration());
       // curprint ( "\n /* con_init->get_declaration() = " + string(con_init->get_declaration() ? "valid" : "null") + " pointer */ \n");
#endif
          if (con_init->get_declaration() != NULL)
             {
            // DQ (6/1/2011): Newest refactored support for name qualification.
            // nm = con_init->get_declaration()->get_qualified_name();
               SgName nameQualifier = con_init->get_qualified_name_prefix();

#error "DEAD CODE!"

#if 0
               printf ("In unparseConInit(): nameQualifier = %s \n",nameQualifier.str());
               printf ("In unparseConInit(): con_init->get_declaration()->get_name() = %s \n",con_init->get_declaration()->get_name().str());
#endif
               nm = nameQualifier + con_init->get_declaration()->get_name();
             }
            else
             {

#error "DEAD CODE!"

#if 0
               printf ("con_init->get_class_decl() = %s \n",con_init->get_class_decl() ? "true" : "false");
#endif
               if (con_init->get_class_decl() != NULL)
                  {
                 // DQ (6/1/2011): Newest refactored support for name qualification.
                 // nm = con_init->get_class_decl()->get_qualified_name();

#error "DEAD CODE!"

                    SgName nameQualifier = con_init->get_qualified_name_prefix();
#if 0
                    printf ("In Unparse_ExprStmt::unparseConInit(): con_init->get_declaration() == NULL -- nameQualifier = %s \n",nameQualifier.str());
#endif
                    nm = nameQualifier + con_init->get_class_decl()->get_name();
                  }
              // DQ (8/4/2012): We need this case to handle tests such as test2012_162.C.
              // DQ (3/29/2012): For EDG 4.x it appear we need a bit more since both con_init->get_declaration() and con_init->get_class_decl() can be NULL (see test2012_52.C).
                 else
                  {
#if 0
                    printf ("Need to handle new case for where both con_init->get_declaration() and con_init->get_class_decl() can be NULL \n");
                    printf ("Get name of type = %p = %s name = %s \n",con_init->get_type(),con_init->get_type()->class_name().c_str(),"NOT EVALUATED YET");
#endif
                    unp->u_type->unparseType(con_init->get_type(),newinfo);

#error "DEAD CODE!"

                 // ROSE_ASSERT ( nm.is_null() == false );
                  }
             }

#if 0
          printf ("In Unparse_ExprStmt::unparseConInit(): nm = %s \n",nm.str());
#endif

       // DQ (8/4/2012): Commented out this test since we output the type name using unparseType() for the case of a primative type.
       // ROSE_ASSERT ( nm.is_null() == false );

#error "DEAD CODE!"

       // printf ("In Unparse_ExprStmt::unparseConInit: info.PrintName() = %s nm = %s \n",info.PrintName() ? "true" : "false",nm.str());
       // curprint ( "\n /* Debugging In Unparse_ExprStmt::unparseConInit: nm = " + nm.str() + " */ \n";

       // purify error: nm.str() could be a NULL string
       // if (unp->u_sage->printConstructorName(con_init) && info.PrintName())
       // if ( unp->u_sage->printConstructorName(con_init) )
          if ( unp->u_sage->printConstructorName(con_init) && !nm.is_null() )
             {
            // printf ("unp->u_sage->printConstructorName(con_init) == true \n");
               curprint(nm.str());
               outputParenthisis = true;
             }
        }

#error "DEAD CODE!"

#else
  // DQ (5/26/2013): Alternative form of SgConstructorInitializer unparsing.

  // DQ (5/26/2013): Combined these predicates so that we could rewrite this to not generate a SgName and ouput it later.  
  // This is required to allow us to call the function that output the member function name (if used), so that we can 
  // under specific cases drop the output of the template argument list where it is redundant with the class.
  // if ( unp->u_sage->printConstructorName(con_init) && !nm.is_null() )
  // if ((con_init->get_need_name() == true) && (con_init->get_is_explicit_cast() == true) )
     if ((con_init->get_need_name() == true) && (con_init->get_is_explicit_cast() == true) && unp->u_sage->printConstructorName(con_init) == true)
        {
       // for foo(B())
          ROSE_ASSERT(con_init->get_associated_class_unknown() == true || con_init->get_declaration() != NULL || con_init->get_class_decl() != NULL);

       // DQ (4/27/2006): Maybe we can finally assert this!
       // ROSE_ASSERT ( con_init->get_associated_class_unknown() == true);

       // DQ (8/5/2005): Now this logic is greatly simplified! Unforntunately not!
       // DQ (6/1/2011): It can't be this simple since con_init->get_declaration() can be NULL where in a struct there is no constructor defined.
#if 0
          printf ("con_init->get_declaration() = %p \n",con_init->get_declaration());
       // curprint ( "\n /* con_init->get_declaration() = " + string(con_init->get_declaration() ? "valid" : "null") + " pointer */ \n");
#endif
          if (con_init->get_declaration() != NULL)
             {
            // DQ (6/1/2011): Newest refactored support for name qualification.
            // nm = con_init->get_declaration()->get_qualified_name();
               SgName nameQualifier = con_init->get_qualified_name_prefix();
#if 0
               printf ("In unparseConInit(): nameQualifier = %s \n",nameQualifier.str());
               printf ("In unparseConInit(): con_init->get_declaration()->get_name() = %s \n",con_init->get_declaration()->get_name().str());
#endif
#if 0
#error "DEAD CODE!"

            // DQ (5/26/2013): This is the older version of the code
               curprint(nameQualifier.str());
               curprint(con_init->get_declaration()->get_name().str());
#else
            // DQ (5/26/2013): This is the newer version of the code.
#if 0
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
               ROSE_ASSERT(declaration != NULL);
            // printf ("Inside of Unparse_ExprStmt::unparseFuncRef(): declaration = %p = %s \n",declaration,declaration->class_name().c_str());
#if 0
            // DQ (4/15/2013): If there is other debug output turned on then nesting of comments inside of comments can occur in this output (see test2007_17.C).
               printf ("In unparseConInit(): put out func_name = %s \n",func_name.str());
#endif
#if 0
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
#endif
                  }
             }
            else
             {
            // In this case there is no constructor member function to name, and so there is only the name of the class to use as a default constructor call.
#if 0
               printf ("con_init->get_class_decl() = %s \n",con_init->get_class_decl() ? "true" : "false");
#endif
               if (con_init->get_class_decl() != NULL)
                  {
                 // DQ (6/1/2011): Newest refactored support for name qualification.
                 // nm = con_init->get_class_decl()->get_qualified_name();

                    SgName nameQualifier = con_init->get_qualified_name_prefix();
#if 0
                    printf ("In Unparse_ExprStmt::unparseConInit(): con_init->get_declaration() == NULL -- nameQualifier = %s \n",nameQualifier.str());
#endif
                 // DQ (2/8/2014): I think this process of trimming the generated name is not required where there is not 
                 // associated member function and we are using the class name directly.
                    bool skipOutputOfFunctionName = false;
                 // nameQualifier = trimOutputOfFunctionNameForGNU_4_5_VersionAndLater(nameQualifier,skipOutputOfFunctionName);

                 // nm = nameQualifier + con_init->get_class_decl()->get_name();
                    curprint(nameQualifier.str());
#if 0
                    curprint(con_init->get_class_decl()->get_name().str());
#else
                 // DQ (8/19/2013): This handles the case represented by test2013_306.C.
                    newinfo.set_reference_node_for_qualification(con_init);

                 // DQ (8/19/2013): This is required to handle references to classes in the throw expression (at least), see test2004_150.C.
                 // DQ (8/19/2013): This has no effect if the type string is taken from the type name map and so the "class" needs
                 // to be eliminated in the generation of the intial string as part of the generation of the name qualification.
                 // printf ("In unparseOneElemConInit(): calling set_SkipClassSpecifier() \n");
                    newinfo.set_SkipClassSpecifier();
#if 0
                    printf ("In unparseConInit(): Unparse the type = %p = %s \n",con_init->get_type(),con_init->get_type()->class_name().c_str());
#endif
                 // unp->u_type->unparseType(con_init->get_type(),newinfo);
                    if (skipOutputOfFunctionName == false)
                       {
                         unp->u_type->unparseType(con_init->get_type(),newinfo);
                       }
#if 0
                    printf ("DONE: In unparseConInit(): unparseType() \n");
#endif
#endif
                  }
              // DQ (8/4/2012): We need this case to handle tests such as test2012_162.C.
              // DQ (3/29/2012): For EDG 4.x it appear we need a bit more since both con_init->get_declaration() and con_init->get_class_decl() can be NULL (see test2012_52.C).
                 else
                  {
#if 0
                    printf ("In unparseConInit(): Need to handle new case for where both con_init->get_declaration() and con_init->get_class_decl() can be NULL \n");
                    printf ("In unparseConInit(): Get name of type = %p = %s name = %s \n",con_init->get_type(),con_init->get_type()->class_name().c_str(),"NOT EVALUATED YET");
#endif
                    unp->u_type->unparseType(con_init->get_type(),newinfo);

                 // ROSE_ASSERT ( nm.is_null() == false );
                  }
             }

#if 0
          printf ("In Unparse_ExprStmt::unparseConInit(): nm = %s \n",nm.str());
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

            // DQ (5/26/2013): In this rewritten part of the function we no longer output the name as a SgName (it was unparsed directly as needed).
            // curprint(nm.str());
               outputParenthisis = true;
             }
        }
#endif

  // printf ("Now unparse the constructor arguments \n");
  // newinfo.display("Unparse_ExprStmt::unparseConInit");

#if 0
  // DQ (11/13/2004): Remove the parenthesis if we don't output the constructor name
  // this would only work if there was a single argument to the constructor!
     if (con_init->get_args() && unp->u_sage->printConstructorName(con_init))
        {
          curprint ( "("); 
        }
#endif

#if 0
  // DQ (4/1/2005): If we have arguments then definitly output the opening and closing parenthesis
  // if (con_init->get_args())
     if ( (con_init->get_args() != NULL) && (con_init->get_args()->get_expressions().empty() == false) )
        {

#error "DEAD CODE!"

          curprint ( "/* (reset) trigger output of ()  outputParenthisis = " + (outputParenthisis ? "true" : "false") + " */ ");
          outputParenthisis = true;
        }
#else
     if ( con_init->get_args() == NULL )
        {
          printf ("Error: con_init->get_args() == NULL \n");
          con_init->get_file_info()->display("Error: con_init->get_args() == NULL");
        }
     ROSE_ASSERT(con_init->get_args() != NULL);
     if ( con_init->get_need_parenthesis_after_name() == true )
        {
#if 0
          printf ("Output the parenthisis after the class name \n");
#endif
          outputParenthisis = true;
        }
#endif

  // DQ (4/1/2005): sometimes con_init->get_args() is NULL (as in test2005_42.C)
     if (outputParenthisis == true)
        {
          curprint("(");
        }

     if (con_init->get_args() != NULL)
        {
       // DQ (11/13/2004): Remove the parenthesis if we don't output the constructor name
       // this would only work if there was a single argument to the constructor!

       // DQ (3/17/2005): Remove the parenthesis if we don't output the constructor name
       // DQ (3/17/2005): Put the parenthesis BACK!
       // We need this to avoid: doubleArray *arrayPtr1 = (new doubleArray 42); (where it should have been "(42)")
       // if (con_init->get_is_explicit_cast() == true)
       //      curprint ( "(";
#if 0
          curprint ( "/* output args */ ");
#endif
          unparseExpression(con_init->get_args(), newinfo);

       // DQ (3/17/2005): Remove the parenthesis if we don't output the constructor name
       // if (con_init->get_is_explicit_cast() == true)
       //      curprint ( ")";
        }

     if (outputParenthisis == true)
        {
          curprint(")");
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

#if 0
  // DQ (11/13/2004): Remove the parenthesis if we don't output the constructor name
  // this would only work if there was a single argument to the constructor!
     if (con_init->get_args() && unp->u_sage->printConstructorName(con_init))
        {
          curprint ( ")"); 
        }
#endif

#if 0
     printf ("Leaving Unparse_ExprStmt::unparseConInit \n");
#endif
   }


void
Unparse_ExprStmt::unparseAssnInit(SgExpression* expr, SgUnparse_Info& info)
   {
     SgAssignInitializer* assn_init = isSgAssignInitializer(expr);
     ROSE_ASSERT(assn_init != NULL);

#if 0
     printf ("In unparseAssnInit(): assn_init->get_is_explicit_cast() = %s \n",(assn_init->get_is_explicit_cast() == true) ? "true" : "false");
     printf ("In unparseAssnInit(): assn_init->get_operand() = %p = %s \n",assn_init->get_operand(),assn_init->get_operand()->class_name().c_str());
#endif
#if 0
     curprint("/* In unparseAssnInit() */ "); 
#endif

     if (assn_init->get_is_explicit_cast() == true)
        {
          unparseExpression(assn_init->get_operand(), info);
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
Unparse_ExprStmt::unparseThrowOp(SgExpression* expr, SgUnparse_Info& info)
   {
     SgThrowOp* throw_op = isSgThrowOp(expr);
     ROSE_ASSERT(throw_op != NULL);

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

               ROSE_ASSERT(throw_op->get_operand() != NULL);
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
     ROSE_ASSERT (varArgStart != NULL);
     SgExpression* lhsOperand = varArgStart->get_lhs_operand();
     SgExpression* rhsOperand = varArgStart->get_rhs_operand();

     ROSE_ASSERT (lhsOperand != NULL);
     ROSE_ASSERT (rhsOperand != NULL);

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
     ROSE_ASSERT (varArgStart != NULL);
     SgExpression* operand = varArgStart->get_operand_expr();
     ROSE_ASSERT (operand != NULL);

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
     ROSE_ASSERT (varArg != NULL);
     SgExpression* operand = varArg->get_operand_expr();

  // DQ (1/14/2006): p_expression_type is no longer stored (type is computed instead)
  // SgType* type = varArg->get_expression_type();
     SgType* type = varArg->get_type();

     ROSE_ASSERT (operand != NULL);
     ROSE_ASSERT (type != NULL);

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
     ROSE_ASSERT (varArgEnd != NULL);
     SgExpression* operand = varArgEnd->get_operand_expr();
     ROSE_ASSERT (operand != NULL);

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

     ROSE_ASSERT (lhsOperand != NULL);
     ROSE_ASSERT (rhsOperand != NULL);

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
#if 0
     printf ("In unparseDesignatedInitializer: expr = %p \n",expr);
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

     bool isDataMemberDesignator   = (varRefExp != NULL);
     bool isArrayElementDesignator = (isSgUnsignedLongVal(designator) != NULL);

  // DQ (7/23/2013): These are relatively rare cases that we want to detect and allow.
     bool isCastDesignator         = (isSgCastExp(designator) != NULL);
     bool isAggregateInitializer   = (isSgAggregateInitializer(designator) != NULL);

     bool isAssignInitializer      = (isSgAssignInitializer(initializer) != NULL);

  // bool outputDesignatedInitializer                   = (isDataMemberDesignator == true && varRefExp->get_symbol() != NULL);
  // bool outputDesignatedInitializerAssignmentOperator = (subTreeContainsDesignatedInitializer(initializer) == false && isCastDesignator == false);
  // bool outputDesignatedInitializerAssignmentOperator = (subTreeContainsDesignatedInitializer(initializer) == false && isCastDesignator == false && isAggregateInitializer == false);
  // bool outputDesignatedInitializerAssignmentOperator = true;
  // bool outputDesignatedInitializerAssignmentOperator = (subTreeContainsDesignatedInitializer(initializer) == false);
  // bool outputDesignatedInitializerAssignmentOperator = (isArrayElementDesignator == false);
  // bool outputDesignatedInitializerAssignmentOperator = (subTreeContainsDesignatedInitializer(initializer) == false) && (isArrayElementDesignator == false);
     bool outputDesignatedInitializerAssignmentOperator = (isArrayElementDesignator == false) || (isAssignInitializer == true);

  // DQ (3/15/2015): Look for nested SgDesignatedInitializer (so we can supress the unparsed "=" syntax) (this case is demonstrated in test2015_03.c).
     bool isInitializer_AggregateInitializer   = (isSgAggregateInitializer(initializer) != NULL);
     if (isInitializer_AggregateInitializer == true)
        {
#if 0
          printf ("Found memberInit to be a SgAggregateInitializer \n");
#endif
          SgAggregateInitializer* aggregateInitializer = isSgAggregateInitializer(initializer);
          ROSE_ASSERT(aggregateInitializer != NULL);
          SgExprListExp* exprListExp = aggregateInitializer->get_initializers();
          ROSE_ASSERT(exprListExp != NULL);
          if (exprListExp->get_expressions().size() == 1)
             {
               SgExpression* tmp_expr = exprListExp->get_expressions()[0];
               ROSE_ASSERT(tmp_expr != NULL);

               SgDesignatedInitializer* designatedInitializer = isSgDesignatedInitializer(tmp_expr);
               if (designatedInitializer != NULL)
                  {
#if 0
                    printf ("Mark outputDesignatedInitializerAssignmentOperator as false since there is a nested SgDesignatedInitializer \n");
#endif
                    outputDesignatedInitializerAssignmentOperator = false;
                  }
             }
        }

#if 0
     printf ("In unparseDesignatedInitializer: designator  = %p = %s \n",designator,designator->class_name().c_str());
     printf ("In unparseDesignatedInitializer: initializer = %p = %s \n",initializer,initializer->class_name().c_str());

     printf ("In unparseDesignatedInitializer: isArrayElementDesignator = %s \n",isArrayElementDesignator ? "true" : "false");
     printf ("In unparseDesignatedInitializer: outputDesignatedInitializerAssignmentOperator = %s \n",outputDesignatedInitializerAssignmentOperator ? "true" : "false");
#endif

#if 0
     info.display("In unparseDesignatedInitializer()");
#endif

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

          bool isInUnion = (classDeclaration != NULL && classDeclaration->get_class_type() == SgClassDeclaration::e_union);
#if 0
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
#if 0
               printf ("isInFunctionCallArgument = %s \n",isInFunctionCallArgument ? "true" : "false");
#endif
               if (isInFunctionCallArgument == false)
                  {
                    isInUnion = false;
#if 0
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
#if 0
               printf ("Reset outputDesignatedInitializerAssignmentOperator = false \n");
#endif
               outputDesignatedInitializerAssignmentOperator = false;
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

#if 0
     printf ("In unparseDesignatedInitializer: outputDesignatedInitializerAssignmentOperator = %s \n",outputDesignatedInitializerAssignmentOperator ? "true" : "false");
     printf ("In unparseDesignatedInitializer: di->get_memberInit()                          = %p = %s \n",di->get_memberInit(),di->get_memberInit()->class_name().c_str());
#endif

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

#if 1
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
               curprint (" = ");

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
     ROSE_ASSERT(pdre != NULL);

     SgType *objt = pdre->get_object_type();

  // printf ("In unparsePseudoDtorRef(): pdre->get_object_type() = %p = %s \n",objt,objt->class_name().c_str());

     curprint("~");

  // if (SgNamedType *nt = isSgNamedType(objt))
     SgNamedType* namedType = isSgNamedType(objt);
     if (namedType != NULL)
        {
       // printf ("Unparser will output SgPseudoDestructorRefExp using the class name only \n");
          curprint(namedType->get_name().str());

       // DQ (3/14/2012): Note that I had to add this for the case of EDG 4.3, but it was not required previously for EDG 3.3, something in ROSE has likely changed.
          curprint("()");
        }
       else
        {
       // DQ (3/14/2012): This is the case of of a primative type (e.g. "~int"), which is allowed.
       // PC: I do not think this case will ever occur in practice.  If it does, the resulting
       // code will be invalid.  It may, however, appear in an implicit template instantiation.
       // printf ("WARNING: This case of unparsing in unparsePseudoDtorRef() using unparseType() may not work \n");
          unp->u_type->unparseType(objt, info);
        }
   }

// TV (05/06/2010): CUDA, Kernel call unparse
void Unparse_ExprStmt::unparseCudaKernelCall(SgExpression* expr, SgUnparse_Info& info) {

     SgCudaKernelCallExp* kernel_call = isSgCudaKernelCallExp(expr);
     ROSE_ASSERT(kernel_call != NULL);
     
     unparseExpression(kernel_call->get_function(), info);
     
     SgCudaKernelExecConfig * exec_config = isSgCudaKernelExecConfig(kernel_call->get_exec_config());
     ROSE_ASSERT(exec_config != NULL);

     curprint ("<<<");
     
     SgExpression * grid_exp = exec_config->get_grid();
     ROSE_ASSERT(grid_exp != NULL);
     SgConstructorInitializer * con_init = isSgConstructorInitializer(grid_exp);
     if (con_init != NULL && unp->u_sage->isOneElementList(con_init))
          unparseOneElemConInit(con_init, info);
     else
          unparseExpression(grid_exp, info);
     curprint (",");
     
     SgExpression * blocks_exp = exec_config->get_blocks();
     ROSE_ASSERT(blocks_exp != NULL);
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
