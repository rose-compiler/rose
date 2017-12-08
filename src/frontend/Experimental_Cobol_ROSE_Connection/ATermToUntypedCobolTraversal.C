#include "sage3basic.h"
#include "untypedBuilder.h"

#include "ATermToUntypedCobolTraversal.h"
#include <iostream>

#define PRINT_ATERM_TRAVERSAL 1
#define PRINT_SOURCE_POSITION 1

using namespace ATermSupport;

ATermToUntypedCobolTraversal::ATermToUntypedCobolTraversal(SgSourceFile* source) : ATermToUntypedTraversal(source)
{
   UntypedBuilder::set_language(SgFile::e_Cobol_language);
}

ATermToUntypedCobolTraversal::~ATermToUntypedCobolTraversal()
{
}

//========================================================================================
// COBOL Source Program
//----------------------------------------------------------------------------------------
ATbool
ATermToUntypedCobolTraversal::traverse_CobolSourceProgram (ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CobolSourceProgram: %s\n", ATwriteToString(term));
#endif

   ATerm t_id, t_period, t_name, t_prog_spec, t_end;
   int alt;
   std::string prog_name, end_prog_name;
   Cobol_ROSE_Translation::ProgramSpec prog_spec_enum;

   SgUntypedGlobalScope* global_scope = get_scope();

   SgUntypedFunctionScope* function_scope = NULL;
   SgUntypedProgramHeaderDeclaration* program = NULL;
   SgUntypedDeclarationStatementList* global_decls = global_scope->get_declaration_list();

   if (ATmatch(term, "CobolSourceProgram(<term>,<term>,<term>,<term>,<term>)", &t_id,&t_period,&t_name,&t_prog_spec,&t_end)) {

   // ("IDENTIFICATION"|"ID")
      if (ATmatch(t_id, "alt(<int>,[])", &alt) == ATfalse)  return ATfalse;

   // ("."|"")
      if (ATmatch(t_period, "alt(<int>,[])", &alt) == ATfalse)  return ATfalse;

      if (traverse_Name(t_name, prog_name)) {
         // MATCHED Name
      } else return ATfalse;

      if (traverse_OptProgramSpec(t_prog_spec, prog_spec_enum)) {
         // MATCHED ProgramSpec
      } else return ATfalse;

      if (traverse_OptEndProgram(t_end, end_prog_name)) {
         // MATCHED EndProgram
      } else return ATfalse;

   } else return ATfalse;

   std::cout << "SUCCESSfully traversed CobolSourceProgram\n";

   SgUntypedDeclarationStatementList* decl_list = new SgUntypedDeclarationStatementList();
   SgUntypedStatementList*            stmt_list = new SgUntypedStatementList();
   SgUntypedFunctionDeclarationList*  func_list = new SgUntypedFunctionDeclarationList();

   std::string label = "";

// TODO: add syntax and traversal for data-division and procedure-division (empty for now)
   function_scope = new SgUntypedFunctionScope(label,decl_list,stmt_list,func_list);

   SgUntypedInitializedNameList* param_list  = new SgUntypedInitializedNameList();
   SgUntypedTokenList*           prefix_list = new SgUntypedTokenList();

   SgUntypedType* type = UntypedBuilder::buildType(SgUntypedType::e_void);
   SgUntypedNamedStatement* end_program_stmt = new SgUntypedNamedStatement(label,0,end_prog_name);

   // create the program
   program = new SgUntypedProgramHeaderDeclaration(label, prog_name, param_list, type,
                                                   function_scope, prefix_list, end_program_stmt);

// This could probably be improved to as it includes decls and funcs in global scope
// setSourcePosition(program, term);
// FIXME: No end statement so this will mimic Fortran usage
// setSourcePositionFromEndOnly(end_program_stmt, main_program);

// add program to the global scope
   global_scope->get_function_list()->get_func_list().push_back(program);

// Generate dot file for Untyped nodes.
   generateDOT(global_scope, "cobol_src_prog.dot");

   return ATtrue;
}

#if 0
//========================================================================================
// 1.2.3 MAIN PROGRAM MODULES
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedCobolTraversal::traverse_MainProgramModule(ATerm term, SgUntypedGlobalScope* global_scope)
{ 
#if DUMP_COBPT_STRUCTURE
   rose_convert_cb_program(const struct cb_program* prog, struct cobpt_base_list* local_cache);
#endif

   ATerm t_decls, t_name, t_body, t_funcs;
   std::string name;

   SgUntypedFunctionScope* function_scope = NULL;
   SgUntypedProgramHeaderDeclaration* main_program = NULL;
   SgUntypedDeclarationStatementList* global_decls = global_scope->get_declaration_list();

   if (ATmatch(term, "MainProgramModule(<term>,<term>,<term>,<term>)", &t_decls,&t_name,&t_body,&t_funcs)) {
      if (traverse_DeclarationList(t_decls, global_decls)) {
         // MATCHED DeclarationList
      } else return ATfalse;

      if (traverse_Name(t_name, name)) {
         // MATCHED Name
      } else return ATfalse;

      if (traverse_ProgramBody(t_body, &function_scope)) {
         // MATCHED ProgramBody
         assert(function_scope != NULL);
      } else return ATfalse;

      std::cout << "MAIN_PROGRAM_MODULE:\n";

      std::string label = "";

      SgUntypedInitializedNameList* param_list  = new SgUntypedInitializedNameList();
      SgUntypedTokenList*           prefix_list = new SgUntypedTokenList();

      SgUntypedType* type = UntypedBuilder::buildType(SgUntypedType::e_void);
      SgUntypedNamedStatement* end_program_stmt = new SgUntypedNamedStatement("",0,"");

   // create the program
      main_program   = new SgUntypedProgramHeaderDeclaration(label, name, param_list, type,
                                                             function_scope, prefix_list, end_program_stmt);

   // This could probably be improved to as it includes decls and funcs in global scope
      setSourcePosition(main_program, term);
   // No end statement so this will mimic Fortran usage
      setSourcePositionFromEndOnly(end_program_stmt, main_program);

   // add program to the global scope
      global_scope->get_function_list()->get_func_list().push_back(main_program);

      if (traverse_NonNestedSubroutineList(t_funcs, global_scope)) {
         // MATCHED NonNestedSubroutineList
      } else return ATfalse;

   } else return ATfalse;

   return ATtrue;
}
#endif

ATbool
ATermToUntypedCobolTraversal::traverse_Name (ATerm term, std::string & name)
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

ATbool
ATermToUntypedCobolTraversal::traverse_OptProgramSpec (ATerm term, Cobol_ROSE_Translation::ProgramSpec & prog_spec_enum)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptProgramSpec: %s\n", ATwriteToString(term));
#endif
   int alt;

// ("."|"")
   if (ATmatch(term, "no-program-spec(<int>,[])"), &alt) {
      prog_spec_enum = Cobol_ROSE_Translation::e_unknown;
   } else return ATfalse;

   return ATtrue;
}

ATbool
ATermToUntypedCobolTraversal::traverse_OptEndProgram (ATerm term, std::string & name)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptEndProgram: %s\n", ATwriteToString(term));
#endif
   ATerm t_name;

   if (ATmatch(term, "EndProgram(<term>)", &t_name)) {
      if (traverse_Name(t_name, name)) {
         // MATCHED Name
      } else return ATfalse;
   }
   else if (ATmatch(term, "no-end-program()")) {
      name = "";
   } else return ATfalse;

  return ATtrue;
}


#if 0
ATbool ATermToUntypedCobolTraversal::traverse_DeclarationList(ATerm term, SgUntypedDeclarationStatementList* decl_list)
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

ATbool ATermToUntypedCobolTraversal::traverse_ProgramBody(ATerm term, SgUntypedFunctionScope** function_scope)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ProgramBody: %s\n", ATwriteToString(term));
#endif

   ATerm t_stmt;
   ATerm t_decls, t_stmts, t_funcs, t_labels;
   std::vector<std::string> labels;
   std::string temp_label = "";

   SgUntypedDeclarationStatementList* decl_list = new SgUntypedDeclarationStatementList();
   SgUntypedStatementList*            stmt_list = new SgUntypedStatementList();
   SgUntypedFunctionDeclarationList*  func_list = new SgUntypedFunctionDeclarationList();

   *function_scope = NULL;

   if (ATmatch(term, "ProgramSimpleBody(<term>)", &t_stmt)) {
      if (traverse_Statement(t_stmt, stmt_list)) {
         // MATCHED Statement
      } else return ATfalse;

      std::cout << "PROGRAM SIMPLE BODY\n";
      std::cout << "  # stmts = " << stmt_list->get_stmt_list().size() << "\n";

      *function_scope = new SgUntypedFunctionScope(temp_label,decl_list,stmt_list,func_list);
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

      std::cout << "PROGRAM BODY\n";
      std::cout << "  # decls = " << decl_list->get_decl_list().size() << "\n";
      std::cout << "  # stmts = " << stmt_list->get_stmt_list().size() << "\n";
      std::cout << "  # funcs = " << func_list->get_func_list().size() << "\n";
      std::cout << "  #labels = " <<     labels.size() << "\n\n";

   // TODO - need list for labels in untyped IR
      assert(labels.size() <= 1);
      if (labels.size() == 1) temp_label = labels[0];

      *function_scope = new SgUntypedFunctionScope(temp_label,decl_list,stmt_list,func_list);
   }

   else return ATfalse;

   return ATtrue;
}

ATbool ATermToUntypedCobolTraversal::traverse_NonNestedSubroutineList(ATerm term, SgUntypedScope* scope)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_NonNestedSubroutineList: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "NonNestedSubroutineList([])")) {
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToUntypedCobolTraversal::traverse_SubroutineDefinitionList(ATerm term, SgUntypedFunctionDeclarationList*)
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
ATbool ATermToUntypedCobolTraversal::traverse_DataDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list)
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
ATbool ATermToUntypedCobolTraversal::traverse_ItemDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ItemDeclaration: %s\n", ATwriteToString(term));
#endif

   ATerm t_name, t_alloc, t_type, t_preset;
   char* name;
   bool  has_spec;
   std::string spec_string;

   SgUntypedType* declared_type;

   SgUntypedVariableDeclaration* variable_decl = NULL;
   SgUntypedInitializedNameList* var_name_list = new SgUntypedInitializedNameList();
   SgUntypedTokenList*               attr_list = new SgUntypedTokenList();

   if (ATmatch(term, "ItemDeclaration(<term>,<term>,<term>,<term>)", &t_name,&t_alloc,&t_type,&t_preset)) {
      if (ATmatch(t_name, "<str>", &name)) {
         // MATCHED ItemName
      } else return ATfalse;

      if (traverse_OptAllocationSpecifier(t_alloc, &has_spec, spec_string)) {
         // MATCHED OptAllocationSpecifier
      } else return ATfalse;

      if (traverse_ItemTypeDescription(t_type, &declared_type)) {
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

   std::cout << "ITEM DECLARATION " << name << "\n";

   std::string label = "";

   SgUntypedInitializedName* initialized_name = new SgUntypedInitializedName(declared_type, name);
   setSourcePosition(initialized_name, t_name);

// There will be only one variable declared in Cobol
   var_name_list->get_name_list().push_back(initialized_name);

   variable_decl = new SgUntypedVariableDeclaration(label, declared_type, attr_list, var_name_list);
   setSourcePosition(variable_decl, term);

   decl_list->get_decl_list().push_back(variable_decl);

   return ATtrue;
}

ATbool ATermToUntypedCobolTraversal::traverse_ItemTypeDescription(ATerm term, SgUntypedType** type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ItemTypeDescription: %s\n", ATwriteToString(term));
#endif

   if (traverse_IntegerItemDescription(term, type)) {
      // MATCHED IntegerItemDescription
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.1.1 INTEGER TYPE DESCRIPTIONS
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedCobolTraversal::traverse_IntegerItemDescription(ATerm term, SgUntypedType** type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_IntegerItemDescription: %s\n", ATwriteToString(term));
#endif

   ATerm t_round_or_truncate, t_size;
   bool has_round_or_truncate, has_size;

   if (ATmatch(term, "IntegerItemDescription (<term>,<term>)", &t_round_or_truncate,&t_size)) {
      *type = UntypedBuilder::buildType(SgUntypedType::e_int);
   }
   else if (ATmatch(term, "IntegerItemDescriptionU(<term>,<term>)", &t_round_or_truncate,&t_size)) {
      *type = UntypedBuilder::buildType(SgUntypedType::e_uint);
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

 ATbool ATermToUntypedCobolTraversal::traverse_OptIntegerSize(ATerm term, bool* has_size /*TODO - return type */)
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


ATbool ATermToUntypedCobolTraversal::traverse_OptRoundOrTruncate(ATerm term, bool* has_round_or_truncate /*TODO - return type */)
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
ATbool ATermToUntypedCobolTraversal::traverse_OptAllocationSpecifier(ATerm term, bool* has_spec, std::string & spec_string)
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
ATbool ATermToUntypedCobolTraversal::traverse_Statement(ATerm term, SgUntypedStatementList* stmt_list)
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

ATbool ATermToUntypedCobolTraversal::traverse_StatementList(ATerm term, SgUntypedStatementList* stmt_list)
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

ATbool ATermToUntypedCobolTraversal::traverse_SimpleStatement(ATerm term, SgUntypedStatementList* stmt_list)
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

      if (traverse_AssignmentStatement(t_stmt, labels, stmt_list)) {
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

ATbool ATermToUntypedCobolTraversal::traverse_LabelList(ATerm term, std::vector<std::string> & labels)
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
ATbool ATermToUntypedCobolTraversal::traverse_AssignmentStatement(ATerm term, std::vector<std::string> & labels, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_AssignmentStatement: %s\n", ATwriteToString(term));
#endif

   ATerm t_vars, t_expr;
   std::string temp_label = "";
   std::vector<SgUntypedExpression*> vars;
   SgUntypedExpression * expr = NULL;

   if (ATmatch(term, "AssignmentStatement(<term>,<term>)", &t_vars,&t_expr)) {

      if (traverse_VariableList(t_vars, vars)) {
         // MATCHED VariableList
      } else return ATfalse;

      if (traverse_Formula(t_expr, &expr)) {
         // MATCHED Formula
      } else return ATfalse;

      std::cout << "ASSIGNMENT STMT\n";

      assert(labels.size() <= 1);
      assert(  vars.size() == 1);
      assert(expr);

   // TODO - need list for labels in untyped IR
      if (labels.size() == 1) temp_label = labels[0];

      SgUntypedAssignmentStatement* assign_stmt = new SgUntypedAssignmentStatement(temp_label,vars[0],expr);
      setSourcePosition(assign_stmt, term);

      stmt_list->get_stmt_list().push_back(assign_stmt);


   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 5.0 FORMULAS
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedCobolTraversal::traverse_Formula(ATerm term, SgUntypedExpression** expr)
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
ATbool ATermToUntypedCobolTraversal::traverse_NumericFormula(ATerm term, SgUntypedExpression** expr)
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
ATbool ATermToUntypedCobolTraversal::traverse_IntegerFormula(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_IntegerFormula: %s\n", ATwriteToString(term));
#endif

   ATerm t_sign, t_expr, t_lhs, t_op, t_rhs;

   if (ATmatch(term, "IntegerFormula(<term>,<term>)", &t_sign,&t_expr)) {
      Cobol_ROSE_Translation::ExpressionKind op_enum;

      // OptSign IntegerTerm -> IntegerFormula

      if (traverse_OptSign(t_sign, op_enum)) {
         // MATCHED OptSign
      } else return ATfalse;

      if (traverse_IntegerTerm(t_expr, expr)) {
         // MATCHED IntegerTerm
      } else return ATfalse;

      std::cout << "INTEGER FORMULA: TODO: add optional unary operator for sign\n";
      // TODO - check op_enum to see if we need to add unary operator to the expression
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

ATbool ATermToUntypedCobolTraversal::traverse_IntegerPrimary(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_IntegerPrimary: %s\n", ATwriteToString(term));
#endif

   char* literal;
   SgUntypedType* type;
   SgToken::ROSE_Fortran_Keywords keyword = SgToken::FORTRAN_TYPE;

   if (ATmatch(term, "<str>", &literal)) {
      type = UntypedBuilder::buildType(SgUntypedType::e_int);
      *expr = new SgUntypedValueExpression(keyword,literal,type);
      std::cout << "INTEGER LITERAL is " << literal << "\n";
      setSourcePosition(*expr, term);
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

ATbool ATermToUntypedCobolTraversal::traverse_OptSign(ATerm term, Cobol_ROSE_Translation::ExpressionKind & op_enum)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptSign: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "no-sign()")) {
      op_enum = Cobol_ROSE_Translation::e_notAnOperator;
   }
   else if (ATmatch(term, "+")) {
      op_enum = Cobol_ROSE_Translation::e_unaryPlusOperator;
   }
   else if (ATmatch(term, "-")) {
      op_enum = Cobol_ROSE_Translation::e_unaryMinusOperator;
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToUntypedCobolTraversal::traverse_IntegerTerm(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_IntegerTerm: %s\n", ATwriteToString(term));
#endif

   ATerm t_lhs, t_op, t_rhs;
   std::string op_name;
   Cobol_ROSE_Translation::ExpressionKind op_enum;
   SgUntypedExpression * lhs, * rhs;

   if (ATmatch(term, "IntegerTerm(<term>,<term>,<term>)", &t_lhs,&t_op,&t_rhs)) {
      if (traverse_IntegerTerm(t_lhs, &lhs)) {
         // MATCHED IntegerTerm
      } else return ATfalse;

      if (traverse_MultiplyDivideOrMod(t_op, op_enum, op_name)) {
         // MATCHED MultiplyDivideOrMod
      } else return ATfalse;

      if (traverse_IntegerFactor(t_rhs, &rhs)) {
         // MATCHED IntegerTerm
      } else return ATfalse;

      *expr = new SgUntypedBinaryOperator(op_enum,op_name,lhs,rhs);
      std::cout << "BINARY OPERATOR " << op_name << "\n";
      setSourcePosition(*expr, term);
   }
   else if (traverse_IntegerFactor(term, expr)) {
         // MATCHED IntegerFactor
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToUntypedCobolTraversal::traverse_IntegerFactor(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_IntegerFactor: %s\n", ATwriteToString(term));
#endif

   ATerm t_lhs, t_rhs;
   SgUntypedExpression * lhs, * rhs;

   if (ATmatch(term, "IntegerFactor(<term>,<term>)", &t_lhs,&t_rhs)) {
      if (traverse_IntegerFactor(t_lhs, &lhs)) {
         // MATCHED IntegerFactor
      } else return ATfalse;

      if (traverse_IntegerPrimary(t_rhs, &rhs)) {
         // MATCHED IntegerPrimary
      } else return ATfalse;

      // TODO - create the expression
      // expr = new SgUntypedExpression()
   }

   else if (traverse_IntegerPrimary(term, expr)) {
      // MATCHED IntegerPrimary
   }

   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 6.1 VARIABLE AND BLOCK REFERENCES
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedCobolTraversal::traverse_Variable(ATerm term, SgUntypedExpression** var)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Variable: %s\n", ATwriteToString(term));
#endif

   char* name;
   Cobol_ROSE_Translation::ExpressionKind expr_enum = Cobol_ROSE_Translation::e_referenceExpression;

   if (ATmatch(term, "<str>" , &name)) {
      // MATCHED NamedVariable
      std::cout << "VARIABLE " << name << "\n";
      *var = new SgUntypedReferenceExpression(expr_enum, name);
      setSourcePosition(*var, term);

   } else return ATfalse;

   //  BitFunctionVariable         -> Variable           {cons("BitFunctionVariable")}
   //  ByteFunctionVariable        -> Variable           {cons("ByteFunctionVariable")}
   //  RepFunctionVariable         -> Variable           {cons("RepFunctionVariable")}

   return ATtrue;
}

ATbool ATermToUntypedCobolTraversal::traverse_VariableList(ATerm term, std::vector<SgUntypedExpression*> & vars)
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

//========================================================================================
// 8.2.3 OPERATORS
//----------------------------------------------------------------------------------------

ATbool
ATermToUntypedCobolTraversal::traverse_MultiplyDivideOrMod(ATerm term, Cobol_ROSE_Translation::ExpressionKind & op_enum, std::string & op_name)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_MultiplyDivideOrMod: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "TIMES()")) {
      op_enum = Cobol_ROSE_Translation::e_multiplyOperator;
      op_name = "*";
   }
   else if (ATmatch(term, "DIV()")) {
      op_enum = Cobol_ROSE_Translation::e_divideOperator;
      op_name = "/";
   }
   else if (ATmatch(term, "MOD()")) {
      op_enum = Cobol_ROSE_Translation::e_modOperator;
      op_name = "MOD";
   }
   else {
      op_enum = Cobol_ROSE_Translation::e_unknown;
      op_name = "Cobol_operator_unknown";
      return ATfalse;
   }

   return ATtrue;
}
#endif
