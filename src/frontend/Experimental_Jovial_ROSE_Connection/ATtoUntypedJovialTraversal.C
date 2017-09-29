#ifdef ROSE
#include "sage3basic.h"
#endif

#include "ATtoUntypedJovialTraversal.h"
#include <iostream>

#define PRINT_ATERM_TRAVERSAL 1
#define PRINT_SOURCE_POSITION 0

using namespace Jovial;

ATtoUntypedJovialTraversal::ATtoUntypedJovialTraversal(SgSourceFile* source)
{
#ifdef ROSE
   SgUntypedDeclarationStatementList* sg_decls = new SgUntypedDeclarationStatementList();
   SgUntypedStatementList*            sg_stmts = new SgUntypedStatementList();
   SgUntypedFunctionDeclarationList*  sg_funcs = new SgUntypedFunctionDeclarationList();
   SgUntypedGlobalScope*          global_scope = new SgUntypedGlobalScope("",SgToken::FORTRAN_UNKNOWN,sg_decls,sg_stmts,sg_funcs);

   pSourceFile = source;
   ROSE_ASSERT(pSourceFile != NULL);

   pUntypedFile = new SgUntypedFile(global_scope);
   ROSE_ASSERT(pUntypedFile != NULL);

// DQ (2/25/2013): Set the default for source position generation to be consistent with other languages (e.g. C/C++).
   SageBuilder::setSourcePositionClassificationMode(SageBuilder::e_sourcePositionFrontendConstruction);
//TODO     SageBuilder::setSourcePositionClassificationMode(SageBuilder::e_sourcePositionCompilerGenerated);
#endif
}

ATtoUntypedJovialTraversal::~ATtoUntypedJovialTraversal()
{
#ifdef ROSE
   delete pUntypedFile;
#endif
}

//========================================================================================
// 1.1 Module
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedJovialTraversal::traverse_Module(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Module: %s\n", ATwriteToString(term));
#endif

   ATerm module;
#ifdef ROSE
   SgUntypedGlobalScope* global_scope = get_scope();
#else
   SgUntypedScope* global_scope = NULL;
#endif

   if (ATmatch(term, "Module(<term>)", &module)) {
      if (traverse_CompoolModule(module, global_scope)) {
         // MATCHED CompoolModule
      }
      else if (traverse_ProcedureModule(module, global_scope)) {
         // MATCHED ProcedureModule
      }
      else if (traverse_MainProgramModule(module, global_scope)) {
         // MATCHED MainProgramModule
      } else return ATfalse;

   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 1.2.1 COMPOOL MODULES
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedJovialTraversal::traverse_CompoolModule(ATerm term, SgUntypedScope* scope)
{ 
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CompoolModule: %s\n", ATwriteToString(term));
#endif

   return ATfalse;
}

//========================================================================================
// 1.2.2 PROCEDURE MODULES
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedJovialTraversal::traverse_ProcedureModule(ATerm term, SgUntypedScope* scope)
{ 
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ProcedureModule: %s\n", ATwriteToString(term));
#endif

   return ATfalse;
}

ATbool ATtoUntypedJovialTraversal::traverse_DeclarationList(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_DeclarationList: %s\n", ATwriteToString(term));
#endif

   ATerm decls;
   if (ATmatch(term, "DeclarationList(<term>)" , &decls)) {
      ATermList tail = (ATermList) ATmake("<term>", decls);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_DataDeclaration(head, decl_list)) {
            // MATCHED DataDeclaration
         } else return ATfalse;
      }
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 1.2.3 MAIN PROGRAM MODULES
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedJovialTraversal::traverse_MainProgramModule(ATerm term, SgUntypedScope* scope)
{ 
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_MainProgramModule: %s\n", ATwriteToString(term));
#endif

   ATerm t_decls, t_name, t_body, t_funcs;
   std::string name;

#ifdef ROSE
   SgUntypedDeclarationStatementList* sg_decls = new SgUntypedDeclarationStatementList();
#else
   SgUntypedDeclarationStatementList* sg_decls = NULL;
#endif

   if (ATmatch(term, "MainProgramModule(<term>,<term>,<term>,<term>)", &t_decls,&t_name,&t_body,&t_funcs)) {
      if (traverse_DeclarationList(t_decls, sg_decls)) {
         // MATCHED DeclarationList
      } else return ATfalse;

      if (traverse_Name(t_name, name)) {
         // MATCHED Name
      } else return ATfalse;

      if (traverse_ProgramBody(t_body, scope)) {
         // MATCHED ProgramBody
      } else return ATfalse;

      if (traverse_NonNestedSubroutineList(t_funcs, scope)) {
         // MATCHED NonNestedSubroutineList
      } else return ATfalse;

   } else return ATfalse;

   return ATtrue;
}

ATbool ATtoUntypedJovialTraversal::traverse_Name(ATerm term, std::string & name)
{ 
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Name: %s\n", ATwriteToString(term));
#endif

   char* str;
   if (ATmatch(term, "Name(<str>)", &str)) {
      name = str;
   } else return ATfalse;

   return ATtrue;
}

ATbool ATtoUntypedJovialTraversal::traverse_ProgramBody(ATerm term, SgUntypedScope* scope)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ProgramBody: %s\n", ATwriteToString(term));
#endif

   ATerm t_stmt;
   ATerm t_decls, t_stmts, t_funcs, t_labels;
   std::vector<std::string> labels;

#ifdef ROSE
   SgUntypedDeclarationStatementList* decl_list = new SgUntypedDeclarationStatementList();
   SgUntypedStatementList*            stmt_list = new SgUntypedStatementList();
   SgUntypedFunctionDeclarationList*  func_list = new SgUntypedFunctionDeclarationList();

#else
   SgUntypedDeclarationStatementList* decl_list = NULL;
   SgUntypedStatementList*            stmt_list = NULL;
   SgUntypedFunctionDeclarationList*  func_list = NULL;
#endif

   if (ATmatch(term, "ProgramSimpleBody(<term>)", &t_stmt)) {
      if (traverse_Statement(t_stmt, stmt_list)) {
         // MATCHED Statement
      } else return ATfalse;
   }

   else if (ATmatch(term, "ProgramBody(<term>,<term>,<term>,<term>)", &t_decls,&t_stmts,&t_funcs,&t_labels)) {
      if (traverse_DeclarationList(t_decls, decl_list)) {
         // MATCHED DeclarationList
      } else return ATfalse;

      if (traverse_StatementList(t_stmts, stmt_list)) {
         // MATCHED StatementList
      } else return ATfalse;

      if (traverse_SubroutineDefinitionList(t_funcs, func_list)) {
         // MATCHED StatementList
      } else return ATfalse;

      if (traverse_LabelList(t_labels, labels)) {
         // MATCHED LabelList
      } else return ATfalse;
   }

   else return ATfalse;

   return ATtrue;
}

ATbool ATtoUntypedJovialTraversal::traverse_NonNestedSubroutineList(ATerm term, SgUntypedScope* scope)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_NonNestedSubroutineList: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "NonNestedSubroutineList([])")) {
   } else return ATfalse;

   return ATtrue;
}

ATbool ATtoUntypedJovialTraversal::traverse_SubroutineDefinitionList(ATerm term, SgUntypedFunctionDeclarationList*)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SubroutineDefinitionList: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "[]")) {
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1 DATA DECLARATIONS
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedJovialTraversal::traverse_DataDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_DataDeclaration: %s\n", ATwriteToString(term));
#endif

   if (traverse_ItemDeclaration(term, decl_list)) {
      // MATCHED ItemDeclaration
   } else return ATfalse;

   //  TableDeclaration            -> DataDeclaration
   //  ConstantDeclaration         -> DataDeclaration
   //  BlockDeclaration            -> DataDeclaration

   return ATtrue;
}

//========================================================================================
// 2.1.1 ITEM DECLARATION
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedJovialTraversal::traverse_ItemDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ItemDeclaration: %s\n", ATwriteToString(term));
#endif

   ATerm t_name, t_alloc, t_type, t_preset;
   char* name;
   bool  has_spec;
   std::string spec_string;

   if (ATmatch(term, "ItemDeclaration(<term>,<term>,<term>,<term>)", &t_name,&t_alloc,&t_type,&t_preset)) {
      if (ATmatch(t_name, "<str>", &name)) {
         // MATCHED ItemName
      } else return ATfalse;

      if (traverse_OptAllocationSpecifier(t_alloc, &has_spec, spec_string)) {
         // MATCHED OptAllocationSpecifier
      } else return ATfalse;

      if (traverse_ItemTypeDescription(t_type)) {
         // MATCHED ItemTypeDescription
      } else return ATfalse;

      if (ATmatch(t_preset, "no-item-preset()")) {
         // MATCHED no-item-preset
      }
      else {
         // TODO - ItemPreset
         return ATfalse;
      }

   } else return ATfalse;

   // TODO - construct ItemDeclration and add to decl_list
   // decl_list->get_decl_list().push_back(item_decl);

   return ATtrue;
}

ATbool ATtoUntypedJovialTraversal::traverse_ItemTypeDescription(ATerm term /* TODO - return type */)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ItemTypeDescription: %s\n", ATwriteToString(term));
#endif

   if (traverse_IntegerItemDescription(term)) {
      // MATCHED IntegerItemDescription
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.1.1 INTEGER TYPE DESCRIPTIONS
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedJovialTraversal::traverse_IntegerItemDescription(ATerm term /*TODO - return type */)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_IntegerItemDescription: %s\n", ATwriteToString(term));
#endif

   ATerm t_round_or_truncate, t_size;
   bool has_round_or_truncate, has_size;

   if      (ATmatch(term, "IntegerItemDescription (<term>,<term>)", &t_round_or_truncate,&t_size)) {
             // MATCHED   signed IntegerItemDescription
   }
   else if (ATmatch(term, "IntegerItemDescriptionU(<term>,<term>)", &t_round_or_truncate,&t_size)) {
             // MATCHED unsigned IntegerItemDescription
   }
   else return ATfalse;

   if (traverse_OptRoundOrTruncate(t_round_or_truncate, &has_round_or_truncate)) {
      // MATCHED OptRoundOrTruncate
   } else return ATfalse;

   if (traverse_OptIntegerSize(t_size, &has_size)) {
      // MATCHED OptIntegerSize
   } else return ATfalse;

   return ATtrue;
}

 ATbool ATtoUntypedJovialTraversal::traverse_OptIntegerSize(ATerm term, bool* has_size /*TODO - return type */)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptIntegerSize: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "no-integer-size()")) {
      *has_size = false;
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.1.2 FLOATING TYPE DESCRIPTIONS
//----------------------------------------------------------------------------------------


ATbool ATtoUntypedJovialTraversal::traverse_OptRoundOrTruncate(ATerm term, bool* has_round_or_truncate /*TODO - return type */)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptRoundOrTruncate: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "no-round-or-truncate()")) {
      *has_round_or_truncate = false;
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.5 ALLOCATION OF DATA OBJECTS
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedJovialTraversal::traverse_OptAllocationSpecifier(ATerm term, bool* has_spec, std::string & spec_string)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptAllocationSpecifier: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "no-allocation-specifier()")) {
      *has_spec = false;
   }
   else if (ATmatch(term, "STATIC()")) {
      *has_spec = false;
      spec_string = "STATIC";
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 4.0 STATEMENTS
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedJovialTraversal::traverse_Statement(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Statement: %s\n", ATwriteToString(term));
#endif

   if (traverse_SimpleStatement(term, stmt_list)) {
      // MATCHED SimpleStatement
   } else return ATfalse;

// TODO CompoundStatement

   return ATtrue;
}

ATbool ATtoUntypedJovialTraversal::traverse_StatementList(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_StatementList: %s\n", ATwriteToString(term));
#endif

   ATermList tail = (ATermList) ATmake("<term>", term);
   while (! ATisEmpty(tail)) {
      ATerm head = ATgetFirst(tail);
      tail = ATgetNext(tail);
      if (traverse_Statement(head, stmt_list)) {
         // MATCHED Statement
      } else return ATfalse;
   }

   return ATtrue;
}

ATbool ATtoUntypedJovialTraversal::traverse_SimpleStatement(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SimpleStatement: %s\n", ATwriteToString(term));
#endif

   ATerm t_labels, t_stmt;
   std::vector<std::string> labels;

   if (ATmatch(term, "SimpleStatement(<term>,<term>)", &t_labels,&t_stmt)) {
      if (traverse_LabelList(t_labels, labels)) {
         // MATCHED LabelList
      } else return ATfalse;

      if (traverse_AssignmentStatement(t_stmt, stmt_list)) {
         // MATCHED AssignmentStatement
      } else return ATfalse;

      //  LoopStatement               -> SimpleStatement
      //  IfStatement                 -> SimpleStatement
      //  CaseStatement               -> SimpleStatement
      //%%ProcedureCallStatement      -> SimpleStatement  %%AMBIGUOUS with AssignmentStatement
      //  ReturnStatement             -> SimpleStatement
      //  GotoStatement               -> SimpleStatement
      //  ExitStatement               -> SimpleStatement
      //  StopStatement               -> SimpleStatement
      //  AbortStatement              -> SimpleStatement
      //  NullStatement               -> SimpleStatement
   } else return ATfalse;

   return ATtrue;
}

ATbool ATtoUntypedJovialTraversal::traverse_LabelList(ATerm term, std::vector<std::string> & labels)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_LabelList: %s\n", ATwriteToString(term));
#endif

   ATerm t_labels;
   if (ATmatch(term, "LabelList(<term>)" , &t_labels)) {
      ATermList tail = (ATermList) ATmake("<term>", t_labels);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         // TODO - match label
         return ATfalse;
      }
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 4.1 ASSIGNMENT STATEMENTS
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedJovialTraversal::traverse_AssignmentStatement(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_AssignmentStatement: %s\n", ATwriteToString(term));
#endif

   ATerm t_vars, t_expr;
   std::vector<SgUntypedExpression*> vars;
   SgUntypedExpression * expr;

   if (ATmatch(term, "AssignmentStatement(<term>,<term>)", &t_vars,&t_expr)) {

      if (traverse_VariableList(t_vars, vars)) {
         // MATCHED VariableList
      } else return ATfalse;

      if (traverse_Formula(t_expr, &expr)) {
         // MATCHED Formula
      } else return ATfalse;

   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 5.0 FORMULAS
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedJovialTraversal::traverse_Formula(ATerm term, SgUntypedExpression** expr)
{
   if (traverse_NumericFormula(term, expr)) {
      // MATCHED NumericFormula
   } else return ATfalse;

   //  BitFormula                  -> Formula
   //  CharacterFormula            -> Formula
   //  StatusFormula               -> Formula
   //  PointerFormula              -> Formula
   //  TableFormula                -> Formula

   return ATtrue;
}

//========================================================================================
// 5.1 NUMERIC FORMULAS
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedJovialTraversal::traverse_NumericFormula(ATerm term, SgUntypedExpression** expr)
{
   if (traverse_IntegerFormula(term, expr)) {
      // MATCHED IntegerFormula
   } else return ATfalse;

   //  FloatingFormula             -> NumericFormula
   //  FixedFormula                -> NumericFormula

   return ATtrue;
}

//========================================================================================
// 5.1.1 INTEGER FORMULAS
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedJovialTraversal::traverse_IntegerFormula(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_IntegerFormula: %s\n", ATwriteToString(term));
#endif

   ATerm t_sign, t_expr, t_lhs, t_op, t_rhs;

   if (ATmatch(term, "IntegerFormula(<term>,<term>)", &t_sign,&t_expr)) {

      // OptSign IntegerTerm -> IntegerFormula

      if (traverse_OptSign(t_sign)) {
         // MATCHED OptSign
      } else return ATfalse;

      if (traverse_IntegerTerm(t_expr, expr)) {
         // MATCHED IntegerTerm
      } else return ATfalse;

      // TODO - create the expression
      // expr = new SgUntypedExpression()

   }

   else if (ATmatch(term, "IntegerFormula(<term>,<term>,<term>)", &t_lhs,&t_op,&t_rhs)) {

      // IntegerFormula PlusOrMinus IntegerTerm -> IntegerFormula

      if (traverse_IntegerFormula(t_lhs, expr)) {
         // MATCHED IntegerFormula
      } else return ATfalse;

      if (ATmatch(t_op, "+")) {
      }
      else if (ATmatch(t_op, "-")) {
      } else return ATfalse;

      // TODO - binary operator PlusOrMinus

      if (traverse_IntegerTerm(t_rhs, expr)) {
         // MATCHED IntegerTerm
      } else return ATfalse;
   }

   else return ATfalse;

   return ATtrue;
}

ATbool ATtoUntypedJovialTraversal::traverse_IntegerPrimary(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_IntegerPrimary: %s\n", ATwriteToString(term));
#endif

   char* literal;

   if (ATmatch(term, "<str>", &literal)) {
      *expr = NULL;  // TODO - create an integer literal expression
   } else return ATfalse;

   //  IntegerMachineParameter     -> IntegerPrimary
   //  IntegerVariable             -> IntegerPrimary
   //  NamedIntegerConstant        -> IntegerPrimary
   //  IntegerFunctionCall         -> IntegerPrimary
   //  '(' IntegerFormula ')'      -> IntegerPrimary         {cons("IntegerPrimary")}
   //  IntegerConversion
   //    '(' Formula ')'           -> IntegerPrimary         {cons("IntegerPrimary")}

   return ATtrue;
}

ATbool ATtoUntypedJovialTraversal::traverse_OptSign(ATerm term /*TODO - enum variable for sign */)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptSign: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "no-sign()")) {
      // sign_enum = NO_SIGN;
   }
   else if (ATmatch(term, "+")) {
      // sign_enum = PLUS;
   }
   else if (ATmatch(term, "+")) {
      // sign_enum = MINUS;
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 6.1 VARIABLE AND BLOCK REFERENCES
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedJovialTraversal::traverse_Variable(ATerm term, SgUntypedExpression** var)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Variable: %s\n", ATwriteToString(term));
#endif

   char* name;

   if (ATmatch(term, "<str>" , &name)) {
      // MATCHED NamedVariable
      *var = NULL;  // TODO - create a variable reference expression
   } else return ATfalse;

   //  BitFunctionVariable         -> Variable           {cons("BitFunctionVariable")}
   //  ByteFunctionVariable        -> Variable           {cons("ByteFunctionVariable")}
   //  RepFunctionVariable         -> Variable           {cons("RepFunctionVariable")}

   return ATtrue;
}

ATbool ATtoUntypedJovialTraversal::traverse_VariableList(ATerm term, std::vector<SgUntypedExpression*> & vars)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_VariableList: %s\n", ATwriteToString(term));
#endif

   ATerm t_labels;
   SgUntypedExpression* var;

   if (ATmatch(term, "VariableList(<term>)" , &t_labels)) {
      ATermList tail = (ATermList) ATmake("<term>", t_labels);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);

         if (traverse_Variable(head, &var)) {
            vars.push_back(var);
         } else return ATfalse;
      }
   } else return ATfalse;

   return ATtrue;
}
