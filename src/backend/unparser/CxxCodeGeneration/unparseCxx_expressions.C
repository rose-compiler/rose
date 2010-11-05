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
#endif

     switch (expr->variant())
        {
          case UNARY_EXPRESSION:  { unparseUnaryExpr (expr, info); break; }
          case BINARY_EXPRESSION: { unparseBinaryExpr(expr, info); break; }
       // case EXPRESSION_ROOT: { unparseExprRoot(expr, info); break; }
       // case EXPR_LIST: { unparseExprList(expr, info); break; }
          case VAR_REF: { unparseVarRef(expr, info); break; }
          case CLASSNAME_REF: { unparseClassRef(expr, info); break; }
          case FUNCTION_REF: { unparseFuncRef(expr, info); break; }
          case MEMBER_FUNCTION_REF: { unparseMFuncRef(expr, info); break; }
#if 0
       // DQ (8/15/2007): These cases are handled in the base class (language independent base class).

       // DQ: These cases are separated out so that we can handle the 
       // original expression tree from any possible constant folding by EDG.
          case BOOL_VAL:
          case SHORT_VAL:
          case CHAR_VAL:
          case UNSIGNED_CHAR_VAL:
          case WCHAR_VAL:
          case STRING_VAL:
          case UNSIGNED_SHORT_VAL:
          case ENUM_VAL:
          case INT_VAL:
          case DOUBLE_VAL:
          case COMPLEX_VAL:
             {
               unparseValue(expr, info);
               break;
             }
#endif
          case UNSIGNED_INT_VAL: { unparseUIntVal(expr, info); break; }
          case LONG_INT_VAL: { unparseLongIntVal(expr, info); break; }
          case LONG_LONG_INT_VAL: { unparseLongLongIntVal(expr, info); break; }
          case UNSIGNED_LONG_LONG_INT_VAL: { unparseULongLongIntVal(expr, info); break; }
          case UNSIGNED_LONG_INT_VAL: { unparseULongIntVal(expr, info); break; }
          case FLOAT_VAL: { unparseFloatVal(expr, info); break; }
          case LONG_DOUBLE_VAL: { unparseLongDoubleVal(expr, info); break; }
          // Liao, 6/18/2008 , UPC identifiers 
          case UPC_THREADS: { unparseUpcThreads(expr, info); break; }
          case UPC_MYTHREAD: { unparseUpcMythread(expr, info); break; }
          case FUNC_CALL: { unparseFuncCall(expr, info); break; }
          case POINTST_OP: { unparsePointStOp(expr, info); break; }
          case RECORD_REF: { unparseRecRef(expr, info); break; }
          case DOTSTAR_OP: { unparseDotStarOp(expr, info); break; }
          case ARROWSTAR_OP: { unparseArrowStarOp(expr, info); break; }
          case EQ_OP: { unparseEqOp(expr, info); break; }
          case LT_OP: { unparseLtOp(expr, info); break; }
          case GT_OP: { unparseGtOp(expr, info); break; }
          case NE_OP: { unparseNeOp(expr, info); break; }
          case LE_OP: { unparseLeOp(expr, info); break; }
          case GE_OP: { unparseGeOp(expr, info); break; }
          case ADD_OP: { unparseAddOp(expr, info); break; }
          case SUBT_OP: { unparseSubtOp(expr, info); break; }
          case MULT_OP: { unparseMultOp(expr, info); break; }
          case DIV_OP: { unparseDivOp(expr, info); break; }
          case INTEGER_DIV_OP: { unparseIntDivOp(expr, info); break; }
          case MOD_OP: { unparseModOp(expr, info); break; }
          case AND_OP: { unparseAndOp(expr, info); break; }
          case OR_OP: { unparseOrOp(expr, info); break; }
          case BITXOR_OP: { unparseBitXOrOp(expr, info); break; }
          case BITAND_OP: { unparseBitAndOp(expr, info); break; }
          case BITOR_OP: { unparseBitOrOp(expr, info); break; }
          case COMMA_OP: { unparseCommaOp(expr, info); break; }
          case LSHIFT_OP: { unparseLShiftOp(expr, info); break; }
          case RSHIFT_OP: { unparseRShiftOp(expr, info); break; }
          case UNARY_MINUS_OP: { unparseUnaryMinusOp(expr, info); break; }
          case UNARY_ADD_OP: { unparseUnaryAddOp(expr, info); break; }
          case SIZEOF_OP: { unparseSizeOfOp(expr, info); break; }
          case TYPEID_OP: { unparseTypeIdOp(expr, info); break; }
          case NOT_OP: { unparseNotOp(expr, info); break; }
          case DEREF_OP: { unparseDerefOp(expr, info); break; }
          case ADDRESS_OP: { unparseAddrOp(expr, info); break; }
          case MINUSMINUS_OP: { unparseMinusMinusOp(expr, info); break; }
          case PLUSPLUS_OP: { unparsePlusPlusOp(expr, info); break; }
          case BIT_COMPLEMENT_OP: { unparseBitCompOp(expr, info); break; }
          case REAL_PART_OP: { unparseRealPartOp(expr, info); break; }
          case IMAG_PART_OP: { unparseImagPartOp(expr, info); break; }
          case CONJUGATE_OP: { unparseConjugateOp(expr, info); break; }
          case EXPR_CONDITIONAL: { unparseExprCond(expr, info); break; }
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

          default:
             {
            // printf ("Default reached in switch statement for unparsing expressions! expr = %p = %s \n",expr,expr->class_name().c_str());
               printf ("Default reached in switch statement for unparsing expressions! expr = %p = %s \n",expr,expr->class_name().c_str());
               ROSE_ASSERT(false);
               break;
             }
        }
   }


// DQ (2/16/2005): This function has been moved to this file from unparse_type.C
void
Unparse_ExprStmt::unparseTemplateName(SgTemplateInstantiationDecl* templateInstantiationDeclaration, SgUnparse_Info& info)
   {
     ROSE_ASSERT (templateInstantiationDeclaration != NULL);

     unp->u_exprStmt->curprint ( templateInstantiationDeclaration->get_templateName().str());
     const SgTemplateArgumentPtrList& templateArgListPtr = templateInstantiationDeclaration->get_templateArguments();
     if (!templateArgListPtr.empty())
        {
       // printf ("templateArgListPtr->size() = %zu \n",templateArgListPtr->size());

       // DQ (4/18/2005): We would like to avoid output of "<>" if possible so verify that there are template arguments
          ROSE_ASSERT(templateArgListPtr.size() > 0);

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
   }

void
Unparse_ExprStmt::unparseTemplateParameter(SgTemplateParameter* templateParameter, SgUnparse_Info& info)
   {
     ROSE_ASSERT(templateParameter != NULL);
     printf ("unparseTemplateParameter(): Sorry, not implemented! \n");
     ROSE_ASSERT(false);
   }


#if 0
// DQ (6/29/2005): It does not appear that this function is used.
void
Unparse_ExprStmt::unparseTemplateArguments(SgTemplateArgumentPtrListPtr templateArgListPtr, SgUnparse_Info& info)
   {
  // printf ("templateArgListPtr->size() = %zu \n",templateArgListPtr->size());
     unp->u_exprStmt->curprint ( "< ");
     SgTemplateArgumentPtrList::const_iterator i = templateArgListPtr->begin();
     while (i != templateArgListPtr->end())
        {
       // printf ("templateArgList element *i = %s \n",(*i)->sage_class_name());
          unparseTemplateArgument(*i,info);
          i++;
          if (i != templateArgListPtr->end())
               unp->u_exprStmt->curprint ( " , ");
        }
     unp->u_exprStmt->curprint ( " > ");
   }
#endif

void
Unparse_ExprStmt::unparseTemplateArgument(SgTemplateArgument* templateArgument, SgUnparse_Info& info)
   {
      ROSE_ASSERT(templateArgument != NULL);

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

  // ROSE_ASSERT(newInfo.isTypeFirstPart() == false);
  // ROSE_ASSERT(newInfo.isTypeSecondPart() == false);

     switch (templateArgument->get_argumentType())
        {
          case SgTemplateArgument::type_argument:
             {
               ROSE_ASSERT (templateArgument->get_type() != NULL);
#if OUTPUT_DEBUGGING_INFORMATION
               printf ("In unparseTemplateArgument(): templateArgument->get_type() = %s \n",templateArgument->get_type()->sage_class_name());
               unp->u_exprStmt->curprint ( "\n /* templateArgument->get_type() */ \n");
#endif

            // DQ (11/27/2004): Set these (though I am not sure that they help!)
            // newInfo.unset_isTypeFirstPart();
            // newInfo.unset_isTypeSecondPart();

               newInfo.set_SkipClassDefinition();
               newInfo.set_SkipClassSpecifier();

               unp->u_type->unparseType(templateArgument->get_type(),newInfo);
               break;
             }

          case SgTemplateArgument::nontype_argument:
             {
               ROSE_ASSERT (templateArgument->get_expression() != NULL);
#if OUTPUT_DEBUGGING_INFORMATION
               printf ("In unparseTemplateArgument(): templateArgument->get_expression() = %s \n",templateArgument->get_expression()->sage_class_name());
               unp->u_exprStmt->curprint ( "\n /* templateArgument->get_expression() */ \n");
#endif

            // DQ (1/5/2007): test2007_01.C demonstrated where this expression argument requires qualification.
            // printf ("Template argument = %p = %s \n",templateArgument->get_expression(),templateArgument->get_expression()->class_name().c_str());

               unp->u_exprStmt->unparseExpression(templateArgument->get_expression(),newInfo);
            // printf ("Error: nontype_argument case not implemented in Unparse_ExprStmt::unparseTemplateArgument \n");
            // ROSE_ABORT();
               break;
             }

          case SgTemplateArgument::template_template_argument:
             {
            // unparseTemplateName(templateArgument->xxx,newInfo);
               ROSE_ASSERT(templateArgument->get_templateDeclaration() != NULL);

            // DQ (8/24/2006): Skip output of the extra space.
            // unp->u_exprStmt->curprint ( templateArgument->get_templateDeclaration()->get_name().str() << " ";
               unp->u_exprStmt->curprint ( templateArgument->get_templateDeclaration()->get_name().str());

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

#if 0
// DQ (8/13/2007): Moved to common (language independent) base class

std::string resBool(bool val) {
   return val ? "True" : "False" ;
}

/**********************************************************
 *  Convert to string
 *********************************************************/
  template<typename T>
  static  std::string tostring(T t){
    std::ostringstream myStream; //creates an ostringstream object
    myStream << t << std::flush;
    return myStream.str(); //returns the string form of the stringstream object
  }

void Unparse_ExprStmt::curprint (std::string str) {
  unp->u_sage->curprint(str);
}
#endif

#if 0
// DQ (8/13/2007): Moved to common (language independent) base class

//-----------------------------------------------------------------------------------
//  void Unparse_ExprStmt::unparseExpression
//  
//  General unparse function for expressions. Then it routes to the appropriate 
//  function to unparse each kind of expression. Type and symbols still use the 
//  original unparse function because they don't have file_info and therefore, 
//  will not print out file information
//-----------------------------------------------------------------------------------
void Unparse_ExprStmt::unparseExpression(SgExpression* expr, SgUnparse_Info& info)
   {
  // directives(expr);

  // DQ (3/21/2004): This assertion should have been in place before now!
     ROSE_ASSERT (expr != NULL);

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES
  // DQ (8/21/2005): Suppress comments when unparsing to build type names
     if ( !info.SkipComments() || !info.SkipCPPDirectives() )
        {
          ROSE_ASSERT(expr->get_startOfConstruct() != NULL);
          ROSE_ASSERT(expr->get_file_info() != NULL);
          printf ("Unparse expression (%p): sage_class_name() = %s compiler-generated = %s \n",expr,expr->class_name().c_str(),expr->get_file_info()->isCompilerGenerated() ? "true" : "false");
          char buffer[100];
          snprintf (buffer,100,"%p",expr);
          curprint ( "\n/* Top of unparseExpression " << expr->class_name() 
              + " at: " << buffer 
              + " compiler-generated (file_info) = " << (expr->get_file_info()->isCompilerGenerated() ? "true" : "false")
              + " compiler-generated (startOfConstruct) = " << (expr->get_startOfConstruct()->isCompilerGenerated() ? "true" : "false") << " */ \n");
        }
#endif

     ROSE_ASSERT(expr != NULL);
     ROSE_ASSERT(expr->get_startOfConstruct() != NULL);
     ROSE_ASSERT(expr->get_file_info() != NULL);
     if (expr->get_file_info()->isCompilerGenerated() != expr->get_startOfConstruct()->isCompilerGenerated())
        {
          printf ("In unparseExpression(%s): Detected error expr->get_file_info()->isCompilerGenerated() != expr->get_startOfConstruct()->isCompilerGenerated() \n",expr->class_name().c_str());
          printf ("     expr->get_file_info() = %p expr->get_operatorPosition() = %p expr->get_startOfConstruct() = %p \n",expr->get_file_info(),expr->get_operatorPosition(),expr->get_startOfConstruct());
          ROSE_ASSERT(expr->get_file_info()->get_parent() != NULL);
          printf ("parent of file info = %p = %s \n",expr->get_file_info()->get_parent(),expr->get_file_info()->get_parent()->class_name().c_str());
          expr->get_file_info()->display("expr->get_file_info(): debug");
          expr->get_startOfConstruct()->display("expr->get_startOfConstruct(): debug");
        }
     ROSE_ASSERT(expr->get_file_info()->isCompilerGenerated() == expr->get_startOfConstruct()->isCompilerGenerated());

#if 0
     printf ("In unparseExpression(%p = %s) \n",expr,expr->class_name().c_str());
     expr->get_file_info()->display("unparseExpression (debug)");
#endif

  // DQ (12/5/2006): Let's ignore the case of a transformation for now!
     if (expr->get_endOfConstruct() == NULL && expr->get_file_info()->isTransformation() == false)
        {
          printf ("Error in unparseExpression(): expr = %p = %s expr->get_endOfConstruct() == NULL \n",expr,expr->class_name().c_str());
          expr->get_file_info()->display("unparseExpression (debug)");
        }
  // ROSE_ASSERT(expr->get_endOfConstruct() != NULL);

#if 0
  // DQ (10/25/2006): Debugging support for file info data for each IR node
     curprint ( "\n/* Top of unparseExpression " + string(expr->sage_class_name()) + " */\n ");
     ROSE_ASSERT (expr->get_startOfConstruct() != NULL);
     curprint ( "/* startOfConstruct: file = " << expr->get_startOfConstruct()->get_filenameString() 
         + " raw filename = " << expr->get_startOfConstruct()->get_raw_filename() 
         + " raw line = " << expr->get_startOfConstruct()->get_raw_line() 
         + " raw column = " << expr->get_startOfConstruct()->get_raw_col() 
         + " */\n ");
     if (expr->get_endOfConstruct() != NULL)
        {
          curprint ( "/* endOfConstruct: file = " << expr->get_endOfConstruct()->get_filenameString()
              + " raw filename = " << expr->get_endOfConstruct()->get_raw_filename() 
              + " raw line = " << expr->get_endOfConstruct()->get_raw_line() 
              + " raw column = " << expr->get_endOfConstruct()->get_raw_col() 
              + " */\n ");
        }
#endif

  // DQ (10/25/2006): Debugging support for file info data for each IR node
#define OUTPUT_EMBEDDED_COLOR_CODES_FOR_EXPRESSIONS 0
#if OUTPUT_EMBEDDED_COLOR_CODES_FOR_EXPRESSIONS
     vector< pair<bool,std::string> > stateVector;
     if (get_embedColorCodesInGeneratedCode() > 0)
        {
          setupColorCodes ( stateVector );
          printColorCodes ( expr, true, stateVector );
        }
#endif

  // MS 2003: experimental backend source replacement
  // Either use the source string attached to the AST by a transformation
  // (and do not traverse the subtree with 'epxr' as its root node)
  // OR unparse the expression (the whole subtree)
  // if (expr->attribute.exists("_UnparserSourceReplacement"))
  // if (expr->get_attribute() != NULL && expr->attribute().exists("_UnparserSourceReplacement"))
     if (expr->attributeExists("_UnparserSourceReplacement") == true)
        {
       // string rep=(expr->attribute["_UnparserSourceReplacement"])->toString();
       // string rep=(expr->attribute()["_UnparserSourceReplacement"])->toString();
          string rep = expr->getAttribute("_UnparserSourceReplacement")->toString();
          cout << "UNPARSER: SOURCE REPLACEMENT:" << rep << endl;
          curprint ( rep);
        }
       else
        {
       // DQ (5/21/2004): revised need_paren handling in EDG/SAGE III and within SAGE III IR)
       // QY (7/9/2004): revised to use the new unp->u_sage->PrintStartParen test
          bool printParen = unp->u_sage->PrintStartParen(expr,info);
#if 0
       // DQ (8/21/2005): Suppress comments when unparsing to build type names
          if ( !info.SkipComments() || !info.SkipCPPDirectives() )
             {
               curprint ( "\n /* In unparseExpression paren " + expr->sage_class_name() +
				  " paren printParen = " + (printParen ? "true" : "false") + " */ \n");
             }
#endif
          if (printParen)
             {
            // Make sure this is not an expresion list
               ROSE_ASSERT (isSgExprListExp(expr) == NULL);

            // Output the left paren
               curprint ( "(");
             }

       // DQ (10/7/2004): Definitions should never be unparsed within code generation for expressions
          if (info.SkipClassDefinition() == false)
             {
            // printf ("Skip output of class definition in unparseExpression \n");
            // DQ (10/8/2004): Skip all definitions when outputing expressions!
            // info.set_SkipClassDefinition();
            // info.set_SkipDefinition();
             }

       // DQ (10/13/2006): Remove output of qualified names from this leve of generality!
       // DQ (12/22/2005): Output any name qualification that is required 
       // (we only explicitly store the global scope qualification since 
       // this is all that it seems that EDG stores).
       // unparseQualifiedNameList(expr->get_qualifiedNameList());

          switch (expr->variant())
             {
               case UNARY_EXPRESSION:  { unparseUnaryExpr (expr, info); break; }
               case BINARY_EXPRESSION: { unparseBinaryExpr(expr, info); break; }
               case EXPRESSION_ROOT: { unparseExprRoot(expr, info); break; }
               case EXPR_LIST: { unparseExprList(expr, info); break; }
               case VAR_REF: { unparseVarRef(expr, info); break; }
               case CLASSNAME_REF: { unparseClassRef(expr, info); break; }
               case FUNCTION_REF: { unparseFuncRef(expr, info); break; }
               case MEMBER_FUNCTION_REF: { unparseMFuncRef(expr, info); break; }

            // DQ: These cases are separated out so that we can handle the 
            // original expression tree from any possible constant folding by EDG.
               case BOOL_VAL:
               case SHORT_VAL:
               case CHAR_VAL:
               case UNSIGNED_CHAR_VAL:
               case WCHAR_VAL:
               case STRING_VAL:
               case UNSIGNED_SHORT_VAL:
               case ENUM_VAL:
               case INT_VAL:
               case UNSIGNED_INT_VAL:
               case LONG_INT_VAL:
               case LONG_LONG_INT_VAL:
               case UNSIGNED_LONG_LONG_INT_VAL:
               case UNSIGNED_LONG_INT_VAL:
               case FLOAT_VAL:
               case DOUBLE_VAL:
               case LONG_DOUBLE_VAL:
               case COMPLEX_VAL:
                  {
                    unparseValue(expr, info);
                    break;
                  }

               case FUNC_CALL: { unparseFuncCall(expr, info); break; }
               case POINTST_OP: { unparsePointStOp(expr, info); break; }
               case RECORD_REF: { unparseRecRef(expr, info); break; }
               case DOTSTAR_OP: { unparseDotStarOp(expr, info); break; }
               case ARROWSTAR_OP: { unparseArrowStarOp(expr, info); break; }
               case EQ_OP: { unparseEqOp(expr, info); break; }
               case LT_OP: { unparseLtOp(expr, info); break; }
               case GT_OP: { unparseGtOp(expr, info); break; }
               case NE_OP: { unparseNeOp(expr, info); break; }
               case LE_OP: { unparseLeOp(expr, info); break; }
               case GE_OP: { unparseGeOp(expr, info); break; }
               case ADD_OP: { unparseAddOp(expr, info); break; }
               case SUBT_OP: { unparseSubtOp(expr, info); break; }
               case MULT_OP: { unparseMultOp(expr, info); break; }
               case DIV_OP: { unparseDivOp(expr, info); break; }
               case INTEGER_DIV_OP: { unparseIntDivOp(expr, info); break; }
               case MOD_OP: { unparseModOp(expr, info); break; }
               case AND_OP: { unparseAndOp(expr, info); break; }
               case OR_OP: { unparseOrOp(expr, info); break; }
               case BITXOR_OP: { unparseBitXOrOp(expr, info); break; }
               case BITAND_OP: { unparseBitAndOp(expr, info); break; }
               case BITOR_OP: { unparseBitOrOp(expr, info); break; }
               case COMMA_OP: { unparseCommaOp(expr, info); break; }
               case LSHIFT_OP: { unparseLShiftOp(expr, info); break; }
               case RSHIFT_OP: { unparseRShiftOp(expr, info); break; }
               case UNARY_MINUS_OP: { unparseUnaryMinusOp(expr, info); break; }
               case UNARY_ADD_OP: { unparseUnaryAddOp(expr, info); break; }
               case SIZEOF_OP: { unparseSizeOfOp(expr, info); break; }
               case TYPEID_OP: { unparseTypeIdOp(expr, info); break; }
               case NOT_OP: { unparseNotOp(expr, info); break; }
               case DEREF_OP: { unparseDerefOp(expr, info); break; }
               case ADDRESS_OP: { unparseAddrOp(expr, info); break; }
               case MINUSMINUS_OP: { unparseMinusMinusOp(expr, info); break; }
               case PLUSPLUS_OP: { unparsePlusPlusOp(expr, info); break; }
               case BIT_COMPLEMENT_OP: { unparseBitCompOp(expr, info); break; }
               case EXPR_CONDITIONAL: { unparseExprCond(expr, info); break; }
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

               default:
                  {
                 // printf ("Default reached in switch statement for unparsing expressions! \n");
                    printf ("Default reached in switch statement for unparsing expressions! expr = %p = %s \n",expr,expr->class_name().c_str());
                    ROSE_ASSERT(false);
                    break;
                  }
      
             }

          if (printParen)
             {
            // Output the right paren
               curprint ( ")");
             }

       // calls the logical_unparse function in the sage files
       // expr->logical_unparse(info, curprint);
        } // unparse source replacement END

#if OUTPUT_EMBEDDED_COLOR_CODES_FOR_EXPRESSIONS
     if (get_embedColorCodesInGeneratedCode() > 0)
        {
          printColorCodes ( expr, false, stateVector );
        }
#endif

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES
  // DQ (8/21/2005): Suppress comments when unparsing to build type names
     if ( !info.SkipComments() || !info.SkipCPPDirectives() )
        {
          printf ("Leaving unparse expression (%p): sage_class_name() = %s \n",expr,expr->sage_class_name());
          unp->u->sage->curprint ( "\n/* Bottom of unparseExpression " << string(expr->sage_class_name()) << " */ \n");
        }
#endif
   }
#endif

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
          printf ("asmOp->get_constraintString() = %s \n",asmOp->get_constraintString().c_str());
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

     curprint ( "(");
     unparseStatement(statement,info2);
     curprint ( ")");
   }

#if 0
// DQ (8/13/2007): Moved to common (language independent) base class

void
Unparse_ExprStmt::unparseNullExpression (SgExpression* expr, SgUnparse_Info& info)
   {
  // Nothing to do here! (unless we need a ";" or something)
   }
#endif


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
     curprint ( "\n /* Inside of unparseUnaryOperator(" + expr->sage_class_name() + "," + op <+ ",SgUnparse_Info) */ \n");
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
     curprint ( string("\n /* Inside of unparseBinaryOperator(") + expr->sage_class_name() + "," + op + ",SgUnparse_Info) */ \n");
#endif
     unparseBinaryExpr(expr, newinfo);
   }

#if 0
// DQ (8/13/2007): Moved to common (language independent) base class

void
Unparse_ExprStmt::unparseUnaryExpr(SgExpression* expr, SgUnparse_Info& info) 
   {
#if 0
     curprint ( "\n /* Inside of unparseUnaryExpr */ \n");
     unp->u_debug->printDebugInfo("entering unparseUnaryExpr", true);
#endif

     SgUnaryOp* unary_op = isSgUnaryOp(expr);
     ROSE_ASSERT(unary_op != NULL);

  // int toplevel_expression = !info.get_nested_expression();

     info.set_nested_expression();

  //
  // Flag to indicate whether the operand contains an overloaded arrow operator
  //
     bool arrow_op = false;
     arrow_op = unp->u_sage->NoDereference(expr);

#if 0
     printf ("In Unparse_ExprStmt::unparseUnaryExpr: unary_op->get_parent() = %s pointer \n",
          (unary_op->get_parent() != NULL) ? "VALID" : "NULL");
  // printf ("In Unparse_ExprStmt::unparseUnaryExpr: toplevel_expression = %d arrow_op = %d \n",toplevel_expression,arrow_op);
#endif

  // We have to test to see if the operand associated with this unary expression is a function pointer 
  // then we can use either function pointer calling syntax
  // (for void (*functionPointer) (int) as a declaration):
  //      1) functionPointer (x);
  //      2) (*functionPointer) (x);
  // Either is valid syntax (see Stroustrup (2nd Edition) section 7.7 "Pointer to Function" page 156.)
  // It seems that EDG and SAGE represent the two similarly, so we have to choose which format we 
  // want to have in the unparsing. Likely this should be an option to the unparser.

  // bool isFunctionType = (isSgFunctionType(unary_op->get_type()) != NULL) ? true : false;

#if 0
     printf ("unary_op->get_mode() != SgUnaryOp::postfix is %s \n",(unary_op->get_mode() != SgUnaryOp::postfix) ? "true" : "false");
     printf ("In Unparse_ExprStmt::unparseUnaryExpr: arrow_op = %d \n",arrow_op);
     printf ("isFunctionType = %s \n",(isFunctionType == true) ? "true" : "false");

     printf ("unary_op->get_operand()->sage_class_name() = %s \n",unary_op->get_operand()->sage_class_name());
     printf ("unary_op->get_type()->sage_class_name() = %s \n",unary_op->get_type()->sage_class_name());
     printf ("info.get_operator_name() = %s \n",info.get_operator_name());
#endif

  // DQ (2/22/2005): Ignoring if this is a SgFunctionType (test ...)
  // Bugfix (2/26/2001) If this is for a function pointer then skip printing out 
  // the operator name (for dereferencing operator)
#if 1
  // if (unary_op->get_mode() != SgUnaryOp::postfix && !arrow_op)
     if (unary_op->get_mode() != SgUnaryOp::postfix)
#else
     if (unary_op->get_mode() != SgUnaryOp::postfix && !arrow_op && !isFunctionType)
#endif
        {
       // curprint ( "\n /* Unparsing a prefix unary operator */ \n";
       // DQ (2/25/2005): Trap case of SgPointerDerefExp so that "*" can't be 
       // turned into "/*" if preceeded by a SgDividOp or overloaded "operator/()"
       // Put in an extra space so that if this happens we only generate "/ *"
       // test2005_09.C demonstrates this bug!
          if (isSgPointerDerefExp(expr) != NULL)
               curprint (  " ");
          curprint ( info.get_operator_name());
        }

     unparseExpression(unary_op->get_operand(), info);

     if (unary_op->get_mode() == SgUnaryOp::postfix && !arrow_op) 
        { 
          curprint (  info.get_operator_name()); 
        }

     info.unset_nested_expression();

#if 0
     curprint ( "\n /* Leaving of unparseUnaryExpr */ \n");
#endif
   }
#endif


#if 0
// DQ (8/13/2007): Moved to common (language independent) base class

void
Unparse_ExprStmt::unparseBinaryExpr(SgExpression* expr, SgUnparse_Info& info) 
   {
#if 0
      curprint ( string("\n /* Inside of unparseBinaryExpr (operator name = ") + info.get_operator_name() + " */ \n");
      printf ("In unparseBinaryExpr() expr = %s \n",expr->class_name().c_str());
#endif

     unp->u_debug->printDebugInfo("entering unparseBinaryExpr", true);
     SgBinaryOp* binary_op = isSgBinaryOp(expr);
     ROSE_ASSERT(binary_op != NULL);

#if 0
  // printf ("In Unparse_ExprStmt::unparseBinaryExpr() expr = %s \n",expr->sage_class_name());
      curprint ( string ("\n /* Inside of unparseBinaryExpr (expr class name = ") + binary_op->class_name() + " */ \n");
      curprint ( string("\n /*                              lhs class name  = ") + binary_op->get_lhs_operand()->class_name() + " */ \n");
      curprint ( string("\n /*                              rhs class name  = ") + binary_op->get_rhs_operand()->class_name() + " */ \n");
#endif

  // int toplevel_expression = !info.get_nested_expression();
     bool iostream_op = false;

  // Same reasoning above except with parenthesis operator function.
     bool paren_op = false;

  // Same reasoning above except with "this" expression.
     bool this_op = false;

  // Flag to indicate whether the rhs operand is an overloaded arrow operator
  // (to control the printing of parenthesis).
     bool arrow_op = false;

  // Flag to indicate whether the lhs operand contains an overloaded arrow operator
  // to control printing of operator.
     bool overload_arrow = false;

     if (!unp->opt.get_overload_opt() && unp->u_sage->isIOStreamOperator(binary_op->get_rhs_operand()) )
          iostream_op = true;

     if (!unp->opt.get_overload_opt() && unp->u_sage->isBinaryParenOperator(binary_op->get_rhs_operand()) )
          paren_op = true;

     if (!unp->opt.get_this_opt() && isSgThisExp(binary_op->get_lhs_operand()) )
          this_op = true;

     if (!unp->opt.get_overload_opt() && unp->u_sage->isOverloadedArrowOperator(binary_op->get_rhs_operand()) )
          arrow_op = true;

     info.set_nested_expression();

  // [DTdbug] 3/23/2000 -- Trying to figure out why overloaded square bracket
  //          operators are not being handled correctly.
  //
  //          3/30/2000 -- operator[]s have been handled.  See unparseFuncCall().
  //

#if 0
     printf ("In unparseBinaryExp(): unp->opt.get_overload_opt() = %s \n",(unp->opt.get_overload_opt() == true) ? "true" : "false");
     curprint ( string("\n /* unp->opt.get_overload_opt() = ") + ((unp->opt.get_overload_opt() == true) ? "true" : "false") + " */ \n");
     curprint ( string("\n /* arrow_op = ") + resBool(arrow_op) + " */ \n");
     curprint ( string("\n /* this_op = ") + resBool(this_op) + " */ \n");
#endif

     if (strcmp(info.get_operator_name(),"[]") == 0 )
        {
       // Special case:
       // curprint ( "/* Special case of operator[] found */\n";
          unp->u_debug->printDebugInfo("we have special case: []", true);
          unp->u_debug->printDebugInfo("lhs: ", false);
          unparseExpression(binary_op->get_lhs_operand(), info);
          curprint (  "[") ; 
          unp->u_debug->printDebugInfo("rhs: ", false);
          unparseExpression(binary_op->get_rhs_operand(), info);
          curprint (  "]"); 
        }
       else 
        {
#if 0
          curprint ( "/* NOT a special case of operator[] */\n");
          curprint ( string("/* unp->opt.get_overload_opt() = ") + (unp->opt.get_overload_opt() == true ? "true" : "false") + " */\n ");
          unp->opt.display("unparseBinaryExpr()");
#endif

       // Check whether overload option is turned on or off.  If it is off, the conditional is true.
       // Meaning that overloaded operators such as "A.operator+(B)" are output as "A + B".
          if (!unp->opt.get_overload_opt())
             {
            // printf ("overload option is turned off! (output as "A+B" instead of "A.operator+(B)") \n");
            // First check if the right hand side is an unary operator function.
#if 0
               curprint ( string("\n /* output as A+B instead of A.operator+(B): (u_sage->isUnaryOperator(binary_op->get_rhs_operand())) = ") + 
                    ((unp->u_sage->isUnaryOperator(binary_op->get_rhs_operand())) ? "true" : "false") + " */ \n");
#endif
               if (unp->u_sage->isUnaryOperator(binary_op->get_rhs_operand()))
                  {
                 // printf ("Found case of rhs being a unary operator! \n");

                 // Two cases must be considered here: prefix unary and postfix unary 
                 // operators. Most of the unary operators are prefix. In this case, we must
                 // first unparse the rhs and then the lhs.	
                 // if (isUnaryPostfixOperator(binary_op->get_rhs_operand())); // Postfix unary operator.
                    if (unp->u_sage->isUnaryPostfixOperator(binary_op->get_rhs_operand()))  // Postfix unary operator.
                       {
                      // ... nothing to do here (output the operator later!) ???
                      // printf ("... nothing to do here (output the postfix operator later!) \n");
                       }
                      else 
                       {
                      // Prefix unary operator.
                      // printf ("Handle prefix operator ... \n");
                         unp->u_debug->printDebugInfo("prefix unary operator found", true);
                         unp->u_debug->printDebugInfo("rhs: ", false);

                      // printf ("Prefix unary operator: Output the RHS operand ... = %s \n",binary_op->get_rhs_operand()->sage_class_name());
                      // curprint ( "\n /* Prefix unary operator: Output the RHS operand ... */ \n";
                         unparseExpression(binary_op->get_rhs_operand(), info);

                         unp->u_debug->printDebugInfo("lhs: ", false);

                      // DQ (2/22/2005): Treat the operator->() the same as an SgArrowExp IR node
                      // DQ (2/19/2005): When converting to overloaded operator syntax (e.g. "++" instead of "operator++()")
                      // detect the case of pointer dereferencing (from the SgArrowOp) via "x->operator++()" and convert 
                      // to "++(*x)" instead of "*++x".
                      // if ( isSgArrowExp(expr) )

                         bool outputParen = false;
                         SgExpression* lhs = binary_op->get_lhs_operand();
                         ROSE_ASSERT(lhs != NULL);
                         SgConstructorInitializer* constructor = isSgConstructorInitializer(lhs);
                         if (constructor != NULL)
                            {
                              outputParen = true;
                            }

                         if ( outputParen == true )
                            {
                           // curprint ( " /* output paren for constructor intializer */ ";
                              curprint ( "(");
                            }

                         if ( isSgArrowExp(expr) || arrow_op == true )
                            {
                           // printf ("This is an isSgArrowExp operator so dereference the lhs when converting to the dot syntax: opening \n");
                           // curprint ( " /* opening paren for prefix fixup */ ";
                              curprint ( "(*");
                            }

                      // printf ("Prefix unary operator: Output the LHS operand ... = %s \n",binary_op->get_lhs_operand()->sage_class_name());
                      // curprint ( "\n /* Prefix unary operator: Output the LHS operand ... */ \n";
                      // unparseExpression(binary_op->get_lhs_operand(), info);
                         unparseExpression(lhs, info);
                         info.unset_nested_expression();

                      // DQ (2/22/2005): Treat the operator->() the same as an SgArrowExp IR node
                      // if ( isSgArrowExp(expr) )
                         if ( isSgArrowExp(expr) || arrow_op == true )
                            {
                           // printf ("This is an isSgArrowExp operator so dereference the lhs when converting to the dot syntax: closing \n");
                           // curprint ( " /* closing paren for prefix fixup */ ";
                              curprint ( " )");
                            }

                         if ( outputParen == true )
                            {
                              curprint ( ")");
                            }
                      // printf ("Calling \"return\" from this binary operator ... \n");
                      // curprint ( "\n /* Calling \"return\" from this binary operator ... */ \n";
                         return;
                       }
                  }
             }

       // Check if this is a dot expression and the overload option is turned off. If so,
       // we need to handle this differently. Otherwise, skip this section and unparse
       // using the default case below.
       // if (!unp->opt.get_overload_opt() && strcmp(info.get_operator_name(),".")==0) 
          if (!unp->opt.get_overload_opt() && isSgDotExp(expr) != NULL )
             {
#if 0
               printf ("overload option is turned off and this is a SgDotExp! \n");
               if ( !info.SkipComments() || !info.SkipCPPDirectives() )
                  {
                    curprint ( "\n /* ( !unp->opt.get_overload_opt() && isSgDotExp(expr) ) == true */ \n");
                  }
#endif
               overload_arrow = unp->u_sage->NoDereference(binary_op->get_lhs_operand());

            // curprint ( "\n /* overload_arrow = " + overload_arrow + " */ \n";

               SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(binary_op->get_rhs_operand());

            // curprint ( "\n /* mfunc_ref = " + StringUtility::numberToString(mfunc_ref) + " */ \n";

               unp->u_debug->printDebugInfo("lhs: ", false);

               bool addParensForLhs = false;
               SgExpression* lhs = binary_op->get_lhs_operand();
               ROSE_ASSERT(lhs != NULL);
            // if (isSgBinaryOp(lhs) != NULL || isSgConstructorInitializer(lhs) != NULL )
               SgConstructorInitializer* constructor = isSgConstructorInitializer(lhs);

            // printf ("############## constructor = %p \n",constructor);
               ROSE_ASSERT( (constructor == NULL) || (constructor != NULL && constructor->get_args() != NULL) );

            // Make sure that lhs of SgDotExp include "()" as in: "length = (pos-fpos).magnitude();"
               if (constructor != NULL)
                  {
                 // printf ("constructor->get_args()->get_expressions().size() = %zu \n",constructor->get_args()->get_expressions().size());
                    if (constructor->get_args()->get_expressions().size() > 0)
                       {
                         addParensForLhs = true;
                       }
                  }

            // Make sure that lhs of SgDotExp include "()" as in: "length = (pos-fpos).magnitude();"
               if (mfunc_ref != NULL)
                  {
                 // addParensForLhs = true;
                  }

               if (addParensForLhs == true)
                  {
                 // DQ (2/22/2005): Removed special case from Unparse_ExprStmt::unp->u_sage->PrintStartParen(SgExpression* expr, SgUnparse_Info& info)
                 // curprint ( "/* added paren in binary operator */ ( ";
                    curprint ( "(");
                  }

            // unparseExpression(binary_op->get_lhs_operand(), info);
               unparseExpression(lhs, info);
               unp->u_debug->printDebugInfo(getSgVariant(expr->variant()), true);

               if (addParensForLhs == true)
                  {
                 // curprint ( "/* closing paren */ ) ";
                    curprint ( ")");
                  }

            // Check if the rhs is not a member function. If so, then it is most likely a  
            // data member of a class. We print the dot in this case.
               if (!mfunc_ref && !overload_arrow)  
                  {
                 // Print out the dot:
                 // curprint ( "\n /* Print out the dot */ \n";
                    curprint ( info.get_operator_name());
                    unp->u_debug->printDebugInfo("printed dot because is not member function", true);
                  }
                 else
                  {
                 // Now check if this member function is an operator overloading function. If it
                 // is, then we don't print the dot. If not, then print the dot.
#if 0
                    curprint ( "\n /* Print out the dot: Now check if this member function is an operator overloading function */ \n");
                    if (mfunc_ref != NULL)
                         curprint ( string("\n /* isOperator(mfunc_ref) = ") + ((unp->u_sage->isOperator(mfunc_ref) == true) ? "true" : "false") + " */ \n");
#endif
                 // DQ (12/11/2004): Added assertion to catch case of "a.operator->();"
                 // ROSE_ASSERT(mfunc_ref != NULL);
                 // if (!isOperator(mfunc_ref) && !overload_arrow)
//                  if ( (mfunc_ref != NULL) && !isOperator(mfunc_ref) && !overload_arrow)
//                  if ( !isOperator(mfunc_ref) && !overload_arrow)
//                  if ( overload_arrow == true )
//                  if ( true )
//                  if ( !isOperator(mfunc_ref) )
//                  if ( (mfunc_ref != NULL) && !isOperator(mfunc_ref) )
                    if ( (mfunc_ref == NULL) || !unp->u_sage->isOperator(mfunc_ref) )
                       {
                      // curprint ( "\n /* Print out the dot in second case */ \n";
                         curprint ( info.get_operator_name());
                         unp->u_debug->printDebugInfo("printed dot because is not operator overloading function", true);
                       }
                  }
             }
         // else if (!unp->opt.get_this_opt() && strcmp(info.get_operator_name(),"->")==0) 
            else
             {
               printf ("overload option is turned on! \n");
            // Check if this is an arrow expression and the "this" option is turned off. If 
            // so, we need to handle this differently. Otherwise, skip this section and 
            // unparse using the default case below.

            // curprint ( "\n /* ( !unp->opt.get_overload_opt() && isSgDotExp(expr) ) == false */ \n";
#if 0
               curprint ( string("\n /* ( !unp->opt.get_overload_opt() && isSgArrowExp(expr) ) = ") + 
                    ((!unp->opt.get_this_opt() && isSgArrowExp(expr)) ? "true" : "false") + " */ \n");
#endif
               if ( !unp->opt.get_this_opt() && isSgArrowExp(expr) ) 
                  {

                 // This is a special case to check for. We are now checking for arrow 
                 // expressions with the overload operator option off. If so, that means that 
                 // we cannot print "operator", which means that printing "->" is wrong. So we 
                 // must dereference the variable and suppress the printing of "->". Jump to
                 // "dereference" is true.
                    if (!unp->opt.get_overload_opt() && unp->u_sage->isOperator(binary_op->get_rhs_operand()))
                       goto dereference;

                    unp->u_debug->printDebugInfo("lhs: ", false);
                    unparseExpression(binary_op->get_lhs_operand(), info);
                    unp->u_debug->printDebugInfo(getSgVariant(expr->variant()), true);

                 // Check if the lhs is a this expression. If so, then don't print the arrow. 
                    if (!isSgThisExp(binary_op->get_lhs_operand())) 
                       {
                      // Is not "this" exp, so print the arrow.
                         curprint ( info.get_operator_name());
                       }
                  }
              // else if (!unp->opt.get_overload_opt() && strcmp(info.get_operator_name(),"->")==0 && isOperator(binary_op->get_rhs_operand())) 
                 else 
                  {
                 // We must also check the special case here mentioned above since the "this" 
                 // option may be true, but the overload option is false.
                    if (!unp->opt.get_overload_opt() && isSgArrowExp(expr) && unp->u_sage->isOperator(binary_op->get_rhs_operand())) 
                       {
                         dereference:

                      // Before dereferencing, first check if the function is preceded by a class
                      // name. If not, then dereference.

                         if (unp->u_sage->noQualifiedName(binary_op->get_rhs_operand())) 
                            {
                              curprint ( "(*"); 
                              unp->u_debug->printDebugInfo("lhs: ", false);
                              unparseExpression(binary_op->get_lhs_operand(), info);
                              unp->u_debug->printDebugInfo(getSgVariant(expr->variant()), true);
                              curprint ( ")");
                            }
                       }
                      else 
                       {
                      // We reach this if the options were on, or if this was not a dot, arrow, or unary
                      // prefix expression. This is the default case.

                         unp->u_debug->printDebugInfo("lhs: ", false);
                         curprint ( "/* lhs = " + binary_op->get_lhs_operand()->class_name() + " */\n ");
                         unparseExpression(binary_op->get_lhs_operand(), info);
                         curprint ( "/* DONE: lhs = " + binary_op->get_lhs_operand()->class_name() + " */\n ");
                         unp->u_debug->printDebugInfo(getSgVariant(expr->variant()), true);

                      // Before checking to insert a newline to prevent linewrapping, check that this
                      // expression is a primitive operator and not dot or arrow expressions.
                         curprint ( string(" ") + info.get_operator_name() + " ");
                       }
                  }
             }

          SgExpression* rhs = binary_op->get_rhs_operand();
          if (strcmp(info.get_operator_name(),",")==0) 
             {
               SgUnparse_Info newinfo(info);
               newinfo.set_inRhsExpr();
               unp->u_debug->printDebugInfo("rhs: ", false);
            // unparseExpression(binary_op->get_rhs_operand(), newinfo);
               unparseExpression(rhs, newinfo);
             }
            else
             {
            // unparseExpression(binary_op->get_rhs_operand(), info);
               unparseExpression(rhs, info);
             }
        }

     info.unset_nested_expression();

#if 0
     curprint ( "\n /* Leaving unparseBinaryExpr */ \n");
#endif
   }
#endif

void
Unparse_ExprStmt::unparseAssnExpr(SgExpression* expr, SgUnparse_Info& info) {}

#if 0
// DQ (8/13/2007): This is not longer used in the ROSE AST.

// DQ(8/4/2006): Suggested by Markus's student (Christoph Bonitz)
// SgExpressionRoot support is not used in the unparser, but code built 
// using SgExpressionRoot should be able to be unparsed the same as any 
// other IR node.
void
Unparse_ExprStmt::unparseExprRoot(SgExpression* expr, SgUnparse_Info& info)
   {
     SgUnaryOp* oper = dynamic_cast<SgUnaryOp*>(expr);
     ROSE_ASSERT(oper != NULL);
     SgExpression* exp = oper->get_operand();
     ROSE_ASSERT(exp != NULL);
     unparseExpression(exp,info);
   }
#endif

#if 0
// This is placed into the base class!

void
Unparse_ExprStmt::unparseExprList(SgExpression* expr, SgUnparse_Info& info)
   {
     SgExprListExp* expr_list = isSgExprListExp(expr);
     ROSE_ASSERT(expr_list != NULL);
  /* code inserted from specification */
  
     SgExpressionPtrList::iterator i = expr_list->get_expressions().begin();

     if (i != expr_list->get_expressions().end())
        {
          while (true)
             {
               SgUnparse_Info newinfo(info);
               newinfo.set_SkipBaseType();
               unparseExpression(*i, newinfo);
               i++;
               if (i != expr_list->get_expressions().end())
                  {
                    curprint ( ",");
                  }
                 else
                  {
                    break;
                  }
             }
        }
   }
#endif

void
Unparse_ExprStmt::unparseVarRef(SgExpression* expr, SgUnparse_Info& info)
   {
     SgVarRefExp* var_ref = isSgVarRefExp(expr);
     ROSE_ASSERT(var_ref != NULL);

#if 0
     printf ("In Unparse_ExprStmt::unparseVarRef() \n");
     var_ref->get_startOfConstruct()->display("In Unparse_ExprStmt::unparseVarRef()");
#endif

  /* code inserted from specification */

  // todo: when get_parent() works for this class we can
  // get back to the lhs of the SgArrowExp or SgDotExp that
  // may be a parent of this expression.  This will let
  // us avoid outputting the class qualifier when its not needed.

  // For now we always output the class qualifier.

     if (var_ref->get_symbol() == NULL)
        {
          printf ("Error in unparseVarRef() at line %d column %d \n",
               var_ref->get_file_info()->get_line(),
               var_ref->get_file_info()->get_col());
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
          nameQualifier = unp->u_name->generateNameQualifier(theName,info);
        }

#if 0
     if (nameQualifier.is_null() == false)
        {
          curprint ( nameQualifier.str());
        }
#else
  // DQ (11/9/2007): Need to ignore these sorts of generated names
     if (nameQualifier.getString().find("__unnamed_class") == string::npos)
        {
       // printf ("In Unparse_ExprStmt::unparseVarRef(): nameQualifier = %s \n",nameQualifier.str());
          curprint ( nameQualifier.str());
        }
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
          curprint (var_ref->get_symbol()->get_name().str());
        }

// DQ (1/7/2007): This is now OLD CODE!
#if 0

#error "DEAD CODE!"

#if 1
  // DQ (1/7/2007): We need to use the previous code and get the scope information from the SgInitializedName instead of from the SgVariableDeclaration!
     printf ("FIXME: We need to use this version of code and get the scope information from the SgInitializedName instead of from the SgVariableDeclaration! \n");

     SgInitializedName* prev_decl = theName->get_prev_decl_item();
     printf ("prev_decl = %p \n",prev_decl);
     SgVariableDeclaration* vd = NULL;
     if (prev_decl != NULL)
        {
          vd = isSgVariableDeclaration(prev_decl->get_declaration());
          ROSE_ASSERT(vd != NULL);
        }
       else
        {
          vd = isSgVariableDeclaration(theName->get_declaration());
        }
#else
  // DQ (6/21/2006): Use the parent instead of the get_declaration() function
     vd = isSgVariableDeclaration(theName->get_declaration());
#endif
  // ROSE_ASSERT(theName->get_parent() != NULL);
  // vd = isSgVariableDeclaration(theName->get_parent());

  // DQ (1/7/2007): I would like to assume this!
  // DQ (6/21/2006): I think we should assume this 
  // ROSE_ASSERT(vd != NULL);

  // if (vd && vd->isStatic())
     printf ("In Unparse_ExprStmt::unparseVarRef: vd = %p \n",vd);
  // printf ("In Unparse_ExprStmt::unparseVarRef: vd->get_declarationModifier().get_storageModifier().isStatic() = %s \n",
  //      (vd->get_declarationModifier().get_storageModifier().isStatic() == true) ? "true" : "false");

  // DQ (10/16/2004): Not clear on why this was previously dependent upon isStatic modifier!
  // if ( (vd != NULL) && (vd->get_declarationModifier().get_storageModifier().isStatic() == true) )
     if (vd != NULL)
        {
          printf ("vd = %p = %s  vd->get_parent() = %p = %s \n",vd,vd->class_name().c_str(),vd->get_parent(),vd->get_parent()->class_name().c_str());

          SgClassDefinition* cdef = isSgClassDefinition(vd->get_parent());
          printf ("isSgClassDefinition: cdef = %p \n",cdef);

          if (cdef != NULL)
             {
               SgClassDeclaration* cdecl = isSgClassDeclaration(cdef->get_declaration());

            // DQ (1/5/2007): Removed requirement of the declaration being static! (as a test)
            // However, only use name qualification if the variable is not used with a field 
            // access operator (SgDotExp or SgArrowExp).
            // printf ("Removed requirement of the declaration being static! (as a test) \n");
            // if (cdecl != NULL &&  vd->get_declarationModifier().get_storageModifier().isStatic())
            // if (cdecl != NULL)
            // if (cdecl != NULL && (isSgDotExp(var_ref->get_parent()) == NULL && isSgArrowExp(var_ref->get_parent()) == NULL) )

               printf ("In unparseVarRef(): var_ref->get_parent() = %p = %s \n",var_ref->get_parent(),var_ref->get_parent()->class_name().c_str());

            // DQ (1/7/2007): We don't want to use qualified names for references to fields, but we do require them for variables.
               SgBinaryOp* binaryOperator = isSgBinaryOp(var_ref->get_parent());
               SgDotExp*   dotExp         = isSgDotExp(binaryOperator);
               SgArrowExp* arrowExp       = isSgArrowExp(binaryOperator);

               bool outputQualifiedName = cdecl != NULL && (dotExp == NULL && arrowExp == NULL);
               printf ("After initialization: outputQualifiedName = %s \n",outputQualifiedName ? "true" : "false");
            // Detect the use on the lhs of SgDotExp or SgArrowExp binary operator
               outputQualifiedName |= (dotExp != NULL || arrowExp != NULL) && (binaryOperator->get_lhs_operand() == var_ref);
               printf ("After being updated: outputQualifiedName = %s \n",outputQualifiedName ? "true" : "false");

               if ( outputQualifiedName == true )
                  {
                 // curprint ( "\n /* In unparseVarRef class declaration qualified name generation " + cdecl->get_name().str() + " */ \n";
                    curprint (  cdecl->get_qualified_name().str() +  "::");
                  }
             }

          SgNamespaceDefinitionStatement* namespacedef = isSgNamespaceDefinitionStatement(vd->get_parent());
       // printf ("namespacedef = %p \n",namespacedef);
          if (namespacedef != NULL)
             {
               SgNamespaceDeclarationStatement* namespacedecl = isSgNamespaceDeclarationStatement(namespacedef->get_namespaceDeclaration());
               if (namespacedecl != NULL)
                  {
                 // DQ (8/24/2006): Added support for unnamed namespaces (see boost_tests/test_boost_lambda.C)
                 // A better fix was to correct this in the SgNamespaceDeclarationStatement::get_qualified_name()
                 // member function directly.
#if 0
                 // curprint ( "\n /* In unparseVarRef namespace declaration qualified name generation " + namespacedecl->get_name().str() + " */ \n";
                    if (namespacedecl->get_name().is_null() == true)
                       {
                      // curprint ( "\n /* In unparseVarRef namespace declaration qualified name generation (for empty namespace) */ \n";
                      // printf ("Found an empty namespace declaration: where is this! \n");
                      // namespacedecl->get_file_info()->display("Found an empty namespace declaration: where is this!");
                         curprint (  namespacedecl->get_qualified_name().str());
                       }
                      else
                       {
                      // curprint ( "\n /* In unparseVarRef namespace declaration qualified name generation (for valid namespace) */ \n";
                         curprint (  namespacedecl->get_qualified_name().str() +  "::");
                       }
#else
                         curprint (  namespacedecl->get_qualified_name().str() +  "::");
#endif
                  }
             }
        }
#if 0
       else
        {
       // This is not a variable declaration so we need to find the associated relavant declaration and (importantly) the SgInitializedName within the declaration, so that we can identify it's scope.
          SgInitializedName* prev_decl = theName->get_prev_decl_item();
          SgVariableDeclaration* vd = NULL;
          if (prev_decl != NULL)
               vd = isSgVariableDeclaration(prev_decl->get_declaration());
          if (vd != NULL)
             {

             }
            else
             {
               printf ("Error: This should have been a variable declaration! \n");
               ROSE_ASSERT(false);
             }
        }
#endif

  // curprint ( "\n /* In unparseVarRef now output the name " + var_ref->get_symbol()->get_name().str() + " */ \n";
     curprint (  var_ref->get_symbol()->get_name().str());
  // curprint (  var_ref->get_symbol()->get_name().str() +  " ";
#endif

#if 0
     printf ("Leaving Unparse_ExprStmt::unparseVarRef() \n");
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

  // DQ: This acceses the string pointed to by the pointer in the SgName object 
  // directly ans is thus UNSAFE! A copy of the string should be made.
  // char* func_name = func_ref->get_symbol()->get_name();
  // char* func_name = strdup (func_ref->get_symbol()->get_name().str());
     ROSE_ASSERT(func_ref->get_symbol() != NULL);
     string func_name = func_ref->get_symbol()->get_name().str();
     int diff = 0; // the length difference between "operator" and function

  // printf ("Inside of Unparse_ExprStmt::unparseFuncRef(): func_name = %s \n",func_name.c_str());

     ROSE_ASSERT(func_ref->get_symbol() != NULL);
     ROSE_ASSERT(func_ref->get_symbol()->get_declaration() != NULL);
     SgDeclarationStatement*         declaration = func_ref->get_symbol()->get_declaration();

  // check that this an operator overloading function
  // if (!unp->opt.get_overload_opt() && !strncmp(func_name.c_str(), "operator", 8))
     if (!unp->opt.get_overload_opt() && !strncmp(func_name.c_str(), "operator", 8))
        {
       // set the length difference between "operator" and function
          diff = strlen(func_name.c_str()) - strlen("operator");

       // DQ (1/6/2006): trap out cases of global new and delete functions called 
       // using ("::operator new" or "::operator delete" syntax).  In these cases 
       // the function are treated as normal function calls and not classified in 
       // the AST as SgNewExp and SgDeleteExp.  See test2006_04.C.
          bool isNewOperator    =  (strncmp(func_name.c_str(), "operator new", 12) == 0)    ? true : false;
          bool isDeleteOperator =  (strncmp(func_name.c_str(), "operator delete", 15) == 0) ? true : false;
#if 0
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
               func_name = strchr(func_name.c_str(), func_name[8]);
             }
        }

  // if func_name is not "()", print it. Otherwise, we don't print it because we want
  // to print out, for example, A(0) = 5, not A()(0) = 5.
     if (strcmp(func_name.c_str(), "()"))
        {
       // DQ (10/21/2006): Only do name qualification of function names for C++
          if (SageInterface::is_Cxx_language() == true)
             {
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

                    SgName nameQualifier = unp->u_name->generateNameQualifier( declaration, info );
                 // printf ("In unparseFuncRef(): nameQualifier = %s \n",nameQualifier.str());
                 // curprint ( "\n /* unparseFuncRef using nameQualifier = " + nameQualifier.str() + " */ \n";
#if 0
                    SgFunctionCallExp* functionCallExpression = isSgFunctionCallExp(expr->get_parent());
                    if (functionCallExpression != NULL)
                       {
                         printf ("Found the function call, global qualification is defined here functionCallExpression->get_global_qualified_name() = %s \n",
                              functionCallExpression->get_global_qualified_name() == true ? "true" : "false");
                         if (functionCallExpression->get_global_qualified_name() == true)
                            {
                              curprint ( "::");
                            }
                       }
#endif
                    curprint ( nameQualifier.str());
                 // curprint ( nameQualifier.str() + " ";
                  }
                 else
                  {
                 // printf ("In unparseFuncRef(): No name qualification permitted in this case! \n");
                  }
             }

       // DQ (12/2/2004): Put a little extra space after the function name (avoids i !=0)
          curprint (  func_name);
       // curprint (  func_name + " ";
        }

  // printDebugInfo("unparseFuncRef, Function Name: ", false); printDebugInfo(func_name.c_str(), true);
   }

void
Unparse_ExprStmt::unparseMFuncRef ( SgExpression* expr, SgUnparse_Info& info )
   {
	   // CH (4/7/2010): This issue is because of using a MSVC keyword 'cdecl' as a variable name

//#ifndef _MSCx_VER
//#pragma message ("WARNING: Commented out body of unparseMFuncRef()")
//	   printf ("Error: Commented out body of unparseMFuncRef() \n");
//	   ROSE_ASSERT(false);
//#else
     SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(expr);
     ROSE_ASSERT(mfunc_ref != NULL);

  // info.display("Inside of unparseMFuncRef");

  // SgMemberFunctionDeclaration* mfd  = mfunc_ref->get_symbol_i()->get_declaration();
     SgMemberFunctionDeclaration* mfd  = mfunc_ref->get_symbol()->get_declaration();

  // printf ("mfunc_ref->get_symbol()->get_name() = %s \n",mfunc_ref->get_symbol()->get_name().str());
  // printf ("mfunc_ref->get_symbol()->get_declaration()->get_name() = %s \n",mfunc_ref->get_symbol()->get_declaration()->get_name().str());

  // DQ (11/17/2004): Interface modified, use get_class_scope() if we want a
  // SgClassDefinition, else use get_scope() if we want a SgScopeStatement.
  // SgClassDefinition*           cdef = mfd->get_scope();
     SgClassDefinition*           cdef = mfd->get_class_scope();

  // DQ (2/16/2004): error in templates (test2004_18.C)
     ROSE_ASSERT (cdef != NULL);
     SgClassDeclaration* decl;
	 decl = cdef->get_declaration();
#if 0
     printf ("Inside of unparseMFuncRef expr = %p (name = %s::%s) \n",expr,cdecl->get_name().str(),mfd->get_name().str());
     curprint ( "\n /* Inside of unparseMFuncRef */ \n");
#endif
#if 0
     mfd->get_functionModifier().display("In unparseMFuncRef: functionModifier");
     mfd->get_specialFunctionModifier().display("In unparseMFuncRef: specialFunctionModifier");
#endif

  // qualified name is always outputed except when the p_need_qualifier is
  // set to 0 (when the naming class is identical to the selection class, and
  // and when we aren't suppressing the virtual function mechanism).
  
  // if (!get_is_virtual_call()) -- take off because this is not properly set

  // DQ (9/17/2004): Added assertion
     ROSE_ASSERT(decl != NULL);
     ROSE_ASSERT(decl->get_parent() != NULL);

     bool print_colons = false;
  // printf ("mfunc_ref->get_need_qualifier() = %s \n",(mfunc_ref->get_need_qualifier() == true) ? "true" : "false");
     if (mfunc_ref->get_need_qualifier() == true)
        {
       // check if this is a iostream operator function and the value of the overload opt is false

       // DQ (12/28/2005): Changed to check for more general overloaded operators (e.g. operator[])
       // if (!unp->opt.get_overload_opt() && isIOStreamOperator(mfunc_ref));
          if (unp->opt.get_overload_opt() == false && unp->u_sage->isOperator(mfunc_ref) == true)
             {
            // ... nothing to do here
             }
            else
             {
            // curprint ( "\n /* Output the qualified class name */ \n";

            // DQ (10/11/2006): I don't think that we want the fully qualified name, just the class name!
            // curprint (  cdecl->get_qualified_name().str() + "::";
               curprint (  string(decl->get_name().str()) + "::");
            // curprint ( "\n /* DONE: Output the qualified class name */ \n";
               print_colons = true;
             }
        }

  // comments about the logic below can be found above in the unparseFuncRef function.

  // char* func_name = mfunc_ref->get_symbol()->get_name();
     string func_name = mfunc_ref->get_symbol()->get_name().str();

#if 0
     printf ("func_name before processing to extract operator substring = %s \n",func_name.c_str());

     printf ("unp->opt.get_overload_opt()                            = %s \n",(unp->opt.get_overload_opt() == true) ? "true" : "false");
     printf ("strncmp(func_name, \"operator\", 8)                 = %d \n",strncmp(func_name.c_str(), "operator", 8));
     printf ("print_colons                                      = %s \n",(print_colons == true) ? "true" : "false");
     printf ("mfd->get_specialFunctionModifier().isConversion() = %s \n",(mfd->get_specialFunctionModifier().isConversion() == true) ? "true" : "false");
#endif

  // DQ (11/24/2004): unparse conversion operators ("operator X&();") as "result.operator X&()"
  // instead of "(X&) result" (which appears as a cast instead of a function call.
  // check that this an operator overloading function and that colons were not printed
  // if (!unp->opt.get_overload_opt() && !strncmp(func_name, "operator", 8) && !print_colons)
     if (!unp->opt.get_overload_opt() &&
          func_name.size() >= 8 && func_name.substr(0, 8) == "operator" && 
         !print_colons && 
         !mfd->get_specialFunctionModifier().isConversion())
        {
          func_name = func_name.substr(8);
        }
#if 0
     printf ("func_name after processing to extract operator substring = %s \n",func_name.c_str());
#endif

     if( func_name == "[]" ) 
        {
       // DQ (12/28/2005): This case now appears to be just dead code!

       //
       // [DT] 3/30/2000 -- Don't unparse anything here.  The square brackets will
       //      be handled from unparseFuncCall().
       //
       //      May want to handle overloaded operator() the same way.
#if 0
          printf ("Case of unparsing \"operator[]\" \n");
          curprint ( "\n /* Case of unparsing \"operator[]\" */ \n");
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
               printf ("lhs = %p = %s \n",lhs,lhs->sage_class_name());
               curprint ( "\n /* lhs = " + StringUtility::numberToString(lhs) + " = " + lhs->sage_class_name() + " */ \n");
#endif

#if 0
            // DQ (12/28/2005): Comment this out since we now handle it directly in the 
            // function call expression to unparse the arguments with "[" and "]".
               printf ("If this works then this code can be simplified because both branches are identical! \n");
               if (isUnaryOperatorArrowSubtree(lhs) == true)
                  {
                 // We still need to output the "()" to enclose the arguments!
                 // curprint ( "\n /* The lhs WAS an operator-> */ \n";
                 // curprint ( func_name;
                 // curprint ( "operator[]";
                    curprint ( ".operator[]");
                  }
                 else
                  {
                  // curprint ( "\n /* The lhs was NOT an operator-> */ \n";

                 // DQ (12/28/2005): Output the name "operator[]" explicitly 
                 // (since within this function we can't output the "[" and 
                 // then the function arguments followed by a "]")
                 // curprint ( " /* Newly handled case in unparser unparseMFuncRef() */ ";
                 // curprint ( ".operator[]";
                  }
#endif
             }
        }
       else
        {
       // printf ("Case of unparsing a member function which is NOT short form of \"operator[]\" (i.e. \"[]\") funct_name = %s \n",func_name);

       // Make sure that the member function name does not include "()" (this prevents "operator()()" from being output)
          if (func_name != "()")
             {
            // printf ("Case of unparsing a member function which is NOT \"operator()\" \n");

            // DQ (12/11/2004): Catch special case of "a.operator->();" and avoid unparsing it as "a->;" (illegal C++ code)
            // Get the parent SgFunctionCall so that we can check if it's parent was a SgDotExp with a valid rhs_operand!
            // if not then we have the case of "a.operator->();"

            // It might be that this could be a "->" instead of a "."
               ROSE_ASSERT(mfunc_ref != NULL);
               SgDotExp   *dotExpression   = isSgDotExp  (mfunc_ref->get_parent());
               SgArrowExp *arrowExpression = isSgArrowExp(mfunc_ref->get_parent());
            // ROSE_ASSERT(dotExpression != NULL || arrowExpression != NULL);

            // printf ("dotExpression = %p arrowExpression = %p \n",dotExpression,arrowExpression);
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
            // printf ("functionCall = %p \n",functionCall);
               if (functionCall != NULL)
                  {
                 // SgExpressionRoot* expressionRoot = isSgExpressionRoot(functionCall->get_parent());
                 // if ( (expressionRoot != NULL) && (isUnaryOperatorArrowSubtree(functionCall) == true) )
                    if ( unp->u_sage->isUnaryOperatorArrowSubtree(functionCall) == true )
                       {
                      // DQ (Dec, 2004): special (rare) case of .operator->() or ->operator->()
                      // decided to handle these cases because they are amusing (C++ Trivia) :-).
#if 0
                         printf ("Output special case of .operator->() or ->operator->() \n");
                         curprint ( "\n /* Output special case of .operator->() or ->operator->() */ \n");
#endif
                         if (dotExpression != NULL)
                              curprint ( ".operator->()");
                           else
                              curprint ( "->operator->()");
                       }
                      else
                       {
                      // DQ (2/9/2010): Fix for test2010_03.C
                      // curprint (func_name);
                         curprint (string(" ") + func_name + " ");
                       }
                  }
                 else
                  {
                 // DQ (2/9/2010): This does not fix test2010_03.C, but defines a uniform handling as in the fix above.
                 // curprint (func_name);
                    curprint (string(" ") + func_name + " ");
                  }

#if 0
               if (arrowExpression != NULL)
                  {
                 // DQ (2/17/2005): If the parent is an arrow expression then output the dereference operator!
                    curprint ( " /* output a derference of the pointer implied by SgArrowExp */ ");
                    curprint ( " * ");
                  }
#endif
             }
            else
             {
            // printf ("Case of unparsing a member function which is \"operator()\" \n");
             }

       // curprint ( mfunc_ref->get_symbol()->get_name();
          unp->u_debug->printDebugInfo("unparseMFuncRef, Function Name: ", false); unp->u_debug->printDebugInfo(func_name.c_str(), true);
        }
#if 0
     printf ("Leaving unparseMFuncRef \n");
     curprint ( "\n/* leaving unparseMFuncRef */ \n");
#endif

//#endif
   }



#if 0
// DQ (8/13/2007): Moved to common (language independent) base class

void
Unparse_ExprStmt::unparseValue(SgExpression* expr, SgUnparse_Info& info)
   {
  // DQ (11/9/2005): refactored handling of expression trees stemming from 
  // the folding of constants.
     SgValueExp* valueExp = isSgValueExp(expr);

#if 0
     printf ("Inside of unparseValue = %p \n",valueExp);
     valueExp->get_file_info()->display("unparseValue");
#endif

     SgExpression* expressionTree = valueExp->get_originalExpressionTree();
     if (expressionTree != NULL && info.SkipConstantFoldedExpressions() == false)
        {
#if 0
          printf ("Found and expression tree representing a constant generated via constant folding \n");
#endif
#if 0
          curprint ( "\n/* Found and expression tree representing a constant generated via constant folding */\n ");
#endif
       // unparseExpression(expressionTree,info);
          switch (valueExp->variantT())
             {
            // Handle enums so that they will be unparsed as "enum name" instead of as integers
            // bool does not require special handling.
            // case V_SgBoolValExp:             { unparseBoolVal(expr, info);         break; }
               case V_SgEnumVal:                { unparseEnumVal(expr, info);         break; }
               default:
                  {
                    unparseExpression(expressionTree,info);
                  }
             }
        }
       else
        {
          switch (valueExp->variantT())
             {
               case V_SgBoolValExp:             { unparseBoolVal(expr, info);         break; }
               case V_SgCharVal:                { unparseCharVal(expr, info);         break; }
               case V_SgShortVal:               { unparseShortVal(expr, info);        break; }
               case V_SgUnsignedCharVal:        { unparseUCharVal(expr, info);        break; }
               case V_SgWcharVal:               { unparseWCharVal(expr, info);        break; }
               case V_SgStringVal:              { unparseStringVal(expr, info);       break; }
               case V_SgUnsignedShortVal:       { unparseUShortVal(expr, info);       break; }
               case V_SgEnumVal:                { unparseEnumVal(expr, info);         break; }
               case V_SgIntVal:                 { unparseIntVal(expr, info);          break; }
               case V_SgUnsignedIntVal:         { unparseUIntVal(expr, info);         break; }
               case V_SgLongIntVal:             { unparseLongIntVal(expr, info);      break; }
               case V_SgLongLongIntVal:         { unparseLongLongIntVal(expr, info);  break; }
               case V_SgUnsignedLongLongIntVal: { unparseULongLongIntVal(expr, info); break; }
               case V_SgUnsignedLongVal:        { unparseULongIntVal(expr, info);     break; }
               case V_SgFloatVal:               { unparseFloatVal(expr, info);        break; }
               case V_SgDoubleVal:              { unparseDoubleVal(expr, info);       break; }
               case V_SgLongDoubleVal:          { unparseLongDoubleVal(expr, info);   break; }
               case V_SgComplexVal:             { unparseComplexVal(expr, info);      break; }

               default:
                  {
                    printf ("Default reached in switch statement valueExp = %p = %s \n",valueExp,valueExp->class_name().c_str());
                    ROSE_ASSERT(false);
                  }
                
             }
        }
   }
#endif

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
          curprint ( "\n" + remainingString + "\n");
        }
       else
        {
       // curprint ( "\"" + str_val->get_value() + "\"";
          if (str_val->get_wcharString() == true)
               curprint ( "L");
          curprint ( "\"" + str_val->get_value() + "\"");
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
  /* code inserted from specification */
  
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
       curprint (" * _Complex_I)");
     } else { // Complex number
       curprint ("(");
       unparseValue(complex_val->get_real_value(), info);
       curprint (" + ");
       unparseValue(complex_val->get_imaginary_value(), info);
       curprint (" * _Complex_I)");
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
     printf ("In Unparse_ExprStmt::unparseFuncCall expr = %p unp->opt.get_overload_opt() = %s \n",expr,(unp->opt.get_overload_opt() == true) ? "true" : "false");
     curprint ( "\n/* In unparseFuncCall */ \n");
#endif

     SgFunctionCallExp* func_call = isSgFunctionCallExp(expr);
     ROSE_ASSERT(func_call != NULL);
     SgUnparse_Info newinfo(info);
     bool needSquareBrackets = false;

#if 0
     curprint ( "/* func_call->get_function()                   = " + func_call->get_function()->class_name() + " */\n ");
     curprint ( "/* unp->opt.get_overload_opt()                      = " + ((unp->opt.get_overload_opt() == true) ? "true" : "false") + " */\n ");
     curprint ( "/* isBinaryOperator(func_call->get_function()) = " + ((isBinaryOperator(func_call->get_function()) == true) ? "true" : "false") + " */\n ");
#endif
#if 0
     printf ("func_call->get_function() = %p = %s \n",func_call->get_function(),func_call->get_function()->class_name().c_str());
#endif
#if 0
     SgFunctionRefExp* func_ref = isSgFunctionRefExp(func_call->get_function());
     ROSE_ASSERT(func_ref != NULL);
     ROSE_ASSERT(func_ref->get_symbol() != NULL);
     printf ("Function name = %s \n",func_ref->get_symbol()->get_name().str());
     printf ("isBinaryOperator(func_call->get_function())     = %s \n",isBinaryOperator(func_call->get_function()) ? "true" : "false");
     printf ("isSgDotExp(func_call->get_function())           = %s \n",isSgDotExp(func_call->get_function()) ? "true" : "false");
     printf ("isSgArrowExp(func_call->get_function())         = %s \n",isSgArrowExp(func_call->get_function()) ? "true" : "false");

     printf ("isUnaryOperatorPlus(func_call->get_function())  = %s \n",isUnaryOperatorPlus(func_call->get_function()) ? "true" : "false");
     printf ("isUnaryOperatorMinus(func_call->get_function()) = %s \n",isUnaryOperatorMinus(func_call->get_function()) ? "true" : "false");
#endif

  // DQ (6/17/2007): Turn off the generation of "B b; b+b" in favor of "B b; b.A::operator+(b)
  // when A::operator+(A) is called instead of B::operator+(A).  See test2007_73.C for an example.
     bool unparseOperatorSyntax = false;
  // if ( !unp->opt.get_overload_opt() && isBinaryOperator(func_call->get_function()) && (isSgDotExp(func_call->get_function()) != NULL) || (isSgArrowExp(func_call->get_function()) != NULL) )
     if ( (unp->opt.get_overload_opt() == false) && ( (isSgDotExp(func_call->get_function()) != NULL) || (isSgArrowExp(func_call->get_function()) != NULL) ) )
        {
#if 0
          printf ("Found case to investigate for generation of \"B b; b.A::operator+(b)\" instead of \"B b; b+b\" \n");
#endif
          SgBinaryOp* binaryOperator = isSgBinaryOp(func_call->get_function());
          ROSE_ASSERT(binaryOperator != NULL);

          SgExpression* lhs = binaryOperator->get_lhs_operand();
       // printf ("lhs = %p = %s \n",lhs,lhs->class_name().c_str());
          SgExpression* rhs = binaryOperator->get_rhs_operand();
       // printf ("rhs = %p = %s \n",rhs,rhs->class_name().c_str());

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
#if 1
                         printf ("Warning: lhs and member function from different classes (linked though class derivation) \n");
                         curprint ( "/* Warning: lhs and member function from different classes (linked though class derivation) */\n ");
#endif
                      // set<SgSymbol*> & hiddenList = functionClassDefinition->get_hidden_declaration_list();
                         printf ("lhsClassDefinition = %p functionClassDefinition = %p \n",lhsClassDefinition,functionClassDefinition);
                         ROSE_ASSERT(lhsClassDefinition != NULL || functionClassDefinition != NULL);
                      // set<SgSymbol*> & hiddenList = lhsClassDefinition->get_hidden_declaration_list();
                         set<SgSymbol*> & hiddenList = (lhsClassDefinition != NULL) ? lhsClassDefinition->get_hidden_declaration_list() : functionClassDefinition->get_hidden_declaration_list();
                         printf ("Looking for symbol = %p \n",memberFunctionSymbol);
                         set<SgSymbol*>::iterator hiddenDeclaration = hiddenList.find(memberFunctionSymbol);
                         if ( hiddenDeclaration != hiddenList.end() )
                            {
#if 1
                              printf ("Warning: lhs class hidding derived class member function call \n");
                              curprint ( "/* Warning: lhs class hidding derived class member function call */\n ");
#endif
                              unparseOperatorSyntax = true;
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
               printf ("Warning: name qualification required = %s \n",unparseOperatorSyntax ? "true" : "false");
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

  // FIRST PART
  // check if this is an binary operator overloading function and if the overloading 
  // option is off. If so, we traverse using "in-order" tree traversal. However, do 
  // not enter this segment if we have a dot expression. Dot expressions are handled 
  // by the second part. 
     if (!unp->opt.get_overload_opt() && unp->u_sage->isBinaryOperator(func_call->get_function()) && 
         !(isSgDotExp(func_call->get_function())) && !(isSgArrowExp(func_call->get_function())))
        {
          unp->u_debug->printDebugInfo("in FIRST PART of unparseFuncCall", true);
#if 0
          printf ("output 1st part (without syntax sugar) \n");
          curprint ( " /* output 1st part (without syntax sugar) */ ");
#endif
          ROSE_ASSERT(func_call->get_args() != NULL);
          SgExpressionPtrList& list = func_call->get_args()->get_expressions();

       // printf ("argument list size = %ld \n",list.size());

          SgExpressionPtrList::iterator arg = list.begin();
          if (arg != list.end())
             {
               newinfo.set_nested_expression();

            // printf ("output function argument (left) \n");

            // unparse the lhs operand
               unp->u_debug->printDebugInfo("left arg: ", false);
               unparseExpression((*arg), newinfo);
            // unparse the operator
               unparseExpression(func_call->get_function(), info);
               arg++;

            // unparse the rhs operand
               unp->u_debug->printDebugInfo("right arg: ", false);

            // DQ (5/6/2007): Added assert, though this was only a problem when handling unary minus implemented as a non-member function
               ROSE_ASSERT (arg != list.end());
               unparseExpression((*arg), newinfo);

               newinfo.unset_nested_expression();

            // printf ("DONE: output function argument (right) \n");
             }
#if 0
          curprint ( "\n/* Leaving processing first part in unparseFuncCall */ \n");
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

          if (unp->opt.get_overload_opt())
               info.set_nested_expression();
#if 0
          printf ("output 2nd part func_call->get_function() = %s \n",func_call->get_function()->sage_class_name());
          curprint ( " /* output 2nd part  func_call->get_function() = " + func_call->get_function()->sage_class_name() + " */ \n");
#endif

       //
       // Unparse the function first.
       //
          SgUnparse_Info alt_info(info);
       // unparseExpression(func_call->get_function(), info);

       // DQ (6/13/2007): First set to NULL then to the correct value (this allows us to have checking which 
       // detects the overwriting of pointer values generally, but it is not relavant in this case).
          alt_info.set_current_function_call(NULL);
          alt_info.set_current_function_call(func_call);
          unparseExpression(func_call->get_function(), alt_info);

#if 0
          curprint ( " /* after output func_call->get_function() */ ");
#endif

          if (unp->opt.get_overload_opt())
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
          if (!unp->opt.get_overload_opt())
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
#if 1  //Liao, work around for bug 320, operator flag for *i is not set properly, 2/18/2009
       // Please turn this code off when the bug is fixed!                  
                     if   (mfunc_ref != NULL)
                     {
                       string name = mfunc_ref->get_symbol()->get_name().getString();
                       if (name=="operator*")
                       {
                         print_paren=false;
                         if (mfunc_ref->get_symbol()->get_declaration()->get_specialFunctionModifier().isOperator() ==false)
                           cerr<<"unparseCxx_expresssions.C error: found a function named as operator* which is not set as isOperator! \n Fixed its unparsing here temporarily but please consult bug 320!"<<endl;
                       }
                     }
#endif                         

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
       // DQ (2/202/005): This case could be merged with the code above!
          if ( !unp->opt.get_overload_opt() && binary_op != NULL &&
               isBinaryBracketOperator(binary_op->get_rhs_operand()) && 
               unp->u_sage->noQualifiedName(binary_op->get_rhs_operand()) )
             {
            // DQ (12/11/2004): Test for special case of operator->().operator[](i)
               if (isUnaryOperatorArrowSubtree(binary_op->get_lhs_operand()) == false)
                  {
                    needSquareBrackets = true;
                 // Turn off parens in order to output [i] instead of [(i)].
                    print_paren = false; 
                  }
             }
#endif
          if ( needSquareBrackets)
             {
               curprint ( "[");
             }

       // now unparse the function's arguments
       // if (func_call->get_args() != NULL)
       //      printDebugInfo("unparsing arguments of function call", true);

       // curprint ( "\n/* Before preint paren in unparseFuncCall: print_paren = " + print_paren + " */ \n";

          if (print_paren)
             {
            // curprint ( "\n/* Unparse args in unparseFuncCall: opening */ \n"; 
               curprint ( "(");
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
                    SgConstructorInitializer* con_init = isSgConstructorInitializer(*arg);
                    if (con_init != NULL && unp->u_sage->isOneElementList(con_init))
                       {
                         unp->u_debug->printDebugInfo("entering unp->u_sage->unparseOneElemConInit", true);
                      // curprint ( "\n/* unp->u_sage->unparseOneElemConInit in unparseFuncCall */ \n"; 
                         unparseOneElemConInit(con_init, newinfo);
                      // curprint ( "\n/* DONE: unp->u_sage->unparseOneElemConInit in unparseFuncCall */ \n"; 
                       }
                      else
                       {
                      // curprint ( "\n/* unparseExpression in args processing in unparseFuncCall */ \n";
                      // printf ("unparseExpression in args processing in unparseFuncCall \n");
                      // newinfo.display("newinfo in unparseFuncCall()");
                         unparseExpression((*arg), newinfo);
                      // curprint ( "\n/* DONE: unparseExpression in args processing in unparseFuncCall */ \n";
                       }

                    arg++;

                    if (arg != list.end())
                       {
                          curprint ( ","); 
                       }
                  }
             }

          if (print_paren)
             {
            // curprint ( "\n/* Unparse args in unparseFuncCall: closing */ \n"; 
               curprint ( ")");
            // printDebugInfo(") from FuncCall", true);
             }

          if ( needSquareBrackets)
             {
               curprint ( "]");
            // curprint ( " /* needSquareBrackets == true */ ]";
             }

       // curprint ( "\n/* Leaving processing second part in unparseFuncCall */ \n";
        }
#if 0
  // printf ("Leaving Unparse_ExprStmt::unparseFuncCall \n");
     curprint ( "\n/* Leaving Unparse_ExprStmt::unparseFuncCall */ \n");
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

     curprint ( "sizeof(");
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
          unp->u_type->unparseType(sizeof_op->get_operand_type(), info2);
        }
     curprint ( ")");
   }

void Unparse_ExprStmt::unparseTypeIdOp(SgExpression* expr, SgUnparse_Info& info) {
  SgTypeIdOp* typeid_op = isSgTypeIdOp(expr);
  ROSE_ASSERT(typeid_op != NULL);
  /* code inserted from specification */

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
  /* code inserted from specification */

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
  /* code inserted from specification */

     SgUnparse_Info newinfo(info);
     newinfo.unset_PrintName();
     newinfo.unset_isTypeFirstPart();
     newinfo.unset_isTypeSecondPart();

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
       // ROSE_ASSERT(cast_op != NULL);
          //if (cast_op == NULL)
          if (cast_op != NULL) // Liao, 11/2/2010, we should use the original expression tree here!!
             {
            // Jeremiah has submitted the following example: int x[2]; char* y = (char*)x + 1; and the expressionTree is just "x+1".
               unparseExpression(expressionTree,info);

            // Don't continue processing this as a cast!
               return;
             }
           else 
             cast_op = isSgCastExp(expr); // restore to the original non-null value otherwise
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
            // DQ (2/28/2005): Only output the cast if it is NOT compiler generated (implicit in the source code)
            // this avoids redundant casts in the output code and avoid errors in the generated code caused by an 
            // implicit cast to a private type (see test2005_12.C).
            // if (cast_op->get_file_info()->isCompilerGenerated() == false)
               if (cast_op->get_startOfConstruct()->isCompilerGenerated() == false)
                  {
                 // (P *) expr
                 // check if the expression that we are casting is not a string
                 // curprint ( "\n /* explicit cast: cast_op->get_operand_i() = " + cast_op->get_operand_i()->sage_class_name() + " */ \n";
                    if (cast_op->get_operand_i()->variant() != STRING_VAL)
                       {
                      // it is not a string, so we always cast
                      // curprint ( "/* unparseCastOp SgCastExp::c_cast_e nonstring */ "; 
                         curprint ( "(");

                      // DQ (1/14/2006): p_expression_type is no longer stored (type is computed instead)
                      // unp->u_type->unparseType(cast_op->get_expression_type(), newinfo);
                         unp->u_type->unparseType(cast_op->get_type(), newinfo);

                         curprint ( ")");
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
                           // curprint ( "/* unparseCastOp SgCastExp::c_cast_e case string */ ";
                              curprint ( "(");

                           // DQ (1/14/2006): p_expression_type is no longer stored (type is computed instead)
                           // unp->u_type->unparseType(cast_op->get_expression_type(), newinfo);
                              unp->u_type->unparseType(cast_op->get_type(), newinfo);

                              curprint ( ")");
                            }
                       }
                  }
                 else
                  {
                 // curprint ( "/* compiler generated cast not output */";
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

  // curprint ( "/* unparse the cast's operand: get_need_paren() = " + (cast_op->get_operand()->get_need_paren() ? "true" : "false") + " */";

  // DQ (6/15/2005): reinterpret_cast always needs parens
     if (addParens == true)
          curprint ( "/* part of cast */ (");
     unparseExpression(cast_op->get_operand(), info); 

     if (addParens == true)
          curprint ( ")");
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
  /* code inserted from specification */

     if (new_op->get_need_global_specifier())
        {
       // DQ (1/5/2006): I don't think that we want the extra space after the "::".
       // curprint ( ":: ";
          curprint ( "::");
        }

     curprint ( "new ");

  // curprint ( "\n /* Output any placement arguments */ \n";
     SgUnparse_Info newinfo(info);
     newinfo.unset_inVarDecl();
     if (new_op->get_placement_args() != NULL)
        {
       // printf ("Output placement arguments for new operator \n");
          curprint ( "\n/* Output placement arguments for new operator */\n");

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

  // curprint ( "\n /* Output type name for new operator */ \n";

  // printf ("In Unparse_ExprStmt::unparseNewOp: new_op->get_type()->sage_class_name() = %s \n",new_op->get_type()->sage_class_name());

  // DQ (1/17/2006): The the type specified explicitly in the new expressions syntax, 
  // get_type() has been modified to return a pointer to new_op->get_specified_type().
  // unp->u_type->unparseType(new_op->get_type(), newinfo);
     unp->u_type->unparseType(new_op->get_specified_type(), newinfo);

  // printf ("DONE: new_op->get_type()->sage_class_name() = %s \n",new_op->get_type()->sage_class_name());

  // curprint ( "\n /* Output constructor args */ \n";

     if (new_op->get_constructor_args() != NULL)
        {
       // printf ("In Unparse_ExprStmt::unparseNewOp: Now unparse new_op->get_constructor_args() \n");
          unparseExpression(new_op->get_constructor_args(), newinfo);
        }

#if 0
       else
        {
       // printf ("In Unparse_ExprStmt::unparseNewOp: Call unparse type \n");
          unp->u_type->unparseType(new_op->get_type(), newinfo);
        }
#endif

  // curprint ( "\n /* Output builtin args */ \n";

     if (new_op->get_builtin_args() != NULL)
        {
       // printf ("In Unparse_ExprStmt::unparseNewOp: Now unparse new_op->get_builtin_args() \n");
          unparseExpression(new_op->get_builtin_args(), newinfo);
        }

  // curprint ( "\n /* Leaving Unparse_ExprStmt::unparseNewOp */ \n";
  // printf ("Leaving Unparse_ExprStmt::unparseNewOp \n");
#endif
   }

void
Unparse_ExprStmt::unparseDeleteOp(SgExpression* expr, SgUnparse_Info& info)
   {
     SgDeleteExp* delete_op = isSgDeleteExp(expr);
     ROSE_ASSERT(delete_op != NULL);
  /* code inserted from specification */

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
  /* code inserted from specification */

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
  /* code inserted from specification */

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
//  SgAssignInitializer -> SgCastExp ->SgCastExp ->SgIntVal
// We should not unparse them
// This function will check if the nth initializer is from a different file from the aggregate initializer
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
void
Unparse_ExprStmt::unparseAggrInit(SgExpression* expr, SgUnparse_Info& info)
   {
     SgAggregateInitializer* aggr_init = isSgAggregateInitializer(expr);
     ROSE_ASSERT(aggr_init != NULL);
  /* code inserted from specification */

     SgUnparse_Info newinfo(info);
     if (aggr_init->get_need_explicit_braces())
      curprint ( "{");

     SgExpressionPtrList& list = aggr_init->get_initializers()->get_expressions();
     SgExpressionPtrList::iterator p = list.begin();
     size_t index = 0;
     if (p != list.end())
        {
          while (1)
             {
               if (!isFromAnotherFile(aggr_init,index))
                 unparseExpression((*p), newinfo);
               p++;
               index ++;
               if (p != list.end())
                  {  curprint ( ", ");  }
               else
                  break;
             }
        }
     unparseAttachedPreprocessingInfo(aggr_init, info, PreprocessingInfo::inside);
     if (aggr_init->get_need_explicit_braces())
      curprint ( "}");
   }

void
Unparse_ExprStmt::unparseConInit(SgExpression* expr, SgUnparse_Info& info)
{
#if 0
  printf ("In Unparse_ExprStmt::unparseConInit expr = %p \n",expr);
  printf ("WARNING: This is redundent with the Unparse_ExprStmt::unp->u_sage->unparseOneElemConInit (This function does not handle qualidied names!) \n");
#endif

  SgConstructorInitializer* con_init = isSgConstructorInitializer(expr);
  ROSE_ASSERT(con_init != NULL);
  /* code inserted from specification */

  SgUnparse_Info newinfo(info);
  bool outputParenthisis = false;

#if 0
     printf ("In unparseConInit(): con_init->get_need_name()        = %s \n",(con_init->get_need_name() == true) ? "true" : "false");
     printf ("In unparseConInit(): con_init->get_is_explicit_cast() = %s \n",(con_init->get_is_explicit_cast() == true) ? "true" : "false");
     curprint ( string("\n /* con_init->get_need_name()        = ") + (con_init->get_need_name() ? "true" : "false") + " */ \n");
     curprint ( string("\n /* con_init->get_is_explicit_cast() = ") + (con_init->get_is_explicit_cast() ? "true" : "false") + " */ \n");
#endif

  // DQ (3/17/2005): Ignoring ned_name in favor of is_explicit_cast!
  // if (con_init->get_need_name() == true)
  if ((con_init->get_need_name() == true) && (con_init->get_is_explicit_cast() == true) )
  {
    // for foo(B())
    SgName nm;

    // DQ (12/4/2003): Added assertion (one of these is required!)
    // ROSE_ASSERT (con_init->get_declaration() != NULL || con_init->get_class_decl() != NULL);

    // DQ (8/5/2005): Both are now required (under a policy of not having NULL pointers)
    // Well actually the case of a cast introduced by initialization from the return of 
    // a function does not have the information about the class declaration or the constructor, 
    // so we can't assert this. The value of the field bool p_associated_class_unknown 
    // now indicates when both pointers are NULL (else they should be valid pointers).
    // ROSE_ASSERT (con_init->get_declaration() != NULL && con_init->get_class_decl() != NULL);

    ROSE_ASSERT ( con_init->get_associated_class_unknown() == true || 
        con_init->get_declaration() != NULL || 
        con_init->get_class_decl() != NULL);

    // DQ (4/27/2006): Maybe we can finally assert this!
    // ROSE_ASSERT ( con_init->get_associated_class_unknown() == true);

    // DQ (8/5/2005): Now this logic is greatly simplified! Unforntunately not!
#if 0
    if (con_init->get_declaration() != NULL)
    {
      nm = con_init->get_declaration()->get_qualified_name();
    }
#else
    // printf ("con_init->get_declaration() = %s \n",con_init->get_declaration() ? "true" : "false");
    // curprint ( "\n /* con_init->get_declaration() = %s " + (con_init->get_declaration() ? "true" : "false") + " */ \n";
    if (con_init->get_declaration() != NULL)
    {
#if 0
      // printf ("con_init->get_need_qualifier() = %s \n",con_init->get_need_qualifier() ? "true" : "false");
      if (con_init->get_need_qualifier())
        nm = con_init->get_declaration()->get_qualified_name();
      else
        nm = con_init->get_declaration()->get_name();
#else
      nm = con_init->get_declaration()->get_qualified_name();
#endif
    }
    else
    {
      // printf ("con_init->get_class_decl() = %s \n",con_init->get_class_decl() ? "true" : "false");
      if (con_init->get_class_decl() != NULL)
      {
#if 0
        // printf ("con_init->get_need_qualifier() = %s \n",con_init->get_need_qualifier() ? "true" : "false");
        if (con_init->get_need_qualifier())
          nm = con_init->get_class_decl()->get_qualified_name();
        else
          nm = con_init->get_class_decl()->get_name();
#else
        nm = con_init->get_class_decl()->get_qualified_name();
#endif
      }
    }
#endif
    ROSE_ASSERT ( nm.is_null() == false );
    // printf ("In Unparse_ExprStmt::unparseConInit: info.PrintName() = %s nm = %s \n",info.PrintName() ? "true" : "false",nm.str());
    // curprint ( "\n /* Debugging In Unparse_ExprStmt::unparseConInit: nm = " + nm.str() + " */ \n";

    // purify error: nm.str() could be a NULL string
    // if (unp->u_sage->printConstructorName(con_init) && info.PrintName())
    // if ( unp->u_sage->printConstructorName(con_init) )
    if ( unp->u_sage->printConstructorName(con_init) && !nm.is_null() )
    {
      // printf ("unp->u_sage->printConstructorName(con_init) == true \n");
      curprint ( nm.str());
      outputParenthisis = true;
    }
  }

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
    // printf ("Output the parenthisis after the class name \n");
    outputParenthisis = true;
  }
#endif

  // DQ (4/1/2005): sometimes con_init->get_args() is NULL (as in test2005_42.C)
  if (outputParenthisis == true)
    curprint ( "(");

  if (con_init->get_args())
  {
    // DQ (11/13/2004): Remove the parenthesis if we don't output the constructor name
    // this would only work if there was a single argument to the constructor!

    // DQ (3/17/2005): Remove the parenthesis if we don't output the constructor name
    // DQ (3/17/2005): Put the parenthesis BACK!
    // We need this to avoid: doubleArray *arrayPtr1 = (new doubleArray 42); (where it should have been "(42)")
    // if (con_init->get_is_explicit_cast() == true)
    //      curprint ( "(";

    unparseExpression(con_init->get_args(), newinfo);

    // DQ (3/17/2005): Remove the parenthesis if we don't output the constructor name
    // if (con_init->get_is_explicit_cast() == true)
    //      curprint ( ")";
  }

  if (outputParenthisis == true)
  {
    curprint ( ")");
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

  // printf ("Leaving Unparse_ExprStmt::unparseConInit \n");
}

void
Unparse_ExprStmt::unparseAssnInit(SgExpression* expr, SgUnparse_Info& info)
   {
     SgAssignInitializer* assn_init = isSgAssignInitializer(expr);
     ROSE_ASSERT(assn_init != NULL);
  /* code inserted from specification */

  // printf ("In unparseAssnInit(): assn_init->get_is_explicit_cast() = %s \n",(assn_init->get_is_explicit_cast() == true) ? "true" : "false");

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

// JJW (8/6/2008): this should never happen
#if 0
          case SgThrowOp::throw_exception_specification:
             {
            // printf ("Case of SgThrowOp::throw_exception_specification in unparseThrowOp() \n");
               curprint ( "throw");
               ROSE_ASSERT(throw_op->get_typeList() != NULL);

            // Loop over list of types and output each one as a name only
               SgTypePtrList & typeList = *(throw_op->get_typeList());

               if (typeList.size() > 0)
                  {
                    curprint ( ")");
                    SgUnparse_Info info1(info);

                    for (SgTypePtrList::iterator i = typeList.begin(); i != typeList.end(); ++i)
                       {
                      // output the type name
                         unp->u_type->unparseType(*i,info1);
                       }

                 // DQ (11/26/2004): I need to see some example of this case before I handle it!
                    printf ("Sorry, typeList.size() > 0 in case SgThrowOp::throw_exception_specification, not implemented yet! \n");
                    ROSE_ASSERT(false);

                    curprint ( ")");
                  }
               
               break;
             }
#endif

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

     curprint ( "va_start(");
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

     curprint ( "va_start(");
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

     curprint ( "va_end(");
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

     curprint ( "va_copy(");
     unparseExpression(lhsOperand,info);
     curprint ( ",");
     unparseExpression(rhsOperand,info);
     curprint ( ")" );
   }

void
Unparse_ExprStmt::unparseDesignatedInitializer(SgExpression* expr, SgUnparse_Info & info)
   {
 // Liao, fixing bug 355, 6/16/2009
 // for multidimensional array's designated initializer, don't emit '=' until it reaches the last dimension
 // TODO this is not the ultimate fix: EDG uses nested tree for multidimensional array's designated initializer
 // while ROSE's SgDesignatedInitializer is designed to have a flat list for designators 
 // But the EDG_SAGE_connect part generated nested ROSE AST tree following EDG's IR tree.
    bool lastDesignator = true; 
    bool isArrayElementDesignator = false;

     SgDesignatedInitializer* di = isSgDesignatedInitializer(expr);
     const SgExpressionPtrList& designators = di->get_designatorList()->get_expressions();
     for (size_t i = 0; i < designators.size(); ++i) {
       SgExpression* designator = designators[i];
       if (isSgVarRefExp(designator)) {
         // A struct field
         curprint ( "." );
         unparseVarRef(designator, info);
       } else if (isSgValueExp(designator)) {
         curprint ( "[" );
         unparseValue(designator, info);
         curprint ( "]" );
         isArrayElementDesignator = true;
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
        //grab the first one
        SgExpression* grand_child = (isSgAggregateInitializer(child_init)->get_initializers()->get_expressions())[0];
        if (isSgDesignatedInitializer(grand_child))
          lastDesignator = false;
      }
    }
    // Don't emit '=' if it is an array element and is not the last designator
    if (!(isArrayElementDesignator&&!lastDesignator))
      curprint ( " = " );
     unparseExpression(di->get_memberInit(), info);
   }

void
Unparse_ExprStmt::unparsePseudoDtorRef(SgExpression* expr, SgUnparse_Info & info)
   {
     SgPseudoDestructorRefExp* pdre = isSgPseudoDestructorRefExp(expr);
     ROSE_ASSERT(pdre != NULL);
     SgType *objt = pdre->get_object_type();

     curprint ( "~" );
     if (SgNamedType *nt = isSgNamedType(objt))
        {
          curprint ( nt->get_name().str() );
        }
     else
        {
       // PC: I do not think this case will ever occur in practice.  If it does, the resulting
       // code will be invalid.  It may, however, appear in an implicit template instantiation.
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
