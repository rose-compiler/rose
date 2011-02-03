
#include "sage3basic.h"
#include "fortran_support.h"
#include "FortranParserState.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

#define SKIP_C_ACTION_IMPLEMENTATION 0

using namespace std;

// ********************************************************************
// ********************************************************************
//     General support functions for c_action implementation
// ********************************************************************
// ********************************************************************

// DQ (11/17/2007): This function must be defined in this file since it
// references a c_action function (defined in this file).  This file
// is not linked into librose.
void
build_implicit_program_statement_if_required()
   {
     initialize_global_scope_if_required();

  // If we will not be seeing a program-stmt, function-stmt, or subroutine-stmt then 
  // we need to generate a program-stmt as a compiler generated construct.
     SgScopeStatement* currentScope = getTopOfScopeStack();
     if (currentScope->variantT() == V_SgGlobal)
        {
          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
               printf ("Building an implicit program-stmt: currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());

       // DQ (1/14/2011): Take care of types declared in the global scope.
       // DQ (1/13/2011): Where this is in a module or type declaration other types may be referenced 
       // which are constructed as SgTypeDefault and thus must be fixed up as soon as possible (i.e. 
       // before references to them are possible in executable statements).  This function fixes up these
       // references.
          fixup_forward_type_declarations();

       // Token_t id = { 1, 1, 0, "rose_implicit_program_header" };
          Token_t id = { 1, 1, 0, (char*)ROSE_IMPLICIT_FORTRAN_PROGRAM_NAME };
          c_action_program_stmt(NULL,NULL,&id,NULL);

       // Reset the currentScope (should be a SgBasicBlock)
          currentScope = getTopOfScopeStack();
#if 0
          printf ("In build_implicit_program_statement_if_required(): currentScope is %s in file = %s \n",currentScope->class_name().c_str(),currentScope->get_startOfConstruct()->get_filename());
#endif
        }
   }



// ********************************************************************
// ********************************************************************
#ifdef __cplusplus
extern "C" {
#endif

#include "ActionEnums.h"
#include "token.h"
#include "FortranParserAction.h"

/**
 * TODO - change/add from C to Fortran grammar in comments and actions
 * The goal is to do this slowly, one action at a time
 */

/** R102 list
 * generic_name (xyz-name)
 * generic_name_list (xyz-list R101)
 *      :       T_IDENT ( T_COMMA T_IDENT )*
 * 
 * @param count The number of items in the list.
 * @param ident The name of the item placed in the list.
 */
void c_action_generic_name_list__begin()
   {
  // Nothing to do here since we keep a stack and handle items pushed on the the stack later.
  // ROSE_ASSERT(astNameListStack.empty() == true);
#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R102 c_action_generic_name_list__begin()");
#endif

     ROSE_ASSERT(astNameStack.empty() == true);
   }

void c_action_generic_name_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_generic_name_list(): count = %d \n",count);

  // printf ("astNameStack.size() = %zu count = %d \n",astNameStack.size(),count);
     ROSE_ASSERT(astNameStack.size() == (size_t)count);
   }

void c_action_generic_name_list_part(Token_t * ident)
   {
  // These generic_name_list functions appear to be used for function parameters (where as 
  // subroutines use the dummy-arg-list functions, I don't know why there is a difference).

  // Push the entities onto the list at the top of the stack
     ROSE_ASSERT(ident != NULL);
     ROSE_ASSERT(ident->text != NULL);

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_generic_name_list_part(): ident->text = %s \n",ident->text);

  // astNameStack.push_front(ident);
     astNameStack.push_back(ident);
   }

/** R204
 * specification_part
 *
 *
 * @param numUseStmts  Number of use statements.
 * @param numImportStmts  Number of import statements.
 * @param numDeclConstructs  Number of declaration constructs.
 */
#if ROSE_OFP_MINOR_VERSION_NUMBER >= 8 & ROSE_OFP_PATCH_VERSION_NUMBER >= 0
void c_action_specification_part(int numUseStmts, int numImportStmts, int numImplStmts, int numDeclConstructs)
#else
void c_action_specification_part(int numUseStmts, int numImportStmts, int numDeclConstructs)
#endif
   {
  // Not clear what to do here, no IR nodes really need to be built.

  // DQ (5/17/2008): When processing the statement: write (1) N (i,i=1,100)
  // (see test2008_30.f03)

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
        {
#if ROSE_OFP_MINOR_VERSION_NUMBER >= 8 & ROSE_OFP_PATCH_VERSION_NUMBER >= 0
          printf ("In c_action_specification_part: numUseStmts = %d numImportStmts = %d numImplStmts = %d numDeclConstructs = %d \n",numUseStmts,numImportStmts,numImplStmts,numDeclConstructs);
#else
          printf ("In c_action_specification_part: numUseStmts = %d numImportStmts = %d numDeclConstructs = %d \n",numUseStmts,numImportStmts,numDeclConstructs);
#endif
        }

  // We have got to this point and not had to build a containing main function then we will not
  // likely be any further before we process an action statement (not declaration statement).
     build_implicit_program_statement_if_required();

  // DQ (1/13/2011): Where this is in a module or type declaration other types may be referenced 
  // which are constructed as SgTypeDefault and thus must be fixed up as soon as possible (i.e. 
  // before references to them are possible in executable statements).  This function fixes up these
  // references.
     fixup_forward_type_declarations();

  // DQ (11/24/2007): It might be that we could output a "contains" statement here (but it would be better if OFP implemented R1237).

     if ( SgProject::get_verbose() > 1 )
        {
          printf ("\n\n");
          printf ("################################################################################# \n");
          printf ("#############   END OF DECLARATIONS START OF ACTION STATEMENTS   ################ \n");
          printf ("################################################################################# \n");
          printf ("\n\n");
        }

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R204 c_action_specification_part()");
#endif
   }


/**
 * R205, R206, R207
 * declaration_construct
 *
 */
void c_action_declaration_construct()
   {
     if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
          printf ("In c_action_declaration_construct() \n");

  // DQ (12/16/2007): This is the end of a declaration, the declaration should have been built already
  // but if there is any trash on the astLabelSymbolStack then report the problem and clear the stack.
     if (astLabelSymbolStack.empty() == false)
        {
          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
               printf ("WARNING: clearing the astLabelSymbolStack (depth = %zu) \n",astLabelSymbolStack.size());

          astLabelSymbolStack.clear();
        }
   }

/**
 * R208
 * execution_part
 *
 */
void c_action_execution_part()
   {
   }

/**
 * R209
 * execution_part_construct
 *
 */
void c_action_execution_part_construct()
   {
   }

/** R210
 * internal_subprogram_part
 *      :       T_CONTAINS T_EOS internal_subprogram (internal_subprogram)*
 *
 * T_CONTAINS inlined for contains_stmt
 *
 * @param count The number of internal subprograms
 */
void c_action_internal_subprogram_part(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_internal_subprogram_part(): count = %d \n",count);
   }

/**
 * R211
 * internal_subprogram
 *
 */
void c_action_internal_subprogram()
   {
   }

/**
 * R212
 *
 * specification_stmt
*/
void c_action_specification_stmt()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_specification_stmt(): astNodeStack = %zu \n",astNodeStack.size());

  // DQ (12/14/2007): This should have been set by now! See test2007_114.f03
     build_implicit_program_statement_if_required();
   }

/**
 * R213
 * executable_construct
 *
 */
void c_action_executable_construct()
   {
  // This function enforces new rules about internal stack handling.  A number of stacks 
  // should be empty at this point where a statement should have been completely processed.

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_executable_construct(): astExpressionStack = %zu astNodeStack = %zu \n",astExpressionStack.size(),astNodeStack.size());

  // DQ (12/4/2010): This works well as a test (after fixing a lot of bugs where it failed).
     if (astExpressionStack.empty() == false)
        {
          printf ("Error: There appear to be unused expressions on the expression stack \n");
        }
     ROSE_ASSERT(astExpressionStack.empty() == true);

  // DQ (12/7/2010): Not sure if we can do this
     if (astLabelSymbolStack.empty() == false)
        {
          printf ("Error: There appear to be unused SgLabelSymbol IR nodes on the astLabelSymbolStack \n");
        }
     ROSE_ASSERT(astLabelSymbolStack.empty() == true);
   }

/**
 * R214
 *
 * action_stmt
 */
void c_action_action_stmt()
   {
  // DQ (11/15/2007): At the start of every new statement (or at the end?) we should have not remaining AST nodes on the stack!
  // This occures because the where statement processing will sometime need the predicate pushed on the stack. But if we don't see 
  // the where statement then we didn't need it.  It is not clear if this is required handling or the result of something not done 
  // well.

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_action_stmt(): astNodeStack = %zu \n",astNodeStack.size());

     if (astNodeStack.empty() == false)
        {
          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
               printf ("Clearing the astNodeStack of IR nodes that were pushed so that a possible where statement could use them \n");

          astNodeStack.clear();
        }
   }

/**
 * R215
 * keyword
 *
 */
void c_action_keyword()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_keyword() \n");
   }

/**
 * R304
 * name
 *
 * @param id T_IDENT token for the name.
 */
void c_action_name(Token_t *id)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_name(): id = %p = %s \n",id,id != NULL ? id->text : "NULL");

     ROSE_ASSERT(id != NULL);
     astNameStack.push_front(id);
   }

/** R305
 * constant
 *      :       literal_constant
 *      |       T_IDENT
 * 
 * ERR_CHK 305 named_constant replaced by T_IDENT
 * 
 * @param id The identifier representing the named constant if present, otherwise is a literal-constant
 */
void c_action_constant(Token_t * id)
{
}

/**
 * R306
 * literal_constant
 *
 */
void c_action_literal_constant()
   {
   }

/** R308
 * int_constant
 *      :       int_literal_constant
 *      |       T_IDENT
 * 
 * ERR_CHK 308 named_constant replaced by T_IDENT
 * C302 R308 int_constant shall be of type integer
 * inlined integer portion of constant
 * 
 * @param id The identifier representing the named constant if present, otherwise is a literal-constant
 */
void c_action_int_constant(Token_t * id)
{
}

/** R309
 *      char_constant
 *      :       char_literal_constant
 *      |       T_IDENT
 * 
 * ERR_CHK 309 named_constant replaced by T_IDENT
 * C303 R309 char_constant shall be of type character
 * inlined character portion of constant
 * 
 * @param id The identifier representing the named constant if present, otherwise is a literal-constant
 */
void c_action_char_constant(Token_t * id)
{
}

/**
 * R310
 * intrinsic_operator
 *
 */
void c_action_intrinsic_operator()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_intrinsic_operator() \n");
   }

/** R311
 * defined_operator
 *      :       T_DEFINED_OP
 *      |       extended_intrinsic_op
 *
 * @param definedOp The operator (either a defined-unary-op, defined-binary-op, or extended-intrinsic-op).
 * @param isExtended True if the token is an extended-intrinsic-op, otherwise is a defined operator.
 */
void c_action_defined_operator(Token_t * definedOp, ofp_bool isExtendedIntrinsicOp)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_defined_operator(): definedOp = %p = %s isExtendedIntrinsicOp = %s \n",definedOp,definedOp != NULL ? definedOp->text : "NULL",isExtendedIntrinsicOp ? "true" : "false");

     ROSE_ASSERT(definedOp != NULL);

     if (astNameStack.empty() == false)
        {
          printf ("astNameStack is non-empty (skip pushing another copy on the stack: astNameStack.front() = %s \n",astNameStack.front()->text);
        }

  // DQ (10/8/2008): this rule causes the token to sometime appear on the stack a 2nd time.
  // printf ("Should we push the token on the stack? (YES) \n");

  // DQ (10/9/2008): To be uniform in the handling of operators (used defined and intrinsic, we ALWAYS push the token onto the stack!
     astNameStack.push_front(definedOp);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R311 c_action_defined_operator()");
#endif
   }

/**
 * R312
 * extended_intrinsic_op
 *
 */
void c_action_extended_intrinsic_op()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_extended_intrinsic_op() \n");

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R312 c_action_extended_intrinsic_op()");
#endif
   }


/** R313
 * label        :       T_DIGIT_STRING
 *
 * @param lbl The token containing the label
 */
void c_action_label(Token_t * lbl)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_label(): lbl = %p = %s \n",lbl,lbl != NULL ? lbl->text : "NULL");

     ROSE_ASSERT(lbl != NULL);
  // astNameStack.push_front(lbl);
#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R313 c_action_label()");
#endif

  // This can return a NULL symbol if the label is attached to statement which can not be associated with a function scope.
     if (lbl->text != NULL)
        {
       // DQ (1/25/2011): This is checking for a bug found and reported to Craig already.
       // This is an advanced warning of a bug caught in buildNumericLabelSymbol().
          if (lbl->line == 0)
             {
               printf ("Warning (OFP bug): lbl->line == 0 for lbl->text = %s \n",lbl->text);
             }
       // ROSE_ASSERT(lbl->line > 0);

          SgLabelSymbol* labelSymbol = buildNumericLabelSymbol(lbl);

          if (labelSymbol != NULL)
             {
            // printf ("Adding a labelSymbol %p for label = %s to astLabelSymbolStack.size() = %zu \n",labelSymbol,lbl->text,astLabelSymbolStack.size());
               astLabelSymbolStack.push_front(labelSymbol);
             }
        }
       else
        {
          printf ("WARNING: label = %p found with empty text string! \n",lbl);
        }

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R313 c_action_label()");
#endif
   }

/** R313 list
 * label        :       T_DIGIT_STRING
 * label_list
 *      :       label ( T_COMMA label )*
 *
 * // ERR_CHK 313 five characters or less
 *
 * @param count The number of items in the list.
 */
void c_action_label_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_label_list__begin(): \n");
   }
void c_action_label_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_label_list(): count = %d \n",count);
   }

/** R402
 * type-param-value
 *      : expr | T_ASTERISK | T_COLON
 *
 * @param hasExpr True if an expr is present
 * @param hasAsterisk True if an '*' is present
 * @param hasColon True if a ':' is present
 */
void c_action_type_param_value(ofp_bool hasExpr, ofp_bool hasAsterisk, ofp_bool hasColon)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_type_param_value(): hasExpr = %s hasAsterisk = %s hasColon = %s \n",hasExpr ? "true" : "false",hasAsterisk ? "true" : "false",hasColon ? "true" : "false");

  // DQ (9/28/2010): This function is no longer called for each type parameter, and is only called for the length parameter.

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R402 c_action_type_param_value()");
#endif

  // Case not yet implemented
     ROSE_ASSERT(hasColon == false);

     if (hasAsterisk == true)
        {
       // printf ("case: hasAsterisk == true \n");

          SgExpression* asteriskExpression = new SgAsteriskShapeExp();
          setSourcePosition(asteriskExpression);
          astTypeParameterStack.push_front(asteriskExpression);

       // ROSE_ASSERT(false);
        }
       else
        {
          ROSE_ASSERT(astExpressionStack.empty() == false);
          ROSE_ASSERT(astTypeParameterStack.empty() == true);

          astTypeParameterStack.push_front(astExpressionStack.front());
          astExpressionStack.pop_front();
        }

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R402 c_action_type_param_value()");
#endif
   }

/** R403
 * intrinsic_type_spec
 * :    T_INTEGER ( kind_selector )?
 * |    T_REAL ( kind_selector )?
 * |    T_DOUBLE T_PRECISION | T_DOUBLEPRECISION
 * |    T_COMPLEX ( kind_selector )?
 * |    T_DOUBLE T_COMPLEX | T_DOUBLECOMPLEX
 * |    T_CHARACTER ( char_selector )?
 * |    T_LOGICAL ( kind_selector )?
 *
 * @param keyword1 The type keyword token.
 * @param keyword2 The optional keyword token (i.e., T_PRECISION)
 * @param type The type specified (i.e., INTEGER)
 * @param hasKindSelector True if a kind_selector (scalar_int_initialization_expr) is present
 */
void c_action_intrinsic_type_spec(Token_t * keyword1, Token_t * keyword2, int type, ofp_bool hasKindSelector)
   {
  // Save the type on the stack for use later.

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_intrinsic_type_spec(): keyword1 = %p = %s keyword2 = %p = %s type = %d, hasKindSelector = %s \n",
               keyword1,keyword1 != NULL ? keyword1->text : "NULL",keyword2,keyword2 != NULL ? keyword2->text : "NULL",type,hasKindSelector ? "true" : "false");

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R403 c_action_intrinsic_type_spec()");
#endif

#if !SKIP_C_ACTION_IMPLEMENTATION
  // Build a SgType from the type code
     SgType* intrinsicType = createType(type);
     ROSE_ASSERT(intrinsicType != NULL);

  // printf ("At TOP of R403 c_action_intrinsic_type_spec(): intrinsicType = %s \n",intrinsicType->class_name().c_str());

     astBaseTypeStack.push_front(intrinsicType);

  // printf ("hasKindSelector = %s \n",hasKindSelector ? "true" : "false");
     if (hasKindSelector == true)
        {
#if 0
       // Output debugging information about saved state (stack) information.
          outputState("hasKindSelector == true in R403 c_action_intrinsic_type_spec()");
#endif
          int rank = astExpressionStack.size();
       // printf ("In R403 c_action_intrinsic_type_spec(): rank = %d \n",rank);

       // We want this to be either a SgAsteriskShapeExp or an SgIntVal
          SgExpression* lengthExpression = NULL;
          SgExpression* kindExpression   = NULL;

       // DQ (8/21/2010): The length expression (kind, length, or tyep-spec) for the 
       // type is what we are trying to identify.  Depending on the version of Fortran 
       // and the syntax used to specify the kind.

       // Note that for CHARACTER types the length of the string is on the astExpressionStack, but for 
       // where it is a kind parameter it is on the astTypeKindStack.
          if (astExpressionStack.empty() == false)
             {
#if 0
               printf ("Taking the length parameter from the stack <type>*<n> syntax used \n");
#endif
               lengthExpression = astExpressionStack.front();
               astExpressionStack.pop_front();
             }
            else
             {
               if (astTypeKindStack.empty() == false)
                  {
                 // Note that it is (I think) possible for a string to be declared "character*n" where
                 // "n" is an integer expression.  I need a test code for this before I can fix it.
                 // lengthExpression = isSgIntVal(astTypeKindStack.front());
                    kindExpression = astTypeKindStack.front();
                    ROSE_ASSERT(kindExpression != NULL);
                    astTypeKindStack.pop_front();
                  }

               if (isSgTypeChar(intrinsicType) != NULL)
                  {
                 // For character strings the kind should always be 1.
                 // ROSE_ASSERT(lengthExpression->get_value() == 1);
                 // Find the length of the string in the astTypeParameterStack
                    if (astTypeParameterStack.empty() == false)
                       {
                         rank = astTypeParameterStack.size();
                         ROSE_ASSERT(rank == 1);

                         lengthExpression = isSgIntVal(astTypeParameterStack.front());
                         if (lengthExpression == NULL)
                            {
#if 0
                              printf ("Warning type parameter is non integer type (OK for F90) \n");
#endif
                              lengthExpression = astTypeParameterStack.front();
                            }
                         ROSE_ASSERT(lengthExpression != NULL);
                         astTypeParameterStack.pop_front();
                       }
                  }
             }
#if 0
          printf ("In c_action_intrinsic_type_spec(): intrinsicType = %p = %s \n",intrinsicType,intrinsicType->class_name().c_str());
#endif
       // Base on the base type, we want to inteprete the type parameters, kind parameters, etc. 
       // and generate the correct mapping to the type IR node in the ROSE and use that in the AST.
          switch(intrinsicType->variantT())
             {
               case V_SgTypeChar:
                  {
                 // DQ (12/3/2010): Commented out due to test2010_119.f90
                    ROSE_ASSERT(rank == 1);

                    ROSE_ASSERT(lengthExpression != NULL);
                 // printf ("In c_action_intrinsic_type_spec(): lengthExpression = %s \n",lengthExpression->class_name().c_str());

                 // DQ (10/4/2010): Always use the expression form of length representation (scalar representation of lenght is deprecated).
                    ROSE_ASSERT(lengthExpression != NULL);
                 // stringType = SgTypeString::createType(lengthExpression,0);
                 // SgTypeString* stringType = SgTypeString::createType(lengthExpression,0,kindExpression);
                    SgTypeString* stringType = SgTypeString::createType(lengthExpression,kindExpression);
                    ROSE_ASSERT(stringType != NULL);
                    ROSE_ASSERT(lengthExpression->get_parent() != NULL);
                    if (kindExpression != NULL)
                       {
                         ROSE_ASSERT(kindExpression->get_parent() != NULL);
                       }

                 // Replace the base type with the just built string type
                    astBaseTypeStack.pop_front();
                    astBaseTypeStack.push_front(stringType);
                    break;
                  }

               case V_SgTypeInt:
                  {
                 // Note that this does not have to be an integer value and can be another variable or "c_int" (for example)
                 // DQ (10/4/2010): Moved to new (improved) design of type_kind data member in SgType.
                    ROSE_ASSERT(kindExpression->get_parent() == NULL);
                    SgTypeInt* integerType = SgTypeInt::createType(0,kindExpression);
                    kindExpression->set_parent(integerType);
                    ROSE_ASSERT(kindExpression->get_parent() != NULL);

                 // Replace the base type with the just built string type
                    astBaseTypeStack.pop_front();
                    astBaseTypeStack.push_front(integerType);
                    break;
                  }

               case V_SgTypeBool:
                  {
                 // DQ (10/4/2010): Logical types are now represented with the associated kind.
                    SgTypeBool* boolType = SgTypeBool::createType(kindExpression);
                    ROSE_ASSERT(kindExpression->get_parent() == NULL);
                    kindExpression->set_parent(boolType);

                 // Replace the base type with the just built string type
                    astBaseTypeStack.pop_front();
                    astBaseTypeStack.push_front(boolType);
                    break;
                  }

               case V_SgTypeFloat:
                  {
                    ROSE_ASSERT(kindExpression->get_parent() == NULL);
                    SgTypeFloat* floatType = SgTypeFloat::createType(kindExpression);
                    kindExpression->set_parent(floatType);
                    ROSE_ASSERT(kindExpression->get_parent() != NULL);

                 // Replace the base type with the just built string type
                    astBaseTypeStack.pop_front();
                    astBaseTypeStack.push_front(floatType);
                    break;
                  }

               case V_SgTypeComplex:
                  {
                    ROSE_ASSERT(kindExpression->get_parent() == NULL);
                    SgTypeComplex* complexType = SgTypeComplex::createType(SgTypeFloat::createType(kindExpression),kindExpression);
                    kindExpression->set_parent(complexType);
                    ROSE_ASSERT(kindExpression->get_parent() != NULL);

                 // Replace the base type with the just built string type
                    astBaseTypeStack.pop_front();
                    astBaseTypeStack.push_front(complexType);
                    break;
                  }

               default:
                  {
                    printf ("Error: unimplemented type = %s \n",intrinsicType->class_name().c_str());
                    ROSE_ASSERT(false);
                    break;
                  }
             }

       // We should have drained these stacks
          ROSE_ASSERT(astTypeKindStack.empty() == true);
          ROSE_ASSERT(astTypeParameterStack.empty() == true);
          ROSE_ASSERT(astExpressionStack.empty() == true);

#if 0
       // Output debugging information about saved state (stack) information.
          outputState("hasKindSelector == true (BOTTOM) in R403 c_action_intrinsic_type_spec()");
#endif
        }
       else
        {
       // DQ (8/14/2010): Not clear if this is a problem so output a message for now to support debugging. (NOT A PROBLEM)
        }

  // Make sure that we have setup the base type for further processing
     ROSE_ASSERT(astBaseTypeStack.empty() == false);
#endif

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R403 c_action_intrinsic_type_spec()");
#endif
   }

/** R404
 * kind_selector
 *      :       T_LPAREN (T_KIND T_EQUALS)? expr T_RPAREN
 *      |       T_ASTERISK T_DIGIT_STRING               // Nonstandard extension: source common practice
 *                                                                        // e.g., COMPLEX*16
 *
 * @param token1 KIND keyword token (or *, nonstandard usage)
 * @param token2 = token (or size of type, nonstandard usage)
 * @param hasExpression True if an expr is present (standard-confirming option)
 */
// void c_action_kind_selector(ofp_bool hasExpression, Token_t * typeSize)
void c_action_kind_selector(Token_t * token1, Token_t * token2, ofp_bool hasExpression)
   {
  // printf ("In c_action_kind_selector(): hasExpression = %s typeSize = %s \n",hasExpression ? "true" : "false", typeSize ? typeSize->text : "NULL");

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_kind_selector(): token1 = %p = %s token2 = %p = %s hasExpression = %s \n",token1,token1 != NULL ? token1->text : "NULL",token2,token2 != NULL ? token2->text : "NULL",hasExpression ? "true" : "false");

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R404 c_action_kind_selector()");
#endif

  // ROSE_ASSERT(hasExpression == false);

  // For the case of "real*8" this is the only rule called before seeing the base type ("real")
#if 0
  // FMZ (07/06/2010): work around, wait for a "real fix" from OFP 
     if (token2 != NULL)
#else
     if (token2 != NULL && atoi(token2->text) != atoi("="))
#endif
        {
       // Note that token1 == "*" if token2 is valid, I think.
       // This is the case of <type>*n (e.g. "real*8").  Should we convert this to an expression?
       // astNameStack.push_front(token2);
          int value = atoi(token2->text);
          SgIntVal* integerValue = new SgIntVal (value,token2->text);
          ROSE_ASSERT(integerValue != NULL);

       // This set the start and end source position to the beginning of the number's text string)
          setSourcePosition(integerValue,token2);

          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
               printf ("In c_action_kind_selector(): token2 = %s integerValue = %d \n",token2->text,integerValue->get_value());

#if 0
       // FMZ (07/06/2010): fix the bug cause "kind" value lost for decl like: integer*8 etc.
       // Push the integer value onto the expression stack
          astExpressionStack.push_front(integerValue);
#else
          astTypeKindStack.push_front(integerValue);
#endif
        }
       else
        {
          ROSE_ASSERT(astExpressionStack.empty() == false);
          ROSE_ASSERT(astTypeKindStack.empty() == true);
          astTypeKindStack.push_front(astExpressionStack.front());
          astExpressionStack.pop_front();
        }
#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R404 c_action_kind_selector()");
#endif
   }

/** R405
 * signed_int_literal_constant
 *  :   (T_PLUS|T_MINUS)? int_literal_constant
 *
 * @param sign The sign: positive, negative, or null.
 */
void c_action_signed_int_literal_constant(Token_t * sign)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_signed_int_literal_constant(): sign = %p = %s \n",sign,(sign != NULL) ? sign->text : "NULL"); 
   }

/** R406
 * int_literal_constant
 *      :       T_DIGIT_STRING (T_UNDERSCORE kind_param)?
 *
 * @param digitString The digit string representing the constant
 * @param kindParam The kind parameter
 */
void c_action_int_literal_constant(Token_t * digitString, Token_t * kindParam)
   {
  // Build a SgValue object

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_int_literal_constant(): digitString = %p = %d kindParam = %p \n",digitString, (digitString != NULL) ? atoi(digitString->text) : -1, kindParam); 

#if !SKIP_C_ACTION_IMPLEMENTATION
  // There should at least be a value string to represent the number
     ROSE_ASSERT(digitString != NULL);
     ROSE_ASSERT(digitString->text != NULL);

  // FMZ added  10/27/2009
     if ( kindParam!=NULL &&  atoi(kindParam->text)==8) {
            long  value = atol(digitString->text);

            string tmp = digitString->text;
            tmp = tmp + "_8";

            //SgLongIntVal* integerValue = new SgLongIntVal (value,digitString->text);
            SgLongIntVal* integerValue = new SgLongIntVal (value,tmp);
            ROSE_ASSERT(integerValue != NULL);
            setSourcePosition(integerValue,digitString);
            astExpressionStack.push_front(integerValue);
     } else  {
            int value = atoi(digitString->text);
            SgIntVal* integerValue = new SgIntVal (value,digitString->text);
            ROSE_ASSERT(integerValue != NULL);
            setSourcePosition(integerValue,digitString);
            astExpressionStack.push_front(integerValue);
     }

#endif

  // printf ("Leaving c_action_int_literal_constant(): astExpressionStack.size() = %ld \n",astExpressionStack.size()); 
   }

/**
 * R407
 * kind_param
 *
 * @param kind T_DIGIT_STRING or T_IDENT token which is the kind_param.
 */
void c_action_kind_param(Token_t * kind)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_kind_param(): kind = %p = %s \n",kind,kind != NULL ? kind->text : "NULL");
   }

/**
 * R411
 * boz_literal_constant
 *
 */
void c_action_boz_literal_constant(Token_t * constant)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_boz_literal_constant(): constant = %p = %s \n",constant,constant != NULL ? constant->text : "NULL");
   }

/** R416
 * signed_real_literal_constant
 *  :   (T_PLUS|T_MINUS)? real_literal_constant
 *
 * @param sign The sign: positive, negative, or null.
 */
void c_action_signed_real_literal_constant(Token_t * sign)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_signed_real_literal_constant(): sign = %p = %s \n",sign,sign != NULL ? sign->text : "NULL");
#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R416 c_action_signed_real_literal_constant()");
#endif

     if (sign == NULL)
        {
       // This is a bug in OFP (I think) See test2007_144.f90.
       // There should be a valid token for the "-" if it was present.

          ROSE_ASSERT(astExpressionStack.empty() == false);
          SgExpression* expression = astExpressionStack.front();
          SgValueExp* valueExpression = isSgValueExp(expression);
          ROSE_ASSERT(valueExpression != NULL);

          switch(valueExpression->variantT())
             {
               case V_SgFloatVal:
                  {
                    SgFloatVal* floatValue = isSgFloatVal(valueExpression);

                 // DQ (1/20/2008): This bug in OFP is now fixed, I think, nope not yet!
                 // Flip the sign on the value stored internally.
                 // floatValue->set_value( - floatValue->get_value() );
                    floatValue->set_value( floatValue->get_value() );

                 // Change the sign on the original string as well.
                    string valueString = floatValue->get_valueString();

                 // DQ (1/20/2008): This bug in OFP is now fixed, I think
                 // if (valueString.empty() == false)
                 //      floatValue->set_valueString( "-" + valueString );
                    ROSE_ASSERT(valueString.empty() == false);
                    break;
                  }
                
               default:
                  {
                    printf ("Error, default reached in switch: valueExpression = %p = %s \n",valueExpression,valueExpression->class_name().c_str());
                    ROSE_ASSERT(false);
                  }
             }
        }
       else
        {
       // This is likely the case of "+" used explicitly

       // No need to do anything here (unless we want to explicitly preserve the "+".
       // We can do this later by using the SgUnaryPlus operator.
        }
#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R416 c_action_signed_real_literal_constant()");
#endif
   }

/** R417
 * real_literal_constant
 *      :   REAL_CONSTANT ( T_UNDERSCORE kind_param )?
 *      |   DOUBLE_CONSTANT ( T_UNDERSCORE kind_param )?
 *
 * Replaced by
 *      :       T_DIGIT_STRING T_PERIOD_EXPONENT (T_UNDERSCORE kind_param)?
 *      |       T_DIGIT_STRING T_PERIOD (T_UNDERSCORE kind_param)?
 *      |       T_PERIOD_EXPONENT (T_UNDERSCORE kind_param)?
 *
 * @param digits The integral part
 * @param fractionExp The fractional part and exponent
 * @param kindParam The kind parameter
 */
void c_action_real_literal_constant(Token_t * realConstant, 
                                                                                                Token_t * kindParam)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_real_literal_constant(): realConstant = %p = %s kindParam = %p = %s \n",
               realConstant,realConstant != NULL ? realConstant->text : "NULL",
               kindParam,kindParam != NULL ? kindParam->text : "NULL");

#if !SKIP_C_ACTION_IMPLEMENTATION
  // There should at least be a value string to represent the number
     ROSE_ASSERT(realConstant != NULL);
     ROSE_ASSERT(realConstant->text != NULL);

     if (kindParam != NULL)
        {
       // If we do have a kind parameter then we use it to build different sizes of integer values
          if ( SgProject::get_verbose() > 0 )
               printf ("In c_action_real_literal_constant(): kind parameter support not implemented, kindParam = %s \n",kindParam->text);
        }

  // Handle ethe case of a regular int (the kind parameter will allow for short, and long sizes)
     double value = atof(realConstant->text);
     SgFloatVal* floatValue = new SgFloatVal (value,realConstant->text);
     ROSE_ASSERT(floatValue != NULL);

  // This set the start and end source position to the beginning of the number's text string)
     setSourcePosition(floatValue,realConstant);

  // printf ("digitString = %s value = %d \n",digitString->text,value);

  // Push the integer value onto the expression stack
     astExpressionStack.push_front(floatValue);
#endif

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R417 c_action_real_literal_constant()");
#endif
   }

/**
 * R421
 * complex_literal_constant
 *
 */
void c_action_complex_literal_constant()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_complex_literal_constant() \n");

  // DQ (1/27/2009): Take the two expressions on the stack (should be literal value
  // expressions) and use them to build a complex value, and push it into the stack.

     ROSE_ASSERT(astExpressionStack.empty() == false);
     SgValueExp* imaginaryValue = isSgValueExp(astExpressionStack.front());
     astExpressionStack.pop_front();

     ROSE_ASSERT(astExpressionStack.empty() == false);
     SgValueExp* realValue = isSgValueExp(astExpressionStack.front());
     astExpressionStack.pop_front();

     SgValueExp* complexValue = new SgComplexVal(realValue,imaginaryValue,realValue->get_type(),"");
     setSourcePosition(complexValue);

  // Push the complex value onto the expression stack
     astExpressionStack.push_front(complexValue);
   }

/** R422
 * real_part
 * 
 * ERR_CHK 422 named_constant replaced by T_IDENT
 * 
 * @param hasIntConstant True if signed-int-literal-constant is present
 * @param hasRealConstant True if signed-real-literal-constant is present
 * @param id The named-constant (optional)
 */
void c_action_real_part(ofp_bool hasIntConstant, ofp_bool hasRealConstant, Token_t * id)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_real_part(): hasIntConstant = %s hasRealConstant = %s id = %s \n",hasIntConstant ? "true" : "false",hasRealConstant ? "true" : "false",id != NULL ? id->text : "NULL");
   }

/** R423
 * imag_part
 * 
 * ERR_CHK 423 named_constant replaced by T_IDENT
 * 
 * @param hasIntConstant True if signed-int-literal-constant is present
 * @param hasRealConstant True if signed-real-literal-constant is present
 * @param id The named-constant (optional)
 */
void c_action_imag_part(ofp_bool hasIntConstant, ofp_bool hasRealConstant, Token_t * id)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_imag_part(): hasIntConstant = %s hasRealConstant = %s id = %s \n",hasIntConstant ? "true" : "false",hasRealConstant ? "true" : "false",id != NULL ? id->text : "NULL");
   }

/** R424
 * char-selector
 *      :       T_ASTERISK char_length (T_COMMA)?
 *      |       T_LPAREN (T_KIND | T_LEN) T_EQUALS type_param_value
 *                       ( T_COMMA (T_KIND | T_LEN) T_EQUALS type_param_value )? T_RPAREN
 *      |       T_LPAREN type_param_value ( T_COMMA (T_KIND T_EQUALS)? expr )? T_RPAREN
 *
 * @param kindOrLen1 Specifies whether the first kind or len type-param-value is present
 * @param kindOrLen2 Specifies whether the second kind or len type-param-value is present
 * @param hasAsterisk True if a '*' char-selector is specified
 */
// void c_action_char_selector(int kindOrLen1, int kindOrLen2, ofp_bool hasAsterisk)
void c_action_char_selector(Token_t * type_keyword, Token_t * optional_keyword, int kindOrLen1, int kindOrLen2, ofp_bool hasAsterisk)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_char_selector(): type_keyword = %p = %s optional_keyword = %p = %s kindOrLen1 = %d kindOrLen2 = %d hasAsterisk = %s \n",
               type_keyword,type_keyword != NULL ? type_keyword->text : "NULL",
               optional_keyword,optional_keyword != NULL ? optional_keyword->text : "NULL",kindOrLen1,kindOrLen2,hasAsterisk ? "true" : "false");

  // DQ (9/27/2010): New for OFP 0.8.2 is that the c_action_type_param_value() is 
  // only called for the length type parameter and not for the kind parameter. It
  // used to be called for each parameter.

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("TOP of R424 list c_action_char_selector()");
#endif

// DQ (1/25/2011): This is a great example of where we should cut loose older versions of OFP
// to reduce the complexity of the Fortran support.
#if ROSE_OFP_MINOR_VERSION_NUMBER >= 8 & ROSE_OFP_PATCH_VERSION_NUMBER >= 2
  // DQ (9/28/2010): This function used to be called twice for "CHARACTER(kind=1,len=2) C"
  // In this version R404 is now not called as well (a previous OFP bug).  This changes
  // what is on the stack and thus how it is processed.
     if (kindOrLen1 == KindLenParam_none)
        {
          if (hasAsterisk == true)
             {
            // See test2007_196.f for an example of this case
            // printf ("In c_action_char_selector(): astTypeParameterStack.empty() = %s \n",astTypeParameterStack.empty() ? "true" : "false");
               if (astTypeParameterStack.empty() == false)
                  {
                    ROSE_ASSERT(astTypeParameterStack.empty() == false);
                    ROSE_ASSERT(astTypeKindStack.empty() == true);
                 // astTypeKindStack.push_front(astTypeParameterStack.front());
                    astExpressionStack.push_front(astTypeParameterStack.front());
                    astTypeParameterStack.pop_front();
                  }
                 else
                  {
                 // Not sure what to do here, I think we have to push an Asterisk onto the astExpressionStack 
                 // and a KindLenParam_len onto either the astTypeParameterStack of the astTypeKindStack.
                 // printf ("In c_action_char_selector(): R424 astTypeParameterStack.empty() == true), but hasAsterisk == true, so this is character*(*) \n");
#if 0
                    SgAsteriskShapeExp* asteriskShapeExp = new SgAsteriskShapeExp();
                    astExpressionStack.push_front(asteriskShapeExp);
#endif
                  }
             }
            else
             {
               ROSE_ASSERT(astTypeParameterStack.empty() == true);
               ROSE_ASSERT(astTypeKindStack.empty() == true);
             }
        }
       else
        {
       // This is the first parameter (if it is "kind=")
#if ROSE_OFP_MINOR_VERSION_NUMBER >= 8 & ROSE_OFP_PATCH_VERSION_NUMBER < 2
          if (kindOrLen1 == KindLenParam_kind && kindOrLen2 == KindLenParam_none)
             {
            // ROSE_ASSERT(astTypeKindStack.size() == 1);

            // This appears to be a bug in OFP.
            // R404 was called even though the "kind=" was explicitly specified,
            // so move the entry put into the KindStack from the astParameterStack
               ROSE_ASSERT(astTypeParameterStack.empty() == false);
               ROSE_ASSERT(astTypeKindStack.empty() == true);
               astTypeKindStack.push_front(astTypeParameterStack.front());
               astTypeParameterStack.pop_front();
             }

          if (kindOrLen1 == KindLenParam_len)
               ROSE_ASSERT(astTypeParameterStack.size() == 1);

          if (kindOrLen2 == KindLenParam_none)
             {
            // or
               ROSE_ASSERT(astTypeParameterStack.size() == 1 || astTypeKindStack.size() == 1);
             }
            else
             {
            // and
               if (astTypeKindStack.size() != 1)
                  {
                    printf ("astTypeKindStack.size() = %zu \n",astTypeKindStack.size());
#if 1
                 // Output debugging information about saved state (stack) information.
                    outputState("Error in R424 list c_action_char_selector()");
#endif
                  }
               ROSE_ASSERT(astTypeKindStack.size() == 1);
               ROSE_ASSERT(astTypeParameterStack.size() == 1);
             }
#else
          ROSE_ASSERT(hasAsterisk == false);
       // printf ("In R424: kindOrLen1 = %d kindOrLen2 = %d \n",kindOrLen1,kindOrLen2);

          if (kindOrLen1 == KindLenParam_kind)
             {
            // This needs to transfered from the astExpressionStack to the astTypeKindStack
               ROSE_ASSERT(astExpressionStack.size() == 1);
               ROSE_ASSERT(astTypeKindStack.empty() == true);

               astTypeKindStack.push_front(astExpressionStack.front());
               astExpressionStack.pop_front();
             }
            else
             {
               if (kindOrLen1 == KindLenParam_len)
                  {
                 // This should have already been moved to the parameter stack.
                  }
                 else
                  {
                  }
             }

          if (kindOrLen2 == KindLenParam_kind)
             {
            // This needs to transfered from the astExpressionStack to the astTypeKindStack
               ROSE_ASSERT(astExpressionStack.size() == 1);
               ROSE_ASSERT(astTypeKindStack.empty() == true);

               astTypeKindStack.push_front(astExpressionStack.front());
               astExpressionStack.pop_front();
             }
            else
             {
               if (kindOrLen2 == KindLenParam_len)
                  {
                  }
                 else
                  {
                  }
             }
#endif
        }
#else
     if (kindOrLen1 == KindLenParam_none)
        {
          if (hasAsterisk == true)
             {
            // See test2007_196.f for an example of this case
            // printf ("In c_action_char_selector(): astTypeParameterStack.empty() = %s \n",astTypeParameterStack.empty() ? "true" : "false");
               if (astTypeParameterStack.empty() == false)
                  {
                    ROSE_ASSERT(astTypeParameterStack.empty() == false);
                    ROSE_ASSERT(astTypeKindStack.empty() == true);
                 // astTypeKindStack.push_front(astTypeParameterStack.front());
                    astExpressionStack.push_front(astTypeParameterStack.front());
                    astTypeParameterStack.pop_front();
                  }
                 else
                  {
                 // Not sure what to do here, I think we have to push an Asterisk onto the astExpressionStack 
                 // and a KindLenParam_len onto either the astTypeParameterStack of the astTypeKindStack.
                 // printf ("In c_action_char_selector(): R424 astTypeParameterStack.empty() == true), but hasAsterisk == true, so this is character*(*) \n");
#if 0
                    SgAsteriskShapeExp* asteriskShapeExp = new SgAsteriskShapeExp();
                    astExpressionStack.push_front(asteriskShapeExp);
#endif
                  }
             }
            else
             {
               ROSE_ASSERT(astTypeParameterStack.empty() == true);
               ROSE_ASSERT(astTypeKindStack.empty() == true);
             }
        }
       else
        {
       // This is the first parameter (if it is "kind=")
          if (kindOrLen1 == KindLenParam_kind && kindOrLen2 == KindLenParam_none)
             {
            // ROSE_ASSERT(astTypeKindStack.size() == 1);

            // This appears to be a bug in OFP.
            // R404 was called even though the "kind=" was explicitly specified,
            // so move the entry put into the KindStack from the astParameterStack
               ROSE_ASSERT(astTypeParameterStack.empty() == false);
               ROSE_ASSERT(astTypeKindStack.empty() == true);
               astTypeKindStack.push_front(astTypeParameterStack.front());
               astTypeParameterStack.pop_front();
             }

          if (kindOrLen1 == KindLenParam_len)
               ROSE_ASSERT(astTypeParameterStack.size() == 1);

          if (kindOrLen2 == KindLenParam_none)
             {
            // or
               ROSE_ASSERT(astTypeParameterStack.size() == 1 || astTypeKindStack.size() == 1);
             }
            else
             {
            // and
               if (astTypeKindStack.size() != 1)
                  {
                    printf ("astTypeKindStack.size() = %zu \n",astTypeKindStack.size());
#if 1
                 // Output debugging information about saved state (stack) information.
                    outputState("Error in R424 list c_action_char_selector()");
#endif
                  }
               ROSE_ASSERT(astTypeKindStack.size() == 1);
               ROSE_ASSERT(astTypeParameterStack.size() == 1);
             }
        }
#endif

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R424 list c_action_char_selector()");
#endif
   }

/** R425
 * length-selector
 *      :       T_LPAREN ( T_LEN T_EQUALS )? type_param_value T_RPAREN
 *      |       T_ASTERISK char_length (T_COMMA)?
 *
 * @param kindOrLen Specifies whether a kind or len type-param-value is present
 * @param hasAsterisk True if a '*' char-selector is specified
 */
// void c_action_length_selector(int kindOrLen, ofp_bool hasAsterisk)
void c_action_length_selector(Token_t * len_keyword, int kindOrLen, ofp_bool hasAsterisk)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_length_selector(): len_keyword = %p = %s kindOrLen = %d hasAsterisk = %s \n",len_keyword,len_keyword != NULL ? len_keyword->text : "NULL",kindOrLen,hasAsterisk ? "true" : "false");
   }

/** R426
 * char_length
 *      :       T_LPAREN type_param_value T_RPAREN
 *      |       scalar_int_literal_constant
 *
 * @param hasTypeParamValue True if a type-param-value is specified, otherwise is a scalar-int-literal-constant
 */
void c_action_char_length(ofp_bool hasTypeParamValue)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_char_length(): hasTypeParamValue = %s \n",hasTypeParamValue ? "true" : "false");
#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R426 list c_action_char_length()");
#endif

  // printf ("astBaseTypeStack.empty() = %s \n",astBaseTypeStack.empty() ? "true" : "false");

  // if (astTypeStack.empty() == true)
     if (astBaseTypeStack.empty() == true)
        {
       // This is a CHARACTER*<char_length> instead of CHARACTER var*<char_length>.
       // The details of this case will be handled in the construciton of the type.
          if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
               printf ("In R426: This is a CHARACTER*<char_length> instead of CHARACTER var*<char_length> \n");
        }
       else
        {
       // ROSE_ASSERT(astTypeStack.empty() == false);
          ROSE_ASSERT(astBaseTypeStack.empty() == false);

       // Convert the SgTypeChar on the astTypeStack to a SgTypeString
       // SgTypeChar* charType = isSgTypeChar(astTypeStack.front());
          SgTypeChar* charType = isSgTypeChar(astBaseTypeStack.front());
          ROSE_ASSERT(charType != NULL);
#if 1
       // Output debugging information about saved state (stack) information.
          outputState("Before generating a string in R426 c_action_char_length()");
#endif

       // DQ (12/5/2010): New code for handling "character name*30" as a string.
       // This action is part of handling: "character name*30" which is a specification of a string and NOT an array of characters.

       // This type transformation of length to string only applies to characters.
          ROSE_ASSERT(astBaseTypeStack.empty() == false);
          ROSE_ASSERT(isSgTypeChar(astBaseTypeStack.front()) != NULL);
#if 0
          ROSE_ASSERT(astExpressionStack.empty() == false);
          SgExpression* stringLength = astExpressionStack.front();
#else
       // DQ (1/30/2011): this is the new version of the code.
          SgExpression* stringLength = NULL;
          if (astExpressionStack.empty() == true)
             {
            // This is the case for test2010_158.f90.
               ROSE_ASSERT(astTypeParameterStack.empty() == false);
               stringLength = astTypeParameterStack.front();
               astTypeParameterStack.pop_front();
             }
            else
             {
            // DQ (1/30/2010): Can we assert this!
               ROSE_ASSERT(astTypeParameterStack.empty() == true);
               stringLength = astExpressionStack.front();
               astExpressionStack.pop_front();
             }
#endif

       // printf ("stringLength = %p = %s \n",stringLength,stringLength->class_name().c_str());

       // Note: Must call the SgTypeString::createType() to support the global type tables.
       // SgTypeString* stringType = new SgTypeString(stringLength);
          SgTypeString* stringType = SgTypeString::createType(stringLength);
       // stringLength->set_parent(stringType);

       // Leave the correct type on the astTypeStack
          astTypeStack.push_front(stringType);
        }

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R426 list c_action_char_length()");
#endif

  // printf ("Exiting as a test! \n");
  // ROSE_ASSERT(false);
   }

/**
 * scalar_int_literal_constant
 *
 */
void c_action_scalar_int_literal_constant()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_scalar_int_literal_constant() \n");
   }


/** R427
 * char_literal_constant
 *      :       T_DIGIT_STRING T_UNDERSCORE T_CHAR_CONSTANT
 *         // removed the T_UNDERSCORE because underscores are valid characters 
 *         // for identifiers, which means the lexer would match the T_IDENT and 
 *         // T_UNDERSCORE as one token (T_IDENT).
 *              |       T_IDENT T_CHAR_CONSTANT
 *              |       T_CHAR_CONSTANT
 * 
 * @param digitString Optional digit-string representing the kind parameter
 * @param id Optional identifier representing the kind parameter variable AND the '_'
 * @param str The token containing the literal character constant
 */
void c_action_char_literal_constant(Token_t * digitString, Token_t * id, Token_t * str)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_char_literal_constant(): digitString = %s id = %s str = %s \n",
               digitString ? digitString->text : "NULL", id ? id->text : "NULL", str ? str->text : "NULL");

#if !SKIP_C_ACTION_IMPLEMENTATION
     ROSE_ASSERT(str != NULL);
     ROSE_ASSERT(str->text != NULL);

  // Handle ethe case of a regular int (the kind parameter will allow for short, and long sizes)
     string value = str->text;

  // This is the Fortran default, but we need to test this since double quotes are an alterrnative
     bool usingSingleQuotes = value[0] == '\'';
     bool usingDoubleQuotes = value[0] == '\"';
     ROSE_ASSERT( usingSingleQuotes ||  usingDoubleQuotes);
     ROSE_ASSERT(!usingSingleQuotes || !usingDoubleQuotes);
  // printf ("usingSingleQuotes = %s \n",usingSingleQuotes ? "true" : "false");
  // printf ("usingDoubleQuotes = %s \n",usingDoubleQuotes ? "true" : "false");

     string subStringWithoutQuotes = value.substr(1,value.length()-2);

  // printf ("value = %s subStringWithoutQuotes = %s \n",value.c_str(),subStringWithoutQuotes.c_str());

     SgStringVal* stringValue = new SgStringVal (subStringWithoutQuotes);
     ROSE_ASSERT(stringValue != NULL);

     stringValue->set_usesSingleQuotes(usingSingleQuotes);

  // This set the start and end source position to the beginning of the number's text string)
  // setSourcePosition(stringValue);
     setSourcePosition(stringValue,str);

  // printf ("digitString = %s value = %d \n",digitString->text,value);

  // Push the integer value onto the expression stack
     astExpressionStack.push_front(stringValue);
#endif
   }

/** R428
 * logical_literal_constant
 *      : T_TRUE | T_FALSE
 *
 * @param isTrue True if logical constant is true, false otherwise
 * @param kindParam The kind parameter
 */
// void c_action_logical_literal_constant(ofp_bool isTrue, Token_t * kindParam)
void c_action_logical_literal_constant(Token_t * logicalValue_keyword, ofp_bool isTrue, Token_t * kindParam)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_logical_literal_constant(): isTrue = %s kindParam = %s \n",isTrue ? "true" : "false",kindParam ? kindParam->text : "NULL");

     SgBoolValExp* logicalValue = NULL;
     if (isTrue == true)
        {
          logicalValue = new SgBoolValExp(1);
        }
       else
        {
          logicalValue = new SgBoolValExp(0);
        }

     setSourcePosition(logicalValue,logicalValue_keyword);

     astExpressionStack.push_front(logicalValue);
   }

/**
 * R429
 * derived_type_def
 *
 */
void c_action_derived_type_def()
   {
  // I think this is the end of a derived type, but is not meaningful as far as I can tell.
   }

/** R429
 * type_param_or_comp_def_stmt
 *
 * : type_param_attr_spec T_COLON_COLON type_param_decl_list T_EOS 
 * |  component_attr_spec_list T_COLON_COLON component_decl_list T_EOS
 *
 * @param eos Token for T_EOS.
 * @param type typeParam if a typeParam. compDef if a compDef.
 */
void c_action_type_param_or_comp_def_stmt(Token_t * eos, int type)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_type_param_or_comp_def_stmt() type = %d \n",type);
   }

/** R429
 * type_param_or_comp_def_stmt
 *
 * : type_param_attr_spec T_COLON_COLON type_param_decl_list T_EOS 
 * |  component_attr_spec_list T_COLON_COLON component_decl_list T_EOS
 *
 * @param type typeParam if a typeParam. compDef if a compDef.
 */
// void c_action_type_param_or_comp_def_stmt(int type)
// void c_action_type_param_or_comp_def_stmt(Token_t *eos, int type)
void c_action_type_param_or_comp_def_stmt_list()
   {
   }

/** R430 
 * derived_type_stmt
 * 
 * T_TYPE ( ( T_COMMA type_attr_spec_list )? T_COLON_COLON )? T_IDENT
 * ( T_LPAREN generic_name_list T_RPAREN )? T_EOS
 *
 * @param label The label.
 * @param keyword The TYPE keyword token.
 * @param id The identifier.
 * @param eos End of statement token
 * @param hasTypeAttrSpecList True if a type-attr-spec-list is present.
 * @param hasGenericNameList True if a generic-name-list is present.
 */
// void c_action_derived_type_stmt(Token_t * label, Token_t * id)
void c_action_derived_type_stmt(Token_t * label, Token_t * keyword, Token_t * id, Token_t * eos, ofp_bool hasTypeAttrSpecList, ofp_bool hasGenericNameList)
   {
  // Build a SgClassDeclaration to hold the Fortran Type (maybe it should be a SgFortranType derived from a SgClassDeclaration?)

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_derived_type_stmt() label = %p id = %p \n",label,id);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R430 c_action_derived_type_stmt()");
#endif

#if !SKIP_C_ACTION_IMPLEMENTATION
     ROSE_ASSERT(id != NULL);
  // printf ("id->text = %s \n",id->text);

     SgScopeStatement* currentScope = getTopOfScopeStack();
     ROSE_ASSERT(currentScope != NULL);
     SgDerivedTypeStatement* derivedTypeStatement = buildDerivedTypeStatementAndDefinition(id->text,currentScope);

     ROSE_ASSERT(keyword != NULL);
     setSourcePosition(derivedTypeStatement,keyword);

     currentScope->append_statement(derivedTypeStatement);

     ROSE_ASSERT(derivedTypeStatement->get_definition() != NULL);
     astScopeStack.push_front(derivedTypeStatement->get_definition());
#endif

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R430 c_action_derived_type_stmt()");
#endif
   }

/** R431
 * type_attr_spec
 *
 * :  access_spec
 * |  T_EXTENDS T_LPAREN T_IDENT T_RPAREN
 * |  T_ABSTRACT
 * |  T_BIND T_LPAREN T_IDENT T_RPAREN
 *
 * @param keyword The type-attr-spec keyword token (null if an access_spec)
 * @param id Identifier if extends or bind. Otherwise, null.
 * @param specType "Enum"  on type: access_spec, extnds, abstrct, or bind. (Weird spelling of extnds and abstrct avoids overrriding java keywords.)
 */
// void c_action_type_attr_spec(Token_t * id, int specType)
void c_action_type_attr_spec(Token_t * keyword, Token_t * id, int specType)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_type_attr_spec(): keyword = %p = %s id = %p = %s specType = %d \n",keyword,keyword != NULL ? keyword->text : "NULL",id,id != NULL ? id->text : "NULL",specType);

  // If this is a access_spec then it will be handled by the R508.
     if (specType != TypeAttrSpec_access_spec)
        {
          astAttributeSpecStack.push_front(specType);

          ROSE_ASSERT(keyword != NULL);
        }

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R431 c_action_type_attr_spec()");
#endif
   }

/** R431 list
 * type_attr_spec
 * type_attr_spec_list
 *      :       type_attr_spec ( T_COMMA type_attr_spec )*
 * 
 * @param count The number of items in the list.
 */
void c_action_type_attr_spec_list__begin()
   {
  // I think there is nothing required to be done here.
   }
void c_action_type_attr_spec_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
        printf ("In c_action_type_attr_spec_list(): count = %d \n",count);
   }

/**
 * R432
 * private_or_sequence
 *
 */
void c_action_private_or_sequence()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_private_or_sequence() \n");
   }

/** R433
 * end_type_stmt
 * : (label)? T_END T_TYPE (T_IDENT)? T_EOS
 * | (label)? T_ENDTYPE (T_IDENT)? T_EOS
 * 
 * @param label The label.
 * @param id The identifier.
 */
// void c_action_end_type_stmt(Token_t * label, Token_t * id)
void c_action_end_type_stmt(Token_t *label, Token_t *endKeyword, Token_t *typeKeyword, Token_t *id, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_end_type_stmt() label = %p id = %p \n",label,id);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R433 c_action_end_type_stmt()");
#endif

#if !SKIP_C_ACTION_IMPLEMENTATION
  // Pop the class definition (SgClassDefinition) used to hold Fortran derived types
     ROSE_ASSERT(astScopeStack.empty() == false);

     resetEndingSourcePosition(astScopeStack.front(),endKeyword);

  // DQ (10/10/2010): Test ending position
     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct() != NULL);
     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct()->get_line() != astScopeStack.front()->get_startOfConstruct()->get_line());

     astScopeStack.pop_front();

  // DQ (12/27/2010): Now see if there is are types that need to be fixed up.
  // Only checking the function return type since this is the only location where a fixup
  // can be required except for the more general type rules inside of modules which are
  // handled separately (and are a bit more complex).
     fixup_possible_incomplete_function_return_type();
#endif
   }

/** R434
 * sequence_stmt
 * :    (label)? T_SEQUENCE T_EOS
 * 
 * @param label The label.
 */
// void c_action_sequence_stmt(Token_t * label)
void c_action_sequence_stmt(Token_t *label, Token_t *sequenceKeyword, Token_t *eos)
   {
  // The sequence statement is really a type attribute, so we implement it as such in ROSE.
     SgClassDefinition* classDefinition = isSgClassDefinition(astScopeStack.front());
     ROSE_ASSERT(classDefinition != NULL);
     classDefinition->set_isSequence(true);
   }

/** R436
 * type_param_decl
 *
:       T_IDENT ( T_EQUALS expr )?
 *
 * @param id Identifier equal to the parameter.
 * @param hasInit True if is initialized.
 */
void c_action_type_param_decl(Token_t * id, ofp_bool hasInit)
{
}

/** R436 list
 * type_param_decl
 * type_param_decl_list
 *      :       type_param_decl ( T_COMMA type_param_decl )*
 * 
 * @param count The number of items in the list.
 */
void c_action_type_param_decl_list__begin()
{
}
void c_action_type_param_decl_list(int count)
{
}

/** R439
 * component_def_stmt
 *
 * :  data_component_def_stmt
 * |  proc_component_def_stmt
 *
 * @param type Type of definition: data or procedure.
 */
void c_action_component_def_stmt(int type)
   {
  // This rule only provides a type value to identify the declaration as data or
  // as a procedure. So I don't know if it is of much use in building the ROSE AST.

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_component_def_stmt(): type = %d \n",type);

  // printf ("astNameStack.size() = %ld \n",astNameStack.size());
   }

/** R440
 *      data_component_def_stmt
 * :    (label)? declaration_type_spec 
 *              ( ( T_COMMA component_attr_spec_list {hasSpecList=true;})?
 *              T_COLON_COLON )? component_decl_list T_EOS
 * 
 * @param label The label.
 * @param hasSpecList Boolean true if has a component_attr_spec(_list).
 */
// void c_action_data_component_def_stmt(Token_t * label, ofp_bool hasSpec)
void c_action_data_component_def_stmt(Token_t *label, Token_t *eos, ofp_bool hasSpec)
   {
  // This is where we build the declaration using whatever was pushed onto the stack (nameStack)

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_data_component_def_stmt(): label = %p hasSpec = %s \n",label,hasSpec ? "true" : "false");

#if !SKIP_C_ACTION_IMPLEMENTATION
  // printf ("astNameStack.size() = %zu \n",astNameStack.size());

  // DQ (1/28/2009): The variable declaration may have already been built (if so then the stacks are empty).
     if (astNodeStack.empty() == false && astBaseTypeStack.empty() == false)
        {
          SgVariableDeclaration* variableDeclaration = buildVariableDeclaration(label,false);

          ROSE_ASSERT(variableDeclaration->get_file_info()->isCompilerGenerated() == false);

       // Attach the variableDeclaration to the current scope
          SgScopeStatement* currentScope = getTopOfScopeStack();
          ROSE_ASSERT(currentScope->variantT() == V_SgClassDefinition);
          currentScope->append_statement(variableDeclaration);
        }
#endif
   }

/** R441
 * component_attr_spec
 *
 * :  T_POINTER
 * |  T_DIMENSION T_LPAREN component_array_spec T_RPAREN
 * |  T_DIMENSION T_LBRACKET co_array_spec T_RBRACKET
 * |  T_ALLOCATABLE
 * |  access_spec
 * |  T_KIND
 * |  T_LEN
 *
 * @param specType  Type of spec in enum form: pointer, dimension_paren,  dimension_bracket, allocable, access_spec, kind, or len.
 */
// void c_action_component_attr_spec(int specType)
void c_action_component_attr_spec(Token_t * attrKeyword, int specType)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_component_attr_spec(): attrKeyword = %p = %s specType = %d \n",attrKeyword,attrKeyword != NULL ? attrKeyword->text : "NULL",specType);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R441 c_action_component_attr_spec()");
#endif

     switch(specType)
        {
          case ComponentAttrSpec_pointer:
             {
               if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                    printf ("found a ComponentAttrSpec_pointer spec \n");
#if 0
            // DQ (1/20/2011): These are not processed until after they are accumulated, so that the base 
            // types will be properly handled for array of pointers, and pointers of arrays, etc.

            // DQ (8/29/2010): Handling multiple type-attr-specs requires checking the astTypeStack 
            // since that is where the results of previous type-attr-specs are pushed.
            // DQ (2/1/2009): This is the earliest point to change this.
            // convertBaseTypeOnStackToPointer();
               if (astTypeStack.empty() == false)
                  {
                 // Clear the astBaseTypeStack and push value on the top of astTypeStack 
                 // onto the astBaseTypeStack, and pop the astTypeStack. This is the case of
                 // "type (io_dim), dimension(:), pointer :: field_dim" (see test2010_35.f90).
                    SgType* previouslyBuiltType = astTypeStack.front();
                    astTypeStack.pop_front();
                    ROSE_ASSERT(astBaseTypeStack.empty() == false);
                 // SgType* previousBaseType = astBaseTypeStack.front();
                    astBaseTypeStack.pop_front();
                    astBaseTypeStack.push_front(previouslyBuiltType);
                  }
               convertBaseTypeOnStackToPointer();
#endif
               break;
             }

#if ROSE_OFP_MINOR_VERSION_NUMBER == 7
       // DQ (4/5/2010): These have been removed from OFP 0.8.0
          case ComponentAttrSpec_dimension_paren:
             {
               if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                    printf ("found a ComponentAttrSpec_dimension_paren spec \n");
               break;
             }

          case ComponentAttrSpec_dimension_bracket:
             {
               if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                    printf ("found a ComponentAttrSpec_dimension_bracket spec \n");
               break;
             }
#endif
          case ComponentAttrSpec_allocatable:
             {
               if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                    printf ("found a ComponentAttrSpec_allocatable spec \n");
               break;
             }

          case ComponentAttrSpec_kind:
             {
               if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                    printf ("found a ComponentAttrSpec_kind spec \n");
               break;
             }

          case ComponentAttrSpec_len:
             {
               if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                    printf ("found a ComponentAttrSpec_len spec \n");
               break;
             }

          case ComponentAttrSpec_access_spec:
             {
               if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                    printf ("found a ComponentAttrSpec_access_spec spec \n");
               break;
             }

#if ROSE_OFP_MINOR_VERSION_NUMBER > 7
          case ComponentAttrSpec_codimension:
             {
               if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                    printf ("found a ComponentAttrSpec_codimension spec \n");
               break;
             }

          case ComponentAttrSpec_contiguous:
             {
               if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                    printf ("found a ComponentAttrSpec_contiguous spec \n");
               break;
             }

          case ComponentAttrSpec_dimension:
             {
               if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                    printf ("found a ComponentAttrSpec_dimension spec \n");
               break;
             }
#endif

          default:
             {
               printf ("default reached in c_action_component_attr_spec() specType = %d \n",specType);
               ROSE_ASSERT(false);
             }
        }

  // Push the attribue onto the stack (e.g. dimension)
     astAttributeSpecStack.push_front(specType);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R441 c_action_component_attr_spec()");
#endif
   }

/** R441 list
 * component_attr_spec
 * component_attr_spec_list
 *      :       component_attr_spec ( T_COMMA component_attr_spec )*
 * 
 * @param count The number of items in the list.
 */
void c_action_component_attr_spec_list__begin()
   {
  // I think this is the correct assertion, but the rest of this rule is not implemented!

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R441 c_action_component_attr_spec_list__begin()");
#endif

  // DQ (1/20/2011): We have to save the astAttributeSpecStack so that we can use the same attribute 
  // stack for processing all variables in the declaration uniformally.  See test2007_248.f90 for an 
  // example.  We will also have to test this support in R441 as well (similar parser action, but for 
  // variable declarations inside of types.
  // printf ("Save the astAttributeSpecStack stack to reuse it for each variable in the declaration. \n");

  // OR just don't delete the attributes until we process "c_action_component_attr_spec_list(int count)"!!!

  // Nothing to do here since we keep a stack and handle items pushed on the the stack later.
  // ROSE_ASSERT(astNameListStack.empty() == true);
     if (astNameStack.empty() == false)
        {
          printf ("Warning, astNameStack.size() = %zu \n",astNameStack.size());
        }
     ROSE_ASSERT(astNameStack.empty() == true);

     if (astTypeStack.empty() == false)
        {
          if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
             {
               printf ("Warning, astTypeStack.size() = %zu \n",astTypeStack.size());
             }
        }

     ROSE_ASSERT(astBaseTypeStack.empty() == false);
   }

void c_action_component_attr_spec_list(int count)
   {
  // This function R441 is similar to R504 R503-F2008 but is used for declarations in types.

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_component_attr_spec_list(): count = %d (building the SgArrayType) astAttributeSpecStack.size() = %zu \n",count,astAttributeSpecStack.size());

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R441 c_action_component_attr_spec_list()");
#endif
  // printf ("astBaseTypeStack.empty() = %s \n",astBaseTypeStack.empty() ? "true" : "false");

  // DQ (12/27/2007): parameters associated with variable declaration type attributes (e.g. dimention attribute) should not be used yet. But I don't know how to avoid it!
  // if (astBaseTypeStack.empty() == true || astAttributeSpecStack.empty() == false)

  // printf ("In R441 c_action_component_attr_spec_list(): calling c_action_array_spec() \n");
  // c_action_array_spec(count);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R441 c_action_component_attr_spec_list()");
#endif
   }

/** R442
 * component_decl
 *
 * :  T_IDENT ( T_LPAREN component_array_spec T_RPAREN )?
 *   ( T_LBRACKET co_array_spec T_RBRACKET )?
 *   ( T_ASTERISK char_length )? ( component_initialization )?
 *
 * @param id Component identifier.
 * @param hasComponentArraySpec True if has component array spec.
 * @param hasCoArraySpec True if has coarray spec.
 * @param hasCharLength True if has char length.
 * @param hasComponentInitialization True if has component initialization.
 */
void c_action_component_decl(Token_t * id, 
        ofp_bool hasComponentArraySpec, ofp_bool hasCoArraySpec, 
        ofp_bool hasCharLength, ofp_bool hasComponentInitialization)
   {
  // Build each variable and append it to the current scope!

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_component_decl() id = %p hasComponentArraySpec = %s hasCoArraySpec = %s hasCharLength = %s hasComponentInitialization = %s \n",
               id,hasComponentArraySpec ? "true" : "false", hasCoArraySpec ? "true" : "false", 
               hasCharLength ? "true" : "false", hasComponentInitialization ? "true" : "false");

#if !SKIP_C_ACTION_IMPLEMENTATION

#if ROSE_OFP_MINOR_VERSION_NUMBER >= 8 & ROSE_OFP_PATCH_VERSION_NUMBER >= 0
  // DQ (1/23/2011): Pass the actual parameters (required for test2011_20.f90).
  // Calling R504, R503-F2008
  // c_action_entity_decl(id,false,false,false,false);
     c_action_entity_decl(id,hasComponentArraySpec,hasCoArraySpec,hasCharLength,hasComponentInitialization);
#else
     c_action_entity_decl(id);
#endif

#endif
   }

/** R442 R438-F2008 list
 * component_decl
 * component_decl_list
 *      :       component_decl ( T_COMMA component_decl )*
 * 
 * @param count The number of items in the list.
 */
void c_action_component_decl_list__begin()
   {
  // No action is required here because the scope for the derived type has been defined in R403

  // Nothing to do here since we keep a stack and handle items pushed on the the stack later.
  // ROSE_ASSERT(astNameListStack.empty() == true);

  // printf ("We need semantics here corresponding to that in R503-F2008??? \n");
     convertBaseTypeToArrayWhereAppropriate();

  // DQ (8/28/2010): This is not an error, but we might want to handle it better.
  // ROSE_ASSERT(astNameStack.empty() == true);
   }

void c_action_component_decl_list(int count)
   {
  // This function R442 R438-F2008 is similar to R504 R503-F2008

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
        printf ("R442 R438-F2008 c_action_component_decl_list(): count = %d \n",count);

  // printf ("astNameStack.size() = %zu count = %d \n",astNameStack.size(),count);
  // ROSE_ASSERT(astNameStack.size() == (size_t)count);
  // printf ("astNodeStack.size() = %zu count = %d \n",astNodeStack.size(),count);
     ROSE_ASSERT(astNodeStack.size() == (size_t)count);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R442 c_action_component_decl_list()");
#endif

  // DQ (1/20/2011): Refactored the code below so it could be called from R443 as well as R504.
     bool hasArraySpec      = false;
     bool hasInitialization = false;
  // printf ("In R442 R438-F2008 calling processAttributeSpecStack(false,false): astAttributeSpecStack.size() = %zu \n",astAttributeSpecStack.size());
     processAttributeSpecStack(hasArraySpec,hasInitialization);

  // DQ (1/28/2009): This is called for variable declarations in structures.
     ROSE_ASSERT (astNodeStack.empty() == false && astBaseTypeStack.empty() == false);

  // DQ (1/28/2009): Refactored this code so it could be called in R442, R501, R504 and R1238.
  // Note that there is no token for this rule, so we pass in a NULL pointer.
     buildVariableDeclarationAndCleanupTypeStack(NULL);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R442 c_action_component_decl_list()");
#endif
   }

/** R443
 * component_array_spec
 *
 * :  explicit_shape_spec_list
 * |  deferred_shape_spec_list
 *
 * @param isExplicit True if this is an explicit shape spec list. false if it is a deferred shape spec list.
 */
void c_action_component_array_spec(ofp_bool isExplicit)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_component_array_spec(): isExplicit = %s \n",isExplicit ? "true" : "false");

  // DQ (2/23/2008): This is allowed, see test2008_20.f90; explicitly dementioned array in a type.
  // DQ (3/2/2008): This is marked as isExplicit == false even when 
  // the ":" operator is explicit. Test if it is ever true.
  // ROSE_ASSERT(isExplicit == false);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R443 c_action_component_array_spec()");
#endif

  // DQ (1/23/2011): This might be better put into R443 c_action_deferred_shape_spec_list(int count), so that count would be available.
  // DQ (1/18/2011): Called by R510 and R443.
     int count = 1;
     processMultidimensionalSubscriptsIntoExpressionList(count);

  // DQ (1/17/2011): Push the AttrSpec_DIMENSION attribute only the stack to trigger this to be handled as an array (build an array type).
  // printf ("In R443 c_action_component_array_spec(): Push the ComponentAttrSpec_dimension attribute only the stack to trigger this to be handled as an array (build an array type). \n");
  // astAttributeSpecStack.push_front(AttrSpec_DIMENSION);
     astAttributeSpecStack.push_front(ComponentAttrSpec_dimension);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R443 c_action_component_array_spec()");
#endif
   }

/** R443 list
 * deferred_shape_spec_list
 *              T_COLON {count++;} ( T_COMMA T_COLON {count++;} )*
 * 
 * @param count The number of items in the list.
 */
void c_action_deferred_shape_spec_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_deferred_shape_spec_list__begin() \n");
   }

void c_action_deferred_shape_spec_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_deferred_shape_spec_list(): count = %d \n",count);

  // At this point we know that this is a deferred shape specification 
  // and that the dimension of the arrya is given by count.

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R443 list c_action_deferred_shape_spec_list()");
#endif

     ROSE_ASSERT(astExpressionStack.empty() == true);
     for (int i=0; i < count; i++)
        {
          SgColonShapeExp* colonExp = new SgColonShapeExp();
          astExpressionStack.push_front(colonExp);

       // We don't have the source position of the ":" expression.
          setSourcePosition(colonExp);
        }

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R443 list c_action_deferred_shape_spec_list()");
#endif
   }

/** R445
 *      proc_component_def_stmt
 * :    (label)? T_PROCEDURE T_LPAREN (proc_interface)? T_RPAREN T_COMMA
 *              proc_component_attr_spec_list T_COLON_COLON proc_decl_list T_EOS
 * 
 * @param label The label.
 * @param hasInterface Boolean true if has a nonempty interface.
 */
// void c_action_proc_component_def_stmt(Token_t * label, ofp_bool hasInterface)
void c_action_proc_component_def_stmt(Token_t *label, Token_t *procedureKeyword, Token_t *eos, ofp_bool hasInterface)
{
}

/** R446
 * proc_component_attr_spec
 *
 * :  T_POINTER
 * |  T_PASS ( T_LPAREN T_IDENT T_RPAREN {id=$T_IDENT;} )?
 * |  T_NOPASS
 * |  access_spec
 *
 * @param id Identifier if present in pass. 
 * @param specType  "Enum" to specify type of spec: pointer, pass, nopass, or access_spec
 */
// void c_action_proc_component_attr_spec(Token_t * id, int specType)
void c_action_proc_component_attr_spec(Token_t * attrSpecKeyword, Token_t * id, int specType)
{
}

/** R446 list
 * proc_component_attr_spec_list
 *              proc_component_attr_spec ( T_COMMA proc_component_attr_spec )*
 * 
 * @param count The number of items in the list.
 */
void c_action_proc_component_attr_spec_list__begin()
{
}
void c_action_proc_component_attr_spec_list(int count)
{
}

/** R447
 * private_components_stmt
 * :    (label)? T_PRIVATE T_EOS
 * 
 * @param label The label.
 */
// void c_action_private_components_stmt(Token_t * label)
void c_action_private_components_stmt(Token_t *label, Token_t *privateKeyword, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_private_components_stmt(): privateKeyword = %p = %s \n",privateKeyword,privateKeyword != NULL ? privateKeyword->text : "NULL");

  // The private statement (in this context) is really a type attribute, so we implement it as such in ROSE.
     SgClassDefinition* classDefinition = isSgClassDefinition(astScopeStack.front());
     ROSE_ASSERT(classDefinition != NULL);
     classDefinition->set_isPrivate(true);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R447 c_action_private_components_stmt()");
#endif
   }

/** R448
 * type_bound_procedure_part
 *
 * :  T_CONTAINS  T_EOS ( binding_private_stmt )? proc_binding_stmt 
 *                      ( proc_binding_stmt )*
 *
 * @param count  Number of procedure binding statements.
 * @param hasBindingPrivateStmt True if has a keyword "private".
 */
// void c_action_type_bound_procedure_part(int count, ofp_bool hasBindingPrivateStmt)
// void c_action_type_bound_procedure_part(Token_t * containsKeyword, Token_t * eos, int count, ofp_bool hasBindingPrivateStmt) 
void c_action_type_bound_procedure_part(int count, ofp_bool hasBindingPrivateStmt) 
   {
  // printf ("In c_action_type_bound_procedure_part(): containsKeyword = %p = %s hasBindingPrivateStmt = %s \n",containsKeyword,containsKeyword != NULL ? containsKeyword->text : "NULL",hasBindingPrivateStmt ? "true" : "false");

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_type_bound_procedure_part(): count = %d hasBindingPrivateStmt = %s \n",count,hasBindingPrivateStmt ? "true" : "false");

     printf ("Is this used! \n");
     ROSE_ASSERT(false);
   }

/** R449
 * binding_private_stmt
 * :    (label)? T_PRIVATE T_EOS
 * 
 * @param label The label.
 */
// void c_action_binding_private_stmt(Token_t * label)
void c_action_binding_private_stmt(Token_t *label, Token_t *privateKeyword, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_binding_private_stmt() \n");
   }

/** R450
 * proc_binding_stmt
 * :    (label)? specific_binding T_EOS
 * |    (label)? generic_binding T_EOS
 * |    (label)? final_binding T_EOS
 * 
 * @param label The label.
 */
// void c_action_proc_binding_stmt(Token_t * label, int type)
void c_action_proc_binding_stmt(Token_t *label, int type, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("c_action_proc_binding_stmt(): type = %d \n",type);
   }

/** R451
 * specific_binding
 *
 * : T_PROCEDURE ( T_LPAREN T_IDENT T_RPAREN )?
 *        ( ( T_COMMA binding_attr_list )? T_COLON_COLON )?
 *        T_IDENT ( T_EQ_GT T_IDENT )?
 *
 * @param interfaceName Optional interface name.
 * @param bindingName Required binding name.
 * @param procedureName Optional procedure name.
 * @param hasBindingAttributeList True if has a binding-attr-list.
 */

/** R452
 * generic_binding
 *
 * :  T_GENERIC ( T_COMMA access_spec )? T_COLON_COLON 
 *              generic_spec T_EQ_GT generic_name_list
 *
 * @param hasAccessSpec True if has public or private access spec.
 */
// void c_action_generic_binding(ofp_bool hasAccessSpec)
void c_action_generic_binding(Token_t * genericKeyword, ofp_bool hasAccessSpec)
{
}

/** R453
 * binding_attr
 *      : T_PASS ( T_LPAREN T_IDENT T_RPAREN )?
 *      | T_NOPASS
 *      | T_NON_OVERRIDABLE
 *      | T_DEFERRED
 *      | access_spec
 *
 * @param attr The binding attribute.
 * @param id Optional identifier in pass attribute.
 */
// void c_action_binding_attr(int attr, Token_t * id)
void c_action_binding_attr(Token_t * bindingAttr, int attr, Token_t * id)
{
}

/** R453 list
 * binding_attr_list
 * 
 *              binding_attr ( T_COMMA binding_attr )*
 *
 * @param count The number of items in the list.
 */
void c_action_binding_attr_list__begin()
{
}
void c_action_binding_attr_list(int count)
{
}

/** R455
 * derived_type_spec
 *      : T_IDENT ( T_LPAREN type_param_spec_list T_RPAREN )?
 *
 * @param typeName The name of the derived type or class.
 * @param hasTypeParamSpecList True if type-param-spec-list is present.
 */
void c_action_derived_type_spec(Token_t * typeName, ofp_bool hasTypeParamSpecList)
   {
  // If a type is being specified and it is a derived type, the name is seen here.
  // The type should have already been constructed within the AST, if it has been 
  // declared, ans so we only have to find it in the declaration which we can find 
  // by searching the symbol tables.  The current implementation of this does not 
  // handle USE statements.
  // printf ("In c_action_derived_type_spec(): typeName = %p hasTypeParamSpecList = %s \n",typeName,hasTypeParamSpecList ? "true" : "false");

     ROSE_ASSERT(typeName != NULL);
     ROSE_ASSERT(typeName->text != NULL);

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_derived_type_spec(): typeName->text = %s hasTypeParamSpecList = %s \n",typeName->text,hasTypeParamSpecList ? "true" : "false");

#if !SKIP_C_ACTION_IMPLEMENTATION
     SgName derivedTypeName = typeName->text;
     SgClassSymbol* derivedTypeSymbol = trace_back_through_parent_scopes_lookup_derived_type_symbol( derivedTypeName, getTopOfScopeStack() );

  // DQ (8/29/2010): Added support for where the referenced type has not been seen yet and must be fixed up later (see R1106: c_action_end_module_stmt()).
     SgType* derivedType = NULL;
     if (derivedTypeSymbol == NULL)
        {
       // Case of where we are referencing a type that we have not seen yet in the source sequence.
       // We will have to make up something (perhaps using SgDefaultType) and then fix it up at the end of the module.
          if ( SgProject::get_verbose() > 0 )
               printf ("Warning: type referenced has not been seen in the source sequence yet, using SgTypeDefault (will be fixed up at the end of the module processing). \n");

       // DQ (12/26/2010): Use SgTypeDefault to trigger that we don't know the type and that it will
       // have to be fixed up later (e.g. function return types when the type is in the use statement 
       // defines the scope where the type is declarted).
       // DQ (9/6/2010): Fix this later!
       // derivedType = SgTypeDefault::createType();
       // derivedType = SgTypeInt::createType();
          derivedType = SgTypeDefault::createType(derivedTypeName);
          ROSE_ASSERT(derivedType != NULL);

       // Set the name of the type (but this is a shared type!!!).
       // derivedType->set_name(derivedTypeName.str());
        }
       else
        {
       // This is the more common case (referenced type has been seen previously).
          ROSE_ASSERT(derivedTypeSymbol != NULL);
          derivedType = derivedTypeSymbol->get_type();
          ROSE_ASSERT(derivedType != NULL);
        }

     ROSE_ASSERT(derivedType != NULL);

  // Save the type on the type stack!  We will use it in the construction of the variable declaration.
     astTypeStack.push_front(derivedType);
#endif
   }

/** R456
 * type_param_spec
 *
 * : (keyword T_EQUALS)? type_param_value
 *
 * @param keyWord Keywod if of the form kw = foo. Null otherwise.
 */
void c_action_type_param_spec(Token_t * keyWord)
{
}

/** R456 list
 * type_param_spec_list
 *              type_param_spec ( T_COMMA type_param_spec )*
 *
 * @param count The number of items in the list.
 */
void c_action_type_param_spec_list__begin()
{
}
void c_action_type_param_spec_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_type_param_spec_list() count = %d \n",count);

  // This action is deminstrated in test2011_26.f03

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R456 c_action_type_param_spec_list()");
#endif

  // DQ (1/24/2011): Delete an entry from the stack to avoid unset parent in testGraphGeneration.
  // This is a temp fix for test2011_26.f03 since we have no full implementation for 
  // type kind handling in data member initializers.  This is part of an outstanding
  // question to Craig about the OFP handling in this case.
     if (astTypeParameterStack.empty() == false)
        {
          SgExpression* lengthValue = astTypeParameterStack.front();
          astTypeParameterStack.pop_front();
          delete lengthValue;
          lengthValue = NULL;
        }

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }

/** R458 list
 * component_spec_list
 *              component_spec ( T_COMMA component_spec )*
 *
 * @param count The number of items in the list.
 */
void c_action_component_spec_list__begin()
{
}
void c_action_component_spec_list(int count)
{
}

/** R460
 * enum_def
 *
 * :  enum_def_stmt enumerator_def_stmt (enumerator_def_stmt)*
 *        end_enum_stmt
 *
 * @param numEls Number of elements in the enum.
 */
void c_action_enum_def(int numEls)
{
}

/** R461
 * enum_def_stmt
 *      :       (label)? T_ENUM T_COMMA T_BIND T_LPAREN T_IDENT T_RPAREN T_EOS
 *
 * @param label The label.
 * @param id The identifier.
 */
// void c_action_enum_def_stmt(Token_t * label, Token_t * id)
void c_action_enum_def_stmt(Token_t *label, Token_t *enumKeyword, Token_t *bindKeyword, Token_t *id, Token_t *eos)
{
}

/** R462
 * enumerator_def_stmt
:       (label)? T_ENUMERATOR ( T_COLON_COLON )? enumerator_list T_EOS
 * @param label The label.
 */
// void c_action_enumerator_def_stmt(Token_t * label)
void c_action_enumerator_def_stmt(Token_t *label, Token_t *enumeratorKeyword, Token_t *eos)
{
}

/** R463 list
 * enumerator_list
 *              enumerator ( T_COMMA enumerator )*
 *
 * @param count The number of items in the list.
 */
void c_action_enumerator_list__begin()
{
}
void c_action_enumerator_list(int count)
{
}

/** R464
 * end_enum_stmt
 * :    (label)? T_END T_ENUM T_EOS
 * |    (label)? T_ENDENUM T_EOS
 *
 * @param label The label.
 */
// void c_action_end_enum_stmt(Token_t * label)
void c_action_end_enum_stmt(Token_t *label, Token_t *endKeyword, Token_t *enumKeyword, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_end_enum_stmt() label = %p id = %p \n",label,endKeyword);

     ROSE_ASSERT(astScopeStack.empty() == false);

  // DQ (10/10/2010): Test ending position
     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct() != NULL);
     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct()->get_line() != astScopeStack.front()->get_startOfConstruct()->get_line());
   }

/** R465
 * array_constructor
 *      :       T_LPAREN T_SLASH ac_spec T_SLASH T_RPAREN
 *      |       T_LBRACKET ac_spec T_RBRACKET
 */
void c_action_array_constructor()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_array_constructor() \n");

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R465 c_action_array_constructor()");
#endif

     SgExprListExp* expressionList = new SgExprListExp();
     setSourcePosition(expressionList);
     while(astExpressionStack.empty() == false && isSgExprListExp(astExpressionStack.front()) != NULL)
        {
          SgExpression* initializer = astExpressionStack.front();
          astExpressionStack.pop_front();

          if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
               printf ("Building SgAggregateInitializer elements initializer = %s \n",initializer->class_name().c_str());

          ROSE_ASSERT(initializer != NULL);
          expressionList->append_expression(initializer);
        }

  // Use a SgAggregateInitializer
     if (expressionList->get_expressions().empty() == false)
        {
          SgAggregateInitializer* initializer = new SgAggregateInitializer(expressionList, NULL);
          setSourcePosition(initializer);

          astExpressionStack.push_front(initializer);
        }
       else
        {
          if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
               printf ("Need to cleanup construction of unused expressionList \n");
        }

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R465 c_action_array_constructor()");
#endif
   }

/**
 * R466
 * ac_spec
 *
 */
void c_action_ac_spec()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_ac_spec() \n");
   }

/**
 * R469
 * ac_value
 *
 */
void c_action_ac_value()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_ac_value() \n");

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("In of R469 c_action_ac_value()");
#endif

  // DQ (21/4/2008): Move the value from the astExpressionStack to the astInitializerStack
     ROSE_ASSERT(astExpressionStack.empty() == false);
   }

/** R469 list
 * ac_value_list
 *              ac_value ( T_COMMA ac_value )*
 *
 * @param count The number of items in the list.
 */
void c_action_ac_value_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_ac_value_list__begin() \n");

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R469 list c_action_ac_value_list__begin()");
#endif

  // DQ (1/17/2011): If we get there and there is an expression on the stack then 
  // we should use it to convert the type into an array type.
  // printf ("MAYBE THIS IS WHERE THE TYPE SHOULD BE CONSTRUCTED AS A LAST RESORT (should be reading the attribute stack!) \n");

  // DQ (1/17/2011): If there is an expression on the astExpressionStack then it is used to convert 
  // the type in astTypeStack into an array.  See test2007_94.f90 for where this is required!
  // Note that an alternative might be to check if the entry on the top of the astExpressionStack is a SgExprListExp...
  // Error: The type should have been built in R504 R503-F2008 c_action_entity_decl(), since this is too late to build the type.
  // if (astExpressionStack.empty() == false)
     if ( (astExpressionStack.empty() == false) && (isSgExprListExp(astExpressionStack.front()) != NULL) )
        {
          printf ("Use the SgExprListExp to convert the type in astTypeStack into an array. \n");

       // DQ (1/22/2011): This is not required for executable statements (where the astAttributeSpecStack 
       // will be empty.  See test2010_49.f90 for an example (e.g. "localCount = (/ 2, 3, 5, 7 /)").
       // Verify that the attribute stack has an entry to tell us what to do (but build an array for now!
       // ROSE_ASSERT(astAttributeSpecStack.empty() == false);
          if (astAttributeSpecStack.empty() == false)
             {
            // We don't know the count yet, not clear where we get that (assume 1 for initial test).
            // Note that this parameter is no longer used, so it should be removed!
            // printf ("REMOVE USE OF count PARAMETER IN convertTypeOnStackToArrayType() \n");

            // Note that this is a different count than then one in the function parameter list for this function.
               int temp_count = 1;
               SgArrayType* arrayType = convertTypeOnStackToArrayType(temp_count);
               ROSE_ASSERT(arrayType != NULL);

            // We might want this to be pushed onto the astBaseTypeStack!
            // astTypeStack.push_front(arrayType);

            // DQ (1/23/2011): Put the new type onto the astTypeStack instead of the astBaseTypeStack.
            // ROSE_ASSERT(astTypeStack.empty() == true);
               if (astTypeStack.empty() == false)
                  {
                 // See test2011_26.f03 for en exmple of where this is required.
                    astTypeStack.pop_front();
                  }
               ROSE_ASSERT(astTypeStack.empty() == true);
               astTypeStack.push_front(arrayType);

            // DQ (1/20/2011): Added test before we pop the entry off the stack.
               ROSE_ASSERT(astAttributeSpecStack.front() == AttrSpec_DIMENSION || astAttributeSpecStack.front() == ComponentAttrSpec_dimension);
               astAttributeSpecStack.pop_front();
             }

#if 1
       // Output debugging information about saved state (stack) information.
          outputState("After converion of type to array type in R469 (list__begin) c_action_ac_value_list__begin()");
#endif
        }

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R469 list c_action_ac_value_list__begin()");
#endif
   }

void c_action_ac_value_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_ac_value_list(): count = %d \n",count);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R469 list c_action_ac_value_list()");
#endif

  // DQ (1/17/2011): I now think that MAYBE this should be handling the initialization instead of the R504 parser action!

  // Build a SgExprListExp and put the elements on the astInitializerStack into the stack and then put the SgExprListExp on the stack.
  // DQ (5/4/2008): switching back from using astInitializerStack to using astExpressionStack.
     if (count > 1)
        {
          SgExprListExp* expressionList = new SgExprListExp();
          setSourcePosition(expressionList);
          for (int i=0; i < count; i++)
             {
               ROSE_ASSERT(astExpressionStack.empty() == false);
               SgExpression* exp = astExpressionStack.front();
               astExpressionStack.pop_front();
            // expressionList->append_expression(exp);
               expressionList->prepend_expression(exp);
             }
          astExpressionStack.push_front(expressionList);
        }
       else
        {
          ROSE_ASSERT(astExpressionStack.empty() == false);
        }

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R469 list c_action_ac_value_list()");
#endif
   }

/** R470
 * ac_implied_do
 *
 * :  T_LPAREN ac_value_list T_COMMA ac_implied_do_control T_RPAREN
 *
 */
void c_action_ac_implied_do()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_ac_implied_do() \n");

  // DQ (9/26/2010): Within the statement: real, dimension (3) :: b = (/(j+i,j=1,3)/)
  // "(j+i,j=1,3)" is the ac_implied_do expression.
  // "j+i" is the ac-value-list
  // "j=1" is the ac-do-variable = scalar-int-exp.
  // and "3" is another scalar-int-exp (interpreted as the upper bound)
  // A current problem is that OFP is not providing the ac-do-variable (this is getting fixed).
  // This similar problem happens for the io-implied-do rule (both are being fixed uniformally in OFP).
  // When this if fixed in OFP:
  //    1) What is now the doLoopVarExp, will be the first element of the object_list.
  //    2) We can remove the doLoopVarExp from the IR and fix the unparser.

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R470 list c_action_ac_implied_do()");
#endif

  // Since we might have an increment, and that information is only available in R471, we 
  // have to process the loop control information in R471 and pass it on the stack to R470.
     ROSE_ASSERT(astExpressionStack.empty() == false);
     SgExprListExp* loopControl = isSgExprListExp(astExpressionStack.front());

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At MIDDLE #1a of R470 list c_action_ac_implied_do()");
#endif

     ROSE_ASSERT(loopControl != NULL);
     astExpressionStack.pop_front();

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At MIDDLE #1b of R470 list c_action_ac_implied_do()");
#endif

  // There should be at most 3 elements in the loopControl (built by R471):
     SgExpressionPtrList & expressionList = loopControl->get_expressions();

     SgExpression* increment = NULL;

  // Now that we setup a SgExprListExp as a loop control (see R471),
  // we always generate all three SgExpression objects.
     ROSE_ASSERT(expressionList.size() == 3);

     increment = expressionList.back();
     expressionList.pop_back();

     ROSE_ASSERT(expressionList.empty() == false);
     SgExpression* upperBound = expressionList.back();
     expressionList.pop_back();

     ROSE_ASSERT(expressionList.empty() == false);
     SgExpression* lowerBound = expressionList.back();
     expressionList.pop_back();
     ROSE_ASSERT(expressionList.empty() == true);

  // We used a SgExprList as a container of SgExpression objects, not delete the container.
     delete loopControl;
     loopControl = NULL;

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At MIDDLE #1c of R470 list c_action_ac_implied_do()");
#endif

     SgExpression* doLoopVarExp = lowerBound;

  // These are not required!
  // SgExpression* increment   = new SgNullExpression();

  // There is only an empty objectList in a SgImpliedDo used in a constructor.
     SgExprListExp* objectList = new SgExprListExp();
  // setSourcePosition(increment);
     setSourcePosition(objectList);

  // ROSE_ASSERT(astExpressionStack.empty() == false);
  // SgExpression* variableReference = astExpressionStack.front();
  // astExpressionStack.pop_front();

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At MIDDLE #2 of R470 list c_action_ac_implied_do()");
#endif

  // DQ (10/9/2010): This should be empty at this point.
     ROSE_ASSERT(objectList->get_expressions().empty() == true);

     ROSE_ASSERT(astExpressionStack.empty() == false);
     SgExpression* objectListEntry = astExpressionStack.front();
     astExpressionStack.pop_front();

     objectList->append_expression(objectListEntry);

  // DQ (10/9/2010): Not clear if we can assert this!
  // ROSE_ASSERT(astExpressionStack.empty() == true);

#if 0
     printf ("objectList   = %p = %s \n",objectList,objectList->class_name().c_str());
     printf ("doLoopVarExp = %p = %s \n",doLoopVarExp,doLoopVarExp->class_name().c_str());
     printf ("upperBound   = %p = %s \n",upperBound,upperBound->class_name().c_str());
     printf ("increment    = %p = %s \n",increment,increment->class_name().c_str());
#endif

  // SgImpliedDo* impliedDo = new SgImpliedDo(doLoopVar,lowerBound,upperBound,increment,objectList);
  // SgImpliedDo* impliedDo = new SgImpliedDo(variableReference,doLoopVar,lowerBound,upperBound,increment,objectList);
  // SgImpliedDo* impliedDo = new SgImpliedDo(variableReference,doLoopVar,lowerBound,upperBound,increment,objectList);
     SgScopeStatement* implied_do_scope = NULL; // new SgBasicBlock();
  // SgImpliedDo* impliedDo = new SgImpliedDo(doLoopVarExp,lowerBound,upperBound,increment,objectList);
  // SgImpliedDo* impliedDo = new SgImpliedDo(doLoopVarExp,lowerBound,upperBound,increment,objectList,implied_do_scope);
     SgImpliedDo* impliedDo = new SgImpliedDo(doLoopVarExp,upperBound,increment,objectList,implied_do_scope);
     setSourcePosition(impliedDo);

     objectList->set_parent(impliedDo);
     doLoopVarExp->set_parent(impliedDo);
     upperBound->set_parent(impliedDo);
  // lowerBound->set_parent(impliedDo);
     increment->set_parent(impliedDo);

  // DQ (4/21/2008): We want to use the astInitializerStack for initialization purposes, actually the R469 ac-value
  // will be called and it should transfer the implicit do-loop from the astExpressionStack to the 
  // astInitializerStack.
  // astExpressionStack.push_front(impliedDo);
  // astInitializerStack.push_front(impliedDo);
     astExpressionStack.push_front(impliedDo);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R470 list c_action_ac_implied_do()");
#endif
   }

/** R471
 * ac_implied_do_control
 *
 * :  T_IDENT T_EQUALS expr T_COMMA expr (T_COMMA expr)?
 *
 * @param hasStride True if is of the form a = b,c,d
 */
void c_action_ac_implied_do_control( ofp_bool hasStride )
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
        printf ("In c_action_ac_implied_do_control(): hasStride = %s \n",hasStride ? "true" : "false");

  // Pull off 2 elements from the astExpressionStack (and a third if hasStride == true)

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R471 list c_action_ac_implied_do_control()");
#endif

     ROSE_ASSERT(astExpressionStack.empty() == false);
     SgExpression* increment = NULL;
     if (hasStride == true)
        {
          increment = astExpressionStack.front();
          astExpressionStack.pop_front();
       // setSourcePosition(increment);
       // printf ("increment = %p \n",increment);
        }
       else
        {
          increment = new SgNullExpression();
          setSourcePosition(increment);
        }

     ROSE_ASSERT(astExpressionStack.empty() == false);
     SgExpression* upperBound = astExpressionStack.front();
     astExpressionStack.pop_front();
  // setSourcePosition(upperBound);
  // printf ("upperBound = %p \n",upperBound);

     ROSE_ASSERT(astExpressionStack.empty() == false);
     SgExpression* lowerBound = astExpressionStack.front();
     astExpressionStack.pop_front();
  // setSourcePosition(lowerBound);
  // printf ("lowerBound = %p \n",lowerBound);

  // DQ (10/9/2010): Reimplementation of support for implied do loop support.
  // We have to form the implied do loop variable initialization. Note that this is not 
  // a variable declaration, since if implicit none is used, the variable must have 
  // already been declared.
     ROSE_ASSERT(astNameStack.empty() == false);
     SgName do_variable_name = astNameStack.front()->text;
     astNameStack.pop_front();

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("implied do loop variable name = %s \n",do_variable_name.str());

     SgVariableSymbol* variableSymbol = trace_back_through_parent_scopes_lookup_variable_symbol(do_variable_name,astScopeStack.front());
     ROSE_ASSERT(variableSymbol != NULL);

     SgVarRefExp* doLoopVar = SageBuilder::buildVarRefExp(variableSymbol);
     ROSE_ASSERT(doLoopVar != NULL);

     SgExpression* doVariableInitialization = SageBuilder::buildAssignOp(doLoopVar,lowerBound);
     ROSE_ASSERT(doVariableInitialization != NULL);
  // printf ("doVariableInitialization = %p = %s \n",doVariableInitialization,doVariableInitialization->class_name().c_str());

     SgExprListExp* loopControl = new SgExprListExp();
     setSourcePosition(loopControl);

  // loopControl->append_expression(lowerBound);
     loopControl->append_expression(doVariableInitialization);
     loopControl->append_expression(upperBound);
     loopControl->append_expression(increment);

     astExpressionStack.push_front(loopControl);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R471 list c_action_ac_implied_do_control()");
#endif

#if 0
     printf ("In c_action_ac_implied_do_control(): handling the implied do control support \n");
     ROSE_ASSERT(false);
#endif
   }

/**
 * R472
 * scalar_int_variable
 *
 */
void
c_action_scalar_int_variable()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_scalar_int_variable() \n");
   }


/** R501
 * type_declaration_stmt
 * :    (label)? declaration_type_spec ( (T_COMMA attr_spec)* T_COLON_COLON )?
 *        entity_decl_list T_EOS
 *
 * @param label Optional statement label
 * @param numAttributes The number of attributes present
 * @param eos Token for the end of the statement.
 */
// void c_action_type_declaration_stmt(Token_t * label, int numAttributes)
void
c_action_type_declaration_stmt(Token_t * label, int numAttributes, Token_t * eos)
   {
  // This function is similar to R441 component-attr-spec-list 

  // This is a variable declaration (build the SgVariableDeclaration and populate it using data saved on the stack).

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_type_declaration_stmt: numAttributes = %d \n",numAttributes);

#if !SKIP_C_ACTION_IMPLEMENTATION
  // DQ (9/15/2007): Moved from the associated __begin() function which was removed
  // Refactored the code to build support function
  // build_implicit_program_statement_if_required();
     initialize_global_scope_if_required();
     build_implicit_program_statement_if_required();

  // printf ("getTopOfScopeStack() = %p = %s \n",getTopOfScopeStack(),getTopOfScopeStack()->class_name().c_str());
     ROSE_ASSERT(getTopOfScopeStack()->variantT() == V_SgBasicBlock || getTopOfScopeStack()->variantT() == V_SgClassDefinition);

  // DQ (1/28/2009): I think we can assert this, and if so we can eliminate the call 
  // to buildVariableDeclarationAndCleanupTypeStack().
     ROSE_ASSERT (astNodeStack.empty() == true && astBaseTypeStack.empty() == true);

  // DQ (1/28/2009): Refactored this code so it could be called in R442, R501, R504 and R1238.
     buildVariableDeclarationAndCleanupTypeStack(label);
#endif
   }

/** R502
 * declaration_type_spec
 *      :       intrinsic_type_spec
 *      |       T_TYPE T_LPAREN derived_type_spec T_RPAREN
 *      |       T_CLASS T_LPAREN derived_type_spec T_RPAREN
 *      |       T_CLASS T_LPAREN T_ASTERISK T_RPAREN
 *
 * @param udtKeyword Token for the T_TYPE or T_CLASS and null for 
 * intrinsic_type_spec.
 * @param type The type of declaration-type-spec {INTRINSIC,TYPE,
 * CLASS,POLYMORPHIC}.
 */
// void c_action_declaration_type_spec(int type)
void c_action_declaration_type_spec(Token_t * udtKeyword, int type)
   {
  // The type value makes it clear what the type will be for the variable declaration being defined in the next setof rules.
  // We need to save this information so that we can know the type of the variable declaration when it is later built.

  // So we need to build the SgType and push it onto the type stack!

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R502 c_action_declaration_type_spec() udtKeyword = %p = %s type = %d \n",udtKeyword,udtKeyword != NULL ? udtKeyword->text : "NULL",type);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R502 c_action_declaration_type_spec()");
#endif

#if !SKIP_C_ACTION_IMPLEMENTATION
     switch(type)
        {
          case DeclarationTypeSpec_INTRINSIC:
             {
               if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
                    printf ("type == DeclarationTypeSpec_INTRINSIC \n");

            // This marks the type in a declaration to be one of the primative types, not really used in ROSE.
            // printf ("In c_action_declaration_type_spec(type = %d = DeclarationTypeSpec_INTRINSIC) \n",type);
            // printf ("Sorry, not implemented: c_action_declaration_type_spec(DeclarationTypeSpec_INTRINSIC) \n");
            // ROSE_ASSERT(false);

            // Modify the type that was previously built
               SgType* typeNode = astTypeStack.front();

            // printf ("In c_action_declaration_type_spec(): astExpressionStack.size() = %zu \n",astExpressionStack.size());
 
            // DQ (12/1/2007): I think this is now taken care of in R404
               ROSE_ASSERT(astExpressionStack.empty() == true);
               if (astExpressionStack.empty() == false)
                  {
                    SgExpression* kindExpression = astExpressionStack.front();
 
                 // DQ (12/1/2007): I think this is now taken care of in R404
                 // typeNode->set_type_kind(kindExpression);

                 // DQ (9/30/2007): Set the parent (if not this is caught in AST whole graph generation)
                    kindExpression->set_parent(typeNode);

                    printf ("After setting the kind, the expression stack size = %zu \n",astExpressionStack.size());
                    astExpressionStack.pop_front();
                  }
               break;
             }

          case DeclarationTypeSpec_TYPE:
             {
            // This is the case of a derived type
               if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                    printf ("In c_action_declaration_type_spec(type = %d = DeclarationTypeSpec_TYPE) \n",type);
               break;
             }

          case DeclarationTypeSpec_CLASS:
             {
            // What is this?
               printf ("Sorry, not implemented: c_action_declaration_type_spec(DeclarationTypeSpec_CLASS) \n");
               ROSE_ASSERT(false);
               break;
             }

          case DeclarationTypeSpec_unlimited:
             {
               printf ("Sorry, not implemented: c_action_declaration_type_spec(DeclarationTypeSpec_unlimited) \n");
               ROSE_ASSERT(false);
               break;
             }

          default:
             {
               printf ("Default reached in c_action_declaration_type_spec(): type = %d \n",type);

            // DQ (11/18/2007): Added an assertion here.
               ROSE_ASSERT(false);
               break;
             }         
        }

#endif

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("Before transfer to astBaseTypeStack in R502 c_action_declaration_type_spec()");
#endif

  // DQ (12/8/2007): Some types are build directly on the astBaseTypeStack.
  // When this happens we have nothing to transfer between stacks.

  // ROSE_ASSERT(astTypeStack.empty() == false);
     if (astTypeStack.empty() == false)
        {

       // DQ (12/8/2007): Now take the type and put it onto the astBaseTypeStack.
       // ROSE_ASSERT(astBaseTypeStack.empty() == true);
          astBaseTypeStack.push_front(astTypeStack.front());
          astTypeStack.pop_front();
        }
       else
        {
          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
               printf ("No type found on astTypeStack (make sure there is a base type on the astBaseTypeStack) \n");
          ROSE_ASSERT(astBaseTypeStack.empty() == false);
        }
#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R502 c_action_declaration_type_spec()");
#endif
   }

/** R503
 * attr_spec
 *      :       access_spec
 *      |       T_ALLOCATABLE
 *      |       T_ASYNCHRONOUS
 *      |       T_DIMENSION T_LPAREN array_spec T_RPAREN
 *      |       T_EXTERNAL
 *      |       T_INTENT T_LPAREN intent_spec T_RPAREN
 *      |       T_INTRINSIC
 *      |       language_binding_spec           
 *      |       T_OPTIONAL
 *      |       T_PARAMETER
 *      |       T_POINTER
 *      |       T_PROTECTED
 *      |       T_SAVE
 *      |       T_TARGET
 *      |       T_VALUE
 *      |       T_VOLATILE
 *
 * @param attrKeyword Token for the keyword of the given attribute.  Will 
 * be null in the cases of access_sepc and language_binding_spec.
 * @param attr The attribute specification
 */
// void c_action_attr_spec(int attr)
void c_action_attr_spec(Token_t * attrKeyword, int attr)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R503 c_action_attr_spec(): attrKeyword = %p = %s attr = %d \n",attrKeyword,attrKeyword != NULL ? attrKeyword->text : "NULL",attr);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R503 c_action_attr_spec()");
#endif

     switch(attr)
        {
          case AttrSpec_none:
             {
               if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                    printf ("found a AttrSpec_none spec \n");
               break;
             }

          case AttrSpec_access:
             {
            // DQ (5/20/2008): This is a redundant specifier, it appears to only be used with AttrSpec_PUBLIC or AttrSpec_PRIVATE
               if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                    printf ("found a AttrSpec_access spec \n");
               break;
             }

           case AttrSpec_language_binding:
              {
                 if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                      printf ("found a language_binding spec \n");
                 break;
              }

           case AttrSpec_PUBLIC:
              {
                 if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                      printf ("found a PUBLIC spec \n");
                 break;
              }

           case AttrSpec_PRIVATE:
              {
                 if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                      printf ("found a PRIVATE spec \n");
                 break;
              }

           case AttrSpec_ALLOCATABLE:
              {
                 if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                      printf ("found a ALLOCATABLE spec \n");
                 break;
              }

           case AttrSpec_ASYNCHRONOUS:
              {
                 if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                      printf ("found a ASYNCHRONOUS spec \n");
                 break;
              }

          case AttrSpec_DIMENSION:
             {
            // DQ (1/16/2011): Just as with the case of AttrSpec_POINTER, we have to process this to generate an array.
            // This should be done here so get the base type onto the astBaseTypeStack as soon as possible. Alternatively,
            // we could reverse the stack entries so that we processed the astAttributeSpecStack in the correct (original) 
            // order. This is only a problem when there are 2 or more entries such as "real, dimension(:), pointer :: h".
               if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                    printf ("found a DIMENSION spec \n");

            // At this point the array type is already built, but is built using the base type, also it is on the astTypeStack, and not the astBaseTypeStack.
            // so we move it to the astBaseTypeStack stack.
               break;
             }

           case AttrSpec_EXTERNAL:
              {
                 if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                      printf ("found a EXTERNAL spec \n");
                 break;
              }

           case AttrSpec_INTENT:
              {
                 if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                      printf ("found a INTENT spec \n");
                 break;
              }

           case AttrSpec_INTRINSIC:
              {
                 if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                      printf ("found a INTRINSIC spec \n");
                 break;
              }

           case AttrSpec_BINDC:
              {
                 if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                      printf ("found a BINDC spec \n");
                 break;
              }

           case AttrSpec_OPTIONAL:
              {
                 if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                      printf ("found a OPTIONAL spec \n");
                 break;
              }

           case AttrSpec_PARAMETER:
              {
                 if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                    printf ("found a PARAMETER spec (AttrSpec_PARAMETER = %d)\n",AttrSpec_PARAMETER);
                 break;
              }

           case AttrSpec_POINTER:
              {
                 if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                      printf ("found a POINTER spec \n");
                 break;
              }

           case AttrSpec_PROTECTED:
              {
                 if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                      printf ("found a PROTECTED spec \n");
                 break;
              }

           case AttrSpec_SAVE:
              {
                 if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                      printf ("found a SAVE spec \n");
                 break;
              }

           case AttrSpec_TARGET:
              {
                 if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                      printf ("found a TARGET spec \n");
                 break;
              }

           case AttrSpec_VALUE:
              {
                 if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                      printf ("found a VALUE spec \n");
                 break;
              }

           case AttrSpec_VOLATILE:
              {
                 if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                      printf ("found a VOLATILE spec \n");
                 break;
              }

           case AttrSpec_PASS:
              {
                 if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                      printf ("found a PASS spec \n");
                 break;
              }

           case AttrSpec_NOPASS:
              {
                 if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                      printf ("found a NOPASS spec \n");
                 break;
              }

           case AttrSpec_NON_OVERRIDABLE:
              {
                 if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                      printf ("found a NON_OVERRIDABLE spec \n");
                 break;
              }

           case AttrSpec_DEFERRED:
              {
                 if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                      printf ("found a DEFERRED spec \n");
                 break;
              }

          default:
             {
               printf ("default reached in c_action_attr_spec() attr = %d \n",attr);
               ROSE_ASSERT(false);
             }
        }

  // DQ (1/23/2011): The dimension attribute will be associated with an attribute pusded by R510 #2 c_action_array_spec_element().
  // DQ (5/20/2008): This is a redundant specifier, it appears to only be used with AttrSpec_PUBLIC or AttrSpec_PRIVATE
  // Push the attribue onto the stack (e.g. dimension)
  // astAttributeSpecStack.push_front(attr);
  // if (attr != AttrSpec_access)
     if (attr != AttrSpec_access && attr != AttrSpec_DIMENSION)
        {
          astAttributeSpecStack.push_front(attr);
        }

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R503 c_action_attr_spec()");
#endif
   }

/** R504, R503-F2008
 * entity_decl
 *  : T_IDENT ( T_LPAREN array_spec T_RPAREN )?
 *                      ( T_LBRACKET co_array_spec T_RBRACKET )?
 *                      ( T_ASTERISK char_length )? ( initialization )? 
 */
#if ROSE_OFP_MINOR_VERSION_NUMBER >= 8 & ROSE_OFP_PATCH_VERSION_NUMBER >= 0
void c_action_entity_decl(Token_t * id, ofp_bool hasArraySpec, ofp_bool hasCoarraySpec, ofp_bool hasCharLength, ofp_bool hasInitialization)
#else
void c_action_entity_decl(Token_t * id)
#endif
   {
  // This function R504 R503-F2008 is similar to R442 R438-F2008

  // Push the entities onto the list at the top of the stack
     ROSE_ASSERT(id != NULL);
     ROSE_ASSERT(id->text != NULL);

  // DQ (1/28/2009): We need to have the global scope and the function scope in place before
  // building variable declarations.  See test2007_09.f90.
  // initialize_global_scope_if_required(); not required!
     build_implicit_program_statement_if_required();

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
        {
       // printf ("In R504 R503-F2008 c_action_entity_decl(): save variableName = %s \n",id->text);
          string current_filename = getCurrentFilename();
#if ROSE_OFP_MINOR_VERSION_NUMBER >= 8 & ROSE_OFP_PATCH_VERSION_NUMBER >= 0
          printf ("In R504 R503-F2008 c_action_entity_decl(): save variableName = %s file = %s hasArraySpec = %s hasCoarraySpec = %s hasCharLength = %s hasInitialization = %s \n",
               id->text,current_filename.c_str(),hasArraySpec ? "true" : "false",
               hasCoarraySpec ? "true" : "false",hasCharLength ? "true" : "false",
               hasInitialization ? "true" : "false");
#else
          printf ("In R504 R503-F2008 c_action_entity_decl(): save variableName = %s file = %s \n",id->text,current_filename.c_str());
#endif
        }

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R504 R503-F2008 c_action_entity_decl()");
#endif

#if !SKIP_C_ACTION_IMPLEMENTATION
  // AstNameListStackType::iterator topOfStack = astNameListStack.begin();
  // (*topOfStack)->push_back(id);

  // DQ (11/23/2007): Changed this to "push_front()"
  // astNameStack.push_front(id);
  // astNameStack.push_back(id);
     astNameStack.push_front(id);

  // DQ (1/18/2011): Refactored the code below so it could be called from R443 as well as R504.
  // printf ("In R504 R503-F2008 calling processAttributeSpecStack(hasArraySpec=%s,hasInitialization=%s): astAttributeSpecStack.size() = %zu \n",
  //      hasArraySpec ? "true" : "false",hasInitialization ? "true" : "false",astAttributeSpecStack.size());
     processAttributeSpecStack(hasArraySpec,hasInitialization);

  // DQ (12/8/2007): The astBaseTypeStack should not be empty, but under new rules, the astTypeStack might be empty.
  // ROSE_ASSERT(astTypeStack.empty() == false);
     ROSE_ASSERT(astBaseTypeStack.empty() == false);

  // Make a copy on the type stack!
  // astTypeStack.push_front(astTypeStack.front());
     if (astTypeStack.empty() == true)
        {
       // printf ("astTypeStack is empty, so get the base type from the astBaseTypeStack \n");
          astTypeStack.push_front(astBaseTypeStack.front());
        }

     ROSE_ASSERT(astTypeStack.empty() == false);

     SgType* type = astTypeStack.front();
     AstNameType* nameToken = astNameStack.front();
     ROSE_ASSERT(nameToken->text != NULL);
     SgName name = nameToken->text;

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("Building the SgInitializer for variable of type = %s \n",type->class_name().c_str());

  // SgExpression* initializer = NULL;
     SgInitializer* initializer = NULL;

  // DQ (1/17/2011): Note that if we handle the initializer before processing the type then this 
  // will pick off the expression on the stack intended for the type.  Unless the initialized is
  // processed by R496 (which contains a count!).

  // if (astInitializerStack.empty() == false)
     if (astExpressionStack.empty() == false)
        {
          SgClassType* classType = isSgClassType(type);
          SgArrayType* arrayType = isSgArrayType(type);
          if (classType != NULL || arrayType != NULL)
             {
            // Use a SgConstructorInitializer or a SgAggregateInitializer

#if 1
            // Output debugging information about saved state (stack) information.
               outputState("In case of classType != NULL || arrayType != NULL in R504 c_action_entity_decl()");
#endif

               if (arrayType != NULL)
                  {
                 // DQ (12/10/2010): It might be that this should not be based on the astExpressionStack 
                 // size, but also on if the type of the expression is SgArrayType. See test2010_136.f90
                 // for an example, however the type of the intrisic function is not correctly generated 
                 // to be an SgArrayType. See test2010_137.f90 for where this special handling must be 
                 // restricted to function calls uses as initializers.

                    size_t astExpressionStackSize = astExpressionStack.size();
                    bool useAggregateInitializer = true;
                    if (astExpressionStackSize == 1)
                       {
                      // If the initializer is a function call, then use an SgAssignInitializer (see test2010_136.f90).
                         SgFunctionCallExp* functionCall = isSgFunctionCallExp(astExpressionStack.front());
                         if (functionCall != NULL)
                            {
                              useAggregateInitializer = false;
                            }
                       }

                 // printf ("In R504 R503-F2008 c_action_entity_decl(): useAggregateInitializer = %s \n",useAggregateInitializer ? "true" : "false");
                    if (useAggregateInitializer == true)
                       {
                         SgExprListExp* exprList = new SgExprListExp();
                         setSourcePosition(exprList);

                         std::list<SgExpression*>::iterator i = astExpressionStack.begin();
                         while (i != astExpressionStack.end())
                            {
                              exprList->prepend_expression(*i);
                              i++;
                            }

                      // Use a SgAggregateInitializer
                         initializer = new SgAggregateInitializer(exprList, NULL);
                         setSourcePosition(initializer);
                       }
                      else
                       {
                         ROSE_ASSERT(astExpressionStackSize == 1);
                         initializer = new SgAssignInitializer(astExpressionStack.front(),NULL);
                         setSourcePosition(initializer);
                       }

                    astExpressionStack.clear();

                 // printf ("In R504 R503-F2008 c_action_entity_decl(): ################# Built a SgAggregateInitializer (%p) \n",initializer);

                    ROSE_ASSERT(astExpressionStack.empty() == true);
                  }
                 else
                  {
                 // Use a SgConstructorInitializer, in this case the SgConstructorInitializer was built elsewhere and we want to use reuse it.
                 // ROSE_ASSERT(exprList->get_expressions().size() == 1);
                 // SgExpression* firstExpression = exprList->get_expressions()[0];
                    ROSE_ASSERT(astExpressionStack.size() == 1);
                    SgExpression* firstExpression = astExpressionStack.front();

                 // DQ (10/23/2010): We need to leave this on the stack since it is the name associated with a bind attribute.
                 // astExpressionStack.pop_front();

                    ROSE_ASSERT(firstExpression != NULL);
                 // printf ("firstExpression = %p = %s \n",firstExpression,firstExpression->class_name().c_str());
                    SgConstructorInitializer* constructorInitializer = isSgConstructorInitializer(firstExpression);
                    initializer = constructorInitializer;
                 // ROSE_ASSERT(initializer != NULL);
                    if (initializer != NULL)
                       {
                      // If this is a SgConstructorInitializer then ???
                         printf ("This is a SgConstructorInitializer, we need an example of code that generates this case. \n");
                         ROSE_ASSERT(false);
                       }
                      else
                       {
                      // This is the case of a name in the bind expression.
                      // printf ("This is the case of a name in the bind expression (leave it on the stack and process it later). \n");
                      // ROSE_ASSERT(false);
                       }
                  }
             }
            else
             {
            // Use a SgAssignInitializer
#if 0
               printf ("Building a SgAssignInitializer for the initializer. \n");
#endif
            // initializer = new SgAssignInitializer(astInitializerStack.front(),NULL);
            // astInitializerStack.pop_front();
               initializer = new SgAssignInitializer(astExpressionStack.front(),NULL);
               astExpressionStack.pop_front();
               setSourcePosition(initializer);

            // DQ (1/17/2011): There could still be entries on the stack since now the type if constructed AFTER the initializer is set.
            // See test2007_94.f90 for an example of this.  Note that we can't generate the type when we process the array because
            // we may still have pointer attributes coming as c_actions and we need to see everything and put it onto the stack
            // and then construct the declaration's type via an unwinding of the stack (of all attributes that could contribute to
            // the construction of the declaration's type.
            // ROSE_ASSERT(astExpressionStack.empty() == true);
             }
        }

  // ROSE_ASSERT(astInitializerStack.empty() == true);
  // ROSE_ASSERT(astExpressionStack.empty() == true);

  // DQ (5/15/2008): Added support to make sure that this is not a previously declared name (see test2008_29.f)
     SgVariableSymbol* variableSymbol = NULL;
     SgFunctionSymbol* functionSymbol = NULL;
     SgClassSymbol*    classSymbol    = NULL;
     SgScopeStatement* currentScope   = astScopeStack.front();
     ROSE_ASSERT(currentScope != NULL);
     trace_back_through_parent_scopes_lookup_variable_symbol_but_do_not_build_variable(name,currentScope,variableSymbol,functionSymbol,classSymbol);

  // ROSE_ASSERT(variableSymbol == NULL);

  // printf ("In R504 R503-F2008: variableSymbol = %p functionSymbol = %p classSymbol = %p \n",variableSymbol,functionSymbol,classSymbol);

     if (functionSymbol != NULL)
        {
#if 0
       // printf ("Warning: found functionSymbol != NULL (avoid resetting the type): name = %s \n",name.str());
          printf ("Warning: found functionSymbol != NULL (now reset the type): function name = %s \n",name.str());
#endif

#if 0
       // Output debugging information about saved state (stack) information.
          outputState("In case of functionSymbol != NULL in R504 c_action_entity_decl()");
#endif
       // printf ("initializer = %p \n",initializer);
          ROSE_ASSERT(initializer == NULL);

          SgFunctionType* functionType = isSgFunctionType(functionSymbol->get_type());
          ROSE_ASSERT(functionType != NULL);

          ROSE_ASSERT(astTypeStack.empty() == false);

       // printf ("Before resetting: functionType->get_return_type() = %p = %s \n",functionType->get_return_type(),functionType->get_return_type()->class_name().c_str());
          functionType->set_return_type(astTypeStack.front());
          functionType->set_orig_return_type(type);
       // printf ("After resetting case 1: functionType->get_return_type() = %p = %s \n",functionType->get_return_type(),functionType->get_return_type()->class_name().c_str());

       // printf ("Exiting at base of case to reset the function type! \n");
       // ROSE_ASSERT(false);
        }
  // ROSE_ASSERT(functionSymbol == NULL);

     ROSE_ASSERT(classSymbol == NULL);

     SgInitializedName* initializedName = NULL;
     if (variableSymbol != NULL)
        {
          ROSE_ASSERT(type != NULL);

          initializedName = variableSymbol->get_declaration();
          ROSE_ASSERT(initializedName != NULL);

       // DQ (1/29/2011): We need this below.
          ROSE_ASSERT(initializedName->get_scope() != NULL);
#if 0
          printf ("Found variableSymbol != NULL (check if this is the return value which was set with implicit type rules): initializedName = %p = %s \n",initializedName,initializedName->get_name().str());
          printf ("initializedName type = %p = %s \n",type,type->class_name().c_str());

          printf ("currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
          printf ("initializedName->get_scope() = %p = %s \n",initializedName->get_scope(),initializedName->get_scope()->class_name().c_str());
#endif

#if 0
       // DQ (1/30/2010): Set this below in only the case where we will be using what could be an outer scope SgInitializedName.
       // Reset the type since this is the variable declaration, even if it was previously declared in the common block (where it was not assigned a type)
          printf ("Resetting the type of the SgInitializedName since it was not known previously \n").
          initializedName->set_type(type);
#endif
          ROSE_ASSERT(initializedName->get_scope() != NULL);

          SgFunctionDefinition* functionDefinition = isSgFunctionDefinition(initializedName->get_scope());
          if (functionDefinition != NULL)
             {
            // This is is a variable that is in the function scope (not the scope of the function body, so it 
            // might be a variable from the return specfication of the function). If so, then it specifies the 
            // return type of the function.

               SgProcedureHeaderStatement* functionDeclaration = isSgProcedureHeaderStatement(functionDefinition->get_parent());
               ROSE_ASSERT(functionDeclaration != NULL);

               if (functionDeclaration->get_result_name() == initializedName)
                  {
                    SgFunctionType* functionType = isSgFunctionType(functionDeclaration->get_type());
                    ROSE_ASSERT(functionType != NULL);

                 // printf ("Before resetting: functionType->get_return_type() = %p = %s \n",functionType->get_return_type(),functionType->get_return_type()->class_name().c_str());
                    functionType->set_return_type(type);
                    functionType->set_orig_return_type(type);
                 // printf ("After resetting case 2: functionType->get_return_type() = %p = %s \n",functionType->get_return_type(),functionType->get_return_type()->class_name().c_str());
                  }

#if 0
               printf ("Case of function parameter (scope == SgFunctionDefinition): Resetting the type of the SgInitializedName since it was not known previously \n");
#endif
            // DQ (1/30/2011): I think this is the case of variables declared in the function definition.
            // Reset the type since this is the variable declaration, even if it was previously declared in the common block (where it was not assigned a type)
               initializedName->set_type(type);
             }
            else
             {
            // DQ (1/30/2011): This might be a variable from an outer scope and if so then we want to ignore it and build a SgInitializedName in the current scope.
               if (currentScope != initializedName->get_scope())
                  {
#if 0
                    printf ("currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
                    printf ("Ignoring the outer scope initializedName and building a new one in the currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
#endif
                 // Overwrite the pointer to the SgInitializedName from the outer scope...
                    initializedName = buildInitializedNameAndPutOntoStack(name,type,initializer);

#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }
                 else
                  {
#if 0
                    printf ("Case of currentScope == initializedName->get_scope(): Resetting the type of the SgInitializedName since it was not known previously \n");
#endif
                 // DQ (1/30/2011): This is the case of a variable declared in the current scope (where we got the name but not the full type, so this is the first point ere we know the full type).
                 // Reset the type since this is the variable declaration, even if it was previously declared in the common block (where it was not assigned a type)
                    initializedName->set_type(type);
                  }
             }

          ROSE_ASSERT(initializedName != NULL);
        }
       else
        {
       // initializedName = new SgInitializedName(name,type,initializer,NULL,NULL);
          if (functionSymbol != NULL)
             {
            // This is the case of where an external function has been declared and it's type is being specified.  It should not generate a variable declaration.
            // printf ("This is the case of where an external function has been declared and it's type is being specified. \n");

            // DQ (12/11/2010): This is not unclear, we don't want a variable declaration built where the purpose was to define the named function's return type.
            // Also, we might want to verify the scope of the function (parent of declaration) is consistant with where its return type is being defined (current scope).
            // However, if we don't build a variable declaration then we have nothing that will be output by the unparser that will define the return type of the 
            // function, unless we force such a statement to be generated when we unparse the external statement.  But then we would loose the order of the statements.
            // Old comment... Unclear what to do here!
               ROSE_ASSERT(initializedName == NULL);
               initializedName = new SgInitializedName(name,type,initializer,NULL,NULL);

            // FMZ 6/8/2010: set the it as a result of the function, avoid unparser duplicate the type decl
               SgProcedureHeaderStatement* functionDeclaration = isSgProcedureHeaderStatement(functionSymbol->get_declaration());
               functionDeclaration->set_result_name(initializedName);
             }
            else
             {
#if 1
            // DQ (1/30/2011): Refactored code to use here and just above as well.
               initializedName = buildInitializedNameAndPutOntoStack(name,type,initializer);
#else
            // DQ (1/30/2011): Older version of code before refactoring...
#error "DEAD CODE"
            // printf ("Building a new SgInitializedName that will be assembled into a variable declaration later. \n");
               initializedName = new SgInitializedName(name,type,initializer,NULL,NULL);

            // DQ (9/11/2010): There is not associated SgVariableSymbol associated with this, so we need to build one.
            // This fixes test2010_45.f90 which references a variable declared in the same variable declaration.
               SgVariableSymbol* variableSymbol = new SgVariableSymbol(initializedName);

#error "DEAD CODE"
            // DQ (11/29/2010): Set the scope for the SgInitializedName IR node (caught when trying to output (print) the symbol table).
               initializedName->set_scope(getTopOfScopeStack());

               ROSE_ASSERT(astScopeStack.empty() == false);
               astScopeStack.front()->insert_symbol(name,variableSymbol);
               ROSE_ASSERT (initializedName->get_symbol_from_symbol_table () != NULL);
#error "DEAD CODE"

            // Test the symbol tables and the new support for case insensitive symbol tables.
               ROSE_ASSERT(astScopeStack.front()->symbol_exists(name) == true);
               ROSE_ASSERT(astScopeStack.front()->isCaseInsensitive() == true);
#error "DEAD CODE"
               SgName invertedCaseName = name.invertCase();
               ROSE_ASSERT(astScopeStack.front()->symbol_exists(invertedCaseName) == true);
               ROSE_ASSERT(initializedName != NULL);
#endif
             }
        }

     ROSE_ASSERT(initializedName != NULL);

  // DQ (1/24/2011): I think that this test should pass. No it fails for test2011_04.f90.
  // ROSE_ASSERT(initializedName->get_symbol_from_symbol_table() != NULL);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("Before cleaning up the stacks in R504 c_action_entity_decl()");
#endif

  // FMZ (2/19/2007): Added for CoArray
#if ROSE_OFP_MINOR_VERSION_NUMBER >= 8 & ROSE_OFP_PATCH_VERSION_NUMBER >= 0
     if (astAttributeSpecStack.empty() == false && astAttributeSpecStack.front()==AttrSpec_COARRAY)
        {
          astAttributeSpecStack.pop_front();
          initializedName->set_isCoArray(true);
        }
       else 
        {
          initializedName->set_isCoArray(false);
        }
#endif

  // printf ("In c_action_entity_decl(): initializedName = %p = %s \n",initializedName,initializedName->get_name().str());

  // setSourcePosition(initializedName);
     setSourcePosition(initializedName,id);

  // DQ (12/14/2007): Set the scope
  // initializedName->set_scope(astScopeStack.front());
  // printf ("Scope not set for initializedName = %p = %s (will be set in buildVariableDeclaration() \n",initializedName,name.str());

     if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
          printf ("In c_action_entity_decl(): Initializer for variable initializedName = %p = %s is %p = %p \n",initializedName,name.str(),initializer,initializedName->get_initializer());

     astNodeStack.push_front(initializedName);

     ROSE_ASSERT(astTypeStack.empty() == false);

     ROSE_ASSERT(astNameStack.empty() == false);
     astNameStack.pop_front();

  // DQ (1/22/2011): We want to always rebuild the type on the astTypeStack, so pop the 
  // current version (just used to build the SgInitializedName) off the astTypeStack.
  // if (hasArraySpec == true)
        {
       // If the type was built specific for given variable then pop the astTypeStack and build a new one for any other variable.
          ROSE_ASSERT(astTypeStack.empty() == false);
          astTypeStack.pop_front();
          ROSE_ASSERT(astTypeStack.empty() == true);
       }
#endif

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R504 c_action_entity_decl()");
#endif
   }

/** R504 R503-F2008 list
 * entity_decl
 * entity_decl_list
 *      :       entity_decl ( T_COMMA entity_decl )*
 * 
 * @param count The number of items in the list.
 */
void c_action_entity_decl_list__begin()
   {
  // The use of the astNameListStack has been discontinued, we just use a stack of names (tokens) now!
  // This make for a simpler implementation and I don't think we require the additional complexity.

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R504 (list__begin) c_action_entity_decl_list__begin()");
#endif

  // DQ (1/20/2011): We have to save the astAttributeSpecStack so that we can use the same attribute 
  // stack for processing all variables in the declaration uniformally.  See test2007_248.f90 for an 
  // example.  We will also have to test this support in R441 as well (similar parser action, but for 
  // variable declarations inside of types.
  // printf ("Save the astAttributeSpecStack stack to reuse it for each variable in the declaration. \n");

  // OR just don't delete the attributes until we process "c_action_entity_decl_list(int count)"!!!

  // printf ("Build the base type for the variable declaration. \n");

  // DQ (12/8/2007): Modified to reflect use of new astBaseTypeStack (see test2007_148.f)
  // DQ (12/7/2007): Added assertion.
  // ROSE_ASSERT(astTypeStack.empty() == false);
  // ROSE_ASSERT(astTypeStack.empty() == true);
     if (astTypeStack.empty() == false)
        {
          if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
               printf ("Warning, astTypeStack.size() = %zu \n",astTypeStack.size());
        }

     ROSE_ASSERT(astBaseTypeStack.empty() == false);

  // This is the start of an entity list (used in variable declarations to hold the variables being declared
  // build the list and add the variable identifiers to the list
  // AstNameListType* nameList = new AstNameListType();
  // astNameListStack.push_front(nameList);

     convertBaseTypeToArrayWhereAppropriate();
   }

void c_action_entity_decl_list(int count)
   {
  // This function R504 R503-F2008 is similar to R441 but is used for declarations in NON-types.

  // Since we already have the elements in the list we don't have to do anything here.
  // In general it is a tradeoff as to if we should build the list in the <rule>_list__begin()
  // function and then add the elements to the list in the element-specific functions or
  // if we should push the elements onto the stack in the element-specific functions and
  // assemble the lists in the <rule>_list() functions.  Except that we want to be consistent
  // it is not clear if there is a best answer to this question.

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R504 c_action_entity_decl_list: (number of variable declarations) count = %d \n",count);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R504 (list) c_action_entity_decl_list()");
#endif

  // Note that we are not really using the count, but the astNodeStack.size() should match the count.
  // DQ (1/28/2009): Can we assert this!
     ROSE_ASSERT(astNodeStack.size() == (size_t) count);

  // DQ (1/28/2009): Refactored this code so it could be called in R442, R501, R504 and R1238.
  // I think this is the earliest point at which this can be called, after this point if
  // there additional modified for the variables, then they will have to update the
  // declaration and/or build new statements to do the modifications (e.g. dimension statments).

     buildVariableDeclarationAndCleanupTypeStack(NULL);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R504 (list) c_action_entity_decl_list()");
#endif
   }

/** R506
 * initialization
 *      :       T_EQUALS expr
 *      |       T_EQ_GT null_init
 *
 * ERR_CHK 506 initialization_expr replaced by expr
 *
 * @param hasExpr True if expr is present
 * @param hasNullInit True if null-init is present
 */
void c_action_initialization(ofp_bool hasExpr, ofp_bool hasNullInit)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_initialization(): hasExpr = %s hasNullInit = %s \n",hasExpr ? "true" : "false",hasNullInit ? "true" : "false");

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R506 c_action_initialization()");
#endif

  // Check and see what kind of initializer this is:
  //    1) For a implied-do loop we need to identify the array length and push it onto the stack.
  //    2) 

     if (hasExpr == true)
        {
          if (astExpressionStack.empty() == false)
             {
            // This already has an SgExprListExp
            // ROSE_ASSERT(isSgExprListExp(astInitializerStack.front()) != NULL);

            // printf ("This is the case were we should process the initializer (see test2008_21.f90) \n");

            // We should have an SgVarRefExp on the astExpressionStack, but we are going to ignore it since this 
            // is for initialization.  So clear the astExpressionStack.  Later we might need to have saved this,
            // but for now it appears to be redundant with the type specification which we can derive in the 
            // unparser.

            // SgVarRefExp* variable = isSgVarRefExp(astExpressionStack.front());
            // variable->set_initializer(astInitializerStack.front());
            // astInitializerStack.pop_front();
             }
            else
             {
            // This is the case for test2007_181.f90 ("integer :: n = 1"). Note that ac_value() is not called here!

            // ROSE_ASSERT(astExpressionStack.empty() == false);
            // ROSE_ASSERT(isSgExprListExp(astExpressionStack.front()) == NULL);

            // DQ (1/23/2011): Added test to explain why nothing is done in this code below.
               ROSE_ASSERT(astExpressionStack.empty() == true);

               SgExprListExp* expressionList = new SgExprListExp();
               setSourcePosition(expressionList);
               if (astExpressionStack.empty() == false)
                  {
                    while(astExpressionStack.empty() == false)
                       {
                      // Transfer the IR node from the astExpressionStack to the astNodeStack.
                         SgExpression* initializer = astExpressionStack.front();
                         astExpressionStack.pop_front();

                         ROSE_ASSERT(initializer != NULL);
                         expressionList->append_expression(initializer);
                       }

                 // astInitializerStack.push_front(expressionList);
                    astExpressionStack.push_front(expressionList);
                  }
                 else
                  {
                    printf ("Could not find an initializer on the stack (either the astInitializerStack nor the astExpressionStack) \n");
                    ROSE_ASSERT(false);
                  }
             }
        }

  // DQ (5/4/2008): Now that we don't use the astInitializerStack, we can't assert this.
  // ROSE_ASSERT(astExpressionStack.empty() == true);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R506 c_action_initialization()");
#endif
   }

/** R507
 * null_init
 *      :       T_IDENT //'NULL'// T_LPAREN T_RPAREN
 *
 * C506 The function-reference shall be a reference to the NULL intrinsic function with no arguments.
 *
 * @param id The function-reference
 */
void c_action_null_init(Token_t * id)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_null_init(): id = %p = %s \n",id,id != NULL ? id->text : "NULL");

  // DQ (1/23/2011): We don't have a NULL() IR node, so I will have to add one.  For the moment (debugging) use a value!.
     SgIntVal* integerValue = new SgIntVal(0,"0");
     ROSE_ASSERT(integerValue != NULL);

     setSourcePosition(integerValue);

     astExpressionStack.push_front(integerValue);

  // printf ("***** Need to build a NULL() value IR node ***** \n");
  // ROSE_ASSERT(false);
   }

/** R508
 * access_spec
 *
 * :  T_PUBLIC
 * |  T_PRIVATE
 *
 * @param keyword The access-spec keyword token.
 * @param type The type of the access-spec.
 */
void c_action_access_spec(Token_t * keyword, int type)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_access_spec(): keyword = %p = %s type = %d \n",keyword,keyword != NULL ? keyword->text : "NULL",type);

     astAttributeSpecStack.push_front(type);

     ROSE_ASSERT(keyword != NULL);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R508 c_action_access_spec()");
#endif
   }

/** R509
 * language_binding_spec
 *      :       T_BIND T_LPAREN T_IDENT // 'C' // (T_COMMA name T_EQUALS expr)? T_RPAREN
 *
 * @param keyword The BIND keyword token.
 * @param id The identifier representing the language binding, must be 'C' or 'c'.
 * @param hasName True if the language-binding-spec has a name expression.       
 */
// void c_action_language_binding_spec(Token_t * id, ofp_bool hasName)
void c_action_language_binding_spec(Token_t * keyword, Token_t * id, ofp_bool hasName)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_language_binding_spec(): keyword = %p = %s id = %p = %s hasName = %s \n",keyword,keyword != NULL ? keyword->text : "NULL",id,id != NULL ? id->text : "NULL",hasName ? "true" : "false");

     ROSE_ASSERT(id != NULL);
     astNameStack.push_front(id);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R509 c_action_language_binding_spec()");
#endif
   }

/** R510
 * array_spec
 *      :       array_spec_element (T_COMMA array_spec_element)*
 * 
 * array_spec_element
 *      :       expr ( T_COLON (expr | T_ASTERISK)? )?
 *      |   T_ASTERISK
 *      |       T_COLON
 *
 * @param count The number of items in the list of array specifications.
 * @param type The type of the array-spec element.
 */
void c_action_array_spec__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_array_spec__begin() \n");
   }

void c_action_array_spec(int count)
   {
  // The stack size should match the count, NO!

  // The stack size here is the number of types used to represent the array (a single SgArrayType), 
  // and the count is the number of dimensions of the array.  This could be used to set the rank, but
  // likely the rank should not be stored explicitly since it is redundant with the array dimension 
  // expression list.

  // ROSE_ASSERT(astTypeStack.size() == (size_t)count);

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R510 c_action_array_spec(): count = %d (building the multi-dimensional shape for the future SgArrayType) astAttributeSpecStack.size() = %zu \n",count,astAttributeSpecStack.size());

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R510 #2 c_action_array_spec()");
#endif
  // printf ("astBaseTypeStack.empty() = %s \n",astBaseTypeStack.empty() ? "true" : "false");

  // DQ (1/17/2011): I think we can assert this now! No, test2007_34.f90 is still a counter example!
  // ROSE_ASSERT(astBaseTypeStack.empty() == false);

  // DQ (12/27/2007): parameters associated with variable declaration type attributes (e.g. dimention attribute) should not be used yet. But I don't know how to avoid it!
  // if (astBaseTypeStack.empty() == true || astAttributeSpecStack.empty() == false)
     if (astBaseTypeStack.empty() == true)
        {
       // If there is no type on the stack, then this might be part of a "allocatable :: i(:)" statement
       // (see test2007_34.f90).  If so then the SgColon expression is on the astExpressionStack.

          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
               printf ("This is an allocatable statement (I hope) \n");
        }
       else
        {
       // This has been moved to c_action_array_spec, since the c_action_array_spec_element() 
       // is called once per dimension of the array type.
        }

  // DQ (1/16/2011): test2011_06.f90 (construction of array of pointers to float)
  // the case of "real, dimension(:), pointer :: h".  However, this causes problems
  // for knowing when to build the array type and causes misinterpretation of
  // the expression list that we leave on the stack (which then causes a problem for
  // test2007_94.f90 (tuning the implicit initialization index variable into a 
  // function call expression instead of a variable reference.  We can either do a 
  // better job of detecting when the variable name should be a variable instead of 
  // a function call, or build the array to use a SgDefaultType and then fix it up 
  // later...

  // printf ("Moving the generation of the array type from R510 to R503! \n");

  // DQ (1/18/2011): Called by R510 and R443.
     processMultidimensionalSubscriptsIntoExpressionList(count);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R510 #2 c_action_array_spec()");
#endif
   }

void c_action_array_spec_element(int type)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R510 c_action_array_spec_element(): (value pushed onto astAttributeSpecStack) type = %d \n",type);

  // We don't really need to push these integers onto the stack, as long as each one traslates (one-to-one) 
  // into expressions on the astExpressionStack.
  // astAttributeSpecStack.push_back(type);

  // If this was a value expression (type == ArraySpecElement_expr), then it has be processed an placed onto the stack.
  // Else we have to process it here.

     ROSE_ASSERT(type >= ArraySpecElement_expr);
     if (type != ArraySpecElement_expr)
        {
       // Build the correct expression and put it onto the stack.
       // printf ("Processing a non-value-expression dimension type = %d \n",type);

          SgExpression* expression = NULL;
          switch(type)
             {
               case ArraySpecElement_expr:
                  {
                    printf ("Error: This sort of case ArraySpecElement_expr should have been processed already \n");
                    ROSE_ASSERT(false);
                    break;
                  }
                
               case ArraySpecElement_expr_colon:
                  {
                 // DQ (1/23/2009): Fix suggested by one of Craig's students (need name of student).
                 // I think that example test code: test2009_03.f90 demonstrates this case.
                    expression = buildSubscriptExpression(true, false, false, false);
                    break;
                  }
                
               case ArraySpecElement_expr_colon_expr:
                  {
                 // See test2008_62.f90 for an example of this.
                 // DQ (11/14/2008): A colon expression at this point can only have a base and bound (no stride).
                    bool hasLowerBound = true;
                    bool hasUpperBound = true;
                    bool hasStride     = false;
                    bool isAmbiguous   = false;

                    expression = buildSubscriptExpression(hasLowerBound,hasUpperBound,hasStride,isAmbiguous);
                    break;
                  }

               case ArraySpecElement_expr_colon_asterisk:
                  {
                 // DQ (11/14/2008): A colon expression at this point can only have a base and bound (no stride).
                    bool hasLowerBound = true;
                    bool hasUpperBound = true;
                    bool hasStride     = false;
                    bool isAmbiguous   = false;

                 // DQ (1/26/2009): Build a "*" and push it onto the stack so that we can form the subscript expression "<expression>:*"
                    SgExpression* tempExpression = new SgAsteriskShapeExp();
                    setSourcePosition(tempExpression);
                    astExpressionStack.push_front(tempExpression);

                    expression = buildSubscriptExpression(hasLowerBound,hasUpperBound,hasStride,isAmbiguous);
                    break;
                  }
                
               case ArraySpecElement_asterisk:
                  {
                 // expression = new SgSubscriptAsterisk();
                    expression = new SgAsteriskShapeExp();

                    setSourcePosition(expression);
                    break;
                  }

               case ArraySpecElement_colon:
                  {
                 // DQ (1/23/2009): Fix suggested by one of Craig's students (need name of student).
                 // I think that example test code: test2009_03.f90 demonstrates this case.
                 // CER (9/24/2008): I think SgSubscriptExpression is the correct thing to do here
                    expression = buildSubscriptExpression(false, false, false, false);
                    break;
                  }
                
               default:
                  {
                    printf ("Error: default reached type = %d \n",type);
                    ROSE_ASSERT(false);
                  }
             }

       // DQ (11/20/2007): Changed to push_front to be consistent.
          astExpressionStack.push_front(expression);
        }

  // DQ (1/17/2011): Push the AttrSpec_DIMENSION attribute only the stack to trigger this to be handled as an array (build an array type).
  // printf ("In R510 #2 c_action_array_spec_element(): Push the AttrSpec_DIMENSION attribute only the stack to trigger this to be handled as an array (build an array type). \n");
     astAttributeSpecStack.push_front(AttrSpec_DIMENSION);

  // A fundamental problem is that we may not know enough about the size of an array type at this point were we 
  // build it.  This will be fixed up later in teh process, so default values are provided which will indicate 
  // that additional data is required.

  // How should we distinguish between:
  //    integer , dimension (2) :: array_A
  //    rank = 2 size is undefined
  // and
  //    integer :: array_A(2)
  //    rank = 1 size is 2

  // This implementation is specific to type == ArraySpecElement_expr, I 
  // don't have examples of where the type is any other value at present.
  // ROSE_ASSERT(type == ArraySpecElement_expr);
#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R510 #3 c_action_array_spec_element()");
#endif
   }

/** R511
 * explicit_shape_spec
         * expr ( T_COLON expr )?
 * 
 * @param hasUpperBound Whether the shape spec is of the form x:y.
 */
void c_action_explicit_shape_spec(ofp_bool hasUpperBound)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_explicit_shape_spec(): hasUpperBound = %s \n",hasUpperBound ? "true" : "false");

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R511 c_action_explicit_shape_spec()");
#endif

  // DQ (1/13/2011): See test2011_01.f90 for an example of where this might be a fix.
  // int type = (hasUpperBound == true) ? ArraySpecElement_expr_colon_expr : ArraySpecElement_expr_colon;
     int type = (hasUpperBound == true) ? ArraySpecElement_expr_colon_expr : ArraySpecElement_expr;

     c_action_array_spec_element(type);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R511 c_action_explicit_shape_spec()");
#endif
   }

/** R511 list
 * explicit_shape_spec_list
 *      :       explicit_shape_spec ( T_COMMA explicit_shape_spec )*
 * 
 * @param count The number of items in the list.
 */
void c_action_explicit_shape_spec_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_explicit_shape_spec_list__begin() \n");
   }

void c_action_explicit_shape_spec_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_explicit_shape_spec_list(): count = %d \n",count);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R511 c_action_explicit_shape_spec_list()");
#endif

  // printf ("Calling R510 directly from R511... \n");

  // DQ (2/20/2008): Call the mechanism used to build an array type
     c_action_array_spec(count);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R511 c_action_explicit_shape_spec_list()");
#endif
   }


/** R517
 * intent_spec
 *      :       T_IN | T_OUT | T_IN T_OUT | T_INOUT
 *
 * @param intentKeyword1 The first of two possible intent keyword tokens
 * (e.g., T_IN, T_OUT, T_INOUT).
 * @param intentKeyword2 The second of two possible intent keyword tokens.  
 * This token can ONLY be T_OUT in the case of "intent(in out)", and must 
 * be null for all other intents.
 * @param intent The type of intent-spec.
 */
// void c_action_intent_spec(int intent)
void c_action_intent_spec(Token_t * intentKeyword1, Token_t * intentKeyword2, int intent)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_intent_spec() intentKeyword1 = %p = %s intentKeyword2 = %p = %s intent = %d \n",intentKeyword1,intentKeyword1 != NULL ? intentKeyword1->text : "NULL",intentKeyword2,intentKeyword2 != NULL ? intentKeyword2->text : "NULL",intent);

     astIntentSpecStack.push_back(intent);
   }

/** R518
 * access_stmt
 *
 *      :       (label)? access_spec ((T_COLON_COLON)? access_id_list)? T_EOS
 *
 * @param label The label.
 * @param eos End of statement token.
 * @param hasList True if access-id-list is present.
 */
// void c_action_access_stmt(Token_t * label, ofp_bool hasList)
void c_action_access_stmt(Token_t * label, Token_t * eos, ofp_bool hasList)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_access_stmt(): hasList = %s \n",hasList ? "true" : "false");

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R518 c_action_access_stmt()");
#endif

  // An AttributeSpecification statement can be the first statement in a program
  // (see test2007_147.f, the original Fortran I code from the IBM 704 Fortran Manual).
     build_implicit_program_statement_if_required();

  // What does this stmt mean if there is no list? This can be false (see test2008_34.f90)
  // ROSE_ASSERT(hasList == true);

     ROSE_ASSERT(astAttributeSpecStack.size() == 1);
     int type = astAttributeSpecStack.front();
     astAttributeSpecStack.pop_front();

  // DQ (10/25/2010): Changed this to back again to NOT use keywords.
  // DQ (10/25/2010): Howver there will be arguments to the "public statement so these will be 
  // on the name stack. So we can't assert that this is empty.
  // DQ (8/28/2010): changed behavior to never put access spec keywords onto the astNameStack.
  // ROSE_ASSERT(astNameStack.empty() == true);

  // Use the eos as a location...
     ROSE_ASSERT(eos != NULL);
     Token_t* private_public_keyword = eos;

     if (type == AttrSpec_PRIVATE)
        {
          buildAttributeSpecificationStatement(SgAttributeSpecificationStatement::e_accessStatement_private,label,private_public_keyword);
        }
       else
        {
          if (type == AttrSpec_PUBLIC)
             {
               buildAttributeSpecificationStatement(SgAttributeSpecificationStatement::e_accessStatement_public,label,private_public_keyword);
             }
            else
             {
               printf ("Error: This can only be PRIVATE or PUBLIC, I think  \n");
               ROSE_ASSERT(false);
             }
        }

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R518 c_action_access_stmt()");
#endif
   }

/**
 * R519-F2008
 * deferred_co_shape_spec
 *
 */
void deferred_co_shape_spec()
{
}

/** R519-08 list
 * deferred_co_shape_spec_list
 *      :       T_COLON ( T_COMMA T_COLON )*
 * 
 * @param count The number of items in the list.
 */
void c_action_deferred_co_shape_spec_list__begin()
{
}
void c_action_deferred_co_shape_spec_list(int count)
{
}

/**
 * R520-F2008
 * explicit_co_shape_spec
 *
 */
void c_action_explicit_co_shape_spec()
{
}

/**
 * explicit_co_shape_spec_suffix
 *
 */
void c_action_explicit_co_shape_spec_suffix()
{
}

/**
 * R519
 * access_id
 *
 */
void c_action_access_id()
   {
   }

/** R519 list
 * access_id_list
 *      :       access_id ( T_COMMA access_id )*
 * 
 * @param count The number of items in the list.
 */
void c_action_access_id_list__begin()
{
}
void c_action_access_id_list(int count)
{
}

/** R520
 * allocatable_stmt
 *
 * : (label)? T_ALLOCATABLE ( T_COLON_COLON )? allocatable_decl 
 *              ( T_COMMA allocatable_decl )* T_EOS
 *
 * @param label The label.
 * @param keyword The allocatable keyword token.
 * @param eos End of statement token.
 * @param count Number of allocatable declarations.
 */
// void c_action_allocatable_stmt(Token_t * label, int count)
#if ROSE_OFP_MINOR_VERSION_NUMBER >= 8 & ROSE_OFP_PATCH_VERSION_NUMBER >= 0
// void allocatable_stmt(Token label, Token keyword, Token eos);
void c_action_allocatable_stmt(Token_t* label, Token_t* keyword, Token_t* eos)
#else
void c_action_allocatable_stmt(Token_t * label, Token_t * keyword, Token_t * eos, int count)
#endif
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
        {
#if ROSE_OFP_MINOR_VERSION_NUMBER >= 8 & ROSE_OFP_PATCH_VERSION_NUMBER >= 0
          printf ("In c_action_allocatable_stmt() label = %p = %s keyword = %p = %s \n",label,label != NULL ? label->text : "NULL",keyword,keyword != NULL ? keyword->text : "NULL");
#else
          printf ("In c_action_allocatable_stmt() label = %p = %s keyword = %p = %s count = %d \n",label,label != NULL ? label->text : "NULL",keyword,keyword != NULL ? keyword->text : "NULL",count);
#endif
        }

  // An AttributeSpecification statement can be the first statement in a program
  // (see test2007_147.f, the original Fortran I code from the IBM 704 Fortran Manual).
     build_implicit_program_statement_if_required();

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R520 c_action_allocatable_stmt()");
#endif

     buildAttributeSpecificationStatement(SgAttributeSpecificationStatement::e_allocatableStatement,label,keyword);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R520 c_action_allocatable_stmt()");
#endif
   }

/** R527-F2008
 * allocatable_decl
 *    : T_IDENT ( T_LPAREN array_spec T_RPAREN )?
 *              ( T_LBRACKET co_array_spec T_RBRACKET )?
 * 
 * @param id The name of the object
 * @param hasArraySpec True if an array_spec is present.
 * @param hasCoArraySpec True if a co_array_spec is present.
 */
void c_action_allocatable_decl(Token_t *id, ofp_bool hasArraySpec, ofp_bool hasCoArraySpec)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_allocatable_decl() id = %p = %s hasArraySpec = %s hasCoArraySpec = %s \n",id,id != NULL ? id->text : "NULL",hasArraySpec ? "true" : "false",hasCoArraySpec ? "true" : "false");

     SgVariableSymbol* variableSymbol = trace_back_through_parent_scopes_lookup_variable_symbol ( id->text , astScopeStack.front() );
     ROSE_ASSERT(variableSymbol != NULL);
     SgVarRefExp* variableReference = new SgVarRefExp(variableSymbol);
     setSourcePosition(variableReference,id);

     if (hasArraySpec == true)
        {
       // There was an index pushed on the stack (get the expression)
       // printf ("Get the expression from the astExpressionStack \n");
          ROSE_ASSERT(astExpressionStack.empty() == false);

          SgType* baseType = variableSymbol->get_type();
          ROSE_ASSERT(baseType != NULL);

       // DQ (12/8/2007): Use astBaseTypeStack instead of astTypeStack
       // astTypeStack.push_front(baseType);
          astBaseTypeStack.push_front(baseType);
#if 0
       // Output debugging information about saved state (stack) information.
          outputState("Before convertTypeOnStackToArrayType() in R527-F2008 c_action_allocatable_decl()");
#endif
          int count = 1;
          SgArrayType* arrayType = convertTypeOnStackToArrayType(count);

       // DQ (12/8/2007): Use astBaseTypeStack instead of astTypeStack
       // astTypeStack.pop_front();
          astBaseTypeStack.pop_front();

       // astTypeStack.push_front(arrayType);
#if 0
       // Output debugging information about saved state (stack) information.
          outputState("After convertTypeOnStackToArrayType() in R527-F2008 c_action_allocatable_decl()");
#endif
          SgInitializedName* initializedName = variableSymbol->get_declaration();
          ROSE_ASSERT(initializedName != NULL);

          initializedName->set_type(arrayType);

       // Mark this variable has having had its shape deferred (not specified in it's declaration)
          initializedName->set_shapeDeferred(true);

          astExpressionStack.push_front(variableReference);
#if 0
       // Output debugging information about saved state (stack) information.
          outputState("Before exiting as a test in R527-F2008 c_action_allocatable_decl()");
#endif
        }
       else
        {
       // This is all that there was (a simple name)
       // printf ("This is all that there was (a simple name) \n");
          ROSE_ASSERT(astExpressionStack.empty() == true);
          astExpressionStack.push_front(variableReference);
        }

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R527-F2008 c_action_allocatable_decl()");
#endif
   }

/** R521
 * asynchronous_stmt
 *
 * :    (label)? T_ASYNCHRONOUS ( T_COLON_COLON )?  generic_name_list T_EOS
 *
 * @param label The label.
 * @param keyword The ASYNCHRONOUS keyword token.
 * @param eos End of statement token.
 */
// void c_action_asynchronous_stmt(Token_t * label)
void c_action_asynchronous_stmt(Token_t * label, Token_t * keyword, Token_t * eos)
{
}

/** R522
 * bind_stmt
 *      :       (label)? language_binding_spec (T_COLON_COLON)? bind_entity_list T_EOS
 *
 * @param label Optional statement label
 * @param eos End of statement token.
 */
// void c_action_bind_stmt(Token_t * label)
void c_action_bind_stmt(Token_t * label, Token_t * eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_bind_stmt(): label = %p \n",label);

  // This could be the first statement in the program.
     build_implicit_program_statement_if_required();

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At MIDDLE of R522 c_action_bind_stmt()");
#endif

  // Since the keyword is missing form the API, until it is available use the eos.
     Token_t* keyword = eos;
     ROSE_ASSERT(keyword != NULL);
     buildAttributeSpecificationStatement(SgAttributeSpecificationStatement::e_bindStatement,label,keyword);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At END of R522 c_action_bind_stmt()");
#endif
   }

/** R523
 * bind_entity
 *      :       T_IDENT | T_SLASH T_IDENT T_SLASH
 *
 * @param entity The thing to bind.
 * @param isCommonBlockName True if the entity is the name of a common block
 */
void c_action_bind_entity(Token_t * entity, ofp_bool isCommonBlockName)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_bind_stmt(): entity = %p = %s isCommonBlockName = %s \n",entity,entity != NULL ? entity->text : "NULL",isCommonBlockName ? "true" : "false");

  // This could be the first statement in the program.
     build_implicit_program_statement_if_required();

     ROSE_ASSERT(entity != NULL);

     if (isCommonBlockName == true)
        {
          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
               printf ("Case of a common block (common blocks map to SgClassDeclaration IR node) \n");

          SgName name = entity->text;
          SgClassSymbol* classSymbol = trace_back_through_parent_scopes_lookup_derived_type_symbol(name,astScopeStack.front());

          if (classSymbol == NULL)
             {
               SgDerivedTypeStatement* derivedTypeStatement = buildDerivedTypeStatementAndDefinition(name,astScopeStack.front());

            // The source position is unknown, but later we can likely find the original declaration through the symbol table.
               setSourcePosition(derivedTypeStatement);
             }
        }
       else
        {
       // Handle this case using implicit typing rules
       // Push a name onto the astNameStack so that we can use c_action_data_ref to convert it into a variable on the astExpressionStack.
          astNameStack.push_front(entity);

       // This takes a name off of the name stack and puts a variable reference onto the astExpressionStack 
       // (building a declaration if required) types are computed using implicit type rules.
          c_action_data_ref(0);

          SgExpression* variableReference = astExpressionStack.front();
          astExpressionStack.pop_front();
          astNodeStack.push_front(variableReference);
        }

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At END of R523 c_action_bind_entity_list()");
#endif
   }

/** R523 list
 * bind_entity_list
 *      :       bind_entity ( T_COMMA bind_entity )*
 * 
 * @param count The number of items in the list.
 */
void c_action_bind_entity_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_bind_entity_list__begin() \n");
   }
void c_action_bind_entity_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_bind_entity_list(): count = %d \n",count);

  // This is all we know since there are additional entries on the astNameStack (bind_language and binding_label might be present for example)
  // printf ("astNameStack.size() = %zu \n",astNameStack.size());
     ROSE_ASSERT(astNameStack.size() >= (size_t)count);

  // buildAttributeSpecificationStatement(SgAttributeSpecificationStatement::e_bindStatement,label,keyword);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At END of R523 list c_action_bind_entity_list()");
#endif
   }

/** R524
 * data_stmt
 *
 * : (label)? T_DATA data_stmt_set ((T_COMMA)? data_stmt_set)* T_EOS
 * 
 * @param label The label.
 * @param keyword The DATA keyword token.
 * @param eos End of statement token.
 * @param count The number of data statement sets.
 */
// void c_action_data_stmt(Token_t * label, int count)
void c_action_data_stmt(Token_t * label, Token_t * keyword, Token_t * eos, int count)
   {
  // collect the data statement groups (sets) from the astNodeStack

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_data_stmt(): keyword = %p = %s count = %d \n",keyword,keyword != NULL ? keyword->text : "NULL",count);

  // An AttributeSpecification statement can be the first statement in a program
  // (see test2007_147.f, the original Fortran I code from the IBM 704 Fortran Manual).
     build_implicit_program_statement_if_required();

     ROSE_ASSERT(astNodeStack.size() == (size_t)count);

     buildAttributeSpecificationStatement(SgAttributeSpecificationStatement::e_dataStatement,label,keyword);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At END of R524 list c_action_data_stmt()");
#endif
   }

/**
 * R525
 * data_stmt_set
 *
 */
void c_action_data_stmt_set()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_data_stmt_set() \n");
   }

/**
 * R526
 * data_stmt_object
 *
 */
void c_action_data_stmt_object()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_data_stmt_object() \n");

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R526 c_action_data_stmt_object()");
#endif
   }

/** R526 list
 * data_stmt_object_list
 *      :       data_stmt_object ( T_COMMA data_stmt_object )*
 * 
 * @param count The number of items in the list.
 */
void c_action_data_stmt_object_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_data_stmt_object_list__begin() \n");
   }
void c_action_data_stmt_object_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_data_stmt_object_list(): count = %d \n",count);
#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R526 c_action_data_stmt_object_list()");
#endif

  // Accumulate these on the astNodeStack and then collect them later
     SgDataStatementGroup* dataGroup = new SgDataStatementGroup();
     astNodeStack.push_front(dataGroup);
     ROSE_ASSERT(dataGroup != NULL);

     SgDataStatementObject* dataObject = new SgDataStatementObject();
     ROSE_ASSERT(dataObject != NULL);

     if (dataObject->get_variableReference_list() == NULL)
        {
          SgExprListExp* exprList = new SgExprListExp();
          dataObject->set_variableReference_list(exprList);

          exprList->set_parent(dataObject);

          setSourcePosition(exprList);
        }

  // Output debugging information about saved state (stack) information.
  // outputState("At MIDDLE of R526 list c_action_data_stmt_object_list()");

     for (int i = 0; i < count; i++)
        {
          ROSE_ASSERT(astExpressionStack.empty() == false);
          SgExpression* variableReference = astExpressionStack.front();
          ROSE_ASSERT(variableReference != NULL);
          astExpressionStack.pop_front();
#if 0
       // Output debugging information about saved state (stack) information.
          outputState("At MIDDLE #2 of R526 list c_action_data_stmt_object_list()");
#endif
          ROSE_ASSERT(dataObject->get_variableReference_list() != NULL);
          dataObject->get_variableReference_list()->append_expression(variableReference);

       // printf ("Set parent of variableReference to dataObject \n");
          variableReference->set_parent(dataObject);
        }

  // printf ("Push dataObject onto astNodeStack \n");
  // astNodeStack.push_front(dataObject);
     dataGroup->get_object_list().push_back(dataObject);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At END of R526 list c_action_data_stmt_object_list()");
#endif
   }

/**
 * R527
 * data_implied_do
 *
 * @param id T_IDENT token.
 * @param hasThirdExpr Flag to specify if optional third expression was 
 * given.  True if expression given; false if not.
 */
void c_action_data_implied_do(Token_t *id, ofp_bool hasThirdExpr)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_data_implied_do(): id = %p = %s hasThirdExpr = %s \n",id,id != NULL ? id->text : "NULL",hasThirdExpr ? "true" : "false");

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R527 c_action_data_implied_do()");
#endif

     int numberOfImplicitDoLoopControlValues = 0;
     list<SgExpression*>::iterator i = astExpressionStack.begin();
     while ( i != astExpressionStack.end() && isSgExprListExp(*i) == NULL )
        {
          numberOfImplicitDoLoopControlValues++;
          i++;
        }

  // There should be a base, bound and optional increment, so either 2 or 3 entries
     ROSE_ASSERT(numberOfImplicitDoLoopControlValues == 2 || numberOfImplicitDoLoopControlValues == 3);

     SgExpression* increment = NULL;
     if (numberOfImplicitDoLoopControlValues == 3)
        {
          increment  = astExpressionStack.front();
          ROSE_ASSERT(increment != NULL);
          astExpressionStack.pop_front();
        }
       else
        {
          increment  = new SgNullExpression();
          ROSE_ASSERT(increment != NULL);
          setSourcePosition(increment);
        }
     
     SgExpression* upperBound = astExpressionStack.front();
     ROSE_ASSERT(upperBound != NULL);
     astExpressionStack.pop_front();

     SgExpression* lowerBound = astExpressionStack.front();
     ROSE_ASSERT(lowerBound != NULL);
     astExpressionStack.pop_front();

  // This is not available from OFP, so we might have to dig for it later.
     printf ("Warning: implied do loop variable is not availble in OFP \n");
     SgVarRefExp*  doLoopVar  = NULL;
     ROSE_ASSERT(doLoopVar != NULL);

     SgExprListExp* objectList = isSgExprListExp(astExpressionStack.front());
     astExpressionStack.pop_front();

     setSourcePosition(objectList);

     SgScopeStatement* implied_do_scope = NULL; // new SgBasicBlock();
  // SgImpliedDo* impliedDo = new SgImpliedDo(doLoopVar,lowerBound,upperBound,increment,objectList);
  // SgImpliedDo* impliedDo = new SgImpliedDo(doLoopVar,lowerBound,upperBound,increment,objectList,implied_do_scope);
     SgImpliedDo* impliedDo = new SgImpliedDo(doLoopVar,upperBound,increment,objectList,implied_do_scope);
     setSourcePosition(impliedDo);

     objectList->set_parent(impliedDo);
     upperBound->set_parent(impliedDo);
     lowerBound->set_parent(impliedDo);

     astExpressionStack.push_front(impliedDo);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R527 c_action_data_implied_do()");
#endif
   }

// DQ (4/5/2010): Added F08 support specific to OFP 0.8.0
#if ROSE_OFP_MINOR_VERSION_NUMBER >= 8 & ROSE_OFP_PATCH_VERSION_NUMBER >= 0
   /** R527-F08 list
    * allocatable_decl_list
    *   :   allocatable_decl ( T_COMMA allocatable_decl )*
    * 
    * @param count The number of items in the list.
    */
// public abstract void allocatable_decl_list__begin();
// public abstract void allocatable_decl_list(int count);
void c_action_allocatable_decl_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_allocatable_decl_list__begin() \n");
   }

void c_action_allocatable_decl_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_allocatable_decl_list(): count = %d \n",count);
   }
#endif

/**
 * R528
 * data_i_do_object
 *
 */
void c_action_data_i_do_object()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_data_i_do_object() \n");
#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R528 c_action_data_i_do_object()");
#endif
   }


/** R528 list
 * data_i_do_object_list
 *      :       data_i_do_object ( T_COMMA data_i_do_object )*
 * 
 * @param count The number of items in the list.
 */
void c_action_data_i_do_object_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_data_i_do_object_list__begin() \n");
   }
void c_action_data_i_do_object_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_data_i_do_object_list(): count = %d \n",count);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R528 c_action_data_i_do_object_list()");
#endif

  // The code implemented here is different from the code in R918, because we have a 
  //    c_action_data_i_do_object_list(int count)
  // function available and so we don't have to accumulate the SgExprListExp
  // on the top of the stack as we process each object list entry.
     SgExprListExp* implied_do_object_list = new SgExprListExp();

     for (int i=0; i < count; i++)
        {
          ROSE_ASSERT(astExpressionStack.empty() == false);
          SgExpression* implied_do_object = astExpressionStack.front();
          astExpressionStack.pop_front();

       // Add (accumlate) the implied_do_object into the implied_do_object_list
          implied_do_object_list->append_expression(implied_do_object);
        }
     
  // Push both back onto the stack
     astExpressionStack.push_front(implied_do_object_list);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R528 c_action_data_i_do_object_list()");
#endif
   }

/**
 * R530
 * data_stmt_value
 *
 * TODO: This action method may need params.  Look at the grammar rule.
 */
// void c_action_data_stmt_value()
void c_action_data_stmt_value(Token_t *asterisk)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_data_stmt_value(): asterisk = %p = %s \n",asterisk,asterisk != NULL ? asterisk->text : "NULL");
#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R530 c_action_data_stmt_value()");
#endif

     SgDataStatementGroup* dataGroup = NULL;
     ROSE_ASSERT(astNodeStack.empty() == false);
     if (astNodeStack.empty() == true)
        {
          dataGroup = new SgDataStatementGroup();
          astNodeStack.push_front(dataGroup);
        }
       else
        {
          dataGroup = isSgDataStatementGroup(astNodeStack.front());
          ROSE_ASSERT(dataGroup != NULL);
        }
     ROSE_ASSERT(dataGroup != NULL);

     SgDataStatementValue* dataValue = new SgDataStatementValue();

     if (dataValue->get_initializer_list() == NULL)
        {
          SgExprListExp* exprList = new SgExprListExp();
          dataValue->set_initializer_list(exprList);

          exprList->set_parent(dataValue);

          setSourcePosition(exprList);
        }

     if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
          printf ("astExpressionStack.size() = %zu \n",astExpressionStack.size());

     if (astExpressionStack.size() == 0)
        {
          printf ("Error: astExpressionStack.size() == 0 \n");
          ROSE_ASSERT(false);
        }
       else
        {
          if (astExpressionStack.size() == 1)
             {
               if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                    printf ("Case of SgDataStatementValue::e_explict_list (expression, typically a simple value) \n");

               dataValue->set_data_initialization_format(SgDataStatementValue::e_explict_list);

            // Make sure this is not an implied DO expression
               ROSE_ASSERT(isSgImpliedDo(astExpressionStack.front()) == NULL);
             }
            else
             {
               if (astExpressionStack.size() == 2)
                  {
                    if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                         printf ("Case of SgDataStatementValue::e_implicit_list (repeat * value) \n");

                    dataValue->set_data_initialization_format(SgDataStatementValue::e_implicit_list);
                  }
                 else
                  {
                 // See test2007_107.f90
                    printf ("This might be an implied do loop (need an example to debug this case!) \n");
                    ROSE_ASSERT(false);
                  }
             }
        }

     switch (dataValue->get_data_initialization_format())
        {
          case SgDataStatementValue::e_explict_list:
             {
               SgExpression* expression = astExpressionStack.front();
               astExpressionStack.pop_front();
               dataValue->get_initializer_list()->append_expression(expression);
               break;
             }

          case SgDataStatementValue::e_implicit_list:
             {
            // Get both values on the astExpression stack.
               SgExpression* constant_expression = astExpressionStack.front();
               dataValue->set_constant_expression(constant_expression);
               constant_expression->set_parent(dataValue);
               astExpressionStack.pop_front();

               SgExpression* repeat_expression = astExpressionStack.front();
               dataValue->set_repeat_expression(repeat_expression);
               repeat_expression->set_parent(dataValue);
               astExpressionStack.pop_front();
               break;
             }

          default:
             {
               printf ("Error: default reached dataValue->get_data_initialization_format() = %d \n",dataValue->get_data_initialization_format());
               ROSE_ASSERT(false);
             }
        }

  // We should have a SgDataStatementObject at the top of the astNodeStack...
  // SgDataStatementObject* dataObject = isSgDataStatementObject(astNodeStack.front());
  // ROSE_ASSERT(dataObject != NULL);

     dataGroup->get_value_list().push_back(dataValue);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R530 c_action_data_stmt_value()");
#endif
   }

/** R530 list
 * data_stmt_value_list
 *      :       data_stmt_value ( T_COMMA data_stmt_value )*
 * 
 * @param count The number of items in the list.
 */
void c_action_data_stmt_value_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_data_stmt_value_list__begin() \n");
#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R530 (list: begin) c_action_data_stmt_value_list_begin()");
#endif
   }
void c_action_data_stmt_value_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_data_stmt_value_list(): count = %d \n",count);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R530 (list: end) c_action_data_stmt_value_list()");
#endif
   }

/**
 * R531
 * scalar_int_constant
 *
 */
void c_action_scalar_int_constant()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_scalar_int_constant() \n");
   }

/**
 * Generated rule.
 * hollerith_constant
 *
 * @param hollerithConstant T_HOLLERITH token.
 */
void c_action_hollerith_constant(Token_t *hollerithConstant)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_hollerith_constant() hollerithConstant = %p = %s \n",hollerithConstant,hollerithConstant != NULL ? hollerithConstant->text : "NULL");
   }

/**
 * R532
 * data_stmt_constant
 *
 */
void c_action_data_stmt_constant()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_data_stmt_constant() \n");
   }

/** R535
 * dimension_stmt
 *
 * (label)? T_DIMENSION ( T_COLON_COLON )? dimension_decl 
 *                      ( T_COMMA dimension_decl {count++;})* T_EOS
 * 
 * @param label The label.
 * @param keyword The DIMENSION keyword token.
 * @param eos End of statement token.
 * @param count The number of dimension declarations.
 */
// void c_action_dimension_stmt(Token_t * label, int count)
void c_action_dimension_stmt(Token_t * label, Token_t * keyword, Token_t * eos, int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_dimension_stmt() label = %p count = %d \n",label,count);

  // An AttributeSpecification statement can be the first statement in a program
  // (see test2007_147.f, the original Fortran I code from the IBM 704 Fortran Manual).
     build_implicit_program_statement_if_required();

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R535 c_action_dimension_stmt()");
#endif

  // This statement's semantics is that it should change the type of the declared variables
     buildAttributeSpecificationStatement(SgAttributeSpecificationStatement::e_dimensionStatement,label,keyword);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R535 c_action_dimension_stmt()");
#endif
   }

/** R544-F2008 (extracted from R535-F2003)
 * dimension_decl
 *
 * :  T_IDENT ( T_LPAREN array_spec T_RPAREN )? 
 *      ( T_LBRACKET co_array_spec T_RBRACKET )?
 *
 * @param id Identifier (e.g., array name).
 * @param hasArraySpec True if has an array spec.
 * @param hasCoArraySpec True if has a co array spec.
 */
#if ROSE_OFP_MINOR_VERSION_NUMBER >= 8 & ROSE_OFP_PATCH_VERSION_NUMBER >= 0
 // public abstract void codimension_decl(Token coarrayName, Token lbracket, Token rbracket);
// void c_action_dimension_decl(Token_t *id, Token_t *lbracket, Token_t *rbracket)
void c_action_dimension_decl(Token_t *id)
#else
void c_action_dimension_decl(Token_t *id, ofp_bool hasArraySpec, ofp_bool hasCoArraySpec)
#endif
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
        {
#if ROSE_OFP_MINOR_VERSION_NUMBER >= 8 & ROSE_OFP_PATCH_VERSION_NUMBER >= 0
       // printf ("In R544-F2008 (extracted from R535-F2003) c_action_dimension_decl(): id = %p = %s lbracket = %p = %s rbracket = %p = %s \n",
       //          id,id != NULL ? id->text : "NULL",lbracket,lbracket != NULL ? lbracket->text : "NULL",rbracket,rbracket != NULL ? rbracket->text : "NULL");
          printf ("In R544-F2008 (extracted from R535-F2003) c_action_dimension_decl(): id = %p = %s \n",id,id != NULL ? id->text : "NULL");
#else
          printf ("In R544-F2008 (extracted from R535-F2003) c_action_dimension_decl(): id = %p = %s hasArraySpec = %s hasCoArraySpec = %s \n",
                   id,id != NULL ? id->text : "NULL",hasArraySpec ? "true" : "false",hasCoArraySpec ? "true" : "false");
#endif
        }

  // A dimension statement can be the first statement in a problem.
     build_implicit_program_statement_if_required();

     ROSE_ASSERT(id != NULL);
  // astNameStack.push_front(id);

  // We need to turn this into a declaration
  // printf ("We need to turn the named array into an array declaration at this point (variable = %s) \n",id->text);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R544-F2008 c_action_dimension_decl()");
#endif

     SgName arrayName = id->text;

  // DQ (1/18/2011): This detects where we have used the semantics of implicitly building symbols for implicit variables.
  // printf ("WARNING: This use of trace_back_through_parent_scopes_lookup_variable_symbol() used the side effect of building a symbol if the reference is not found! \n");
  // ROSE_ASSERT(false);

  // We need to get a base type for the array.
     SgVariableSymbol* arraySymbol = trace_back_through_parent_scopes_lookup_variable_symbol(arrayName,astScopeStack.front());
     if (arraySymbol == NULL)
        {
#if 1
       // DQ (1/19/2011): Build the implicit variable
          buildImplicitVariableDeclaration(arrayName);

       // Now verify that it is present.
          arraySymbol = trace_back_through_parent_scopes_lookup_variable_symbol(arrayName,astScopeStack.front());
          ROSE_ASSERT(arraySymbol != NULL);
#else
       // The array was not previously declared, but a declaration has been built (by the
       // first call to trace_back_through_parent_scopes_lookup_variable_symbol() and it is 
       // implicitly typed as a variable which we will still have to convert to an array.
          arraySymbol = trace_back_through_parent_scopes_lookup_variable_symbol(arrayName,astScopeStack.front());
          ROSE_ASSERT(arraySymbol != NULL);
#endif
        }
       else
        {
       // This was previously declared
       // printf ("The variable was previously declarted name = %s \n",arrayName.str());
        }

  // Now we have a scalar variable declaration, and we can process the type into an array type.
     SgInitializedName* arrayVariable = arraySymbol->get_declaration();
     ROSE_ASSERT(arrayVariable != NULL);

     SgType* arrayVariableBaseType = arrayVariable->get_type();
     ROSE_ASSERT(arrayVariableBaseType != NULL);

  // printf ("arrayVariableBaseType = %s \n",arrayVariableBaseType->class_name().c_str());

     astBaseTypeStack.push_front(arrayVariableBaseType);
          
  // This needs to count down to the first SgPntrArrRefExp.
     int numberOfIndexExpressions = 0;
     std::list<SgExpression*>::iterator stackEntry = astExpressionStack.begin();
     while (stackEntry != astExpressionStack.end() && isSgPntrArrRefExp(*stackEntry) == NULL)
        {
          numberOfIndexExpressions++;
          stackEntry++;
        }
     int rank = numberOfIndexExpressions;
  // printf ("Array rank = %d \n",rank);

     SgArrayType* arrayVariableType = convertTypeOnStackToArrayType(rank);
  // printf ("arrayVariableType = %s \n",arrayVariableType->class_name().c_str());

     arrayVariable->set_type(arrayVariableType);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("Branch 2: building an array reference in R544-F2008 c_action_dimension_decl()");
#endif

     SgVarRefExp* variableReference = new SgVarRefExp(arraySymbol);
     setSourcePosition(variableReference,id);

     SgTreeCopy copyHelp;
     SgNode* expressionListNode = arrayVariableType->get_dim_info()->copy(copyHelp);
     ROSE_ASSERT(expressionListNode != NULL);

     SgExprListExp* expressionList = isSgExprListExp(expressionListNode);
     ROSE_ASSERT(expressionList != NULL);

     SgPntrArrRefExp* arrayRefExpression = new SgPntrArrRefExp(variableReference,expressionList,NULL);
     ROSE_ASSERT(arrayRefExpression != NULL);
     setSourcePosition(arrayRefExpression,id);

     astExpressionStack.push_front(arrayRefExpression);

     astBaseTypeStack.pop_front();

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R544-F2008 c_action_dimension_decl()");
#endif
   }

/**
 * R509-F2008
 * dimension_spec
 *
 * @param dimensionKeyword T_DIMENSION token.
 */
void c_action_dimension_spec(Token_t *dimensionKeyword)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R509-F2008 c_action_dimension_spec(): \n");
   }

/** R536
 * intent_stmt
 *
 * (label)? T_INTENT T_LPAREN intent_spec T_RPAREN ( T_COLON_COLON )? 
 *              generic_name_list T_EOS
 *
 * @param label The label.
 * @param keyword The INTENT keyword token.
 * @param eos End of statement token.
 */
// void c_action_intent_stmt(Token_t * label)
void c_action_intent_stmt(Token_t * label, Token_t * keyword, Token_t * eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_intent_spec() label = %p = %s keyword = %p = %s \n",label,label != NULL ? label->text : "NULL",keyword,keyword != NULL ? keyword->text : "NULL");

  // An AttributeSpecification statement can be the first statement in a program
  // (see test2007_147.f, the original Fortran I code from the IBM 704 Fortran Manual).
     build_implicit_program_statement_if_required();

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R536 c_action_intent_stmt()");
#endif

     buildAttributeSpecificationStatement(SgAttributeSpecificationStatement::e_intentStatement,label,keyword);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R536 c_action_intent_stmt()");
#endif
   }

/** R537
 * optional_stmt
 *
 * : (label)? T_OPTIONAL ( T_COLON_COLON )? generic_name_list T_EOS
 *
 * @param label The label.
 * @param keyword The OPTIONAL keyword token.
 * @param eos End of statement token.
 */
// void c_action_optional_stmt(Token_t * label)
void c_action_optional_stmt(Token_t * label, Token_t * keyword, Token_t * eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_optional_stmt(): label = %p \n",label);

  // An AttributeSpecification statement can be the first statement in a program
  // (see test2007_147.f, the original Fortran I code from the IBM 704 Fortran Manual).
     build_implicit_program_statement_if_required();

     buildAttributeSpecificationStatement(SgAttributeSpecificationStatement::e_optionalStatement,label,keyword);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R537 c_action_optional_stmt()");
#endif
   }

/** R538
 * parameter_stmt
 *
 * : (label)? T_PARAMETER T_LPAREN named_constant_def_list T_RPAREN T_EOS
 *
 * @param label The label.
 * @param keyword The PARAMETER keyword token.
 * @param eos End of statement token.
 */
// void c_action_parameter_stmt(Token_t * label)
void c_action_parameter_stmt(Token_t * label, Token_t * keyword, Token_t * eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_parameter_stmt(): keyword = %p = %s \n",keyword,keyword != NULL ? keyword->text : "NULL");

  // An AttributeSpecification statement can be the first statement in a program
  // (see test2007_147.f, the original Fortran I code from the IBM 704 Fortran Manual).
     build_implicit_program_statement_if_required();

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R538 c_action_parameter_stmt()");
#endif

     buildAttributeSpecificationStatement(SgAttributeSpecificationStatement::e_parameterStatement,label,keyword);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R538 c_action_parameter_stmt()");
#endif
   }

/** R539 list
 * named_constant_def_list
 *      :       named_constant_def ( T_COMMA named_constant_def )*
 * 
 * @param count The number of items in the list.
 */
void c_action_named_constant_def_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_named_constant_def_list__begin() \n");
   }
void c_action_named_constant_def_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_named_constant_def_list(): count = %d \n",count);
   }

/** R539
 * named_constant_def
 *
 * :  T_IDENT T_EQUALS expr
 *
 * @param id Identifier if present.
 */
void c_action_named_constant_def(Token_t *id)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_named_constant_def(): id = %p = %s \n",id,id != NULL ? id->text : "NULL");

     ROSE_ASSERT(id != NULL);

  // An AttributeSpecification statement can be the first statement in a program
  // (see test2007_213.f, if so then this can be one of the first c_action statements called.
     build_implicit_program_statement_if_required();

  // Lookup the variable defined by "id" and build a variable reference using it
     SgName name = id->text;

  // DQ (1/18/2011): This detects where we have used the semantics of implicitly building symbols for implicit variables.
  // printf ("WARNING: This use of trace_back_through_parent_scopes_lookup_variable_symbol() used the side effect of building a symbol if the reference is not found! \n");
  // ROSE_ASSERT(false);

     SgVariableSymbol* variableSymbol = trace_back_through_parent_scopes_lookup_variable_symbol(name,astScopeStack.front());

     if (variableSymbol == NULL)
        {
#if 1
       // DQ (1/19/2011): Build the implicit variable
          buildImplicitVariableDeclaration(name);

       // Now verify that it is present.
          variableSymbol = trace_back_through_parent_scopes_lookup_variable_symbol(name,astScopeStack.front());
          ROSE_ASSERT(variableSymbol != NULL);
#else
       // If it was NULL the first time then it was at least built as a variable so ask a second time to return the symbol.
          variableSymbol = trace_back_through_parent_scopes_lookup_variable_symbol(name,astScopeStack.front());
          ROSE_ASSERT(variableSymbol != NULL);
#endif
        }

     SgVarRefExp* variableReference = new SgVarRefExp(variableSymbol);
     ROSE_ASSERT(variableReference != NULL);

     setSourcePosition(variableReference,id);

     SgExpression* rhs = astExpressionStack.front();
     astExpressionStack.pop_front();

     SgAssignOp* assignmentExpression = new SgAssignOp(variableReference,rhs,NULL);
     ROSE_ASSERT(assignmentExpression != NULL);

  // DQ (1/23/2008): Set the source position to the only availalbe reference point!
  // setSourcePosition(assignmentExpression);
     setSourcePosition(assignmentExpression,id);

  // After we push this we have a pair of expressions on the stack (we could process it here of later).
     astExpressionStack.push_front(assignmentExpression);

  // Build the expression "id" = "current expression on astExpressionStack, attach it to the SgAttributeSpecificationStatement

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R539 c_action_named_constant_def()");
#endif
   }

/** R540
 * pointer_stmt
 *      :       (label)? T_POINTER ( T_COLON_COLON )? pointer_decl_list T_EOS
 *
 * @param label The label.
 * @param keyword The POINTER keyword token.
 * @param eos End of statement token.
 */
// void c_action_pointer_stmt(Token_t * label)
void c_action_pointer_stmt(Token_t * label, Token_t * keyword, Token_t * eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_pointer_stmt(): label = %p = %s keyword = %p = %s \n",label,label != NULL ? label->text : "NULL",keyword,keyword != NULL ? keyword->text : "NULL");

  // An AttributeSpecification statement can be the first statement in a program
  // (see test2007_147.f, the original Fortran I code from the IBM 704 Fortran Manual).
     build_implicit_program_statement_if_required();

     ROSE_ASSERT(keyword != NULL);
     buildAttributeSpecificationStatement(SgAttributeSpecificationStatement::e_pointerStatement,label,keyword);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R540 c_action_pointer_stmt()");
#endif
   }

/** R541 list
 * pointer_decl_list
 *      :       pointer_decl ( T_COMMA pointer_decl )*
 * 
 * @param count The number of items in the list.
 */
void c_action_pointer_decl_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_pointer_decl_list__begin() \n");
   }
void c_action_pointer_decl_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_pointer_decl_list() \n");
   }

/** R541
 * pointer_decl
 *      :  T_IDENT ( T_LPAREN deferred_shape_spec_list T_RPAREN )?
 *
 * @param id Identifier.
 * @param hasSpecList True if has a deferred shape spec list.
 */
void c_action_pointer_decl(Token_t *id, ofp_bool hasSpecList)
   {
  // I think this is part of the Cray Fortran pointer support (or it uses the same rule).

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_pointer_decl(): id = %p = %s hasSpecList = %s \n",id,id != NULL ? id->text : "NULL",hasSpecList ? "true" : "false");

     ROSE_ASSERT(id != NULL);
     astNameStack.push_front(id);
   }

/** R542
 * protected_stmt
 *      :       (label)? T_PROTECTED ( T_COLON_COLON )? generic_name_list T_EOS
 *
 * @param label The label.
 * @param keyword The PROTECTED keyword token.
 * @param eos End of statement token.
 */
// void c_action_protected_stmt(Token_t * label)
void c_action_protected_stmt(Token_t * label, Token_t * keyword, Token_t * eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_protected_stmt(): keyword = %p = %s \n",keyword,keyword != NULL ? keyword->text : "NULL");

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R542 c_action_protected_stmt()");
#endif

  // An AttributeSpecification statement can be the first statement in a program
  // (see test2007_147.f, the original Fortran I code from the IBM 704 Fortran Manual).
     build_implicit_program_statement_if_required();

     ROSE_ASSERT(keyword != NULL);
     buildAttributeSpecificationStatement(SgAttributeSpecificationStatement::e_protectedStatement,label,keyword);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R542 c_action_protected_stmt()");
#endif
   }

/** R543
 * save_stmt
 *      : (label T_SAVE ( ( T_COLON_COLON )? saved_entity_list )? T_EOS
 *
 * @param label The label.
 * @param keyword The SAVE keyword token.
 * @param eos End of statement token.
 * @param hasSavedEntityList True if has saved-entity-list.
 */
// void c_action_save_stmt(Token_t * label,ofp_bool hasSavedEntityList)
void c_action_save_stmt(Token_t * label, Token_t * keyword, Token_t * eos, ofp_bool hasSavedEntityList)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_save_stmt(): keyword = %p = %s hasSavedEntityList = %s \n",keyword,keyword != NULL ? keyword->text : "NULL",hasSavedEntityList ? "true" : "false");

  // An AttributeSpecification statement can be the first statement in a program
  // (see test2007_147.f, the original Fortran I code from the IBM 704 Fortran Manual).
     build_implicit_program_statement_if_required();

     ROSE_ASSERT(keyword != NULL);
     buildAttributeSpecificationStatement(SgAttributeSpecificationStatement::e_saveStatement,label,keyword);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R543 c_action_save_stmt()");
#endif
   }

/** R544 list
 * saved_entity_list
 *      :       saved_entity ( T_COMMA saved_entity )*
 * 
 * @param count The number of items in the list.
 */
void c_action_saved_entity_list__begin()
   {
  // I don't think I need these
   }
void c_action_saved_entity_list(int count)
   {
  // I don't think I need these
   }

/** R544
 * saved_entity
 *
 * : T_IDENT
 * | T_SLASH T_IDENT T_SLASH
 *
 * @param id Identifier.
 * @param isCommonBlockHame True if identifier is a common block name.
 */
void c_action_saved_entity(Token_t *id, ofp_bool isCommonBlockName)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_saved_entity(): id = %p = %s isCommonBlockName = %s \n",id,id != NULL ? id->text : "NULL",isCommonBlockName ? "true" : "false");

     ROSE_ASSERT(id != NULL);
     astNameStack.push_front(id);
   }

/** R546
 * target_stmt
 *      
 * : (label)? T_TARGET ( T_COLON_COLON )? target_decl 
 *                      ( T_COMMA target_decl)* T_EOS
 * 
 * @param label The label.
 * @param keyword The SAVE keyword token.
 * @param eos End of statement token.
 * @param count The number of target declarations.
 */
// void c_action_target_stmt(Token_t * label, int count)
#if ROSE_OFP_MINOR_VERSION_NUMBER >= 8 & ROSE_OFP_PATCH_VERSION_NUMBER >= 0
void c_action_target_stmt(Token_t * label, Token_t * keyword, Token_t * eos)
#else
void c_action_target_stmt(Token_t * label, Token_t * keyword, Token_t * eos, int count)
#endif
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
        {
#if ROSE_OFP_MINOR_VERSION_NUMBER >= 8 & ROSE_OFP_PATCH_VERSION_NUMBER >= 0
          printf ("In c_action_target_stmt(): keyword = %p = %s \n",keyword,keyword != NULL ? keyword->text : "NULL");
#else
          printf ("In c_action_target_stmt(): keyword = %p = %s count = %d \n",keyword,keyword != NULL ? keyword->text : "NULL",count);
#endif
        }
      

  // An AttributeSpecification statement can be the first statement in a program
  // (see test2007_147.f, the original Fortran I code from the IBM 704 Fortran Manual).
     build_implicit_program_statement_if_required();

     ROSE_ASSERT(keyword != NULL);
     buildAttributeSpecificationStatement(SgAttributeSpecificationStatement::e_targetStatement,label,keyword);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R546 c_action_target_stmt()");
#endif
   }

/** R556-F2008
 * target_decl
 *
 * : T_IDENT (T_LPAREN array_spec T_RPAREN)?
 *      (T_LBRACKET co_array_spec T_RBRACKET)?
 *
 * @param id Identifier.
 * @param hasArraySpec True if has an array spec.
 * @param hasCoArraySpec True if has a co array spec.
 */
void c_action_target_decl(Token_t *id, ofp_bool hasArraySpec, ofp_bool hasCoArraySpec)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_target_decl(): id = %p = %s hasArraySpec = %s hasCoArraySpec = %s \n",id,id != NULL ? id->text : "NULL",hasArraySpec ? "true" : "false",hasCoArraySpec ? "true" : "false");

     ROSE_ASSERT(id != NULL);
     astNameStack.push_front(id);
   }

/** R547
 * value_stmt
 *
 *      (label)? T_VALUE ( T_COLON_COLON )? generic_name_list T_EOS
 *
 * @param label The label.
 * @param keyword The VALUE keyword token.
 * @param eos End of statement token.
 */
// void c_action_value_stmt(Token_t * label)
void c_action_value_stmt(Token_t * label, Token_t * keyword, Token_t * eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_value_stmt(): keyword = %p = %s \n",keyword,keyword != NULL ? keyword->text : "NULL");

  // An AttributeSpecification statement can be the first statement in a program
  // (see test2007_147.f, the original Fortran I code from the IBM 704 Fortran Manual).
     build_implicit_program_statement_if_required();

     ROSE_ASSERT(keyword != NULL);
     buildAttributeSpecificationStatement(SgAttributeSpecificationStatement::e_valueStatement,label,keyword);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R547 c_action_value_stmt()");
#endif
   }

/** R548
 * volatile_stmt
 *
 *      (label)? T_VOLATILE ( T_COLON_COLON )? generic_name_list T_EOS
 *
 * @param label The label.
 * @param keyword The VOLATILE keyword token.
 * @param eos End of statement token.
 */
// void c_action_volatile_stmt(Token_t * label)
void c_action_volatile_stmt(Token_t * label, Token_t * keyword, Token_t * eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_volatile(): keyword = %p = %s \n",keyword,keyword != NULL ? keyword->text : "NULL");

  // An AttributeSpecification statement can be the first statement in a program
  // (see test2007_147.f, the original Fortran I code from the IBM 704 Fortran Manual).
     build_implicit_program_statement_if_required();

     ROSE_ASSERT(keyword != NULL);
     buildAttributeSpecificationStatement(SgAttributeSpecificationStatement::e_volatileStatement,label,keyword);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R548 c_action_volatile_stmt()");
#endif
   }

/** R549
 * implicit_stmt
 *
 * :    (label)? T_IMPLICIT implicit_spec_list T_EOS
 * |    (label)? T_IMPLICIT T_NONE T_EOS
 *
 * @param label The label.
 * @param implicitKeyword Token for the T_IMPLICIT.
 * @param noneKeyword Token for T_NONE, if applicable; null otherwise.
 * @param eos T_EOS.
 * @param hasImplicitSpecList Could be none, or it could have a list.
 */
// void c_action_implicit_stmt(Token_t * label,ofp_bool hasImplicitSpecList)
void c_action_implicit_stmt(Token_t * label, Token_t * implicitKeyword, Token_t * noneKeyword, Token_t * eos, ofp_bool hasImplicitSpecList)
   {
  // This is the case of an IMPICIT NONE statement
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_implicit_stmt(label = %p, hasImplicitSpecList = %s) \n",label,hasImplicitSpecList ? "true" : "false");

#if !SKIP_C_ACTION_IMPLEMENTATION
     build_implicit_program_statement_if_required();

     bool isImplicitNone = (hasImplicitSpecList == false);
     SgImplicitStatement* implicitStatement = new SgImplicitStatement(isImplicitNone);

  // There is nothing to use to set the position of the SgImplicitStatement
  // Send email about this interesting example where no source position information is available (once I get my email back up!).
     ROSE_ASSERT(implicitKeyword != NULL);
  // setSourcePosition(implicitStatement);
     setSourcePosition(implicitStatement,implicitKeyword);

  // Now add the implicit declaration to the current scope!
  // ROSE_ASSERT(astScopeStack.empty() == false);
  // SgScopeStatement* topOfStack = *(astScopeStack.begin());
  // printf ("topOfStack = %p = %s \n",topOfStack,topOfStack->class_name().c_str());
  // ROSE_ASSERT(getTopOfScopeStack()->variantT() == V_SgBasicBlock);
     ROSE_ASSERT(getTopOfScopeStack()->variantT() == V_SgBasicBlock || getTopOfScopeStack()->variantT() == V_SgClassDefinition);
     getTopOfScopeStack()->append_statement(implicitStatement);
#endif

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("Before clearing stack for unimplemented implicit statement in R549 c_action_implicit_stmt()");
#endif

#if 1
  // DQ (12/2/2010): Now that we have cleaned up the stack we can proceed (but check the stack).
     ROSE_ASSERT (astBaseTypeStack.empty() == true);
#else
  // Since we have not finished the implementation of the implicit statement (for cases other than 
  // "implicit none") drain the stack to avoid leaving trach on the stack.
     if (astBaseTypeStack.empty() == false)
        {
          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
               printf ("WARNING: unfinished implicit statement (for not implicit none case) base types being cleared from stack! (implicit) \n");
          while (astBaseTypeStack.empty() == false)
             {
               astBaseTypeStack.pop_front();
             }
        }
#endif

     if (astNodeStack.empty() == false)
        {
          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
               printf ("WARNING: unfinished implicit statement (for not implicit none case) base types being cleared from stack! (implicit) \n");
          SgInitializedNamePtrList tempStack;
          while (astNodeStack.empty() == false)
             {
               SgInitializedName* implicitTypeName = isSgInitializedName(astNodeStack.front());
               ROSE_ASSERT(implicitTypeName != NULL);
               tempStack.push_back(implicitTypeName);
               astNodeStack.pop_front();
             }

       // SgInitializedNamePtrList::iterator i = tempStack.begin();
          while (tempStack.empty() == false)
             {
               SgInitializedName* implicitTypeName = isSgInitializedName(tempStack.back());
               ROSE_ASSERT(implicitTypeName != NULL);
               implicitStatement->get_variables().push_back(implicitTypeName);
               tempStack.pop_back();
             }
        }

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R549 c_action_implicit_stmt()");
#endif
#if 0
     printf ("Exiting after clearing the stack as a move to skip the implementation of more complex implicit statements. \n");
     ROSE_ASSERT(false);
#endif
   }

/** 
 * R550
 *
 * implict-spec
 * 
 * : declaration-type-spec ( letter-spec-list )
 *
 */
void c_action_implicit_spec()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R550 c_action_implicit_spec() \n");
   }

/** R550 list
 * implicit_spec_list
 *      :       implicit_spec ( T_COMMA implicit_spec )*
 * 
 * @param count The number of items in the list.
 */
void c_action_implicit_spec_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R550 c_action_implicit_spec_list__begin() \n");
   }
void c_action_implicit_spec_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R550 c_action_implicit_spec_list(): count = %d \n",count);
   }

/**
 * R551
 * 
 * letter-spec
 *
 * : letter [ - letter ]
 *
 * @param id1 Token for the required T_IDENT
 * @param id2 Token for the optional T_IDENT; null if wasn't provided.
 */
void c_action_letter_spec(Token_t * id1, Token_t * id2)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_letter_spec(): id1 = %p = %s id2 = %p = %s \n",id1, id1 != NULL ? id1->text : "NULL",id2, id2 != NULL ? id2->text : "NULL");

  // There should be at least one character in the range.
     ROSE_ASSERT(id1 != NULL);
     ROSE_ASSERT(id1->text != NULL);
     SgName implicitCharacerRange = id1->text;

     if (id2 != NULL)
        {
          implicitCharacerRange += "-";
          implicitCharacerRange += id2->text;
        }

  // printf ("implicitCharacerRange = %s \n",implicitCharacerRange.str());

     SgType* implicitType = NULL;
     ROSE_ASSERT(astBaseTypeStack.empty() == false);
     if (astBaseTypeStack.empty() == false)
        {
          implicitType = astBaseTypeStack.front();
       // astBaseTypeStack.pop_front();
        }
     ROSE_ASSERT(implicitType != NULL);

  // We will use a SgInitializedName to bind a type with a name (using implicitCharacerRangePtr as the name).
     SgInitializedName* implicitTypeName = SageBuilder::buildInitializedName (implicitCharacerRange,implicitType);
     ROSE_ASSERT(implicitTypeName != NULL);

  // SgInitializedName IR nodes carry a scope which should always be initialized.
     implicitTypeName->set_scope(getTopOfScopeStack());

     astNodeStack.push_front(implicitTypeName);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R552 c_action_letter_spec()");
#endif
   }

/** R551 list
 * letter_spec_list
 *      :       letter_spec ( T_COMMA letter_spec )*
 * 
 * @param count The number of items in the list.
 */
void c_action_letter_spec_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_letter_spec_list__begin() \n");
   }
void c_action_letter_spec_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_letter_spec_list(): count = %d \n",count);

  // The base type can be used across multiple implicitCharacerRange, so we need only clean it up once.
     ROSE_ASSERT(astBaseTypeStack.empty() == false);
     astBaseTypeStack.pop_front();

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R551 c_action_letter_spec_list()");
#endif
   }

/** R552
 * namelist_stmt
 *
 * : (label)? T_NAMELIST T_SLASH T_IDENT T_SLASH namelist_group_object_list
 * ( ( T_COMMA )? T_SLASH T_IDENT T_SLASH namelist_group_object_list)* T_EOS
 *
 * @param label The label.
 * @param keyword The NAMELIST keyword token.
 * @param eos End of statement token.
 * @param count Number of namelist group object lists. 
 */
// void c_action_namelist_stmt(Token_t * label, int count)
void c_action_namelist_stmt(Token_t * label, Token_t * keyword, Token_t * eos, int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_namelist_stmt() keyword = %p = %s count = %d \n",keyword,keyword != NULL ? keyword->text : "NULL",count);

     SgNamelistStatement* namelistStatement = new SgNamelistStatement();
  // setSourcePosition(namelistStatement);
     ROSE_ASSERT(keyword != NULL);
     setSourcePosition(namelistStatement,keyword);

     SgNameGroupPtrList localList;
     ROSE_ASSERT(astNodeStack.size() == (size_t)count);
     for (int i=0; i < count; i++)
        {
          SgNameGroup* nameGroup = isSgNameGroup(astNodeStack.front());
          ROSE_ASSERT(nameGroup != NULL);
          localList.push_back(nameGroup);
       // namelistStatement->get_group_list().push_back(nameGroup);
       // ROSE_ASSERT(namelistStatement->get_group_list().empty() == false);
          astNodeStack.pop_front();
        }

  // Now reverse list to put list into ROSE in the reverse order.
     for (int i=count-1; i >= 0; i--)
        {
          namelistStatement->get_group_list().push_back(localList[i]);
        }

     ROSE_ASSERT(getTopOfScopeStack()->variantT() == V_SgBasicBlock);
  // ROSE_ASSERT(getTopOfScopeStack()->variantT() == V_SgBasicBlock || getTopOfScopeStack()->variantT() == V_SgClassDefinition);
     getTopOfScopeStack()->append_statement(namelistStatement);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R552 c_action_namelist_stmt()");
#endif
   }

/** R552
 * namelist_group_name
 *
 * : T_SLASH T_IDENT T_SLASH 
 *
 * @param id Identifier (i.e., actual group name).
 */
void c_action_namelist_group_name(Token_t *id)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_namelist_group_name() id = %p = %s \n",id,id != NULL ? id->text : "NULL");

     ROSE_ASSERT(id != NULL);
     astNameStack.push_front(id);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R552 c_action_namelist_group_name()");
#endif
   }

/** R553
 * namelist_group_object
 *      :    T_IDENT
 * 
 * @param id Identifier (variable-name).
 */
void c_action_namelist_group_object(Token_t *id)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_namelist_group_object() id = %p = %s \n",id,id != NULL ? id->text : "NULL");

     ROSE_ASSERT(id != NULL);
     astNameStack.push_front(id);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R553 c_action_namelist_group_object()");
#endif
   }

/** R553 list
 * namelist_group_object_list
 *      :       T_IDENT ( T_COMMA T_IDENT )*
 * 
 * @param count The number of items in the list.
 */
void c_action_namelist_group_object_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_namelist_group_object_list__begin() \n");
   }
void c_action_namelist_group_object_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_namelist_group_object_list() count = %d \n",count);

  // Gather the names off the name stack and use the last name as the group name and build a SgNameGroup object and put it onto the astNodeStack.
     SgNameGroup* nameGroup = new SgNameGroup();

     SgStringList localList;
     ROSE_ASSERT(astNameStack.size() == (size_t)(count + 1));
     for (int i=0; i < count; i++)
        {
          string variableName = astNameStack.front()->text;
          localList.push_back(variableName);
       // nameGroup->get_name_list().push_back(variableName);
          astNameStack.pop_front();
        }

  // Now reverse list to put list into ROSE in the reverse order.
     for (int i=count-1; i >= 0; i--)
        {
          nameGroup->get_name_list().push_back(localList[i]);
        }

     string groupName = astNameStack.front()->text;
     astNameStack.pop_front();

  // printf ("groupName = %s \n",groupName.c_str());
     nameGroup->set_group_name(groupName);
     astNodeStack.push_front(nameGroup);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R553 list c_action_namelist_group_object_list()");
#endif
   }

/** R554
 * equivalence_stmt
 *
 * : (label)? T_EQUIVALENCE equivalence_set_list T_EOS
 *
 * @param label The label.
 */
// void c_action_equivalence_stmt(Token_t * label)
void c_action_equivalence_stmt(Token_t *label, Token_t *equivalenceKeyword, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_equivalence_stmt() equivalenceKeyword = %p = %s \n",equivalenceKeyword,equivalenceKeyword != NULL ? equivalenceKeyword->text : "NULL");

     ROSE_ASSERT(equivalenceKeyword != NULL);
  // buildAttributeSpecificationStatement(SgAttributeSpecificationStatement::e_equivalenceStatement,label,equivalenceKeyword);

     SgEquivalenceStatement* equivalenceStatement = new SgEquivalenceStatement();
     setSourcePosition(equivalenceStatement,equivalenceKeyword);

     SgExprListExp* setList = new SgExprListExp();

     ROSE_ASSERT(equivalenceStatement->get_equivalence_set_list() == NULL);
     equivalenceStatement->set_equivalence_set_list(setList);
     ROSE_ASSERT(equivalenceStatement->get_equivalence_set_list() != NULL);

     setList->set_parent(equivalenceStatement);
     setSourcePosition(setList);

     SgExpressionPtrList localList;
     while (astNodeStack.empty() == false)
        {
          SgExpression* expression = isSgExpression(astNodeStack.front());
          ROSE_ASSERT(expression != NULL);
          localList.push_back(expression);
          astNodeStack.pop_front();
        }

  // reverse the list
     int count = localList.size();
     for (int i = count-1; i >= 0; i--)
        {
       // equivalenceStatement->get_equivalence_set_list()->append_expression(localList[i]);
          setList->append_expression(localList[i]);
          localList[i]->set_parent(setList);
        }

     ROSE_ASSERT(astExpressionStack.empty() == true);
  // astExpressionStack.push_front(setList);

     astScopeStack.front()->append_statement(equivalenceStatement);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R554 c_action_equivalence_stmt()");
#endif
   }

/**
 * R555
 * 
 * equivalence_set
 *
 */
void c_action_equivalence_set()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_equivalence_set() \n");
   }

/** R555 list
 * equivalence_set_list
 *      :       equivalence_set ( T_COMMA equivalence_set )*
 * 
 * @param count The number of items in the list.
 */
void c_action_equivalence_set_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_equivalence_set_list__begin() \n");
   }
void c_action_equivalence_set_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_equivalence_set_list(): count = %d \n",count);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R555 c_action_equivalence_set_list()");
#endif
   }

/**
 * R556
 *
 * equivalence_object
 *
 */
void c_action_equivalence_object()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_equivalence_object() \n");
   }

/** R556 list
 * equivalence_object_list
 *      :       equivalence_object ( T_COMMA equivalence_object )*
 * 
 * @param count The number of items in the list.
 */
void c_action_equivalence_object_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_equivalence_object_list__begin() \n");
   }
void c_action_equivalence_object_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_equivalence_object_list(): count = %d \n",count);

     SgExprListExp* setList = new SgExprListExp();
     setSourcePosition(setList);

     SgExpressionPtrList localList;
     while (astExpressionStack.empty() == false)
        {
          localList.push_back(astExpressionStack.front());
          astExpressionStack.pop_front();
        }

  // reverse the list  (we want count instead of count-1, because count is the 
  // number of equivalences entriese, which is the number of entries minus one).
     for (int i = count; i >= 0; i--)
        {
          setList->append_expression(localList[i]);
          localList[i]->set_parent(setList);
        }

     astNodeStack.push_front(setList);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R556 c_action_equivalence_object_list()");
#endif
   }

/** R557
 * common_stmt
 *
 * : (label)? T_COMMON ( T_SLASH ( T_IDENT )? T_SLASH )?
 *              common_block_object_list ( ( T_COMMA )? T_SLASH ( T_IDENT )? 
 *              T_SLASH common_block_object_list )* T_EOS
 *
 * @param label The label.
 * @param count Number of common block object lists. 
 */
// void c_action_common_stmt(Token_t * label, int count)
void c_action_common_stmt(Token_t *label, Token_t *commonKeyword, Token_t *eos, int numBlocks)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_common_stmt(): numBlocks = %d \n",numBlocks);

     SgCommonBlock* commonBlock = new SgCommonBlock();
     ROSE_ASSERT(commonBlock != NULL);
     setSourcePosition(commonBlock,commonKeyword);

     ROSE_ASSERT(astNodeStack.size() == (size_t)numBlocks);

     SgCommonBlockObjectPtrList commonBlockObjectList;
     while (astNodeStack.empty() == false)
        {
          SgCommonBlockObject* commonBlockObject = isSgCommonBlockObject(astNodeStack.front());
          ROSE_ASSERT(commonBlockObject != NULL);
          commonBlockObjectList.push_back(commonBlockObject);

       // printf ("Processing commonBlockObject = %p commonBlock = %p \n",commonBlockObject,commonBlock);
          commonBlockObject->set_parent(commonBlock);
          ROSE_ASSERT(commonBlockObject->get_parent() != NULL);

          astNodeStack.pop_front();
        }

     for (int i = numBlocks-1; i >= 0; i--)
        {
       // printf ("Processing commonBlockObjectList[i=%d] = %p \n",i,commonBlockObjectList[i]);
          commonBlock->get_block_list().push_back(commonBlockObjectList[i]);
          ROSE_ASSERT(commonBlock->get_block_list().empty() == false);
       // commonBlockObjectList[i]->set_parent(commonBlock);
        }

     astScopeStack.front()->append_statement(commonBlock);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R557 c_action_common_stmt()");
#endif
   }

#if ROSE_OFP_MINOR_VERSION_NUMBER >= 8 & ROSE_OFP_PATCH_VERSION_NUMBER >= 0
/** R557-F08 list
 * target_decl_list
 *   :   target_decl ( T_COMMA target_decl )*
 * 
 * @param count The number of items in the list.
*/
// public abstract void target_decl_list__begin();
// public abstract void target_decl_list(int count);
void c_action_target_decl_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_target_decl_list__begin() \n");
   }
   
void c_action_target_decl_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_target_decl_list(): count = %d \n",count);
   }
#endif

/** R557
 * common_block_name
 *
 * : T_SLASH ( T_IDENT )? T_SLASH
 *
 * @param id Identifier if present. Otherwise, null.
 */
void c_action_common_block_name(Token_t *id)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_common_block_name(): id = %p = %s \n",id,id != NULL ? id->text : "NULL");

  // DQ (12/14/2007): Make sure that we have added the implicit program function to the scope (see test2007_161.f90)
     build_implicit_program_statement_if_required();

  // ROSE_ASSERT(id != NULL);
  // astNameStack.push_front(id);
     if (id != NULL)
        {
       // This is an un-named common block: "COMMON X, Y, Z"
          astNameStack.push_front(id);
        }

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R557 c_action_common_block_name()");
#endif
   }

/** R558 list
 * common_block_object_list
 *      :       common_block_object ( T_COMMA common_block_object )*
 * 
 * @param count The number of items in the list.
 */
void c_action_common_block_object_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_common_block_object_list__begin() \n");
   }
void c_action_common_block_object_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_common_block_object_list(): count = %d \n",count);

  // ROSE_ASSERT(astNameStack.empty() == false);
     string commonBlockName;
     if (astNameStack.empty() == false)
        {
          commonBlockName = astNameStack.front()->text;
          astNameStack.pop_front();
        }

     SgCommonBlockObject* commonBlockObject = new SgCommonBlockObject();
     // Liao 12/9/2010
     // We have to do this after moving SgCommonBlockObject to be under SgLocatedNodeSupport
     // TODO: ask Dan to fill in real file info for both SgCommonBlockObject and the contained varRefList.
     setSourcePosition(commonBlockObject);

     commonBlockObject->set_block_name(commonBlockName);

     SgExprListExp* varRefList = new SgExprListExp();
     commonBlockObject->set_variable_reference_list(varRefList);

     // TODO: ask Dan to fill in real file info for varRefList.
     setSourcePosition(varRefList);
     varRefList->set_parent(commonBlockObject);

     ROSE_ASSERT(astExpressionStack.size() == (size_t) count);

     SgExpressionPtrList localList;
     while (astExpressionStack.empty() == false)
        {
          localList.push_back(astExpressionStack.front());
          astExpressionStack.pop_front();
        }

  // Reverse the list
     for (int i = count-1; i >= 0; i--)
        {
          varRefList->append_expression(localList[i]);
          localList[i]->set_parent(varRefList);
        }

     ROSE_ASSERT(commonBlockObject->get_parent() == NULL);

     astNodeStack.push_front(commonBlockObject);
   }

/** R558
 * common_block_object
 *
 * : T_IDENT ( T_LPAREN explicit_shape_spec_list T_RPAREN )?
 *
 * @param id Identifier.
 * @param hasShapeSpecList True if has an explicit shape spec list.
 */
void c_action_common_block_object(Token_t *id, ofp_bool hasShapeSpecList)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_common_block_object(): id = %p = %s hasShapeSpecList = %s \n",id,id != NULL ? id->text : "NULL",hasShapeSpecList ? "true" : "false");

     ROSE_ASSERT(id != NULL);
     SgName variableName = id->text;

  // DQ (1/18/2011): This detects where we have used the semantics of implicitly building symbols for implicit variables.
  // printf ("WARNING: This use of trace_back_through_parent_scopes_lookup_variable_symbol() used the side effect of building a symbol if the reference is not found! \n");
  // ROSE_ASSERT(false);

  // Look for the symbol associated with the variable given by the name starting
  // at the current scope and working backwards through the parent scopes.
  // SgVariableSymbol* variableSymbol = getTopOfScopeStack()->lookup_variable_symbol(variableName);
     SgVariableSymbol* variableSymbol = trace_back_through_parent_scopes_lookup_variable_symbol(variableName,getTopOfScopeStack());

     SgExpression* constructedReference = NULL;

  // To be referenced in the common block it should have been defined already, but this is not required
  // ROSE_ASSERT(variableSymbol != NULL);
  // printf ("In c_action_common_block_object(): variableSymbol = %p \n",variableSymbol);
     if (variableSymbol != NULL)
        {
          SgVarRefExp* variableReference = new SgVarRefExp(variableSymbol);
          setSourcePosition(variableReference,id);

          if (hasShapeSpecList == true)
             {
            // I think we need to build the array reference for this case.
            // printf ("hasShapeSpecList == true, case not implemented \n");

               printf ("Sorry, hasShapeSpecList == true, case not implemented \n");
               ROSE_ASSERT(false);

               constructedReference = NULL;
             }
            else
             {
               constructedReference = variableReference;
             }
        }
       else
        {
       // The variable has not previously been declared.
       // OR it maybe declared at some point in the future (see test2010_51.90).

#if 1
       // DQ (1/19/2011): Build the implicit variable
          buildImplicitVariableDeclaration(variableName);

       // Now verify that it is present.
          variableSymbol = trace_back_through_parent_scopes_lookup_variable_symbol(variableName,astScopeStack.front());
          ROSE_ASSERT(variableSymbol != NULL);
#else
       // Note that the second time we look for it we will get a valid symbol.
          variableSymbol = trace_back_through_parent_scopes_lookup_variable_symbol(variableName,getTopOfScopeStack());
          ROSE_ASSERT(variableSymbol != NULL);
#endif
          constructedReference = new SgVarRefExp(variableSymbol);
          setSourcePosition(constructedReference,id);

       // printf ("The variable has not previously been declared (case not implemented)\n");
       // ROSE_ASSERT(false);
        }

     ROSE_ASSERT(constructedReference != NULL);
     astExpressionStack.push_front(constructedReference);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R558 c_action_common_block_object()");
#endif
   }

/** R601
 * variable
 * :    designator
 */
void c_action_variable()
   {
  // I think that this just defines the previously built SgVarRefExp as an l-value and that the creation of the SgVarRefExp should be moved to R612.

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_variable() \n");

#if !SKIP_C_ACTION_IMPLEMENTATION
     ROSE_ASSERT(astExpressionStack.empty() == false);
     getTopOfExpressionStack()->set_lvalue(true);
#endif
   }

/* R602 variable_name was name inlined as T_IDENT */

/** R603
 * designator
 *  :   object-name                      // T_IDENT (data-ref isa T_IDENT)
 *      |       array-element              // R616 is data-ref
 *      |       array-section              // R617 is data-ref [ (substring-range) ] 
 *      |       structure-component      // R614 is data-ref
 *      |       substring
 *
 *@param hasSubstringRange True if substring-range is present.
 */
void c_action_designator(ofp_bool hasSubstringRange)
   {
  // This is a part of a variable reference (any likely used many other places as well)

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_designator() hasSubstringRange = %s \n",hasSubstringRange ? "true" : "false");

     if (hasSubstringRange == true)
        {
       // printf ("Sorry, case hasSubstringRange == true not implemented \n");

          ROSE_ASSERT(astExpressionStack.empty() == false);
          SgSubscriptExpression* subscriptRange = isSgSubscriptExpression(astExpressionStack.front());
          ROSE_ASSERT(subscriptRange != NULL);
          astExpressionStack.pop_front();

       // setSourcePosition(subscriptRange);

          ROSE_ASSERT(astExpressionStack.empty() == false);
          SgExpression* simpleExpression = astExpressionStack.front();
          ROSE_ASSERT(simpleExpression != NULL);
          astExpressionStack.pop_front();

          SgPntrArrRefExp* arrayReference = isSgPntrArrRefExp(simpleExpression);
          ROSE_ASSERT(arrayReference != NULL);
#if 0
          printf ("arrayReference lhs  = %p = %s \n",arrayReference->get_lhs_operand(),arrayReference->get_lhs_operand()->class_name().c_str());
          printf ("arrayReference rhs  = %p = %s \n",arrayReference->get_rhs_operand(),arrayReference->get_rhs_operand()->class_name().c_str());
          printf ("arrayReference type = %p = %s \n",arrayReference->get_type(),arrayReference->get_type()->class_name().c_str());
#endif

       // DQ (12/3/2010): This should maybe have a type that is array of strings.
       // SgPntrArrRefExp* arrayOfStringsArrayRef = new SgPntrArrRefExp (arrayReference,subscriptRange,arrayReference->get_type());
          SgPntrArrRefExp* arrayOfStringsArrayRef = new SgPntrArrRefExp (arrayReference,subscriptRange,/* type should not be specified */ NULL);
          ROSE_ASSERT(arrayOfStringsArrayRef != NULL);

       // arrayReference->set_parent(arrayOfStringsArrayRef);
       // subscriptRange->set_parent(arrayOfStringsArrayRef);

          setSourcePosition(arrayOfStringsArrayRef);

          astExpressionStack.push_front(arrayOfStringsArrayRef);
        }
#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R602 c_action_designator()");
#endif
   }


/**
 * Unknown rule.
 * designator_or_func_ref
 *
 */
void c_action_designator_or_func_ref()
   {
  // DQ (5/4/2008): I am unclear what to do with this rule! (called by test2008_22.f90)
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In (unknown rule) c_action_designator_or_func_ref() \n");
   }

/**
 * Unknown rule.
 * substring_range_or_arg_list
 *
 */
void c_action_substring_range_or_arg_list()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_substring_range_or_arg_list() \n");
   }

/**
 * Unknown rule.
 * substr_range_or_arg_list_suffix
 *
 */
void c_action_substr_range_or_arg_list_suffix()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_substring_range_or_arg_list_suffix() \n");
   }


/**
 * R604
 * logical_variable
 *
 */
void c_action_logical_variable()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_logical_variable() \n");
   }

/**
 * R605
 * default_logical_variable
 *
 */
void c_action_default_logical_variable()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_default_logical_variable() \n");
   }

/**
 * Unknown rule.
 * scalar_default_logical_variable
 *
 */
void c_action_scalar_default_logical_variable()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_scalar_default_logical_variable() \n");
   }

/**
 * R606
 * char_variable
 *
 */
void c_action_char_variable()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_char_variable() \n");
   }

/**
 * R607
 * default_char_variable
 *
 */
void c_action_default_char_variable()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_default_char_variable() \n");
   }

/**
 * Unknown rule.
 * scalar_default_char_variable
 *
 */
void c_action_scalar_default_char_variable()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_scalar_default_char_variable() \n");
   }

/**
 * R608
 * int_variable
 *
 */
void c_action_int_variable()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_int_variable() \n");
   }

/** R609
 * substring
 *      :       data_ref (T_LPAREN substring_range T_RPAREN)?
 *      |       char_literal_constant T_LPAREN substring_range T_RPAREN
 *
 * C608 (R610) parent_string shall be of type character
 * fix for ambiguity in data_ref allows it to match T_LPAREN substring_range T_RPAREN,
 * so required T_LPAREN substring_range T_RPAREN made optional
 * ERR_CHK 609 ensure final () is (substring-range)
 * 
 * @param hasSubstringRange True if substring-range is present, otherwise it must be extracted from
 * the data-ref.
 */
void c_action_substring(ofp_bool hasSubstringRange)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_substring() hasSubstringRange = %s \n",hasSubstringRange ? "true" : "false");
   }
 
/** R611
 * substring_range
 *      :       (expr)? T_COLON (expr)?
 *
 * ERR_CHK 611 scalar_int_expr replaced by expr
 *
 * @param hasLowerBound True if lower bound is present in a substring-range (lower_bound:upper_bound).
 * @param hasUpperBound True if upper bound is present in a substring-range (lower_bound:upper_bound).
 */
void c_action_substring_range(ofp_bool hasLowerBound, ofp_bool hasUpperBound)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R611 c_action_substring_range() hasLowerBound = %s hasUpperBound = %s \n",hasLowerBound ? "true" : "false",hasUpperBound ? "true" : "false");

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R611 c_action_substring_range()");
#endif

     SgExpression* subscriptRange = buildSubscriptExpression(hasLowerBound,hasUpperBound, /* hasStride = */ false, /* isAmbiguous = */ false);
     ROSE_ASSERT(subscriptRange != NULL);
     astExpressionStack.push_front(subscriptRange);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R611 c_action_substring_range()");
#endif
   }

/** R612
 *      data_ref
 *      :       part_ref (T_PERCENT part_ref)*
 *
 * @param numPartRef The number of optional part_refs
 */

SgCAFCoExpression *rice_dataref_coexpr;         // for 'c_action_rice_spawn_stmt'

void c_action_data_ref(int numPartRef)
   {
  // DQ (12/29/2010): See notes on how R612 and R613 operate together.

  // This is a part of a variable reference (and likely used many other places as well)
  // I am not sure what to do with this rule (unless the point is to build a variable here, instead of in R601)

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R612 c_action_data_ref(): (variable built here) numPartRef = %d \n",numPartRef);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R612 c_action_data_ref()");
#endif

  // This should at least be non-negative.
     ROSE_ASSERT(numPartRef >= 0);

  // FMZ (2/11/2009): Here we could have SgCAFCoExpression in the astExpressionStack
  // We need to pop out the SgNode to complete whatever need to be done regularly
  // (without co-array image selector)
     bool processCoarray = false;
     SgCAFCoExpression* coExpr = NULL;
     if (numPartRef == 1)
        {
          if (astExpressionStack.empty() == false)
             {
               processCoarray = (astExpressionStack.front()->variantT() == V_SgCAFCoExpression);
             }

          if (processCoarray)
             {
            // need to pop out the CoExpresion first
               coExpr = isSgCAFCoExpression(astExpressionStack.front());
               ROSE_ASSERT(coExpr != NULL);
               if (coExpr->get_referData()) //already processed
                    processCoarray = false;
                 else
                    astExpressionStack.pop_front();
             }
        }

  // DQ (12/29/2010): Form the full name as a vector of strcutures specific to suporting R612 and R613.
     std::vector<MultipartReferenceType> qualifiedNameList(numPartRef);
  // Iterate backwards from the front of the list for the first numPartRef entries.
     std::list<MultipartReferenceType>::iterator j = astMultipartReferenceStack.begin();
     ROSE_ASSERT (j != astMultipartReferenceStack.end());
     for (int i = 0; i < numPartRef; i++)
        {
       // Assemble the entries into the qualifiedNameList from back to front.
          qualifiedNameList[(numPartRef-1)-i].name                      = (*j).name;
          qualifiedNameList[(numPartRef-1)-i].hasSelectionSubscriptList = (*j).hasSelectionSubscriptList;

          ROSE_ASSERT (j != astMultipartReferenceStack.end());
          j++;
        }

#if 0
     string qualifiedNameString = generateQualifiedName(qualifiedNameList);
     printf ("@@@@@@@@@@@@@@@@@@@ R612 qualifiedNameString = %s \n",qualifiedNameString.c_str());
#endif

  // printf ("numPartRef = %d astNameStack.size() = %zu \n",numPartRef,astNameStack.size());
  // ROSE_ASSERT(numPartRef == (int)astNameStack.size());

#if !SKIP_C_ACTION_IMPLEMENTATION
  // Build the SgVarRefExp object

  // Get the token (and the name in side of the token) previously saved on the stack
     AstNameType* nameToken = getTopOfNameStack();
     ROSE_ASSERT(nameToken != NULL);

  // SgName variableName = nameToken->text;
     SgName variableName = qualifiedNameList[numPartRef-1].name;
     ROSE_ASSERT(variableName == nameToken->text);

  // printf ("In R612 (part of multi-part ref) variableName = %s \n",variableName.str());

  // Look for the symbol associated with the variable given by the name starting
  // at the current scope and working backwards through the parent scopes.
  // SgVariableSymbol* variableSymbol = trace_back_through_parent_scopes_lookup_variable_symbol(variableName,getTopOfScopeStack());
  // std::vector<SgVariableSymbol*> variableSymbolList = trace_back_through_parent_scopes_lookup_member_variable_symbol(qualifiedNameList,getTopOfScopeStack());
  // SgVariableSymbol* variableSymbol = variableSymbolList.empty() ? NULL : variableSymbolList[0];
     std::vector<SgSymbol*> variableSymbolList = trace_back_through_parent_scopes_lookup_member_variable_symbol(qualifiedNameList,getTopOfScopeStack());
     SgVariableSymbol* variableSymbol = variableSymbolList.empty() ? NULL : isSgVariableSymbol(variableSymbolList[0]);

     if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
          printf ("After trace_back_through_parent_scopes_lookup_variable_symbol(%s,astScopeStack) variableSymbol = %p = %s \n",
               variableName.str(),variableSymbol,variableSymbol ? SageInterface::get_name(variableSymbol).c_str() : "NULL");

  // We should have found no more symbols than numPartRef (.e.g if this is an implicit variable then variableSymbolList would be empty.
     ROSE_ASSERT(variableSymbolList.size() <= (size_t)numPartRef);

  // We want to debug cases at the moment for which the following assertion will always be true.
  // ROSE_ASSERT(variableSymbol != NULL);

  // DQ (12/29/2010): Moved outside of conditional so that it could be tested afteward.
     SgFunctionSymbol* functionSymbol = NULL;

#if 0
     printf ("In R612: variableSymbol = %p \n",variableSymbol);
#endif
     if (variableSymbol == NULL)
        {
       // And there was no list of variables returned...(see test2010_176.f90 for an  
       // example that fails this test; return value initialization for derived type).
       // ROSE_ASSERT(variableSymbolList.empty() == true);

       // DQ (12/29/2010): I think I can assert this...(fails for test2010_176.f03)
       // ROSE_ASSERT(numPartRef == 1);

         // This is a function call and the 'coExpr' detected above, if any, will be ignored in this routine.
         // Preserve it for future use in 'c_action_rice_spawn_stmt'.
         // TODO: check that coExpr is present here only if in the context of a Rice CAF2 'spawn' statement.
          rice_dataref_coexpr = (processCoarray ? coExpr : NULL);

       // DQ (12/21/2010): For the variable to not have existed, this must be the simple case of a reference with a single part.
       // (see test2010_176.f90 for an example that fails this test; return value initialization for derived type).
       // ROSE_ASSERT(numPartRef <= 1);
          ROSE_ASSERT(numPartRef <= 1 || ( (variableSymbolList.empty() == false) && (isSgFunctionSymbol(variableSymbolList[0]) != NULL) ) );

       // This is a variable that has not been previously declared (Fortran allows this inplicit declaration), 
       // but first check to make sure it is not an implicit function.

       // printf ("variableName = %s getTopOfScopeStack() = %p = %s \n",variableName.str(),getTopOfScopeStack(),getTopOfScopeStack()->class_name().c_str());
       // SgFunctionSymbol* functionSymbol = trace_back_through_parent_scopes_lookup_function_symbol(variableName,getTopOfScopeStack());
       // SgFunctionSymbol* functionSymbol = NULL;

          if ( (variableSymbolList.empty() == false) && (isSgFunctionSymbol(variableSymbolList[0]) != NULL) )
             {
            // This is the case of an initialization of a function return type for a derived type (see test2010_176.f90 for an example).
            // printf ("This is the case of an initialization of a function return type for a derived type \n");
               functionSymbol = isSgFunctionSymbol(variableSymbolList[0]);
               ROSE_ASSERT(functionSymbol != NULL);
             }
            else
             {
            // DQ (12/29/2010): I think I can assert this...works for all our basic test codes in Fortran_tests/*.f,f90,f03...
               ROSE_ASSERT(numPartRef == 1);

            // Note that this function call would not make sense if numPartRef > 1 since the input scope would be wrong.
               functionSymbol = trace_back_through_parent_scopes_lookup_function_symbol(variableName,getTopOfScopeStack());
             }
       // printf ("result of call to trace_back_through_parent_scopes_lookup_function_symbol() = %p \n",functionSymbol);

       // DQ (4/29/2008): Added support for detecting derived types
       // SgClassSymbol* classSymbol = trace_back_through_parent_scopes_lookup_derived_type_symbol(variableName,getTopOfScopeStack());
          SgClassSymbol* classSymbol = NULL;
          if (numPartRef == 1)
             {
            // This function call only makes since if numPartRef == 1 since only then is the scope reasonable.
               classSymbol = trace_back_through_parent_scopes_lookup_derived_type_symbol(variableName,getTopOfScopeStack());
             }

          bool isAnIntrinsicFunction = matchAgainstIntrinsicFunctionList(nameToken->text);
          bool isANonIntrinsicFunction = (functionSymbol != NULL);

       // DQ (4/29/2008): Added support for detecting derived types
          bool isADerivedType = classSymbol != NULL;

          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
             {
               printf ("isAnIntrinsicFunction   = %s \n",isAnIntrinsicFunction   ? "true" : "false");
               printf ("isANonIntrinsicFunction = %s \n",isANonIntrinsicFunction ? "true" : "false");
               printf ("isADerivedType          = %s \n",isADerivedType          ? "true" : "false");
               printf ("variableSymbol = %p functionSymbol = %p classSymbol = %p \n",variableSymbol,functionSymbol,classSymbol);
             }

       // ROSE_ASSERT(isANonIntrinsicFunction == false);

          if ( (isAnIntrinsicFunction == true) || (isANonIntrinsicFunction == true) )
             {
            // DQ (4/29/2008): If it is to be handled as a function then is must not be a derived type.
               ROSE_ASSERT(isADerivedType == false);
#if 0
               printf ("In R612: isAnIntrinsicFunction = %s \n",isAnIntrinsicFunction ? "true" : "false");
#endif
               if (isAnIntrinsicFunction == true)
                  {
                 // This is a function call and not an array reference (this case is an implicit function).
                    if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                         printf ("Found an implicit function call: not implemented yet! name = %s \n",nameToken->text);
                 // ROSE_ASSERT(false);

                    SgName functionName = nameToken->text;

                 // DQ (11/26/2010): Fixing bug as represented in test2010_111.f90 (use of implicit function name as a variable).
                 // If this is an implicit function that has function arguements, then we should see an expression list on the 
                 // astExpressionStack.  What other information could be used to trigger this?  We would like to not have behavior
                 // within the ROFP/ROE translation that would be dependent on our parser state (stack sizes).

                 // Using "isANonIntrinsicFunction == false" instead of the size of the astExpressionStack fails for test2007_57.f90.
                 // if (isANonIntrinsicFunction == false)
#if 0
                    printf ("In R612: astExpressionStack.empty() = %s \n",astExpressionStack.empty() ? "true" : "false");
#endif
                    if (astExpressionStack.empty() == true)
                       {
                      // printf ("This is NOT an implicit function call (likely a data variable reference matching the name of a implicit function): variableName = %s \n",variableName.str());
                      // printf ("isANonIntrinsicFunction = %s \n",isANonIntrinsicFunction ? "true" : "false");

                      // SgVariableSymbol* variableSymbolMatchingImplicitFunctionName = trace_back_through_parent_scopes_lookup_variable_symbol(variableName,getTopOfScopeStack());
                      // ROSE_ASSERT(variableSymbolMatchingImplicitFunctionName != NULL);

                         bool isAnImplicitNoneScope = isImplicitNoneScope();
                         if (isAnImplicitNoneScope == false)
                            {
                           // This is an implicitly defined variable declaration that matches the name of an implicit function (so requires some special handling).
                              buildImplicitVariableDeclaration(variableName);

                              variableSymbol = NULL;
                              functionSymbol = NULL;
                              classSymbol    = NULL;
                              SgScopeStatement* currentScope = astScopeStack.front();

                           // This does not build a variable, but it does build a SgVariableSymbol.
                           // printf ("Building a SgVariableSymbol, though not building a SgVarRefExp \n");
                              trace_back_through_parent_scopes_lookup_variable_symbol_but_do_not_build_variable(variableName,currentScope,variableSymbol,functionSymbol,classSymbol);
                              ROSE_ASSERT(variableSymbol != NULL);

                           // Add the new variableSymbol to the list so that the processing of the list can be properly triggered.
                              variableSymbolList.push_back(variableSymbol);
                            }
                           else
                            {
                           // This the case of calling the implicit function without the "()" as in "call random_seed" instead 
                           // of "call random_seed()" (See test2010_169.f90).
                           // printf ("Note: This the case of calling the implicit function without the \"()\" as in \"call random_seed\" instead of \"call random_seed()\"\n");

                           // DQ (12/29/2010): Fixed to use the new return type and push the function symbol.
                           // generateFunctionCall(nameToken);
                              functionSymbol = generateFunctionCall(nameToken);
                              variableSymbolList.push_back(functionSymbol);
                           // printf ("Test #1 variableSymbolList.size() = %zu \n",variableSymbolList.size());

                           // printf ("Error: This is an implicit variable that has a name matching an implicit function, but isImplicitNoneScope() == true (so this is an inconsistancy).\n");
                           // ROSE_ASSERT(false);
                            }
                      // ROSE_ASSERT(false);
                       }
                      else
                       {
                      // DQ (12/29/2010): Fixed to use the new return type and push the function symbol.
                      // DQ (12/29/2010): This is the case in evaluating an implicit function call for the first time (see test2007_57.f90 for function "sign").
                      // generateFunctionCall(nameToken);
                         functionSymbol = generateFunctionCall(nameToken);

                      // variableSymbolList.push_back(functionSymbol);
                         if (find(variableSymbolList.begin(),variableSymbolList.end(),functionSymbol) == variableSymbolList.end())
                            {
                           // printf ("Adding functionSymbol to variableSymbolList since it was not present. \n");
                              variableSymbolList.push_back(functionSymbol);
                            }

                      // printf ("Test #2 variableSymbolList.size() = %zu \n",variableSymbolList.size());
                       }

#if 0
                 // Output debugging information about saved state (stack) information.
                    outputState("Build the implicit function call in R612 c_action_data_ref()");
                 // ROSE_ASSERT(false);
#endif
                  }
                 else
                  {
                 // printf ("This function %s is NOT a recognized implicit function \n",variableName.str());
                 // ROSE_ASSERT(false);
#if 0
                    printf ("In R612: isANonIntrinsicFunction = %s \n",isANonIntrinsicFunction ? "true" : "false");
#endif
                 // if (isANonIntrinsicFunction == true && functionSymbol == NULL)
                    if (isANonIntrinsicFunction == true)
                       {
                      // This is a function call and not an array reference (this case is NOT an implicit function).
#if 0
                      // Output debugging information about saved state (stack) information.
                         outputState("Build the non-implicit function call in R612 c_action_data_ref()");
#endif
                      // Pop the name off the stack (clean up the stack, we have the name as the variableName)
                      // This is popped off the stack at the end of this function!
                      // astNameStack.pop_front();

                         ROSE_ASSERT(functionSymbol != NULL);
                         SgFunctionRefExp* functionRefExp = new SgFunctionRefExp(functionSymbol,NULL);
                      // setSourcePosition(functionRefExp);
                         setSourcePosition(functionRefExp,nameToken);
#if 0
                         printf ("astExpressionStack.empty() = %s \n",astExpressionStack.empty() ? "true" : "false");
#endif
                      // ROSE_ASSERT(astExpressionStack.empty() == false);

                      // Original code!
                         if (astExpressionStack.empty() == true)
                            {
                           // This should be the SgFunctionSymbol for the current function where the return value 
                           // is set (fortran syntax is <function-name> = <expression>)

                           // DQ (1/20/2008): If this is associated with a function call lacking the "()" syntax then the expressionStack will be empty.
                           // printf ("Skipping tests of functionDeclarationFoundViaScopes vs. functionDeclarationFoundViaSymbol \n");
                           // ROSE_ASSERT(functionDeclarationFoundViaScopes->get_definingDeclaration() == functionDeclarationFoundViaSymbol->get_definingDeclaration());
                           // ROSE_ASSERT(functionDeclarationFoundViaScopes->get_firstNondefiningDeclaration() == functionDeclarationFoundViaSymbol->get_firstNondefiningDeclaration());

                           // Push the function reference expression onto the stack (this is going to be strange to C/C++ analysis)
                           // Fortran pretends that the function reference is assigned to as part of the return.  It is unclear
                           // if we should handle this is any specific uniform way within the AST.  For now I will reproduce what
                           // the Fortran does.  But we might want to generate a "return <expression>" statement instead so that
                           // the analysis between C/C++ and Fortran will be more uniform. Then we could handle the syntax difference
                           // within the code generation (unparser).
                              astExpressionStack.push_front(functionRefExp);
                            }
                           else
                            {
                           // DQ (5/15/2008): Trying to fix test2008_29.f
                           // Trying to fix case in test2007_164.f
                              SgExprListExp* functionArguments = NULL;
                              if (astExpressionStack.empty() == true)
                                 {
                                   functionArguments = new SgExprListExp();
                                   setSourcePosition(functionArguments);
                                 }
                                else
                                 {
                                   functionArguments = isSgExprListExp(astExpressionStack.front());
                                   if (functionArguments != NULL)
                                      {
                                        astExpressionStack.pop_front();
                                      }
                                     else
                                      {
                                     // Nothing...
                                      }
                                 }

                           // DQ (5/15/2008): test the exclusion of this code!
                                   if (functionArguments != NULL) 
                                 {
                                   SgFunctionCallExp* functionCallExp  = new SgFunctionCallExp(functionRefExp,functionArguments,NULL);
                                   setSourcePosition(functionCallExp,nameToken);
                                   astExpressionStack.push_front(functionCallExp);
                                 }
                                else
                                 {
                                   astExpressionStack.push_front(functionRefExp);
                                 }
                            }

                      // DQ (12/29/2010): Add to the variableSymbolList to try to enforce one-to-one mapping to input number of parts in multi-part references.
                         ROSE_ASSERT(functionSymbol != NULL);
                      // variableSymbolList.push_back(functionSymbol);
                         if (find(variableSymbolList.begin(),variableSymbolList.end(),functionSymbol) == variableSymbolList.end())
                            {
                           // printf ("Adding functionSymbol to variableSymbolList since it was not present. \n");
                              variableSymbolList.push_back(functionSymbol);
                            }

                      // printf ("Test #3 variableSymbolList.size() = %zu \n",variableSymbolList.size());

#if 0
                      // Output debugging information about saved state (stack) information.
                         outputState("DONE: Build the non-implicit function call in R612 c_action_data_ref()");
#endif
                      // printf ("Found an function call (but NOT an implicit function): not implemented yet! \n");
                      // ROSE_ASSERT(false);
                       }
                  }
             }
            else
             {
            // This branch is the case that both isAnIntrinsicFunction and isANonIntrinsicFunction are false.
               ROSE_ASSERT( (isAnIntrinsicFunction == false) && (isANonIntrinsicFunction == false) );

            // DQ (4/30/2008): If it is to be handled as a function then is must not be a derived type.
            // ROSE_ASSERT(isADerivedType == false);

#if 0
            // Output debugging information about saved state (stack) information.
               outputState("This may be a derived type or a function in R612 c_action_data_ref()");
#endif
            // DQ (4/30/2008): Added support for where this is a derived type.
#if 0
               printf ("isADerivedType = %s \n",isADerivedType ? "true" : "false");
#endif
               if (isADerivedType == true)
                  {
                 // I think there is nothing to do in this case, we just want to avoid building a variable or a function from this derived type reference!
                 // printf ("No variable to build here since isADerivedType == true \n");
                 // ROSE_ASSERT(false);

                 // DQ (12/29/2010): For us to be in this branch then numPartRef == 1.
                    ROSE_ASSERT(numPartRef == 1);

                 // Maybe we should be building a derived type reference (as in the case of an initializer).
                 // This rule is called for type initializers, and also nested type initializers.

                 // SgConstructorInitializer* constructorInitializer = new SgConstructorInitializer();

                    SgType* type = classSymbol->get_type();
                    ROSE_ASSERT(type != NULL);
                    SgClassType* classType = isSgClassType(type);

                 // DQ (5/14/2008): The top of astExpressionStack should be a SgExprListExp IR node
                    SgExprListExp* exprList = isSgExprListExp(astExpressionStack.front());
                    ROSE_ASSERT(exprList != NULL);
                    astExpressionStack.pop_front();

                 // Copied from R504, please refactor this code!
                    SgName functionName = "fortran_constructor_function";
                    ROSE_ASSERT(classType != NULL);
                    SgFunctionType* functionType = new SgFunctionType(classType,false);

                    SgName mangledName = functionType->get_mangled_type();
                    SgNode::get_globalFunctionTypeTable()->insert_function_type(mangledName,functionType);

                    SgMemberFunctionDeclaration* memberfuncdecl = new SgMemberFunctionDeclaration( functionName, functionType, NULL );
                    ROSE_ASSERT(classType->get_declaration() != NULL);
                    SgScopeStatement* typeDeclarationScope = classType->get_declaration()->get_scope();
                    ROSE_ASSERT(typeDeclarationScope != NULL);
                    memberfuncdecl->set_scope(typeDeclarationScope);
                    memberfuncdecl->set_parent(typeDeclarationScope);
                    memberfuncdecl->set_firstNondefiningDeclaration(memberfuncdecl);
                    setSourcePosition(memberfuncdecl->get_parameterList());
                    setSourcePosition(memberfuncdecl->get_CtorInitializerList());
                    setSourcePosition(memberfuncdecl);

                    SgConstructorInitializer* constructorInitializer = new SgConstructorInitializer(memberfuncdecl,exprList,classType,false,false,true,false);
                    ROSE_ASSERT(constructorInitializer != NULL);
                    setSourcePosition(constructorInitializer);

                    astExpressionStack.push_front(constructorInitializer);

                 // DQ (12/29/2010): Debugging gfortranTestSuite/gfortran.dg/array_constructor_35.f90
                 // A member function symbol might make more sense here.
                    ROSE_ASSERT(classSymbol != NULL);
                    SgMemberFunctionSymbol* memberFunctionSymbol = new SgMemberFunctionSymbol(memberfuncdecl);

                    if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                         printf ("WARNING: Still need to add SgMemberFunctionSymbol to typeDeclarationScope = %p \n",typeDeclarationScope);

                    variableSymbolList.push_back(memberFunctionSymbol);
                 // printf ("Test #4 variableSymbolList.size() = %zu \n",variableSymbolList.size());

#if 0
                 // Output debugging information about saved state (stack) information.
                    outputState("Just built a SgConstructorInitializer in R612 c_action_data_ref()");
#endif
                  }
                 else
                  {
                 // This is the case that isADerivedType == false.
                    ROSE_ASSERT(isADerivedType == false);

                 // DQ (1/18/2011): In general everything is a function unless there is a hint that it is an array.

                 // DQ (1/17/2011): This is not a good enough test to trigger a function vs. variable declaration...
                 // printf ("This is not a good enough test to trigger a function vs. variable declaration... variableName = %s \n",variableName.str());

                 // bool inCurrentScope = (astScopeStack.front()->lookup_variable_symbol(variableName) != NULL);
                 // printf ("inCurrentScope = %s \n",inCurrentScope ? "true" : "false");

                 // DQ (1/17/2011): Check if this is supposed to be a function or a variable.
                 // If this is a SgExprListExp then it is part of a function
                 // SgExprListExp* expressionList = isSgExprListExp( (astExpressionStack.empty() == false) ? astExpressionStack.front() : NULL);
                    SgSymbol* symbol = SageInterface::lookupSymbolInParentScopes(variableName,astScopeStack.front());

                 // DQ (1/19/2011): Since we don't have the side-effect of trace_back_through_parent_scopes_lookup_variable_symbol() 
                 // generating a symbol, their is not symbol to be found and interpretAsVariable will be false.  This causes the
                 // named reference to be handled as a function (a SgFunctionSymbol will be incerted into the variableSymbolList.

                 // We need to check if this is a name that has a valid  qualifiedNameList[(numPartRef-1)-i].hasSelectionSubscriptList.
                 // ROSE_ASSERT(symbol != NULL);
                 // bool interpretAsVariable = (isSgVariableSymbol(symbol) != NULL) ? true : false;
                 // bool interpretAsVariable = (isSgFunctionSymbol(symbol) != NULL) ? false : true;
                    bool interpretAsVariable = false;
                 // printf ("isSgVariableSymbol(symbol) = %p \n",isSgVariableSymbol(symbol));
                    if (isSgVariableSymbol(symbol) == NULL)
                       {
                      // printf ("isSgFunctionSymbol(symbol) = %p \n",isSgFunctionSymbol(symbol));
                         if (isSgFunctionSymbol(symbol) != NULL)
                          {
                            interpretAsVariable = false;
                          }
                         else
                          {
                         // If this is not clearly a variable and it has parameters then it is to be interpreted to be a function.
                            bool hasSelectionSubscriptList = qualifiedNameList[(numPartRef-1)].hasSelectionSubscriptList;
                         // printf ("hasSelectionSubscriptList = %s \n",hasSelectionSubscriptList ? "true" : "false");
                         // interpretAsVariable = (hasSelectionSubscriptList == false);
                            if (hasSelectionSubscriptList == true)
                               {
                              // Found a parameter list, this is a function!
                                 interpretAsVariable = false;
                               }
                              else
                               {
                              // No parameter list, so this is variable (neither a function nor an array).
                                 interpretAsVariable = true;
                               }                            
                          }
                       }
                      else
                       {
                      // interpretAsVariable = (isSgFunctionSymbol(symbol) != NULL) ? false : true;
                         interpretAsVariable = true;
                       }

                 // printf ("interpretAsVariable = %s \n",interpretAsVariable ? "true" : "false");

                 // if (expressionList != NULL)
                    if (interpretAsVariable == false)
                       {
                      // printf ("Calling generateFunctionCall() for nameToken = %s \n",nameToken->text);
                      // generateFunctionCall(nameToken);

                      // DQ (12/29/2010): Fixed to use the new return type and push the function symbol.
                      // generateFunctionCall(nameToken);
                         functionSymbol = generateFunctionCall(nameToken);
                         variableSymbolList.push_back(functionSymbol);
                      // printf ("Test #5 variableSymbolList.size() = %zu \n",variableSymbolList.size());
                       }
                      else
                       {
                         variableSymbol = NULL;
                         functionSymbol = NULL;
                         classSymbol    = NULL;
                         SgScopeStatement* currentScope = astScopeStack.front();

                      // DQ (1/19/2011): code to build new variable.
                      // printf ("We need to build a variable in the current scope = %p = %s for variableName = %s \n",currentScope,currentScope->class_name().c_str(),variableName.str());
                         buildImplicitVariableDeclaration(variableName);
                      // Add the new variableSymbol to the list so that the processing of the list can be properly triggered.
                         trace_back_through_parent_scopes_lookup_variable_symbol_but_do_not_build_variable(variableName,currentScope,variableSymbol,functionSymbol,classSymbol);
                         ROSE_ASSERT(variableSymbol != NULL);
                         variableSymbolList.push_back(variableSymbol);
                       }
                  }
             }
        }
       else
        {
       // This is the case of (variableSymbol != NULL)
          ROSE_ASSERT(variableSymbol != NULL);

          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
               printf ("Found a previously defined variableSymbol = %p variable = %s \n",variableSymbol,variableName.str());

       // DQ (1/24/2011): Test the variable symbol just found...
          SgInitializedName* initializedName = variableSymbol->get_declaration();

          ROSE_ASSERT(initializedName != NULL);

       // These don't work because the SageInterface versions fail to handle SgAlaisSymbols and non-SgVariableSymbols correctly.
       // SgVariableSymbol* tmp_variableSymbol = SageInterface::lookupVariableSymbolInParentScopes("x",astScopeStack.front());
       // ROSE_ASSERT(tmp_variableSymbol != NULL);

       // SgVariableSymbol* local_variableSymbol = SageInterface::lookupVariableSymbolInParentScopes(initializedName->get_name(),astScopeStack.front());
       // ROSE_ASSERT(local_variableSymbol != NULL);

          ROSE_ASSERT( initializedName->get_scope()->get_symbol_table() != NULL);
#if 0
          initializedName->get_scope()->get_symbol_table()->print("Output symbol table from R612");
#endif
          ROSE_ASSERT( initializedName->get_scope()->lookup_variable_symbol(initializedName->get_name()) != NULL);
          ROSE_ASSERT( initializedName->get_scope()->get_symbol_table()->find(initializedName) != NULL);

          ROSE_ASSERT(initializedName->get_symbol_from_symbol_table() != NULL);
        }

  // DQ (12/22/2010): We can't assert this (see testcode test2007_07.f90).
  // DQ (12/21/2010): I think at this point we should have a valid variableSymbol pointer.
  // ROSE_ASSERT(variableSymbol != NULL);
  // printf ("Before processing variableSymbolList --- We can't assert that the variableSymbol != NULL --- variableSymbol = %p \n",variableSymbol);

  // DQ (12/29/2010): We can't assert this (see testcode test2007_57.f90).
  // DQ (12/29/2010): I think we can assume this, but test it.
  // ROSE_ASSERT(variableSymbol != NULL || functionSymbol != NULL);

  // DQ (12/29/2010): Can we assert this! This works!
  // printf ("numPartRef = %d variableSymbolList.size() = %zu \n",numPartRef,variableSymbolList.size());
     ROSE_ASSERT( (size_t) numPartRef >= variableSymbolList.size());

  // DQ (12/29/2010): This fails for test2007_57.f90 (but after a lot of work we can enforce this uniromally for all of out simple test codes at least).
  // This is important since now we can use information about the index number of the symbol in variableSymbolList to map back to the
  // hasSelectionSubscriptList data field in the vector of MultipartReferenceType object build in R612 for each part of the multi-part reference.
  // printf ("numPartRef = %d variableSymbolList.size() = %zu \n",numPartRef,variableSymbolList.size());
     ROSE_ASSERT( (size_t) numPartRef == variableSymbolList.size());

  // Build a list of intermedate expressions so that we don't disturb the astExpressionStack.
     std::list<SgExpression*> intermediateExpresionList;

  // Loop over the available symbols...
  // Note that we might want to build this is reverse order.
     for (size_t i = 0; i < variableSymbolList.size(); i++)
        {
       // DQ (12/27/2010): We need a more general concept of a SgSymbol since this can be either a SgVariableSymbol or an SgFunctionSymbol.
          SgSymbol* tempSymbol = NULL;

       // printf ("In loop over variableSymbolList: i = %zu \n",i);
          size_t lastElement = variableSymbolList.size() - 1;
          ROSE_ASSERT(i < variableSymbolList.size());
          ROSE_ASSERT(lastElement < variableSymbolList.size());
       // ROSE_ASSERT(numPartRef >= variableSymbolList.size());

       // variableSymbol = isSgVariableSymbol(variableSymbolList[lastElement - i]);
          tempSymbol = variableSymbolList[lastElement - i];

          bool hasSelectionSubscriptList = qualifiedNameList[lastElement - i].hasSelectionSubscriptList;

          ROSE_ASSERT(tempSymbol != NULL);
       // printf ("hasSelectionSubscriptList = %s tempSymbol = %p = %s \n",hasSelectionSubscriptList ? "true" : "false",tempSymbol,tempSymbol->class_name().c_str());

       // DQ (12/27/2010): Handle case of function return value initialization for derived type (see test2010_176.f90).
       // ROSE_ASSERT(variableSymbol != NULL);
          if (isSgVariableSymbol(tempSymbol) != NULL)
             {
            // This is the typical case.
#if 0
               printf ("Variable name = %s \n",tempSymbol->get_name().str());
#endif
             }
            else
             {
            // Case of function return type reference for derived type.
            // ROSE_ASSERT(functionSymbol != NULL);

            // DQ (12/27/2010): We have a function reference expresion on the stack, as part of building a function but this is not to be a function call...
               ROSE_ASSERT(astExpressionStack.empty() == false);
#if 0
               printf ("$$$$$$$$$$$$$$$$$$$ Why are we poping this top expression in the astExpressionStack off? \n");
               printf ("$$$$$$$$$$$$$$$$$$$ Commented out removing the function call from the astExpressionStack! \n");
            // astExpressionStack.pop_front();
#endif
             }
          ROSE_ASSERT(tempSymbol != NULL);

       // DQ (1/13/2011): This is not computed correctly for test2011_02.f90.
       // DQ (12/23/2010): Removed this code...I think it is not longer required with the better handling of multi-part references...
       // Changed to because of test2007_59.f90 "a = sign(b,c) + int(real(c))" processing of "c" ate the function call on the stack "sign"
       // bool stackHoldsAnIndexExpression = (astExpressionStack.empty() == false) && ((isSgExprListExp(astExpressionStack.front()) != NULL) || (numPartRef > 1) && isSgNullExpression(astExpressionStack.front()));
          bool stackHoldsAnIndexExpression = hasSelectionSubscriptList;

          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
             {
               printf ("stackHoldsAnIndexExpression = %s \n",stackHoldsAnIndexExpression ? "true" : "false");

               if (astExpressionStack.empty() == false)
                    printf ("astExpressionStack.front() = %s \n",astExpressionStack.front()->class_name().c_str());
             }
#if 1
       // Output debugging information about saved state (stack) information.
          outputState("At variableSymbol != NULL of R612 c_action_data_ref()");
#endif

          SgExpression* variable = NULL;
       // SgType* variableType = variableSymbol->get_type();
          SgType* variableType = tempSymbol->get_type();
          ROSE_ASSERT(variableType != NULL);
          switch (variableType->variantT())
             {
            // See test2010_174.f90 for an example of where SgPointerType is handled.
               case V_SgPointerType:

            // DQ (12/28/2010): I think this is a mistake to include here (see test2010_172.f90 (general test) and test2010_184.f90 (specific test)).
            // This is however required to support test2007_103.f90 (and a collection of about 6 other test codes.  The only conclusion that I
            // can come to is that much of the logic in this R612 rule must be moved to the R613 rule where the exact information is available
            // as to which parts of a multipart reference are associated with index expressions!  Oh shit, I didn't want to rewrite this!
            // case V_SgTypeString:
               case V_SgTypeString:
                  {
                 // DQ (12/29/2010): A huge amount of work has gone into supporting a clean way to have this information from R613 be available in R613.
                 // Many issues have been forced to be made uniform and rewritten internally.  This support could not have been moved to R613 for
                 // reasons made more clear in the commencts at the top of R613.
                    if (hasSelectionSubscriptList == true)
                       {
                      // THIS IS A COPY OF THE CODE IN CASE V_SgArrayType
                         SgVariableSymbol* temp_variableSymbol = isSgVariableSymbol(tempSymbol);
                         ROSE_ASSERT(temp_variableSymbol != NULL);

                      // SgExpression* arrayVariable = new SgVarRefExp(variableSymbol);
                         SgExpression* arrayVariable = new SgVarRefExp(temp_variableSymbol);
                         setSourcePosition(arrayVariable,nameToken);

                      // See test2007_36.f90 for an example of where this can be false (data statement)
                      // ROSE_ASSERT(astExpressionStack.empty() == false);
                         if (astExpressionStack.empty() == false)
                            {
                              SgExpression* indexExpression = astExpressionStack.front();
                           // printf ("#################### variable name = %s indexExpression->get_type() = %s stackHoldsAnIndexExpression = %s \n",variableSymbol->get_name().str(),indexExpression->get_type()->class_name().c_str(),stackHoldsAnIndexExpression ? "true" : "false");

                           // Don't allow arrays to be index by arrays (can we do that).
                              SgArrayType* arrayType = isSgArrayType(indexExpression->get_type());
                           // if (arrayType == NULL)
                              if (arrayType == NULL && stackHoldsAnIndexExpression == true)
                                 {
                                   astExpressionStack.pop_front();
                                   variable = new SgPntrArrRefExp(arrayVariable,indexExpression,NULL);
                                 }
                                else
                                 {
                                // This is the case of an array expression (array language expression)
                                   variable = arrayVariable;
                                 }
                            }
                           else
                            {
                           // This is the case of a data statement
                              variable = arrayVariable;
                            }
                       }
                      else
                       {
                         SgVariableSymbol* temp_variableSymbol = isSgVariableSymbol(tempSymbol);
                         ROSE_ASSERT(temp_variableSymbol != NULL);
                         variable = new SgVarRefExp(temp_variableSymbol);
                       }

                    break;
                  }

               case V_SgArrayType:
                  {
                    SgVariableSymbol* temp_variableSymbol = isSgVariableSymbol(tempSymbol);
                    ROSE_ASSERT(temp_variableSymbol != NULL);

                 // SgExpression* arrayVariable = new SgVarRefExp(variableSymbol);
                    SgExpression* arrayVariable = new SgVarRefExp(temp_variableSymbol);
                    setSourcePosition(arrayVariable,nameToken);

                 // See test2007_36.f90 for an example of where this can be false (data statement)
                 // ROSE_ASSERT(astExpressionStack.empty() == false);
                    if (astExpressionStack.empty() == false)
                       {
                         SgExpression* indexExpression = astExpressionStack.front();
                      // printf ("#################### variable name = %s indexExpression->get_type() = %s stackHoldsAnIndexExpression = %s \n",variableSymbol->get_name().str(),indexExpression->get_type()->class_name().c_str(),stackHoldsAnIndexExpression ? "true" : "false");

                      // Don't allow arrays to be index by arrays (can we do that).
                         SgArrayType* arrayType = isSgArrayType(indexExpression->get_type());
                      // if (arrayType == NULL)
                         if (arrayType == NULL && stackHoldsAnIndexExpression == true)
                            {
                              astExpressionStack.pop_front();
                              variable = new SgPntrArrRefExp(arrayVariable,indexExpression,NULL);
                            }
                           else
                            {
                           // This is the case of an array expression (array language expression)
                              variable = arrayVariable;
                            }
                       }
                      else
                       {
                      // This is the case of a data statement
                         variable = arrayVariable;
                       }
                    break;
                  }

            // See test2010_167.f90 for an example of where SgFunctionType is handled.
               case V_SgFunctionType:
                  {
                 // printf ("Handling variableType as SgFunctionType \n");
                    SgFunctionSymbol* functionSymbol = isSgFunctionSymbol(tempSymbol);

                 // ROSE_ASSERT(functionSymbol != NULL);
                    if (functionSymbol != NULL)
                       {
                         ROSE_ASSERT(functionSymbol->get_declaration() != NULL);
                         ROSE_ASSERT(functionSymbol->get_declaration()->get_type() != NULL);
                         SgFunctionType* functionType = isSgFunctionType(functionSymbol->get_declaration()->get_type());
                         ROSE_ASSERT(functionType != NULL);
                         SgExpression* functionReference = new SgFunctionRefExp(functionSymbol,functionType);
                         ROSE_ASSERT(functionReference != NULL);

                         setSourcePosition(functionReference,nameToken);
                      // DQ (12/28/2010): This branch is required for test2007_57.f90 to work.
                      // Take the function call expression from the astExpressionStack
                         variable = astExpressionStack.front();
                         astExpressionStack.pop_front();
                       }
                      else
                       {
                      // DQ (1/24/2011): Added case to support variables with SgFunctionType to support procedure pointers.
                      // See test2011_28.f90 for an example.
                         SgVariableSymbol* variableSymbol = isSgVariableSymbol(tempSymbol);

                         if (stackHoldsAnIndexExpression == true)
                            {
                           // printf ("This variable reference must be converted into a function call. \n");
                           // printf ("tempSymbol = %p = %s \n",tempSymbol,tempSymbol->class_name().c_str());

                           // Get the function symbol so that we can construct a function call.
                              SgScopeStatement* topOfStack = *(astScopeStack.rbegin());
                              ROSE_ASSERT(topOfStack != NULL);

                           // printf ("topOfStack = %p = %s \n",topOfStack,topOfStack->class_name().c_str());
                              SgGlobal* globalScope = isSgGlobal(topOfStack);
                              ROSE_ASSERT(globalScope != NULL);

                           // DQ (1/24/2011): Default name for procedure to use as declaration for symbol needed for function reference...(where required)...
                              SgName programName = "procedure";
                              ROSE_ASSERT(globalScope->symbol_exists(programName) == true);
                           // functionSymbol = globalScope->lookup_function_symbol(programName);
                              functionSymbol = globalScope->lookup_function_symbol(programName);
                              ROSE_ASSERT(functionSymbol != NULL);

                              ROSE_ASSERT(functionSymbol->get_declaration() != NULL);
                              ROSE_ASSERT(functionSymbol->get_declaration()->get_type() != NULL);
                              SgFunctionType* functionType = isSgFunctionType(functionSymbol->get_declaration()->get_type());

                              ROSE_ASSERT(functionType != NULL);
                              SgExpression* functionReference = new SgFunctionRefExp(functionSymbol,functionType);
                              ROSE_ASSERT(functionReference != NULL);

                              setSourcePosition(functionReference,nameToken);

                              ROSE_ASSERT(astExpressionStack.empty() == false);
                              SgExprListExp* argumentList = isSgExprListExp(astExpressionStack.front());
                              ROSE_ASSERT(argumentList != NULL);
                              astExpressionStack.pop_front();
                              SgFunctionCallExp* functionCall = new SgFunctionCallExp(functionReference,argumentList,SgTypeVoid::createType());

                           // DQ (12/28/2010): Fixing test2007_57.f90...
                           // variable = functionReference;
                              variable = functionCall;
                           // DQ (12/28/2010): This branch is required for test2007_57.f90 to work.
                           // Take the function call expression from the astExpressionStack
                           // variable = astExpressionStack.front();
                           // astExpressionStack.pop_front();
                            }
                           else
                            {
                              SgExpression* arrayVariable = new SgVarRefExp(variableSymbol);
                              setSourcePosition(arrayVariable,nameToken);
                              variable = arrayVariable;
                            }
                       }

                    break;
                  }

               default:
                  {
#if 0
                    printf ("No special processing is required, this variable type is %s tempSymbol = %s \n",variableType->class_name().c_str(),tempSymbol->class_name().c_str());
#endif
                 // variable = new SgVarRefExp(variableSymbol);
                    ROSE_ASSERT(tempSymbol != NULL);
                    SgVariableSymbol* temp_variableSymbol = isSgVariableSymbol(tempSymbol);
                    ROSE_ASSERT(temp_variableSymbol != NULL);
                    variable = new SgVarRefExp(temp_variableSymbol);
                  }
             }

          ROSE_ASSERT(variable != NULL);

       // printf ("Calling setSourcePosition \n");
       // setSourcePosition(variableReference,getTopOfNameStack());
       // setSourcePosition(variable,nameToken);

       // Only call setSourcePosition() where it has not been previously called.
          if (variable != NULL && variable->get_startOfConstruct() == NULL)
             {
               setSourcePosition(variable,nameToken);
             }
       // printf ("DONE: Calling setSourcePosition \n");

          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
               printf ("Pushing variable = %p = %s onto the expression stack \n",variable,variable->class_name().c_str());

       // Save the expression on the stack
       //   astExpressionStack.push_front(variable);

       // FMZ (2/10/2009): Need to check if it is a CoArrayExpression
          if (processCoarray)
             {
               coExpr->set_referData(variable);
               setSourcePosition(coExpr,nameToken);
            // astExpressionStack.push_front(coExpr);
            // intermediateExpresionList.push_front(variable);
             }
            else
             {
            // astExpressionStack.push_front(variable);
            // intermediateExpresionList.push_front(variable);
             }

       // DQ (12/28/2010): Fixing test2007_57.f90...
       // intermediateExpresionList.push_front(variable);
          if (variable != NULL)
               intermediateExpresionList.push_front(variable);
        }

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("Before removing names from astNameStack at BOTTOM of R612 c_action_data_ref()");
#endif

  // Transfer the expressions from the intermediateExpresionList to the astExpressionStack
  // printf ("In R612: intermediateExpresionList.size() = %zu \n",intermediateExpresionList.size());
     size_t intermediateExpresionListSize = intermediateExpresionList.size();
     for (size_t i = 0; i < intermediateExpresionListSize; i++)
        {
#if 0
          printf ("intermediateExpresionList.front() = %p = %s \n",intermediateExpresionList.front(),intermediateExpresionList.front()->class_name().c_str());
#endif
          astExpressionStack.push_front(intermediateExpresionList.back());
          intermediateExpresionList.pop_back();
        }

  // Need to pop tokens from stack
     ROSE_ASSERT(astNameStack.empty() == false);

  // DQ (12/14/2010): Need to pop of a specific number of tokens from the stack!
  // astNameStack.pop_front();
  // printf ("----------- Removing %d from the stack! \n",numPartRef);
     for (int i = 0; i < numPartRef; i++)
        {
          ROSE_ASSERT(astNameStack.empty() == false);
          astNameStack.pop_front();

       // DQ (12/29/2010): These should be kept in sync (or at least there shuld be one astNameStack
       // entry for each astHasSelectionSubscriptStack entry.  But maybe not the other way around.
          ROSE_ASSERT(astHasSelectionSubscriptStack.empty() == false);
          astHasSelectionSubscriptStack.pop_front();

       // DQ (12/29/2010): Enforce our new rule (fails for test2007_17.f90).
       // ROSE_ASSERT(astNameStack.size() == astHasSelectionSubscriptStack.size());

       // DQ (12/29/2010): Alternative structure to using astNameStack and astHasSelectionSubscriptStack.
          ROSE_ASSERT(astMultipartReferenceStack.empty() == false);
          astMultipartReferenceStack.pop_front();
        }
#endif

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("After removing names from astNameStack at BOTTOM of R612 c_action_data_ref()");
#endif

     for (int i = 1; i < numPartRef; i++)
        {
       // This is a record reference, in Fortran this is "record%field"
       // in C it is "record.field".  "field" is on the top of the stack
       // and "record" is next on the stack.  First we have to find the
       // record then reference the field off of the record.
#if 1
       // Output debugging information about saved state (stack) information.
          outputState("Loop to build structure member references in R612 c_action_data_ref()");
#endif

          ROSE_ASSERT(astExpressionStack.empty() == false);
          SgExpression* lhs = astExpressionStack.front();
          astExpressionStack.pop_front();

       // printf ("Check the type of the LHS and if it is an array then assume there is an index expression on the astExpressionStack! \n");
          ROSE_ASSERT(lhs != NULL);
          ROSE_ASSERT(lhs->get_type() != NULL);
          SgArrayType* arrayType = isSgArrayType(lhs->get_type());
          if (arrayType != NULL)
             {
            // printf ("Need to factor in indexing expression on stack to this array expression \n");
               ROSE_ASSERT(astExpressionStack.empty() == false);

            // Note that the index expression is under the list of expressions given by the size == numPartRef!!!
            // Maybe this should be handled in R613 more directly!!!

            // printf ("Finish this tomorrow \n");
            // ROSE_ASSERT(false);
             }

          ROSE_ASSERT(astExpressionStack.empty() == false);
          SgExpression* rhs = astExpressionStack.front();
          astExpressionStack.pop_front();

       // printf ("Check the type of the RHS and if it is an array then assume there is an index expression on the astExpressionStack! \n");

          SgExpression* recordReference = new SgDotExp(lhs,rhs,NULL);
          setSourcePosition(recordReference);
          setSourcePositionCompilerGenerated(recordReference);

       // Push the record reference onto the expression stack
          astExpressionStack.push_front(recordReference);
        }

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R612 c_action_data_ref()");
#endif
   }


/** R613, R613-F2008
 * part_ref
 *  :   T_IDENT T_LPAREN section_subscript_list T_RPAREN (image_selector)? 
 *      |       T_IDENT image_selector
 *      |       T_IDENT
 *
 * @param id The identifier (variable name in most cases (all?))
 * @param hasSelectionSubscriptList True if a selection-subscript-list is present
 * @param hasImageSelector Ture if an image-selector is present
 */
void c_action_part_ref(Token_t * id, ofp_bool hasSelectionSubscriptList, ofp_bool hasImageSelector)
   {
  // This is a part of a variable reference (any likely used many other places as well)

  // DQ (12/29/2010): Notes on how R612 and R613 operate together.
  // This rule works with R612 to support references to names that have multiple parts (e.g. struct%field, etc.)
  // However, it is difficult to support but R612 and R613, so a lot of work has gone into these functions.
  // This function (R613) does not modify state (except for the handling of a statement function where any
  // previous symbol of that name is removed in preparation for R612 to build a proper statement function IR node
  // (as I recall).  Mostly this function pushes the name onto the astNameStack, but it is a problem that the
  // value of hasSelectionSubscriptList is lost and not saved since it is needed to correctly handle the
  // interpretation of the names and know which are associated with index expression (SgExprList IR nodes 
  // already saved onto the stack.  So we add another auxilery stack to save the information about the value
  // of the hasSelectionSubscriptList.  

  // A possible alternative to saving the value of the hasSelectionSubscriptList might be to do more processing
  // in this R613 rule. However, the order of calls to R613 does not permit a stack to be used to save the
  // scope to which a named reference can be interpreted.  All names must be evaluated once the full list of
  // multiple parts are available (i.e. in R612).  I don't think there is any option here.  so most processing is
  // done in R612 and very little can be done in R612.

  // DQ (12/14/2007): Make sure that we have added the implicit program function to the scope (see test2007_17.f90)
     build_implicit_program_statement_if_required();

     ROSE_ASSERT(id != NULL);

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R613 c_action_part_ref() id = %s on line %d hasSelectionSubscriptList = %s hasImageSelector = %s \n",
               id->text,id->line,hasSelectionSubscriptList ? "true" : "false",hasImageSelector ? "true" : "false");

#if !SKIP_C_ACTION_IMPLEMENTATION
     SgName name = id->text;
     SgVariableSymbol* variableSymbol = NULL;
     SgFunctionSymbol* functionSymbol = NULL;
     SgClassSymbol*    classSymbol    = NULL;
     trace_back_through_parent_scopes_lookup_variable_symbol_but_do_not_build_variable(name,getTopOfScopeStack(),variableSymbol,functionSymbol,classSymbol);

  // if (hasSelectionSubscriptList == true || hasSelectionSubscriptList == false)
     if (hasSelectionSubscriptList == true)
        {
       // This means that it is has "()" as in "foo()" or "foo(i)"
       // Note that this does not imply that this is an array or a function yet.
       // Also this R613 is called to process "N" within "write (1) N (i,i=1,100)" so it could also be a scalar.
       // And even if it didn't have a "()" it could still be a function in a function call (see test2010_169.f90).

#if 0
       // Output debugging information about saved state (stack) information.
          outputState("At TOP of R613 c_action_part_ref() hasSelectionSubscriptList == true");
#endif
       // If this is a previously declared as an array then we can leave it alone, since this is the 
       // indexing of that array.  But if it was declared as a variable then we need to change it to 
       // a function as if there was an "external" declaration (not available until F90).

       // printf ("In R613 c_action_part_ref() (hasSelectionSubscriptList == true): variableSymbol = %p \n",variableSymbol);
          if (variableSymbol != NULL)
             {
            // This variable was declared previously, if it is an array, then we do nothing. But if it is a 
            // scalar then we have to convert it to a function returning the type of the scalar declaration.
               if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                    printf ("We might have to convert this from a scalar to a function = %s \n",name.str());

            // FMZ (9/8/2009): the following line cause trouble when "interface" block presented. 
            // ROSE_ASSERT(functionSymbol == NULL);

               SgInitializedName* variableName = variableSymbol->get_declaration();
               ROSE_ASSERT(variableName != NULL);

               SgType* variableType = variableName->get_type();
               ROSE_ASSERT(variableType != NULL);

               if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                    printf ("variableType = %s \n",variableType->class_name().c_str());

               SgArrayType* arrayType = isSgArrayType(variableType);

            // FMZ (10/30/2009) could be pointer type 
               SgPointerType* pointerType = isSgPointerType(variableType);
               if (pointerType !=NULL) 
                  {
                    arrayType = isSgArrayType(pointerType->get_base_type());
                  }

            // DQ (8/21/2010): Added support for string type so we have to eliminate SgTypeString as a posability before we conclude that we should build a function.
               SgTypeString* stringType = isSgTypeString(variableType);

            // DQ (1/24/2011): Added support for procedure pointer variables.
               SgFunctionType* functionType = isSgFunctionType(variableType);

            // If this is either an array or a string type or a function type, don't convert it to a function.
            // if (arrayType != NULL || stringType != NULL)
               if (arrayType != NULL || stringType != NULL || functionType != NULL)
                  {
                    if (arrayType != NULL)
                       {
                         if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                              printf ("This is an array type so it is OK for it to be indexed \n");

  // DQ (12/14/2010): Removed the support for recursive handling of R612 by NOT pushing the class type's scope onto the astScopeStack.
#if 0
                      // FMZ (2/9/2010) derived type
                         class_type = isSgClassType(arrayType->get_base_type());
#endif
                       }

                 // DQ (1/24/2011): Added support for procedure pointer variables.
                    if (functionType != NULL)
                       {
                         if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                              printf ("This is an function type so it is OK for it to be called with parameters (procedure pointer variable) \n");
                       }
                  }
                 else
                  {
                 // This case is visited in the handling of Fortran statement functions (see test2007_179.f90).
                 // See also test2011_30.f90 for procedure pointers when used to call the functions to which they are pointed.
#if 0
                    printf ("This case is visited in the handling of fortran statement functions (see test2007_179.f90). \n");
#endif
                    if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                         printf ("This is NOT an array type so it must be converted to a function call with argument (if arguments are required)\n");
                    ROSE_ASSERT(matchingName(variableName->get_name().str(),id->text) == true);
#if 0
                 // This will be built in R612, to trigger it we need to remove the variableSymbol
                 // convertVariableSymbolToFunctionCallExp(variableSymbol,id);
                    generateFunctionCall(id);
#endif
                 // Remove the associated variable symbol so this will not be confused later
                    SgScopeStatement* scope = variableName->get_scope();
                    if (scope == NULL)
                       {
                         printf ("Error: scope == NULL for variableName = %p = %s \n",variableName,variableName->get_name().str());
                       }
                    ROSE_ASSERT(scope != NULL);

                 // Note that we might want to clean up more than just removing the variableSymbol from the symbol table
                    scope->remove_symbol(variableSymbol);
#if 0
                 // Output debugging information about saved state (stack) information.
                    outputState("At BOTTOM of R613 c_action_part_ref()");
#endif
#if 0
                    printf ("Exiting as a test -- after calling scope->remove_symbol(variableSymbol) \n");
                    ROSE_ASSERT(false);
#endif
                  }
             }
            else
             {
            // If this is either a SgFunctionSymbol or a SgClassSymbol, it will be handled by another rule (likely R612).
               if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                    printf ("This will be converted to a function or type later since it does not have a previous declaration as a variable \n");
             }
        }
       else
        {
       // Note that test2007_164.f demonstrates that this can be a function reference... and so lacking selection-subscript-list implies this is a scalar variable.

          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
               printf ("case of hasSelectionSubscriptList == false, but test2007_164.f demonstrates that this can still be a function reference in stead of a scalar variable \n");
        }

     if (hasImageSelector == true)
        {
       // DQ (12/29/2010): I have never seen this to be true in any Fortran code (but it is likely a F03 or F08 detail).

          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
               printf ("Sorry, case of hasImageSelector == true not implemented (I forget what this even means, likely some self-esteem issue for Fortran) \n");

          printf ("Detect when hasImageSelector == true within R613 (we need an example of this sort of code before it can be supported in ROSE) \n");
          ROSE_ASSERT(false);
        }

  // DQ (12/29/2010): These should be kept in sync (or at least there shuld be one astNameStack 
  // entry for each astHasSelectionSubscriptStack entry.  But maybe not the other way around.
     astNameStack.push_front(id);
     astHasSelectionSubscriptStack.push_front(hasSelectionSubscriptList);

  // DQ (12/29/2010): Enforce our new rule (fails for test2007_17.f90).
  // ROSE_ASSERT(astNameStack.size() == astHasSelectionSubscriptStack.size());

  // DQ (12/29/2010): Stand up redundant stack to replace use of astNameStack and astHasSelectionSubscriptStack in R612 and R613 handling.
     SgName id_name = id->text;
     astMultipartReferenceStack.push_front(MultipartReferenceType(id_name,hasSelectionSubscriptList));
#endif

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R613 c_action_part_ref()");
#endif
   }

/** R619  (see R1220, actual_arg_spec)
 * section_subscript/actual_arg_spec
 *      :       expr section_subscript_suffix
 *      |       T_COLON (expr)? (T_COLON expr)?
 *      |       T_COLON_COLON expr
 *      |       T_IDENT T_EQUALS (expr | T_ASTERISK label ) // could be an actual-arg-spec, see R1220
 *      |       T_ASTERISK label // could be an actual-arg-spec, see R1220 
 *      |       { // empty could be an actual-arg, see R1220 // }
 *
 * R619, section_subscript has been combined with actual_arg_spec (R1220) 
 * to reduce backtracking.  Only the first alternative is truly ambiguous.
 * 
 * @param hasLowerBound True if lower bound is present in a section-subscript (lower_bound:upper_bound:stride).
 * @param hasUpperBound True if upper bound is present in a section-subscript (lower_bound:upper_bound:stride).
 * @param hasStride True if stride is present in a section-subscript (lower_bound:upper_bound:stride).
 * @param isAmbiguous True if the third alternative is taken
 */
void c_action_section_subscript(ofp_bool hasLowerBound, ofp_bool hasUpperBound, ofp_bool hasStride, ofp_bool isAmbiguous)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R619 c_action_section_subscript() hasLowerBound = %s hasUpperBound = %s hasStride = %s isAmbiguous = %s \n",
               hasLowerBound ? "true" : "false", hasUpperBound ? "true" : "false", hasStride ? "true" : "false", isAmbiguous ? "true" : "false");

  // DQ (4/26/2008): The problem here is that this rule can be called in the evaluation of initializers for 
  // types in structures (fortran types).  This might be an error in the parser, since it allows that there
  // is another rule that should be called but it is never called by OFP.  Discussions with Craig have
  // concluded that we should handle type initialization using this rule and just get over it :-).
  // The only clue that this rule is being used for initialization is that there are no arguments on the
  // astExpressionStack and that all the arguments are on the astInitializationStack (after a recent rewrite
  // of the general initialization support in the Fortran support of ROSE).

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R619 c_action_section_subscript()");
#endif

  // This is required for the processing of "a(:,:,:,n)", however going to be a problem for the type initialization!
     SgExpression* subscript = buildSubscriptExpression(hasLowerBound,hasUpperBound,hasStride,isAmbiguous);

     astExpressionStack.push_front(subscript);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R619 c_action_section_subscript()");
#endif
   }

/** R619 list
 * section_subscript
 * section_subscript_list
 *      :       section_subscript ( T_COMMA section_subscript )*
 * 
 * @param count The number of items in the list.
 */
void c_action_section_subscript_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R619 c_action_section_subscript_list__begin() \n");
   }

void c_action_section_subscript_list(int count)
   {
  // This is either a subscript or a function parameter list, or an implicit do loop.  If it is an implicit function then treat 
  // it as a function call directly, later some array references may have be be fixed up to be function 
  // calls but this will be a post processing step.

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R619 c_action_section_subscript_list(): count = %d \n",count);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R619 c_action_section_subscript_list()");
#endif

  // DQ (4/28/2008): If the astExpressionStack is empty then this is being called as part
  // of the rules for initializers (which I think mistakenly use the subscript support).
  //
  // FMZ (5/10/2010): When function call doesn't have argument, we have to generate SgExpListExp:
  //  example:  
  //       -if (foo() .eq. 1) then ....end if
  //         the express stack is empty in this case.
  //       -if (1 .eq. foo()) then ... end if 
  //         the expression stack is not empty, but it is "SgIntVal" for "1" not for the subscript list
     if (astExpressionStack.empty() == false || count == 0)
        {

          SgExprListExp* expressionList = new SgExprListExp();
       // rose_check(expressionList != NULL);

          ROSE_ASSERT(expressionList != NULL);

          setSourcePosition(expressionList);

       // printf ("In c_action_section_subscript_list(): astActualArgumentNameStack.size() = %zu \n",astActualArgumentNameStack.size());

          for (int i=0; i < count; i++)
             {
            // printf ("count = %d i = %d \n",count,i);

            // rose_check(astExpressionStack.empty() == false);

#if 1
            // Output debugging information about saved state (stack) information.
               outputState("In loop over expressions and labels in R619 c_action_section_subscript_list()");
#endif
            // DQ (1/30/2011): Note that test2010_164.f90 demonstrates alternative return arguments.
            // If the astExpressionStack is empty, then we check the astLabelSymbolStack and use the
            // argument there (though we need to some sort of label expression to handle this properly.
            // ROSE_ASSERT(astExpressionStack.empty() == false);
               SgExpression* expression = NULL;
               if (astExpressionStack.empty() == false)
                  {
                    expression = astExpressionStack.front();

                 // Older code...
                 // DQ (11/30/2007): Actual arguments have associated names which have to be recorded on to a separate stack.
                 // test2007_162.h demonstrates this problems (and test2007_184.f). Built astActualArgumentNameStack to support this.
                 // Actual argument specification permits function arguments to be specified in any order. Examples: sum(array,dim=1)
                    if (astActualArgumentNameStack.empty() == false)
                       {
                      // print_token(astActualArgumentNameStack.front());
                         SgName name = astActualArgumentNameStack.front()->text;

                      // See test2007_162.f for an example of where this restriction is required.  It might 
                      // be that we need a special stack for this!
                      // if (name != "defaultString")
                            {
                              expression = new SgActualArgumentExpression(name,expression);
                              setSourcePosition(expression,astActualArgumentNameStack.front());
                            }

                         astActualArgumentNameStack.pop_front();
                       }

                    astExpressionStack.pop_front();
                  }
                 else
                  {
                 // This is an alternative return arguments.
                    ROSE_ASSERT(astLabelSymbolStack.empty() == false);
                 // expression = astLabelSymbolStack.front()->get_numeric_label_value();
                    int integerLabelValue = astLabelSymbolStack.front()->get_numeric_label_value();
                 // printf ("integerLabelValue = %d \n",integerLabelValue);
                    expression = new SgIntVal(integerLabelValue,"");
                    setSourcePosition(expression);
                    SgName name = "*";
                 // printf ("name = %s \n",name.str());
                    expression = new SgActualArgumentExpression(name,expression);
                    setSourcePosition(expression,astActualArgumentNameStack.front());

                    astLabelSymbolStack.pop_front();
                 // astActualArgumentNameStack.pop_front();
                  }

            // DQ (1/30/2010): These cases appear to be inconsistant, but I think that the append_expression() is the correct version.
            // This is a bug that needs to be resolved.  Using "prepend_expression()" is what is required to pass the regression tests.
#if 0
            // This will cause test2010_164.f90 to be unparsed correctly (it passes but the arguments are in the wrong order).
            // DQ (1/30/2010): This is fixed to generate the function call arguments in the correct order.
            // expressionList->prepend_expression(expression);
               expressionList->append_expression(expression);
#else
            // This will cause test2007_57.f90 to be unparsed correctly.
               expressionList->prepend_expression(expression);
#endif

               expression->set_parent(expressionList);
            // astExpressionStack.pop_front();
             }

       // printf ("In c_action_section_subscript_list(): AFTER astExpressionStack processing \n");

          astExpressionStack.push_front(expressionList);
        }

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R619 c_action_section_subscript_list()");
#endif
   }

/** R623
 * allocate_stmt
 *      :       (label)? T_ALLOCATE_STMT_1 T_ALLOCATE T_LPAREN type_spec T_COLON_COLON
 *              allocation_list (T_COMMA alloc_opt_list)? T_RPAREN T_EOS
 *      |       (label)? T_ALLOCATE T_LPAREN
 *              allocation_list (T_COMMA alloc_opt_list)? T_RPAREN T_EOS
 *
 * @param label Optional statement label
 * @param hasTypeSpec True if type-spec is present
 * @param hasAllocOptList True if alloc-opt-list is present
 */
// void c_action_allocate_stmt(Token_t * label, ofp_bool hasTypeSpec, ofp_bool hasAllocOptList)
void c_action_allocate_stmt(Token_t *label, Token_t *allocateKeyword, Token_t *eos, ofp_bool hasTypeSpec, ofp_bool hasAllocOptList)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R623 c_action_allocate_stmt() label = %p = %s allocateKeyword = %p = %s hasTypeSpec = %s hasAllocOptList = %s \n",
               label,label != NULL ? label->text : "NULL",
               allocateKeyword,allocateKeyword != NULL ? allocateKeyword->text : "NULL",
               hasTypeSpec ? "true" : "false", hasAllocOptList ? "true" : "false");

  // An AttributeSpecification statement can be the first statement in a program
     build_implicit_program_statement_if_required();

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R623 c_action_allocate_stmt()");
#endif

  // There should be a SgExprListExp on the astExpressionStack.
     ROSE_ASSERT(astExpressionStack.empty() == false);

     SgAllocateStatement* allocateStatement = new SgAllocateStatement();

     ROSE_ASSERT(allocateKeyword != NULL);
     setSourcePosition(allocateStatement,allocateKeyword);

  // DQ (1/31/2009): Added support for alloc options
     if (hasAllocOptList == true)
        {
          while (astNameStack.empty() == false)
             {
               ROSE_ASSERT(astNameStack.empty() == false);
               Token_t* token = astNameStack.front();
               astNameStack.pop_front();

               ROSE_ASSERT(astExpressionStack.empty() == false);
               string text = token->text;
               if (matchingName(text,"STAT"))
                  {
                    allocateStatement->set_stat_expression(astExpressionStack.front());
                  }
                 else
                  {
                    if (matchingName(text,"ERRMSG"))
                       {
                         allocateStatement->set_errmsg_expression(astExpressionStack.front());
                       }
                      else
                       {
                      // This is an F2003 specific option.
                         if (matchingName(text,"SOURCE"))
                            {
                              allocateStatement->set_source_expression(astExpressionStack.front());
                            }
                           else
                            {
                           // Error
                              printf ("Unknown dealloc option: %s \n",text.c_str());
                              ROSE_ASSERT(false);
                            }
                       }
                  }

               astExpressionStack.pop_front();
             }
        }

     SgExprListExp* exprList = isSgExprListExp(astExpressionStack.front());
     ROSE_ASSERT(exprList != NULL);
     allocateStatement->set_expr_list(exprList);

     astExpressionStack.pop_front();

     astScopeStack.front()->append_statement(allocateStatement);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R623 c_action_allocate_stmt()");
#endif
   }

/** R624
 * alloc_opt
 *      :       T_IDENT                 // {'STAT','ERRMSG'} are variables {SOURCE'} is expr
 *              T_EQUALS expr
 *
 * @param allocOpt Identifier representing {'STAT','ERRMSG','SOURCE'}
 */
void c_action_alloc_opt(Token_t * allocOpt)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R624 c_action_alloc_opt() allocOpt = %p = %s \n",allocOpt,allocOpt != NULL ? allocOpt->text : "NULL");

     ROSE_ASSERT(allocOpt != NULL);

  // This is either the STAT token, ERRMSG token, or SOURCE token.
     astNameStack.push_front(allocOpt);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R624 c_action_alloc_opt()");
#endif
   }

/** R624 list
 * alloc_opt_list
 *      :       alloc_opt ( T_COMMA alloc_opt )*
 * 
 * @param count The number of items in the list.
 */
void c_action_alloc_opt_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R624 c_action_alloc_opt_list__begin() \n");
   }
void c_action_alloc_opt_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R624 c_action_alloc_opt_list() count = %d \n",count);
   }

/** R628, R631-F2008
 * allocation
 *              ( T_LPAREN allocate_shape_spec_list {hasAllocateShapeSpecList=true;} T_RPAREN )?
 *              ( T_LBRACKET allocate_co_array_spec {hasAllocateCoArraySpec=true;} T_RBRACKET )?
 *
 * NOTE: In current parser, hasAllocateShapeSpecList is always false, appears as
 * R619 section-subscript-list.  In a section-subscript, the stride shall not be present
 * and if hasUpperBound is false, hasLowerBound shall be present and must be interpreted
 * as an upper-bound-expr.
 * 
 * @param hasAllocateShapeSpecList True if allocate-shape-spec-list is present.
 * @param hasAllocateCoArraySpec True if allocate-co-array-spec is present.
 */
void c_action_allocation(ofp_bool hasAllocateShapeSpecList, ofp_bool hasAllocateCoArraySpec)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R628 c_action_allocation() hasAllocateShapeSpecList = %s hasAllocateCoArraySpec = %s \n",
               hasAllocateShapeSpecList ? "true" : "false", hasAllocateCoArraySpec ? "true" : "false");
   }

/** R628 list
 * allocation_list
 *      :       allocation ( T_COMMA allocation )*
 * 
 * @param count The number of items in the list.
 */
void c_action_allocation_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R628 (list) c_action_allocation_list__begin() \n");
   }
void c_action_allocation_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R628 (list) c_action_allocation_list() count = %d \n",count);

     SgExprListExp* expressionList = new SgExprListExp();
     setSourcePosition(expressionList);

     for (int i = 0; i < count; i++)
        {
       // Transfer the IR node from the astExpressionStack to the astInitializerStack.
          ROSE_ASSERT(astExpressionStack.empty() == false);
          SgExpression* expr = astExpressionStack.front();
          astExpressionStack.pop_front();

          ROSE_ASSERT(expr != NULL);
       // expressionList->append_expression(expr);
          expressionList->prepend_expression(expr);
        }

  // Save the SgExprListExp on the astExpressionStack
     astExpressionStack.push_front(expressionList);
   }

/**
 * R629
 * allocate_object
 *
 */
void c_action_allocate_object()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R629 c_action_allocate_object() \n");
   }

/** R629 list
 * allocate_object_list
 *      :       allocate_object ( T_COMMA allocate_object )*
 * 
 * @param count The number of items in the list.
 */
void c_action_allocate_object_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R629 (list) c_action_allocate_object_list__begin() \n");
   }
void c_action_allocate_object_list(int count)
   {
  // This is the function that is called before the deallocate action to do for
  // c_action_deallocate_stmt() what c_action_allocation_list() does for 
  // c_action_allocate_stmt().  I don't know why it is named "c_action_allocate_object_list()"

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R629 (list) c_action_allocate_object_list() count = %d \n",count);

     SgExprListExp* expressionList = new SgExprListExp();
     setSourcePosition(expressionList);

     for (int i = 0; i < count; i++)
        {
       // Transfer the IR node from the astExpressionStack to the astInitializerStack.
          ROSE_ASSERT(astExpressionStack.empty() == false);
          SgExpression* expr = astExpressionStack.front();
          astExpressionStack.pop_front();

          ROSE_ASSERT(expr != NULL);
       // expressionList->append_expression(expr);
          expressionList->prepend_expression(expr);
        }

  // Save the SgExprListExp on the astExpressionStack
     astExpressionStack.push_front(expressionList);
   }

/** R630
 * allocate_shape_spec
 *      :       expr (T_COLON expr)?
 *
 * NOTE: not called by current parser, appears as R619 section-subscript instead
 *
 * @param hasLowerBound True if optional lower-bound-expr is present.
 * @param hasUpperBound True if upper-bound-expr is present (note always true).
 */
void c_action_allocate_shape_spec(ofp_bool hasLowerBound, ofp_bool hasUpperBound)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R630 c_action_allocate_shape_spec(): hasLowerBound = %s hasUpperBound = %s \n",
               hasLowerBound ? "true" : "false", hasUpperBound ? "true" : "false");
   }

/** R630 list
 * allocate_shape_spec_list
 *      :       allocate_shape_spec ( T_COMMA allocate_shape_spec )*
 * 
 * @param count The number of items in the list.
 */
void c_action_allocate_shape_spec_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R630 (list) c_action_allocate_shape_spec_list__begin() \n");
   }
void c_action_allocate_shape_spec_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R630 (list) c_action_allocate_shape_spec_list() count = %d \n",count);
   }

/** R633
 *      nullify_stmt
 *      :       (label)? T_NULLIFY T_LPAREN pointer_object_list T_RPAREN T_EOS
 *
 * @param label Optional statement label
 */
// void c_action_nullify_stmt(Token_t * label)
void c_action_nullify_stmt(Token_t *label, Token_t *nullifyKeyword, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_nullify_stmt(): label = %p = %s nullifyKeyword = %p = %s \n",
               label,label != NULL ? label->text : "NULL",
               nullifyKeyword,nullifyKeyword != NULL ? nullifyKeyword->text : "NULL");

  // argument list 
  // SgNullifyStatement
     SgExpression* exp = NULL;
     while (astExpressionStack.empty() == false)
        {
          exp =  astExpressionStack.front();
          astExpressionStack.pop_front();
        }
   }

/** R634 list
 * pointer_object_list
 *      :       pointer_object ( T_COMMA pointer_object )*
 * 
 * @param count The number of items in the list.
 */
void c_action_pointer_object_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R634 (list) c_action_pointer_object_list__begin() \n");
   }
void c_action_pointer_object_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R634 (list) c_action_pointer_object_list() count = %d \n",count);
   }

/** R635
 * deallocate_stmt
 *
 * :   (label)? T_DEALLOCATE T_LPAREN allocate_object_list 
 *              ( T_COMMA dealloc_opt_list)? T_RPAREN T_EOS
 *
 * @param label The label.
 * @param hasDeallocOptList True if there is an option list.
 */
// void c_action_deallocate_stmt(Token_t * label,ofp_bool hasDeallocOptList)
void c_action_deallocate_stmt(Token_t *label, Token_t *deallocateKeyword, Token_t *eos, ofp_bool hasDeallocOptList)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_deallocate_stmt(): label = %p = %s nullifyKeyword = %p = %s hasDeallocOptList = %s \n",
               label,label != NULL ? label->text : "NULL",
               deallocateKeyword,deallocateKeyword != NULL ? deallocateKeyword->text : "NULL",
               hasDeallocOptList ? "true" : "false");

  // An AttributeSpecification statement can be the first statement in a program
     build_implicit_program_statement_if_required();

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R635 c_action_deallocate_stmt()");
#endif

  // There should be a SgExprListExp on the astExpressionStack.
     ROSE_ASSERT(astExpressionStack.empty() == false);

     SgDeallocateStatement* deallocateStatement = new SgDeallocateStatement();

     ROSE_ASSERT(deallocateKeyword != NULL);
     setSourcePosition(deallocateStatement,deallocateKeyword);

  // DQ (1/31/2009): Added support for dealloc options
     if (hasDeallocOptList == true)
        {
          while (astNameStack.empty() == false)
             {
               ROSE_ASSERT(astNameStack.empty() == false);
               Token_t* token = astNameStack.front();
               astNameStack.pop_front();

               ROSE_ASSERT(astExpressionStack.empty() == false);
               string text = token->text;
               if (matchingName(text,"STAT"))
                  {
                    deallocateStatement->set_stat_expression(astExpressionStack.front());
                  }
                 else
                  {
                    if (matchingName(text,"ERRMSG"))
                       {
                         deallocateStatement->set_errmsg_expression(astExpressionStack.front());
                       }
                      else
                       {
                      // Error
                         printf ("Unknown dealloc option: %s \n",text.c_str());
                         ROSE_ASSERT(false);
                       }
                  }

               astExpressionStack.pop_front();
             }
        }

     SgExprListExp* exprList = isSgExprListExp(astExpressionStack.front());
     ROSE_ASSERT(exprList != NULL);
     deallocateStatement->set_expr_list(exprList);

     astExpressionStack.pop_front();

     astScopeStack.front()->append_statement(deallocateStatement);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R635 c_action_deallocate_stmt()");
#endif
   }

/**
 * R636
 * dealloc_opt
 *
 * @param id T_IDENT for 'STAT' or 'ERRMSG'.
 */
void c_action_dealloc_opt(Token_t *id)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_dealloc_opt(): id = %p = %s \n",id,id != NULL ? id->text : "NULL");

     ROSE_ASSERT(id != NULL);

  // This is either the STAT token or the ERRMSG token.
     astNameStack.push_front(id);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R636 c_action_dealloc_opt()");
#endif
   }


/** R636 list
 * dealloc_opt_list
 *      :       dealloc_opt ( T_COMMA dealloc_opt )*
 * 
 * @param count The number of items in the list.
 */
void c_action_dealloc_opt_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R636 (list) c_action_dealloc_opt_list__begin() \n");
   }
void c_action_dealloc_opt_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R636 (list) c_action_dealloc_opt_list() count = %d \n",count);
   }

/** R637-F2008 list
 * allocate_co_shape_spec_list
 *      :       allocate_co_shape_spec ( T_COMMA allocate_co_shape_spec )*
 * 
 * @param count The number of items in the list.
 */
void c_action_allocate_co_shape_spec_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R637 (list) c_action_allocate_co_shape_spec_list__begin() \n");
   }
void c_action_allocate_co_shape_spec_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R637 (list) c_action_allocate_co_shape_spec_list() count = %d \n",count);
   }

/** R701
 * primary
 *      :       designator_or_func_ref
 *      |       literal_constant
 *      |       array_constructor
 *      |       structure_constructor
 *      |       T_LPAREN expr T_RPAREN
 *
 */
void c_action_primary()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_primary() \n");
   }

/** R702
 * level_1_expr
 *  : (defined_unary_op)? primary
 */
void c_action_level_1_expr(Token_t * definedUnaryOp)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_level_1_expr(): definedUnaryOp = %p = %s \n",definedUnaryOp,definedUnaryOp != NULL ? definedUnaryOp->text : "NULL");

     if (definedUnaryOp != NULL)
        {
#if 0
       // Output debugging information about saved state (stack) information.
          outputState("At TOP of R702 c_action_level_1_expr()");
#endif

          ROSE_ASSERT(astExpressionStack.empty() == false);
          SgExpression* tmp = astExpressionStack.front();
          astExpressionStack.pop_front();

          SgExpression* result = createUnaryOperator(tmp,definedUnaryOp->text, /* is_user_defined_operator */ true);
          setSourcePosition(result,definedUnaryOp);

          astExpressionStack.push_front(result);

       // printf ("Exiting after building user defined unary operator... \n");
       // ROSE_ASSERT(false);
        }

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R702 c_action_level_1_expr()");
#endif
   }

/**
 * R703
 * defined_unary_op
 *
 * @param definedOp T_DEFINED_OP token.
 */
void c_action_defined_unary_op(Token_t * definedOp)
   {
  // This action is called with the parser sees the unary operator, but the operand for 
  // the unary operator has not been processed yet (will be done in R702).  So this is
  // too early a point to process the unary operator!

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_defined_unary_op(): definedOp = %p \n",definedOp);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R703 c_action_defined_unary_op()");
#endif
   }


/** R704: note, inserted as R704 functionality
 * power_operand
 *      : level_1_expr (power_op power_operand)?
 */
void c_action_power_operand(ofp_bool hasPowerOperand)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_power_operand(): hasPowerOperand = %s \n",hasPowerOperand ? "true" : "false");

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R704 c_action_power_operand()");
#endif

#if !SKIP_C_ACTION_IMPLEMENTATION
  // This appears to be called a number of times, but we only want to process entries 
  // on the stack to generate a SgExponentiationOp when hasPowerOperand == true.
     if (hasPowerOperand == true)
        {
          ROSE_ASSERT(astExpressionStack.empty() == false);
          SgExpression* rhs = astExpressionStack.front();
          astExpressionStack.pop_front();

          ROSE_ASSERT(astExpressionStack.empty() == false);
          SgExpression* lhs = astExpressionStack.front();
          astExpressionStack.pop_front();

       // Note that the type provided is NULL, since ROSE will internally (dynamically) evaluate the type 
       // as required.  This avoids state in the AST and better supports transformations on the AST.
          SgBinaryOp* expr = new SgExponentiationOp(lhs,rhs,NULL);

          setSourcePosition(expr);

       // DQ (1/23/2008): Set the position based on the sub expressions.
          resetSourcePosition(expr,lhs);

          ROSE_ASSERT(astNameStack.empty() == false);
          Token_t* powerKeyword = astNameStack.front();
          astNameStack.pop_front();

          ROSE_ASSERT(powerKeyword != NULL);
          setOperatorSourcePosition(expr,powerKeyword);

       // Save the expression on the stack
          astExpressionStack.push_front(expr);
        }
#endif

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R704 c_action_power_operand()");
#endif
   }

void c_action_power_operand__power_op(Token_t * powerOp)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_power_operand__power_op(): powerOp = %p \n",powerOp);
   }

/** R704: note, see power_operand
 * mult_operand
 *  : power_operand (mult_op power_operand)*
 *  
 *  @param numMults The number of optional mult_ops
 */
void c_action_mult_operand(int numMultOps)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_mult_operand(): numMultOps = %d \n",numMultOps);

     ROSE_ASSERT(numMultOps >= 0);
   }

void c_action_mult_operand__mult_op(Token_t * multOp)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_mult_operand__mult_op(): multOp = %p \n",multOp);

#if !SKIP_C_ACTION_IMPLEMENTATION
     ROSE_ASSERT(astExpressionStack.empty() == false);
     SgExpression* rhs = astExpressionStack.front();
     astExpressionStack.pop_front();

     ROSE_ASSERT(astExpressionStack.empty() == false);
     SgExpression* lhs = astExpressionStack.front();
     astExpressionStack.pop_front();

  // Note that the type provided is NULL, since ROSE will internally (dynamically) evaluate the type 
  // as required.  This avoids state in the AST and better supports transformations on the AST.

     SgBinaryOp* expr = NULL;
     if (multOp->text[0] == '*')
        {
          expr = new SgMultiplyOp(lhs,rhs,NULL);
        }
       else
        {
          if (multOp->text[0] == '/')
             {
               expr = new SgDivideOp(lhs,rhs,NULL);
             }
            else
             {
               printf ("Error: multOp->text not + or - multOp->text = %s \n",multOp->text);
               ROSE_ASSERT(false);
             }
        }

     setSourcePosition(expr,multOp);

     ROSE_ASSERT(multOp != NULL);
     setOperatorSourcePosition(expr,multOp);

  // Save the expression on the stack
     astExpressionStack.push_front(expr);
#endif
   }

/** R705: note, moved leading optionals to mult_operand
 * add_operand
 *  : (add_op)? mult_operand (add_op mult_operand)*
 *  
 * @param addOp Optional add_op for this operand
 * @param numAddOps The number of optional add_ops
 */
#if ROSE_OFP_MINOR_VERSION_NUMBER >= 8 & ROSE_OFP_PATCH_VERSION_NUMBER >= 2
void c_action_add_operand(int numAddOps)
#else
void c_action_add_operand(Token_t * addOp, int numAddOps)
#endif
   {
  // I am unclear about the purpose of this rule!

  // DQ (9/26/2010): Added support for OFP version 0.8.2
#if ROSE_OFP_MINOR_VERSION_NUMBER >= 8 & ROSE_OFP_PATCH_VERSION_NUMBER >= 2
  // printf ("WARNING: In OFP versions 0.8.2 and greater it is not clear what to do in c_action_add_operand() \n");

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_add_operand(): numAddOps = %d \n",numAddOps);
#else
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_add_operand(): addOp = %p = %s numAddOps = %d \n",addOp,addOp != NULL ? addOp->text : "NULL",numAddOps);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R705 c_action_add_operand()");
#endif

     if (addOp != NULL)
        {
       // printf ("In c_action_add_operand(): this might be a unary minus! \n");

          ROSE_ASSERT(astExpressionStack.empty() == false);
          SgExpression* operand = astExpressionStack.front();
          astExpressionStack.pop_front();

          ROSE_ASSERT(addOp != NULL);
       // printf ("addOp->text[0] = %c \n",addOp->text[0]);

          SgUnaryOp* expr = NULL;
          if (addOp->text[0] == '+')
             {
               expr = new SgUnaryAddOp(operand,NULL);
             }
            else
             {
               if (addOp->text[0] == '-')
                  {
                    expr = new SgMinusOp(operand,NULL);
                  }
                 else
                  {
                    printf ("Error: addOp->text not + or - addOp->text = %s \n",addOp->text);
                    ROSE_ASSERT(false);
                  }
             }

          ROSE_ASSERT(expr != NULL);
          setSourcePosition(expr,addOp);

          ROSE_ASSERT(addOp != NULL);
          setOperatorSourcePosition(expr,addOp);

       // Save the expression on the stack
          astExpressionStack.push_front(expr);
        }
#endif

  // ROSE_ASSERT(addOp != NULL);
   }

/** R705 addition:
 * This rule has been added so the unary plus/minus has the correct
 * precedence when actions are fired.
 *   :   (add_op)? mult_operand
 *  
 * @param addOp Optinoal add_op for this operand
 */
#if ROSE_OFP_MINOR_VERSION_NUMBER >= 8 & ROSE_OFP_PATCH_VERSION_NUMBER >= 2
void c_action_signed_operand(Token_t *addOp)
   {
  // DQ (9/26/2010): Moved the support for selecting unary +/- to fix Rice fortran bug (test2010_??.f90).

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_signed_operand(): addOp = %p = %s \n",addOp,addOp != NULL ? addOp->text : "NULL");

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R705 c_action_signed_operand()");
#endif

     if (addOp != NULL)
        {
       // printf ("In c_action_signed_operand(): this might be a unary minus! \n");

          ROSE_ASSERT(astExpressionStack.empty() == false);
          SgExpression* operand = astExpressionStack.front();
          astExpressionStack.pop_front();

          ROSE_ASSERT(addOp != NULL);
       // printf ("addOp->text[0] = %c \n",addOp->text[0]);

          SgUnaryOp* expr = NULL;
          if (addOp->text[0] == '+')
             {
               expr = new SgUnaryAddOp(operand,NULL);
             }
            else
             {
               if (addOp->text[0] == '-')
                  {
                    expr = new SgMinusOp(operand,NULL);
                  }
                 else
                  {
                    printf ("Error: addOp->text not + or - addOp->text = %s \n",addOp->text);
                    ROSE_ASSERT(false);
                  }
             }

          ROSE_ASSERT(expr != NULL);
          setSourcePosition(expr,addOp);

          ROSE_ASSERT(addOp != NULL);
          setOperatorSourcePosition(expr,addOp);

       // Save the expression on the stack
          astExpressionStack.push_front(expr);
        }

  // printf ("ERROR: Unimplemented OFP 0.8.2 function \n");
  // ROSE_ASSERT(false);
   }
#endif

void c_action_add_operand__add_op(Token_t * addOp)
   {
  // Unclear if I should be using c_action_add_operand__add_op or c_action_add_operand.
  // However, since we want to set the source position of the addOp in the generated 
  // expression I have elected to build the expression here.
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_add_operand__add_op():addOp->text = %s \n",addOp->text);

#if !SKIP_C_ACTION_IMPLEMENTATION
     ROSE_ASSERT(astExpressionStack.empty() == false);
     SgExpression* rhs = astExpressionStack.front();
     astExpressionStack.pop_front();

     ROSE_ASSERT(astExpressionStack.empty() == false);
     SgExpression* lhs = astExpressionStack.front();
     astExpressionStack.pop_front();

  // Note that the type provided is NULL, since ROSE will internally (dynamically) evaluate the type 
  // as required.  This avoids state in the AST and better supports transformations on the AST.

     ROSE_ASSERT(addOp != NULL);
  // printf ("addOp->text[0] = %c \n",addOp->text[0]);

     SgBinaryOp* expr = NULL;
     if (addOp->text[0] == '+')
        {
          expr = new SgAddOp(lhs,rhs,NULL);
        }
       else
        {
          if (addOp->text[0] == '-')
             {
               expr = new SgSubtractOp(lhs,rhs,NULL);
             }
            else
             {
               printf ("Error: addOp->text not + or - addOp->text = %s \n",addOp->text);
               ROSE_ASSERT(false);
             }
        }

     ROSE_ASSERT(expr != NULL);
  // setSourcePosition(expr);
     setSourcePosition(expr,addOp);

     ROSE_ASSERT(addOp != NULL);
     setOperatorSourcePosition(expr,addOp);

  // Save the expression on the stack
     astExpressionStack.push_front(expr);
#endif
   }

/** R706: note, moved leading optionals to add_operand
 * level_2_expr
 *  : add_operand (concat_op add_operand)*
 *  
 *  @param numConcatOps The number of optional numConcatOps
 */
void c_action_level_2_expr(int numConcatOps)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_level_2_expr(): numConcatOps = %d \n",numConcatOps);
#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R706 c_action_level_2_expr()");
#endif

#if !SKIP_C_ACTION_IMPLEMENTATION
     if (numConcatOps > 0)
        {
          Token_t* relOp = astNameStack.front();
          astNameStack.pop_front();
          ROSE_ASSERT(relOp != NULL);

          ROSE_ASSERT(relOp->text != NULL);
       // printf ("relOp->text = %s \n",relOp->text);

          SgExpression* result = NULL;

          ROSE_ASSERT(astExpressionStack.empty() == false);
          SgExpression* rhs = astExpressionStack.front();
          astExpressionStack.pop_front();

          ROSE_ASSERT(astExpressionStack.empty() == false);
          SgExpression* lhs = astExpressionStack.front();
          astExpressionStack.pop_front();

          ROSE_ASSERT(relOp != NULL);

          result = createBinaryOperator(lhs,rhs,relOp->text, /* is_user_defined_operator */ false);

          ROSE_ASSERT(result != NULL);
          setSourcePosition(result,relOp);

          SgExpression* accumulator_rhs = result;
          for (int i=0; i < numConcatOps-1; i++)
             {
               ROSE_ASSERT(astExpressionStack.empty() == false);
               lhs = astExpressionStack.front();
               astExpressionStack.pop_front();

               ROSE_ASSERT(astNameStack.empty() == false);
               relOp = astNameStack.front();
               astNameStack.pop_front();
               ROSE_ASSERT(relOp != NULL);

               result = createBinaryOperator(lhs,accumulator_rhs,relOp->text, /* is_user_defined_operator */ false);
               setSourcePosition(result,relOp);

            // DQ (8/5/2010): Bug fix suggested by Matt.
               accumulator_rhs = result;
             }

          astExpressionStack.push_front(result);
        }
#endif

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R706 c_action_level_2_expr()");
#endif
#if 0
     if (numConcatOps > 0)
        {
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
        }
#endif
   }

/**
 * R707
 * power_op
 *
 * @param powerKeyword T_POWER token.
 */
void c_action_power_op(Token_t * powerKeyword)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_power_op(): powerKeyword = %s \n",powerKeyword != NULL ? powerKeyword->text : "NULL");

     ROSE_ASSERT(powerKeyword != NULL);
     astNameStack.push_front(powerKeyword);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R707 c_action_power_op()");
#endif
   }

/**
 * R708
 * mult_op
 *
 * @param multKeyword T_ASTERISK or T_SLASH token.
 */
void c_action_mult_op(Token_t * multKeyword)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_mult_op(): multKeyword = %s \n",multKeyword != NULL ? multKeyword->text : "NULL");
   }

/**
 * R709
 * add_op
 *
 * @param addKeyword T_PLUS or T_MINUS token.
 */
void c_action_add_op(Token_t * addKeyword)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_add_op(): addKeyword = %s \n",addKeyword != NULL ? addKeyword->text : "NULL");

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R709 c_action_add_op()");
#endif
   }

/** R710: note, moved leading optional to level_2_expr
 * level_3_expr
 *  : level_2_expr (rel_op level_2_expr)?
 *  
 *  @param relOp The rel-op, if present, null otherwise
 */
void c_action_level_3_expr(Token_t * relOp)
   {
  // Relational operators
  // printf ("relOp = %s \n",relOp->text);

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_level_3_expr(): relOp = %p = %s \n",relOp,relOp != NULL ? relOp->text : "NULL");

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R710 c_action_level_3_expr()");
#endif

#if !SKIP_C_ACTION_IMPLEMENTATION
     if (relOp != NULL)
        {
          ROSE_ASSERT(relOp->text != NULL);
       // printf ("In c_action_level_3_expr(): relOp->text = %s \n",relOp->text);

          SgExpression* result = NULL;

          ROSE_ASSERT(astExpressionStack.empty() == false);
          SgExpression* rhs = astExpressionStack.front();
          astExpressionStack.pop_front();

          ROSE_ASSERT(astExpressionStack.empty() == false);
          SgExpression* lhs = astExpressionStack.front();
          astExpressionStack.pop_front();

          ROSE_ASSERT(relOp != NULL);

          result = createBinaryOperator(lhs,rhs,relOp->text, /* is_user_defined_operator */ false);

          ROSE_ASSERT(result != NULL);
          setSourcePosition(result,relOp);

          astExpressionStack.push_front(result);

       // DQ (10/7/2008): There should uniformally be a toke on the stack for the ".<operator>." name.
          ROSE_ASSERT(astNameStack.empty() == false);
          astNameStack.pop_front();
        }
#endif

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R710 c_action_level_3_expr()");
#endif
   }

/**
 * R711
 * concat_op
 *
 * @param concatKeyword T_SLASH_SLASH token.
 */
void c_action_concat_op(Token_t * concatKeyword)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_concat_op(): concatKeyword = %s \n",concatKeyword != NULL ? concatKeyword->text : "NULL");

     ROSE_ASSERT(concatKeyword != NULL);
     astNameStack.push_front(concatKeyword);
#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R711 c_action_concat_op()");
#endif
   }

/**
 * R713
 * rel_op
 *
 * @param relOp Relational operator token.
 */
void c_action_rel_op(Token_t * relOp)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_rel_op(): relOp = %s \n",relOp != NULL ? relOp->text : "NULL");

  // DQ (10/7/2008): Added support for relational operators to be handled symetricly with other operators.
     ROSE_ASSERT(relOp != NULL);
     astNameStack.push_front(relOp);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R713 c_action_rel_op()");
#endif
   }


/** R714
 * and_operand
 *  :   (not_op)? level_3_expr (and_op level_3_expr)*
 *
 * @param hasNotOp True if optional not_op is present
 * @param numAndOps The number of optional and_ops
 */
void c_action_and_operand(ofp_bool hasNotOp, int numAndOps)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_and_operand(): hasNotOp = %s numAndOps = %d \n",hasNotOp ? "true" : "false",numAndOps);

#if !SKIP_C_ACTION_IMPLEMENTATION

     bool processRelationalOperator = hasNotOp || (numAndOps > 0);
  // bool processRelationalOperator = (numAndOps > 0);
  // if (astNameStack.empty() == false)
     if (processRelationalOperator == true)
        {
          ROSE_ASSERT(astNameStack.empty() == false);
          Token_t* relOp = astNameStack.front();
       // astNameStack.pop_front();
          ROSE_ASSERT(relOp != NULL);

          ROSE_ASSERT(relOp->text != NULL);
       // printf ("relOp->text = %s \n",relOp->text);

       // ROSE_ASSERT( (hasNotOp == true && numAndOps == 0) || (hasNotOp == false && numAndOps > 0) );

       // Support for chained relational operators

       // This will be right-associative instead of left-associative 
       // which is required, but I will fix that afterward.

          ROSE_ASSERT(astExpressionStack.empty() == false);
          SgExpression* rhs = astExpressionStack.front();
          astExpressionStack.pop_front();

          SgExpression* tempExpression = rhs;
          SgExpression* result = NULL;
          for (int i=0; i < numAndOps; i++)
             {
            // Then this is a BINARY operator (.AND.)
               relOp = astNameStack.front();
               astNameStack.pop_front();
            // printf ("relOp->text = %s (should be .AND.)\n",relOp->text);

               ROSE_ASSERT(astExpressionStack.empty() == false);
               SgExpression* lhs = astExpressionStack.front();
               astExpressionStack.pop_front();

            // This applies only to the first operand (which is a special case, and last on the stack since it was pushed first)
               if (i == numAndOps-1 && hasNotOp == true)
                  {
                 // Then this is a UNARY operator (.NOT.)
                    Token_t* local_relOp  = astNameStack.front();
                 // printf ("local_relOp->text = %s (should be .NOT.)\n",local_relOp->text);
                    ROSE_ASSERT(local_relOp != NULL);
                    lhs = createUnaryOperator(lhs,local_relOp->text, /* is_user_defined_operator */ false);
                    setSourcePosition(lhs,local_relOp);

                    astNameStack.pop_front();
                  }

               ROSE_ASSERT(relOp != NULL);
               result = createBinaryOperator(lhs,tempExpression,relOp->text, /* is_user_defined_operator */ false);
               setSourcePosition(result,relOp);


               tempExpression = result;
             }

       // This applies only to the first operand (which is a special case, and last on the stack since it was pushed first)
          if (numAndOps == 0 && hasNotOp == true)
             {
            // Then this is a UNARY operator (.NOT.)
               ROSE_ASSERT(relOp != NULL);
            // printf ("relOp->text = %s (should be .NOT.)\n",relOp->text);
               result = createUnaryOperator(rhs,relOp->text, /* is_user_defined_operator */ false);
               setSourcePosition(result,relOp);

               astNameStack.pop_front();
             }

          ROSE_ASSERT(result != NULL);
          astExpressionStack.push_front(result);
        }
#endif
#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R714 c_action_and_operand()");
#endif
   }

void c_action_and_operand__not_op(ofp_bool hasNotOp)
   {
  // This rule applies only to each of child operands except the first one in a chain (of ".and." operators).

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_and_operand__not_op(): hasNotOp = %s \n",hasNotOp ? "true" : "false");

     if (hasNotOp == true)
        {
       // Then this is a UNARY operator (.NOT. or !)
           ROSE_ASSERT(astNameStack.empty() == false);
          Token_t* local_relOp = astNameStack.front();
          ROSE_ASSERT(local_relOp != NULL);
          astNameStack.pop_front();
       // printf ("c_action_and_operand__not_op() (hasNotOp == true): local_relOp->text = %s \n",local_relOp->text);

          ROSE_ASSERT(astExpressionStack.empty() == false);
          SgExpression* tmp = astExpressionStack.front();
          astExpressionStack.pop_front();

          SgExpression* result = createUnaryOperator(tmp,local_relOp->text, /* is_user_defined_operator */ false);
          setSourcePosition(result,local_relOp);

          astExpressionStack.push_front(result);
        }
#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R714 c_action_and_operand__not_op()");
#endif
   }

/** R715: note, moved leading optional to or_operand
 * or_operand
 *      : and_operand (or_op and_operand)*
 *
 * @param numOrOps The number of optional or_ops
 */
void c_action_or_operand(int numOrOps)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_or_operand(): numOrOps = %d \n",numOrOps);
#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R715 c_action_or_operand()");
#endif
     if (numOrOps > 0)
        {
          ROSE_ASSERT(astNameStack.empty() == false);
          Token_t* relOp = astNameStack.front();
          ROSE_ASSERT(relOp != NULL);

          ROSE_ASSERT(relOp->text != NULL);
       // printf ("relOp->text = %s \n",relOp->text);

          ROSE_ASSERT(astExpressionStack.empty() == false);
          SgExpression* rhs = astExpressionStack.front();
          astExpressionStack.pop_front();

          SgExpression* result = NULL;

       // Support for chained relational operators
          SgExpression* tempExpression = rhs;
          for (int i=0; i < numOrOps; i++)
             {
            // Then this is a BINARY operator (.OR.)
               relOp = astNameStack.front();

               ROSE_ASSERT(astExpressionStack.empty() == false);
               SgExpression* lhs = astExpressionStack.front();
               astExpressionStack.pop_front();

               ROSE_ASSERT(relOp != NULL);
               result = createBinaryOperator(lhs,tempExpression,relOp->text, /* is_user_defined_operator */ false);
               setSourcePosition(result,relOp);

               astNameStack.pop_front();

               tempExpression = result;
             }

          ROSE_ASSERT(result != NULL);
          astExpressionStack.push_front(result);
        }
#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R715 c_action_or_operand()");
#endif
   }

/** R716: note, moved leading optional to or_operand
 * equiv_operand
 *  : or_operand (equiv_op or_operand)*
 *  
 * @param numEquivOps The number of optional or_operands
 * @param equivOp Optional equiv_op for this operand
 */
void c_action_equiv_operand(int numEquivOps)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_equiv_operand(): numEquivOps = %d \n",numEquivOps);
   }

void c_action_equiv_operand__equiv_op(Token_t * equivOp)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_equiv_operand__equiv_op(): equivOp = %p \n",equivOp);

#if !SKIP_C_ACTION_IMPLEMENTATION
     if (equivOp != NULL)
        {
          ROSE_ASSERT(equivOp->text != NULL);
       // printf ("In c_action_equiv_operand__equiv_op() (level 5 expr): equivOp->text = %s \n",equivOp->text);

          SgExpression* result = NULL;

          ROSE_ASSERT(astExpressionStack.empty() == false);
          SgExpression* rhs = astExpressionStack.front();
          astExpressionStack.pop_front();

          ROSE_ASSERT(astExpressionStack.empty() == false);
          SgExpression* lhs = astExpressionStack.front();
          astExpressionStack.pop_front();

          ROSE_ASSERT(equivOp != NULL);

          result = createBinaryOperator(lhs,rhs,equivOp->text, /* is_user_defined_operator */ false);

          ROSE_ASSERT(result != NULL);
          setSourcePosition(result,equivOp);

          astExpressionStack.push_front(result);

       // DQ (10/7/2008): There should uniformally be a toke on the stack for the ".<operator>." name.
          ROSE_ASSERT(astNameStack.empty() == false);
          astNameStack.pop_front();
        }
#endif

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R716 c_action_level_3_expr()");
#endif
   }

/** R717: note, moved leading optional to equiv_operand
 * level_5_expr
 *  : equiv_operand (defined_binary_op equiv_operand)*
 *  
 * @param numDefinedBinaryOps The number of optional equiv_operands
 * @param definedBinaryOp Optional defined_binary_op for this operand
 */
void c_action_level_5_expr(int numDefinedBinaryOps)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_level_5_expr(): numDefinedBinaryOps = %d \n",numDefinedBinaryOps);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R717 c_action_level_5_expr()");
#endif
   }

void c_action_level_5_expr__defined_binary_op(Token_t * definedBinaryOp)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_level_5_expr__defined_binary_op(): definedBinaryOp = %p = %s \n",definedBinaryOp,definedBinaryOp != NULL ? definedBinaryOp->text : "NULL");

#if !SKIP_C_ACTION_IMPLEMENTATION
     if (definedBinaryOp != NULL)
        {
          ROSE_ASSERT(definedBinaryOp->text != NULL);
       // printf ("In c_action_level_3_expr(): relOp->text = %s \n",relOp->text);

          SgExpression* result = NULL;

          ROSE_ASSERT(astExpressionStack.empty() == false);
          SgExpression* rhs = astExpressionStack.front();
          astExpressionStack.pop_front();

          ROSE_ASSERT(astExpressionStack.empty() == false);
          SgExpression* lhs = astExpressionStack.front();
          astExpressionStack.pop_front();

          result = createBinaryOperator(lhs,rhs,definedBinaryOp->text, /* is_user_defined_operator */ true);

          ROSE_ASSERT(result != NULL);
          setSourcePosition(result,definedBinaryOp);

          astExpressionStack.push_front(result);

       // DQ (10/7/2008): There should uniformally be a token on the stack for the ".<operator>." name.
       // ROSE_ASSERT(astNameStack.empty() == false);
       // astNameStack.pop_front();

          outputState("Before EXIT of R717 c_action_level_5_expr__defined_binary_op()");

       // printf ("Exiting after building the user-defined binary operator... \n");
       // ROSE_ASSERT(false);
        }
#endif

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R717 c_action_level_5_expr__defined_binary_op()");
#endif
   }

/**
 * R718
 * not_op
 *
 * @param notOp T_NOT token.
 */
void c_action_not_op(Token_t *notOp)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_not_op(): notOp = %p = %s \n",notOp,notOp != NULL ? notOp->text : "NULL");

     ROSE_ASSERT(notOp != NULL);
     astNameStack.push_front(notOp);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R718 c_action_not_op()");
#endif
   }

/**
 * R719
 * and_op
 *
 * @param andOp T_AND token.
 */
void c_action_and_op(Token_t *andOp)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_and_op(): andOp = %p = %s \n",andOp,andOp != NULL ? andOp->text : "NULL");

     ROSE_ASSERT(andOp != NULL);
     astNameStack.push_front(andOp);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R719 c_action_and_op()");
#endif
   }

/**
 * R720
 * or_op
 *
 * @param orOp T_OR token.
 */
void c_action_or_op(Token_t *orOp)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_or_op(): orOp = %p = %s \n",orOp,orOp != NULL ? orOp->text : "NULL");

     ROSE_ASSERT(orOp != NULL);
     astNameStack.push_front(orOp);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R720 c_action_or_op()");
#endif
   }

/**
 * R721
 * equiv_op
 *
 * @param equivOp T_EQV or T_NEQV token.
 */
void c_action_equiv_op(Token_t *equivOp)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_equiv_op(): equivOp = %p = %s \n",equivOp,equivOp != NULL ? equivOp->text : "NULL");

     ROSE_ASSERT(equivOp != NULL);
     astNameStack.push_front(equivOp);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R721 c_action_equiv_op()");
#endif
   }

/** R722: note, moved leading optional to level_5_expr
 * expr
 *  : level_5_expr
 */
void c_action_expr()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R722 c_action_expr() \n");

  // DQ (12/14/2007): This should have been set by now! See test2007_114.f03
  // build_implicit_program_statement_if_required();

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R722 c_action_expr()");
#endif
   }

/**
 * R723
 * defined_binary_op
 *
 * @param binaryOp T_DEFINED_OP token.
 */
void c_action_defined_binary_op(Token_t *binaryOp)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_defined_binary_op(): binaryOp = %p = %s \n",binaryOp,binaryOp != NULL ? binaryOp->text : "NULL");
   }


/** R734
 *      assignment_stmt 
 *      :       (label)? T_ASSIGNMENT_STMT variable     T_EQUALS expr T_EOS
 *
 * @param label Optional statement label
 */
// void c_action_assignment_stmt(Token_t * label)
void c_action_assignment_stmt(Token_t *label, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_assignment_stmt(): label = %p = %s \n",label,label ? label->text : "NULL");

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R734 c_action_assignment_stmt()");
#endif

#if !SKIP_C_ACTION_IMPLEMENTATION
  // DQ (1/31/2009): Moved this to the TOP of the function.
  // Refactored the code to build support function
     initialize_global_scope_if_required();
     build_implicit_program_statement_if_required();

  // DQ (1/31/2009): Refactored code to support us in R735 pointer assignment statement.
     bool isPointerAssignment = false;
     generateAssignmentStatement(label,isPointerAssignment);
#endif
   }

/** R735
 * pointer_assignment_stmt
 *
 * : (label)? T_PTR_ASSIGNMENT_STMT data_ref T_EQ_GT expr T_EOS
 * | (label)? T_PTR_ASSIGNMENT_STMT data_ref T_LPAREN bounds_spec_list 
 *              T_RPAREN T_EQ_GT expr T_EOS
 * | (label)? T_PTR_ASSIGNMENT_STMT data_ref T_LPAREN 
 *              bounds_remapping_list T_RPAREN T_EQ_GT expr T_EOS
 *
 * @param label The label.
 * @param hasBoundsSpecList True if has a bounds spec list.
 * @param hasBoundsRemappingList True if has a bounds remapping list.
 */
// void c_action_pointer_assignment_stmt(Token_t * label, ofp_bool hasBoundsSpecList, ofp_bool hasBoundsRemappingList)
void c_action_pointer_assignment_stmt(Token_t *label, Token_t *eos, ofp_bool hasBoundsSpecList, ofp_bool hasBRList)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R735 c_action_pointer_assignment_stmt() label = %p hasBoundsSpecList = %s hasBRList = %s \n",label,hasBoundsSpecList ? "true" : "false",hasBRList ? "true" : "false");


  // We map the pointer assignment operator "=>" to the regular assignment expression.
  // But in the unparser we detect that the l-value is a pointer type and use "=>" in the
  // generated code instead of "=".

  // DQ (1/31/2009): Refactored code to support us in R735 pointer assignment statement.
     bool isPointerAssignment = true;
     generateAssignmentStatement(label,isPointerAssignment);

  // DQ (1/31/2009): I need an example of this code before I can handle these cases.
     ROSE_ASSERT(hasBoundsSpecList == false);
     ROSE_ASSERT(hasBRList == false);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R735 c_action_pointer_assignment_stmt()");
#endif
   }

/** R737 list
 * bounds_spec_list
 *      :       bounds_spec ( T_COMMA bounds_spec )*
 * 
 * @param count The number of items in the list.
 */
void c_action_bounds_spec_list__begin()
{
}
void c_action_bounds_spec_list(int count)
{
}

/** R738 list
 * bounds_remapping_list
 *      :       bounds_remapping ( T_COMMA bounds_remapping )*
 * 
 * @param count The number of items in the list.
 */
void c_action_bounds_remapping_list__begin()
{
}
void c_action_bounds_remapping_list(int count)
{
}

/** R743 
 * where_stmt
 *
 *      (label {lbl=$label.tk;})? T_WHERE_STMT T_WHERE
 *              T_LPAREN expr T_RPAREN assignment_stmt
 *
 * @param label The label
 */
void c_action_where_stmt__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_where_stmt__begin() \n");

  // DQ (10/2/2007): Using the new c_action_where_stmt__begin()
     SgBasicBlock* body  = new SgBasicBlock();
     ROSE_ASSERT(body != NULL);

  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     body->setCaseInsensitive(true);

     SgWhereStatement* whereStatement = new SgWhereStatement(NULL,body,NULL);

  // DQ (1/23/2008): This will be set later when we have more information about its source position.
  // setSourcePosition(whereStatement);

     body->set_parent(whereStatement);
     setSourcePosition(body);

     ROSE_ASSERT(astScopeStack.empty() == false);
     astScopeStack.front()->append_statement(whereStatement);

     astScopeStack.push_front(body);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R743 c_action_where_stmt__begin()");
#endif
   }

// void c_action_where_stmt(Token_t * label)
void c_action_where_stmt(Token_t *label, Token_t *whereKeyword)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R743 c_action_where_stmt() label = %p \n",label);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R743 c_action_where_stmt()");
#endif

  // This is the case of: "where (a) b = 0"

  // SgExpression* condition = NULL;
     ROSE_ASSERT(astExpressionStack.empty() == false);
     SgExpression* condition = astExpressionStack.front();
     astExpressionStack.pop_front();

  // SgBasicBlock* body  = new SgBasicBlock();
     ROSE_ASSERT(astScopeStack.empty() == false);
     SgBasicBlock* body  = isSgBasicBlock(astScopeStack.front());
     ROSE_ASSERT(body != NULL);

  // DQ (10/10/2010): Test ending position
     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct() != NULL);

     SgStatement* lastStatement = astScopeStack.front()->lastStatement();
  // printf ("In c_action_where_stmt(): lastStatement = %p \n",lastStatement);
     ROSE_ASSERT(lastStatement != NULL);
     resetEndingSourcePosition(astScopeStack.front(),lastStatement);

     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct()->get_line() != astScopeStack.front()->get_startOfConstruct()->get_line());

  // DQ (11/15/2007): Pop the SgBasicBlock that was previously pushed on the stack for the body!
     astScopeStack.pop_front();

     SgWhereStatement* whereStatement = isSgWhereStatement(body->get_parent());
     ROSE_ASSERT(whereStatement != NULL);

     ROSE_ASSERT(whereKeyword != NULL);
     setSourcePosition(whereStatement,whereKeyword);

  // DQ (11/17/2007): Added support for numeric labels
     setStatementNumericLabel(whereStatement,label);

     whereStatement->set_condition(condition);
     condition->set_parent(whereStatement);

     SgScopeStatement* currentScope  = astScopeStack.front();
     SgBasicBlock* currentBlockScope = isSgBasicBlock(currentScope);

  // printf ("currentBlockScope = %p whereStatement = %p body = %p \n",currentBlockScope,whereStatement,body);
     ROSE_ASSERT(currentBlockScope != NULL);

  // DQ (11/15/2007): Check if the whereStatement has already been added previously!
  // ROSE_ASSERT(find(currentBlockScope->get_statements().begin(),currentBlockScope->get_statements().end(),whereStatement) == currentBlockScope->get_statements().end());
  // currentBlockScope->append_statement(whereStatement);
     ROSE_ASSERT(find(currentBlockScope->get_statements().begin(),currentBlockScope->get_statements().end(),whereStatement) != currentBlockScope->get_statements().end());

     ROSE_ASSERT(body != currentBlockScope);

  // astScopeStack.push_front(body);

  // Add any saved statements on the stack to the body (there should only be a single statement, I think).
     ROSE_ASSERT(astNodeStack.empty() == false);
     SgStatement* statement = isSgStatement(astNodeStack.front());
     ROSE_ASSERT(statement != NULL);

  // The c_action_where_stmt() is called when we don't have an end where statement
     whereStatement->set_has_end_statement(false);

  // DQ (12/26/2007): Strip the label from any statement used in a where statement (the label belongs to the where statement itself)
     if (statement->get_numeric_label() != NULL)
        {
       // printf ("Warning: Strip the label from any statement used in a where statement \n");
          statement->set_numeric_label(NULL);
        }

     ROSE_ASSERT(find(body->get_statements().begin(),body->get_statements().end(),statement) != body->get_statements().end());
  // body->append_statement(statement);
     astNodeStack.pop_front();

  // DQ (11/15/2007): also pop the SgBasicBlock that was pushed on the stack!
  // astScopeStack.pop_front();
  // ROSE_ASSERT(astScopeStack.empty() == false);
  // astScopeStack.pop_front();

  // DQ (11/15/2007): At this point we should have the body of the function or the SgBasicBlock of another scope which can has statements.
     ROSE_ASSERT(astScopeStack.empty() == false);
     ROSE_ASSERT(isSgBasicBlock(astScopeStack.front()) != NULL);

     setStatementNumericLabelUsingStack(whereStatement);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R743 c_action_where_stmt()");
#endif
   }

/** R744 
 * where_construct
 *
 *      : where_construct_stmt ( where_body_construct )*
 *              ( masked_elsewhere_stmt ( where_body_construct )* )*
 *              ( elsewhere_stmt ( where_body_construct {numElsewhereConstructs += 1;} )* )?
 *        end_where_stmt
 *
 * @param numConstructs The number of where-body-constructs.
 * @param hasMaskedElsewhere True if where-construct has a masked-elsewhere-stmt.
 * @param hasElsewhere True if where-construct has an elsewhere-stmt.
 */
void c_action_where_construct(int numConstructs, ofp_bool hasMaskedElsewhere, ofp_bool hasElsewhere)
   {
  // This is called after the end where statement
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R744 c_action_where_construct() numConstructs = %d hasMaskedElsewhere = %s hasElsewhere = %s \n",
               numConstructs, hasMaskedElsewhere ? "true" : "false", hasElsewhere ? "true" : "false");

  // ROSE_ASSERT(hasElsewhere == false);
  // ROSE_ASSERT(hasMaskedElsewhere == false); 
           
#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R744 c_action_where_construct()");
#endif
   }

/** R745
 * where_construct_stmt
 *      :       (T_IDENT T_COLON)? T_WHERE_CONSTRUCT_STMT T_WHERE T_LPAREN expr T_RPAREN T_EOS
 * 
 * @param id Optional name for the loop. If you use this up front, you have to use it in the end, and vice versa.
 */
// void c_action_where_construct_stmt(Token_t * id)
void c_action_where_construct_stmt(Token_t *id, Token_t *whereKeyword, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R745 c_action_where_construct_stmt() id = %p = %s \n",id,id ? id->text : "NULL");

  // SgExpression* condition  = NULL;
  // SgExpression* condition  = new SgNullExpression();
     ROSE_ASSERT(astExpressionStack.empty() == false);
     SgExpression* condition = astExpressionStack.front();
     astExpressionStack.pop_front();

     SgBasicBlock* body = new SgBasicBlock();
  // SgBasicBlock* falseBlock = new SgBasicBlock();

  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     body->setCaseInsensitive(true);

     SgWhereStatement* whereStatement = new SgWhereStatement(condition,body,NULL);

  // setSourcePosition(whereStatement);
     ROSE_ASSERT(whereKeyword != NULL);
     setSourcePosition(whereStatement,whereKeyword);

  // DQ (11/17/2007): Added support for string labels
     setStatementStringLabel(whereStatement,id);

     condition->set_parent(whereStatement);
  // setSourcePosition(condition);

     body->set_parent(whereStatement);
     setSourcePosition(body);

  // falseBlock->set_parent(whereStatement);
  // setSourcePosition(falseBlock);

     SgScopeStatement* currentScope      = astScopeStack.front();
     SgScopeStatement* currentBlockScope = isSgBasicBlock(currentScope);

     ROSE_ASSERT(currentBlockScope != NULL);
     currentBlockScope->append_statement(whereStatement);

     setStatementNumericLabelUsingStack(whereStatement);

     astScopeStack.push_front(body);

  // The c_action_construct_where_stmt() is called when we do have an end where statement
     whereStatement->set_has_end_statement(true);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R745 c_action_where_construct_stmt()");
#endif
   }

/**
 * R746
 * where_body_construct
 *
 */
void c_action_where_body_construct()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R746 c_action_where_body_construct() \n");
   }

/** R749 
 * masked_elsewhere_stmt
 *
 * : T_ELSE T_WHERE T_LPAREN expr T_RPAREN ( T_IDENT )? T_EOS
 * |    T_ELSEWHERE     T_LPAREN expr T_RPAREN ( T_IDENT )? T_EOS
 *
 * @param id Optional name for the loop. If you use this up front, you have to use it in the end, and vice versa.
 */
// void c_action_masked_elsewhere_stmt(Token_t * label, Token_t * id)
void c_action_masked_elsewhere_stmt(Token_t *label, Token_t *elseKeyword, Token_t *whereKeyword, Token_t *id, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R749 c_action_masked_elsewhere_stmt() label = %p id = %p = %s \n",label,id,id ? id->text : "NULL");

  // Get the associated where statement and push the false body onto the scope stack.
     ROSE_ASSERT(astScopeStack.empty() == false);
     SgBasicBlock* current_where_body = isSgBasicBlock(astScopeStack.front());
     SgWhereStatement* parentWhereStmt = isSgWhereStatement(current_where_body->get_parent());
     SgElseWhereStatement* parentElseWhereStmt = isSgElseWhereStatement(current_where_body->get_parent());

     bool parentIsWhereStmt = true;
           
     if (parentWhereStmt == NULL) {
       parentIsWhereStmt = false;
       ROSE_ASSERT(parentElseWhereStmt != NULL);
     }

     ROSE_ASSERT(astExpressionStack.empty() == false);
     SgExpression* condition  = astExpressionStack.front();
     astExpressionStack.pop_front();

     SgBasicBlock* body = new SgBasicBlock();

  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     body->setCaseInsensitive(true);

     SgElseWhereStatement* elseWhereStatement = new SgElseWhereStatement(condition,body,NULL);

     ROSE_ASSERT(elseKeyword != NULL);
     setSourcePosition(elseWhereStatement,elseKeyword);

     condition->set_parent(elseWhereStatement);
     //setSourcePosition(condition);

     body->set_parent(elseWhereStatement);
     setSourcePosition(body);

     SgBasicBlock* elseWhereBody = NULL;
     if (parentIsWhereStmt) {
       parentWhereStmt->set_elsewhere(elseWhereStatement);
       elseWhereStatement->set_parent(parentWhereStmt);
       ROSE_ASSERT(parentWhereStmt->get_elsewhere() != NULL);
       elseWhereBody = parentWhereStmt->get_elsewhere()->get_body();
     } else {
       parentElseWhereStmt->set_elsewhere(elseWhereStatement);
       elseWhereStatement->set_parent(parentElseWhereStmt);
       ROSE_ASSERT(parentElseWhereStmt->get_elsewhere() != NULL);
       elseWhereBody = parentElseWhereStmt->get_elsewhere()->get_body();
     }
     ROSE_ASSERT(elseWhereBody != NULL);

  // DQ (10/10/2010): Test ending position
     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct() != NULL);

  // DQ (10/10/2010): Set the end position using the endKeyword.
     ROSE_ASSERT(elseKeyword != NULL);
     resetEndingSourcePosition(astScopeStack.front(),elseKeyword);

     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct()->get_line() != astScopeStack.front()->get_startOfConstruct()->get_line());

     astScopeStack.pop_front();
     astScopeStack.push_front(elseWhereBody);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R749 c_action_masked_elsewhere_stmt()");
#endif
   }

        
/** R749 end
 * masked_elsewhere_stmt__end
 *
 * @param numBodyConstructs The number of where-body-constructs in the elsewhere-stmt (called from R744)
 */
        
void c_action_masked_elsewhere_stmt__end(int numBodyConstructs)
        {
          if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
                        printf ("In R749 c_action_masked_elsewhere_stmt__end() numBodyConstructs = %d \n",numBodyConstructs);
                
          ROSE_ASSERT(astScopeStack.empty() == false);
          SgBasicBlock* basicBlock = isSgBasicBlock(astScopeStack.front());
          ROSE_ASSERT(basicBlock != NULL);

  // The elsewhere needs to attach itself to the elsewhere-stmt
  // astScopeStack.pop_front();
        }
        
        
/** R750 
 * elsewhere_stmt
 *
 *              (label {lbl=$label.tk;})? T_ELSE T_WHERE (id=T_IDENT)? T_EOS
 * |    (label {lbl=$label.tk;})? T_ELSEWHERE   (id=T_IDENT)? T_EOS 
 *
 * @param label The label
 * @param id Optional name for the loop. If you use this up front, you have to use it in the end, and vice versa.
 */
// void c_action_elsewhere_stmt(Token_t * label, Token_t * id)
void c_action_elsewhere_stmt(Token_t *label, Token_t *elseKeyword, Token_t *whereKeyword, Token_t *id, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R750 c_action_elsewhere_stmt() label = %p id = %p = %s \n",label,id,id ? id->text : "NULL");

  // Get the associated where statement and push the false body onto the scope stack.
     ROSE_ASSERT(astScopeStack.empty() == false);
     SgBasicBlock* current_where_body = isSgBasicBlock(astScopeStack.front());
     SgWhereStatement* parentWhereStmt = isSgWhereStatement(current_where_body->get_parent());
     SgElseWhereStatement* parentElseWhereStmt = isSgElseWhereStatement(current_where_body->get_parent());

     bool parentIsWhereStmt = true;
           
     if (parentWhereStmt == NULL) {
       parentIsWhereStmt = false;
       ROSE_ASSERT(parentElseWhereStmt != NULL);
     }

     SgExpression* condition  = new SgNullExpression();
     SgBasicBlock* body = new SgBasicBlock();

  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     body->setCaseInsensitive(true);

     SgElseWhereStatement* elseWhereStatement = new SgElseWhereStatement(condition,body,NULL);

     ROSE_ASSERT(elseKeyword != NULL);
     setSourcePosition(elseWhereStatement,elseKeyword);

     condition->set_parent(elseWhereStatement);
     setSourcePosition(condition);

     body->set_parent(elseWhereStatement);
     setSourcePosition(body);

     SgBasicBlock* elseWhereBody = NULL;
     if (parentIsWhereStmt) {
       parentWhereStmt->set_elsewhere(elseWhereStatement);
       elseWhereStatement->set_parent(parentWhereStmt);
       ROSE_ASSERT(parentWhereStmt->get_elsewhere() != NULL);
       elseWhereBody = parentWhereStmt->get_elsewhere()->get_body();
     } else {
       parentElseWhereStmt->set_elsewhere(elseWhereStatement);
       elseWhereStatement->set_parent(parentElseWhereStmt);
       ROSE_ASSERT(parentElseWhereStmt->get_elsewhere() != NULL);
       elseWhereBody = parentElseWhereStmt->get_elsewhere()->get_body();
       printf("In else where part %s\r\n", __FUNCTION__);
     }
     ROSE_ASSERT(elseWhereBody != NULL);

  // DQ (10/10/2010): Test ending position
     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct() != NULL);

  // DQ (10/10/2010): Set the end position using the endKeyword.
     ROSE_ASSERT(elseKeyword != NULL);
     resetEndingSourcePosition(astScopeStack.front(),elseKeyword);

     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct()->get_line() != astScopeStack.front()->get_startOfConstruct()->get_line());

     astScopeStack.pop_front();
     astScopeStack.push_front(elseWhereBody);
   }

/** R750 end
 * elsewhere_stmt__end
 *
 * @param numBodyConstructs The number of where-body-constructs in the elsewhere-stmt (called from R744)
 */
void c_action_elsewhere_stmt__end(int numBodyConstructs)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R750 c_action_elsewhere_stmt__end() numBodyConstructs = %d \n",numBodyConstructs);

     ROSE_ASSERT(astScopeStack.empty() == false);
     SgBasicBlock* basicBlock = isSgBasicBlock(astScopeStack.front());
     ROSE_ASSERT(basicBlock != NULL);

  // astScopeStack.pop_front();
   }

/** R751 
 * end_where_stmt
 *
 * : T_END T_WHERE ( T_IDENT )? T_EOS
 * | T_ENDWHERE ( T_IDENT )? T_EOS
 *
 * @param label The label
 * @param id Optional name for the loop. If you use this up front, you have to use it in the end, and vice versa.
 */
// void c_action_end_where_stmt(Token_t * label, Token_t * id)
void c_action_end_where_stmt(Token_t *label, Token_t *endKeyword, Token_t *whereKeyword, Token_t *id, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R751 c_action_end_where_stmt() label = %p id = %p = %s \n",label,id,id ? id->text : "NULL");

  // DQ (10/10/2010): Test ending position
     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct() != NULL);

  // DQ (10/10/2010): Set the end position using the endKeyword.
     ROSE_ASSERT(endKeyword != NULL);
     resetEndingSourcePosition(astScopeStack.front(),endKeyword);

     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct()->get_line() != astScopeStack.front()->get_startOfConstruct()->get_line());

     astScopeStack.pop_front();

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R751 c_action_end_where_stmt()");
#endif
   }

/** R753 
 * forall_construct_stmt
 *
 * (T_IDENT T_COLON)? T_FORALL_CONSTRUCT_STMT T_FORALL forall_header T_EOS
 * @param label The label
 * @param id Optional name for the forall loop. If you use this up front, you have to use it in the end, and vice versa.
 */
// void c_action_forall_construct_stmt(Token_t * label, Token_t * id)
void c_action_forall_construct_stmt(Token_t *label, Token_t *id, Token_t *forallKeyword, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R753 c_action_forall_construct_stmt() label = %p = %s id = %p = %s forallKeyword = %p = %s \n",
               label,label != NULL ? label->text : "NULL",id,id ? id->text : "NULL",forallKeyword,forallKeyword ? forallKeyword->text : "NULL");

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R753 c_action_forall_construct_stmt()");
#endif

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R753 c_action_forall_construct_stmt()");
#endif
   }

/**
 * R754
 * forall_header
 *
 */
void c_action_forall_header()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_forall_header() \n");
   }

        
/** R755
 * forall_triplet_spec
 *
 * : T_IDENT T_EQUALS expr T_COLON expr ( T_COLON expr )?
 *
 * @param id Identifier on left-hand side, e.g., id = 1:10.
 * @param hasStride True if has a stride element in spec., e.g., 1:100:5.
 */
void c_action_forall_triplet_spec(Token_t * id, ofp_bool hasStride)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_forall_triplet_spec(): id = %p = %s hasStride = %s \n",id,id != NULL ? id->text : "NULL",hasStride ? "true" : "false");

     bool hasLowerBound = true;
     bool hasUpperBound = true;
     bool isAmbiguous   = false;
     SgExpression* subscript = buildSubscriptExpression(hasLowerBound,hasUpperBound,hasStride,isAmbiguous);

     ROSE_ASSERT(id != NULL);
     SgName name = id->text;

  // DQ (1/18/2011): This detects where we have used the semantics of implicitly building symbols for implicit variables.
  // printf ("WARNING: This use of trace_back_through_parent_scopes_lookup_variable_symbol() used the side effect of building a symbol if the reference is not found! \n");
  // ROSE_ASSERT(false);

     SgVariableSymbol* variableSymbol = trace_back_through_parent_scopes_lookup_variable_symbol(name,astScopeStack.front());
     if (variableSymbol == NULL)
        {
       // DQ (1/19/2011): Build the implicit variable
          buildImplicitVariableDeclaration(name);

       // Now verify that it is present.
          variableSymbol = trace_back_through_parent_scopes_lookup_variable_symbol(name,astScopeStack.front());
          ROSE_ASSERT(variableSymbol != NULL);
        }

     ROSE_ASSERT(variableSymbol != NULL);
     SgVarRefExp* indexVar = new SgVarRefExp(variableSymbol);

     setSourcePosition(indexVar,id);

     SgAssignOp* indexExpression = new SgAssignOp(indexVar,subscript,NULL);
     setSourcePosition(indexExpression);

  // DQ (1/16/2008): Must pass new consistency test for lvalues
     indexVar->set_lvalue(true);

  // astExpressionStack.push_front(subscript);
  // astNodeStack.push_front(subscript);
     astNodeStack.push_front(indexExpression);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R755 c_action_forall_triplet_spec()");
#endif
   }

/** R755 list
 * forall_triplet_spec_list
 *      :       forall_triplet_spec ( T_COMMA forall_triplet_spec )*
 * 
 * @param count The number of items in the list.
 */
void c_action_forall_triplet_spec_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_forall_triplet_spec_list__begin() \n");

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R755 (list) c_action_forall_triplet_spec_list__begin()");
#endif
   }

void c_action_forall_triplet_spec_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_forall_triplet_spec_list(): count = %d \n",count);

     SgExprListExp* expressionList = new SgExprListExp();
     for (int i=0; i < count; i++)
        {
          ROSE_ASSERT(astNodeStack.empty() == false);
          SgExpression* expression = isSgExpression(astNodeStack.front());
          ROSE_ASSERT(expression != NULL);

          expressionList->prepend_expression(expression);

          astNodeStack.pop_front();
        }

     setSourcePosition(expressionList);

     astNodeStack.push_front(expressionList);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R755 (list) c_action_forall_triplet_spec_list()");
#endif
   }

/** R757
 * forall_assignment_stmt
 *
 * :  assignment_stmt
 * |  pointer_assignment_stmt
 *
 * @param isPointerAssignment True if this is pointer assignment statement.
 */
void c_action_forall_assignment_stmt(ofp_bool isPointerAssignment)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_forall_assignment_stmt() \n");
   }

/** R758
 * end_forall_stmt
 *
 * : (label)? T_END T_FORALL (T_IDENT)? T_EOS
 * | (label)? T_ENDFORALL (T_IDENT)? T_EOS
 *
 * @param label The label.
 * @param id Optional identifier for the loop.
 */
// void c_action_end_forall_stmt(Token_t * label, Token_t * id)
void c_action_end_forall_stmt(Token_t *label, Token_t *endKeyword, Token_t *forallKeyword, Token_t *id, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R753 c_action_end_forall_stmt() label = %p = %s endKeyword = %p = %s forallKeyword = %p = %s id = %p = %s \n",
               label,label != NULL ? label->text : "NULL",endKeyword,endKeyword != NULL ? endKeyword->text : "NULL",
               forallKeyword,forallKeyword ? forallKeyword->text : "NULL",id,id != NULL ? id->text : "NULL");
   }

/** R759
 * forall_stmt
 *
 * :(label)? T_FORALL_STMT T_FORALL forall_header forall_assignment_stmt
 *
 * @param label The label.
 */
void c_action_forall_stmt__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_forall_stmt__begin() \n");

#if !SKIP_C_ACTION_IMPLEMENTATION
     SgBasicBlock* body  = new SgBasicBlock();

  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     body->setCaseInsensitive(true);

     SgForAllStatement* forAllStatement = new SgForAllStatement(NULL,body);

  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     forAllStatement->setCaseInsensitive(true);

     setSourcePosition(body);
     body->set_parent(forAllStatement);

     SgScopeStatement* currentScope = getTopOfScopeStack();
     currentScope->append_statement(forAllStatement);
     forAllStatement->set_parent(currentScope);

  // Push the if scope (it is a scope in C/C++, even if not in Fortran)
  // treating it as a scope will allow it to be consistent across C,C++, and Fortran.
     astScopeStack.push_front(forAllStatement);

  // Now push the SgBasicBlock
     astScopeStack.push_front(body);
#endif
   }

// void c_action_forall_stmt(Token_t * label)
void c_action_forall_stmt(Token_t *label, Token_t *forallKeyword)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R753 c_action_forall_stmt() label = %p = %s forallKeyword = %p = %s \n",
               label,label != NULL ? label->text : "NULL",forallKeyword,forallKeyword ? forallKeyword->text : "NULL");

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R759 c_action_forall_stmt()");
#endif

     ROSE_ASSERT(astNodeStack.empty() == false);
     SgStatement* statement = isSgStatement(astNodeStack.front());
     ROSE_ASSERT(statement != NULL);
     astNodeStack.pop_front();

     ROSE_ASSERT(astNodeStack.empty() == false);
     SgExprListExp* forAllHeader = isSgExprListExp(astNodeStack.front());
     ROSE_ASSERT(forAllHeader != NULL);
     astNodeStack.pop_front();

  // DQ (10/10/2010): Test ending position
     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct() != NULL);

     SgStatement* lastStatement = astScopeStack.front()->lastStatement();
  // printf ("In c_action_forall_stmt(): lastStatement = %p \n",lastStatement);
     ROSE_ASSERT(lastStatement != NULL);
     resetEndingSourcePosition(astScopeStack.front(),lastStatement);

     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct()->get_line() != astScopeStack.front()->get_startOfConstruct()->get_line());

     SgBasicBlock* body = isSgBasicBlock(astScopeStack.front());
     ROSE_ASSERT(body != NULL);
     astScopeStack.pop_front();

  // SgForAllStatement* forAllStatement = isSgForAllStatement(body->get_parent());
     SgForAllStatement* forAllStatement = isSgForAllStatement(astScopeStack.front());
     ROSE_ASSERT(forAllStatement != NULL);
     astScopeStack.pop_front();

     forAllStatement->set_forall_header(forAllHeader);
     forAllHeader->set_parent(forAllStatement);

     forAllStatement->set_has_end_statement(false);

     setSourcePosition(forAllStatement,forallKeyword);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R759 c_action_forall_stmt()");
#endif
   }

/**
 * R801
 * block
 *
 */
void c_action_block()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_block() (popping the current scope from a case statement) \n");

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R801 c_action_block()");
#endif

  // Pop off the select block!
     ROSE_ASSERT(astScopeStack.empty() == false);

  // DQ (10/10/2010): Test ending position
     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct() != NULL);

  // DQ (10/10/2010): Set the end position to a better value.
     SgStatement* lastStatement = astScopeStack.front()->lastStatement();
  // printf ("In c_action_block(): lastStatement = %p \n",lastStatement);
     if (lastStatement != NULL)
        {
          resetEndingSourcePosition(astScopeStack.front(),lastStatement);
        }
       else
        {
       // DQ (10/10/2010): Set the end position to be on the next line (for now)
          resetEndingSourcePosition(astScopeStack.front(),astScopeStack.front()->get_endOfConstruct()->get_line()+1);
        }

     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct()->get_line() != astScopeStack.front()->get_startOfConstruct()->get_line());

  // This scope on the stack shuold be a SgBasicBlock
     ROSE_ASSERT(isSgBasicBlock(astScopeStack.front()) != NULL);
     astScopeStack.pop_front();

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R801 c_action_block()");
#endif
   }

/**
 * R802
 * if_construct
 *
 */
void c_action_if_construct()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_if_construct() \n");

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R802 c_action_if_construct()");
#endif
   }

/** R803
 * if_then_stmt
 *
 * : (label)? ( T_IDENT T_COLON )? T_IF T_LPAREN expr T_RPAREN T_THEN T_EOS
 *
 * @param label The label.
 * @param id Optional identifier used for the statement.
 */
// void c_action_if_then_stmt(Token_t * label, Token_t * id)
void c_action_if_then_stmt( Token_t *label, Token_t *id, Token_t *ifKeyword, Token_t *thenKeyword, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_if_then_stmt(): label = %p id = %p = %s \n",label,id,id != NULL ? id->text : "NULL");

#if !SKIP_C_ACTION_IMPLEMENTATION
   // Do we need a 3rd label for the SgIfStmt?
      if (label != NULL)
         {
           ROSE_ASSERT(label->text != NULL);
        // printf ("label->text = %s \n",label->text);
         }

     SgExpression* conditionalExpression   = getTopOfExpressionStack();
     astExpressionStack.pop_front();

     SgExprStatement* conditionalStatement = new SgExprStatement(conditionalExpression);
     SgBasicBlock* true_block  = new SgBasicBlock();
     SgBasicBlock* false_block = new SgBasicBlock();
     SgIfStmt* ifStatement     = new SgIfStmt(conditionalStatement,true_block,false_block);

  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     true_block->setCaseInsensitive(true);
     false_block->setCaseInsensitive(true);
     ifStatement->setCaseInsensitive(true);

  // DQ (12/7/2010): Let the unparser know that this came from R803 sio that we know to output the "then" keyword.
     ifStatement->set_use_then_keyword(true);

  // Save the if-stmt that might be the start the a chain of if-then-else-if-else-endif
     astIfStatementStack.push_front(ifStatement);
  // printf ("Pushed ifStatement = %p onto astIfStatementStack (size = %zu) \n",ifStatement,astIfStatementStack.size());

  // This is the version of the if-stmt which must have an associated endif, but only at the end of a chain of if then .. else .. else .. endif
  // ifStatement->set_has_end_statement(true);

  // DQ (11/17/2007): Added support for numeric labels
     setStatementNumericLabel(ifStatement,label);

  // DQ (11/17/2007): Added support for string labels
     setStatementStringLabel(ifStatement,id);

     setSourcePosition(conditionalStatement);
  // DQ (1/22/2008): Try this
     resetSourcePosition(conditionalStatement,conditionalExpression);

     setSourcePosition(true_block);
     setSourcePosition(false_block);

  // DQ (11/17/2007): Get the source position from the "if" keyword.
  // setSourcePosition(ifStatement);
     if (ifKeyword != NULL)
        {
          setSourcePosition(ifStatement,ifKeyword);
        }
       else
        {
       // See test2007_226.f for case of ELSEIF which cause ifKeyword to be NULL
       // ROSE_ASSERT(id != NULL);
       // setSourcePosition(ifStatement,id);
          ROSE_ASSERT(thenKeyword != NULL);
          setSourcePosition(ifStatement,thenKeyword);
        }

     SgScopeStatement* currentScope = getTopOfScopeStack();
     currentScope->append_statement(ifStatement);
     ifStatement->set_parent(currentScope);

     SgBasicBlock* currentBlockScope = isSgBasicBlock(currentScope);
     ROSE_ASSERT(currentBlockScope != NULL);
  // printf ("Appending ifStatement = %p to scope = %p currentBlockScope->get_statements().size() = %zu \n",ifStatement,currentScope,currentBlockScope->get_statements().size());

  // Push the if scope (it is a scope in C/C++, even if not in Fortran)
  // treating it as a scope will allow it to be consistent across C,C++, and Fortran.
     astScopeStack.push_front(ifStatement);

  // Now push the SgBasicBlock
     astScopeStack.push_front(true_block);

  // Set or clear the astLabelSymbolStack (since it is redundant with the label being passed in)
  // There are two mechanisms used to set labels, and we never know which will be used by OFP.
     setStatementNumericLabelUsingStack(ifStatement);
#endif

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R803 c_action_if_then_stmt()");
#endif

  // DQ (11/26/2007): This should be empty now. Check to make sure that we have not left track on the stack.
     ROSE_ASSERT(astExpressionStack.empty() == true);
   }

/** R804
 * else_if_stmt
 *
 * : (label)? T_ELSE T_IF T_LPAREN expr T_RPAREN T_THEN ( T_IDENT )? T_EOS
 * | (label)? T_ELSEIF T_LPAREN expr T_RPAREN T_THEN ( T_IDENT )? T_EOS
 *
 * @param label The label.
 * @param id Optional identifier used for the statement.
 */
// void c_action_else_if_stmt(Token_t * label, Token_t * id)
void c_action_else_if_stmt(Token_t *label, Token_t *elseKeyword, Token_t *ifKeyword, Token_t *thenKeyword, Token_t *id, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_else_if_stmt() label = %p = %s elseKeyword = %p = %s ifKeyword = %p = %s caseKeyword = %p = %s id = %p = %s \n",
               label,label ? label->text : "NULL",
               elseKeyword,elseKeyword ? elseKeyword->text : "NULL",
               ifKeyword,ifKeyword ? ifKeyword->text : "NULL",
               thenKeyword,thenKeyword ? thenKeyword->text : "NULL",
               id,id ? id->text : "NULL");

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R804 c_action_else_if_stmt()");
#endif

   // Do we need a 3rd label for the SgIfStmt?
      if (label != NULL)
         {
           ROSE_ASSERT(label->text != NULL);
        // printf ("label->text = %s \n",label->text);
         }

     SgScopeStatement* currentScope = getTopOfScopeStack();
     SgIfStmt* ifStatement = isSgIfStmt(currentScope);
     ROSE_ASSERT(ifStatement != NULL);

  // DQ (12/6/2010): I think that this can be set explicitly.
  // ifStatement->set_has_end_statement(true);
     ifStatement->set_use_then_keyword(true);
     ifStatement->set_is_else_if_statement(true);

  // Find the previously built false body in the SgIfStmt
     SgBasicBlock* false_body = isSgBasicBlock(ifStatement->get_false_body());
     ROSE_ASSERT(false_body != NULL);

  // Push the false body onto the scope stack
     astScopeStack.push_front(false_body);

  // Not call the R803 rule to build a SgIfStmt
     c_action_if_then_stmt(label,id,ifKeyword,thenKeyword,eos);

  // After calling c_action_if_then_stmt() to reuse code there, pop the if-stmt that was saved, 
  // since it is NOT the beginning of a chain of if-then-else-if-else-endif statements.
  // printf ("Pop ifStatement = %p off of astIfStatementStack (pushed on by R804 calling R803) (size = %zu) \n",ifStatement,astIfStatementStack.size());
     astIfStatementStack.pop_front();

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("After calling R803 from R804 in c_action_else_if_stmt()");
#endif

  // printf ("CLEAR THE astLabelSymbolStack (c_action_else_if_stmt) \n");
     astLabelSymbolStack.clear();

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R804 c_action_else_if_stmt()");
#endif
   }

/** R805
 * else_stmt
 *
 * : (label)? T_ELSE ( T_IDENT )? T_EOS
 *
 * @param label The label.
 * @param id Optional identifier used for the statement.
 */
// void c_action_else_stmt(Token_t * label, Token_t * id)
void c_action_else_stmt(Token_t *label, Token_t *elseKeyword, Token_t *id, Token_t *eos)
   {
  // Support for else in SgIfStmt

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_else_stmt(): label = %p = %s elseKeyword = %p = %s id = %p = %s \n",label,label != NULL ? label->text : "NULL",
               elseKeyword,elseKeyword != NULL ? elseKeyword->text : "NULL",id,id != NULL ? id->text : "NULL");

   // Do we need a 3rd label for the SgIfStmt?
      if (label != NULL)
         {
        // Make sure that the text string is valid (this was a problem for a while)
           ROSE_ASSERT(label->text != NULL);
        // printf ("label->text = %s \n",label->text);
         }

  // DQ (12/12/2007): After implimenting the support for the select and case statements
  // it made more sense to pop the scope in R801 instead of here.
  // pop off the true block!
  // astScopeStack.pop_front();

     SgScopeStatement* currentScope = getTopOfScopeStack();
     SgIfStmt* ifStatement = isSgIfStmt(currentScope);
     ROSE_ASSERT(ifStatement != NULL);

  // DQ (12/7/2010): Mark this explicitly as being associated with an else (might not be required).
  // ifStatement->set_is_else_if_statement(true);

  // Find the previously built false body in the SgIfStmt
     SgBasicBlock* false_body = isSgBasicBlock(ifStatement->get_false_body());
     ROSE_ASSERT(false_body != NULL);

  // Push the false body onto the scope stack
     astScopeStack.push_front(false_body);

  // DQ (12/16/2007): Added support for specification of label on the else part of the SgIfStmt
     setStatementElseNumericLabel(ifStatement,label);

  // printf ("CLEAR THE astLabelSymbolStack (c_action_else_stmt) \n");
     astLabelSymbolStack.clear();
   }

/** R806
 * end_if_stmt
 *
: (label)? T_END T_IF ( T_IDENT )? T_EOS
| (label)? T_ENDIF      ( T_IDENT )? T_EOS
 *
 * @param label The label.
 * @param id True if has what you think it has.
 */
// void c_action_end_if_stmt(Token_t * label, Token_t * id)
void c_action_end_if_stmt(Token_t *label, Token_t *endKeyword, Token_t *ifKeyword, Token_t *id, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
        {
       // printf ("In c_action_end_if_stmt(): label = %p id = %p \n",label,id);
          printf ("In R1230 c_action_end_function_stmt(): label = %p = %s endKeyword = %p = %s ifKeyword = %p = %s id = %p = %s \n",
               label,label != NULL ? label->text : "NULL",
               endKeyword,endKeyword != NULL ? endKeyword->text : "NULL",
               ifKeyword,ifKeyword != NULL ? ifKeyword->text : "NULL",
               id,id != NULL ? id->text : "NULL");
        }

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R806 c_action_end_if_stmt()");
#endif

  // Do we need a 3rd label for the SgIfStmt?
     if (label != NULL)
        {
          ROSE_ASSERT(label->text != NULL);
       // printf ("label->text = %s \n",label->text);
        }

  // DQ (12/12/2007): After implimenting the support for the select and case statements
  // it made more sense to pop the scope in R801 instead of here.
  // Pop off the true block!
  // ROSE_ASSERT(astScopeStack.empty() == false);
  // astScopeStack.pop_front();

  // Remove the accumulated scopes on the stack back to the original starting SgIfStmt object
     ROSE_ASSERT(astIfStatementStack.empty() == false);
     SgIfStmt* startingIfStatement = astIfStatementStack.front();
  // printf ("startingIfStatement = %p \n",startingIfStatement);

  // printf ("START: astScopeStack.front() = %p \n",astScopeStack.front());
     while (astScopeStack.empty() == false && astScopeStack.front() != startingIfStatement)
        {
       // Unwind through all the accumulated IF ... ELSE using a single END IF.
       // printf ("LOOP: (unwinding through if ... else statements) astScopeStack.front() = %p = %s \n",astScopeStack.front(),astScopeStack.front()->class_name().c_str());

       // DQ (10/10/2010): Test ending position
          ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct() != NULL);

          ROSE_ASSERT(endKeyword != NULL);
          resetEndingSourcePosition(astScopeStack.front(),endKeyword);

          ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct()->get_line() != astScopeStack.front()->get_startOfConstruct()->get_line());

          astScopeStack.pop_front();
        }
  // printf ("END: astScopeStack.front() = %p \n",astScopeStack.front());
     astIfStatementStack.pop_front();

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("After cleaning up stack of if-stmt and block pairs in R806 c_action_end_if_stmt()");
#endif

     SgIfStmt* ifStatement = isSgIfStmt(astScopeStack.front());
     ROSE_ASSERT(ifStatement != NULL);

  // DQ (12/26/2007): record that the if statement has an associated endif statement
     ifStatement->set_has_end_statement(true);

     if (label != NULL)
          setStatementEndNumericLabel(ifStatement,label);

  // DQ (10/10/2010): Test ending position
     ROSE_ASSERT(astScopeStack.empty() == false);
     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct() != NULL);

     ROSE_ASSERT(endKeyword != NULL);
     resetEndingSourcePosition(astScopeStack.front(),endKeyword);

     // ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct()->get_line() != astScopeStack.front()->get_startOfConstruct()->get_line());

  // Pop off the if scope (it is a scope in C/C++, even if not in Fortran)
  // treating it as a scope will allow it to be consistent across C,C++, and Fortran.
     ROSE_ASSERT(astScopeStack.empty() == false);
     astScopeStack.pop_front();

  // printf ("CLEAR THE astLabelSymbolStack (c_action_end_if_stmt) \n");
     astLabelSymbolStack.clear();

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R806 c_action_end_if_stmt()");
#endif
   }

/** R807
 * if_stmt
 *
 * : (label)? T_IF_STMT T_IF T_LPAREN expr T_RPAREN action_stmt
 *
 * @param label The label.
 */
void c_action_if_stmt__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R807 c_action_if_stmt__begin() \n");

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R807 c_action_if_stmt__begin()");
#endif

#if !SKIP_C_ACTION_IMPLEMENTATION
     SgBasicBlock* true_block  = new SgBasicBlock();
     SgBasicBlock* false_block = new SgBasicBlock();
     SgIfStmt* ifStatement     = new SgIfStmt((SgStatement*)NULL,true_block,false_block);

  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     true_block->setCaseInsensitive(true);
     false_block->setCaseInsensitive(true);
     ifStatement->setCaseInsensitive(true);

  // astIfStatementStack.push_front(ifStatement);

     setSourcePosition(true_block);
     setSourcePosition(false_block);

     SgScopeStatement* currentScope = getTopOfScopeStack();
     currentScope->append_statement(ifStatement);
     ifStatement->set_parent(currentScope);

  // DQ (12/7/2010): Need to handle the label, see test2010_133.f90.
  // processLabelOnStack(ifStatement);
     specialFixupForLabelOnStackAndNotPassedAsParameter(ifStatement);

  // DQ (12/9/2010): For test2010_135.f90 this should be valid.
  // ROSE_ASSERT(ifStatement->get_numeric_label() != NULL);

  // Push the if scope (it is a scope in C/C++, even if not in Fortran)
  // treating it as a scope will allow it to be consistent across C,C++, and Fortran.
     astScopeStack.push_front(ifStatement);

  // Now push the SgBasicBlock
     astScopeStack.push_front(true_block);
#endif
   }

// void c_action_if_stmt(Token_t * label)
void c_action_if_stmt(Token_t *label, Token_t *ifKeyword)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R807 c_action_if_stmt() label = %p = %s \n",label,label ? label->text : "NULL"); 

  // This rule is for statements of the sort: "if (a) b = 0" withouth an associated endif statement.

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R807 c_action_if_stmt()");
#endif

#if !SKIP_C_ACTION_IMPLEMENTATION
     ROSE_ASSERT(astExpressionStack.empty() == false);

     SgExpression* conditionalExpression = getTopOfExpressionStack();
     astExpressionStack.pop_front();

     SgExprStatement* conditionalStatement = new SgExprStatement(conditionalExpression);

  // setSourcePosition(conditionalStatement);
     ROSE_ASSERT(ifKeyword != NULL);
     setSourcePosition(conditionalStatement,ifKeyword);

     SgIfStmt* ifStatement = isSgIfStmt(astScopeStack.front()->get_parent());
     ROSE_ASSERT(ifStatement != NULL);
     ifStatement->set_conditional(conditionalStatement);

  // DQ (12/7/2010): Mark to not use the "then" keyword (this is the default).
     ifStatement->set_use_then_keyword(false);

  // DQ (11/17/2007): Added support for numeric labels
     setStatementNumericLabel(ifStatement,label);

     if (label != NULL)
        {
          ROSE_ASSERT(ifStatement->get_numeric_label() != NULL);
        }

  // DQ (11/17/2007): Get the source position from the "if" keyword.
     setSourcePosition(ifStatement,ifKeyword);

     SgBasicBlock* body = isSgBasicBlock(astScopeStack.front());
     ROSE_ASSERT(body != NULL);

     SgStatementPtrList & statementList = body->get_statements();
     SgStatementPtrList::iterator i = statementList.begin();
     while (i != statementList.end())
        {
       // Clear the numeric labels in the block of an if-statement (should have only a single statement)
       // If that had been R803, we would not clear the labels since that if construct has a block where 
       // the statements in the block can have labels.
          (*i)->set_numeric_label(NULL);
          i++;
        }     

  // DQ (10/10/2010): Test ending position
     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct() != NULL);

     SgStatement* lastStatement = astScopeStack.front()->lastStatement();
  // printf ("In c_action_if_stmt(): lastStatement = %p \n",lastStatement);
     ROSE_ASSERT(lastStatement != NULL);
     resetEndingSourcePosition(astScopeStack.front(),lastStatement);

     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct()->get_line() != astScopeStack.front()->get_startOfConstruct()->get_line());

  // Now pop the SgBasicBlock
     astScopeStack.pop_front();

  // DQ (10/10/2010): Test ending position
     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct() != NULL);

     SgStatement* nextLastStatement = astScopeStack.front()->lastStatement();
  // printf ("In c_action_if_stmt(): nextLastStatement = %p \n",nextLastStatement);
     ROSE_ASSERT(nextLastStatement != NULL);
     resetEndingSourcePosition(astScopeStack.front(),nextLastStatement);

  // DQ (10/10/2010): See example test2007_17.f90 of if statment on a single line for were we can't enforce this.
  // ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct()->get_line() != astScopeStack.front()->get_startOfConstruct()->get_line());

     SgIfStmt* topOfStack_ifStatement = isSgIfStmt(astScopeStack.front());
     ROSE_ASSERT(topOfStack_ifStatement != NULL);
     ROSE_ASSERT(ifStatement == topOfStack_ifStatement);

  // This should be a simple if-stmt without and endif statement.
     ROSE_ASSERT(ifStatement->get_has_end_statement() == false);

  // Pop the if scope (it is a scope in C/C++, even if not in Fortran)
  // treating it as a scope will allow it to be consistent across C,C++, and Fortran.
     astScopeStack.pop_front();

  // printf ("CLEAR THE astLabelSymbolStack (c_action_if_stmt) \n");
     astLabelSymbolStack.clear();
#endif

  // DQ (11/26/2007): This should be empty now. Check to make sure that we have not left trash on the stack.
     ROSE_ASSERT(astExpressionStack.empty() == true);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R807 c_action_if_stmt()");
#endif
   }

/**
 * R808
 * case_construct
 *
 */
void c_action_case_construct()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_case_construct() \n");
   }


/** R809
 * select_case_stmt
 *
 * :  (label)? ( T_IDENT T_COLON )?  (T_SELECT T_CASE | T_SELECTCASE)
 *   T_LPAREN expr T_RPAREN T_EOS
 *
 * @param label The label.
 * @param id Identifier if present. Otherwise, null.
 */
// void c_action_select_case_stmt(Token_t * label, Token_t * id)
void c_action_select_case_stmt(Token_t *label, Token_t *id, Token_t *selectKeyword, Token_t *caseKeyword, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_select_case_stmt() label = %p = %s id = %p = %s selectKeyword = %p = %s caseKeyword = %p = %s \n",
               label,label ? label->text : "NULL",id,id ? id->text : "NULL",selectKeyword,selectKeyword ? selectKeyword->text : "NULL",
               caseKeyword,caseKeyword ? caseKeyword->text : "NULL");

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R809 c_action_select_case_stmt()");
#endif

     SgExpression* itemSelectorExpression = getTopOfExpressionStack();
     astExpressionStack.pop_front();
     SgExprStatement* itemSelectorStatement = new SgExprStatement(itemSelectorExpression);
     setSourcePosition(itemSelectorStatement);

     SgBasicBlock* body  = new SgBasicBlock();
     ROSE_ASSERT(body != NULL);

  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     body->setCaseInsensitive(true);

     SgSwitchStatement* switchStatement = new SgSwitchStatement(itemSelectorStatement,body);
     ROSE_ASSERT(selectKeyword != NULL);

  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     switchStatement->setCaseInsensitive(true);

     setSourcePosition(switchStatement,selectKeyword);

  // A valid id is will be a named label
     if (id != NULL)
        {
          setStatementStringLabel(switchStatement,id);
          printf ("Set the named label: %s (in switchStatement = %p) \n",id->text,switchStatement);
       // ROSE_ASSERT(false);
        }

     body->set_parent(switchStatement);
     setSourcePosition(body);

     ROSE_ASSERT(astScopeStack.empty() == false);
     astScopeStack.front()->append_statement(switchStatement);

     astScopeStack.push_front(body);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R809 c_action_select_case_stmt()");
#endif
   }

/** R810
 * case_stmt
 *
 * :  (label)? T_CASE case_selector ( T_IDENT )? T_EOS
 *
 * @param label The label.
 * @param id Identifier if present. Otherwise, null.
 */
// void c_action_case_stmt(Token_t * label, Token_t * id)
void c_action_case_stmt(Token_t *label, Token_t *caseKeyword, Token_t *id, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_case_stmt() label = %p = %s caseKeyword = %p = %s id = %p = %s \n",
               label,label ? label->text : "NULL",caseKeyword,caseKeyword ? caseKeyword->text : "NULL",id,id ? id->text : "NULL");
     
#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R810 c_action_case_stmt()");
#endif

  // ROSE_ASSERT(astExpressionStack.empty() == false);

     SgBasicBlock* body  = new SgBasicBlock();
     ROSE_ASSERT(body != NULL);

  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     body->setCaseInsensitive(true);

     SgStatement* caseOrDefaultStatement = NULL;

     if (astExpressionStack.empty() == true)
        {
          ROSE_ASSERT(astNameStack.empty() == false);
          ROSE_ASSERT(matchingName(astNameStack.front()->text,"default") == true);
          astNameStack.pop_front();

          caseOrDefaultStatement = new SgDefaultOptionStmt(body);

       // A valid id is will be a "default construct name"
          if (id != NULL)
             {
               isSgDefaultOptionStmt(caseOrDefaultStatement)->set_default_construct_name(id->text);
             }
        }
       else
        {
       // DQ (9/6/2010): The case stmt can take a list of expressions as keys.
       // SgExpression* keyExpression = getTopOfExpressionStack();
       // astExpressionStack.pop_front();
       // caseOrDefaultStatement = new SgCaseOptionStmt(keyExpression,body);
          SgExprListExp* exprListExp = new SgExprListExp();
          ROSE_ASSERT(exprListExp != NULL);
          setSourcePosition(exprListExp);
          while (astExpressionStack.empty() == false)
             {
               exprListExp->prepend_expression(astExpressionStack.front());
               astExpressionStack.pop_front();
             }

          caseOrDefaultStatement = new SgCaseOptionStmt(exprListExp,body);

       // A valid id will be a "case construct name"
          if (id != NULL)
             {
               isSgCaseOptionStmt(caseOrDefaultStatement)->set_case_construct_name(id->text);
             }
        }

     setSourcePosition(caseOrDefaultStatement);

     body->set_parent(caseOrDefaultStatement);
     setSourcePosition(body);

     ROSE_ASSERT(astScopeStack.empty() == false);
     astScopeStack.front()->append_statement(caseOrDefaultStatement);

     astScopeStack.push_front(body);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R810 c_action_case_stmt()");
#endif

  // DQ (9/6/2010): Added error checking to support test2010_39.f90 test code.
  // Error checking for astExpressionStack
     ROSE_ASSERT(astScopeStack.empty() == false);
     ROSE_ASSERT(astScopeStack.front()->get_parent() != NULL);
     SgWhereStatement* whereStatement = isSgWhereStatement(astScopeStack.front()->get_parent());
     SgIfStmt* ifStatement = isSgIfStmt(astScopeStack.front()->get_parent());
     if (whereStatement != NULL || ifStatement != NULL)
        {
       // If in a where statement produced with R 619:section-subscript-list__begin then the 
       // condition is on the stack, else if it was produced with R744:where-construct-stmt 
       // then the condition was used directly and already cleared from the stack.
       // ROSE_ASSERT(astExpressionStack.empty() == false);
          ROSE_ASSERT(astExpressionStack.size() <= 1);
        }
       else
        {
       // If this is NOT a where statement then the stack should be empty.
          ROSE_ASSERT(astExpressionStack.empty() == true);
        }
   }

/** R811
 * end_select_stmt
 *
 * : (label)? T_END T_SELECT (T_IDENT)? T_EOS
 * | (label)? T_ENDSELECT       (T_IDENT)? T_EOS
 *
 * @param label The label.
 * @param id Identifier if present. Otherwise, null.
 */
// void c_action_end_select_stmt(Token_t * label, Token_t * id)
void c_action_end_select_stmt(Token_t *label, Token_t *endKeyword, Token_t *selectKeyword, Token_t *id, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_end_select_stmt() label = %p = %s endKeyword = %p = %s selectKeyword = %p = %s id = %p = %s \n",
               label,label ? label->text : "NULL",endKeyword,endKeyword ? endKeyword->text : "NULL",
               selectKeyword,selectKeyword ? selectKeyword->text : "NULL",id,id ? id->text : "NULL");

     if (label != NULL)
        {
          ROSE_ASSERT(label->text != NULL);
          printf ("label->text = %s \n",label->text);
        }

  // DQ (10/10/2010): Test ending position
     ROSE_ASSERT(astScopeStack.empty() == false);
     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct() != NULL);

  // DQ (10/10/2010): Moved the poping of the stack to this function so that we could set the source end position of the scope.
     ROSE_ASSERT(endKeyword != NULL);
     resetEndingSourcePosition(astScopeStack.front(),endKeyword);
     
     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct()->get_line() != astScopeStack.front()->get_startOfConstruct()->get_line());

  // Pop off the select block!
     ROSE_ASSERT(astScopeStack.empty() == false);
     astScopeStack.pop_front();
   }

/**
 * R813
 * case_selector
 *
 * @param defaultToken T_DEFAULT token or null.
 */
void c_action_case_selector(Token_t *defaultToken)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_case_selector(): defaultToken = %p = %s \n",defaultToken,defaultToken != NULL ? defaultToken->text : "NULL");

  // There is no expression that it makes sense to push onto the stack, so let an empty
  // expression stack and a name stack with the "default" token imply the use of the
  // default case.

  // ROSE_ASSERT(defaultToken != NULL);
     if (defaultToken != NULL)
          astNameStack.push_front(defaultToken);
   }

/**
 * R814
 * case_value_range
 *
 */
void c_action_case_value_range()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_case_value_range() \n");
   }

/** R814 list
 * case_value_range_list
 *      :       case_value_range ( T_COMMA case_value_range )*
 * 
 * @param count The number of items in the list.
 */
void c_action_case_value_range_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_case_value_range_list__begin() \n");
   }
void c_action_case_value_range_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_case_value_range_list(): count = %d \n",count);
   }

/**
 * Unknown rule.
 * case_value_range_suffix
 * 
 */
void c_action_case_value_range_suffix()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_case_value_range_suffix() \n");
   }

/**
 * R815
 * case_value
 *
 */
void c_action_case_value()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_case_value() \n");
   }

/**
 * R816
 * associate_construct
 *
 */
void c_action_associate_construct()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_associate_construct() \n");
   }

/** R817
 * associate_stmt
 *
 * : (label)? (T_IDENT T_COLON)? T_ASSOCIATE T_LPAREN association_list 
 *              T_RPAREN T_EOS
 *
 * @param label The label.
 * @param id Identifier if present. Otherwise, null.
 */
// void c_action_associate_stmt(Token_t * label, Token_t * id)
void c_action_associate_stmt(Token_t *label, Token_t *id, Token_t *associateKeyword, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_associate_stmt(): label = %p id = %p \n",label,id);

     SgNode* scopeParent = astScopeStack.front()->get_parent();
     ROSE_ASSERT(scopeParent != NULL);
     SgAssociateStatement* associateStatement = isSgAssociateStatement(scopeParent);
     ROSE_ASSERT(associateStatement != NULL);

     SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(astNodeStack.front());
     ROSE_ASSERT(variableDeclaration != NULL);
     associateStatement->set_variable_declaration(variableDeclaration);
     variableDeclaration->set_parent(associateStatement);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R817 c_action_associate_stmt()");
#endif
   }

/** R817 list
 * association_list
 *      :       association ( T_COMMA association )*
 * 
 * @param count The number of items in the list.
 */
void c_action_association_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_associate_list__begin(): \n");

  // DQ (10/2/2007): Using the new c_action_where_stmt__begin()
     SgBasicBlock* body  = new SgBasicBlock();
     ROSE_ASSERT(body != NULL);

  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     body->setCaseInsensitive(true);

     SgAssociateStatement* associateStatement = new SgAssociateStatement();
     associateStatement->set_body(body);

  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     associateStatement->setCaseInsensitive(true);

     setSourcePosition(associateStatement);

     body->set_parent(associateStatement);
     setSourcePosition(body);

     ROSE_ASSERT(astScopeStack.empty() == false);
     astScopeStack.front()->append_statement(associateStatement);

     astScopeStack.push_front(associateStatement);
     astScopeStack.push_front(body);
   }

void c_action_association_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_associate_list(): count = %d \n",count);

  // At this point we have a SgVariableDeclaration on the astNodeStack.
  // This will be come the entry in the SgAssociateStatemen's variable_declaration_list.
  // Note that we require a list of variable declarations because the types can be different.

  // DQ (11/30/2007): The current implementation only handles the case of a single associate variable. 
  // Later we need to extend this to handle a list of variable declarations.
     ROSE_ASSERT(count == 1);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R817 (list) c_action_association_list()");
#endif

  // printf ("Exiting at end of rule R817 \n");
  // ROSE_ASSERT(false);
   }

/**
 * R818
 * association
 *
 * @param id T_IDENT for associate_name.
 */
void c_action_association(Token_t *id)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_association(): id = %p = %s \n",id,id != NULL ? id->text : "NULL");

     ROSE_ASSERT(id != NULL);
  // astNameStack.push_front(id);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R818 (list) c_action_association()");
#endif

  // Transfer the expression on the astExpressionStack to the astInitializerStack (for the upcoming variable declaration)
     ROSE_ASSERT(astExpressionStack.empty() == false);

     SgAssignInitializer* initializer = new SgAssignInitializer(astExpressionStack.front(),NULL);
     setSourcePosition(initializer);

     astExpressionStack.pop_front();

     ROSE_ASSERT(id->text != NULL);
     SgName variableName = id->text;

  // Build a new variable to be the same type as the rhs expression.
     SgType* variableType = initializer->get_type();
     ROSE_ASSERT(variableType != NULL);
     astTypeStack.push_front(variableType);

     SgInitializedName* associateVariable = new SgInitializedName(variableName,variableType,initializer);

  // printf ("In c_action_association(): associateVariable = %p = %s \n",associateVariable,associateVariable->get_name().str());

     astNodeStack.push_front(associateVariable);

     SgScopeStatement* currentScope = getTopOfScopeStack();

     associateVariable->set_parent(currentScope);
  // associateVariable->set_scope(currentScope);

  // This has not been set yet (it will be set in buildVariableDeclaration())
  // ROSE_ASSERT(associateVariable->get_symbol_from_symbol_table() != NULL);

  // This will be handled by the buildVariableDeclaration() function.
  // SgVariableSymbol* variableSymbol = new SgVariableSymbol(associateVariable);
  // currentScope->insert_symbol(variableName,variableSymbol);
  // Make sure we can find the newly added symbol!
  // ROSE_ASSERT(currentScope->lookup_variable_symbol(variableName) != NULL);

  // Set the position of the declaration to be the first use of the implicitly defined variable.
  // setSourcePosition(implicitVariable,getTopOfNameStack());
     setSourcePosition(associateVariable,id);

  // printf ("Calling buildVariableDeclaration in c_action_association() \n");
     bool buildingImplicitVariable = false;
     SgVariableDeclaration* variableDeclaration = buildVariableDeclaration(NULL,buildingImplicitVariable);
  // printf ("DONE: Calling buildVariableDeclaration in c_action_association() \n");

     ROSE_ASSERT(variableDeclaration->get_file_info()->isCompilerGenerated() == false);

     ROSE_ASSERT(associateVariable->get_scope() != NULL);
     ROSE_ASSERT(associateVariable->get_scope() == currentScope);

     astNodeStack.push_front(variableDeclaration);

  // Clean up the stacks
  // astTypeStack.pop_front();
  // astInitializerStack.pop_front();

  // This takes a name off of the name stack and puts a variable reference onto the astExpressionStack 
  // (building a declaration if required) types are computed using implicit type rules.
  // c_action_data_ref(0);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R818 c_action_association()");
#endif

  // printf ("Exiting at end of rule R818 \n");
  // ROSE_ASSERT(false);
   }

/**
 * R819
 * selector
 *
 */
void c_action_selector()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_selector() \n");
   }


/** R820
 * end_associate_stmt
 *
 * : (label)? T_END T_ASSOCIATE (T_IDENT)? T_EOS
 * | (label)? T_ENDASSOCIATE  (T_IDENT)? T_EOS
 *
 * @param label The label.
 * @param id Identifier if present. Otherwise, null.
 */
// void c_action_end_associate_stmt(Token_t * label, Token_t * id)
void c_action_end_associate_stmt(Token_t *label, Token_t *endKeyword, Token_t *associateKeyword, Token_t *id, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_end_associate_stmt(): label = %p id = %p = %s \n",label,id,id != NULL ? id->text : "NULL");

  // DQ (12/12/2007): After implimenting the support for the select and case statements
  // it made more sense to pop the scope in R801 instead of here.
  // Pop off the body
  // astScopeStack.pop_front();

  // DQ (10/10/2010): Test ending position
     ROSE_ASSERT(astScopeStack.empty() == false);
     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct() != NULL);

     ROSE_ASSERT(endKeyword != NULL);
     resetEndingSourcePosition(astScopeStack.front(),endKeyword);

     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct()->get_line() != astScopeStack.front()->get_startOfConstruct()->get_line());

  // Pop off the SgAssociateStatement (which is a scope, derived from SgScopeStatement)
     astScopeStack.pop_front();

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R820 c_action_end_associate_stmt()");
#endif
   }

/** R822
 * select_type_stmt
 *
 *
 * @param label The label.
 * @param selectConstructName Name of the select construct.
 * @param associateName Name of association.
 */
void c_action_select_type(Token_t *selectKeyword, Token_t *typeKeyword)
{
}

// void c_action_select_type_stmt(Token_t * label, Token_t * selectConstructName, Token_t * associateName)
void c_action_select_type_stmt(Token_t *label, Token_t *selectConstructName, Token_t *associateName, Token_t *eos)
{
}

/** R823
 * type_guard_stmt
 *
 * :  (label)? T_TYPE T_IDENT T_LPAREN type_spec T_RPAREN (T_IDENT)? T_EOS
 * |  (label)? T_CLASS T_IDENT T_LPAREN type_spec T_RPAREN (T_IDENT)? T_EOS
 * |  (label)? T_CLASS  T_DEFAULT (T_IDENT)? T_EOS
 *
 * @param label The label.
 * @param selectConstructName Optional identifier immediately before end.
 */
// void c_action_type_guard_stmt(Token_t * label, Token_t * selectConstructName)
void c_action_type_guard_stmt(Token_t *label, Token_t *typeKeyword, Token_t *isOrDefaultKeyword, Token_t *selectConstructName, Token_t *eos)
{
}

/** R824
 * end_select_type_stmt
 *
 * :    (label )? T_END T_SELECT ( T_IDENT )? T_EOS
 * |    (label )? T_ENDSELECT   ( T_IDENT )? T_EOS
 *
 * @param label The label.
 * @param id The identifier, if present. Otherwise null.
 */
// void c_action_end_select_type_stmt(Token_t * label, Token_t * id)
void c_action_end_select_type_stmt(Token_t *label, Token_t *endKeyword, Token_t *selectKeyword, Token_t *id, Token_t *eos)
   {

  // DQ (10/10/2010): Test ending position
     ROSE_ASSERT(astScopeStack.empty() == false);
     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct() != NULL);
     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct()->get_line() != astScopeStack.front()->get_startOfConstruct()->get_line());
   }

/**
 * R825
 * do_construct
 *
 */
void c_action_do_construct()
   {
   }

/**
 * R826
 * block_do_construct
 * 
 */
void c_action_block_do_construct()
   {
   }

/** R827
 * do_stmt
 *
 *
 * @param label The label.
 * @param id Identifier for do construct name, if present. Otherwise, null.
 * @param digitString The value of the digit stringi, if there. Otherwise, null.
 * @param hasLoopControl True if there is a loop control.
 */
// void c_action_do_stmt(Token_t * label, Token_t * id, Token_t * digitString, ofp_bool hasLoopControl)
void c_action_do_stmt(Token_t *label, Token_t *id, Token_t *doKeyword, Token_t *digitString, Token_t *eos, ofp_bool hasLoopControl)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
        {
          printf ("In c_action_do_stmt(): hasLoopControl = %s \n",hasLoopControl ? "true" : "false");
          printf ("   label       = %p = %s \n",label,(label != NULL) ? label->text : "NULL");
          printf ("   id          = %p = %s \n",id,(id != NULL) ? id->text : "NULL");
          printf ("   doKeyword   = %p = %s \n",doKeyword,(doKeyword != NULL) ? doKeyword->text : "NULL");
          printf ("   digitString = %p = %s \n",digitString,(digitString != NULL) ? digitString->text : "NULL");
          printf ("   eos         = %p = %s \n",eos,(eos != NULL) ? eos->text : "NULL");
        }

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R827 c_action_do_stmt()");
#endif

  // At this point for "DO I = 1, 100" the expressions for "I", "1", and "100" are on the astExpressionStack.

     bool buildWhileLoop           = false;

     SgExpression* indexExpression = NULL;
     SgExpression* endingIndex     = NULL;
     SgExpression* stride          = NULL;
     SgExpression* predicate       = NULL;
     if (hasLoopControl == true)
        {
       // We have a "DO I=1,100" style loop

#if ROSE_OFP_MINOR_VERSION_NUMBER >= 8 & ROSE_OFP_PATCH_VERSION_NUMBER >= 2
          if (astExpressionStack.size() == 3)
#else
          if (astExpressionStack.size() == 4)
#endif
             {
            // There is a stride expression on the top of the stack
               stride = astExpressionStack.front();
               astExpressionStack.pop_front();
             }
            else
             {
            // stride = new SgNullExpression();
            // setSourcePosition(stride);
             }

#if ROSE_OFP_MINOR_VERSION_NUMBER >= 8 & ROSE_OFP_PATCH_VERSION_NUMBER >= 2
          if (astExpressionStack.size() == 2)
#else
          if (astExpressionStack.size() == 3)
#endif
             {
            // There is a ending index, starting, index, and the index variable on the stack
               endingIndex = astExpressionStack.front();
               astExpressionStack.pop_front();

               SgExpression* startingIndex = astExpressionStack.front();
               astExpressionStack.pop_front();

#if ROSE_OFP_MINOR_VERSION_NUMBER >= 8 & ROSE_OFP_PATCH_VERSION_NUMBER >= 2
               Token_t* variableToken = astNameStack.front();
               ROSE_ASSERT(astNameStack.empty() == false);
               SgName variableName = variableToken->text;
            // printf ("Generating reference for index variable name = %s \n",variableName.str());
               astNameStack.pop_front();

            // DQ (1/18/2011): This detects where we have used the semantics of implicitly building symbols for implicit variables.
            // printf ("WARNING: This use of trace_back_through_parent_scopes_lookup_variable_symbol() used the side effect of building a symbol if the reference is not found! \n");
            // ROSE_ASSERT(false);

               SgVariableSymbol* variableSymbol = trace_back_through_parent_scopes_lookup_variable_symbol(variableName, astScopeStack.front());
            // ROSE_ASSERT(variableSymbol != NULL);
               if (variableSymbol == NULL)
                  {
                 // DQ (1/19/2011): Build the implicit variable
                    buildImplicitVariableDeclaration(variableName);

                 // Now verify that it is present.
                    variableSymbol = trace_back_through_parent_scopes_lookup_variable_symbol(variableName,astScopeStack.front());
                    ROSE_ASSERT(variableSymbol != NULL);
                  }
               ROSE_ASSERT(variableSymbol != NULL);
               SgExpression* index = new SgVarRefExp(variableSymbol);
               setSourcePosition(index,variableToken);
#else
               ROSE_ASSERT(astExpressionStack.empty() == false);
               SgExpression* index = astExpressionStack.front();
               astExpressionStack.pop_front();
#endif
            // The index expression is the "I=1" part (which is built so that we can hand it to the SgFortranDo constructor)
               indexExpression = new SgAssignOp(index,startingIndex,NULL);

#if ROSE_OFP_MINOR_VERSION_NUMBER >= 8 & ROSE_OFP_PATCH_VERSION_NUMBER >= 2
            // index->set_parent(indexExpression);
#endif
            // Note that the "=" does not exist as a token in OFP
            // setSourcePosition(indexExpression);
               ROSE_ASSERT(doKeyword != NULL);
               setSourcePosition(indexExpression,doKeyword);

            // DQ (11/16/2007): I think that stack should be empty now!
               ROSE_ASSERT(astExpressionStack.empty() == true);
             }
            else
             {
#if ROSE_OFP_MINOR_VERSION_NUMBER >= 8 & ROSE_OFP_PATCH_VERSION_NUMBER >= 2
               if (astExpressionStack.size() == 1)
#else
               if (astExpressionStack.size() == 1)
#endif
                  {
                 // This is the case of a "DO WHILE" loop, with the condition on the astExpressionStack...
                    buildWhileLoop = true;
                    predicate = astExpressionStack.front();
                    astExpressionStack.pop_front();
                  }
                 else
                  {
#if 1
                 // Output debugging information about saved state (stack) information.
                    outputState("Error in R827 c_action_do_stmt()");
#endif
                    printf ("Error: astExpressionStack.size() incorrect for do_stmt astExpressionStack.size() = %zu \n",astExpressionStack.size());
                    ROSE_ASSERT(false);
                  }
             }

#if 1
       // Output debugging information about saved state (stack) information.
          outputState("Middle of R827 c_action_do_stmt()");
#endif
       // DQ (11/16/2007): I think that stack should be empty now!
          ROSE_ASSERT(astExpressionStack.empty() == true);
        }
       else
        {
       // The stack size in this case should be zero!
          ROSE_ASSERT(astExpressionStack.empty() == true);

       // We have a simpler "DO" style loop, there are no expressions on the stack to get
          indexExpression = new SgNullExpression();
          endingIndex     = new SgNullExpression();
          stride          = new SgNullExpression();

          setSourcePosition(indexExpression);
          setSourcePosition(endingIndex);
          setSourcePosition(stride);
        }

     SgBasicBlock* body = new SgBasicBlock();
     ROSE_ASSERT(body != NULL);

  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     body->setCaseInsensitive(true);

     setSourcePosition(body);

  // SgStatement* loopStatement = NULL;
     SgScopeStatement* loopStatement = NULL;

  // printf ("buildWhileLoop = %s \n",buildWhileLoop ? "true" : "false");
     if (buildWhileLoop == true)
        {
          ROSE_ASSERT(indexExpression == NULL);
          ROSE_ASSERT(endingIndex == NULL);
          ROSE_ASSERT(stride == NULL);

          ROSE_ASSERT(predicate != NULL);
          SgExprStatement* expressionStatement = new SgExprStatement(predicate);

       // setSourcePosition(expressionStatement,doKeyword);
          setSourcePosition(expressionStatement);
          resetSourcePosition(expressionStatement,predicate);

          SgWhileStmt* whileStatement = new SgWhileStmt(expressionStatement,body);

       // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
          whileStatement->setCaseInsensitive(true);

       // ROSE_ASSERT(body->get_parent() == whileStatement);
          body->set_parent(whileStatement);

       // DQ (11/17/2007): Added support for numeric labels
          setStatementNumericLabel(whileStatement,label);

       // DQ (11/17/2007): Added support for string labels
          setStatementStringLabel(whileStatement,id);

          setStatementEndNumericLabel(whileStatement,digitString);
          loopStatement = whileStatement;
        }
       else
        {
          ROSE_ASSERT(predicate == NULL);

       // Use valid pointers to SgNullExpression when nothing was present in the source code.
          if (stride == NULL)
             {
               stride = new SgNullExpression();
               setSourcePosition(stride);
             }

       // SgFortranDo* fortranDo = new SgFortranDo(index,startingIndex,endingIndex,stride,body);
          SgFortranDo* fortranDo = new SgFortranDo(indexExpression,endingIndex,stride,body);

       // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
          fortranDo->setCaseInsensitive(true);

       // ROSE_ASSERT(body->get_parent() == fortranDo);
          body->set_parent(fortranDo);

       // DQ (12/26/2007): This field is depricated in favor of the has_end_statement boolean field used uniformally in several IR nodes).
       // DQ (12/24/2007): Default to true and then if we see the c_action_end_do_stmt we can set it to false.
       // fortranDo->set_old_style(true);

       // printf ("In R827 c_action_do_stmt(): fortranDo = %p body = %p \n",fortranDo,body);

       // outputState("At stage 1 of R827 c_action_do_stmt()");
       // DQ (11/17/2007): Added support for numeric labels
          setStatementNumericLabel(fortranDo,label);

       // outputState("At stage 2 of R827 c_action_do_stmt()");

       // DQ (11/17/2007): Added support for string labels
          setStatementStringLabel(fortranDo,id);

       // fortranDo->set_scope(astScopeStack.front());
       // body->set_scope(fortranDo);

       // outputState("At stage 3 of R827 c_action_do_stmt()");
          setStatementEndNumericLabel(fortranDo,digitString);
       // outputState("At stage 4 of R827 c_action_do_stmt()");
          loopStatement = fortranDo;
        }

     ROSE_ASSERT(loopStatement != NULL);

     setSourcePosition(loopStatement,doKeyword);
     body->set_parent(loopStatement);
  // Liao 2/1/2011, set the body's endOfConstruct to be the same as the loop statement's endOfConstruct
  // We should not use the endOfConstruct of fortran loop here since c_action_end_do_stmt()
  // will call resetEndingSourcePosition() to change it later on.
  //   body->set_endOfConstruct (new Sg_File_Info(*(loopStatement->get_endOfConstruct())));
  //   body->get_endOfConstruct()->set_line(6);

     astScopeStack.front()->append_statement(loopStatement);

  // Set or clear the astLabelSymbolStack (since it is redundant with the label being passed in)
  // There are two mechanisms used to set labels, and we never know which will be used by OFP.
     setStatementNumericLabelUsingStack(loopStatement);

     ROSE_ASSERT(loopStatement->get_parent() == astScopeStack.front());

  // DQ (12/25/2007): Made the FortranDo a SgScopeStatement to be uniform with other loop constructs.
  // So now we want to push the loopStatement onto the astScopeStack.
     astScopeStack.push_front(loopStatement);
     astScopeStack.push_front(body);

     ROSE_ASSERT(body->get_parent() == loopStatement);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R827 c_action_do_stmt()");
#endif
   }

/** R828
 * label_do_stmt
 *
 * :  (label)? ( T_IDENT T_COLON )? 
 *              T_DO T_DIGIT_STRING ( loop_control )? T_EOS
 *
 * @param label The label.
 * @param id Identifier for do construct name, if present. Otherwise, null.
 * @param hasLoopControl True if there is a loop control.
 */
// void c_action_label_do_stmt(Token_t * label, Token_t * id, ofp_bool hasLoopControl)
void c_action_label_do_stmt(Token_t *label, Token_t *id, Token_t *doKeyword, Token_t *digitString, Token_t *eos, ofp_bool hasLoopControl)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_label_do_stmt(): hasLoopControl = %s \n",hasLoopControl ? "true" : "false");

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R828 c_action_label_do_stmt()");
#endif
   }

/**
 * R830
 * loop_control
 *
 * @param whileKeyword T_WHILE or null.
 * @param hasOptExpr Flag specifying if optional expression was given.  
 * This only applies for alternative 2 of the rule.
 */
#if ROSE_OFP_MINOR_VERSION_NUMBER >= 8 & ROSE_OFP_PATCH_VERSION_NUMBER >= 0
void c_action_loop_control(Token_t * whileKeyword, int doConstructType, ofp_bool hasOptExpr)
#else
void c_action_loop_control(Token_t * whileKeyword, ofp_bool hasOptExpr)
#endif
   {
  // This is the case of a "DO WHILE" (this rule communicates the "WHILE" part)
  // However, we trigger the construction of a SgWhileStmt IR node instead of a 
  // SgFortranDo IR node by looking at the stack depth (one is for "DO WHILE").
  // The alternative would be to set and unset special state variables, which 
  // I would like to avoid.

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
        {
#if ROSE_OFP_MINOR_VERSION_NUMBER >= 8 & ROSE_OFP_PATCH_VERSION_NUMBER >= 0
          printf ("In c_action_loop_control(): whileKeyword = %p = %s doConstructType = %d hasOptExpr = %s \n",whileKeyword,whileKeyword != NULL ? whileKeyword->text : "NULL",doConstructType,hasOptExpr ? "true" : "false");
#else
          printf ("In c_action_loop_control(): whileKeyword = %p = %s hasOptExpr = %s \n",whileKeyword,whileKeyword != NULL ? whileKeyword->text : "NULL",hasOptExpr ? "true" : "false");
#endif
        }
   }

/**
 * R831
 * do_variable
 * do_variable is scalar-int-variable-name
 *
 */
#if ROSE_OFP_MINOR_VERSION_NUMBER >= 8 & ROSE_OFP_PATCH_VERSION_NUMBER >= 2
void c_action_do_variable(Token_t *id)
   {
  // This identifies the do loop index variable, but at this point it is on the astExpressionStack already.
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
        printf ("In c_action_do_variable() id = %s \n",(id != NULL) ? id->text : "NULL");

  // DQ (9/27/2010): Push this so that we can have it on the stack and determine the name of the 
  // variable in the implied-do-control loop. This is also used in the c_action_do_stmt.
     astNameStack.push_front(id);
   }
#else
void c_action_do_variable()
   {
  // This identifies the do loop index variable, but at this point it is on the astExpressionStack already.
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_do_variable() \n");
   }
#endif

/**
 * R833
 * end_do
 *
 */
void c_action_end_do()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_end_do() \n");

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R833 c_action_end_do()");
#endif

  // DQ (12/12/2007): After implimenting the support for the select and case statements
  // it made more sense to pop the scope in R801 instead of here.

  // Pop the stack here, since the c_action_end_do_stmt() will only be called for "DO" statement that uses 
  // an "END DO" and not a "CONTINUE" statement.  There are at least two ways to close of a Fortran "DO" loop.
  // This function is called for each of the two ways which close off the Fortran "DO" loop.
  // astScopeStack.pop_front();

  // DQ (10/10/2010): Test ending position
     ROSE_ASSERT(astScopeStack.empty() == false);
     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct() != NULL);

#if 0
  // ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct()->get_line() != astScopeStack.front()->get_startOfConstruct()->get_line());
     if (astScopeStack.front()->get_endOfConstruct()->get_line() != astScopeStack.front()->get_startOfConstruct()->get_line())
        {
          printf ("In c_action_end_do(): astScopeStack.front()->get_startOfConstruct()->get_line() = %d \n",astScopeStack.front()->get_startOfConstruct()->get_line());
          printf ("In c_action_end_do(): astScopeStack.front()->get_endOfConstruct()->get_line()   = %d \n",astScopeStack.front()->get_endOfConstruct()->get_line());
          printf ("WARNING: In c_action_end_do() -- astScopeStack.front()->get_endOfConstruct()->get_line() != astScopeStack.front()->get_startOfConstruct()->get_line() \n");
        }
#endif

  // Pop off the loop construct's scope (it is a scope in C/C++, even if not in Fortran)
  // treating it as a scope will allow it to be consistent across C,C++, and Fortran.
     ROSE_ASSERT(astScopeStack.empty() == false);

  // printf ("NOTE: Moved call to pop astScopeStack from R833 c_action_end_do() both R834 c_action_end_do_stmt() and R838 c_action_do_term_action_stmt() \n");
  // astScopeStack.pop_front();

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R833 c_action_end_do()");
#endif
   }

/** R834
 * end_do_stmt
 *
 * : (label)? T_END T_DO ( T_IDENT )? T_EOS
 * | (label)? T_ENDDO   ( T_IDENT )? T_EOS
 *
 * @param label The label.
 * @param id The do construct name, if present. Otherwise, null.
 */
// void c_action_end_do_stmt(Token_t * label, Token_t * id)
void c_action_end_do_stmt(Token_t *label, Token_t *endKeyword, Token_t *doKeyword, Token_t *id, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R834 c_action_end_do_stmt() (popping the scope) \n");

     if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
        {
       // Note that if doKeyword == NULL then the endKeyword containes the shorted form "ENDDO"
       // Also eos contains the "\n" at the end of the line
          printf ("   label      = %p = %s \n",label,label != NULL ? label->text : "NULL");
          printf ("   endKeyword = %p = %s \n",endKeyword,endKeyword != NULL ? endKeyword->text : "NULL");
          printf ("   doKeyword  = %p = %s \n",doKeyword,doKeyword != NULL ? doKeyword->text : "NULL");
          printf ("   id         = %p = %s \n",id,id != NULL ? id->text : "NULL");
          printf ("   eos        = %p = \"%s\" \n",eos,eos != NULL ? eos->text : "NULL");
          printf ("End of token output! \n");
        }

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R834 c_action_end_do_stmt()");
#endif

  // printf ("***** Calling markDoLoopAsUsingEndDo() from c_action_end_do_stmt() \n");
     markDoLoopAsUsingEndDo();

  // printf ("In c_action_end_do_stmt(): We should pop the astScopeStack here instead of in c_action_end_do() \n");

  // DQ (10/10/2010): Test ending position
     ROSE_ASSERT(astScopeStack.empty() == false);
     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct() != NULL);

  // DQ (10/10/2010): Moved the poping of the stack to this function so that we could set the source end position of the scope.
     ROSE_ASSERT(endKeyword != NULL);
     resetEndingSourcePosition(astScopeStack.front(),endKeyword);
     
     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct()->get_line() != astScopeStack.front()->get_startOfConstruct()->get_line());

     setStatementNumericLabel(astScopeStack.front(),label);

  // DQ (12/7/2010): We left a lable on the astLabelSymbolStack, see test2007_76.f90 (with R213 stack empty rule enforced).
     processLabelOnStack(astScopeStack.front());

     astScopeStack.pop_front();

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R834 c_action_end_do_stmt()");
#endif
   }

/** R838
 * do_term_action_stmt
 *
 * Try requiring an action_stmt and then we can simply insert the new
 * T_LABEL_DO_TERMINAL during the Sale's prepass.  T_EOS is in action_stmt.
 * added the T_END T_DO and T_ENDDO options to this rule because of the
 * token T_LABEL_DO_TERMINAL that is inserted if they end a labeled DO.
 *
 * :  label T_LABEL_DO_TERMINAL 
 *              (action_stmt | ( (T_END T_DO | T_ENDDO) (T_IDENT)? ) T_EOS)
 *
 * @param label The label, which must be present.
 * @param id The identifier, if present. Otherwise, null.
 */
// void c_action_do_term_action_stmt(Token_t * label, Token_t * id)
void c_action_do_term_action_stmt(Token_t *label, Token_t *endKeyword, Token_t *doKeyword, Token_t *id, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R838 c_action_do_term_action_stmt() \n");

  // This is repeated code from R834
     if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
        {
       // Note that if doKeyword == NULL then the endKeyword containes the shorted form "ENDDO"
       // Also eos contains the "\n" at the end of the line
          printf ("   label      = %p = %s \n",label,label != NULL ? label->text : "NULL");
          printf ("   endKeyword = %p = %s \n",endKeyword,endKeyword != NULL ? endKeyword->text : "NULL");
          printf ("   doKeyword  = %p = %s \n",doKeyword,doKeyword != NULL ? doKeyword->text : "NULL");
          printf ("   id         = %p = %s \n",id,id != NULL ? id->text : "NULL");
          printf ("   eos        = %p = \"%s\" \n",eos,eos != NULL ? eos->text : "NULL");
          printf ("End of token output! \n");
        }

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R838 c_action_do_term_action_stmt()");
#endif

  // This rule can be called even where the end-do does not exist (in this case, the endKeyword == NULL).
     if (endKeyword != NULL)
        {
       // There is a valid "end do" statement in the source code, so mark this as a do statement using the new (F90) syntax.
       // printf ("***** Calling markDoLoopAsUsingEndDo() from c_action_do_term_action_stmt() \n");
          markDoLoopAsUsingEndDo();

       // DQ (10/10/2010): Moved the poping of the stack to this function so that we could set the source end position of the scope.
          ROSE_ASSERT(endKeyword != NULL);
          resetEndingSourcePosition(astScopeStack.front(),endKeyword);
        }
       else
        {
       // These was no "end do" statement in the source code, even though this rule was called by the parser.
          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
               printf ("***** WARNING, c_action_do_term_action_stmt() called, yet no 'end do' exists in the source code \n");
        }

     if (endKeyword == NULL && label != NULL)
        {
       // DQ (10/10/2010): This is the backup when endKeyword == NULL
          resetEndingSourcePosition(astScopeStack.front(),label);
        }

  // DQ (10/10/2010): Test ending position
     ROSE_ASSERT(astScopeStack.empty() == false);
     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct() != NULL);

     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct()->get_line() != astScopeStack.front()->get_startOfConstruct()->get_line());

  // Note that it CORRECT to set the label after the scope is popped.
     astScopeStack.pop_front();

     setStatementNumericLabel(astScopeStack.front(),label);

  // DQ (12/7/2010): We left a lable on the astLabelSymbolStack, see test2007_76.f90 (with R213 stack empty rule enforced).
     processLabelOnStack(astScopeStack.front());

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R838 c_action_do_term_action_stmt()");
#endif

  // printf ("Should this label be set before or after astScopeStack.pop_front() \n");
  // ROSE_ASSERT(false);
   }

/** R843
 * cycle_stmt
 *      :       (label)? T_CYCLE (T_IDENT)? T_EOS
 * 
 * T_IDENT inlined for do_construct_name
 * 
 * @param label Optional statement label
 * @param id Optional do-construct-name
 */
// void c_action_cycle_stmt(Token_t * label, Token_t * id)
void c_action_cycle_stmt(Token_t *label, Token_t *cycleKeyword, Token_t *id, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_cycle_stmt() label = %p = %s id = %p = %s \n",label,label != NULL ? label->text : "NULL",id,id != NULL ? id->text : "NULL");

     SgContinueStmt* continueStatement = new SgContinueStmt();

     if (id != NULL)
        {
          continueStatement->set_do_string_label(id->text);
        }

     setSourcePosition(continueStatement,cycleKeyword);

     astScopeStack.front()->append_statement(continueStatement);
   }

/** R844
 * exit_stmt
 *      :       (label)? T_EXIT (T_IDENT)? T_EOS
 *
 * T_IDENT inlined for do_construct_name
 * 
 * @param label Optional statement label
 * @param id Optional do-construct-name
 */
// void c_action_exit_stmt(Token_t * label, Token_t * id)
void c_action_exit_stmt(Token_t *label, Token_t *exitKeyword, Token_t *id, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_exit_stmt() label = %p = %s id = %p = %s \n",label,label != NULL ? label->text : "NULL",id,id != NULL ? id->text : "NULL");

     SgBreakStmt* breakStatement = new SgBreakStmt();

     if (id != NULL)
        {
          breakStatement->set_do_string_label(id->text);
        }

     setSourcePosition(breakStatement,exitKeyword);

     astScopeStack.front()->append_statement(breakStatement);
   }

/** R845
 * goto_stmt
 *      :       t_go_to label T_EOS
 *
 * @param label The branch target statement label
 */
// void c_action_goto_stmt(Token_t * label)
void c_action_goto_stmt(Token_t *goKeyword, Token_t *toKeyword, Token_t *label, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_goto_stmt() label = %p = %s goKeyword = %p = %s toKeyword = %p = %s \n",
            // label,label != NULL ? label->text : "NULL",
               goKeyword,goKeyword != NULL ? goKeyword->text : "NULL",
               toKeyword,toKeyword != NULL ? toKeyword->text : "NULL",
               label,label != NULL ? label->text : "NULL");

     ROSE_ASSERT(label != NULL);
     SgLabelSymbol* label_symbol = buildNumericLabelSymbol(label);
     ROSE_ASSERT(label_symbol != NULL);

  // This takes a SgStatement as a label, but that is being replaced to take a SgLabelSymbol.
     SgLabelStatement* labelStatement = NULL;
     SgGotoStatement* gotoStatement = new SgGotoStatement(labelStatement);

  // Set the generated SgLabelSymbol
  // gotoStatement->set_label_symbol(label_symbol);

     SgLabelRefExp* labelRefExp = new SgLabelRefExp(label_symbol);
     gotoStatement->set_label_expression(labelRefExp);
     labelRefExp->set_parent(gotoStatement);
     setSourcePosition(labelRefExp,label);

     ROSE_ASSERT(goKeyword != NULL);
     setSourcePosition(gotoStatement,goKeyword);

  // When this statement can handle a numericl label (on the statement itself) then 
  // uncomment this line.  This is an OFP bug that was reported 12/20/2007.
  // setStatementNumericLabel(gotoStatement,label);

     astScopeStack.front()->append_statement(gotoStatement);

  // printf ("CLEAR THE astLabelSymbolStack (c_action_goto_stmt) \n");
     astLabelSymbolStack.clear();

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R845 c_action_goto_stmt()");
#endif
   }

/** R846
 * computed_goto_stmt
 *      :       (label)? t_go_to T_LPAREN label_list T_RPAREN (T_COMMA)? expr T_EOS
 *
 * ERR_CHK 846 scalar_int_expr replaced by expr
 * 
 * @param label Optional statement label
 */
// void c_action_computed_goto_stmt(Token_t * label)
void c_action_computed_goto_stmt(Token_t *label, Token_t *goKeyword, Token_t *toKeyword, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_computed_goto_stmt() label = %p = %s goKeyword = %p = %s toKeyword = %p = %s \n",
               label,label != NULL ? label->text : "NULL",
               goKeyword,goKeyword != NULL ? goKeyword->text : "NULL",
               toKeyword,toKeyword != NULL ? toKeyword->text : "NULL");

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R846 c_action_computed_goto_stmt()");
#endif

  // DQ (12/30/2007): This is now modified due to the introduction of the SgLabelRefExp.
  // SgLabelSymbolPtrList labelList;
     SgExprListExp* labelList = new SgExprListExp();
     setSourcePosition(labelList);

  // printf ("astLabelSymbolStack.size() = %zu \n",astLabelSymbolStack.size());
     while (astLabelSymbolStack.empty() == false)
        {
       // labelList.insert(labelList.begin(),astLabelSymbolStack.front());
          SgLabelRefExp* labelRefExp = new SgLabelRefExp(astLabelSymbolStack.front());
          setSourcePosition(labelRefExp);
          labelList->prepend_expression(labelRefExp);

       // Clear off the top of the stack
          astLabelSymbolStack.pop_front();

#if OFP_LABEL_BUG
       // DQ (2/18/2008): I think this might be fixed now!
       // Due to a bug in OFP, where labels are processed twice by R313, we have to pop two elements from the stack.
          ROSE_ASSERT(astLabelSymbolStack.empty() == false);
          astLabelSymbolStack.pop_front();
#endif
        }

     ROSE_ASSERT(astExpressionStack.empty() == false);
     SgExpression* index_expression = astExpressionStack.front();
     astExpressionStack.pop_front();

     SgComputedGotoStatement* computedGoto = new SgComputedGotoStatement(labelList,index_expression);

  // At least the goKeyword should be a valid pointer.
     ROSE_ASSERT(goKeyword != NULL);
     setSourcePosition(computedGoto,goKeyword);

     astScopeStack.front()->append_statement(computedGoto);

  // printf ("CLEAR THE astLabelSymbolStack (c_action_computed_goto_stmt) \n");
     astLabelSymbolStack.clear();

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R846 c_action_computed_goto_stmt()");
#endif
   }

/**
 * assign_stmt
 *
 * @param label1 Optional statement label.
 * @param assignKeyword T_ASSIGN token.
 * @param label2 Required label for assign_stmt.
 * @param toKeyword T_TO token.
 * @param name T_IDENT for name subrule.
 * @param eos T_EOS token.
 * Note: This is a deleted feature.
 */
void c_action_assign_stmt(Token_t* label1, Token_t* assignKeyword, Token_t* label2, Token_t* toKeyword, Token_t* name, Token_t* eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_assign_stmt(): label1 = %p = %s goKeyword = %p = %s label2 = %p = %s toKeyword = %p = %s name = %p = %s \n",
               label1,label1 != NULL ? label1->text : "NULL",
               assignKeyword,assignKeyword != NULL ? assignKeyword->text : "NULL",
               label2,label2 != NULL ? label2->text : "NULL",
               toKeyword,toKeyword != NULL ? toKeyword->text : "NULL",
               name,name != NULL ? name->text : "NULL");

     printf ("Assign statement not implemented (very old langauge feature) \n");
     ROSE_ASSERT(false);
   }

/**
 * assigned_goto_stmt
 *
 * @param label Optional statement label.
 * @param goKeyword T_GO or T_GOTO token.
 * @param toKeyword T_TO token if given; null otherwise.
 * @param name T_IDENT token for name subrule.
 * @param eos T_EOS token.
 * Note: This is a deleted feature.
 */
void c_action_assigned_goto_stmt(Token_t* label, Token_t* goKeyword, Token_t* toKeyword, Token_t* name, Token_t* eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_assigned_goto_stmt(): label = %p = %s goKeyword = %p = %s toKeyword = %p = %s name = %p = %s \n",
               label,label != NULL ? label->text : "NULL",
               goKeyword,goKeyword != NULL ? goKeyword->text : "NULL",
               toKeyword,toKeyword != NULL ? toKeyword->text : "NULL",
               name,name != NULL ? name->text : "NULL");

     printf ("Assigned GOTO statement not implemented (very old langauge feature) \n");
     ROSE_ASSERT(false);
   }


/**
 * Unknown rule.
 * stmt_label_list
 *
 */
void c_action_stmt_label_list()
{
}


/**
 * pause_stmt
 *
 * @param label Optional statement label.
 * @param pauseKeyword T_PAUSE token.
 * @param constant T_DIGIT_STRING or null if is a char_literal_constant.
 * @param eos T_EOS token.
 * Note: This is a deleted feature.
 */
void c_action_pause_stmt(Token_t* label, Token_t* pauseKeyword, Token_t* constant, Token_t* eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_pause_stmt(): label = %p = %s pauseKeyword = %p = %s constant = %p = %s \n",
               label,label != NULL ? label->text : "NULL",
               pauseKeyword,pauseKeyword != NULL ? pauseKeyword->text : "NULL",
               constant,constant != NULL ? constant->text : "NULL");

     build_implicit_program_statement_if_required();

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R(_unknown_) c_action_pause_stmt()");
#endif

     SgExpression* pauseExpression = NULL;
     if (astExpressionStack.empty() == false)
        {
       // ROSE_ASSERT(astExpressionStack.empty() == false);
          pauseExpression = astExpressionStack.front();
          astExpressionStack.pop_front();
       // pauseStatement->set_code(pauseExpression);
        }
       else
        {
          pauseExpression = new SgNullExpression();
          setSourcePosition(pauseExpression);
        }

     SgStopOrPauseStatement* pauseStatement = new SgStopOrPauseStatement(pauseExpression);

     pauseStatement->set_stop_or_pause(SgStopOrPauseStatement::e_pause);

     if (constant != NULL)
        {
       // Note that this constant is also mistakenly interpreted as a label by OFP and so 
       // we end up with a label matching the constant.  This should be fixed in OFP.
       // See test2010_134.f90 (with error: Duplicate statement label 10 at (1) and (2)).
          long  value = atol(constant->text);
       // SgLongIntVal* constantValue = new SgLongIntVal(value,constant->text);
          SgLongIntVal* constantValue = SageBuilder::buildLongIntVal(value);
          pauseStatement->set_code(constantValue);
          constantValue->set_parent(pauseStatement);
        }

     setSourcePosition(pauseStatement,pauseKeyword);

     setStatementNumericLabel(pauseStatement,label);

     processLabelOnStack(pauseStatement);

     ROSE_ASSERT(astScopeStack.empty() == false);
     astScopeStack.front()->append_statement(pauseStatement);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R(_unknown_) c_action_pause_stmt()");
#endif
   }


/** R847
 * arithmetic_if_stmt
 *      :       (label)? T_ARITHMETIC_IF_STMT T_IF
 *              T_LPAREN expr T_RPAREN label T_COMMA label T_COMMA label T_EOS
 *
 * ERR_CHK 847 scalar_numeric_expr replaced by expr
 * 
 * @param label  Optional statement label
 * @param label1 The first branch target statement label
 * @param label2 The second branch target statement label
 * @param label3 The third branch target statement label
 */
// void c_action_arithmetic_if_stmt(Token_t * label, Token_t * label1, Token_t * label2, Token_t * label3)
void c_action_arithmetic_if_stmt(Token_t *label, Token_t *ifKeyword, Token_t *label1, Token_t *label2, Token_t *label3, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_arithmetic_if_stmt() label = %p = %s label = %p = %s label = %p = %s label = %p = %s label = %p = %s \n",
               label,label != NULL ? label->text : "NULL",
               ifKeyword,ifKeyword != NULL ? ifKeyword->text : "NULL",
               label1,label1 != NULL ? label1->text : "NULL",
               label2,label2 != NULL ? label2->text : "NULL",
               label3,label3 != NULL ? label3->text : "NULL");

     ROSE_ASSERT(ifKeyword != NULL);
     ROSE_ASSERT(label1 != NULL);
     ROSE_ASSERT(label2 != NULL);
     ROSE_ASSERT(label3 != NULL);

     SgExpression* condition = astExpressionStack.front();
     astExpressionStack.pop_front();

     SgLabelSymbol* less_label_symbol    = buildNumericLabelSymbol(label1);
     ROSE_ASSERT(less_label_symbol != NULL);

     SgLabelSymbol* equal_label_symbol   = buildNumericLabelSymbol(label2);
     ROSE_ASSERT(equal_label_symbol != NULL);

     SgLabelSymbol* greater_label_symbol = buildNumericLabelSymbol(label3);
     ROSE_ASSERT(greater_label_symbol != NULL);

     SgLabelRefExp* labelRefExp_1 = new SgLabelRefExp(less_label_symbol);
     SgLabelRefExp* labelRefExp_2 = new SgLabelRefExp(equal_label_symbol);
     SgLabelRefExp* labelRefExp_3 = new SgLabelRefExp(greater_label_symbol);

     SgArithmeticIfStatement* arithmeticIf = new SgArithmeticIfStatement(condition,labelRefExp_1,labelRefExp_2,labelRefExp_3);

     labelRefExp_1->set_parent(arithmeticIf);
     setSourcePosition(labelRefExp_1,label1);
     labelRefExp_2->set_parent(arithmeticIf);
     setSourcePosition(labelRefExp_2,label1);
     labelRefExp_3->set_parent(arithmeticIf);
     setSourcePosition(labelRefExp_3,label1);

     setSourcePosition(arithmeticIf,ifKeyword);
     setStatementNumericLabel(arithmeticIf,label);

     ROSE_ASSERT(astScopeStack.empty() == false);
     astScopeStack.front()->append_statement(arithmeticIf);

  // printf ("CLEAR THE astLabelSymbolStack (c_action_arithmetic_if_stmt) \n");
     astLabelSymbolStack.clear();

  // printf ("c_action_arithmetic_if_stmt() not implemented \n");
  // ROSE_ASSERT(false);
   }

/** R848
 * continue_stmt
 *      :       (label)? T_CONTINUE
 * 
 * @param label  Optional statement label
 */
// void c_action_continue_stmt(Token_t * label)
void c_action_continue_stmt(Token_t *label, Token_t *continueKeyword, Token_t *eos)
   {
  // A Fortran "CONTINUE" statement is mapped to a C "label" statement, so we use the existing SgLabelStatement IR node.

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_continue_stmt(): label = %p = %s continueKeyword = %p = %s \n",label,label != NULL ? label->text : "NULL",continueKeyword,continueKeyword != NULL ? continueKeyword->text : "NULL");

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R848 c_action_continue_stmt()");
#endif

     SgName name = label != NULL ? label->text : "";
  // printf ("In c_action_continue_stmt(): name of SgLabelStatement = %s \n",name.str());
     SgLabelStatement* labelStatement = new SgLabelStatement(name,NULL);

     ROSE_ASSERT(continueKeyword != NULL);
     setSourcePosition(labelStatement,continueKeyword);
  // setSourcePosition(labelStatement);

  // setStatementNumericLabel(labelStatement,label);
  // printf ("In c_action_continue_stmt(): label = %p \n",label);
     if (label != NULL)
        {
          setStatementNumericLabel(labelStatement,label);
        }
       else
        {
       // printf ("In c_action_continue_stmt(): astLabelSymbolStack.empty() = %s \n",astLabelSymbolStack.empty() ? "true" : "false");
          if (astLabelSymbolStack.empty() == false)
             {
               SgLabelSymbol *labelSymbolFromStack = astLabelSymbolStack.front();
               astLabelSymbolStack.pop_front();

            // Mark this as a regular numerical label (in columns 2-6) and not associated with an else statement or an end statement.
               labelSymbolFromStack->set_label_type(SgLabelSymbol::e_start_label_type);

               SgLabelRefExp* labelRefExp = new SgLabelRefExp(labelSymbolFromStack);
               labelStatement->set_numeric_label(labelRefExp);
               labelRefExp->set_parent(labelStatement);
               setSourcePosition(labelRefExp);

            // Reset the statement referenced by this label (was previously referenced to a 
            // statement which referenced it but was not that statement's numerical_label).
               labelSymbolFromStack->set_fortran_statement(labelStatement);
             }
            else
             {
            // printf ("Warning: No label found in argument to c_action_continue_stmt() and astLabelSymbolStack.empty() == true \n");
             }
        }

  // DQ (12/9/2007): set the scope.
  // labelStatement->set_scope(astScopeStack.front());
     SgFunctionDefinition* currentFunctionScope = TransformationSupport::getFunctionDefinition(astScopeStack.front());
     ROSE_ASSERT(currentFunctionScope != NULL);
     labelStatement->set_scope(currentFunctionScope);
     ROSE_ASSERT(labelStatement->get_scope() != NULL);

     ROSE_ASSERT(astScopeStack.empty() == false);
     astScopeStack.front()->append_statement(labelStatement);

  // DQ (12/16/2007): A label had been pushed onto the stack, but it is redundant with the label provided as a token.
     if (label != NULL)
        {
       // outputState("Before astNameStack.pop_front() in R848 c_action_continue_stmt()");
          ROSE_ASSERT(astLabelSymbolStack.empty() == false);
          astLabelSymbolStack.pop_front();
        }
       else
        {
       // The label is not always passed into the continue statement
          if (astLabelSymbolStack.empty() == false)
             {
               if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                    printf ("Warning: found a LabelSymbol on the stack, but it was not passed into the c_action_continue_stmt() \n");

               astLabelSymbolStack.pop_front();
             }
        }

  // DQ (10/10/2010): Mark the end of the do loop scope using the continueKeyword token.
     ROSE_ASSERT(continueKeyword != NULL);
     resetEndingSourcePosition(astScopeStack.front(),continueKeyword);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R848 c_action_continue_stmt()");
#endif
   }

/** R849
 * stop_stmt
 *      :       (label)? T_STOP (stop_code)? T_EOS
 *
 *@param label Optional statement label
 *@param hasStopCode True if the stop-code is present, false otherwise
 */
// void c_action_stop_stmt(Token_t * label, ofp_bool hasStopCode)
void c_action_stop_stmt(Token_t *label, Token_t *stopKeyword, Token_t *eos, ofp_bool hasStopCode)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_stop_stmt(): label = %p = %s stopKeyword = %p = %s \n",
               label,label != NULL ? label->text : "NULL",stopKeyword,stopKeyword != NULL ? stopKeyword->text : "NULL");

     build_implicit_program_statement_if_required();

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R849 c_action_stop_stmt()");
#endif

     SgExpression* stopExpression = NULL;
     if (astExpressionStack.empty() == false)
        {
       // ROSE_ASSERT(astExpressionStack.empty() == false);
          stopExpression = astExpressionStack.front();
          astExpressionStack.pop_front();
       // pauseStatement->set_code(pauseExpression);
       // setSourcePosition(stopExpression);
        }
       else
        {
          stopExpression = new SgNullExpression();
          setSourcePosition(stopExpression);
        }

     SgStopOrPauseStatement* stopStatement = new SgStopOrPauseStatement(stopExpression);

     stopStatement->set_stop_or_pause(SgStopOrPauseStatement::e_stop);

     setSourcePosition(stopStatement,stopKeyword);

     ROSE_ASSERT(astScopeStack.empty() == false);
     astScopeStack.front()->append_statement(stopStatement);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R849 c_action_stop_stmt()");
#endif
   }

/** R850
 * stop_code
 *      : scalar_char_constant
 *      | T_DIGIT_STRING
 * 
 * ERR_CHK 850 T_DIGIT_STRING must be 5 digits or less
 * 
 * @param digitString The stop-code token, otherwise is a scalar-char-constant
 */
void c_action_stop_code(Token_t * digitString)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_stop_code(): digitString = %p = %s \n",digitString,digitString != NULL ? digitString->text : "NULL");

     if (digitString != NULL)
        {
          SgStringVal* stringValue = new SgStringVal(digitString->text);

          setSourcePosition(stringValue,digitString);

          astExpressionStack.push_front(stringValue);
        }
   }

/*
 * R856-F08 allstop-stmt
 *   is ALL STOP [ stop-code ]
 *   :  (label)? T_ALL T_STOP (stop_code)? T_EOS
 *
 * @param label Optional statement label
 * @param allKeyword T_ALL token.
 * @param stopKeyword T_STOP token.
 * @param eos T_EOS token.
 * @param hasStopCode True if the stop-code is present, false otherwise
 *
 * New v0.7.2
 */
void c_action_allstop_stmt(Token_t * label, Token_t * allKeyword,
                           Token_t * stopKeyword, Token_t * eos, ofp_bool hasStopCode)
   {
     printf ("In c_action_allstop_stmt() - this function needs to be implemented.\n");
   }

/*
 * R858-F08 sync-all-stmt
 *   is SYNC ALL [([ sync-stat-list ])]
 *   :  (label)? T_SYNC T_ALL (sync_stat_list)? T_EOS
 *
 * @param label Optional statement label
 * @param syncKeyword T_SYNC token.
 * @param allKeyword T_ALL token.
 * @param eos T_EOS token.
 * @param hasStopCode True if the sync-stat-list is present, false otherwise
 *
 * New v0.7.2
 */
void c_action_sync_all_stmt(Token_t * label, Token_t * syncKeyword,
                            Token_t * allKeyword, Token_t * eos, ofp_bool hasStopCode)
{
   printf ("In c_action_sync_all_stmt() - this function needs to be implemented.\n");
}

/*
 * R859-F08 sync-stat
 *   is STAT = stat-variable
 *   or ERRMSG = errmsg-variable
 *   :  T_IDENT                 // {'STAT','ERRMSG'} are variables}
 *              T_EQUALS expr   // expr is a stat-variable or an errmsg-variable
 *
 * @param syncStat Identifier representing {'STAT','ERRMSG'}
 *
 * New v0.7.2
 */
void c_action_sync_stat(Token_t * syncStat)
{
   printf ("In c_action_sync_stat() - this function needs to be implemented.\n");
}

/*
 * sync_stat_list__begin - helper function, see R859-F08 sync-stat
 *
 * New v0.7.2
 */
void c_action_sync_stat_list__begin()
{
   printf ("In c_action_sync_stat_list__begin() - this function needs to be implemented.\n");
}

/*
 * sync_stat_list - helper function, see R859-F08 sync-stat
 *   :    sync_stat ( T_COMMA sync_stat )*
 * 
 * @param count The number of items in the list.
 *
 * New v0.7.2
 */
void c_action_sync_stat_list(int count)
{
   printf ("In c_action_sync_stat_list() - this function needs to be implemented.\n");
}

/**
 * Unknown rule.
 * scalar_char_constant
 *
 */
void c_action_scalar_char_constant()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_scalar_char_constant() \n");

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R(_unknown_) c_action_scalar_char_constant()");
#endif
   }


/**
 * R901
 * io_unit
 *
 */
void c_action_io_unit()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_file_io_unit() \n");
   }

/**
 * R902
 * file_unit_number
 *
 */
void c_action_file_unit_number()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_file_unit_number() \n");

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R902 c_action_file_unit_number()");
#endif

     if (astExpressionStack.empty() == true)
        {
          printf ("In c_action_file_unit_number(): Unclear what to do in this case (empty astExpressionStack)! \n");
          ROSE_ASSERT(false);

       // If there was nothing pushed to the stack, then push the SgAsteriskShapeExp ???
       // I think we are just borrowing the fact that this expression will output a "*" and 
       // perhaps we should have a specialized expression more specific to format semantics.
          SgExpression* asteriskExpression = new SgAsteriskShapeExp();
          setSourcePosition(asteriskExpression);
          astExpressionStack.push_front(asteriskExpression);

          push_token("unit");
        }
       else
        {
       // Note that if this is a scalar-logical-expression, then the expression is likely from a if-stmt and not a part of a format statement.
          SgExpression* expressionOnStack = astExpressionStack.front();
       // Using a switch statement allows us to more cleanly handle a growing number of special cases.
       // printf ("expressionOnStack = %p = %s \n",expressionOnStack,expressionOnStack->class_name().c_str());
          switch(expressionOnStack->variantT())
             {
            // Handle case of format label (integer): print 1, N
               case V_SgIntVal:
                  {
                    push_token("unit");
                    break;
                  }

            // Handle case of format string (passed as a variable):  CHARACTER(len=*) :: fmtstr;  PRINT fmtstr, str (see test2010_125.f90).
               case V_SgVarRefExp:
                  {
                 // printf ("expressionOnStack->get_type() = %p = %s \n",expressionOnStack->get_type(),expressionOnStack->get_type()->class_name().c_str());
                    SgTypeInt* intType = isSgTypeInt(expressionOnStack->get_type());
                    if (intType != NULL)
                       {
                         push_token("unit");
                       }
                      else
                       {
                         printf ("In c_action_file_unit_number(): Unclear what to do in this case! \n");
                         ROSE_ASSERT(false);
                       }
                    
                    break;
                  }

               default:
                  {
                 // Add not "fmt" token.
                    printf ("Non integer scalars can NOT be used in format statements so ignore this (no fmt pushed onto astNameStack). \n");

                    printf ("In c_action_file_unit_number(): Unclear what to do in this case (default)! \n");
                    ROSE_ASSERT(false);
                  }
             }
        }

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R902 c_action_file_unit_number()");
#endif
   }

/** R904
 * open_stmt
 *
 * : (label)? T_OPEN T_LPAREN connect_spec_list T_RPAREN T_EOS
 *
 * @param label The label.
 */
// void c_action_open_stmt(Token_t * label)
void c_action_open_stmt(Token_t *label, Token_t *openKeyword, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_open_stmt(): label = %p = %s openKeyword = %p = %s \n",
               label,label != NULL ? label->text : "NULL",openKeyword,openKeyword != NULL ? openKeyword->text : "NULL");

     SgOpenStatement* openStatement = new SgOpenStatement();
     setSourcePosition(openStatement,openKeyword);

     SgExpression* expression = NULL;
     Token_t* name = NULL;

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R904 c_action_open_stmt()");
#endif

  // printf ("Warning: Ignoring all but the 'unit' in the OpenStatement \n");

  // DQ (12/11/2010): There can be more expressions on the astExpressionStack than names on the astNameStack,
  // but only the number of names on the astNameStack count.  See test2010_143.f90.
  // while (astExpressionStack.empty() == false)
     while (astNameStack.empty() == false)
        {
          ROSE_ASSERT(astNameStack.empty() == false);

          expression = astExpressionStack.front();
          name = astNameStack.front();

          ROSE_ASSERT(name != NULL);
          string nameString = name->text;

       // ROSE_ASSERT(current_IO_Control_Spec != NULL);

       // We don't need the current_IO_Control_Spec data structure in the code below.

       // The "unit=" string is optional, if it was not present then a toekn was pushed onto the stack with the text value "defaultString"
          if ( strncasecmp(name->text,"unit",4) == 0 || strncmp(name->text,"defaultString",13) == 0)
             {
               openStatement->set_unit(expression);
             }
          else if ( strncasecmp(name->text,"iostat",6) == 0 )
             {
               openStatement->set_iostat(expression);
             }
          else if ( strncasecmp(name->text,"err",3) == 0 )
             {
            // openStatement->set_err(expression);
               ROSE_ASSERT(expression != NULL);
               SgExpression* labelRefExp = buildLabelRefExp(expression);
               ROSE_ASSERT(labelRefExp != NULL);
               openStatement->set_err(labelRefExp);
               labelRefExp->set_parent(openStatement);
             }
          else if ( strncasecmp(name->text,"file",4) == 0 )
             {
               openStatement->set_file(expression);
             }
          else if ( strncasecmp(name->text,"status",6) == 0 )
             {
               openStatement->set_status(expression);
             }
          else if ( strncasecmp(name->text,"access",6) == 0 )
             {
               openStatement->set_access(expression);
             }
          else if ( strncasecmp(name->text,"form",4) == 0 )
             {
               openStatement->set_form(expression);
             }
          else if ( strncasecmp(name->text,"recl",4) == 0 )
             {
               openStatement->set_recl(expression);
             }
          else if ( strncasecmp(name->text,"blank",5) == 0 )
             {
               openStatement->set_blank(expression);
             }
          else if ( strncasecmp(name->text,"position",8) == 0 )
             {
               openStatement->set_position(expression);
             }
          else if ( strncasecmp(name->text,"action",6) == 0 )
             {
               openStatement->set_action(expression);
             }
          else if ( strncasecmp(name->text,"delim",5) == 0 )
             {
               openStatement->set_delim(expression);
             }
          else if ( strncasecmp(name->text,"pad",3) == 0 )
             {
               openStatement->set_pad(expression);
             }
          else if ( strncasecmp(name->text,"iomsg",5) == 0 )
             {
               openStatement->set_iomsg(expression);
             }
          else if ( strncasecmp(name->text,"round",5) == 0 )
             {
               openStatement->set_round(expression);
             }
          else if ( strncasecmp(name->text,"sign",4) == 0 )
             {
               openStatement->set_sign(expression);
             }
          else if ( strncasecmp(name->text,"asynchronous",12) == 0 )
             {
               openStatement->set_asynchronous(expression);
             }

          astNameStack.pop_front();
          astExpressionStack.pop_front();

          expression->set_parent(openStatement);
        }

     openStatement->set_unit(expression);
     expression->set_parent(openStatement);

     astScopeStack.front()->append_statement(openStatement);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R904 c_action_open_stmt()");
#endif
   }

/**
 * R905
 * connect_spec
 *
 * @param id T_IDENT token for second alternative; otherwise null.
 */
void c_action_connect_spec(Token_t *id)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_connect_spec(): id = %p = %s \n",id,id != NULL ? id->text : "NULL");

  // ROSE_ASSERT(id != NULL);
     if (id != NULL)
        {
          astNameStack.push_front(id);
        }
       else
        {
       // This is the case of an option not being specified, as in "open(1)" instead of "open(UNIT=1)"
       // To make the astExpressionStack match the astNameStack we have to push a default token onto the astNameStack.
          Token_t* defaultToken = create_token(0,0,0,"defaultString");
          ROSE_ASSERT(defaultToken != NULL);
          astNameStack.push_front(defaultToken);
        }
   }

/** R905 list
 * connect_spec_list
 *      :       connect_spec ( T_COMMA connect_spec )*
 * 
 * @param count The number of items in the list.
 */
void c_action_connect_spec_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_connect_spec_list__begin() \n");
   }

void c_action_connect_spec_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_connect_spec_list() \n");
   }

/** R908
 * close_stmt
 *
 * :  (label)? T_CLOSE T_LPAREN close_spec_list T_RPAREN T_EOS
 *
 * @param label The label.
 */
// void c_action_close_stmt(Token_t * label)
void c_action_close_stmt(Token_t *label, Token_t *closeKeyword, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_close_stmt(): label = %p = %s closeKeyword = %p = %s \n",
               label,label != NULL ? label->text : "NULL",closeKeyword,closeKeyword != NULL ? closeKeyword->text : "NULL");

     SgCloseStatement* closeStatement = new SgCloseStatement();
     ROSE_ASSERT(closeStatement != NULL);

  // DQ (10/10/2010): Set the source position using the closeKeyword
  // setSourcePosition(closeStatement);
     ROSE_ASSERT(closeKeyword != NULL);
     setSourcePosition(closeStatement,closeKeyword);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R908 c_action_close_stmt()");
#endif

  // DQ (9/6/2010): Switch to using the astNameStack since there can be an additional 
  // expression on the astExpressionStack due to an if stmt predicate (see test2010_40.f90).
  // printf ("Warning: Ignoring all but the 'unit' in the OpenStatement \n");
  // while (astExpressionStack.empty() == false)
     while (astNameStack.empty() == false)
        {
          ROSE_ASSERT(astNameStack.empty() == false);
          ROSE_ASSERT(astExpressionStack.empty() == false);

          SgExpression* expression = astExpressionStack.front();
          Token_t* name = astNameStack.front();

       // We don't need the current_IO_Control_Spec data structure in the code below.

       // The "unit=" string is optional, if it was not present then a toekn was pushed onto the stack with the text value "defaultString"
          if ( strncasecmp(name->text,"unit",4) == 0 || strncmp(name->text,"defaultString",13) == 0)
             {
               closeStatement->set_unit(expression);
             }
          else if ( strncasecmp(name->text,"iostat",6) == 0 )
             {
               closeStatement->set_iostat(expression);
             }
          else if ( strncasecmp(name->text,"err",3) == 0 )
             {
            // closeStatement->set_err(expression);
               ROSE_ASSERT(expression != NULL);
               SgExpression* labelRefExp = buildLabelRefExp(expression);
               ROSE_ASSERT(labelRefExp != NULL);
               closeStatement->set_err(labelRefExp);
               labelRefExp->set_parent(closeStatement);
             }
          else if ( strncasecmp(name->text,"status",6) == 0 )
             {
               closeStatement->set_status(expression);
             }
          else if ( strncasecmp(name->text,"iomsg",5) == 0 )
             {
               closeStatement->set_iomsg(expression);
             }

          astNameStack.pop_front();
          astExpressionStack.pop_front();

          expression->set_parent(closeStatement);
        }

     setStatementNumericLabel(closeStatement,label);

  // DQ (12/8/2010): Added label handling support for the redundant label stack.
     processLabelOnStack(closeStatement);

     astScopeStack.front()->append_statement(closeStatement);
   }

/**
 * R909
 * close_spec
 *
 * @param closeSpec T_IDENT for second alternative; null otherwise.
 */
void c_action_close_spec(Token_t *closeSpec)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_close_spec(): closeSpec = %p = %s \n",closeSpec,closeSpec != NULL ? closeSpec->text : "NULL");

     if (closeSpec != NULL)
        {
          astNameStack.push_front(closeSpec);
        }
       else
        {
       // This is the case of an option not being specified, as in "close(1)" instead of "close(UNIT=1)"
       // To make the astExpressionStack match the astNameStack we have to push a default token onto the astNameStack.
          Token_t* defaultToken = create_token(0,0,0,"defaultString");
          ROSE_ASSERT(defaultToken != NULL);
          astNameStack.push_front(defaultToken);
        }
   }

/** R909 list
 * close_spec_list
 *      :       close_spec ( T_COMMA close_spec )*
 * 
 * @param count The number of items in the list.
 */
void c_action_close_spec_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_close_spec_list__begin() \n");
   }
void c_action_close_spec_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_close_spec_list(): count = %d \n",count);
   }

/** R910
 * read_stmt
 *
 * :  ((label)? T_READ T_LPAREN) => (label)? T_READ T_LPAREN 
 *                      io_control_spec_list T_RPAREN ( input_item_list )? T_EOS
 * |  ((label)? T_READ) => (label)? T_READ format 
 *                      ( T_COMMA input_item_list )? T_EOS
 *
 * @param label The label.
 * @param hasInputItemList True if has an input item list.
 */
// void c_action_read_stmt(Token_t * label, ofp_bool hasInputItemList)
void c_action_read_stmt(Token_t *label, Token_t *readKeyword, Token_t *eos, ofp_bool hasInputItemList)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_read_stmt(): label = %p = %s readKeyword = %p = %s hasInputItemList = %s \n",
               label,label != NULL ? label->text : "NULL",readKeyword,readKeyword != NULL ? readKeyword->text : "NULL",hasInputItemList ? "true" : "false");

     SgReadStatement* readStatement = new SgReadStatement();
     ROSE_ASSERT(readStatement != NULL);
     setSourcePosition(readStatement,readKeyword);

     setStatementNumericLabel(readStatement,label);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R910 c_action_read_stmt()");
#endif

  // DQ (12/19/2007): This is a more uniform handling of the SgExprListExp (computed in R915)
  // SgExprListExp* exprListExp = isSgExprListExp(astExpressionStack.front());
     SgExprListExp* exprListExp = NULL;
     if (astExpressionStack.empty() == false)
        {
       // DQ (9/6/2010): This is used in test2007_109.f90
          exprListExp = isSgExprListExp(astExpressionStack.front());

       // DQ (9/6/2010): Handle the case of empty input-item-list (see test2010_40.f90)
       // astExpressionStack.pop_front();
       // ROSE_ASSERT(exprListExp != NULL);
          if (exprListExp == NULL)
             {
               exprListExp = new SgExprListExp();
               setSourcePosition(exprListExp);
             }
            else
             {
               astExpressionStack.pop_front();
               ROSE_ASSERT(exprListExp != NULL);
             }
        }
       else
        {
       // DQ (12/30/2007): This is the trivial case where no input items have been specified (e.g. "read *")
          exprListExp = new SgExprListExp();
          setSourcePosition(exprListExp);
        }

     ROSE_ASSERT(exprListExp != NULL);
          readStatement->set_io_stmt_list(exprListExp);
     exprListExp->set_parent(readStatement);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("After processing the numberOfVariables in R910 c_action_read_stmt()");
#endif

  // If there is only one entry then it is the unit, not the format.
     int initalStackDepth = astExpressionStack.size();

  // printf ("In R910 c_action_read_stmt(): initalStackDepth = %d \n",initalStackDepth);

  // If this list is empty then there was nothing specified, as reported by OFP, but this is the case of "read *, ..." so we will interpret this as the case of "read fmt=*. ..." or "read *, ..."
     bool availableFormatSpecifier = astNameStack.empty();
     if (availableFormatSpecifier == true)
        {
       // There was no option recorded by OFP, so this means that "*" was used (OFP bug, I think).
       // So push the asterisk, and record it as the fmt option.
          SgAsteriskShapeExp* asterisk = new SgAsteriskShapeExp();
          setSourcePosition(asterisk);

          astExpressionStack.push_front(asterisk);

       // This is the case of an option not being specified, as in "read(1)" instead of "read(UNIT=1)"
       // To make the astExpressionStack match the astNameStack we have to push a default token onto the astNameStack.
          Token_t* defaultToken = create_token(0,0,0,"fmt");
          ROSE_ASSERT(defaultToken != NULL);
          astNameStack.push_front(defaultToken);
        }

  // astExpressionStack.pop_front();
  // printf ("Warning: Ignoring all but the 'unit' in the OpenStatement \n");
  // while (astExpressionStack.empty() == false)
     while (astNameStack.empty() == false)
        {
       // ROSE_ASSERT(astNameStack.empty() == false);
          ROSE_ASSERT(astExpressionStack.empty() == false);

          SgExpression* expression = astExpressionStack.front();
          Token_t* name = astNameStack.front();

       // Pop these off immediately, so that we look ahead in the stack!
          astNameStack.pop_front();
          astExpressionStack.pop_front();

          Token_t* lookAheadName = NULL;
          if (astNameStack.empty() == false)
             {
            // This is required for cases such as: "read (1,asynchronous='YES') array", see test2007_123.f03.
               lookAheadName = astNameStack.front();
             }
       // printf ("In c_action_read_stmt(): processing token = %s with expression = %p = %s lookAheadName = %p \n",name->text,expression,expression->class_name().c_str(),lookAheadName);

#if 1
       // Output debugging information about saved state (stack) information.
          outputState("In loop over io-control-list in R910 c_action_read_stmt()");
#endif

       // We don't need the current_IO_Control_Spec data structure in the code below.

       // The "unit=" string is optional, if it was not present then a token was pushed onto the stack with the text value "defaultString"
       // if ( (strncasecmp(name->text,"fmt",3) == 0) || (strncmp(name->text,"defaultString",13) == 0) && (readStatement->get_format() == NULL) )
       // if ( (strncasecmp(name->text,"fmt",3) == 0) || ( (strncmp(name->text,"defaultString",13) == 0) && (readStatement->get_format() == NULL) && (initalStackDepth >= 2) ) )
          if ( (strncasecmp(name->text,"fmt",3) == 0) || ( (strncmp(name->text,"defaultString",13) == 0) && (readStatement->get_format() == NULL) && (initalStackDepth >= 2) && (lookAheadName != NULL && strncmp(lookAheadName->text,"defaultString",13) == 0)) )
             {
            // printf ("Processing token = %s as format spec \n",name->text);
            // DQ (12/3/2010): This code fails for test2007_211.f.
               ROSE_ASSERT(expression != NULL);
               SgExpression* labelRefExp = buildLabelRefExp(expression);
               ROSE_ASSERT(labelRefExp != NULL);
            // printf ("In c_action_read_stmt(): readStatement->set_format(%p = %s) \n",labelRefExp,labelRefExp->class_name().c_str());
               readStatement->set_format(labelRefExp);
               labelRefExp->set_parent(readStatement);

               ROSE_ASSERT(expression->get_parent() != NULL);
             }
       // Process this second because the unit expression is deeper on the stack!
          else if ( (strncasecmp(name->text,"unit",4) == 0) || (strncmp(name->text,"defaultString",13) == 0) && (readStatement->get_unit() == NULL) )
             {
            // printf ("Processing token = %s as unit spec expression = %s \n",name->text,expression->class_name().c_str());
               readStatement->set_unit(expression);
             }
          else if ( strncasecmp(name->text,"iostat",6) == 0 )
             {
            // printf ("Processing token = %s as iostat spec expression = %s \n",name->text,expression->class_name().c_str());
               readStatement->set_iostat(expression);
             }
          else if ( strncasecmp(name->text,"err",3) == 0 )
             {
            // readStatement->set_err(expression);
               ROSE_ASSERT(expression != NULL);
               SgExpression* labelRefExp = buildLabelRefExp(expression);
               ROSE_ASSERT(labelRefExp != NULL);
               readStatement->set_err(labelRefExp);
               labelRefExp->set_parent(readStatement);
             }
          else if ( strncasecmp(name->text,"rec",3) == 0 )
             {
               readStatement->set_rec(expression);
             }
          else if ( strncasecmp(name->text,"iomsg",5) == 0 )
             {
               readStatement->set_iomsg(expression);
             }
          else if ( strncasecmp(name->text,"end",3) == 0 )
             {
            // readStatement->set_end(expression);
               ROSE_ASSERT(expression != NULL);
               SgExpression* labelRefExp = buildLabelRefExp(expression);
               ROSE_ASSERT(labelRefExp != NULL);
               readStatement->set_end(labelRefExp);
               labelRefExp->set_parent(readStatement);
             }
          else if ( strncasecmp(name->text,"nml",3) == 0 )
             {
               readStatement->set_namelist(expression);
             }
          else if ( strncasecmp(name->text,"advance",7) == 0 )
             {
               readStatement->set_advance(expression);
             }
          else if ( strncasecmp(name->text,"size",4) == 0 )
             {
               readStatement->set_size(expression);
             }
          else if ( strncasecmp(name->text,"eor",3) == 0 )
             {
               ROSE_ASSERT(expression != NULL);
               SgExpression* labelRefExp = buildLabelRefExp(expression);
               ROSE_ASSERT(labelRefExp != NULL);
               readStatement->set_eor(labelRefExp);
               labelRefExp->set_parent(readStatement);
             }
          else if ( strncasecmp(name->text,"asynchronous",12) == 0 )
             {
               readStatement->set_asynchronous(expression);
             }

          expression->set_parent(readStatement);

#if 1
       // Output debugging information about saved state (stack) information.
          outputState("BOTTOM of loop over io-control-list in R910 c_action_read_stmt()");
#endif
        }

     ROSE_ASSERT(readStatement != NULL);

     setStatementNumericLabel(readStatement,label);

     processLabelOnStack(readStatement);

  // ROSE_ASSERT(readStatement->get_format() != NULL);
  // printf ("At BOTTOM of R910 c_action_read_stmt(): FMT = %p = %s \n",readStatement->get_format(),readStatement->get_format()->class_name().c_str());

     astScopeStack.front()->append_statement(readStatement);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R910 c_action_read_stmt()");
#endif
   }

/** R911
 * write_stmt
 *      :       (label)? T_WRITE T_LPAREN io_control_spec_list T_RPAREN 
 *                      (output_item_list)? T_EOS
 *
 * @param label The statement label
 * @param hasOutputItemList True if output-item-list is present
 */
// void c_action_write_stmt(Token_t * label, ofp_bool hasOutputItemList)
void c_action_write_stmt(Token_t *label, Token_t *writeKeyword, Token_t *eos, ofp_bool hasOutputItemList)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_write_stmt(): label = %p = %s writeKeyword = %p = %s hasOutputItemList = %s \n",
               label,label != NULL ? label->text : "NULL",writeKeyword,writeKeyword != NULL ? writeKeyword->text : "NULL",hasOutputItemList ? "true" : "false");
#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R911 c_action_write_stmt()");
#endif

     SgWriteStatement* writeStatement = new SgWriteStatement();
     ROSE_ASSERT(writeStatement != NULL);
  // setSourcePosition(writeStatement);
     ROSE_ASSERT(writeKeyword != NULL);
     setSourcePosition(writeStatement,writeKeyword);

     setStatementNumericLabel(writeStatement,label);

     writeStatement->set_io_statement(SgIOStatement::e_write);

  // DQ (12/19/2007): This is a more uniform handling of the SgExprListExp (computed in R915)
     SgExprListExp* exprListExp = isSgExprListExp(astExpressionStack.front());

     if (exprListExp == NULL)
        {
       // This is a case where there is nothing to write (see test2007_221.f)
       // so make an empty list.
          exprListExp = new SgExprListExp();
          setSourcePosition(exprListExp);
        }
       else
        {
       // If we got the SgExprListExp from the stack then we pop the stack (but only then)
          astExpressionStack.pop_front();
        }

     ROSE_ASSERT(exprListExp != NULL);

          writeStatement->set_io_stmt_list(exprListExp);
     exprListExp->set_parent(writeStatement);

  // If there is only one entry then it is the unit, not the format.
  // int initalStackDepth = astExpressionStack.size();
     int numberOfDefaultOptions = 0;
     AstNameListType::iterator i = astNameStack.begin();
     while (i != astNameStack.end())
        {
          Token_t* tmp_name = *i;
          bool isDefaultString = (strncmp(tmp_name->text,"defaultString",13) == 0);
          if (isDefaultString)
               numberOfDefaultOptions++;
          i++;
        }

  // printf ("Number of default options on stack = %d \n",numberOfDefaultOptions);
     ROSE_ASSERT(numberOfDefaultOptions >= 0);
     ROSE_ASSERT(numberOfDefaultOptions <= 2);

  // astExpressionStack.pop_front();
  // printf ("Warning: Ignoring all but the 'unit' in the OpenStatement \n");
  // while (astExpressionStack.empty() == false)
     while (astNameStack.empty() == false)
        {
       // ROSE_ASSERT(astNameStack.empty() == false);
          ROSE_ASSERT(astExpressionStack.empty() == false);

          SgExpression* expression = astExpressionStack.front();
          Token_t* name = astNameStack.front();

       // printf ("In c_action_write_stmt(): processing token = %s with expression = %p = %s \n",name->text,expression,expression->class_name().c_str());

       // We don't need the current_IO_Control_Spec data structure in the code below.

       // The "unit=" string is optional, if it was not present then a toekn was pushed onto the stack with the text value "defaultString"
       // if ( (strncasecmp(name->text,"fmt",3) == 0) || (strncmp(name->text,"defaultString",13) == 0) && (writeStatement->get_format() == NULL) )
       // if ( (strncasecmp(name->text,"fmt",3) == 0) || (strncmp(name->text,"defaultString",13) == 0) && (writeStatement->get_format() == NULL) && initalStackDepth >= 2)
          if ( (strncasecmp(name->text,"fmt",3) == 0) || (strncmp(name->text,"defaultString",13) == 0) && (writeStatement->get_format() == NULL) && numberOfDefaultOptions == 2)
             {
            // printf ("Processing token = %s as format spec \n",name->text);
            // writeStatement->set_format(expression);
               ROSE_ASSERT(expression != NULL);
               SgExpression* labelRefExp = buildLabelRefExp(expression);
               ROSE_ASSERT(labelRefExp != NULL);
               writeStatement->set_format(labelRefExp);
               labelRefExp->set_parent(writeStatement);
             }
       // Process this second because the unit expression is deeper on the stack!
          else if ( (strncasecmp(name->text,"unit",4) == 0) || (strncmp(name->text,"defaultString",13) == 0) && (writeStatement->get_unit() == NULL) )
             {
            // printf ("Processing token = %s as unit spec \n",name->text);
               writeStatement->set_unit(expression);
             }
          else if ( strncasecmp(name->text,"iostat",6) == 0 )
             {
               writeStatement->set_iostat(expression);
             }
          else if ( strncasecmp(name->text,"err",3) == 0 )
             {
            // writeStatement->set_err(expression);
               ROSE_ASSERT(expression != NULL);
               SgExpression* labelRefExp = buildLabelRefExp(expression);
               ROSE_ASSERT(labelRefExp != NULL);
               writeStatement->set_err(labelRefExp);
               labelRefExp->set_parent(writeStatement);
             }
          else if ( strncasecmp(name->text,"rec",3) == 0 )
             {
               writeStatement->set_rec(expression);
             }
          else if ( strncasecmp(name->text,"iomsg",5) == 0 )
             {
               writeStatement->set_iomsg(expression);
             }
          else if ( strncasecmp(name->text,"nml",3) == 0 )
             {
               writeStatement->set_namelist(expression);
             }
          else if ( strncasecmp(name->text,"advance",7) == 0 )
             {
               writeStatement->set_advance(expression);
             }
          else if ( strncasecmp(name->text,"asynchronous",12) == 0 )
             {
               writeStatement->set_asynchronous(expression);
             }

          astNameStack.pop_front();
          astExpressionStack.pop_front();

          expression->set_parent(writeStatement);
        }

     setStatementNumericLabel(writeStatement,label);

     processLabelOnStack(writeStatement);

     astScopeStack.front()->append_statement(writeStatement);

  // Set or clear the astLabelSymbolStack (since it is redundant with the label being passed in)
  // There are two mechanisms used to set labels, and we never know which will be used by OFP.
     setStatementNumericLabelUsingStack(writeStatement);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R911 c_action_write_stmt()");
#endif

  // Error checking: there may still be something on the stack from an  unhandled implied do loop
  // when we have a correctly handled implied do loop (there are three flavors) this should not be required.
     if (astExpressionStack.empty() == false)
        {
       // DQ (12/12/2010): Make this a warning only output within verbose mode.
          if ( SgProject::get_verbose() > 0 )
               printf ("WARNING: unfinished implied do loop expressions may be left on stack (or it may be from a c_action_if_stmt()) (write) \n");
        }
   }

/** R912
 * print_stmt
 *
 * :  (label)? T_PRINT format ( T_COMMA output_item_list )? T_EOS
 *
 * @param label The label.
 * @param hasOutputItemList True if output-item-list is present
 */
// void c_action_print_stmt(Token_t * label, ofp_bool hasOutputItemList)
void c_action_print_stmt(Token_t *label, Token_t *printKeyword, Token_t *eos, ofp_bool hasOutputItemList)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_print_stmt(): label = %p = %s printKeyword = %p = %s hasOutputItemList = %s \n",
               label,label != NULL ? label->text : "NULL",printKeyword,printKeyword != NULL ? printKeyword->text : "NULL",hasOutputItemList ? "true" : "false");
#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R912 c_action_print_stmt()");
#endif

     SgPrintStatement* printStatement = new SgPrintStatement();
     ROSE_ASSERT(printStatement != NULL);

  // setSourcePosition(printStatement);
     ROSE_ASSERT(printKeyword != NULL);

  // DQ (1/22/2008): Switched back to passing the printKeyword
     setSourcePosition(printStatement,printKeyword);
  // setSourcePosition(printStatement);

  // Set below in this function...
  // setStatementNumericLabel(printStatement,label);

  // ioStatement->set_io_statement(SgIOStatement::e_print);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R912 c_action_print_stmt()");
#endif

  // DQ (12/20/2007): Handle the case of print without an argument.
  // ROSE_ASSERT(astExpressionStack.empty() == false);
  // SgExprListExp* exprListExp = isSgExprListExp(astExpressionStack.front());
     SgExprListExp* exprListExp = NULL;
     if (astExpressionStack.empty() == true)
        {
          exprListExp = new SgExprListExp();
       // setSourcePosition(exprListExp);
          setSourcePosition(exprListExp,printKeyword);
        }
       else
        {
          ROSE_ASSERT(astExpressionStack.empty() == false);
          exprListExp = isSgExprListExp(astExpressionStack.front());

          resetSourcePosition(exprListExp,printStatement);
        }

     ROSE_ASSERT(exprListExp != NULL);
          printStatement->set_io_stmt_list(exprListExp);

     exprListExp->set_parent(printStatement);

     astExpressionStack.pop_front();

  // DQ (1/26/2009): test2009_07.f demonstrates that the astExpressionStack can have the
  // conditional expression from another part of the statement if this is an "if"
  // statement.

  // If this was "print *,i" then we would now have a "*" on the stack, but if
  // there was a format label then we will have a integer literal on the stack.

  // DQ (1/26/2009): Avoid getting things from the stack that are at least wrong to be used in a print format.
  // if (astExpressionStack.empty() == false)
  // if (astExpressionStack.empty() == false && isSgBinaryOp(astExpressionStack.front()) == NULL)
     if (astExpressionStack.empty() == false)
        {
          SgExpression* formatLabel = astExpressionStack.front();

       // DQ (1/26/2009): This should not be a binary operator (else it is likely an error).
       // Can be a string concatination operator which is a binary op (see test2010_124.f90).
       // ROSE_ASSERT(isSgBinaryOp(formatLabel) == NULL);

       // R914 should have pushed a "fmt" token onto the astNameStack
       // ROSE_ASSERT(astNameStack.empty() == false);
          if (astNameStack.empty() == false)
             {
               Token_t* name = astNameStack.front();

            // printf ("In c_action_print_stmt(): processing token = %s with expression = %p = %s \n",name->text,formatLabel,formatLabel->class_name().c_str());

               ROSE_ASSERT (strncasecmp(name->text,"fmt",3) == 0);

            // The "unit=" string is optional, if it was not present then a toekn was pushed onto the stack with the text value "defaultString"
            // if ( (strncasecmp(name->text,"fmt",3) == 0) || (strncmp(name->text,"defaultString",13) == 0) && (writeStatement->get_format() == NULL) && numberOfDefaultOptions == 2)
            // if (strncasecmp(name->text,"fmt",3) == 0)
            // printf ("Processing token = %s as format spec \n",name->text);

               SgExpression* labelRefExp = buildLabelRefExp(formatLabel);
               printStatement->set_format(labelRefExp);
               labelRefExp->set_parent(printStatement);

               astExpressionStack.pop_front();
               astNameStack.pop_front();
             }
        }

     setStatementNumericLabel(printStatement,label);

     processLabelOnStack(printStatement);

  // Attach the variableDeclaration to the current scope
     SgScopeStatement* currentScope = getTopOfScopeStack();
     ROSE_ASSERT(currentScope->variantT() == V_SgBasicBlock);
     currentScope->append_statement(printStatement);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("Before test of (astExpressionStack.empty() == false) in R912 c_action_print_stmt()");
#endif

  // DQ (1/26/2009): test2009_07.f shows the use of "if (7==8) print *, 'proc coords: ', 1, 2, 3"
  // for this case we can't clear the stack, let's see what other test codes fail if we 
  // skip clearing the stack after and implied do in a print statement.
     if (astExpressionStack.empty() == false)
        {
          printf ("WARNING: astExpressionStack still has %zu entries (Ok if used in a statement for example) (print) \n",astExpressionStack.size());
        }

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R912 c_action_print_stmt()");
#endif
   }

/** R913
 * io_control_spec
 *      :       expr
 *      |       T_ASTERISK
 *      |       T_IDENT // {'UNIT','FMT'}
 *                      T_EQUALS T_ASTERISK
 *      |       T_IDENT
 *              // {'UNIT','FMT'} are expr 'NML' is T_IDENT}
 *              // {'ADVANCE','ASYNCHRONOUS','BLANK','DECIMAL','DELIM'} are expr
 *              // {'END','EOR','ERR'} are labels
 *              // {'ID','IOMSG',IOSTAT','SIZE'} are variables
 *              // {'PAD','POS','REC','ROUND','SIGN'} are expr
 *              T_EQUALS expr
 *
 * ERR_CHK 913 check expr type with identifier
 * io_unit and format are both (expr|'*') so combined
 * 
 * @param hasExpression True if the io-control-spec has an expression
 * @param keyword Represents the keyword if present
 * @param hasAsterisk True if an '*' is present
 */
void c_action_io_control_spec(ofp_bool hasExpression, Token_t * keyword, ofp_bool hasAsterisk)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In R913 c_action_io_control_spec(): hasExpression = %s keyword = %p = %s hasAsterisk = %s \n",
               hasExpression ? "true" : "false",keyword,keyword != NULL ? keyword->text : "NULL",hasAsterisk ? "true" : "false");

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R913 c_action_io_control_spec()");
#endif

  // if (hasExpression == true)
     if (hasExpression == true)
        {
       // printf ("In c_action_io_control_spec() -- hasExpression == true: Need an example of this case before I can support it! \n");

       // This is the case of an option not being specified, as in "read(1)" instead of "read(UNIT=1)"
       // To make the astExpressionStack match the astNameStack we have to push a default token onto the astNameStack.
          if (keyword == NULL)
              {
                Token_t* defaultToken = create_token(0,0,0,"defaultString");
                ROSE_ASSERT(defaultToken != NULL);
                astNameStack.push_front(defaultToken);
              }
             else
              {
             // else use the correct name of the option specified
                astNameStack.push_front(keyword);
              }
          
       // ROSE_ASSERT(false);
        }

     if (hasAsterisk == true)
        {
          SgAsteriskShapeExp* asterisk = new SgAsteriskShapeExp();
          setSourcePosition(asterisk);

          astExpressionStack.push_front(asterisk);

       // This is the case of an option not being specified, as in "read(1)" instead of "read(UNIT=1)"
       // To make the astExpressionStack match the astNameStack we have to push a default token onto the astNameStack.
          Token_t* defaultToken = create_token(0,0,0,"defaultString");
          ROSE_ASSERT(defaultToken != NULL);
          astNameStack.push_front(defaultToken);
        }
       else
        {
          if (hasExpression == false)
             {
               printf ("In c_action_io_control_spec() -- hasAsterisk == false: Need an example of this case before I can support it! \n");
             }
       // ROSE_ASSERT(false);
          ROSE_ASSERT(hasExpression == true);
        }

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R913 c_action_io_control_spec()");
#endif

  // DQ (9/6/2010): Added error checking. We can't assert equalify since an if stmt can have a predicate on the stack.
  // ROSE_ASSERT(astExpressionStack.size() == astNameStack.size());
     ROSE_ASSERT(astExpressionStack.size() >= astNameStack.size());
   }

/** R913 list
 * io_control_spec_list
 *      :       io_control_spec ( T_COMMA io_control_spec )*
 * 
 * @param count The number of items in the list.
 */
void c_action_io_control_spec_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("c_action_io_control_spec_list__begin() \n");
   }
void c_action_io_control_spec_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("c_action_io_control_spec_list(): count = %d \n",count);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R913 list c_action_io_control_spec_list()");
#endif

// Note that we need to associate each expression with names on the astNameStack.

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R913 list c_action_io_control_spec_list()");
#endif
   }

/**
 * R914
 * format
 *
 */
void c_action_format()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_format() \n");

  // DQ (12/3/2010): I think this is the problem with test2007_211.f.  Namely we have to push a name ("fmt") on the the astNamestack!

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R914 c_action_format()");
#endif

     if (astExpressionStack.empty() == true)
        {
       // If there was nothing pushed to the stack, then push the SgAsteriskShapeExp ???
       // I think we are just borrowing the fact that this expression will output a "*" and 
       // perhaps we should have a specialized expression more specific to format semantics.
          SgExpression* asteriskExpression = new SgAsteriskShapeExp();
          setSourcePosition(asteriskExpression);
          astExpressionStack.push_front(asteriskExpression);

          push_token("fmt");
        }
       else
        {
       // Note that if this is a scalar-logical-expression, then the expression is likely from a if-stmt and not a part of a format statement.
          SgExpression* expressionOnStack = astExpressionStack.front();

       // Using a switch statement allows us to more cleanly handle a growing number of special cases.
       // printf ("expressionOnStack = %p = %s \n",expressionOnStack,expressionOnStack->class_name().c_str());
          switch(expressionOnStack->variantT())
             {
            // Handle case of format label (integer): print 1, N
               case V_SgIntVal:

            // Handle case of format string: print "(a, i8)", "a = ", N (see test2010_123.f90).
               case V_SgStringVal:

            // Handle case of format string (formed from string concatination): print "(a, "||" i8)", "a = ", N (see test2010_124.f90).
               case V_SgConcatenationOp:
                  {
                    push_token("fmt");
                    break;
                  }

            // Handle case of format string (passed as a variable):  CHARACTER(len=*) :: fmtstr;  PRINT fmtstr, str (see test2010_125.f90).
               case V_SgVarRefExp:
                  {
                 // printf ("expressionOnStack->get_type() = %p = %s \n",expressionOnStack->get_type(),expressionOnStack->get_type()->class_name().c_str());
                    SgTypeString* stringType = isSgTypeString(expressionOnStack->get_type());
                 // ROSE_ASSERT(stringType != NULL);
                    if (stringType != NULL)
                       {
                         push_token("fmt");
                       }
                    break;
                  }

               default:
                  {
                 // Add no "fmt" token.
                    printf ("Non integer scalars can NOT be used in format statements so ignore this (no fmt pushed onto astNameStack). \n");
                  }
             }
        }

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R914 c_action_format()");
#endif
   }

/**
 * R915
 * input_item
 * 
 */
void c_action_input_item()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("c_action_input_item() \n");
   }

/** R915 list
 * input_item_list
 *      :       input_item ( T_COMMA input_item )*
 * 
 * @param count The number of items in the list.
 */
void c_action_input_item_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("c_action_input_item() \n");
   }
void c_action_input_item_list(int count)
   {
  // This takes the number of expression on the stack (given by 'count'). clears them from
  // the stack, and puts them into an expression list and puts that on the stack.

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("c_action_input_item_list(): count = %d \n",count);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R915 list c_action_input_item_list()");
#endif

     SgExprListExp* exprListExp = new SgExprListExp();
     ROSE_ASSERT(exprListExp != NULL);

  // while (astExpressionStack.empty() == false)
     for (int i = 0; i < count; i++)
        {
          exprListExp->prepend_expression(astExpressionStack.front());
          astExpressionStack.front()->set_parent(exprListExp);
          astExpressionStack.pop_front();
        }

     setSourcePosition(exprListExp);
  // ROSE_ASSERT(astExpressionStack.empty() == true);
     astExpressionStack.push_front(exprListExp);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R915 list c_action_input_item_list()");
#endif
   }

/**
 * R916
 * output_item
 *
 */
void c_action_output_item()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("c_action_output_item() \n");

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R916 c_action_output_item()");
#endif
   }

/** R916 list
 * output_item_list
 *      :       output_item ( T_COMMA output_item )*
 * 
 * @param count The number of items in the list.
 */
void c_action_output_item_list__begin()
   {
  // I don't think that we need this function!

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R916 list c_action_output_item_list__begin()");
#endif
   }

void c_action_output_item_list(int count)
   {
  // This takes the number of expression on the stack (given by 'count'). clears them from
  // the stack, and puts them into an expression list and puts that on the stack.

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("c_action_output_item_list(): count = %d \n",count);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R916 list c_action_output_item_list()");
#endif

  // Convert the list of expressions into an expression list (and put it onto the stack)
  // ROSE_ASSERT(astExpressionStack.size() == (unsigned)count);

     SgExprListExp* exprListExp = new SgExprListExp();
     ROSE_ASSERT(exprListExp != NULL);

  // while (astExpressionStack.empty() == false)
  // printf ("In R916: count = %d \n",count);
     for (int i = 0; i < count; i++)
        {
          ROSE_ASSERT(astExpressionStack.empty() == false);
          exprListExp->prepend_expression(astExpressionStack.front());
          astExpressionStack.front()->set_parent(exprListExp);
          astExpressionStack.pop_front();
        }

     setSourcePosition(exprListExp);
  // ROSE_ASSERT(astExpressionStack.empty() == true);
     astExpressionStack.push_front(exprListExp);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R916 list c_action_output_item_list()");
#endif
   }

/**
 * R917
 * io_implied_do
 *
 */
void c_action_io_implied_do()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("c_action_io_implied_do() \n");

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R917 c_action_io_implied_do()");
#endif

  // Note that non-unit stride is not allowed here, I think.

  // The implied_do_control is at the top of the stack
     SgExpression* implied_do_control_temp = astExpressionStack.front();
     astExpressionStack.pop_front();

     SgExprListExp* implied_do_control = isSgExprListExp(implied_do_control_temp);
     ROSE_ASSERT(implied_do_control != NULL);

  // DQ (10/9/2010): This improved design uses a single expression to hold the do loop variable initialization.
     ROSE_ASSERT(implied_do_control->get_expressions().size() == 2);

     SgExpression*  doLoopVarInitialization  = implied_do_control->get_expressions()[0];
     ROSE_ASSERT(doLoopVarInitialization != NULL);

     SgExpression* upperBound = implied_do_control->get_expressions()[1];
     ROSE_ASSERT(upperBound != NULL);

     implied_do_control->get_expressions().clear();
     delete implied_do_control;
     implied_do_control = NULL;

     SgExpression* increment  = new SgNullExpression();
     ROSE_ASSERT(increment != NULL);
     setSourcePosition(increment);

     ROSE_ASSERT(astExpressionStack.empty() == false);
  // printf ("In R917 c_action_io_implied_do(): astExpressionStack.front() = %s \n",astExpressionStack.front()->class_name().c_str());
     SgExprListExp* objectList = isSgExprListExp(astExpressionStack.front());
     astExpressionStack.pop_front();

     ROSE_ASSERT(objectList != NULL);
     setSourcePosition(objectList);

  // objectList->append_expression(variableReference);

     SgScopeStatement* implied_do_scope = NULL; // new SgBasicBlock();
  // SgImpliedDo* impliedDo = new SgImpliedDo(doLoopVar,assignment,upperBound,increment,objectList);
  // SgImpliedDo* impliedDo = new SgImpliedDo(variableReference,doLoopVar,lowerBound,upperBound,increment,objectList);
  // SgImpliedDo* impliedDo = new SgImpliedDo(doLoopVar,lowerBound,upperBound,increment,objectList);
  // SgImpliedDo* impliedDo = new SgImpliedDo(doLoopVar,lowerBound,upperBound,increment,objectList,implied_do_scope);
  // SgImpliedDo* impliedDo = new SgImpliedDo(doLoopVarInitialization,lowerBound,upperBound,increment,objectList,implied_do_scope);
     SgImpliedDo* impliedDo = new SgImpliedDo(doLoopVarInitialization,upperBound,increment,objectList,implied_do_scope);
     ROSE_ASSERT(impliedDo != NULL);
     setSourcePosition(impliedDo);

     objectList->set_parent(impliedDo);
     upperBound->set_parent(impliedDo);
  // lowerBound->set_parent(assignment);
  // lowerBound->set_parent(impliedDo);
  // doLoopVar->set_parent(impliedDo);
     doLoopVarInitialization->set_parent(impliedDo);

     astExpressionStack.push_front(impliedDo);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R917 list c_action_io_implied_do()");
#endif
   }

/**
 * R918
 * io_implied_do_object
 *
 */
void c_action_io_implied_do_object()
   {
  // This rule is called for each object so we have to accumulate the results into a SgExprListExp which we store on the stack.
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_io_implied_do_object() \n");

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R918 c_action_io_implied_do_object()");
#endif

     SgExpression* implied_do_object = astExpressionStack.front();
     astExpressionStack.pop_front();

  // Accumulate the entries into a SgExprListExp that we replace at the top of the list.
     ROSE_ASSERT(astExpressionStack.empty() == false);
     SgExprListExp* implied_do_object_list = isSgExprListExp(astExpressionStack.front());
     if (implied_do_object_list == NULL)
        {
       // This is the first entry, so we have to build the list
          implied_do_object_list = new SgExprListExp();
        }
       else
        {
          astExpressionStack.pop_front();
        }

  // Add (accumlate) the implied_do_object into the implied_do_object_list
     implied_do_object_list->append_expression(implied_do_object);

  // Push both back onto the stack
     astExpressionStack.push_front(implied_do_object_list);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R918 c_action_io_implied_do_object()");
#endif
   }

/**
 * R919
 * io_implied_do_control
 *
 */
#if ROSE_OFP_MINOR_VERSION_NUMBER >= 8 & ROSE_OFP_PATCH_VERSION_NUMBER >= 2
void c_action_io_implied_do_control(ofp_bool xxx)
#else
void c_action_io_implied_do_control()
#endif
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
#if ROSE_OFP_MINOR_VERSION_NUMBER >= 8 & ROSE_OFP_PATCH_VERSION_NUMBER >= 2
          printf ("In c_action_io_implied_do_control() xxx = %s \n",xxx ? "true" : "false");
#else
          printf ("In c_action_io_implied_do_control() \n");
#endif

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R919 c_action_io_implied_do_control()");
#endif

     ROSE_ASSERT(astExpressionStack.empty() == false);
     SgExpression* upperBound = astExpressionStack.front();
     astExpressionStack.pop_front();
  // setSourcePosition(upperBound);
  // printf ("upperBound = %p \n",upperBound);

     ROSE_ASSERT(astExpressionStack.empty() == false);
     SgExpression* lowerBound = astExpressionStack.front();
     astExpressionStack.pop_front();
  // setSourcePosition(lowerBound);
  // printf ("lowerBound = %p \n",lowerBound);

  // SgExpression* loopVar = astExpressionStack.front();
  // astExpressionStack.pop_front();
  // setSourcePosition(loopVar);

  // SgAssignOp* assignment = new SgAssignOp(loopVar,lowerBound,NULL);
  // setSourcePosition(assignment);

  // DQ (10/9/2010): Reimplementation of support for implied do loop support.
  // We have to form the implied do loop variable initialization. Note that this is not 
  // a variable declaration, since if implicit none is used, the variable must have 
  // already been declared.
     ROSE_ASSERT(astNameStack.empty() == false);
     SgName do_variable_name = astNameStack.front()->text;
     astNameStack.pop_front();

  // printf ("implied do loop variable name = %s \n",do_variable_name.str());

  // DQ (1/18/2011): This detects where we have used the semantics of implicitly building symbols for implicit variables.
  // printf ("WARNING: This use of trace_back_through_parent_scopes_lookup_variable_symbol() used the side effect of building a symbol if the reference is not found! \n");
  // ROSE_ASSERT(false);

     SgVariableSymbol* variableSymbol = trace_back_through_parent_scopes_lookup_variable_symbol(do_variable_name,astScopeStack.front());

  // If this was an implicit variable then variableSymbol would be NULL, but the variable would have  
  // been built as a side-effect of calling trace_back_through_parent_scopes_lookup_variable_symbol().
  // So now we have to call it again.

  // Note that the location of the scope of the definition of the implicit implied do index will 
  // later be a special scope stored in the implied do loop IR node.
     if (variableSymbol == NULL)
        {
       // DQ (1/19/2011): Build the implicit variable
          buildImplicitVariableDeclaration(do_variable_name);

       // Now verify that it is present.
          variableSymbol = trace_back_through_parent_scopes_lookup_variable_symbol(do_variable_name,astScopeStack.front());
          ROSE_ASSERT(variableSymbol != NULL);
        }

  // Now we can assert that this should be a pointer to a valid symbol.
     ROSE_ASSERT(variableSymbol != NULL);

     SgVarRefExp* doLoopVar = SageBuilder::buildVarRefExp(variableSymbol);
     ROSE_ASSERT(doLoopVar != NULL);

     SgExpression* doVariableInitialization = SageBuilder::buildAssignOp(doLoopVar,lowerBound);
     ROSE_ASSERT(doVariableInitialization != NULL);
  // printf ("doVariableInitialization = %p = %s \n",doVariableInitialization,doVariableInitialization->class_name().c_str());

     SgExprListExp* implied_do_control = new SgExprListExp();
     ROSE_ASSERT(implied_do_control != NULL);

  // implied_do_control->append_expression(doLoopVar);
     implied_do_control->append_expression(doVariableInitialization);
  // implied_do_control->append_expression(lowerBound);
     implied_do_control->append_expression(upperBound);

     ROSE_ASSERT(implied_do_control->get_expressions().size() == 2);

     astExpressionStack.push_front(implied_do_control);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R919 c_action_io_implied_do_control()");
#endif
   }

/**
 * R920
 * dtv_type_spec
 *
 * @param typeKeyword T_TYPE or T_CLASS token.
 */
void c_action_dtv_type_spec(Token_t * typeKeyword)
   {
   }

/** R921
 * wait_stmt
 *
 * :  (label)? T_WAIT T_LPAREN wait_spec_list T_RPAREN T_EOS
 *
 * @param label The label.
 */
// void c_action_wait_stmt(Token_t * label)
void c_action_wait_stmt(Token_t *label, Token_t *waitKeyword, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_wait_stmt(): waitKeyword = %p = %s \n",waitKeyword,waitKeyword != NULL ? waitKeyword->text : "NULL");

     initialize_global_scope_if_required();
     build_implicit_program_statement_if_required();

     SgWaitStatement* waitStatement = new SgWaitStatement();
     ROSE_ASSERT(waitStatement != NULL);
     setSourcePosition(waitStatement);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R921 c_action_wait_stmt()");
#endif

  // printf ("Warning: Ignoring all but the 'unit' in the OpenStatement \n");
     while (astExpressionStack.empty() == false)
        {
          ROSE_ASSERT(astNameStack.empty() == false);

          SgExpression* expression = astExpressionStack.front();
          Token_t* name = astNameStack.front();

       // We don't need the current_IO_Control_Spec data structure in the code below.

       // The "unit=" string is optional, if it was not present then a toekn was pushed onto the stack with the text value "defaultString"
          if ( strncasecmp(name->text,"unit",4) == 0 || strncmp(name->text,"defaultString",13) == 0)
             {
               waitStatement->set_unit(expression);
             }
          else if ( strncasecmp(name->text,"iostat",6) == 0 )
             {
               waitStatement->set_iostat(expression);
             }
          else if ( strncasecmp(name->text,"err",3) == 0 )
             {
            // waitStatement->set_err(expression);
               ROSE_ASSERT(expression != NULL);
               SgExpression* labelRefExp = buildLabelRefExp(expression);
               ROSE_ASSERT(labelRefExp != NULL);
               waitStatement->set_err(labelRefExp);
               labelRefExp->set_parent(waitStatement);
             }
          else if ( strncasecmp(name->text,"iomsg",5) == 0 )
             {
               waitStatement->set_iomsg(expression);
             }

          astNameStack.pop_front();
          astExpressionStack.pop_front();

          expression->set_parent(waitStatement);
        }

     astScopeStack.front()->append_statement(waitStatement);
   }

/**
 * R922
 * wait_spec
 * 
 * @param id T_IDENT or null.
 */
void c_action_wait_spec(Token_t *id)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_wait_spec(): id = %p = %s \n",id,id != NULL ? id->text : "NULL");

  // ROSE_ASSERT(id != NULL);
     if (id != NULL)
        {
          astNameStack.push_front(id);
        }
       else
        {
       // This is the case of an option not being specified, as in "wait(1)" instead of "wait(UNIT=1)"
       // To make the astExpressionStack match the astNameStack we have to push a default token onto the astNameStack.
          Token_t* defaultToken = create_token(0,0,0,"defaultString");
          ROSE_ASSERT(defaultToken != NULL);
          astNameStack.push_front(defaultToken);
        }
   }

/** R922 list
 * wait_spec_list
 *      :       wait_spec ( T_COMMA wait_spec )*
 * 
 * @param count The number of items in the list.
 */
void c_action_wait_spec_list__begin()
{
}
void c_action_wait_spec_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_wait_spec_list(): count = %d \n",count);
   }

/** R923
 * backspace_stmt
 *
 * :  ((label)? T_BACKSPACE T_LPAREN) => (label)? T_BACKSPACE T_LPAREN 
                        position_spec_list T_RPAREN T_EOS
 * |  ((label)? T_BACKSPACE) => (label)? T_BACKSPACE file_unit_number T_EOS
 *
 * @param label The label.
 * @param hasPositionSpecList True if there is a position spec list. False is there is a file unit number.
 */
// void c_action_backspace_stmt(Token_t * label, ofp_bool hasPositionSpecList)
void c_action_backspace_stmt(Token_t *label, Token_t *backspaceKeyword, Token_t *eos, ofp_bool hasPositionSpecList)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_backspace_stmt(): waitKeyword = %p = %s \n",backspaceKeyword,backspaceKeyword != NULL ? backspaceKeyword->text : "NULL");

     initialize_global_scope_if_required();
     build_implicit_program_statement_if_required();

     SgBackspaceStatement* backspaceStatement = new SgBackspaceStatement();
     ROSE_ASSERT(backspaceStatement != NULL);

  // setSourcePosition(backspaceStatement);
     ROSE_ASSERT(backspaceKeyword != NULL);
     setSourcePosition(backspaceStatement,backspaceKeyword);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R923 c_action_backspace_stmt()");
#endif

  // printf ("Warning: Ignoring all but the 'unit' in the OpenStatement \n");
     while (astExpressionStack.empty() == false)
        {
       // DQ (12/5/2010): This fails for test2010_129.f90.
          ROSE_ASSERT(astNameStack.empty() == false);

          SgExpression* expression = astExpressionStack.front();
          Token_t* name = astNameStack.front();

       // We don't need the current_IO_Control_Spec data structure in the code below.

       // The "unit=" string is optional, if it was not present then a toekn was pushed onto the stack with the text value "defaultString"
          if ( strncasecmp(name->text,"unit",4) == 0 || strncmp(name->text,"defaultString",13) == 0)
             {
               backspaceStatement->set_unit(expression);
             }
          else if ( strncasecmp(name->text,"iostat",6) == 0 )
             {
               backspaceStatement->set_iostat(expression);
             }
          else if ( strncasecmp(name->text,"err",3) == 0 )
             {
            // backspaceStatement->set_err(expression);
               ROSE_ASSERT(expression != NULL);
               SgExpression* labelRefExp = buildLabelRefExp(expression);
               ROSE_ASSERT(labelRefExp != NULL);
               backspaceStatement->set_err(labelRefExp);
               labelRefExp->set_parent(backspaceStatement);
             }
          else if ( strncasecmp(name->text,"iomsg",5) == 0 )
             {
               backspaceStatement->set_iomsg(expression);
             }

          astNameStack.pop_front();
          astExpressionStack.pop_front();

          expression->set_parent(backspaceStatement);
        }

     astScopeStack.front()->append_statement(backspaceStatement);
   }

/** R924
 * endfile_stmt
 *
 * :  ((label)? T_END T_FILE T_LPAREN) => (label)? T_END T_FILE 
 *                      T_LPAREN position_spec_list T_RPAREN T_EOS
 * |  ((label)? T_ENDFILE T_LPAREN) => (label)? T_ENDFILE T_LPAREN 
 *                      position_spec_list T_RPAREN T_EOS
 * |  ((label)? T_END T_FILE)=> (label)? T_END T_FILE file_unit_number T_EOS
 * |  ((label)? T_ENDFILE) => (label)? T_ENDFILE file_unit_number T_EOS
 *
 * @param label The label.
 * @param hasPositionSpecList True if there is a position spec list. False is there is a file unit number.
 */
// void c_action_endfile_stmt(Token_t * label, ofp_bool hasPositionSpecList)
void c_action_endfile_stmt(Token_t *label, Token_t *endKeyword, Token_t *fileKeyword, Token_t *eos, ofp_bool hasPositionSpecList)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_wait_stmt(): endKeyword = %p = %s fileKeyword = %p = %s hasPositionSpecList = %s \n",
               endKeyword,endKeyword != NULL ? endKeyword->text : "NULL",
               fileKeyword,fileKeyword != NULL ? fileKeyword->text : "NULL",hasPositionSpecList ? "true" : "false");

     initialize_global_scope_if_required();
     build_implicit_program_statement_if_required();

     SgEndfileStatement* endfileStatement = new SgEndfileStatement();
     ROSE_ASSERT(endfileStatement != NULL);
     setSourcePosition(endfileStatement);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R924 c_action_endfile_stmt()");
#endif

  // printf ("Warning: Ignoring all but the 'unit' in the OpenStatement \n");
     while (astExpressionStack.empty() == false)
        {
          ROSE_ASSERT(astNameStack.empty() == false);

          SgExpression* expression = astExpressionStack.front();
          Token_t* name = astNameStack.front();

       // We don't need the current_IO_Control_Spec data structure in the code below.

       // The "unit=" string is optional, if it was not present then a toekn was pushed onto the stack with the text value "defaultString"
          if ( strncasecmp(name->text,"unit",4) == 0 || strncmp(name->text,"defaultString",13) == 0)
             {
               endfileStatement->set_unit(expression);
             }
          else if ( strncasecmp(name->text,"iostat",6) == 0 )
             {
               endfileStatement->set_iostat(expression);
             }
          else if ( strncasecmp(name->text,"err",3) == 0 )
             {
            // endfileStatement->set_err(expression);
               ROSE_ASSERT(expression != NULL);
               SgExpression* labelRefExp = buildLabelRefExp(expression);
               ROSE_ASSERT(labelRefExp != NULL);
               endfileStatement->set_err(labelRefExp);
               labelRefExp->set_parent(endfileStatement);
             }
          else if ( strncasecmp(name->text,"iomsg",5) == 0 )
             {
               endfileStatement->set_iomsg(expression);
             }

          astNameStack.pop_front();
          astExpressionStack.pop_front();

          expression->set_parent(endfileStatement);
        }

     astScopeStack.front()->append_statement(endfileStatement);
   }

/** R925
 * rewind_stmt
 *
 * :  ((label)? T_REWIND T_LPAREN) => (label)? T_REWIND T_LPAREN 
                        position_spec_list T_RPAREN T_EOS
 * |  ((label)? T_REWIND) => (label)? T_REWIND file_unit_number T_EOS
 *
 * @param label The label.
 * @param hasPositionSpecList True if there is a position spec list. False is there is a file unit number.
 */
// void c_action_rewind_stmt(Token_t * label, ofp_bool hasPositionSpecList)
void c_action_rewind_stmt(Token_t *label, Token_t *rewindKeyword, Token_t *eos, ofp_bool hasPositionSpecList)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_rewind_stmt(): rewindKeyword = %p = %s hasPositionSpecList = %s \n",rewindKeyword,rewindKeyword != NULL ? rewindKeyword->text : "NULL",hasPositionSpecList ? "true" : "false");

     initialize_global_scope_if_required();
     build_implicit_program_statement_if_required();

     SgRewindStatement* rewindStatement = new SgRewindStatement();
     ROSE_ASSERT(rewindStatement != NULL);
     setSourcePosition(rewindStatement);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R925 c_action_rewind_stmt()");
#endif

  // printf ("Warning: Ignoring all but the 'unit' in the OpenStatement \n");
  // while (astExpressionStack.empty() == false)
     while (astNameStack.empty() == false)
        {
          ROSE_ASSERT(astExpressionStack.empty() == false);

          SgExpression* expression = astExpressionStack.front();
          Token_t* name = astNameStack.front();

       // We don't need the current_IO_Control_Spec data structure in the code below.

       // The "unit=" string is optional, if it was not present then a toekn was pushed onto the stack with the text value "defaultString"
          if ( strncasecmp(name->text,"unit",4) == 0 || strncmp(name->text,"defaultString",13) == 0)
             {
               rewindStatement->set_unit(expression);
             }
          else if ( strncasecmp(name->text,"iostat",6) == 0 )
             {
               rewindStatement->set_iostat(expression);
             }
          else if ( strncasecmp(name->text,"err",3) == 0 )
             {
            // rewindStatement->set_err(expression);
               ROSE_ASSERT(expression != NULL);
               SgExpression* labelRefExp = buildLabelRefExp(expression);
               ROSE_ASSERT(labelRefExp != NULL);
               rewindStatement->set_err(labelRefExp);
               labelRefExp->set_parent(rewindStatement);
             }
          else if ( strncasecmp(name->text,"iomsg",5) == 0 )
             {
               rewindStatement->set_iomsg(expression);
             }

          astNameStack.pop_front();
          astExpressionStack.pop_front();

          expression->set_parent(rewindStatement);
        }

  // If this was "rewind 1" then we would now have a "1" on the stack, and the astNameStack would have been empty.
     if (astExpressionStack.empty() == false)
        {
          SgExpression* formatLabel = astExpressionStack.front();
          SgIntVal* integerValueLabel = isSgIntVal(formatLabel);
          if (integerValueLabel != NULL)
             {
            // Need to lookup the label from the symbol table
               rewindStatement->set_unit(integerValueLabel);
               integerValueLabel->set_parent(rewindStatement);

               astExpressionStack.pop_front();
             }
        }

     astScopeStack.front()->append_statement(rewindStatement);
   }

/**
 * R926
 * position_spec
 *
 * @param id T_IDENT for the specifier or null.
 */
void c_action_position_spec(Token_t *id)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_position_spec(): id = %p = %s \n",id,id != NULL ? id->text : "NULL");

  // ROSE_ASSERT(id != NULL);
     if (id != NULL)
        {
          astNameStack.push_front(id);
        }
       else
        {
       // This is the case of an option not being specified, as in "flush(1)" instead of "flush(UNIT=1)"
       // To make the astExpressionStack match the astNameStack we have to push a default token onto the astNameStack.
          Token_t* defaultToken = create_token(0,0,0,"defaultString");
          ROSE_ASSERT(defaultToken != NULL);
          astNameStack.push_front(defaultToken);
        }
   }

/** R926 list
 * position_spec_list
 *      :       position_spec ( T_COMMA position_spec )*
 * 
 * @param count The number of items in the list.
 */
void c_action_position_spec_list__begin()
{
}
void c_action_position_spec_list(int count)
{
}

/** R927
 * flush_stmt
 *
 * :  ((label)? T_FLUSH T_LPAREN) => (label)? T_FLUSH T_LPAREN \
 *                      flush_spec_list T_RPAREN T_EOS
 * |  ((label)? T_FLUSH) => (label)? T_FLUSH file_unit_number T_EOS
 *
 * @param label The label.
 * @param hasFlushSpecList True if there is a flush spec list. False is there is a file unit number.
 */
// void c_action_flush_stmt(Token_t * label, ofp_bool hasFlushSpecList)
void c_action_flush_stmt(Token_t *label, Token_t *flushKeyword, Token_t *eos, ofp_bool hasFlushSpecList)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_flush_stmt(): label = %p = %s flushKeyword = %p = %s hasFlushSpecList = %s \n",
               label,label != NULL ? label->text : "NULL",flushKeyword,flushKeyword != NULL ? flushKeyword->text : "NULL",hasFlushSpecList ? "true" : "false");

     initialize_global_scope_if_required();
     build_implicit_program_statement_if_required();

     SgFlushStatement* flushStatement = new SgFlushStatement();
     ROSE_ASSERT(flushStatement != NULL);

     ROSE_ASSERT(flushKeyword != NULL);
     setSourcePosition(flushStatement,flushKeyword);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R908 c_action_flush_stmt()");
#endif

  // printf ("Warning: Ignoring all but the 'unit' in the OpenStatement \n");
     while (astExpressionStack.empty() == false)
        {
          ROSE_ASSERT(astNameStack.empty() == false);

          SgExpression* expression = astExpressionStack.front();
          Token_t* name = astNameStack.front();

       // We don't need the current_IO_Control_Spec data structure in the code below.

       // The "unit=" string is optional, if it was not present then a toekn was pushed onto the stack with the text value "defaultString"
          if ( strncasecmp(name->text,"unit",4) == 0 || strncmp(name->text,"defaultString",13) == 0)
             {
               flushStatement->set_unit(expression);
             }
          else if ( strncasecmp(name->text,"iostat",6) == 0 )
             {
               flushStatement->set_iostat(expression);
             }
          else if ( strncasecmp(name->text,"err",3) == 0 )
             {
            // flushStatement->set_err(expression);
               ROSE_ASSERT(expression != NULL);
               SgExpression* labelRefExp = buildLabelRefExp(expression);
               ROSE_ASSERT(labelRefExp != NULL);
               flushStatement->set_err(labelRefExp);
               labelRefExp->set_parent(flushStatement);
             }
          else if ( strncasecmp(name->text,"iomsg",5) == 0 )
             {
               flushStatement->set_iomsg(expression);
             }

          astNameStack.pop_front();
          astExpressionStack.pop_front();

          expression->set_parent(flushStatement);
        }

     astScopeStack.front()->append_statement(flushStatement);
   }

/**
 * R928
 * flush_spec
 *
 * @param id T_IDENT for specifier or null.
 */
void c_action_flush_spec(Token_t *id)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_flush_spec(): id = %p = %s \n",id,id != NULL ? id->text : "NULL");

  // ROSE_ASSERT(id != NULL);
     if (id != NULL)
        {
          astNameStack.push_front(id);
        }
       else
        {
       // This is the case of an option not being specified, as in "flush(1)" instead of "flush(UNIT=1)"
       // To make the astExpressionStack match the astNameStack we have to push a default token onto the astNameStack.
          Token_t* defaultToken = create_token(0,0,0,"defaultString");
          ROSE_ASSERT(defaultToken != NULL);
          astNameStack.push_front(defaultToken);
        }
   }

/** R928 list
 * flush_spec_list
 *      :       flush_spec ( T_COMMA flush_spec )*
 * 
 * @param count The number of items in the list.
 */
void c_action_flush_spec_list__begin()
{
}
void c_action_flush_spec_list(int count)
{
}

/** R929
 * inquire_stmt
 *
 * :    (label)? T_INQUIRE T_LPAREN inquire_spec_list T_RPAREN T_EOS
 * |    (label)? T_INQUIRE_STMT_2 T_INQUIRE T_LPAREN T_IDENT T_EQUALS 
 *                      scalar_int_variable T_RPAREN output_item_list T_EOS
 *
 * @param label The label.
 * @param boolean True if this is inquire statement of type 2.
 */
// void c_action_inquire_stmt(Token_t * label, ofp_bool isType2)
void c_action_inquire_stmt(Token_t *label, Token_t *inquireKeyword, Token_t *id, Token_t *eos, ofp_bool isType2)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
        printf ("In c_action_inquire_stmt(): inquireKeyword = %p = %s id = %s isType2 = %s \n",inquireKeyword,inquireKeyword != NULL ? inquireKeyword->text : "NULL",id != NULL ? id->text : "NULL",isType2 ? "true" : "false");

     initialize_global_scope_if_required();
     build_implicit_program_statement_if_required();

     SgInquireStatement* inquireStatement = new SgInquireStatement();
     ROSE_ASSERT(inquireStatement != NULL);
     setSourcePosition(inquireStatement);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R929 c_action_inquire_stmt()");
#endif

     if (isType2 == true)
        {
       // This is the "INQUIRE (IOLENGTH=IOL)" case.
          ROSE_ASSERT(id != NULL);
          ROSE_ASSERT(id->text != NULL);
          ROSE_ASSERT( matchingName(id->text,"iolength") == true);

          ROSE_ASSERT(astExpressionStack.empty() == false);
       // printf ("astExpressionStack.front() = %p = %s \n",astExpressionStack.front(),astExpressionStack.front()->class_name().c_str());

          ROSE_ASSERT(astExpressionStack.empty() == false);
          SgExprListExp* outputList = isSgExprListExp(astExpressionStack.front());
          ROSE_ASSERT(outputList != NULL);
          astExpressionStack.pop_front();
          inquireStatement->set_io_stmt_list(outputList);

          SgVarRefExp* scalarIntegerExpression = isSgVarRefExp(astExpressionStack.front());
          ROSE_ASSERT(scalarIntegerExpression != NULL);
          astExpressionStack.pop_front();
          inquireStatement->set_iolengthExp(scalarIntegerExpression);

          ROSE_ASSERT(astExpressionStack.empty() == true);
        }
       else
        {
       // This is the "INQUIRE(inquire-spec-list)" case.

       // printf ("Warning: Ignoring all but the 'unit' in the OpenStatement \n");
          while (astExpressionStack.empty() == false)
             {
               ROSE_ASSERT(astNameStack.empty() == false);

               SgExpression* expression = astExpressionStack.front();
               Token_t* name = astNameStack.front();

            // We don't need the current_IO_Control_Spec data structure in the code below.

            // The "unit=" string is optional, if it was not present then a token was pushed onto the stack with the text value "defaultString"
               if ( strncasecmp(name->text,"unit",4) == 0 || strncmp(name->text,"defaultString",13) == 0)
                  {
                    inquireStatement->set_unit(expression);
                  }
               else if ( strncasecmp(name->text,"iostat",6) == 0 )
                  {
                    inquireStatement->set_iostat(expression);
                  }
               else if ( strncasecmp(name->text,"err",3) == 0 )
                  {
                 // inquireStatement->set_err(expression);
                    ROSE_ASSERT(expression != NULL);
                    SgExpression* labelRefExp = buildLabelRefExp(expression);
                    ROSE_ASSERT(labelRefExp != NULL);
                    inquireStatement->set_err(labelRefExp);
                    labelRefExp->set_parent(inquireStatement);
                  }
               else if ( strncasecmp(name->text,"iomsg",5) == 0 )
                  {
                    inquireStatement->set_iomsg(expression);
                  }
               else if ( strncasecmp(name->text,"file",4) == 0 )
                  {
                    inquireStatement->set_file(expression);
                  }
               else if ( strncasecmp(name->text,"access",6) == 0 )
                  {
                    inquireStatement->set_access(expression);
                  }
               else if ( strncasecmp(name->text,"form",4) == 0 )
                  {
                    inquireStatement->set_form(expression);
                  }
               else if ( strncasecmp(name->text,"recl",4) == 0 )
                  {
                    inquireStatement->set_recl(expression);
                  }
               else if ( strncasecmp(name->text,"blank",5) == 0 )
                  {
                    inquireStatement->set_blank(expression);
                  }
               else if ( strncasecmp(name->text,"exist",5) == 0 )
                  {
                    inquireStatement->set_exist(expression);
                  }
               else if ( strncasecmp(name->text,"opened",6) == 0 )
                  {
                    inquireStatement->set_opened(expression);
                  }
               else if ( strncasecmp(name->text,"number",6) == 0 )
                  {
                    inquireStatement->set_number(expression);
                  }
               else if ( strncasecmp(name->text,"named",5) == 0 )
                  {
                    inquireStatement->set_named(expression);
                  }
               else if ( strncasecmp(name->text,"name",4) == 0 )
                  {
                    inquireStatement->set_name(expression);
                  }
               else if ( strncasecmp(name->text,"sequential",10) == 0 )
                  {
                    inquireStatement->set_sequential(expression);
                  }
               else if ( strncasecmp(name->text,"direct",6) == 0 )
                  {
                    inquireStatement->set_direct(expression);
                  }
               else if ( strncasecmp(name->text,"formatted",9) == 0 )
                  {
                    inquireStatement->set_formatted(expression);
                  }
               else if ( strncasecmp(name->text,"unformatted",11) == 0 )
                  {
                    inquireStatement->set_unformatted(expression);
                  }
               else if ( strncasecmp(name->text,"nextrec",7) == 0 )
                  {
                    inquireStatement->set_nextrec(expression);
                  }
               else if ( strncasecmp(name->text,"position",8) == 0 )
                  {
                    inquireStatement->set_position(expression);
                  }
               else if ( strncasecmp(name->text,"action",6) == 0 )
                  {
                    inquireStatement->set_action(expression);
                  }
               else if ( strncasecmp(name->text,"read",4) == 0 )
                  {
                    inquireStatement->set_read(expression);
                  }
               else if ( strncasecmp(name->text,"write",5) == 0 )
                  {
                    inquireStatement->set_write(expression);
                  }
               else if ( strncasecmp(name->text,"readwrite",9) == 0 )
                  {
                    inquireStatement->set_readwrite(expression);
                  }
               else if ( strncasecmp(name->text,"delim",5) == 0 )
                  {
                    inquireStatement->set_delim(expression);
                  }
               else if ( strncasecmp(name->text,"pad",3) == 0 )
                  {
                    inquireStatement->set_pad(expression);
                  }
               else if ( strncasecmp(name->text,"asynchronous",12) == 0 )
                  {
                    inquireStatement->set_asynchronous(expression);
                  }
               else if ( strncasecmp(name->text,"decimal",7) == 0 )
                  {
                    inquireStatement->set_decimal(expression);
                  }
               else if ( strncasecmp(name->text,"stream",5) == 0 )
                  {
                    inquireStatement->set_stream(expression);
                  }
               else if ( strncasecmp(name->text,"size",4) == 0 )
                  {
                    inquireStatement->set_size(expression);
                  }
               else if ( strncasecmp(name->text,"pending",7) == 0 )
                  {
                    inquireStatement->set_pending(expression);
                  }

               astNameStack.pop_front();
               astExpressionStack.pop_front();

               expression->set_parent(inquireStatement);
             }
        }

     astScopeStack.front()->append_statement(inquireStatement);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R929 c_action_inquire_stmt()");
#endif
   }

/** R930 list
 * inquire_spec_list
 *      :       inquire_spec ( T_COMMA inquire_spec )*
 * 
 * @param count The number of items in the list.
 */
void c_action_inquire_spec_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_inquire_spec_list__begin() \n");

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R930 c_action_inquire_spec_list__begin()");
#endif
   }

void c_action_inquire_spec(Token_t *specName)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_inquire_spec(): specName = %p = %s \n",specName,specName ? specName->text : "NULL");

     ROSE_ASSERT(specName != NULL);
     astNameStack.push_front(specName);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R930 c_action_inquire_spec()");
#endif
}

void c_action_inquire_spec_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_inquire_spec_list(): count = %d \n",count);

     ROSE_ASSERT(astNameStack.size() == (size_t)count);
     ROSE_ASSERT(astExpressionStack.size() == (size_t)count);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R930 c_action_inquire_spec_list()");
#endif
   }

/** R1001
 * format_stmt
 *
 * :  (label)? T_FORMAT format_specification T_EOS
 *
 * @param label The label.
 */
// void c_action_format_stmt(Token_t * label)
void c_action_format_stmt(Token_t *label, Token_t *formatKeyword, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_format_stmt(): label = %p = %s formatKeyword = %p = %s \n",
               label,label ? label->text : "NULL",formatKeyword,formatKeyword != NULL ? formatKeyword->text : "NULL");

     build_implicit_program_statement_if_required();

     SgFormatItemList* formatItemList = isSgFormatItemList(astNodeStack.front());
     ROSE_ASSERT(formatItemList != NULL);

     astNodeStack.pop_front();

     SgFormatStatement* formatStatement = new SgFormatStatement(formatItemList);

     formatItemList->set_parent(formatStatement);

     setSourcePosition(formatStatement,formatKeyword);

     setStatementNumericLabel(formatStatement,label);

     SgScopeStatement* topOfScope = astScopeStack.front();
     topOfScope->append_statement(formatStatement);

  // Set or clear the astLabelSymbolStack (since it is redundant with the label being passed in)
  // There are two mechanisms used to set labels, and we never know which will be used by OFP.
     setStatementNumericLabelUsingStack(formatStatement);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R1001 c_action_format_stmt()");
#endif
   }

/** R1002
 * format_specification
 *
 * :  T_LPAREN ( format_item_list )? T_RPAREN
 *
 * @param hasFormatItemList True if has a format item list.
 */
void c_action_format_specification(ofp_bool hasFormatItemList)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_format_specification(): hasFormatItemList = %s \n",hasFormatItemList ? "true" : "false");

     if (hasFormatItemList == true)
        {
       // There was something specified in the format statement.
        }
       else
        {
       // this is the case of "format()", we need to push something onto some stack so that
       // R1001 can know that it has something (even if it is nothing).

          SgFormatItemList* formatItemList = new SgFormatItemList();
          astNodeStack.push_front(formatItemList);
        }

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R1002 c_action_format_specification()");
#endif
   }

/** R1003
 * format_item
 *
 * :  T_DATA_EDIT_DESC 
 * |  T_CONTROL_EDIT_DESC
 * |  T_CHAR_STRING_EDIT_DESC
 * |  (T_DIGIT_STRING)? T_LPAREN format_item_list T_RPAREN
 *
 * @param descOrDigit Edit descriptor, unless has a format item list, then either the optinal digit string or null.
 * @param hasFormatItemList True if has a format item list.
 */
void c_action_format_item(Token_t * descOrDigit, ofp_bool hasFormatItemList)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_format_item(): descOrDigit = %s hasFormatItemList = %s \n",descOrDigit ? descOrDigit->text : "NULL",hasFormatItemList ? "true" : "false");

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R1003 c_action_format_item()");
#endif

  // DQ (12/2/2010): I think this is the case of an inner list as an entry in the outer list (see test2010_115.f90).
     if (descOrDigit == NULL && hasFormatItemList == false)
        {
          printf ("Exiting from In c_action_format_item(): already processed list! \n");
          return;
        }

     ROSE_ASSERT(descOrDigit != NULL);

     ROSE_ASSERT(hasFormatItemList == false);

     SgFormatItem* formatItem = new SgFormatItem();

  // printf ("R1003 c_action_format_item(): formatItem = %p \n",formatItem);

  // Handle ethe case of a regular int (the kind parameter will allow for short, and long sizes)
     string value = descOrDigit->text;

     bool isNumber = true;
     unsigned long int i = 0;
     while (i < value.length())
        {
          isNumber = isNumber && isdigit(value[i]);
          i++;
        }
  // printf ("In c_action_format_item(): isNumber = %s \n",isNumber ? "true" : "false");

     if (isNumber == true)
        {
       // long strtol(const char *restrict str, char **restrict endptr, int base);
       // int repeat_specifier = atoi(value.c_str());
          int repeat_specifier = atoi(value.c_str());

       // SgIntVal* integerValue = new SgIntVal (repeat_specifier,value);
       // ROSE_ASSERT(integerValue != NULL);
       // setSourcePosition(integerValue,descOrDigit);
       // integerValue->set_parent(formatItem);

          formatItem->set_repeat_specification(repeat_specifier);

       // ROSE_ASSERT(astNodeStack.empty() == false);
          if (astNodeStack.empty() == false)
             {
            // This is the case of "format (10/)" which processes "10" and "/" seperately (I think this is a bug, see test2007_241.f).
               SgFormatItemList* formatItemList = isSgFormatItemList(astNodeStack.front());
               astNodeStack.pop_front();

               formatItem->set_format_item_list(formatItemList);
               formatItemList->set_parent(formatItem);
             }
        }
       else
        {
       // This is the Fortran default, but we need to test this since double quotes are an alterrnative
          bool usingSingleQuotes = value[0] == '\'';
          bool usingDoubleQuotes = value[0] == '\"';

#if 0
          printf ("usingSingleQuotes = %s \n",usingSingleQuotes ? "true" : "false");
          printf ("usingDoubleQuotes = %s \n",usingDoubleQuotes ? "true" : "false");
#endif

          string subStringWithoutQuotes;
          if (usingSingleQuotes || usingDoubleQuotes)
             {
               ROSE_ASSERT( usingSingleQuotes ||  usingDoubleQuotes);
               ROSE_ASSERT(!usingSingleQuotes || !usingDoubleQuotes);

               subStringWithoutQuotes = value.substr(1,value.length()-2);
             }
            else
             {
            // there were no quotes used
               subStringWithoutQuotes = value;
             }

       // printf ("value = %s subStringWithoutQuotes = %s \n",value.c_str(),subStringWithoutQuotes.c_str());

          SgStringVal* stringValue = new SgStringVal (subStringWithoutQuotes);
          ROSE_ASSERT(stringValue != NULL);

       // Set both usesSingleQuotes and usesDoubleQuotes so that we can distinguish un-quoted strings.
          stringValue->set_usesSingleQuotes(usingSingleQuotes);
          stringValue->set_usesDoubleQuotes(usingDoubleQuotes);

          setSourcePosition(stringValue,descOrDigit);

          formatItem->set_data(stringValue);

          stringValue->set_parent(formatItem);
        }

     astNodeStack.push_front(formatItem);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R1003 c_action_format_item()");
#endif
   }

/** R1003 list
 * format_item_list
 *      :       format_item ( T_COMMA format_item )*
 * 
 * @param count The number of items in the list.
 */
void c_action_format_item_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_format_item_list_begin() \n");

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R1003 c_action_format_item_list__begin()");
#endif
   }
void c_action_format_item_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_format_item_list(): count = %d \n",count);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R1003 c_action_format_item_list()");
#endif

     SgFormatItemList* formatItemList = new SgFormatItemList();
     for (int i=0; i < count; i++)
        {
          ROSE_ASSERT(astNodeStack.empty() == false);
          SgFormatItem* formatItem = isSgFormatItem(astNodeStack.front());

          if (formatItem == NULL)
             {
               SgFormatItemList* nestedFormatItemList = isSgFormatItemList(astNodeStack.front());
               ROSE_ASSERT(nestedFormatItemList != NULL);
               formatItem = new SgFormatItem();
               formatItem->set_format_item_list(nestedFormatItemList);
            // formatItemList->get_format_item_list().insert(formatItemList->get_format_item_list().begin(),nestedFormatItemList);
               nestedFormatItemList->set_parent(formatItem);
             }

       // formatItemList->get_format_item_list().push_back(formatItem);
          formatItemList->get_format_item_list().insert(formatItemList->get_format_item_list().begin(),formatItem);

       // printf ("Set the parent of SgFormatItem = %p to SgFormatItemList = %p \n",formatItem,formatItemList);
          formatItem->set_parent(formatItemList);

          astNodeStack.pop_front();
        }

     astNodeStack.push_front(formatItemList);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R1003 c_action_format_item_list()");
#endif
   }

/** R1010 list
 * v_list_part
 * v_list
 *      :       (T_PLUS|T_MINUS)? T_DIGIT_STRING 
 *                      ( T_COMMA (T_PLUS|T_MINUS)? T_DIGIT_STRING )*
 * 
 * @param plus_minus Optional T_PLUSIT_MINUS token.
 * @param digitString The digit string token.
 * @param count The number of items in the list.
 */
void c_action_v_list_part(Token_t * plus_minus, Token_t * digitString)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_v_list_part(): plus_minus = %p = %s digitString = %p = %s \n",
               plus_minus,plus_minus != NULL ? plus_minus->text : "NULL",
               digitString,digitString != NULL ? digitString->text : "NULL");

     printf ("Unclear where this c_action is called! \n");
     ROSE_ASSERT(false);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R1010 c_action_v_list_part()");
#endif
   }
void c_action_v_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_v_list__begin() \n");

     printf ("Unclear where this c_action is called! \n");
     ROSE_ASSERT(false);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R1010 c_action_v_list__begin()");
#endif
   }
void c_action_v_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_v_list(): count = %d \n",count);

     printf ("Unclear where this c_action is called! \n");
     ROSE_ASSERT(false);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R1010 c_action_v_list()");
#endif
   }


/** R1101
 * main_program
 *      (program_stmt)? specification_part (execution_part)? (internal_subprogram_part)?
 *      end_program_stmt
 * 
 * @param hasProgramStmt Optional program-stmt
 * @param hasExecutionPart Optional execution-part
 * @param hasInternalSubprogramPart Optional internal-subprogram-part
 */
void c_action_main_program__begin()
   {
#if !SKIP_C_ACTION_IMPLEMENTATION
  // This has been refactored to a function which should be called instead.

  // DQ (8/19/2007): This is the new code to use the SgFile built by ROSE (by translator calling the Open Fortran Parser).

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_main_program__begin(): OpenFortranParser_globalFilePointer = %p \n",OpenFortranParser_globalFilePointer);

     initialize_global_scope_if_required();
#endif
   }

void c_action_main_program(ofp_bool hasProgramStmt, ofp_bool hasExecutionPart, ofp_bool hasInternalSubprogramPart)
   {
  // Now can we setup the endOfConstruct in the global scope! But we have no token to use to do this!

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_main_program() \n");

#if !SKIP_C_ACTION_IMPLEMENTATION
     ROSE_ASSERT(astScopeStack.empty() == false);

  // DQ (10/10/2010): Test ending position
     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct() != NULL);
     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct()->get_line() != astScopeStack.front()->get_startOfConstruct()->get_line());

     astScopeStack.pop_front(); // Pop off the SgBasicBlock

     ROSE_ASSERT(astScopeStack.empty() == false);

  // resetSourcePosition(astScopeStack.front(),endKeyword);
     SgStatement* lastStatement = astScopeStack.front()->lastStatement();
  // printf ("In c_action_main_program__begin(): lastStatement = %p \n",lastStatement);
     if (lastStatement != NULL)
        {
          resetEndingSourcePosition(astScopeStack.front(),lastStatement);
        }
       else
        {
       // DQ (10/10/2010): Set the end position to be on the next line (for now)
          resetEndingSourcePosition(astScopeStack.front(),astScopeStack.front()->get_endOfConstruct()->get_line()+1);
        }

  // DQ (10/10/2010): Test ending position
     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct() != NULL);
     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct()->get_line() != astScopeStack.front()->get_startOfConstruct()->get_line());

     astScopeStack.pop_front(); // Pop off the SgFunctionDefinition

     ROSE_ASSERT(astScopeStack.empty() == false);
     SgScopeStatement* topOfStack = *(astScopeStack.begin());
  // printf ("topOfStack = %p = %s \n",topOfStack,topOfStack->class_name().c_str());
     SgGlobal* globalScope = isSgGlobal(topOfStack);

     ROSE_ASSERT(globalScope != NULL);
     ROSE_ASSERT(globalScope->get_startOfConstruct() != NULL);
  // ROSE_ASSERT(globalScope->get_endOfConstruct()   == NULL);
  // ROSE_ASSERT(globalScope->get_endOfConstruct()   != NULL);

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("Leaving c_action_main_program() \n");
   }
#endif

/** R1101
 * ext_function_subprogram
 *
 * : (prefix)? function_subprogram
 *
 * @param hasPrefix True if has a prefix.
 */
void c_action_ext_function_subprogram(ofp_bool hasPrefix)
{
}

/** R1102
 * program_stmt
 * :    (label)? ...
 * 
 * @param label Optional statement label
 * @param id Optional program name
 */
// void c_action_program_stmt(Token_t * label, Token_t * id)
// void c_action_program_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3)
void c_action_program_stmt(Token_t *label, Token_t *programKeyword, Token_t *id, Token_t *eos)
   {
  // If this is called then we want to mark the SgProgramHeaderStatement (derived from a SgFunctionDeclaration)
  // as explicit in the source code (using the tokens to communicate the source position information)
  // Also need to make the SgFunctionDefinition and the SgBasicBlock (function body) as being explicit 
  // in the source code.

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_program_stmt(): label = %s id = %s \n",label ? label->text : "NULL", id ? id->text : "NULL");

#if !SKIP_C_ACTION_IMPLEMENTATION
     SgScopeStatement* topOfStack = getTopOfScopeStack();
  // printf ("topOfStack = %p = %s \n",topOfStack,topOfStack->class_name().c_str());
     ROSE_ASSERT(topOfStack->variantT() == V_SgGlobal);

//   // The id should be a valid name (else this action would not have been called, I think)
//   // Can we have an un-named program statement?
//      ROSE_ASSERT(id != NULL);
//      ROSE_ASSERT(id->line > 0);
//      ROSE_ASSERT(id->text != NULL);

  // Inconsistancy to fix: notice that we are pusing tokens to the back of the tokenList and scopes 
  // to the front of the astScopeStack.  This is however a local scope (local scope).
     TokenListType tokenList;
     if (label != NULL) 
          tokenList.push_back(label);
     tokenList.push_back(id);

  // printf ("programStatement name = %s \n",id->text);
     SgName programName = id->text;

  // We should test if this is in the function type table, but do this later
     SgFunctionType* functionType = new SgFunctionType(SgTypeVoid::createType(),false);
     SgProgramHeaderStatement* programDeclaration = new SgProgramHeaderStatement(programName,functionType,NULL);

  // This is the defining declaration and there is no non-defining declaration!
     programDeclaration->set_definingDeclaration(programDeclaration);

     programDeclaration->set_scope(topOfStack);
     programDeclaration->set_parent(topOfStack);

  // Add the program declaration to the global scope
  // topOfStack->append_statement(programDeclaration);
     SgGlobal* globalScope = isSgGlobal(topOfStack);
     ROSE_ASSERT(globalScope != NULL);
     globalScope->append_statement(programDeclaration);

  // A symbol using this name should not already exist
     ROSE_ASSERT(globalScope->symbol_exists(programName) == false);

  // Add a symbol to the symbol table in global scope
     SgFunctionSymbol* symbol = new SgFunctionSymbol(programDeclaration);
     globalScope->insert_symbol(programName, symbol);

     if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
          printf ("Inserted SgFunctionSymbol in globalScope using name = %s \n",programName.str());

  // Add a symbol to the symbol table in global scope
  // SgFunctionTypeSymbol* symbol = new SgFunctionTypeSymbol(programDeclaration,functionType);
  // globalScope->insert_symbol(programName, symbol);

  // This statement, like all statements, could have a numeric label.
  // Setup the label on the statement if it is available.
  // setStatementNumericLabel(programDeclaration,label);

     SgBasicBlock* programBody               = new SgBasicBlock();
     SgFunctionDefinition* programDefinition = new SgFunctionDefinition(programDeclaration,programBody);

  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     programBody->setCaseInsensitive(true);
     programDefinition->setCaseInsensitive(true);

     astScopeStack.push_front(programDefinition);
     astScopeStack.push_front(programBody);

     programBody->set_parent(programDefinition);
     programDefinition->set_parent(programDeclaration);

  // DQ (12/9/2007): Moved so that the label's symbol can be put into the function 
  // definition's scope.  See test2007_01.f90.
     setStatementNumericLabel(programDeclaration,label);

  // programBody->set_scope(programDefinition);
  // programDefinition->set_scope(topOfStack);

     ROSE_ASSERT(programDeclaration->get_parameterList() != NULL);

  // setSourcePosition(programDeclaration->get_parameterList());
  // setSourcePosition(programDeclaration,tokenList);
     if (programKeyword != NULL)
        {
          setSourcePosition(programDeclaration->get_parameterList(),programKeyword);
          setSourcePosition(programDeclaration,programKeyword);
        }
       else
        {
       // These will be marked as isSourcePositionUnavailableInFrontend = true and isOutputInCodeGeneration = true
       // setSourcePosition(programDeclaration->get_parameterList());
       // setSourcePosition(programDeclaration);

       // DQ (12/18/2008): These need to make marked with a valid file id (not NULL_FILE, internally),
       // so that any attached comments and CPP directives will be properly attached.
          setSourcePosition(programDeclaration->get_parameterList(),tokenList);
          setSourcePosition(programDeclaration,tokenList);

       // programDeclaration->get_startOfConstruct()->display("In c_action_program_stmt()");

       // Mark these as compiler generated
       // setSourcePositionCompilerGenerated(programDeclaration->get_parameterList());
       // setSourcePositionCompilerGenerated(programDeclaration);
        }
     
  // Unclear if we should use the same token list for resetting the source position in all three IR nodes.
     setSourcePosition(programDefinition,tokenList);
     setSourcePosition(programBody,tokenList);

  // Set the program name
  // functionDeclaration->set_name(id->get_lexeme_string());
     programDeclaration->set_name(id->text);

  // DQ (12/5/2010): This is related to a new test in the AST consistancy tests.
     ROSE_ASSERT(programDeclaration->get_firstNondefiningDeclaration() != programDeclaration);
     ROSE_ASSERT(programDeclaration->get_firstNondefiningDeclaration() == NULL);
#endif
   }

/** R1103
 * end_program_stmt
 * :    (label)? ...
 * 
 * @param label Optional statement label
 * @param id Optional program name
 */
// void c_action_end_program_stmt(Token_t * label, Token_t * id)
void c_action_end_program_stmt(Token_t *label, Token_t *endKeyword, Token_t *programKeyword, Token_t *id, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_end_program_stmt() label = %s id = %s \n",label ? label->text : "null",id ? id->text : "null");

#if !SKIP_C_ACTION_IMPLEMENTATION
  // Refactored the code to build support function
     initialize_global_scope_if_required();
     build_implicit_program_statement_if_required();
     SgScopeStatement* currentScope = getTopOfScopeStack();

  // We asume that this is a SgBasicBlock
  // SgBasicBlock* programBody = isSgBasicBlock(*(astScopeStack.begin()));
     SgBasicBlock* programBody = isSgBasicBlock(currentScope);
     ROSE_ASSERT(programBody != NULL);

     SgFunctionDefinition* programDefinition = isSgFunctionDefinition(programBody->get_parent());
     ROSE_ASSERT(programDefinition != NULL);
  // SgFunctionDeclaration* programDeclaration = programDefinition->get_declaration();
     SgProgramHeaderStatement* programDeclaration = isSgProgramHeaderStatement(programDefinition->get_declaration());
     ROSE_ASSERT(programDeclaration != NULL);

  // Set the label if it is defined
     setStatementEndNumericLabel(programDeclaration,label);

  // simple error checking
  // ROSE_ASSERT(functionDeclaration->get_name().getString() == id->get_lexeme_string());
     if (id != NULL)
        {
                 // printf("%s == %s\n", programDeclaration->get_name().getString().c_str(), id->text);
       // ROSE_ASSERT(programDeclaration->get_name().getString() == string(id->text));
          programDeclaration->set_named_in_end_statement(true);
        }

     ROSE_ASSERT(astScopeStack.empty() == false);

  // resetEndingSourcePosition(astScopeStack.front(),endKeyword,getCurrentFilename());
     resetEndingSourcePosition(astScopeStack.front(),endKeyword);

     if (programDeclaration->get_program_statement_explicit() == false)
        {
       // The function declaration should be forced to match the "end" keyword.
#if 0
          printf ("This was an implicit main function declaration. \n");
          printf ("   programDeclaration->get_startOfConstruct()->get_filename()    = %s \n",programDeclaration->get_startOfConstruct()->get_filenameString().c_str());
          printf ("   programDeclaration->get_endOfConstruct()->get_filename()      = %s \n",programDeclaration->get_endOfConstruct()->get_filenameString().c_str());
          printf ("   astScopeStack.front()->get_startOfConstruct()->get_filename() = %s \n",astScopeStack.front()->get_startOfConstruct()->get_filenameString().c_str());
          printf ("   astScopeStack.front()->get_endOfConstruct()->get_filename()   = %s \n",astScopeStack.front()->get_endOfConstruct()->get_filenameString().c_str());
#endif
       // Reset the declaration to the current filename.
          programDeclaration->get_startOfConstruct()->set_filenameString(getCurrentFilename());
          programDeclaration->get_endOfConstruct()->set_filenameString(getCurrentFilename());
        }
#if 0
     printf ("In c_action_end_program_stmt(): astScopeStack.front() = %s \n",astScopeStack.front()->class_name().c_str());
#endif

     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct()->get_line() != astScopeStack.front()->get_startOfConstruct()->get_line());
#endif
   }

/**
 * R1104
 * module
 * 
 */
void c_action_module()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_module() \n");
   }

/** R1105
 * module_stmt__begin
 *
 */
void c_action_module_stmt__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_module_stmt__begin() \n");
   }


/** R1105
 * module_stmt
 *
 * :  (label)? T_MODULE (T_IDENT)? T_EOS
 *
 * @param label The label.
 * @param id The identifier, if present, otherwise null.
 */
// void c_action_module_stmt(Token_t * label, Token_t * id)
void c_action_module_stmt(Token_t *label, Token_t *moduleKeyword, Token_t *id, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_module_stmt(): label = %p = %s moduleKeyword = %p = %s id = %p = %s \n",
               label,label != NULL ? label->text : "NULL",
               moduleKeyword,moduleKeyword != NULL ? moduleKeyword->text : "NULL",
               id,id != NULL ? id->text : "NULL");

     initialize_global_scope_if_required();
  // build_implicit_program_statement_if_required();

     ROSE_ASSERT(id != NULL);
     SgName name = id->text;

     SgModuleStatement* moduleStatement = buildModuleStatementAndDefinition(name,astScopeStack.front());

     ROSE_ASSERT(moduleKeyword != NULL);
     setSourcePosition(moduleStatement,moduleKeyword);

     astScopeStack.front()->append_statement(moduleStatement);
     moduleStatement->set_parent(astScopeStack.front());

     ROSE_ASSERT(moduleStatement->get_definition() != NULL);
     astScopeStack.push_front(moduleStatement->get_definition());
   }

/** R1106
 * end_module_stmt
 *
 * :  (label)? T_END T_MODULE (T_IDENT)? end_of_stmt
 * |  (label)? T_ENDMODULE      (T_IDENT)? end_of_stmt
 * |  (label)? T_END end_of_stmt
 *
 * @param label The label.
 * @param id The identifier, if present, otherwise null.
 */
// void c_action_end_module_stmt(Token_t * label, Token_t * id)
void c_action_end_module_stmt(Token_t *label, Token_t *endKeyword, Token_t *moduleKeyword, Token_t *id, Token_t *eos)
   {
  // printf ("SgProject::get_verbose() = %d \n",SgProject::get_verbose());
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_end_module_stmt(): id = %p = %s \n",id,id != NULL ? id->text : "NULL");

#if 0
     printf ("In c_action_end_module_stmt(): id = %p = %s \n",id,id != NULL ? id->text : "NULL");
#endif

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R1106 c_action_end_module_stmt()");
#endif

     ROSE_ASSERT(astScopeStack.front() != NULL);
     SgModuleStatement* moduleStatement = isSgModuleStatement(astScopeStack.front()->get_parent());
     ROSE_ASSERT(moduleStatement != NULL);

     SgClassDefinition* moduleScope = isSgClassDefinition(astScopeStack.front());
     ROSE_ASSERT(moduleScope != NULL);
  // printf ("moduleScope = %p = %s \n",moduleScope,moduleScope->class_name().c_str());

  // FMZ: 05/30/2008  add the subtree root to the map
     string fileName = moduleStatement->get_name();

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("Creating a module file %s using module name = %s \n",StringUtility::convertToLowerCase(fileName).c_str(),fileName.c_str());

  // DQ (11/12/2008): Modified to force filename to lower case.
     fileName = StringUtility::convertToLowerCase(fileName);

     setStatementNumericLabel(moduleStatement,label);

  // DQ (10/10/2010): Test ending position
     ROSE_ASSERT(astScopeStack.empty() == false);
     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct() != NULL);

  // DQ (10/10/2010): Set the end position using the endKeyword.
     ROSE_ASSERT(endKeyword != NULL);
     resetEndingSourcePosition(astScopeStack.front(),endKeyword);

     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct()->get_line() != astScopeStack.front()->get_startOfConstruct()->get_line());

  // Pop the module's scope
     astScopeStack.pop_front();

  // DQ (5/21/2008): This should be cleared (see test2008_35.f90, where "public" is left on the stack)
     if (astNameStack.empty() == false)
        {
          printf ("Warning: entry remaining in name stack astNameStack.front() = %s \n",astNameStack.front()->text);
        }
     astNameStack.clear();

     SgClassDefinition* moduleDefinition = moduleStatement->get_definition();
     ROSE_ASSERT(moduleDefinition != NULL);

  // Note that there can be many interface statements in a module.
     std::vector<SgInterfaceStatement*> interfaceList = moduleStatement->get_interfaces();
     for (size_t i = 0; i < interfaceList.size(); i++)
        {
          SgInterfaceStatement* interfaceStatement = interfaceList[i];

          SgScopeStatement* currentScope = interfaceStatement->get_scope();
          ROSE_ASSERT(currentScope != NULL);

       // I think that this is true, enforce it!
          ROSE_ASSERT(currentScope == moduleDefinition);

          SgName interfaceName = interfaceStatement->get_name();

       // Note that there can be many function declarations (interface bodies) in each interface statement in a module.
          std::vector<SgInterfaceBody*> interfaceBodyList = interfaceStatement->get_interface_body_list();

       // Accumulate the new SgRenameSymbol nodes into a list and then add them after we are finished traversing the symbols in the current scope!
          std::vector<SgRenameSymbol*> renameSymbolList;

          for (size_t j = 0; j < interfaceBodyList.size(); j++)
             {
               SgInterfaceBody* interfaceBody = interfaceBodyList[j];
               if (interfaceBody->get_functionDeclaration() == NULL)
                  {
                 // Fixup the functionDeclaration in the interface body
                    SgName functionName = interfaceBody->get_function_name();

                 // printf ("Fixup functionName = %s in interfaceName = %s \n",functionName.str(),interfaceName.str());

                 // DQ (9/29/2008): inject all symbols from the module's symbol table into symbol table at current scope.
                    SgSymbol* symbol = moduleDefinition->first_any_symbol();
                    while(symbol != NULL)
                       {
                         SgFunctionSymbol* functionSymbol = isSgFunctionSymbol(symbol);

                      // I don't think that the mapped function in the interface has to be public.
                      // if (isPubliclyAccessible(symbol) == true)

                         if (functionSymbol != NULL)
                            {
                           // By definition we assume that the interface name is different from the function name
                           // bool isRenamed = true;

                              ROSE_ASSERT(interfaceName != functionName);
                              ROSE_ASSERT(interfaceName != "");

                              SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(functionSymbol->get_declaration());
                              ROSE_ASSERT(functionDeclaration != NULL);

                           // printf ("Found symbol for function = %s in interfaceName = %s \n",functionDeclaration->get_name().str(),interfaceName.str());

                              if (functionDeclaration->get_name() == functionName)
                                 {
                                // Set the previously NULL function declaration stored in the SgInterfaceBody
                                   interfaceBody->set_functionDeclaration(functionDeclaration);

                                // DQ (10/8/2008): Build a symbol to map the function name to the name defined by the interface.
                                // We might want to have a different sort of symbol for this purpose since the functionSymbol
                                // and the aliasSymbol are in the same scope (where as previously the SgAliasSymbol has been 
                                // used to map symbols between scopes).
                                // SgAliasSymbol* aliasSymbol = new SgAliasSymbol(functionSymbol,/* isRenamed = true */ true,interfaceName);
                                // SgRenameSymbol* renameSymbol = new SgRenameSymbol(functionSymbol,interfaceName);
                                   SgRenameSymbol* renameSymbol = new SgRenameSymbol(functionSymbol->get_declaration(),functionSymbol,interfaceName);
#if 1
                                   if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
                                        printf ("R1106 Insert aliased symbol name = %s (renamed from functionName = %s )\n",interfaceName.str(),functionName.str());
#endif
                                // Accumulate the list of required SgRenameSymbol IR nodes, and add them after we finish the traversal over the current scops symbols
                                // currentScope->insert_symbol(interfaceName,aliasSymbol);
                                // currentScope->insert_symbol(interfaceName,renameSymbol);
                                   renameSymbolList.push_back(renameSymbol);
                                 }
                            }

                      // Look at the next symbol in the module's symbol table
                         symbol = moduleDefinition->next_any_symbol();
                       }
                  }
             }

       // Insert the list of accumulated symbols into the current scope
       // printf ("renameSymbolList.size() = %zu \n",renameSymbolList.size());
          for (size_t i = 0; i < renameSymbolList.size(); i++)
             {
               currentScope->insert_symbol(interfaceName,renameSymbolList[i]);
             }
        }

  // This is part of a bug fix demonstrated by test2010_32.f90.
  // DQ (8/28/2010): A function call in a module can apprea before it's declaration. This is supported by:
  //    1) building the function declaration 
  //    2) Building a function symbol
  //    3) Put the function sysmbol into the global scope (since it is not clear where to put it)
  //    4) Fixup function symbols of referenced function calls in the module
  // printf ("Now process function references in the module to match against functions defined in the module scope! \n");
  // printf ("Current scope = %p = %s \n",moduleScope,moduleScope->class_name().c_str());
     fixupModuleScope(moduleScope);
   }

/**
 * R1107
 * module_subprogram_part
 *
 * @param containsKeyword T_CONTAINS token.
 * @param eos T_EOS token.
 */

// DQ (2/18/2008): New version of OFP does not have function parameters for this function.
// void c_action_module_subprogram_part(Token_t* containsKeyword, Token_t* eos)
#if ROSE_OFP_MINOR_VERSION_NUMBER >= 8 & ROSE_OFP_PATCH_VERSION_NUMBER >= 0
void c_action_module_subprogram_part(int count)
#else
void c_action_module_subprogram_part()
#endif
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
        {
#if ROSE_OFP_MINOR_VERSION_NUMBER >= 8 & ROSE_OFP_PATCH_VERSION_NUMBER >= 0
          printf ("In c_action_module_subprogram_part(): count = %d \n",count);
#else
          printf ("In c_action_module_subprogram_part(): \n");
#endif
        }
     
#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R1107 c_action_module_subprogram_part()");
#endif
   }

/** R1108
 * module_subprogram
 *
 * : (prefix)? function_subprogram
 * | subroutine_subprogram
 *
 * @param ihasPrefix Boolean true if has a prefix.
 */
void c_action_module_subprogram(ofp_bool hasPrefix)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_module_subprogram(): hasPrefix = %s \n",hasPrefix ? "true" : "false");

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R1108 c_action_module_subprogram()");
#endif
   }


/** R1109
 * use_stmt
 *
 * :  (label)? T_USE ( (T_COMMA module_nature)? T_COLON_COLON )? T_IDENT 
 *                      ( T_COMMA rename_list)? T_EOS
 * |    (label)? T_USE ( (T_COMMA module_nature)? T_COLON_COLON )? T_IDENT 
 *                      T_COMMA T_ONLY T_COLON (only_list)? T_EOS
 *
 * @param label The label.
 * @param hasModuleNature True if has a module nature.
 * @param hasRenameList True if has a rename list.
 * @param hasOnly True if has an only statement, regardless of whether a list is present.
 */
// void c_action_use_stmt(Token_t * label, ofp_bool hasModuleNature, ofp_bool hasRenameList, ofp_bool hasOnly)
void c_action_use_stmt(Token_t *label, Token_t *useKeyword, Token_t *id, Token_t *onlyKeyword, Token_t *eos, ofp_bool hasModuleNature, ofp_bool hasRenameList, ofp_bool hasOnly)
   {
  // DQ (9/14/2010): I want to track the calling of use statements for debugging
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
        {
          printf ("In c_action_use_stmt(): label = %p = %s useKeyword = %p = %s id = %p = %s onlyKeyword = %p = %s hasModuleNature = %s hasRenameList = %s hasOnly = %s \n",
               label,label != NULL ? label->text : "NULL",
               useKeyword,useKeyword != NULL ? useKeyword->text : "NULL",
               id,id != NULL ? id->text : "NULL",
               onlyKeyword,onlyKeyword != NULL ? onlyKeyword->text : "NULL",
               hasModuleNature ? "true" : "false",
               hasRenameList ? "true" : "false",
               hasOnly ? "true" : "false");
        }

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R1109 c_action_use_stmt()");
#endif

  // This could be the first stament in a program.
     build_implicit_program_statement_if_required();

     ROSE_ASSERT(id != NULL);
     SgName name = id->text;

  // SgExprListExp* nameList                = NULL;
  // SgUseOnlyExpression* useOnlyExpression = NULL;
  // SgUseStatement* useStatement = new SgUseStatement(name,nameList,useOnlyExpression);

  // SgRenamePairPtrList nameList;
  // SgUseStatement* useStatement = new SgUseStatement(name,hasOnly,nameList);
     SgUseStatement* useStatement = new SgUseStatement(name,hasOnly);

     ROSE_ASSERT(useKeyword != NULL);
     setSourcePosition(useStatement,useKeyword);

     astScopeStack.front()->append_statement(useStatement);

  // printf ("Found the SgUseStatement \n");
     SgScopeStatement* currentScope = astScopeStack.front();

     SgClassSymbol* moduleSymbol = NULL;

  // DQ (10/23/2010): Added intrinsic module support.
     moduleSymbol = buildIntrinsicModule(name.str());

  // DQ (1/25/2011): I think this is always NULL.
     ROSE_ASSERT(moduleSymbol == NULL);

  // If moduleSymbol is still null then this was not an intrinsic module and we have to search a bit.
     if ( moduleSymbol == NULL )
        {
          trace_back_through_parent_scopes_searching_for_module(name,currentScope,moduleSymbol);
        }

  // If moduleSymbol is still null then we have to go and look for the *.rmod file and read in the module directly.
     SgModuleStatement* moduleStatement = NULL;
     if ( moduleSymbol == NULL )
        {
       // This is part of work with Rice, this case is currently an error until we get their 
       // support in place for reading the *.mod files of any previously declared modules 
       // from other translation units.

       // FMZ (5/28/2008) importing the module declaration from modName.rmod file save the global variable
       // printf ("Importing the module declaration from %s.rmod file \n",name.str());

       // FMZ (10/22008) in the new version (rice branch) SgFile*=>SgSourceFile
       // SgFile* savedFilePointer = OpenFortranParser_globalFilePointer;
          SgSourceFile* savedFilePointer = OpenFortranParser_globalFilePointer;
        
          FortranModuleInfo rmodFile;
        
       // need to check module/def/decl or read from .rmod file
          string modName = useStatement->get_name();

       // DQ (11/12/2008): Convert generated file name to lower case.
       // Fortran is case insensitive so this maps any module name to 
       // a unique name module file.
          modName = StringUtility::convertToLowerCase(modName);

          if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
               printf ("Calling FortranModuleInfo::getModule(modName) for modName = %s \n",modName.c_str());

       // This should open the appropriate *.rmod file using ROSE and read it.
       // DQ (1/28/2009): Currently there is a bug which causes a Fortran include
       // directive to be output in the generated module file, but it does not appear
       // to be a problem when we read the *.rmod file.
          moduleStatement = FortranModuleInfo::getModule(modName);

          if (moduleStatement == NULL)
             {
               cout << "Error : cannot find the module (module should have been seen in a previously generated file) : "<< modName << endl;
               ROSE_ASSERT(false);
             }

          ROSE_ASSERT (moduleStatement != NULL);

          OpenFortranParser_globalFilePointer = savedFilePointer;
        }
       else
        {
       // Need to use the defining declaration to get the module (class) definition.
          SgClassDeclaration* nonDefiningClassDeclaration = moduleSymbol->get_declaration();
          SgClassDeclaration* classDeclaration = isSgClassDeclaration(nonDefiningClassDeclaration->get_definingDeclaration());
          ROSE_ASSERT(classDeclaration != NULL);
#if 0
          printf ("In c_action_use_stmt(): classDeclaration = %p classDeclaration->get_definition() = %p classDeclaration->get_definingDeclaration() = %p classDeclaration->get_firstNondefiningDeclaration() = %p \n",
               classDeclaration,classDeclaration->get_definition(),classDeclaration->get_definingDeclaration(),classDeclaration->get_firstNondefiningDeclaration());
#endif
          ROSE_ASSERT(classDeclaration->get_definition() != NULL);

       // printf ("Found cached (or local) module declaration in AST for module = %s \n",name.str());

          moduleStatement = isSgModuleStatement(classDeclaration);
          ROSE_ASSERT(moduleStatement != NULL);
        }
     
  // DQ (11/12/2008): Now set the reference to the module statement from the use statement
  // (to help support general analysis).  Note that we still do all the symbol table support
  // to alias symbols in the module statement.
     useStatement->set_module(moduleStatement);

  // DQ (9/14/2010): This this should clarify that the current scope was set to be where the use
  // statement is located and that the moduleStatement is the module that is extracted from an
  // rmod file or exists in the current file and was visited previously.
     ROSE_ASSERT(currentScope != moduleStatement->get_definition());

  // Found the module, now read the module's symbols for public members...
  // printf ("Found the module, now read the symbols from the module's symbol table for all public members...\n");

  // This appears to always be false, and I think it is a bug in OFP.
  // Actually, the c_action_only steals the result, so this does work when not using the "only" option.
  // ROSE_ASSERT(hasRenameList == false);

  // Only supporting hasOnly == false in initial work.
     if (hasOnly == false)
        {
       // This case can only include optional rename entries (and it must be on the astNodeList).

          SgClassDefinition* classDefinition = moduleStatement->get_definition();
          ROSE_ASSERT(classDefinition != NULL);

       // if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          if ( SgProject::get_verbose() > 2 )
               printf ("Case hasOnly == false: astNodeStack.size() = %zu \n",astNodeStack.size());

          if (astNodeStack.empty() == true)
             {
            // There are no rename IR nodes
               ROSE_ASSERT(hasRenameList == false);

            // DQ (9/29/2008): inject all symbols from the module's symbol table into symbol table at current scope.
               SgSymbol* symbol = classDefinition->first_any_symbol();
               while(symbol != NULL)
                  {
                 // Assume for now that there is no renaming of symbols
                    if (isPubliclyAccessible(symbol) == true)
                       {
                         SgName symbolName = symbol->get_name();

                      // DQ (9/13/2010): Check if this symbol is already in the symbol table and avoid adding it redundently.
                      // This is (I think) related to a bug causing an exponential number of symbols to be used.
                      // if (classDefinition->symbol_exists(symbolName) == false)
                         if (currentScope->symbol_exists(symbolName) == false)
                            {
                              SgAliasSymbol* aliasSymbol = new SgAliasSymbol(symbol,/* isRenamed */ false);

                              if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
                                   printf ("R1109 (hasOnly == false && empty astNodeStack) Insert aliased symbol name = %s \n",symbolName.str());

                              currentScope->insert_symbol(symbolName,aliasSymbol);
                            }
                           else
                            {
                           // printf ("R1109 (hasOnly == false && astNodeStack.empty() == true) This symbol already exists (at least using this name = %s) in the symbol table. \n",symbolName.str());
                            }
                       }

                 // Look at the next symbol in the module's symbol table
                    symbol = classDefinition->next_any_symbol();
                  }
             }
            else
             {
            // There are a number of rename pairs on the stack which we have to handle
            // We handle these first and then all the other symbols (not-renamed) from the module's symbol table.

            // Handle the rename entries first
               set<SgSymbol*> setOfRenamedSymbols;
               while(astNodeStack.empty() == false)
                  {
                    SgRenamePair* renamePair = isSgRenamePair(astNodeStack.front());
                    astNodeStack.pop_front();

                    ROSE_ASSERT(renamePair != NULL);
                    useStatement->get_rename_list().push_back(renamePair);
                    renamePair->set_parent(useStatement);

                    if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
                         printf ("renamePair->get_local_name() = %s renamePair->get_use_name() = %s \n",renamePair->get_local_name().str(),renamePair->get_use_name().str());

                 // DQ (9/29/2008): inject symbols from module into symbol table at current scope.
                    SgSymbol* symbol = classDefinition->first_any_symbol();
                    while(symbol != NULL)
                       {
                      // Assume for now that there is no renaming of declarations
                         if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
                              printf ("renamePair->get_use_name() = %s symbol->get_name() = %s renamePair->get_local_name() = %s \n",renamePair->get_use_name().str(),symbol->get_name().str(),renamePair->get_local_name().str());

                      // Look for a match against the name used in the module
                         if (renamePair->get_use_name() == symbol->get_name() && isPubliclyAccessible(symbol) == true)
                            {
                              bool isRenamed = hasRenameList;
                              SgName declarationName = renamePair->get_local_name();
                              SgAliasSymbol* aliasSymbol = new SgAliasSymbol(symbol,/* isRenamed = true */ true,declarationName);
#if 1
                              if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
                                   printf ("R1109 (non-empty astNodeStack)Insert aliased symbol name = %s (renamed = %s)\n",declarationName.str(),isRenamed ? "true" : "false");
#endif
                              currentScope->insert_symbol(declarationName,aliasSymbol);

                              setOfRenamedSymbols.insert(symbol);
                            }

                      // Look at the next symbol in the module's symbol table
                         symbol = classDefinition->next_any_symbol();
                       }
#if 0
                    outputState("In R1109 c_action_use_stmt(): hasOnly == true");
#endif
                  }

            // New add the non-renamed symbols from the module's symbol table, retraverse the module's symbol table to do this.
               SgSymbol* symbol = classDefinition->first_any_symbol();
               while(symbol != NULL)
                  {
                    bool isRenamedSymbol = ( setOfRenamedSymbols.find(symbol) != setOfRenamedSymbols.end() );

                    if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
                         printf ("symbol->get_name() = %s isRenamedSymbol = %s \n",symbol->get_name().str(),isRenamedSymbol ? "true" : "false");

                    if (isRenamedSymbol == false && isPubliclyAccessible(symbol) == true)
                       {
                      // Add the symbols not renamed explicitly.
                         SgName symbolName = symbol->get_name();
                         SgAliasSymbol* aliasSymbol = new SgAliasSymbol(symbol,/* isRenamed */ false);
#if 1
                         if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
                              printf ("R1109 (add the non-renamed symbols) Insert aliased symbol name = %s (non-renamed symbol) \n",symbolName.str());
#endif
                         currentScope->insert_symbol(symbolName,aliasSymbol);
                       }

                 // Look at the next symbol in the module's symbol table
                    symbol = classDefinition->next_any_symbol();
                  }
             }
        }
       else
        {
       // For the case of hasOnly == true, we don't have to check if the current symbol table has already 
       // has symbols from the module already (semantics assumes the union of all use statements), I think.

          SgClassDefinition* classDefinition = moduleStatement->get_definition();
          ROSE_ASSERT(classDefinition != NULL);
#if 1
          outputState("In R1109 c_action_use_stmt(): hasOnly == true");
#endif
          if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
               printf ("Case hasOnly == true: astNodeStack.size() = %zu \n",astNodeStack.size());

       // SgRenamePair IR nodes are on the stack (even if there is no renaming done), this provides a uniform interface.
          while(astNodeStack.empty() == false)
             {
               SgRenamePair* renamePair = isSgRenamePair(astNodeStack.front());
               astNodeStack.pop_front();

               ROSE_ASSERT(renamePair != NULL);
               useStatement->get_rename_list().push_back(renamePair);
               renamePair->set_parent(useStatement);

               if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
                    printf ("renamePair->get_local_name() = %s renamePair->get_use_name() = %s \n",renamePair->get_local_name().str(),renamePair->get_use_name().str());

            // DQ (9/13/2010): I think this may be the cause of some inefficency in the symbol table handling, 
            // but it is only for the short number of entries on the stack, so not likely significant.
#if 0
               printf ("*** There is a more efficient way to find the symbol that matches the name of a symbol to rename. \n");
#endif
            // DQ (9/29/2008): inject symbols from module's symbol table into local symbol table at current scope.
               SgSymbol* symbol = classDefinition->first_any_symbol();
               while(symbol != NULL)
                  {
                 // Assume for now that there is no renaming of declarations
                    bool isRenamed = ( renamePair->get_use_name() != renamePair->get_local_name() );

                    SgName local_name  = renamePair->get_local_name();
                    SgName use_name    = renamePair->get_use_name();
                 // SgName declarationName = symbol->get_name();
                    SgName symbolName  = symbol->get_name();

                    if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
                         printf ("Test: local_name = %s use_name = %s symbol = %p = %s symbol name = %s \n",local_name.str(),use_name.str(),symbol,symbol->class_name().c_str(),symbolName.str());

                    if (use_name == symbol->get_name())
                       {
                      // This should be a public sysmbol, but check to make sure!
                         ROSE_ASSERT(isPubliclyAccessible(symbol) == true);

                      // DQ (9/13/2010): I think this is true by definition, so we need a better predicate in the conditional below!
                         ROSE_ASSERT(classDefinition->symbol_exists(symbolName) == true);

                      // DQ (9/13/2010): Check that the symbol using the "local_name" does not already exist in the symbol table.
                      // if (classDefinition->symbol_exists(local_name) == false)
                         if (currentScope->symbol_exists(local_name) == false)
                            {
                              SgAliasSymbol* aliasSymbol = NULL;
                              if (isRenamed == true)
                                 {
                                   aliasSymbol = new SgAliasSymbol(symbol,isRenamed,renamePair->get_local_name());
                                 }
                                else
                                 {
                                   aliasSymbol = new SgAliasSymbol(symbol,isRenamed);
                                 }
#if 0
                              if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
                                   printf ("R1109 (hasOnly == true && astNodeStack.empty() == false) Insert aliased symbol name = %s isRenamed = %s \n",local_name.str(),isRenamed ? "true" : "false");
#endif
                              currentScope->insert_symbol(local_name,aliasSymbol);
                            }
                           else
                            {
#if 0
                              printf ("R1109 (hasOnly == true && astNodeStack.empty() == false) This symbol already exists (at least using this name = %s) in the symbol table. \n",symbolName.str());
#endif
                            }
                       }
                      else
                       {
                         if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
                              printf ("The use name is different from the symbol's name (in the declaration) (%s vs. %s) \n",use_name.str(),symbolName.str());
                       }

                 // Increment to the next symbol in the module's symbol table
                    symbol = classDefinition->next_any_symbol();
                  }
#if 1
               outputState("In R1109 c_action_use_stmt(): hasOnly == true");
#endif
             }
        }

  // DQ (12/26/2010): Factor out the fixup required for where use statements are used (e.g. function return types).
     use_statement_fixup();

#if 0
  // Debugging output for tracking down exponential grown of the number of alias symbols in symbol tables (introduced as part of use statement support).
     int    line       = useStatement->get_file_info()->get_line();
     string filename   = useStatement->get_file_info()->get_filename();
     string moduleName = "empty default name";

  // DQ (9/14/2010): The use statement need not appear only in a module body (for example, it can be in the main program).
     SgClassDefinition* moduleDefinition = isSgClassDefinition(useStatement->get_parent());
     if (moduleDefinition != NULL)
        {
          ROSE_ASSERT(moduleDefinition == astScopeStack.front());
       // SgClassDeclaration* module = isSgClassDeclaration(moduleDefinition->get_declaration());
          SgClassDeclaration* moduleDeclaration = moduleDefinition->get_declaration();
          ROSE_ASSERT(moduleDeclaration != NULL);
          moduleName = moduleDeclaration->get_name();
        }

     size_t numberOfROSE_IR_Nodes = numberOfNodes();
     size_t numberOfAliasSymbols  = SgAliasSymbol::numberOfNodes();

     printf ("--- At BOTTOM of R1109 c_action_use_stmt(): moduleName = %s symbol table size = %d numberOfNodes = %zu numberOfAliasSymbols = %zu in file = %s line = %d \n",
             moduleName.c_str(),astScopeStack.front()->get_symbol_table()->size(),numberOfROSE_IR_Nodes,numberOfAliasSymbols,filename.c_str(),line);
#endif

  // astScopeStack.front()->print_symboltable("Output from R1109 c_action_use_stmt()");

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R1109 c_action_use_stmt()");
#endif
   }

/**
 * R1110
 * module_nature
 * 
 * @param nature T_INTRINSIC or T_NON_INTRINSIC token.
 */
void c_action_module_nature(Token_t *nature)
   {
  // This controls use of keyword: INTRINSIC or NON_INTRINSIC

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_module_nature(): nature = %p = %s \n",nature,nature != NULL ? nature->text : "NULL");

  // outputState("At BOTTOM of R1110 c_action_module_nature()");
   }

/**
 * R1111 
 * rename
 * 
 * @param id1 First T_IDENT for alt1 or null if alt2.
 * @param id2 Second T_IDENT for alt1 or null if alt2.
 * @param op1 First T_OPERATOR for alt2 or null if alt1.
 * @param defOp1 First T_DEFINED_OP for alt2 or null if alt1.
 * @param op2 Second T_OPERATOR for alt2 or null if alt1.
 * @param defOp2 Second T_DEFINED_OP for alt2 or null if alt1.
 */
void c_action_rename(Token_t *id1, Token_t *id2, Token_t *op1, Token_t *defOp1, Token_t *op2, Token_t *defOp2)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_rename(): id1 = %p = %s id2 = %p = %s op1 = %p = %s defOp1 = %p = %s op2 = %p = %s defOp2 = %p = %s \n",
               id1,id1 != NULL ? id1->text : "NULL",
               id2,id2 != NULL ? id2->text : "NULL",
               op1,op1 != NULL ? op1->text : "NULL",
               defOp1,defOp1 != NULL ? defOp1->text : "NULL",
               op2,op2 != NULL ? op2->text : "NULL",
               defOp2,defOp2 != NULL ? defOp2->text : "NULL");

  // Construct the name pair used
  // SgRenamePair* renamePair = new SgRenamePair(id1->text,id2->text);
     SgRenamePair* renamePair = NULL;
     if (id1 != NULL && id2 != NULL)
        {
       // This is a simple variable or defined type renaming
          renamePair = new SgRenamePair(id1->text,id2->text);

       // Since there is more than one token used to define a rename, the source position is not accurately set using just "id1".
          setSourcePosition(renamePair,id1);
        }
       else
        {
       // This is the case of an operator renaming, note that op1 and op2 will contain the token "operator" if this 
       // is an operator renaming.  These should maybe be folded into the names that are input into the SgRenamePair.
          ROSE_ASSERT(defOp1 != NULL && defOp2 != NULL);
          ROSE_ASSERT(op1 != NULL && op2 != NULL);

          ROSE_ASSERT(id1 == NULL && id2 == NULL);

          string local_name = defOp1->text;
          string use_name   = defOp2->text;
          if (op1 != NULL && op2 != NULL)
             {
               local_name = string(op1->text) + "(" + local_name + ")";
               use_name   = string(op2->text) + "(" + use_name + ")";
             }
            else
             {
            // If they are not both valid pointers, make sure that they are both NULL, else we may have missed 
            // some corner case for which I don't have a test code yet.
               ROSE_ASSERT(op1 == NULL && op2 == NULL);
             }

          renamePair = new SgRenamePair(local_name,use_name);

       // Since there is more than one token used to define a rename, the source position is not accurately set using just "id1".
          setSourcePosition(renamePair,defOp1);
        }

     ROSE_ASSERT(renamePair->get_file_info() != NULL);

     astNodeStack.push_front(renamePair);

  // Note that any variable reference to the name id1->text will use the alias symbol that will be built in later steps (R1109)

  // outputState("At BOTTOM of R1111 c_action_rename()");
   }

/** R1111 list
 * rename_list
 *      :       rename ( T_COMMA rename )*
 * 
 * @param count The number of items in the list.
 */
void c_action_rename_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_rename__begin() \n");

  // outputState("At BOTTOM of R1111 c_action_rename__begin()");
   }

void c_action_rename_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_rename_list(): count = %d \n",count);

  // Since the SgRenamePair IR nodes was built in R1111 (above), I don't think there is anything to do here.

  // outputState("At BOTTOM of R1111 c_action_rename_list()");
   }

/**
 * R1112
 * only
 *
 *   : generic-spec
 *   | only-use-name
 *   | rename
 *
 * @param hasGenericSpec True if has a generic spec.
 * @param hasRename True if has a rename
 * @param hasOnlyUseName True if has an only use name
 *
 * Modified v0.7.2 (new arguments added)
 */
void c_action_only(ofp_bool hasGenericSpec, ofp_bool hasRename, ofp_bool hasOnlyUseName)
   {
  //  - boolean hasGenericSpec, boolean hasRename, boolean hasOnlyUseName

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_only() \n");

  // outputState("At BOTTOM of R1112 c_action_only()");
   }

/** R1112 list
 * only_list
 *      :       only ( T_COMMA only )*
 * 
 * @param count The number of items in the list.
 */
void c_action_only_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_only_list__begin() \n");
   }

void c_action_only_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_only_list(): count = %d \n",count);

  // DQ (10/1/2008): Bug in OFP, discussed with Craig, will be fixed to permit 
  // this value to be passed in (assume false for now). Remove this declaration
  // when the bug is fixed to have it be passed in via the parameter list.
  // ofp_bool hasRenameList = astNameStack.empty() ? false : true;
     ofp_bool hasRenameList = astNameStack.empty() ? true : false;

  // outputState("At TOP of R1112 list c_action_only_list()");

  // printf ("In c_action_only_list(): hasRenameList = %s \n",hasRenameList ? "true" : "false");

  // If we don't have a renameList then the tokens are on the astNameStack, else 
  // they are already processed into SgRenamePair IR nodes and on the astNodeStack.
     if (hasRenameList == false)
        {
       // If there was not renaming, then build the SgRenamePair using empty names for the local name to signal
       // that there was no renaming. This permits a consistant interface when they are processed by R1109.
          for (int i = 0; i < count; i++)
             {
            // Construct the name pair for the case of the "only" clause, where there is no renaming.
               ROSE_ASSERT(astNameStack.empty() == false);
               SgName name = astNameStack.front()->text;

            // printf ("In c_action_only_list(): Building SgRenamePair for name = %s (not renamed) \n",name.str());
               if (matchingName(name,"OPERATOR") == true)
                  {
                 // Then get the next token and append it to the name.
                    astNameStack.pop_front();
                    SgName operatorName = astNameStack.front()->text;
                    name = operatorName;
                 // printf ("In c_action_only_list() this is an operator: name = %s \n",name.str());
                  }

            // Use the rename pir IR node to provide a uniform interface to the construction of the SgUseStatement, but set the local-name to be "".
            // SgRenamePair* renamePair = new SgRenamePair("",name);
               SgRenamePair* renamePair = new SgRenamePair(name,name);

               setSourcePosition(renamePair,astNameStack.front());

               ROSE_ASSERT(renamePair->get_file_info() != NULL);

               astNameStack.pop_front();

               astNodeStack.push_front(renamePair);
             }
        }
       else
        {
       // The SgRenamePair nodes should already be on the astNodeStack.
          ROSE_ASSERT(astNodeStack.empty() == false);

          if (astNameStack.empty() == false)
             {
               printf ("Error: rename list with only clause not supported yet, bug in OFP. \n");
               ROSE_ASSERT(false);
             }
          ROSE_ASSERT(astNameStack.empty() == true);
        }
#if 0
     outputState("At BOTTOM of R1112 list c_action_only_list()");
#endif
   }

/**
 * R1116
 * block_data
 *
 */
void c_action_block_data()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_block_data() \n");
   }

/** R1117
 * block_data_stmt__begin
 *
 */
void c_action_block_data_stmt__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_block_data_stmt__begin() \n");
   }

/** R1117
 * block_data_stmt
 *
 * :  (label)? T_BLOCK T_DATA (T_IDENT)? T_EOS
 * |  (label)? T_BLOCKDATA  (T_IDENT)? T_EOS
 *
 * @param label The label.
 * @param id Identifier if it exists. Otherwise, null.
 */
// void c_action_block_data_stmt(Token_t * label, Token_t * id)
void c_action_block_data_stmt(Token_t *label, Token_t *blockKeyword, Token_t *dataKeyword, Token_t *id, Token_t *eos)
   {
  // The block data statement is implemented to build a function (which initializes data)
  // Note that it can be declared with the "EXTERNAL" statement and as such it works much
  // the same as any other procedure.

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_block_data_stmt(): label = %p id = %p \n",label,id);

     ROSE_ASSERT(id != NULL);
     ROSE_ASSERT(id->text != NULL);
  // printf ("In c_action_block_data_stmt(): label = %p id->text = %s \n",label,id->text);

#if !SKIP_C_ACTION_IMPLEMENTATION
  // This could be the first function or subroutine in the program, so setup the global scope!
     initialize_global_scope_if_required();
  // build_implicit_program_statement_if_required();

     SgName name = id->text;
     SgFunctionType* functionType = new SgFunctionType(SgTypeVoid::createType(),false);

  // Note that a ProcedureHeaderStatement is derived from a SgFunctionDeclaration (and is Fortran specific).
  // The SgProcedureHeaderStatement can be used for a Fortran function, subroutine, or block data declaration.
     SgProcedureHeaderStatement* blockDataDeclaration = new SgProcedureHeaderStatement(name,functionType,NULL);

  // DQ (1/21/2008): Set the source position to avoid it being set without accurate token position information
     ROSE_ASSERT(blockKeyword != NULL);
     setSourcePosition(blockDataDeclaration,blockKeyword);
     setSourcePosition(blockDataDeclaration->get_parameterList(),blockKeyword);

  // Mark this as NOT a function, thus it is a subroutine.
  // blockDataDeclaration->set_is_a_function(false);
     blockDataDeclaration->set_subprogram_kind(SgProcedureHeaderStatement::e_block_data_subprogram_kind);

  // This does not do everything required to build a function or subroutine, but it does as much as possible
  // (factors out code so that it can be called for R1117, R1224, and R1232.
     bool hasDummyArgList = false;
     buildProcedureSupport(blockDataDeclaration,hasDummyArgList);
#endif
   }

/** R1118
 * end_block_data_stmt
 *
 * :  (label)? T_END T_BLOCK T_DATA ( T_IDENT )? end_of_stmt
 * |  (label)? T_ENDBLOCK T_DATA        ( T_IDENT )? end_of_stmt
 * |  (label)? T_END T_BLOCKDATA        ( T_IDENT )? end_of_stmt
 * |  (label)? T_ENDBLOCKDATA      ( T_IDENT )? end_of_stmt
 * |  (label)? T_END end_of_stmt
 *
 * @param label The label.
 * @param id Identifier if it exists. Otherwise, null.
 */
// void c_action_end_block_data_stmt(Token_t * label, Token_t * id)
void c_action_end_block_data_stmt(Token_t *label, Token_t *endKeyword, Token_t *blockKeyword, Token_t *dataKeyword, Token_t *id, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_end_block_data_stmt(): label = %p id= %p \n",label,id);

#if !SKIP_C_ACTION_IMPLEMENTATION
  // DQ (10/10/2010): Test ending position
     ROSE_ASSERT(astScopeStack.empty() == false);
     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct() != NULL);

  // DQ (10/10/2010): Set the end position of the block.
     ROSE_ASSERT(endKeyword != NULL);
     resetEndingSourcePosition(astScopeStack.front(),endKeyword);

     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct()->get_line() != astScopeStack.front()->get_startOfConstruct()->get_line());

  // Pop off the function body (SgBasicBlock)
     ROSE_ASSERT(astScopeStack.empty() == false);
  // printf ("astScopeStack.front() = %p = %s \n",astScopeStack.front(),astScopeStack.front()->class_name().c_str());
     astScopeStack.pop_front();

  // Pop off the function definition (SgFunctionDefinition)
     ROSE_ASSERT(astScopeStack.empty() == false);

  // DQ (10/10/2010): Test ending position
     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct() != NULL);

  // DQ (10/10/2010): Set the end position of the block.
     ROSE_ASSERT(endKeyword != NULL);
     resetEndingSourcePosition(astScopeStack.front(),endKeyword);

     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct()->get_line() != astScopeStack.front()->get_startOfConstruct()->get_line());

  // printf ("astScopeStack.front() = %p = %s \n",astScopeStack.front(),astScopeStack.front()->class_name().c_str());
     astScopeStack.pop_front();

     ROSE_ASSERT(astScopeStack.empty() == false);
     SgScopeStatement* topOfStack = getTopOfScopeStack();
  // printf ("In c_action_end_block_data_stmt(): topOfStack = %p = %s \n",topOfStack,topOfStack->class_name().c_str());
     ROSE_ASSERT(topOfStack->variantT() == V_SgGlobal);
#endif
   }

/**
 * R1201
 * interface_block
 *
 */
void c_action_interface_block()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_interface_block() \n");

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R1201 c_action_interface_block()");
#endif
   }

/**
 * R1202
 * interface_specification
 *
 */
void c_action_interface_specification()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_interface_specification() \n");
   }

/** R1203
 * interface_stmt__begin
 *
 */
void c_action_interface_stmt__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_interface_stmt__begin() \n");

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R1203 c_action_interface_stmt__begin()");
#endif
   }

/** R1203
 * interface_stmt
*
 * :    (label)? T_INTERFACE ( generic_spec )? T_EOS
 * |    (label)? T_ABSTRACT T_INTERFACE T_EOS
 *
 * @param label The label.
 * @param abstract The ABSTRACT keyword token (null if the
 * interface is not abstract).
 * @param keyword The INTERFACE keyword token.
 * @param eos End of statement token.
 * @param hasGenericSpec True if has a generic spec.
 */
void c_action_interface_stmt(Token_t *label, Token_t *abstractToken, Token_t *keyword, Token_t *eos, ofp_bool hasGenericSpec)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_interface_stmt(): kw1 = %p = %s kw2 = %p = %s hasPrefix = %s \n",
               abstractToken,abstractToken != NULL ? abstractToken->text : "NULL",
               keyword,keyword != NULL ? keyword->text : "NULL",hasGenericSpec ? "true" : "false");

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R1203 c_action_interface_stmt()");
#endif

  // This could be the first stament in a program.
     build_implicit_program_statement_if_required();

  // Note that an interface need not have a name!
  // ROSE_ASSERT(astNameStack.empty() == false);

     SgInterfaceStatement::generic_spec_enum generic_spec_kind = SgInterfaceStatement::e_unnamed_interface_type;

     string interfaceNameString;
     if (astNameStack.empty() == false)
        {
          interfaceNameString = astNameStack.front()->text;
          astNameStack.pop_front();

          generic_spec_kind = SgInterfaceStatement::e_named_interface_type;
        }

  // Use case insensitive string compare
     if (matchingName(interfaceNameString,"OPERATOR") == true)
        {
       // If this is an operator, then we have to get the name of the operator
          ROSE_ASSERT(astNameStack.empty() == false);
          string interfaceOperatorNameString = astNameStack.front()->text;

       // DQ (10/10/2008): We want to use the actual operator name if it is a .xxx. form
          interfaceNameString = interfaceOperatorNameString;

       // printf ("interfaceNameString = %s \n",interfaceNameString.c_str());
       // ROSE_ASSERT(interfaceNameString.size() > 1);

          astNameStack.pop_front();

          generic_spec_kind = SgInterfaceStatement::e_operator_interface_type;
        }
       else
        {
       // Use case insensitive string compare
          if (matchingName(interfaceNameString,"ASSIGNMENT") == true)
             {
            // If this is an assignment operator, then the "=" is not on the stack (inconsistant handling in OFP).
               ROSE_ASSERT(astNameStack.empty() == true);
               interfaceNameString = "=";
            // astNameStack.pop_front();

               generic_spec_kind = SgInterfaceStatement::e_assignment_interface_type;
             }
        }

  // SgBasicBlock* body = new SgBasicBlock();
  // setSourcePosition(body);

     ROSE_ASSERT(astScopeStack.empty() == false);
     SgScopeStatement* currentScope = astScopeStack.front();
     ROSE_ASSERT(currentScope != NULL);

     SgName interfaceName = interfaceNameString;
  // SgInterfaceStatement* interfaceStatement = new SgInterfaceStatement(interfaceName,body,currentScope);
  // SgInterfaceStatement* interfaceStatement = new SgInterfaceStatement(interfaceName,NULL);

  // DQ (10/1/2008): Changed interface to support a list of interface blocks (now a SgDeclarationStatementPtrList).
     SgInterfaceStatement* interfaceStatement = new SgInterfaceStatement(interfaceName,generic_spec_kind);

     ROSE_ASSERT(keyword != NULL);
     setSourcePosition(interfaceStatement,keyword);

     astScopeStack.front()->append_statement(interfaceStatement);

  // DQ (10/2/2008): Push the new SgInterfaceStatement onto the astInterfaceStack.
     astInterfaceStack.push_front(interfaceStatement);

  // astScopeStack.push_front(body);

  // DQ (10/8/2008): Fixup astNameStack to clear any remaining entries (see test2008_55.f90)
     if (astNameStack.empty() == false)
        {
          if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
               printf ("WARNING: astNameStack not empty in c_action_end_interface_stmt() \n");

          astNameStack.clear();
        }
     ROSE_ASSERT(astNameStack.empty() == true);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R1203 c_action_interface_stmt()");
#endif
   }

/** R1204
 * end_interface_stmt
 *
 * : (label)? T_END T_INTERFACE ( generic_spec )? T_EOS
 * | (label)? T_ENDINTERFACE    ( generic_spec )? T_EOS
 *
 * @param label The label.
 * @param kw1 The END keyword token (may be ENDINTERFACE).
 * @param kw2 The INTERFACE keyword token (may be null).
 * @param eos End of statement token.
 * @param hasGenericSpec True if has a generic spec.
 */
void c_action_end_interface_stmt(Token_t *label, Token_t *kw1, Token_t *kw2, Token_t *eos, ofp_bool hasGenericSpec)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_end_interface_stmt(): kw1 = %p = %s kw2 = %p = %s hasPrefix = %s \n",
               kw1,kw1 != NULL ? kw1->text : "NULL",kw2,kw2 != NULL ? kw2->text : "NULL",hasGenericSpec ? "true" : "false");

  // astScopeStack.pop_front();

  // DQ (10/1/2008): Fixup astNameStack to clear any remaining entries (see test2008_42.f90)
     if (astNameStack.empty() == false)
        {
          if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
               printf ("WARNING: astNameStack not empty in c_action_end_interface_stmt() \n");

          astNameStack.clear();
        }
     ROSE_ASSERT(astNameStack.empty() == true);

  // DQ (10/2/2008): Pop the interface stack
     ROSE_ASSERT(astInterfaceStack.empty() == false);
     astInterfaceStack.pop_front();

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R1204 c_action_end_interface_stmt()");
#endif
   }

/**
 * R1205
 * interface_body
 * 
 * @param hasPrefix Boolean flag for whether the optional prefix was given.
 * This only applies for alt1 (always false for alt2).
 */
void c_action_interface_body(ofp_bool hasPrefix)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_interface_body(): hasPrefix = %s \n",hasPrefix ? "true" : "false");
   }

/** R1206
 * procedure_stmt
 *
 * : (label)? ( T_MODULE )? T_PROCEDURE generic_name_list T_EOS
 *
 * @param label The label.
 * @param module Name of the module, if present.
 */
// void c_action_procedure_stmt(Token_t * label, Token_t * module)
void c_action_procedure_stmt(Token_t *label, Token_t *module, Token_t *procedureKeyword, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_procedure_stmt(): module = %p = %s procedureKeyword = %p = %s \n",module,module != NULL ? module->text : "NULL",procedureKeyword,procedureKeyword != NULL ? procedureKeyword->text : "NULL");

  // This function is called when processing function prototypes in interface blocks.
  // At this point we may not have seen the function definition yet, so there may not be a valid function declaration.
  // Except for the case of an assignment interface there should be enough information to construct the non-defining declaration.
  // For the case of an assignment interface it is not clear that we can do more than just save the name and fixup the 
  // interace statement to have a proper function declaration when we later see the function definition.

     if (astInterfaceStack.empty() == false)
        {
          SgInterfaceStatement* interfaceStatement = astInterfaceStack.front();

          ROSE_ASSERT(astNameStack.empty() == false);

       // DQ (9/7/2010): A procedure interface can have a list of names, so we have to process the list saved on the astNameStack.
       // See test2010_42.f90 for an example of this problem.
          while (astNameStack.empty() == false)
             {
               string procedure_name = astNameStack.front()->text;
            // printf ("procedure_name = %s \n",procedure_name.c_str());
               SgName name = procedure_name;
               SgFunctionDeclaration* nullFunctionDeclaration = NULL;
               SgInterfaceBody* interfaceBody = new SgInterfaceBody(name,nullFunctionDeclaration,/*use_function_name*/ true);
               interfaceStatement->get_interface_body_list().push_back(interfaceBody);
               interfaceBody->set_parent(interfaceStatement);
               setSourcePosition(interfaceBody);
               astNameStack.pop_front();
             }
        }


#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R1206 c_action_procedure_stmt()");
#endif

  // DQ (9/7/2010): Added error checking.
     ROSE_ASSERT(astNameStack.empty() == true);
   }

/** R1207
 * generic_spec
 *      :       T_IDENT
 *      |       T_OPERATOR T_LPAREN defined_operator T_RPAREN
 *      |       T_ASSIGNMENT T_LPAREN T_EQUALS T_RPAREN
 *      |       dtio_generic_spec
 *
 * @param keyword OPERATOR or ASSIGNMENT keyword (null otherwise).
 * @param name The name of the spec (null unless first option T_IDENT).
 * @param type Type of generic-spec.
 */
void c_action_generic_spec(Token_t *keyword, Token_t *name, int type)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_generic_spec(): keyword = %p = %s name = %p = %s type = %d \n",keyword,keyword != NULL ? keyword->text : "NULL",name,name != NULL ? name->text : "NULL",type);

     switch(type)
        {
          case GenericSpec_generic_name:
             {
               ROSE_ASSERT(name != NULL);
               astNameStack.push_front(name);
               break;
             }

          case GenericSpec_OPERATOR:
             {
               ROSE_ASSERT(keyword != NULL);
               astNameStack.push_front(keyword);
               break;
             }

          case GenericSpec_ASSIGNMENT:
             {
            // printf ("Operator definition not yet implemented type = GenericSpec_ASSIGNMENT \n");
            // ROSE_ASSERT(false);
               ROSE_ASSERT(keyword != NULL);
               astNameStack.push_front(keyword);
               break;
             }

          case GenericSpec_dtio_generic_spec:
             {
               printf ("Operator definition not yet implemented type = GenericSpec_dtio_generic_spec \n");
               ROSE_ASSERT(false);
             }

          default:
             {
               printf ("Error default reached type = %d \n",type);
               ROSE_ASSERT(false);
               break;
             }
        }

  // ROSE_ASSERT(name != NULL);
  // astNameStack.push_front(name);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R1207 c_action_generic_spec()");
#endif
   }

/** R1208
 * dtio_generic_spec
 *      :       T_READ T_LPAREN T_FORMATTED T_RPAREN
 *      |       T_READ T_LPAREN T_UNFORMATTED T_RPAREN
 *      |       T_WRITE T_LPAREN T_FORMATTED T_RPAREN
 *      |       T_WRITE T_LPAREN T_UNFORMATTED T_RPAREN
 *
 * @param rw Read or write token.
 * @param format Formatted or unformatted token.
 * @param type Type of dtio-generic-spec.
 */
void c_action_dtio_generic_spec(Token_t *rw, Token_t *format, int type)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_dtio_generic_spec():  rw = %p = %s format = %p = %s type = %d \n",rw,rw != NULL ? rw->text : "NULL",format,format != NULL ? format->text : "NULL",type);
   }

/** R1209
 * import_stmt
 *
 * :    (label)? T_IMPORT ( ( T_COLON_COLON )? generic_name_list)? T_EOS
 *
 * @param label The label.
 * @param importKeyword T_IMPORT token.
 * @param eos T_EOS token.
 * @param hasGenericNameList True if has generic name list.
 */
void c_action_import_stmt(Token_t *label, Token_t *importKeyword, Token_t *eos, ofp_bool hasGenericNameList)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_import_stmt(): importKeyword = %p = %s hasGenericNameList = %s \n",importKeyword,importKeyword != NULL ? importKeyword->text : "NULL",hasGenericNameList ? "true" : "false");

     SgImportStatement* importStatement = new SgImportStatement();

  // DQ (10/6/2008): It seems that we all of a sudden need thes to be set!
     importStatement->set_definingDeclaration(importStatement);
     importStatement->set_firstNondefiningDeclaration(importStatement);

     ROSE_ASSERT(importKeyword != NULL);
     setSourcePosition(importStatement,importKeyword);

     SgExpressionPtrList localList;
     while(astNameStack.empty() == false)
        {
          SgName importName = astNameStack.front()->text;
          astNameStack.pop_front();

          SgVariableSymbol* variableSymbol = trace_back_through_parent_scopes_lookup_variable_symbol(importName,astScopeStack.front());
          ROSE_ASSERT(variableSymbol != NULL);

          SgVarRefExp* variableReference = new SgVarRefExp(variableSymbol);
          setSourcePosition(variableReference);

          localList.push_back(variableReference);
        }

  // Reverse the list
     int count = localList.size();
     for (int i = count-1; i >= 0; i--)
        {
          importStatement->get_import_list().push_back(localList[i]);
        }

     astScopeStack.front()->append_statement(importStatement);
   }


/** R1210
 * external_stmt
 *
 * : (label)? T_EXTERNAL ( T_COLON_COLON )? generic_name_list T_EOS
 *
 * @param label The label.
 * @param externalKeyword T_EXTERNAL token.
 * @param eos T_EOS token.
 */
// void c_action_external_stmt(Token_t * label)
void c_action_external_stmt(Token_t *label, Token_t *externalKeyword, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_external_stmt(): label = %p \n",label);

  // An AttributeSpecification statement can be the first statement in a program
  // (see test2007_147.f, the original Fortran I code from the IBM 704 Fortran Manual).
     build_implicit_program_statement_if_required();

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R1210 c_action_external_stmt()");
#endif

  // DQ (12/11/2010): I think this is always NULL, since labels can't be used with the external statement.
     ROSE_ASSERT(label == NULL);

  // Take the name on the astNameStack and convert it to a SgFunctionCallExp

     ROSE_ASSERT(astNameStack.empty() == false);

  // This implementation will not handle a list of names yet.
  // ROSE_ASSERT(astNameStack.size() == 1);

     while (astNameStack.empty() == false)
        {
          Token_t* nameToken = astNameStack.front();
          astNameStack.pop_front();

          ROSE_ASSERT(nameToken != NULL);
          SgName name = nameToken->text;

       // SgExprListExp* expressionList = new SgExprListExp();
       // ROSE_ASSERT(expressionList != NULL);
       // setSourcePosition(expressionList);
       // astExpressionStack.push_front(expressionList);

          SgFunctionRefExp* functionRefExp = generateFunctionRefExp(nameToken);
          astExpressionStack.push_front(functionRefExp);

          SgScopeStatement* currentScope = astScopeStack.front();
          ROSE_ASSERT(currentScope != NULL);

       // Since we have changed this from a variable to a function, remove the variable symbol
          SgVariableSymbol* variableSymbol = currentScope->lookup_variable_symbol(name);
       // ROSE_ASSERT(variableSymbol != NULL);
          if (variableSymbol != NULL)
             {
               currentScope->remove_symbol(variableSymbol);
             }
        }

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("Before buildAttributeSpecificationStatement() in R1210 c_action_external_stmt()");
#endif

     buildAttributeSpecificationStatement(SgAttributeSpecificationStatement::e_externalStatement,label,externalKeyword);

#if 0
     ROSE_ASSERT(astScopeStack.empty() == false);
     astScopeStack.front()->print_symboltable("In c_action_external_stmt()");
#endif

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R1210 c_action_external_stmt()");
#endif
   }

/** R1211
 * procedure_declaration_stmt
 *
 *
 * @param label The label.
 * @param hasProcInterface True if has a procedure interface.
 * @param count Number of procedure attribute specifications.
 */
// void c_action_procedure_declaration_stmt(Token_t * label, ofp_bool hasProcInterface, int count)
void c_action_procedure_declaration_stmt(Token_t *label, Token_t *procedureKeyword, Token_t *eos, ofp_bool hasProcInterface, int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_procedure_declaration_stmt(): label = %p = %s procedureKeyword = %p = %s eos = %p hasProcInterface = %s count = %d \n",
               label,label != NULL ? label->text : "NULL",procedureKeyword,procedureKeyword != NULL ? procedureKeyword->text : "NULL",eos,hasProcInterface ? "true" : "false",count);

  // We have got to this point and not had to build a containing main function then we will not
  // likely be any further before we process an action statement (not declaration statement).
     build_implicit_program_statement_if_required();

#if 0
  // DQ (1/18/2011): Also called by R510 and R443.
     processMultidimensionalSubscriptsIntoExpressionList(count);
#endif

  // DQ (1/24/2011): Test2011_25.f90 demonstrates a use of a procedure attribure spec.
  // printf ("Sorry, procedure declaration statements are not implemented yet. \n");
   }

/** R1212
 * proc_interface
 *      :       T_IDENT
 *      |       declaration_type_spec
 *
 * @param id The interface name.
 */

/** R1213
 * proc_attr_spec
 *      :       access_spec
 *      |       proc_language_binding_spec
 *      |       T_INTENT T_LPAREN intent_spec T_RPAREN
 *      |       T_OPTIONAL
 *      |       T_POINTER
 *      |       T_SAVE
 *
 * @param spec The procedure attribute specification.
 */
// void c_action_proc_attr_spec(int spec)
void c_action_proc_attr_spec(Token_t * attrKeyword, Token_t * id, int spec)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_proc_attr_spec(): attrKeyword = %p = %s id = %p = %s spec = %d \n",attrKeyword,attrKeyword != NULL ? attrKeyword->text : "NULL",id,id != NULL ? id->text : "NULL",spec);

  // DQ (1/24/2011): Test2011_25.f90 demonstrates a use of a procedure attribure spec.
     printf ("Sorry, procedure attributes specs are not implemented yet. \n");
   }

/** R1214
 * proc_decl
 *      :       T_IDENT ( T_EQ_GT null_init {hasNullInit=true;} )?
 *      
 * @param id The name of the procedure.
 * @param hasNullInit True if null-init is present.
 */
void c_action_proc_decl(Token_t * id, ofp_bool hasNullInit)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_proc_decl(): id = %p = %s hasNullInit = %s \n",id,id != NULL ? id->text : "NULL",hasNullInit ? "true" : "false");

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R1214 c_action_proc_decl()");
#endif

  // DQ (1/25/2011): This could be the first statement in a program (see test2011_35.f90).
     build_implicit_program_statement_if_required();

  // DQ (1/24/2011): In this function we build a variable as a procedure and define it as a function type 
  // AND we build it as a variable with a symbol.  Not clear if this is how I really want to support fortran 
  // procedure pointers.

     ROSE_ASSERT(id != NULL);
     astNameStack.push_front(id);

     SgFunctionType* functionType = new SgFunctionType(SgTypeVoid::createType(),false);
     astBaseTypeStack.push_front(functionType);

  // DQ (1/24/2011): Add an assocciated function "procedure()" to the global scope so that 
  // fortran can build procedure pointers.
     SgScopeStatement* topOfStack = *(astScopeStack.rbegin());
     ROSE_ASSERT(topOfStack != NULL);

  // printf ("topOfStack = %p = %s \n",topOfStack,topOfStack->class_name().c_str());
     SgGlobal* globalScope = isSgGlobal(topOfStack);
     ROSE_ASSERT(globalScope != NULL);

  // printf ("topOfStack = %p = %s \n",topOfStack,topOfStack->class_name().c_str());
     ROSE_ASSERT(topOfStack->variantT() == V_SgGlobal);
     SgName programName = "procedure";
  // SgProgramHeaderStatement* programDeclaration = new SgProgramHeaderStatement(programName,functionType,NULL);
     SgProcedureHeaderStatement* programDeclaration = new SgProcedureHeaderStatement(programName,functionType,NULL);

     ROSE_ASSERT(id != NULL);

     setSourcePosition(programDeclaration->get_parameterList(),id);
     setSourcePosition(programDeclaration,id);

  // This is the defining declaration and there is no non-defining declaration!
     programDeclaration->set_definingDeclaration(programDeclaration);

     programDeclaration->set_scope(topOfStack);
     programDeclaration->set_parent(topOfStack);

  // Add the program declaration to the global scope
  // topOfStack->append_statement(programDeclaration);
  // SgGlobal* globalScope = isSgGlobal(topOfStack);
     ROSE_ASSERT(globalScope != NULL);
  // globalScope->append_statement(programDeclaration);

  // A symbol using this name should not already exist
  // ROSE_ASSERT(globalScope->symbol_exists(programName) == false);
     if (globalScope->symbol_exists(programName) == false)
        {
       // Add a symbol to the symbol table in global scope
          SgFunctionSymbol* functionSymbol = new SgFunctionSymbol(programDeclaration);
          globalScope->insert_symbol(programName, functionSymbol);

          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
               printf ("Inserted SgFunctionSymbol in globalScope using name = %s \n",programName.str());

       // SgVariableSymbol* variableSymbol = new SgVariableSymbol(variableInitilizedName);
       // globalScope->insert_symbol(programName, variableSymbol);
        }

  // Now the symbol should be in place.
     ROSE_ASSERT(globalScope->symbol_exists(programName) == true);

  // DQ (1/25/2011): We can't use c_action_entity_decl() since it tests for the existence of the referenced name
  // in an outer scope this causes problems when the name was previously used in a outer scope (see test2011_32.f90
  // as an example).

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("In R1214 c_action_proc_decl(): BEFORE building a procedure pointer...");
#endif


  // DQ (1/25/2011): This support used the fortran support function: buildVariableDeclaration() in R1214 c_action_proc_decl_list(int count).
     SgName variableName = id->text;
     ROSE_ASSERT(astBaseTypeStack.empty() == false);
     SgType* type = astBaseTypeStack.front();

  // DQ (1/25/2011): We need this on the stack for when buildVariableDeclarationAndCleanupTypeStack() is called in R1214 c_action_proc_decl_list(int count).
  // astBaseTypeStack.pop_front();

     SgInitializedName* initializedName = new SgInitializedName(variableName,type,NULL,NULL,NULL);
     setSourcePosition(initializedName);

  // This will be filled in by buildVariableDeclaration (fortran support), but we set it here since it is tested below.
     initializedName->set_scope(astScopeStack.front());

  // The semantics of the call to buildVariableDeclaration (fortran support) required the SgInitializedName objects on the stack.
     astNodeStack.push_front(initializedName);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("In R1214 c_action_proc_decl(): AFTER building a procedure pointer...");
#endif

     ROSE_ASSERT(initializedName != NULL);

  // DQ (1/25/2011): Until the SgVariableDeclaration is built, we can't assert this!
  // ROSE_ASSERT(initializedName->get_symbol_from_symbol_table() != NULL);

  // printf ("In R1214 c_action_proc_decl(): initializedName = %p \n",initializedName);

     ROSE_ASSERT( initializedName->get_scope() != NULL);
  // printf ("initializedName->get_scope() = %s \n",initializedName->get_scope()->class_name().c_str());
     ROSE_ASSERT( initializedName->get_scope()->get_symbol_table() != NULL);
  // initializedName->get_scope()->get_symbol_table()->print("Output symbol table from R612");

     if (hasNullInit == true)
        {
       // We could use the stack value to build the SgInitializedName more directly.
       // See test2011_25.f90 for an example of this (procedure pointer).

       // printf ("Use the initializer on the astExpressionStack \n");
          ROSE_ASSERT(astExpressionStack.empty() == false);
          SgAssignInitializer* initializer = new SgAssignInitializer(astExpressionStack.front(),NULL);
          setSourcePosition(initializer);
          initializedName->set_initptr(initializer);
          astExpressionStack.pop_front();
        }

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R1214 c_action_proc_decl()");
#endif
   }
   
/** R1214 list
 * proc_decl_list
 *      :       proc_decl ( T_COMMA proc_decl )*
 * 
 * @param count The number of items in the list.
 */
void c_action_proc_decl_list__begin()
   {
  // I don't think we have to do anything here (unless we have to complete the definition of the base type as we do for regular variable declarations).
   }

void c_action_proc_decl_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_proc_decl_list(): count = %d \n",count);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R1214 c_action_proc_decl_list()");
#endif

     buildVariableDeclarationAndCleanupTypeStack(NULL);

     ROSE_ASSERT(astNameStack.empty() == false);
     for (int i = 0; i < count; i++)
        {
          ROSE_ASSERT(astNameStack.empty() == false);
          astNameStack.pop_front();
        }
     ROSE_ASSERT(astNameStack.empty() == true);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R1214 c_action_proc_decl_list()");
#endif
   }

/** R1216
 * intrinsic_stmt
 *
 *      (label)? T_INTRINSIC ( T_COLON_COLON )?  generic_name_list T_EOS
 *
 * @param label The label.
 */
// void c_action_intrinsic_stmt(Token_t * label)
void c_action_intrinsic_stmt(Token_t *label, Token_t *intrinsicToken, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_intrinsic_stmt(): label = %p \n",label);

  // An AttributeSpecification statement can be the first statement in a program
  // (see test2007_147.f, the original Fortran I code from the IBM 704 Fortran Manual).
     build_implicit_program_statement_if_required();

     buildAttributeSpecificationStatement(SgAttributeSpecificationStatement::e_intrinsicStatement,label,intrinsicToken);
   }

/** R1217
 * function_reference
 *      :       procedure-designator LPAREN (actual_arg_spec_list)* RPAREN
 * 
 * Called from designator_or_proc_ref to reduce ambiguities.
 * procedure-designator is replaced by data-ref thus function-reference may also
 * be matched in data-ref as an array-ref, i.e., foo(1) looks like an array
 * 
 * @param hasActualArgSpecList True if an actual-arg-spec-list is present
 */
void c_action_function_reference(ofp_bool hasActualArgSpecList)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_function_reference(): hasActualArgSpecList = %s \n",hasActualArgSpecList ? "true" : "false");
   }

/** R1218
 * call_stmt
 *      :       (label)? T_CALL procedure_designator
                ( T_LPAREN (actual_arg_spec_list)? T_RPAREN )? T_EOS
 * 
 * @param label Optional statement label
 * @param hasActionArgSpecList True if an actual-arg-spec-list is present
 */
// void c_action_call_stmt(Token_t * label, ofp_bool hasActualArgSpecList)
void c_action_call_stmt(Token_t *label, Token_t *callKeyword, Token_t *eos, ofp_bool hasActualArgSpecList)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_call_stmt(): label = %p = %s callKeyword = %p = %s hasActualArgSpecList = %s \n",
               label,label != NULL ? label->text : "NULL",callKeyword,callKeyword != NULL ? callKeyword->text : "NULL",hasActualArgSpecList ? "true" : "false");

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R1218 c_action_call_stmt()");
#endif

     ROSE_ASSERT(astExpressionStack.empty() == false);
     SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(astExpressionStack.front());

     if (functionCallExp == NULL)
        {
       // If the syntax didn't include "()" then we get a SgFunctionRefExp on the stack and we have to build the SgFunctionCallExp.

          ROSE_ASSERT(isSgFunctionRefExp(astExpressionStack.front()) != NULL);
          printf ("We got a SgFunctionRefExp instead of a SgFunctionCallExp \n");
          ROSE_ASSERT(false);
        }

     ROSE_ASSERT(functionCallExp != NULL);

     astExpressionStack.pop_front();

     SgExprStatement* expressionStatement = new SgExprStatement(functionCallExp);

  // Since this is used in a subroutine call, we know that the function is a subroutine, 
  // so mark it as such so that the unparser will generate the correct code.
     SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(functionCallExp->get_function());
     ROSE_ASSERT(functionRefExp != NULL);
     SgFunctionSymbol* functionSymbol = functionRefExp->get_symbol();
     ROSE_ASSERT(functionSymbol != NULL);
     SgFunctionDeclaration* functionDeclaration = functionSymbol->get_declaration();

  // printf ("In R1218 c_action_call_stmt(): functionDeclaration = %p = %s name = %s \n",functionDeclaration,functionDeclaration->class_name().c_str(),functionDeclaration->get_name().str());

  // SgProgramHeaderStatement* programHeaderStatement = isSgProgramHeaderStatement(functionDeclaration);
     SgProcedureHeaderStatement* procedureHeaderStatement = isSgProcedureHeaderStatement(functionDeclaration);
     ROSE_ASSERT(procedureHeaderStatement != NULL);
  // procedureHeaderStatement->set_subprogram_kind(SgProcedureHeaderStatement::e_function_subprogram_kind);
     procedureHeaderStatement->set_subprogram_kind(SgProcedureHeaderStatement::e_subroutine_subprogram_kind);

     ROSE_ASSERT(callKeyword != NULL);
     setSourcePosition(expressionStatement,callKeyword);

     setStatementNumericLabel(expressionStatement,label);

     astScopeStack.front()->append_statement(expressionStatement);

  // Set or clear the astLabelSymbolStack (since it is redundant with the label being passed in)
  // There are two mechanisms used to set labels, and we never know which will be used by OFP.
     setStatementNumericLabelUsingStack(expressionStatement);

  // DQ (8/25/2010): Check if there is a proper symbol in the symbol table.
  // SgSymbol* local_symbol = subroutineDeclaration->get_symbol_from_symbol_table();
  // SgFunctionSymbol* functionSymbol = trace_back_through_parent_scopes_lookup_function_symbol(procedureDeclaration->get_name(),currentScopeOfFunctionDeclaration);
  // ROSE_ASSERT(functionSymbol != NULL);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R1218 c_action_call_stmt()");
#endif
   }

/**
 * R1219
 * procedure_designator
 *
 */
void c_action_procedure_designator()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_procedure_designator() \n");

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R1219 c_action_procedure_designator()");
#endif

  // This means that the item on the astExpressionStack should have been a function call expression.
  // printf ("This means that the item on the astExpressionStack should have been a function call expression \n");
  // DQ (5/15/2008): Temp code!

  // Note that convertExpressionOnStackToFunctionCallExp() will also inforce that "astExpressionStack.empty() == false"
     ROSE_ASSERT(astExpressionStack.empty() == false);
     if (astExpressionStack.empty() == false)
        {
          convertExpressionOnStackToFunctionCallExp();
        }

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R1219 c_action_procedure_designator()");
#endif
   }

/** R1220
 * actual_arg_spec
 *      :       (T_IDENT T_EQUALS)? actual_arg
 *
 * R619, section_subscript has been combined with actual_arg_spec (R1220) 
 * to reduce backtracking thus R619 is called from R1220.
 * 
 * @param keyword The keyword is the name of the dummy argument in the explicit
 * interface of the procedure.
 */
void c_action_actual_arg_spec(Token_t * keyword)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_actual_arg_spec() keyword = %p = %s \n",keyword,(keyword != NULL) ? keyword->text : "NULL");

  // This handles the case of "DIM" in "sum(array,DIM=1)"

  // DQ (1/30/2011): This is allowed to be NULL, see test2010_164.f90.
  // ROSE_ASSERT(keyword != NULL);
     if (keyword != NULL)
        {
       // DQ (11/30/2007): Actual arguments have associated names which have to be recorded on to a separate stack.
       // test2007_162.h demonstrates this problems (and test2007_184.f)
       // astNameStack.push_front(keyword);
          astActualArgumentNameStack.push_front(keyword);
        }

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R1220 c_action_actual_arg_spec()");
#endif
   }

/** R1220 list
 * actual_arg_spec_list
 *      :       actual_arg_spec ( T_COMMA actual_arg_spec )*
 *
 * List begin may be called incorrectly from substring_range_or_arg_list.  This
 * will be noted by a count of less than zero.
 *
 * @param count The number of items in the list.  If count is less than zero, clean
 * up the effects of list begin (as if it had not been called).
 */
void c_action_actual_arg_spec_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_actual_arg_spec_list__begin() \n");

  // DQ (12/3/2010): This is called by test2010_119.f90.
  // printf ("I don't think that this is ever called! c_action_actual_arg_spec_list__begin() \n");
  // FMZ ROSE_ASSERT(false);
   }

void c_action_actual_arg_spec_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_actual_arg_spec_list() count = %d \n",count);

  // DQ (12/3/2010): This is called by test2010_119.f90.
  // printf ("I don't think that this is ever called! c_action_actual_arg_spec_list() \n");
  // FMZ ROSE_ASSERT(false);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R1220 (list) c_action_actual_arg_spec_list()");
#endif
   }

/** R1221
 * actual_arg
 *      :       expr
 *      |       T_ASTERISK label
 *
 * ERR_CHK 1221 ensure ( expr | designator ending in T_PERCENT T_IDENT)
 * T_IDENT inlined for procedure_name
 * 
 * @param hasExpr True if actual-arg is an expression.
 * @param label The label of the alt-return-spec (if not null).
 */
void c_action_actual_arg(ofp_bool hasExpr, Token_t * label)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_actual_arg() hasExpr = %s label = %p = %s \n",hasExpr ? "true" : "false",label,(label != NULL) ? label->text : "NULL");
   }

/**
 * R1223
 * function_subprogram
 * 
 * @param hasExePart Flag specifying if optional execution_part was given.
 * @param hasIntSubProg Flag specifying if optional 
 * internal_subprogram_part was given.
 */
void c_action_function_subprogram(ofp_bool hasExePart, ofp_bool hasIntSubProg)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_function_subprogram() hasExePart = %s hasIntSubProg = %s \n",hasExePart ? "true" : "false",hasIntSubProg ? "true" : "false");
   }

/** R1224
 * function_stmt
 *
 * : (label)? T_FUNCTION T_IDENT
         *              T_LPAREN ( generic_name_list )? T_RPAREN ( suffix )? T_EOS
 *
 * @param label The label.
 * @param keyword The ENTRY keyword token.
 * @param name The name of the function.
 * @param eos End of statement token.
 * @param hasGenericNameList True if has a generic name list.
 * @param hasSuffix True if has a suffix.
 */
void c_action_function_stmt__begin() {};

// void c_action_function_stmt(Token_t * label, ofp_bool hasGenericNameList, ofp_bool hasSuffix)
// void c_action_function_stmt(Token_t * label, Token_t * functionName, ofp_bool hasGenericNameList, ofp_bool hasSuffix)
void c_action_function_stmt(Token_t * label, Token_t * keyword, Token_t * name, Token_t * eos, ofp_bool hasGenericNameList, ofp_bool hasSuffix)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_function_stmt(): label = %p (function name) name = %s hasGenericNameList = %s hasSuffix = %s \n",
               label,name ? name->text : "NULL",hasGenericNameList ? "true" : "false",hasSuffix ? "true" : "false");

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R1224 c_action_function_stmt()");
#endif

#if !SKIP_C_ACTION_IMPLEMENTATION
  // *** Note that function and subroutine code is nearly the same ***

  // *** This setup is the same for functions, subroutines, and the program statement ***

  // This could be the first function or subroutine in the program, so setup the global scope!
     initialize_global_scope_if_required();
  // build_implicit_program_statement_if_required();

  // We will only know the name when we get to the end? No, this appears to be a bug in OFP.
  // SgName tempName = "temp_function_name";
     ROSE_ASSERT(name != NULL);
     ROSE_ASSERT(name->text != NULL);
     SgName tempName = name->text;

  // ROSE_ASSERT(astBaseTypeStack.empty() == false);
     SgType* returnType = NULL;
     if (astBaseTypeStack.empty() == false)
        {
          returnType = astBaseTypeStack.front();
          astBaseTypeStack.pop_front();
        }
       else
        {
       // Note that this type might have to be replaced if the older stype syntax is seen that defines the function type (in the next line)
       // FMZ(6/9/2010): this cause error: when a function doesn't have type, with implicit none we need to issue error message.
       // p.s.  This is not ROSE bug! This is caused by our (Rice folks) using "skip_syntax_check". 
          returnType=SgTypeVoid::createType();
        }

     ROSE_ASSERT(returnType != NULL);

  // Note that since each function builds a new function type, we will have to in a post processing step coalesce 
  // the function types used in the program.  The advantage of not sharing function types is that the return type 
  // is each to change as we parse the function specification statement section (before the executable statment section).
     SgFunctionType* functionType = new SgFunctionType(returnType,false);

  // Note that a ProcedureHeaderStatement is derived from a SgFunctionDeclaration (and is Fortran specific).
     SgProcedureHeaderStatement* functionDeclaration = new SgProcedureHeaderStatement(tempName,functionType,NULL);

  // DQ (1/21/2008): Set the source position to avoid it being set without accurate token position information
  // setSourcePosition(functionDeclaration,name);
     ROSE_ASSERT(keyword != NULL);
     setSourcePosition(functionDeclaration,keyword);
     setSourcePosition(functionDeclaration->get_parameterList(),keyword);

  // Mark this as NOT a subroutine, thus it is a function.
  // functionDeclaration->set_is_a_function(true);
     functionDeclaration->set_subprogram_kind(SgProcedureHeaderStatement::e_function_subprogram_kind);

     processFunctionPrefix(functionDeclaration);

  // This does not do everything required to build a function or subroutine, but it does as much as possible
  // (factors out code so that it can be called for R1117, R1224, and R1232.
     bool hasDummyArgList = hasGenericNameList;
     buildProcedureSupport(functionDeclaration,hasDummyArgList);
#endif

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R1224 c_action_function_stmt()");
#endif

  // DQ (11/30/2007): This should be true here. We have just build a function and so all the working stacks should be empty (except astScopeStack)!
     if (astTypeStack.empty() == false)
        {
          printf ("WARNING: clearing the astTypeStack in R1224 c_action_function_stmt() \n");
          astTypeStack.clear();
        }
     ROSE_ASSERT(astTypeStack.empty() == true);
   }


/**
 * R1225
 * proc_language_binding_spec
 *
 */
void c_action_proc_language_binding_spec()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_proc_language_binding_spec() \n");
   }

/** R1227
 * prefix
 *
 * :  prefix_spec ( prefix_spec (prefix_spec)? )?
 *
 * @param specCount  Number of specs.
 */
void c_action_prefix(int specCount)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_prefix(): specCount = %d \n",specCount);
   }

/** R1227
 * t_prefix
 *
 * :  t_prefix_spec (t_prefix_spec)?
 *
 * @param specCount  Number of specifiers.
 */
void c_action_t_prefix(int specCount)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_t_prefix(): specCount = %d \n",specCount);
   }

/** R1228
 * prefix_spec
 *
 * :  declaration_type_spec
 * |  t_prefix_spec
 *
 * @param isDecTypeSpec  True if is a declaration type spec. False if pure,
 * elemental, or recursive.
 */
void c_action_prefix_spec(ofp_bool isDecTypeSpec)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_prefix_spec(): isDecTypeSpec = %s \n",isDecTypeSpec ? "true" : "false");
   }

/** R1228
 * t_prefix_spec
 *
 * :  T_RECURSIVE
 * |  T_PURE
 * |  T_ELEMENTAL       
 *
 * @param spec  The actual token (pure, elemental, or recursive).
 */
void c_action_t_prefix_spec(Token_t *spec)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_t_prefix_spec():  spec = %p = %s \n",spec,spec != NULL ? spec->text : "NULL");

     ROSE_ASSERT(spec != NULL);
  // astNameStack.push_front(spec);
     astFunctionAttributeStack.push_front(spec);
   }

/**
 * R1229
 * suffix
 * 
 * @param resultKeyword T_RESULT token if given; null otherwise.
 * @param hasProcLangBindSpec Flag specifying whether a 
 * proc-language-binding-spec was given.
 */
void c_action_suffix(Token_t *resultKeyword, ofp_bool hasProcLangBindSpec)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_suffix(): resultKeyword = %p = %s hasProcLangBindSpec = %s \n",resultKeyword,resultKeyword != NULL ? resultKeyword->text : "NULL",hasProcLangBindSpec ? "true" : "false");
   }
        
/**
 * Unknown rule.
 * result_name
 *
 */
void c_action_result_name()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_result_name() \n");

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of Unknown rule c_action_result_name()");
#endif

  // Capture the function return parameter.
     ROSE_ASSERT(astNameStack.empty() == false);
     SgName arg_name = astNameStack.front()->text;

  // printf ("Warning: type for return parameter to function assumed to be integer (arg_name = %s) \n",arg_name.str());

  // Build a SgInitializedName with a SgTypeDefault and fixup the type later when we see the declaration inside the subroutine.
  // SgInitializedName* initializedName = new SgInitializedName(arg_name,SgTypeDefault::createType());
  // SgInitializedName* initializedName = new SgInitializedName(arg_name,SgTypeInt::createType());
     SgInitializedName* initializedName = new SgInitializedName(arg_name,generateImplicitType(arg_name.str()));

  // printf ("In c_action_result_name(): initializedName = %p = %s \n",initializedName,initializedName->get_name().str());

     astNodeStack.push_front(initializedName);

     setSourcePosition(initializedName,astNameStack.front());

     astNameStack.pop_front();
   }

/** R1230
 * end_function_stmt
 *
 * : (label)? T_END T_FUNCTION ( T_IDENT )? end_of_stmt
 * | (label)? T_ENDFUNCTION     ( T_IDENT )? end_of_stmt
 * | (label)? T_END end_of_stmt
 *
 * @param label The label.
 * @param keyword1 The END or ENDFUNCTION keyword token.
 * @param keyword2 The FUNCTION keyword token (may be null).
 * @param name The name of the function.
 * @param eos End of statement token.
 * @param id The identifier, if present. Otherwise null.
 */
// void c_action_end_function_stmt(Token_t * label, Token_t * id)
void c_action_end_function_stmt(Token_t * label, Token_t * keyword1, Token_t * keyword2, Token_t * name, Token_t * eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
        {
          printf ("In R1230 c_action_end_function_stmt(): label = %p = %s keyword1 = %p = %s keyword2 = %p = %s name = %p = %s \n",
               label,label != NULL ? label->text : "NULL",
               keyword1,keyword1 != NULL ? keyword1->text : "NULL",
               keyword2,keyword2 != NULL ? keyword2->text : "NULL",
               name,name != NULL ? name->text : "NULL");
        }

#if !SKIP_C_ACTION_IMPLEMENTATION
  // *** Note that functions and subroutine code is the same ***

  // Pop off the function body (SgBasicBlock)
     ROSE_ASSERT(astScopeStack.empty() == false);

  // DQ (10/10/2010): Test ending position
     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct() != NULL);

     ROSE_ASSERT(keyword1 != NULL);
     resetEndingSourcePosition(astScopeStack.front(),keyword1);

     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct()->get_line() != astScopeStack.front()->get_startOfConstruct()->get_line());

     astScopeStack.pop_front();

  // Pop off the function definition (SgFunctionDefinition)
     ROSE_ASSERT(astScopeStack.empty() == false);
     SgFunctionDefinition* functionDefinition = isSgFunctionDefinition(getTopOfScopeStack());

  // FMZ(6/9/2010): with "implicit none" presented, a function must have type explicitly declared
     ROSE_ASSERT(functionDefinition != NULL);
     SgProcedureHeaderStatement* func_decl = isSgProcedureHeaderStatement(functionDefinition->get_declaration());
     string func_name = func_decl->get_name().str();
     bool isAnImplicitScope = isImplicitNoneScope();

     if (func_decl!= NULL && func_decl->isFunction() == true)
        {
          SgFunctionType*  func_type = func_decl->get_type();
          SgType* func_return_type = func_type->get_return_type();

          if (isSgTypeVoid(func_return_type) !=NULL)
             {
               if (isAnImplicitScope == true)
                  {
                    cout << "Error: '" << func_name << "' has not been explicitly declared." << endl;
                  }
                 else
                  {
                    SgFunctionType* implicit_type = generateImplicitFunctionType(func_name);
                    ROSE_ASSERT(implicit_type != NULL);
                    func_decl->set_type(implicit_type);
                  }
             }
        }

  // DQ (10/10/2010): Test ending position
     ROSE_ASSERT(astScopeStack.empty() == false);
     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct() != NULL);

     ROSE_ASSERT(keyword1 != NULL);
     resetEndingSourcePosition(astScopeStack.front(),keyword1);

     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct()->get_line() != astScopeStack.front()->get_startOfConstruct()->get_line());

     astScopeStack.pop_front();

  // SgScopeStatement* topOfStack = getTopOfScopeStack();
  // printf ("topOfStack = %p = %s \n",topOfStack,topOfStack->class_name().c_str());
  
  // DQ (2/1/2008): This does not have to be true since a function can be defined as a nested 
  // function in an existing function (SgProgramHeaderStatement).
  // ROSE_ASSERT(topOfStack->variantT() == V_SgGlobal);

     if (name != NULL)
        {
       // If the id is NULL then the name was not provided.
          SgFunctionDeclaration* functionDeclaration = functionDefinition->get_declaration();
          ROSE_ASSERT(functionDeclaration != NULL);

          functionDeclaration->set_named_in_end_statement(true);
        }
#endif
   }

/** R1232
 * subroutine_stmt__begin
 *
 */
void c_action_subroutine_stmt__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_subroutine_stmt__begin(): nothing to do here! \n");
   }

/** R1232
 * subroutine_stmt
 *
 *  (label)? (t_prefix )? T_SUBROUTINE T_IDENT
 *              ( T_LPAREN ( dummy_arg_list )? T_RPAREN 
 *              ( proc_language_binding_spec )? )? T_EOS
 *
 * @param label The label.
 * @param keyword The SUBROUTINE keyword token.
 * @param name The name of the subroutine.
 * @param eos End of statement token.
 * @param hasPrefix True if has a prefix
 * @param hasDummyArgList True if has an argument list.
 * @param hasBindingSpec True is has a binding spec.
 * @param hasArgSpecifier True if has anything between parentheses following statement.
 */
// void c_action_subroutine_stmt(Token_t * label, Token_t * routineName, ofp_bool hasPrefix, ofp_bool hasDummyArgList, ofp_bool hasBindingSpec, ofp_bool hasArgSpecifier)
void c_action_subroutine_stmt(Token_t * label, Token_t * keyword, Token_t * name, Token_t * eos, ofp_bool hasPrefix, ofp_bool hasDummyArgList, ofp_bool hasBindingSpec, ofp_bool hasArgSpecifier)
   {
  // Support for subroutines maps to functions with void return type in the ROSE AST.

  // printf ("In c_action_subroutine_stmt() \n");
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_subroutine_stmt(): label = %p (routine name) name = %s hasPrefix = %s hasDummyArgList = %s hasBindingSpec = %s hasArgSpecifier = %s \n",
               label,name ? name->text : "empty",
               hasPrefix ? "true" : "false",hasDummyArgList ? "true" : "false",
               hasBindingSpec ? "true" : "false",hasArgSpecifier ? "true" : "false");

#if !SKIP_C_ACTION_IMPLEMENTATION
  // This could be the first function or subroutine in the program, so setup the global scope!

     initialize_global_scope_if_required();
  // build_implicit_program_statement_if_required();

  // We will only know the name when we get to the end? No, this appears to be a bug in OFP.
  // SgName tempName = "temp_subroutine_name";
     ROSE_ASSERT(name != NULL);
     ROSE_ASSERT(name->text != NULL);
     SgName tempName = name->text;

  // Building a void return type since a subroutine returns type void by definition.
  // However, once we see the function parameters and their type we will have to update the function type!
     SgFunctionType* functionType = new SgFunctionType(SgTypeVoid::createType(),false);

  // Note that a ProcedureHeaderStatement is derived from a SgFunctionDeclaration (and is Fortran specific).
     SgProcedureHeaderStatement* subroutineDeclaration = new SgProcedureHeaderStatement(tempName,functionType,NULL);
 
  // DQ (1/21/2008): Set the source position to avoid it being set without accurate token position information
  // setSourcePosition(subroutineDeclaration,name);
     ROSE_ASSERT(keyword != NULL);
     setSourcePosition(subroutineDeclaration,keyword);
     setSourcePosition(subroutineDeclaration->get_parameterList(),keyword);

#if 0
     subroutineDeclaration->get_startOfConstruct()->display("In c_action_subroutine_stmt()");
     subroutineDeclaration->get_endOfConstruct()->display("In c_action_subroutine_stmt()");
#endif

  // Mark this as NOT a function, thus it is a subroutine.
  // subroutineDeclaration->set_is_a_function(false);
     subroutineDeclaration->set_subprogram_kind(SgProcedureHeaderStatement::e_subroutine_subprogram_kind);

  // This has to be done before the buildProcedureSupport() function is called (must use values on the stack in a specific order).

  // Need to figure out which data is on the stack (should hasBindingSpec be processed before hasDummyArgList?).
     if (hasBindingSpec == true)
        {
       // printf ("Process binding spec ... \n");
          processBindingAttribute(subroutineDeclaration);
        }

     if (hasPrefix == true)
        {
       // This is likely the "pure" attribute

       // Output debugging information about saved state (stack) information.
       // outputState("Process hasPrefix in R1232 c_action_subroutine_stmt()");

          ROSE_ASSERT(astFunctionAttributeStack.empty() == false);

          processFunctionPrefix(subroutineDeclaration);
        }

  // This does not do everything required to build a function or subroutine, but it does as much as possible
  // (factors out code so that it can be called for R1117, R1224, and R1232.
     buildProcedureSupport(subroutineDeclaration,hasDummyArgList);

  // DQ (8/25/2010): Check if there is a proper symbol in the symbol table.
  // SgSymbol* local_symbol = subroutineDeclaration->get_symbol_from_symbol_table();
  // ROSE_ASSERT(local_symbol != NULL);
#endif

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R1232 c_action_subroutine_stmt()");
#endif
   }

/** R1233
 * dummy_arg
 *      :       T_IDENT | T_ASTERISK
 *
 * @param dummy The dummy argument token.
 */
void c_action_dummy_arg(Token_t * dummy)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_dummy_arg(): dummy = %p \n",dummy);

     ROSE_ASSERT(dummy != NULL);

  // DQ (11/15/2007): If there is more than one element then this will be false.
  // ROSE_ASSERT(astNameStack.empty() == true);

     astNameStack.push_back(dummy);
   }

/** R1233 list
 * dummy_arg_list
 *      :       dummy_arg ( T_COMMA dummy_arg )*
 * 
 * @param count The number of items in the list.
 */
void c_action_dummy_arg_list__begin()
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_dummy_arg_list__begin() \n");
   }

void c_action_dummy_arg_list(int count)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_dummy_arg_list(): count = %d \n",count);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R1233 c_action_dummy_arg_list()");
#endif
   }

/** R1234
 * end_subroutine_stmt
 *
 * : (label)? T_END T_SUBROUTINE ( T_IDENT )? end_of_stmt
 * | (label)? T_ENDSUBROUTINE   ( T_IDENT )? end_of_stmt
 * | (label)? T_END end_of_stmt
 *
 * @param label The label.
 * @param keyword1 The END or ENDSUBROUTINE keyword token.
 * @param keyword2 The SUBROUTINE keyword token (may be null).
 * @param name The name of the subroutine (may be null).
 * @param eos End of statement token.
 */
// void c_action_end_subroutine_stmt(Token_t * label, Token_t * id)
void c_action_end_subroutine_stmt(Token_t * label, Token_t * keyword1, Token_t * keyword2, Token_t * name, Token_t * eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
        {
          printf ("In R1234 c_action_end_subroutine_stmt(): label = %p = %s keyword1 = %p = %s keyword2 = %p = %s name = %p = %s \n",
               label,label != NULL ? label->text : "NULL",
               keyword1,keyword1 != NULL ? keyword1->text : "NULL",
               keyword2,keyword2 != NULL ? keyword2->text : "NULL",
               name,name != NULL ? name->text : "NULL");
        }

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R1234 c_action_end_subroutine_stmt()");
#endif

#if !SKIP_C_ACTION_IMPLEMENTATION
  // DQ (10/10/2010): Test ending position
     ROSE_ASSERT(astScopeStack.empty() == false);
     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct() != NULL);

     ROSE_ASSERT(keyword1 != NULL);
     resetEndingSourcePosition(astScopeStack.front(),keyword1);

     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct()->get_line() != astScopeStack.front()->get_startOfConstruct()->get_line());

  // Pop off the function body (SgBasicBlock)
     ROSE_ASSERT(astScopeStack.empty() == false);
  // printf ("astScopeStack.front() = %p = %s \n",astScopeStack.front(),astScopeStack.front()->class_name().c_str());
     SgBasicBlock* basicBlock = isSgBasicBlock(astScopeStack.front());
     ROSE_ASSERT(basicBlock != NULL);
     astScopeStack.pop_front();

  // Pop off the function definition (SgFunctionDefinition)
     ROSE_ASSERT(astScopeStack.empty() == false);
  // printf ("astScopeStack.front() = %p = %s \n",astScopeStack.front(),astScopeStack.front()->class_name().c_str());
     SgFunctionDefinition* functionDefinition = isSgFunctionDefinition(getTopOfScopeStack());
     ROSE_ASSERT(functionDefinition != NULL);

  // DQ (10/10/2010): Test ending position
     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct() != NULL);

     ROSE_ASSERT(keyword1 != NULL);
     resetEndingSourcePosition(astScopeStack.front(),keyword1);

     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct()->get_line() != astScopeStack.front()->get_startOfConstruct()->get_line());

     astScopeStack.pop_front();

  // SgScopeStatement* topOfStack = getTopOfScopeStack();

  // DQ (11/21/2007): This is not required, and not true for subroutines in an interface block.
  // printf ("In R1234 c_action_end_subroutine_stmt(): topOfStack = %p = %s \n",topOfStack,topOfStack->class_name().c_str());
  // ROSE_ASSERT(topOfStack->variantT() == V_SgGlobal);

#if 0
  // DQ (10/10/2010): I don't think we need this and we certainly don't want to be
  // calling globalScope->get_endOfConstruct()->setSourcePositionUnavailableInFrontend();
     topOfStack->set_endOfConstruct(Sg_File_Info::generateDefaultFileInfo());
     topOfStack->get_endOfConstruct()->set_parent(topOfStack);
     topOfStack->get_endOfConstruct()->setSourcePositionUnavailableInFrontend();
#endif

     if (name != NULL)
        {
       // If the name is NULL then the name was not provided.
          SgFunctionDeclaration* functionDeclaration = functionDefinition->get_declaration();
          ROSE_ASSERT(functionDeclaration != NULL);

          functionDeclaration->set_named_in_end_statement(true);
        }

  // printf ("In c_action_end_subroutine_stmt() astNodeStack = %zu \n",astNodeStack.size());

  // DQ (11/15/2007): Enforce that we have not left trash on the astNodeStack!
     if (astNodeStack.empty() == false)
        {
       // In possible anticipation of where statements we sometime leave some statements on the stack, but since 
       // where statements can't span function boundaries we know that we can clear them at this point.
          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
               printf ("In R1234 c_action_end_subroutine_stmt(): trash left on stack, clearing it astNodeStack.size() = %zu \n",astNodeStack.size());
          astNodeStack.clear();
        }
     ROSE_ASSERT(astNodeStack.empty() == true);
#endif

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R1234 c_action_end_subroutine_stmt()");
#endif
   }

/** R1235
 * entry_stmt
 *
 *      (label)? T_ENTRY T_IDENT
 *      ( T_LPAREN ( dummy_arg_list)? T_RPAREN (suffix)? )? T_EOS
 *
 * @param label The label.
 * @param keyword The ENTRY keyword token.
 * @param id T_IDENT for entry name.
 * @param eos End of statement token.
 * @param hasDummyArgList True if has a dummy argument list.
 * @param hasSuffix True if has a suffix.
 */
// void c_action_entry_stmt(Token_t * label, ofp_bool hasDummyArgList, ofp_bool hasSuffix)
// void c_action_entry_stmt(Token_t * label, Token_t * keyword, Token_t * eos, ofp_bool hasDummyArgList, ofp_bool hasSuffix)
void c_action_entry_stmt(Token_t * label, Token_t * keyword, Token_t * id, Token_t * eos, ofp_bool hasDummyArgList, ofp_bool hasSuffix)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
        {
          printf ("In c_action_entry_stmt(): label = %p = %s keyword = %p = %s id = %p = %s hasDummyArgList = %s hasSuffix = %s \n",
               label,label != NULL ? label->text : "NULL",keyword,keyword != NULL ? keyword->text : "NULL",
               id,id != NULL ? id->text : "NULL",hasDummyArgList ? "true" : "false",hasSuffix ? "true" : "false");
        }

  // This bug appears to be fixed in OFP as of Feb 18th 2008.
  // SgName entryFunctionName = "entry_function_name_unavailable_from_OFP";
     ROSE_ASSERT(id != NULL);
     SgName entryFunctionName = id->text;

  // Unclear how we should handle the entry, since it shares only a part of the function definition!

     SgType* returnType = NULL;
     if (astBaseTypeStack.empty() == false)
        {
          printf ("In c_action_entry_stmt(): Unclear if this case is required! \n");
          returnType = astBaseTypeStack.front();
          astBaseTypeStack.pop_front();
        }
       else
        {
          returnType = generateImplicitType(entryFunctionName.str());
        }

     ROSE_ASSERT(returnType != NULL);
     SgFunctionType* functionType = new SgFunctionType(returnType,false);

  // Reuse the current functions scope (this makes for a strange design, but entry statements are a primative concept).
  // SgFunctionDefinition* functionDefinition = NULL;
     SgFunctionDefinition* functionDefinition = getFunctionDefinitionFromScopeStack();

  // It is an error to have the entryFunctionDefinition be defined to be the functionDefinition.
  // It is conceptually wrong, but also results in a seg-fault internally in ROSE immediately 
  // after returning from OFP.  I am not clear on how to define this concept of entry function 
  // definition.
     SgFunctionDefinition* entryFunctionDefinition = NULL;

  // SgEntryStatement* entryStatement = new SgEntryStatement(entryFunctionName,functionType,functionDefinition);
     SgEntryStatement* entryStatement = new SgEntryStatement(entryFunctionName,functionType,entryFunctionDefinition);

  // Set the scope of the entryStatement (the scope is stroed explicit since SgEntryStatement is derived from a SgFunctionDeclaration
     entryStatement->set_scope(functionDefinition);

  // Get the function parameter list and set its position (this might only be required when the hasDummyArgList is false).
     ROSE_ASSERT(entryStatement->get_parameterList() != NULL);
     setSourcePosition(entryStatement->get_parameterList());

     if (hasDummyArgList == true)
        {
       // DQ (12/12/2010): This code is copied from buildProcedureSupport in fortran_support.C

       // Take the arguments off of the token stack (astNameStack).
          while (astNameStack.empty() == false)
             {
            // Capture the procedure parameters.
               SgName arg_name = astNameStack.front()->text;

            // printf ("arg_name = %s \n",arg_name.str());

            // Build a SgInitializedName with a SgTypeDefault and fixup the type later when we see the declaration inside the procedure.
               SgInitializedName* initializedName = new SgInitializedName(arg_name,generateImplicitType(arg_name.str()),NULL,entryStatement,NULL);

               entryStatement->append_arg(initializedName);

               initializedName->set_parent(entryStatement->get_parameterList());
               ROSE_ASSERT(initializedName->get_parent() != NULL);

            // DQ (12/17/2007): set the scope
               initializedName->set_scope(astScopeStack.front());

               setSourcePosition(initializedName,astNameStack.front());

               ROSE_ASSERT(astNameStack.empty() == false);
               astNameStack.pop_front();

            // Now build associated SgVariableSymbol and put it into the current scope (function definition scope)
            // SgVariableSymbol* variableSymbol = new SgVariableSymbol(initializedName);
            // entryStatement->insert_symbol(arg_name,variableSymbol);

            // DQ (12/17/2007): Make sure the scope was set!
               ROSE_ASSERT(initializedName->get_scope() != NULL);
             }

          ROSE_ASSERT(entryStatement->get_args().empty() == false);
        }

     if (hasSuffix == true)
        {
          printf ("Need to set the return type for the SgEntryStatement in R1235 \n");
       // This has been copied from buildProcedureSupport()
          if (astNodeStack.empty() == false)
             {
               SgInitializedName* returnVar = isSgInitializedName(astNodeStack.front());
               ROSE_ASSERT(returnVar != NULL);
            // returnVar->set_scope(functionBody);
               returnVar->set_parent(entryStatement);
               returnVar->set_scope(functionDefinition);
               entryStatement->set_result_name(returnVar);
               astNodeStack.pop_front();

               SgFunctionType* functionType = entryStatement->get_type();
               returnVar->set_type(functionType->get_return_type());

            // Note that for a SgEntryStatement, since it shares a scope with an existing function 
            // the symbol might already exist. Not clear how to handle this case!

            // Now build associated SgVariableSymbol and put it into the current scope (function definition scope)
               SgVariableSymbol* returnVariableSymbol = new SgVariableSymbol(returnVar);
               functionDefinition->insert_symbol(returnVar->get_name(),returnVariableSymbol);

               printf ("Processing the return var in a SgEntryStatement \n");
            // ROSE_ASSERT(false);
             }
        }

     ROSE_ASSERT(keyword != NULL);
     setSourcePosition(entryStatement,keyword);

     setStatementNumericLabel(entryStatement,label);

     astScopeStack.front()->append_statement(entryStatement);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R1235 c_action_entry_stmt()");
#endif
   }

/** R1236
 * return_stmt
 *
 *      (label)? T_RETURN ( expr )? T_EOS
 *
 * @param label The label.
 * @param keyword The RETURN keyword token.
 * @param eos End of statement token.
 * @param hasScalarIntExpr True if there is a scalar in in the return; 
 */
// void c_action_return_stmt(Token_t * label, ofp_bool hasScalarIntExpr)
void c_action_return_stmt(Token_t * label, Token_t * keyword, Token_t * eos, ofp_bool hasScalarIntExpr)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_return_stmt(): label = %s hasScalarIntExpr = %s \n",(label != NULL) ? label->text : "NULL", hasScalarIntExpr ? "true" : "false");

     SgExpression* returnValue = NULL;
     if (hasScalarIntExpr == true)
        {
          printf ("Warning: Need to get the correct expression value, returning zero \n");
          returnValue = new SgIntVal(0,"0");
        }
       else
        {
          returnValue = new SgNullExpression();
        }

     setSourcePosition(returnValue);

     SgReturnStmt* returnStatement = new SgReturnStmt(returnValue);
     returnValue->set_parent(returnStatement);

     ROSE_ASSERT(keyword != NULL);
     setSourcePosition(returnStatement,keyword);

  // DQ (12/8/2010): Added label handling support for the redundant label stack.
     processLabelOnStack(returnStatement);

     setStatementNumericLabel(returnStatement,label);

     astScopeStack.front()->append_statement(returnStatement);
   }

/** R1237
 * contains_stmt
 *
 *      (label)? T_CONTAINS ( expr )? T_EOS
 *
 * @param label The label.
 * @param keyword The CONTAINS keyword token.
 * @param eos End of statement token.
 */
void c_action_contains_stmt(Token_t *label, Token_t *keyword, Token_t *eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_contains_stmt(): label = %p = %s keyword = %p = %s \n",label,label != NULL ? label->text : "NULL",keyword,keyword != NULL ? keyword->text : "NULL");

     SgContainsStatement* containsStatement = new SgContainsStatement();
     SageInterface::setSourcePosition(containsStatement);
     containsStatement->set_definingDeclaration(containsStatement);

     astScopeStack.front()->append_statement(containsStatement);
     ROSE_ASSERT(containsStatement->get_parent() != NULL);
   }

/** R1238
 * stmt_function_stmt
 *
 *      (label)? T_STMT_FUNCTION T_IDENT T_LPAREN 
 *              ( generic_name_list )? T_RPAREN T_EQUALS expr T_EOS
 *
 * @param label The label.
 * @param functionName The name of the function.
 * @param hasGenericNameList True if there is a list in the statement.
 */
// void c_action_stmt_function_stmt(Token_t * label, ofp_bool hasGenericNameList)
// void c_action_stmt_function_stmt(Token_t * label, Token_t * functionName, ofp_bool hasGenericNameList)
void c_action_stmt_function_stmt(Token_t *label, Token_t *functionName, Token_t *eos, ofp_bool hasGenericNameList)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_stmt_function_stmt() label = %p functionName = %s hasGenericNameList = %s \n",
               label,functionName ? functionName->text : "empty", hasGenericNameList ? "true" : "false");
   }

void c_action_end_of_stmt(Token_t * eos)
   {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_end_of_stmt() astNodeStack size = %zu astExpressionStack size = %zu \n",astNodeStack.size(),astExpressionStack.size());

  // DQ (1/28/2009): If at this point we have not yet setup the function scope (see
  // test2007_19.f90) then do so.
  // build_implicit_program_statement_if_required();

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of R1238 c_action_end_of_stmt()");
#endif

#if 0
     astScopeStack.front()->print_symboltable("In c_action_end_of_stmt()");
#endif

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of R1238 c_action_end_of_stmt()");
#endif

  // DQ (1/28/2009): This should be something that we can assert now!
  // Comment this out see test2007_19.f90.
  // ROSE_ASSERT(astNodeStack.empty() == true);
   }

/*
 * start_of_file
 *
 * @param filename The name of the file
 */
void c_action_start_of_file(const char *filename)
   {
  // New function to support Fortran include mechanism
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_start_of_file(%s) \n",filename);

  // DQ: This is the earliest location to setup the global scope (I think).
     initialize_global_scope_if_required();

  // DXN: We create a SgFortranIncludeLine node only when the current file is not a top level file
  // and is not an rmod file.  When parsing a top level file, the astIncludeStack should be empty.
     if (!astIncludeStack.empty() && !isARoseModuleFile(filename))
        {
       // After the first time, ever call to this function is significant (represents use of the
       // Fortran include mechanism; not formally a part of the language grammar).

          SgFortranIncludeLine* includeLine = new SgFortranIncludeLine(filename);

          if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
               printf ("Note: Need a token to represent the the filename so that we can get the position of the include statment \n");

       // Get the last statment (this is an expensive way to do that).
          SgScopeStatement* scope = astScopeStack.front();
          SgStatement* lastStatement = scope;
          SgStatementPtrList statementList = scope->generateStatementList();
          if (statementList.empty() == false)
             {
               lastStatement = statementList.back();
             }

          int lineNumberOfLastStatement = (astScopeStack.empty() == false) ? lastStatement->get_file_info()->get_line() : 0;

       // Increment to next line since an include must be on it's own line (as I understand the Fortran standard).
          lineNumberOfLastStatement++;

          int columnNumber = 0;

       // The filename where the include is located is not the filename specified with the include!
          string filenameOfIncludeLocation = getCurrentFilename();

          Sg_File_Info* fileInfo = new Sg_File_Info(filenameOfIncludeLocation,lineNumberOfLastStatement,columnNumber);

       // We need a way to get the source position o fthe Fortran include line.
       // setSourcePositionCompilerGenerated(includeLine);
       // setSourcePosition(includeLine);
          includeLine->set_file_info(fileInfo);

       // DQ (11/20/2010): Also set the end of the construct for this statement (end of column position is not correct).
          Sg_File_Info* ending_fileInfo = new Sg_File_Info(filenameOfIncludeLocation,lineNumberOfLastStatement,columnNumber);
          includeLine->set_endOfConstruct(ending_fileInfo);

          ROSE_ASSERT(includeLine->get_endOfConstruct() != NULL);
#if 0
          includeLine->get_file_info()->display("c_action_start_of_file()");
#endif
       // DQ (1/27/2009): Make sure that the filenames match (make sure this is a STL string equality test!).
       // This is a test for a recursive file inclusion.
       // This fails for: module_A_file.f90 or module_B_file.f90.  Also for these
       // files the call to c_action_start_of_file() is triggered by a symantic
       // handling of the use statement, not the existance of the Fortran include
       // so it should be a mistake to insert an Fortran include statement!
       // ROSE_ASSERT(includeLine->get_file_info()->get_filenameString() != string(filename));

          ROSE_ASSERT(astScopeStack.empty() == false);

       // This does not always appear to be appending to the scope
          astScopeStack.front()->append_statement(includeLine);
          // patch up defining and non-defining pointers, Liao 12/7/2010
          includeLine->set_definingDeclaration(includeLine); 
          includeLine->set_firstNondefiningDeclaration(includeLine); 
        }

     astIncludeStack.push_back(filename);
   }

/*
 * end_of_file
 *
 * @param filename The name of the file
 *
 * Modified v0.7.2 (new argument added)
 */
void c_action_end_of_file(const char * filename)
   {
  // New function to support Fortran include mechanism

     ROSE_ASSERT(astIncludeStack.empty() == false);
     string filenameString = astIncludeStack.back();

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_end_of_file(): filenameString = %s \n",filenameString.c_str());

#if 0
     printf ("In c_action_end_of_file(): filenameString = %s astIncludeStack.size() = %zu \n",filenameString.c_str(),astIncludeStack.size());
#endif
     astIncludeStack.pop_back();

  // DQ (10/10/2010): Set the end position to a better value.
     SgStatement* lastStatement = astScopeStack.front()->lastStatement();
#if 0
     printf ("In c_action_end_of_file(): lastStatement = %p \n",lastStatement);
#endif
     if (lastStatement != NULL)
        {
#if 0
          printf ("In c_action_end_of_file(): lastStatement = %p = %s \n",lastStatement,lastStatement->class_name().c_str());
          printf ("In c_action_end_of_file(): lastStatement->get_startOfConstruct()->get_line() = %d \n",lastStatement->get_startOfConstruct()->get_line());

          ROSE_ASSERT(lastStatement->get_endOfConstruct() != NULL);
          printf ("In c_action_end_of_file(): lastStatement->get_endOfConstruct()->get_line()   = %d \n",lastStatement->get_endOfConstruct()->get_line());

          printf ("In c_action_end_of_file(): lastStatement->get_endOfConstruct()->get_filename()   = %s \n",lastStatement->get_endOfConstruct()->get_filenameString().c_str());
          printf ("In c_action_end_of_file(): lastStatement->get_startOfConstruct()->get_filename() = %s \n",lastStatement->get_startOfConstruct()->get_filenameString().c_str());
#endif
          ROSE_ASSERT(lastStatement->get_endOfConstruct() != NULL);
          resetEndingSourcePosition(astScopeStack.front(),lastStatement);
        }
       else
        {
       // DQ (10/10/2010): Set the end position to be on the next line (for now)
          resetEndingSourcePosition(astScopeStack.front(),astScopeStack.front()->get_endOfConstruct()->get_line()+1);
        }

#if 0
     astScopeStack.front()->get_startOfConstruct()->display("In c_action_end_of_file(): start");
     astScopeStack.front()->get_endOfConstruct  ()->display("In c_action_end_of_file(): end");

     printf ("astScopeStack.front() = %s \n",astScopeStack.front()->class_name().c_str());
#endif

  // DQ (11/11/2010): The header file can have only a single statement and if the original code 
  // has only an include then the starting line number is equal to the ending line number.
  // See test2010_81.f90 and test2010_81.h for an example of this.
  // ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct()->get_line() != astScopeStack.front()->get_startOfConstruct()->get_line());
     ROSE_ASSERT(astScopeStack.front()->get_endOfConstruct()->get_line() >= astScopeStack.front()->get_startOfConstruct()->get_line());

#if 0
     printf ("In c_action_end_of_file(): astScopeStack.front() = %s get_startOfConstruct()->get_line() = %d \n",astScopeStack.front()->class_name().c_str(),astScopeStack.front()->get_startOfConstruct()->get_line());
     printf ("In c_action_end_of_file(): astScopeStack.front() = %s get_endOfConstruct()->get_line()   = %d \n",astScopeStack.front()->class_name().c_str(),astScopeStack.front()->get_endOfConstruct()->get_line());
     astScopeStack.front()->get_startOfConstruct()->display("In c_action_end_of_file(): startOfConstruct");
     astScopeStack.front()->get_endOfConstruct()->display("In c_action_end_of_file(): endOfConstruct");
#endif

  // DQ (12/3/2010): I think that the astExpressionStack should be empty at the end of a file.
  // If this is not true then maybe it should at least be true for the last file processed.
  // For now this is just a status warning.
     if (astExpressionStack.empty() == false)
        {
          printf ("WARNING: astExpressionStack non-empty at c_action_end_of_file(filenameString = %s) \n",filenameString.c_str());
        }

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of c_action_end_of_file()");
#endif
   }

void c_action_cleanUp()
   {
  // This function permits memory to be cleaned up, not required for use in ROSE.
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_cleanUp() \n");
   }


//------------------------------------------------------------------------
// RICE IMPLEMENTATION
//------------------------------------------------------------------------
#if ROSE_OFP_MINOR_VERSION_NUMBER >= 8 & ROSE_OFP_PATCH_VERSION_NUMBER >= 0
void c_action_coarray_spec(int arg0) 
    {

     if (arg0 == 1) {
          astExpressionStack.pop_front();
     } else {
          cout << "ERROR (Rice CoArray Fortran 2.0): the co-rank must be 1." << endl;
     }



     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_coarray_spec() \n");

      astAttributeSpecStack.push_front(AttrSpec_COARRAY);

   }


void c_action_rice_co_shape_spec(const char *arg) 
    {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_co_shape_spec() \n");
    }


void c_action_allocate_coarray_spec() 
    {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_allocate_co_array_spec() \n");
    }

#endif

/**
 * carg_0 is a flag about the coarray status:
 *   0 --> no team image
 *   1 --> using a team
 *   .... --> unused
 */
// FMZ (2/9/2009): possible cases: 
/**
 *     variable declared: A(:)[*], SA[*]
 *     variable refer:    A(12)[1@team1], SA[1@team1] A(12)[1] and SA[1]
 *     generate:          SgCAFCoExpression(teamID, teamRank, Expression*)
*/
void c_action_rice_image_selector(Token_t *team_id) 
    {
     if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
        printf ("In c_action_rice_image_selector(), team_id = %p \n",team_id);

     SgVarRefExp* teamIdReference = NULL;

     if (team_id) {
       SgVariableSymbol* teamId = trace_back_through_parent_scopes_lookup_variable_symbol(team_id->text,
                                                                                       astScopeStack.front());
       if (teamId == NULL) {
            string teamName = team_id->text;  
            string teamWorld   = "team_world";
            string teamDefault = "team_default";
            std::transform(teamName.begin(),teamName.end(),teamName.begin(),::tolower); 

            if (teamName == teamWorld || teamName == teamDefault)
                        teamId =  add_external_team_decl(teamName);

        } 

       if (teamId != NULL) {
            teamIdReference = new SgVarRefExp(teamId);
            setSourcePosition(teamIdReference,team_id);
       }
     }

     SgExpression* rankExpr =  NULL;

     if (astExpressionStack.empty() == false) {
        rankExpr = astExpressionStack.front();
        // pop out the expresison of the "rank"
        astExpressionStack.pop_front();
      }

     SgExpression* dataExpr = NULL;

     SgCAFCoExpression* coExpr = new SgCAFCoExpression(teamIdReference,rankExpr,dataExpr);

     astExpressionStack.push_front(coExpr);
   }
  
/**
 * carg_0 is a flag about the coarray status:
 *   0 --> no team image
 *   1 --> using a team
 *   .... --> unused
 */
//FMZ (2/12/2009): Added CoArray image team ID
void c_action_rice_allocate_coarray_spec(int type, Token_t *team_id) {

     if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
          printf ("In c_action_rice_allocate_co_array_spec() \n");

     SgVarRefExp* teamIdReference = NULL;

     if (team_id) {
           SgVariableSymbol* teamId = trace_back_through_parent_scopes_lookup_variable_symbol(team_id->text,                                                                                                                                                 astScopeStack.front());
     if (teamId == NULL) {

         string teamName = team_id->text;  
         string teamWorld   = "team_world";
         string teamDefault = "team_default";

         std::transform(teamName.begin(),teamName.end(),teamName.begin(),::tolower); 

         if (teamName == teamWorld || teamName == teamDefault) {
                  teamId =  add_external_team_decl(teamName);
          }

       }


           teamIdReference = new SgVarRefExp(teamId);
           setSourcePosition(teamIdReference,team_id);
        }


     SgExpression* dataExpr = astExpressionStack.front();

     astExpressionStack.pop_front();

     //SgCAFCoExpression* coExpr = new SgCAFCoExpression(team_id->text,NULL,dataExpr);
     SgCAFCoExpression* coExpr = new SgCAFCoExpression(teamIdReference,NULL,dataExpr);

     coExpr->set_parent(dataExpr->get_parent());

     dataExpr->set_parent(coExpr);

     if(team_id == NULL) { //set the  position based on the subexpression's location
        // laksono 2009.01.06: when there is no team id provided, the resetSourcePosition will fail
        //      adding setSourcePosition before reset will solve this.
         setSourcePosition(coExpr);
         resetSourcePosition(coExpr,dataExpr);
     } else { //set the position based on the Token_t
         setSourcePosition(coExpr,team_id);
     }

     astExpressionStack.push_front(coExpr);
   }



/**
 * Rice's with team statemetn:
 * @param label: label of the statement
 * @param team_id: the team ID
 */
void c_action_rice_co_with_team_stmt(Token_t *label, Token_t *team_id) {
     if(team_id == NULL)
        printf("ERROR: Rice 'with team' statement without identifier\n");

     SgBasicBlock * body = new SgBasicBlock(Sg_File_Info::generateDefaultFileInfo());
     ROSE_ASSERT(body != NULL);

  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     body->setCaseInsensitive(true);

     setSourcePosition(body,team_id);

     SgVariableSymbol* teamId = trace_back_through_parent_scopes_lookup_variable_symbol(team_id->text,
                                                                                         astScopeStack.front());
     if (teamId == NULL) {
         printf("ERROR(CAF): The team selector:\"%s\" must be declared with \"Team\" statement.\n",
                                                                                        team_id->text);
         ROSE_ASSERT(false);
      }

     SgVarRefExp* teamIdReference = new SgVarRefExp(teamId);
     ROSE_ASSERT(teamIdReference != NULL);

     setSourcePosition(teamIdReference,team_id);

     SgCAFWithTeamStatement *withTeam = new SgCAFWithTeamStatement(teamIdReference, body);
   
  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     withTeam->setCaseInsensitive(true);

     setSourcePosition(withTeam,team_id);

     SgScopeStatement* currentScope = getTopOfScopeStack();

     withTeam->set_parent(currentScope);

     currentScope->append_statement(withTeam);

     body->set_parent(withTeam);
   
     ROSE_ASSERT(astScopeStack.empty() == false);

     astScopeStack.push_front(withTeam);

     astScopeStack.push_front(body);

}

/**
 * Rice's end with team statement:
 * @param label: label of the statement
 * @param team_id: the team ID (optional)
 */
void c_action_rice_end_with_team_stmt(Token_t *label, Token_t *team_id, Token_t *eos) {

     ROSE_ASSERT(astScopeStack.empty() == false);
    setSourceEndPosition(getTopOfScopeStack(), eos);
     astScopeStack.pop_front();

}


/**
 * Rice's finish statement:
 * @param label Optional statement label.
 * @param idTeam Optional team identifier.
 */

static std::stack<Token *> finish_stack;
static const char * FINISH_SUBR_NAME = "CAF_FINISH";
static const char * ENDFINISH_SUBR_NAME = "CAF_END_FINISH";

void c_action_rice_finish_stmt(Token_t *label, Token_t *teamToken, Token_t *eos)
{
#if 1
        const char * s_label = (label     ? label->text     : "<no label>");
        const char * s_team  = (teamToken ? teamToken->text : "<no team>" );
        const char * s_eos   = eos->text;
        printf("In c_action_rice_finish_stmt(%s, %s, %s)", s_label, s_team, s_eos);
        outputState("At TOP of c_action_rice_finish_stmt()");
#endif

        finish_stack.push(teamToken);

        // add translation to current scope
    Token * caf_finish = create_token(eos->line, eos->col, 0, FINISH_SUBR_NAME);
        c_action_section_subscript_list__begin();                                       // R619
        if( teamToken != NULL)
        {
                c_action_part_ref(teamToken, false, false);                             // R613
                c_action_data_ref(1);                                                                   // R612
                c_action_designator_or_func_ref();                                              // R1217
                c_action_primary();                                                                             // R701
        }
        else
        {
                SgExpression * nullExpr = new SgNullExpression();
                setSourcePosition(nullExpr, eos);
                astExpressionStack.push_front(nullExpr);
        }
        c_action_section_subscript(true, false, false, true);   // R619
        c_action_section_subscript_list(1);                                             // R619
        c_action_part_ref(caf_finish, true, false);                             // R613
        c_action_data_ref(1);                                                                   // R612
        c_action_procedure_designator();                                                // R1219
        c_action_call_stmt(NULL, caf_finish, NULL, false);              // R1218

    // start a new block scope for body of finish construct
    SgScopeStatement * currentScope = getTopOfScopeStack();
    SgBasicBlock * body = new SgBasicBlock(Sg_File_Info::generateDefaultFileInfo());
    setSourcePosition(body, eos);
    body->set_parent(currentScope);
    currentScope->append_statement(body);
    astScopeStack.push_front(body);

#if 1
        outputState("At BOTTOM of c_action_rice_finish_stmt()");
#endif
}


/**
 * Rice's end finish statement:
 * @param label Optional statement label.
 * @param eoms T_EOS token.
 */

static const char * SPAWN_SUBR_NAME = "CAF_SPAWN";

// DQ (12/2/2010): This is not used and so causes a warning.
// static const char * TEAM_DEFAULT_NAME = "TEAM_DEFAULT";

void c_action_rice_end_finish_stmt(Token_t *label, Token_t *eos)
{
#if 1
        const char * s_label    = (label      ? label->text  : "<no label>");
        const char * s_eos      = (eos        ? eos->text    : "no eos"    );
        printf("In c_action_rice_end_finish_stmt(%s, %s)", s_label, s_eos);
        outputState("At TOP of c_action_rice_end_finish_stmt()");
#endif

        // scope for enclosed statements has already been removed by 'c_action_block'

    // pop finish construct off our stack
    if( ! finish_stack.empty() )
        finish_stack.pop();
    else
        {
                printf("ERROR(CAF): 'end finish' without matching 'finish'\n");
                ROSE_ASSERT(false);
        }

        // add translation to current scope
        Token * caf_end_finish = create_token(eos->line, eos->col, 0, ENDFINISH_SUBR_NAME);
        c_action_section_subscript_list__begin();                               // R619
        c_action_section_subscript_list(0);                                             // R619
        c_action_part_ref(caf_end_finish, true, false);                 // R613
        c_action_data_ref(1);                                                                   // R612
        c_action_procedure_designator();                                                // R1219
        c_action_call_stmt(NULL, caf_end_finish, NULL, false);  // R1218

#if 1
        outputState("At BOTTOM of c_action_rice_end_finish_stmt()");
#endif
}

/**
 * Rice's spawn statement:
 * @param label Optional statement label.
 * @param callKeyword T_CALL token.
 * @param processor Optional processor number.
 * @param team Optional team identifier.
 * @param eos T_EOS token.
 * @param hasArgs True if an actual-arg-spec-list is present.
 * @param hasWhere True if a 'processor@team' is present; if so, a processor number is on expr stack.
 */
void c_action_rice_spawn_stmt(Token_t * label, Token_t * spawn, Token_t * eos, ofp_bool hasEvent)
{
#if 1
        outputState("At TOP of c_action_rice_spawn_stmt()");
#endif

        // get the function reference and its arg list
    ROSE_ASSERT(astExpressionStack.empty() == false);
    SgFunctionCallExp * originalCallExpr = isSgFunctionCallExp(astExpressionStack.front());
    ROSE_ASSERT(originalCallExpr != NULL);      // TODO: can we get a SgFunctionRefExp instead, if no actual param list given?
    astExpressionStack.pop_front();

    SgFunctionRefExp * functionRef = isSgFunctionRefExp(originalCallExpr->get_function());
    ROSE_ASSERT(functionRef != NULL);
    SgExprListExp * argumentList   = originalCallExpr->get_args();

        // get the notification event if any
        SgExpression * eventExpr;
        if( hasEvent )
        {
            ROSE_ASSERT(astExpressionStack.empty() == false);
                eventExpr = astExpressionStack.front();
                astExpressionStack.pop_front();
        }
        else
                eventExpr = NULL;

    // get processor # and team id
    ROSE_ASSERT(rice_dataref_coexpr != NULL);
    SgExpression * pnumExpr     = rice_dataref_coexpr->get_teamRank();
    SgVarRefExp * teamExpr = rice_dataref_coexpr->get_teamId();
    delete rice_dataref_coexpr; rice_dataref_coexpr = NULL;

    // argument list
                c_action_section_subscript_list__begin();                                       // R619

                // function to be spawned
                astExpressionStack.push_front(functionRef);
                c_action_section_subscript(true, false, false, true);           // R619

                // original function arguments from source
                SgExpressionPtrList & argPtrs = argumentList->get_expressions();
                int num = argPtrs.size();
                for( int k = 0; k < num; k++)
                {
                        astExpressionStack.push_front(argPtrs[k]);
                        c_action_section_subscript(true, false, false, true);   // R619
                }

                // processor number
                astExpressionStack.push_front(pnumExpr);
                c_action_section_subscript(true, false, false, true);           // R619

                // team id
                if( teamExpr != NULL )                                                                          // R619
                        astExpressionStack.push_front(teamExpr);
                else
                {
                        SgExpression * nullExpr = new SgNullExpression();
                        setSourcePosition(nullExpr, spawn);
                        astExpressionStack.push_front(nullExpr);
                }
                c_action_section_subscript(true, false, false, true);

                // notification event
                if( hasEvent )                                                                                          // R619
                        astExpressionStack.push_front(eventExpr);
                else
                {
                        SgExpression * nullExpr = new SgNullExpression();
                        setSourcePosition(nullExpr, spawn);
                        astExpressionStack.push_front(nullExpr);
                }
                c_action_section_subscript(true, false, false, true);

                // number of args = original number + 4 (f, p, t, e)
                c_action_section_subscript_list(num + 4);                                       // R619

        // function call
        Token * caf_spawn_subr = create_token(spawn->line, spawn->col, 0, SPAWN_SUBR_NAME);
        c_action_part_ref(caf_spawn_subr, true, false);                                 // R613
        c_action_data_ref(1);                                                                                   // R612
        c_action_procedure_designator();                                                                // R1219
        c_action_call_stmt(NULL, caf_spawn_subr, NULL, false);                  // R1218
}


// DQ (10/1/2009): Added these functions because the NMI platforms complain about it being undefined.
// It appears that it is strict.
void c_action_component_initialization(){}
void c_action_allocate_co_shape_spec(ofp_bool carg_0){}
void c_action_bounds_remapping(){}
void c_action_bounds_spec(){}
void c_action_component_data_source(){}
void c_action_component_spec(Token_t *carg_0){}
void c_action_data_pointer_object(){}
void c_action_deferred_co_shape_spec(){}
void c_action_enumerator(Token_t *carg_0, ofp_bool carg_1){}
void c_action_final_binding(Token_t *carg_0){}
void c_action_forall_body_construct(){}
void c_action_forall_construct(){}

#if ROSE_OFP_MINOR_VERSION_NUMBER >= 8 & ROSE_OFP_PATCH_VERSION_NUMBER >= 0
void c_action_image_selector(Token_t *leftBracket,Token_t *rightBracket){}
#else
void c_action_image_selector(int carg_0){}
#endif

void c_action_pointer_object(){}
void c_action_proc_interface(Token_t *carg_0){}
void c_action_proc_pointer_object(){}
void c_action_scalar_constant(){}
void c_action_select_type_construct(){}
void c_action_specific_binding(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, ofp_bool carg_4){}
void c_action_structure_constructor(Token_t *carg_0){}
void c_action_type_param_attr_spec(Token_t *carg_0){}
void c_action_type_spec(){}
void c_action_vector_subscript(){}

// DQ (11/20/2010): Added Token support using newest version of OFP 0.8.2.
void c_action_next_token(Token_t *token)
   {
  // This parser action is used in a separate mode to read the tokens from 
  // the file as part of a separate pass over the AST.

     string text            = token->text;
     string currentFilename = getCurrentFilename();
     int line_number        = token->line;
     int column_number      = token->col;

     Sg_File_Info* starting_fileInfo = new Sg_File_Info(currentFilename,line_number,column_number);

  // Building the token as unclassified tokens (tokens can be classified by language, language construct, etc.)
     SgToken* roseToken = new SgToken(starting_fileInfo,text,0);

  // Currently all SgLocatedNode objects are required to have a valid source code position for the start and end.
  // However this might be a bit redundant for the case of SgToken objects.  So we might want to handle this
  // differently in the future.
     Sg_File_Info* ending_fileInfo   = new Sg_File_Info(currentFilename,line_number,column_number+text.length());
     roseToken->set_endOfConstruct(ending_fileInfo);

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
        {
          printf ("In c_action_next_token = %s file = %s line = %d column = %d \n",SageInterface::get_name(roseToken).c_str(),currentFilename.c_str(),line_number,column_number);
        }

  // Verify that we have a propoer start and end source code position for the token.
     ROSE_ASSERT(roseToken->get_startOfConstruct() != NULL);
     ROSE_ASSERT(roseToken->get_endOfConstruct()   != NULL);

  // Get the current file.
     SgSourceFile* currentFile = OpenFortranParser_globalFilePointer;
     ROSE_ASSERT(currentFile != NULL);

  // Add to the token list kept in the SgSourceFile object.
     currentFile->get_token_list().push_back(roseToken);
   }

// FMZ (5/4/2010)
#if ROSE_OFP_MINOR_VERSION_NUMBER >= 8 & ROSE_OFP_PATCH_VERSION_NUMBER >= 0
void c_action_cosubscript_list__begin() {
     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_cosubscript_list__begin. \n");

}


void c_action_cosubscript_list(int carg_0,Token_t* team_id) {

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("In c_action_cosubscript_list,co_rank = %d \n",carg_0);
      ROSE_ASSERT(carg_0 ==1);
      c_action_rice_image_selector(team_id);
}

#endif

#if 0 //FMZ(5/5/2010) : using the implementation from Rice branch
#if ROSE_OFP_MINOR_VERSION_NUMBER >= 8 & ROSE_OFP_PATCH_VERSION_NUMBER >= 0
// DQ (4/5/2010): Added new functions for OFP 0.8.0
void c_action_rice_image_selector(Token_t *carg_0) {};
void c_action_rice_allocate_coarray_spec(int carg_0, Token_t *carg_1){};
void c_action_rice_co_with_team_stmt(Token_t *carg_0, Token_t *carg_1){};
void c_action_rice_end_with_team_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2){};
#endif
#endif


#ifdef __cplusplus
} /* End extern C. */
#endif
