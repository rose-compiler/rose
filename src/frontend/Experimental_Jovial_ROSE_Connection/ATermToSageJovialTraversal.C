#include "sage3basic.h"
#include "untypedBuilder.h"

#include "ATermToSageJovialTraversal.h"
#include "Jovial_to_ROSE_translation.h"
#include <iostream>

#define PRINT_ATERM_TRAVERSAL 0
#define PRINT_SOURCE_POSITION 0

using namespace ATermSupport;
using std::cout;
using std::cerr;
using std::endl;

ATermToSageJovialTraversal::ATermToSageJovialTraversal(SgSourceFile* source) : ATermToUntypedTraversal(source)
{
   UntypedBuilder::set_language(SgFile::e_Jovial_language);
}

ATermToSageJovialTraversal::~ATermToSageJovialTraversal()
{
}

//========================================================================================
// 1.1 Module
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_Module(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Module: %s\n", ATwriteToString(term));
#endif

   ATerm module;
   SgUntypedGlobalScope* global_scope = get_scope();

   if (ATmatch(term, "Module(<term>)", &module)) {

      SgScopeStatement* sage_tree_scope{nullptr};
      sage_tree_builder.Enter(sage_tree_scope);

      if (traverse_CompoolModule(module, global_scope)) {
         // MATCHED CompoolModule
      }
      else if (traverse_ProcedureModule(module, global_scope)) {
         // MATCHED ProcedureModule
      }
      else if (traverse_MainProgramModule(module, global_scope)) {
         // MATCHED MainProgramModule
      } else return ATfalse;

      sage_tree_builder.Leave(sage_tree_scope);

   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 1.2.1 COMPOOL MODULES
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_CompoolModule(ATerm term, SgUntypedScope* scope)
{ 
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CompoolModule: %s\n", ATwriteToString(term));
#endif

   ATerm t_dirs, t_name, t_decls;
   std::string name;

   SgUntypedNameListDeclaration* compool_decl;

// Compool declarations go in global scope
   SgUntypedDeclarationStatementList* decls = scope->get_declaration_list();

   if (ATmatch(term, "CompoolModule(<term>,<term>,<term>)", &t_dirs, &t_name, &t_decls)) {

      if (traverse_DirectiveList(t_dirs, decls)) {
         // MATCHED DirectiveList
      } else return ATfalse;

      if (traverse_Name(t_name, name)) {
         // MATCHED Name
      } else return ATfalse;

      SgUntypedName* ut_name = new SgUntypedName(name);
      ROSE_ASSERT(ut_name);
      setSourcePosition(ut_name, t_name);

      SgUntypedNameList* name_list = new SgUntypedNameList();
      ROSE_ASSERT(name_list);
      setSourcePosition(name_list, t_name);

      name_list->get_name_list().push_back(ut_name);

      int stmt_enum = General_Language_Translation::e_jovial_compool_stmt;
      compool_decl = new SgUntypedNameListDeclaration("", stmt_enum, name_list);
      setSourcePosition(compool_decl, term);

   // Add the compool module before the compool declarations
      decls->get_decl_list().push_back(compool_decl);

   // Begin SageTreeBuilder
      SgJovialCompoolStatement* compool_stmt = NULL;
      Rose::builder::SourcePositionPair sources;
      sage_tree_builder.Enter(compool_stmt, name, sources);

      sage_tree_builder.Leave(compool_stmt);

      if (traverse_DeclarationList(t_decls, decls)) {
         // MATCHED DeclarationList and CompoolDeclarationList
      } else return ATfalse;

   } else return ATfalse;

   return ATtrue;

}

//========================================================================================
// 1.2.2 PROCEDURE MODULES
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_ProcedureModule(ATerm term, SgUntypedScope* scope)
{ 
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ProcedureModule: %s\n", ATwriteToString(term));
#endif

   ATerm t_dirs, t_decls, t_funcs;

   if (ATmatch(term, "ProcedureModule(<term>,<term>,<term>)", &t_dirs, &t_decls, &t_funcs)) {

      SgUntypedDeclarationStatementList* decls = scope->get_declaration_list();

      if (traverse_DirectiveList(t_dirs, decls)) {
         // MATCHED DirectiveList
      } else return ATfalse;

      if (traverse_DeclarationList(t_decls, decls)) {
         // MATCHED DeclarationList
      } else return ATfalse;

      if (traverse_NonNestedSubroutineList(t_funcs, scope)) {
         // MATCHED NonNestedSubroutineList
      } else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_DeclarationList(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_DeclarationList: %s\n", ATwriteToString(term));
#endif

   ATerm t_decls;

   if (ATmatch(term, "DeclarationList(<term>)", &t_decls)) {
      ATermList tail = (ATermList) ATmake("<term>", t_decls);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_Declaration(head, decl_list)) {
            // MATCHED Declaration & CompoolDeclaration
         } else return ATfalse;
      }
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_NullDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list, int def_or_ref)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_NullDeclaration: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "NullDeclaration()")) {
      SgUntypedNullDeclaration* null_decl = new SgUntypedNullDeclaration("");
      setSourcePosition(null_decl, term);
      decl_list->get_decl_list().push_back(null_decl);
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 1.2.3 MAIN PROGRAM MODULES
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_MainProgramModule(ATerm term, SgUntypedGlobalScope* global_scope)
{ 
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_MainProgramModule: %s\n", ATwriteToString(term));
#endif

   using namespace General_Language_Translation;

   ATerm t_dirs, t_decls, t_name, t_body, t_funcs;
   std::string name;

   SgUntypedFunctionScope* function_scope = NULL;
   SgUntypedProgramHeaderDeclaration* main_program = NULL;
   SgUntypedDeclarationStatementList* global_decls = global_scope->get_declaration_list();

   if (ATmatch(term, "MainProgramModule(<term>,<term>,<term>,<term>,<term>)", &t_dirs, &t_decls,&t_name,&t_body,&t_funcs)) {

      std::string label = "";

      if (traverse_DirectiveList(t_dirs, global_decls)) {
         // MATCHED DirectiveList
      } else return ATfalse;

      if (traverse_DeclarationList(t_decls, global_decls)) {
         // MATCHED DeclarationList
      } else return ATfalse;

      if (traverse_Name(t_name, name)) {
         // MATCHED Name
      } else return ATfalse;

      // Need
      // 1. program_decl pointer
      // 2. progran_name (optional)
      // 3. labels list
      // 4. source positions

   // Begin SageTreeBuilder
      SgProgramHeaderStatement* program_decl;
      boost::optional<std::string> program_name = name;
      if (name.length() == 0) program_name = boost::none;
      std::cout << "--> program_name is " << program_name.value() << "\n";

      std::list<std::string> labels;
      if (label.length() > 0) labels.push_back(label);
      if (labels.size() > 0) std::cout << "--> label is " << label << "\n";

      Rose::builder::SourcePosition prog_start, prog_end;  // start and end of program
      Rose::builder::SourcePosition dirs_start, dirs_end;  // start and end of directives
      setSourcePositions(term,   prog_start, prog_end);
      setSourcePositions(t_dirs, dirs_start, dirs_end);

      Rose::builder::SourcePositions sources(prog_start, dirs_start, prog_end);
      sage_tree_builder.Enter(program_decl, program_name, labels, sources);

      if (traverse_ProgramBody(t_body, function_scope)) {
         // MATCHED ProgramBody
         assert(function_scope != NULL);
      } else return ATfalse;

      SgUntypedInitializedNameList* param_list = new SgUntypedInitializedNameList();
      ROSE_ASSERT(param_list != NULL);
      SageInterface::setSourcePosition(param_list);

      SgUntypedExprListExpression* prefix_list = new SgUntypedExprListExpression(e_function_modifier_list);
      ROSE_ASSERT(prefix_list != NULL);
      SageInterface::setSourcePosition(prefix_list);

      SgUntypedType* type = UntypedBuilder::buildType(SgUntypedType::e_void);
      ROSE_ASSERT(type != NULL);

      SgUntypedNamedStatement* end_program_stmt = new SgUntypedNamedStatement("",0,"");
      ROSE_ASSERT(end_program_stmt != NULL);
      SageInterface::setSourcePosition(end_program_stmt);

   // create the program
      main_program = new SgUntypedProgramHeaderDeclaration(label, name, param_list, type,
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

      sage_tree_builder.Leave(program_decl);

   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_Name(ATerm term, std::string & name)
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

ATbool ATermToSageJovialTraversal::traverse_ProgramBody(ATerm term, SgUntypedFunctionScope* & function_scope)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ProgramBody: %s\n", ATwriteToString(term));
#endif

   ATerm t_stmt;
   ATerm t_decls, t_stmts, t_funcs, t_labels;
   std::vector<std::string> labels;
   std::vector<PosInfo> locations;
   std::string temp_label = "";

   SgUntypedDeclarationStatementList* decl_list = NULL;
   SgUntypedStatementList*            stmt_list = NULL;
   SgUntypedFunctionDeclarationList*  func_list = NULL;

   function_scope = NULL;

   if (ATmatch(term, "ProgramSimpleBody(<term>)", &t_stmt)) {

      function_scope = UntypedBuilder::buildScope<SgUntypedFunctionScope>(temp_label);
      ROSE_ASSERT(function_scope);
      setSourcePosition(function_scope, term);

      stmt_list = function_scope->get_statement_list();
      ROSE_ASSERT(stmt_list);

      if (traverse_Statement(t_stmt, stmt_list)) {
         // MATCHED Statement
      } else return ATfalse;
   }

   else if (ATmatch(term, "ProgramBody(<term>,<term>,<term>,<term>)", &t_decls,&t_stmts,&t_funcs,&t_labels)) {

      function_scope = UntypedBuilder::buildScope<SgUntypedFunctionScope>(temp_label);
      ROSE_ASSERT(function_scope);
      setSourcePosition(function_scope, term);

      decl_list = function_scope->get_declaration_list();
      ROSE_ASSERT(decl_list);

      if (traverse_DeclarationList(t_decls, decl_list)) {
         // MATCHED DeclarationList
      } else return ATfalse;

      stmt_list = function_scope->get_statement_list();
      ROSE_ASSERT(stmt_list);

      if (traverse_StatementList(t_stmts, stmt_list)) {
         // MATCHED StatementList
      } else return ATfalse;

      func_list = function_scope->get_function_list();
      ROSE_ASSERT(func_list);

      if (traverse_SubroutineDefinitionList(t_funcs, func_list)) {
         // MATCHED SubroutineDefinitionList
      } else return ATfalse;

      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;

#if 0
      std::cout << "PROGRAM BODY\n";
      std::cout << "  # decls = " << decl_list->get_decl_list().size() << "\n";
      std::cout << "  # stmts = " << stmt_list->get_stmt_list().size() << "\n";
      std::cout << "  # funcs = " << func_list->get_func_list().size() << "\n";
      std::cout << "  #labels = " << labels.size() << "\n\n";
#endif

   // TODO - need list for labels in untyped IR
   //        can labels be on program definitions?

      if (labels.size() > 1) {
         cerr << "WARNING UNIMPLEMENTED: ProgramBody - labels.size > 1\n";
         return ATtrue;
      }

      assert(labels.size() <= 1);
      if (labels.size() == 1) temp_label = labels[0];

   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_NonNestedSubroutineList(ATerm term, SgUntypedScope* scope)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_NonNestedSubroutineList: %s\n", ATwriteToString(term));
#endif

   ATerm t_procs, t_proc;

   SgUntypedFunctionDeclarationList* func_list = scope->get_function_list();

   if (ATmatch(term, "NonNestedSubroutineList(<term>)", &t_procs)) {
      ATermList tail = (ATermList) ATmake("<term>", t_procs);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
      // TODO - Probably will need a NonNestedSubroutineDef term
         if (ATmatch(head, "NonNestedSubroutine(<term>)", &t_proc)) {
            if (traverse_ProcedureDefinition(t_proc, func_list)) {
               // MATCHED ProcedureDefinition
            }
            else if (traverse_FunctionDefinition(t_proc, func_list)) {
               // MATCHED FunctionDefinition
            } else return ATfalse;
         } else return ATfalse;
      }
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_SubroutineDefinitionList(ATerm term, SgUntypedFunctionDeclarationList* func_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SubroutineDefinitionList: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "[]")) {
   // Matched an empty list
      return ATtrue;
   }

// At this point there must be a non-empty list to succeed
//
   ATermList tail = (ATermList) ATmake("<term>", term);
   if (! ATisEmpty(tail)) {
      // found a non-empty list
   } else return ATfalse;

   while (! ATisEmpty(tail)) {
      ATerm head = ATgetFirst(tail);
      tail = ATgetNext(tail);
      if (traverse_ProcedureDefinition(head, func_list)) {
         // MATCHED ProcedureDefinition
      } else if (traverse_FunctionDefinition(head, func_list)) {
         // MATCHED FunctionDefinition
      } else return ATfalse;
   }

   return ATtrue;
}

//========================================================================================
// 1.4 IMPLEMENTATION PARAMETERS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_IntegerMachineParameter(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_IntegerMachineParameter: %s\n", ATwriteToString(term));
#endif

   ATerm t_precision, t_scale_spec, t_frac_spec, t_formula;
   SgUntypedExpression *precision, *scale_spec, *frac_spec, *formula;

   if (ATmatch(term, "BITSINBYTE")) {
      cerr << "WARNING UNIMPLEMENTED: IntegerMachineParameter - BITSINBYTE\n";
      // MATCHED BITSINBYTE
   }
   else if (ATmatch(term, "BITSINWORD")) {
      cerr << "WARNING UNIMPLEMENTED: IntegerMachineParameter - BITSINWORD\n";
      // MATCHED BITSINWORD
   }
   else if (ATmatch(term, "LOCSINWORD")) {
      cerr << "WARNING UNIMPLEMENTED: IntegerMachineParameter - LOCSINWORD\n";
      // MATCHED LOCSINWORD
   }
   else if (ATmatch(term, "BYTEPOS(<term>)", &t_formula)) {
      cerr << "WARNING UNIMPLEMENTED: IntegerMachineParameter - BYTEPOS\n";
      // MATCHED BYTEPOS
      if (traverse_NumericFormula(t_formula, sg_expr, formula)) {
         // MATCHED CompileTimeNumericFormula
      } else return ATfalse;
   }

   //TODO: 'BYTESINWORD'              -> IntegerMachineParameter {cons("BYTESINWORD")}
   //      'BITSINPOINTER'            -> IntegerMachineParameter {cons("BITSINPOINTER")}
   //      'INTPRECISION'             -> IntegerMachineParameter {cons("INTPRECISION")}
   //      'FLOATPRECISION'           -> IntegerMachineParameter {cons("FLOATPRECISION")}
   //      'FIXEDPRECISION'           -> IntegerMachineParameter {cons("FIXEDPRECISION")}
   //      'FLOATRADIX'               -> IntegerMachineParameter {cons("FLOATRADIX")}


   else if (ATmatch(term, "IMPLFLOATPRECISION(<term>)", &t_precision)) {
      cerr << "WARNING UNIMPLEMENTED: IntegerMachineParameter - IMPLFLOATPRECISION\n";
      // MATCHED IMPLFLOATPRECISION
      if (traverse_NumericFormula(t_precision, sg_expr, precision)) {
         // MATCHED Precision
      } else return ATfalse;
   }
   else if (ATmatch(term, "IMPLFIXEDPRECISION(<term>,<term>)", &t_scale_spec, &t_frac_spec)) {
      cerr << "WARNING UNIMPLEMENTED: IntegerMachineParameter - IMPLFIXEDPRECISION\n";
      // MATCHED IMPLFIXEDPRECISION
      if (traverse_NumericFormula(t_scale_spec, sg_expr, scale_spec)) {
         // MATCHED ScaleSpecifier
      } else return ATfalse;
      if (traverse_NumericFormula(t_frac_spec, sg_expr, frac_spec)) {
         // MATCHED FractionSpecifier
      } else return ATfalse;
   }

   //TODO:
   //  'IMPLINTSIZE' '(' IntegerSize ')'   -> IntegerMachineParameter {cons("IMPLINTSIZE")}
   //  'MAXFLOATPRECISION'                 -> IntegerMachineParameter {cons("MAXFLOATPRECISION")}
   //  'MAXFIXEDPRECISION'                 -> IntegerMachineParameter {cons("MAXFIXEDPRECSION")}
   //  'MAXINTSIZE'                        -> IntegerMachineParameter {cons("MAXINTSIZE")}
   //  'MAXBYTES'                          -> IntegerMachineParameter {cons("MAXBYTES")}
   //  'MAXBITS'                           -> IntegerMachineParameter {cons("MAXBITS")}
   //  'MAXINT'      '(' IntegerSize ')'   -> IntegerMachineParameter {cons("MAXINT")}
   //  'MININT'      '(' IntegerSize ')'   -> IntegerMachineParameter {cons("MININT")}
   //  'MAXTABLESIZE'                      -> IntegerMachineParameter {cons("MAXTABLESIZE")}
   //  'MAXSTOP'                           -> IntegerMachineParameter {cons("MAXSTOP")}
   //  'MINSTOP'                           -> IntegerMachineParameter {cons("MINSTOP")}
   //  'MAXSIGNDIGITS'                     -> IntegerMachineParameter {cons("MAXSIGNDIGITS")}
   //  'MINSIZE'
   //    '(' CompileTimeIntegerFormula ')' -> IntegerMachineParameter {cons("MINSIZE")}
   //  'MINFRACTION'
   //    '(' CompileTimeIntegerFormula ')' -> IntegerMachineParameter {cons("MINFRACTION")}
   //  'MINSCALE'
   //    '(' CompileTimeIntegerFormula ')' -> IntegerMachineParameter {cons("MINSCALE")}
   //  'MINRELPRECISION'
   //    '(' CompileTimeIntegerFormula ')' -> IntegerMachineParameter {cons("MINRELPRECISION")}

   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_FloatingMachineParameter(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* & expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FloatingMachineParameter: %s\n", ATwriteToString(term));
#endif

   ATerm t_precision;
   SgUntypedExpression* precision;

   if (ATmatch(term, "MAXFLOAT(<term>)", &t_precision)) {
      cerr << "WARNING UNIMPLEMENTED: FloatingMachineParameter - MAXFLOAT\n";
      // MATCHED MAXFLOAT
      if (traverse_NumericFormula(t_precision, sg_expr, precision)) {
         // MATCHED Precision
      } else return ATfalse;
   }
   else if (ATmatch(term, "MINFLOAT(<term>)", &t_precision)) {
      cerr << "WARNING UNIMPLEMENTED: FloatingMachineParameter - MINFLOAT\n";
      // MATCHED MINFLOAT
      if (traverse_NumericFormula(t_precision, sg_expr, precision)) {
         // MATCHED Precision
      } else return ATfalse;
   }
   else if (ATmatch(term, "FLOATRELPRECISION(<term>)", &t_precision)) {
      cerr << "WARNING UNIMPLEMENTED: FloatingMachineParameter - FLOATRELPRECISION\n";
      // MATCHED FLOATRELPRECISION
      if (traverse_NumericFormula(t_precision, sg_expr, precision)) {
         // MATCHED Precision
      } else return ATfalse;
   }
   else if (ATmatch(term, "FLOATUNDERFLOW(<term>)", &t_precision)) {
      cerr << "WARNING UNIMPLEMENTED: FloatingMachineParameter - FLOATUNDERFLOW\n";
      // MATCHED FLOATUNDERFLOW
      if (traverse_NumericFormula(t_precision, sg_expr, precision)) {
         // MATCHED Precision
      } else return ATfalse;
   }

   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_FixedMachineParameter(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* & expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FixedMachineParameter: %s\n", ATwriteToString(term));
#endif

   ATerm t_spec1, t_spec2, t_scale_spec, t_frac_spec;
   SgUntypedExpression *scale_spec, *frac_spec;

   if (ATmatch(term, "MAXFIXED(<term>,<term>)", &t_spec1, &t_spec2)) {
      cerr << "WARNING UNIMPLEMENTED: FixedMachineParameter - MAXFIXED\n";
      // MATCHED MAXFIXED
      if (ATmatch(t_spec1, "ScaleSpecifier(<term>)", &t_scale_spec)) {
         if (traverse_NumericFormula(t_scale_spec, sg_expr, scale_spec)) {
            // MATCHED ScaleSpecifier
         } else return ATfalse;
      } else return ATfalse;

      if (ATmatch(t_spec2, "FractionSpecifier(<term>)", &t_frac_spec)) {
         if (traverse_NumericFormula(t_frac_spec, sg_expr, frac_spec)) {
            // MATCHED FractionSpecifier
         } else return ATfalse;
      } else return ATfalse;
   }
   else if (ATmatch(term, "MINFIXED(<term>,<term>)", &t_spec1, &t_spec2)) {
      cerr << "WARNING UNIMPLEMENTED: FixedMachineParameter - MINFIXED\n";
      // MATCHED MINFIXED
      if (ATmatch(t_spec1, "ScaleSpecifier(<term>)", &t_scale_spec)) {
         if (traverse_NumericFormula(t_scale_spec, sg_expr, scale_spec)) {
            // MATCHED ScaleSpecifier
         } else return ATfalse;
      } else return ATfalse;

      if (ATmatch(t_spec2, "FractionSpecifier(<term>)", &t_frac_spec)) {
         if (traverse_NumericFormula(t_frac_spec, sg_expr, frac_spec)) {
            // MATCHED FractionSpecifier
         } else return ATfalse;
      } else return ATfalse;
   }

   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.0 DECLARATIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_Declaration(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Declaration: %s\n", ATwriteToString(term));
#endif

   ATerm decls;

   if (ATmatch(term, "Declaration(<term>)" , &decls)) {
      ATermList tail = (ATermList) ATmake("<term>", decls);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_Declaration(head, decl_list)) {
            // MATCHED Declaration & CompoolDeclaration
         } else return ATfalse;
      }
   }
   else if (traverse_ExternalDeclaration(term, decl_list)) {
      // MATCHED ExternalDeclaration
   }
   else if (traverse_DataDeclaration(term, decl_list)) {
      // MATCHED DataDeclaration
   }
   else if (traverse_ConstantDeclaration(term, decl_list)) {
      // MATCHED ConstantDeclaration
   }
   else if (traverse_TypeDeclaration(term, decl_list)) {
      // MATCHED TypeDeclaration
   }
   else if (traverse_DefineDeclaration(term, decl_list)) {
      // MATCHED DefineDeclaration
   }
   else if (traverse_OverlayDeclaration(term, decl_list)) {
      // MATCHED OverlayDeclaration
   }
   else if (traverse_StatementNameDeclaration(term, decl_list)) {
      // MATCHED StatementNameDeclaration
   }
   else if (traverse_InlineDeclaration(term, decl_list)) {
      // MATCHED InlineDeclaration
   }
   else if (traverse_NullDeclaration(term, decl_list)) {
      // MATCHED NullDeclaration
   }
   else return ATfalse;

   return ATtrue;

}

//========================================================================================
// 2.1 DATA DECLARATIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_DataDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list, int def_or_ref)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_DataDeclaration: %s\n", ATwriteToString(term));
#endif

   if (traverse_ItemDeclaration(term, decl_list, def_or_ref)) {
      // MATCHED ItemDeclaration
   }
   else if (traverse_TableDeclaration(term, def_or_ref)) {
      // MATCHED TableDeclaration -> DataDeclaration
   }
   else if (traverse_ConstantDeclaration(term, decl_list)) {
      // MATCHED ConstantDeclaration
   }
   else if (traverse_BlockDeclaration(term, decl_list, def_or_ref)) {
      // MATCHED BlockDeclaration
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.1 ITEM DECLARATION
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_ItemDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list, int def_or_ref)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ItemDeclaration: %s\n", ATwriteToString(term));
#endif

   ATerm t_name, t_alloc, t_type, t_preset;
   char* name;

   SgType* sg_declared_type = nullptr;
   SgExpression* sg_preset = nullptr;

   SgUntypedType* declared_type = NULL;
   SgUntypedExpression*  preset = NULL;
   SgUntypedVariableDeclaration* variable_decl = NULL;
   SgUntypedInitializedNameList* var_name_list = NULL;
   SgUntypedInitializedName*  initialized_name = NULL;
   SgUntypedExprListExpression*      attr_list = NULL;

// For StatusItemDescription
   bool has_size;
   SgUntypedExpression* status_size;
   SgUntypedInitializedNameList* status_list = NULL;

   std::string label = "";

   if (ATmatch(term, "ItemDeclaration(<term>,<term>,<term>,<term>)", &t_name,&t_alloc,&t_type,&t_preset)) {
      if (ATmatch(t_name, "<str>", &name)) {
         // MATCHED ItemName
      } else return ATfalse;

      attr_list = new SgUntypedExprListExpression();
      ROSE_ASSERT(attr_list);
      setSourcePosition(attr_list, t_type);

      if (    def_or_ref == General_Language_Translation::e_storage_modifier_jovial_def
          ||  def_or_ref == General_Language_Translation::e_storage_modifier_jovial_ref ) {
         SgUntypedOtherExpression* modifier = new SgUntypedOtherExpression(def_or_ref);
         ROSE_ASSERT(modifier);
         setSourcePosition(modifier, term);
         attr_list->get_expressions().push_back(modifier);
      }

      if (traverse_OptAllocationSpecifier(t_alloc, attr_list)) {
         // MATCHED OptAllocationSpecifier
      } else return ATfalse;

      if (traverse_ItemTypeDescription(t_type, sg_declared_type, declared_type, attr_list)) {
         // MATCHED ItemTypeDescription
      }
      else if (traverse_StatusItemDescription(t_type, status_list, has_size, status_size)) {
         // MATCHED StatusItemDescription

         // status item declarations have to be handled differently than other ItemTypeDescription terms

         // also assume an int is sufficient for status_size for now
         //         ROSE_ASSERT(has_size == false);
         if (!has_size) {
            cerr << "WARNING UNIMPLEMENTED: ItemDeclaration - StatusItemDescription with size\n";
         }
         ROSE_ASSERT(status_list);

         SgUntypedEnumDeclaration* enum_decl = new SgUntypedEnumDeclaration(label, name, status_list);
         ROSE_ASSERT(enum_decl);
         setSourcePosition(enum_decl, term);

         decl_list->get_decl_list().push_back(enum_decl);
      } else return ATfalse;

      if (traverse_ItemPreset(t_preset, sg_preset, preset)) {
         // MATCHED ItemPreset
      } else return ATfalse;
   }
   else return ATfalse;

   if (sg_declared_type == nullptr) {
      cerr << "WARNING UNIMPLEMENTED: ItemDeclaration - type is null \n";
      return ATtrue;
   }

// Begin SageTreeBuilder
   SgVariableDeclaration* sg_var_decl;
   sage_tree_builder.Enter(sg_var_decl, std::string(name), sg_declared_type, sg_preset);
   setSourcePosition(sg_var_decl, term);

// Begin language specific constructs
   if (def_or_ref == General_Language_Translation::e_storage_modifier_jovial_def) {
      sg_var_decl->get_declarationModifier().get_storageModifier().setJovialDef();
   }
   else if (def_or_ref == General_Language_Translation::e_storage_modifier_jovial_ref) {
      sg_var_decl->get_declarationModifier().get_storageModifier().setJovialRef();
   }

#if 0 // DELETE_ME
   variable_decl = UntypedBuilder::buildVariableDeclaration(name, declared_type, attr_list, preset);
   ROSE_ASSERT(variable_decl);
   setSourcePosition(variable_decl, term);

   var_name_list = variable_decl->get_variables();
   ROSE_ASSERT(var_name_list);
   setSourcePosition(var_name_list, t_name);

// There will be only one variable declared in Jovial
   initialized_name = var_name_list->get_name_list().front();
   ROSE_ASSERT(initialized_name);
   setSourcePosition(initialized_name, t_name);

   decl_list->get_decl_list().push_back(variable_decl);
#endif

// End SageTreeBuilder
   sage_tree_builder.Leave(sg_var_decl);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_ItemTypeDescription(ATerm term, SgType* &sg_type, SgUntypedType* &type, SgUntypedExprListExpression* attr_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ItemTypeDescription: %s\n", ATwriteToString(term));
#endif

   std::string name;

   if (traverse_IntegerItemDescription(term, sg_type, attr_list)) {
      // MATCHED IntegerItemDescription
   }
   else if (traverse_FloatingItemDescription(term, sg_type, type, attr_list)) {
      // MATCHED FloatingItemDescription
   }
   else if (traverse_FixedItemDescription(term, type, attr_list)) {
      // MATCHED FixedItemDescription
   }
   else if (traverse_BitItemDescription(term, sg_type, type)) {
      // MATCHED BitItemDescription
   }
   else if (traverse_CharacterItemDescription(term, type)) {
      // MATCHED CharacterItemDescription
   }

// traverse_StatusItemDescription call moved to callee traverse_ItemDeclaration
// because it takes different argument types

   else if (traverse_PointerItemDescription(term, type)) {
      // MATCHED PointerItemDescription
   }
   else if (traverse_Name(term, name)) {
      // MATCHED ItemTypeName
      cerr << "WARNING UNIMPLEMENTED: ItemTypeDescription - ItemTypeName\n";
      type = UntypedBuilder::buildType(SgUntypedType::e_user_defined, name);
      ROSE_ASSERT(type);
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.1.1 INTEGER TYPE DESCRIPTIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_IntegerItemDescription(ATerm term, SgType* &sg_type, SgUntypedExprListExpression* attr_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_IntegerItemDescription: %s\n", ATwriteToString(term));
#endif

   ATerm t_type, t_round_or_truncate, t_size;
   bool has_round_or_truncate, has_size, is_signed;
   SgUntypedExpression* size;
   SgExpression* sg_kind = nullptr;
   General_Language_Translation::ExpressionKind modifier_enum;

// The first term, t_type, comes from the lexer and is direct user input: "S", "s", "U", "u".
// We disambiguate it via terms IntegerItemDescription or IntegerItemDescriptionU.
   if (ATmatch(term, "IntegerItemDescription(<term>,<term>,<term>)", &t_type,&t_round_or_truncate,&t_size)) {
      is_signed = true;
   }
   else if (ATmatch(term, "IntegerItemDescriptionU(<term>,<term>,<term>)", &t_type,&t_round_or_truncate,&t_size)) {
      is_signed = false;
   }
   else return ATfalse;

   if (traverse_OptRoundOrTruncate(t_round_or_truncate, &has_round_or_truncate, &modifier_enum)) {
      // MATCHED OptRoundOrTruncate
      if (has_round_or_truncate) {
         if (modifier_enum == General_Language_Translation::e_type_modifier_round) {
            cerr << "WARNING UNIMPLEMENTED: OptRoundOrTruncate - e_type_modifier_round in IntegerItemDescription \n";
         }
         else if (modifier_enum == General_Language_Translation::e_type_modifier_truncate) {
            cerr << "WARNING UNIMPLEMENTED: OptRoundOrTruncate - e_type_modifier_truncate in IntegerItemDescription \n";
         }
         else if (modifier_enum == General_Language_Translation::e_type_modifier_z) {
            cerr << "WARNING UNIMPLEMENTED: OptRoundOrTruncate - e_type_modifier_z in IntegerItemDescription \n";
         }
         else ROSE_ASSERT(false);
      }
   } else return ATfalse;

   if (traverse_OptItemSize(t_size, has_size, sg_kind, size)) {
      // MATCHED OptItemSize
   } else return ATfalse;

// BEWARE! The type has to be built correctly or there will be only one integer type with kind value set to the
// last value used in set_type_kind(expr)
   if (has_size) ROSE_ASSERT(sg_kind);

   if (is_signed) {
      sg_type = SageBuilder::buildIntType(sg_kind);
   } else {
      sg_type = SageBuilder::buildUnsignedIntType(sg_kind);
   }
   ROSE_ASSERT(sg_type);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_OptItemSize(ATerm term, bool & has_size, SgExpression* &sg_size, SgUntypedExpression* & size)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptItemSize: %s\n", ATwriteToString(term));
#endif

   ATerm t_size;

   has_size = false;
   sg_size = NULL;

   if (ATmatch(term, "no-item-size()")) {
     // MATCHED no-item-size
   }
   else if (ATmatch(term, "ItemSize(<term>)", &t_size)) {
      if (traverse_NumericFormula(t_size, sg_size, size)) {
        // MATCHED NumericFormula
      } else return ATfalse;
      has_size = true;
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.1.2 FLOATING TYPE DESCRIPTIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_FloatingItemDescription(ATerm term, SgType* &sg_type, SgUntypedType* &type, SgUntypedExprListExpression* attr_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FloatingItemDescription: %s\n", ATwriteToString(term));
#endif

   ATerm t_round_or_truncate, t_precision;
   bool has_round_or_truncate;
   General_Language_Translation::ExpressionKind modifier_enum;
   SgUntypedExpression* precision;
   SgExpression* sg_precision = nullptr;

   if (ATmatch(term, "FloatingItemDescription(<term>,<term>)", &t_round_or_truncate, &t_precision)) {
      type = UntypedBuilder::buildType(SgUntypedType::e_float);
      sg_type = SgTypeFloat::createType();
   }
   else return ATfalse;

// rounding or truncate mode
   if (traverse_OptRoundOrTruncate(t_round_or_truncate, &has_round_or_truncate, &modifier_enum)) {
      // MATCHED OptRoundOrTruncate
      if (has_round_or_truncate) {
         if (modifier_enum == General_Language_Translation::e_type_modifier_round) {
            cerr << "WARNING UNIMPLEMENTED: OptRoundOrTruncate - e_type_modifier_round in FloatingItemDescription \n";
         }
         else if (modifier_enum == General_Language_Translation::e_type_modifier_truncate) {
            cerr << "WARNING UNIMPLEMENTED: OptRoundOrTruncate - e_type_modifier_truncate in FloatingItemDescription \n";
         }
         else if (modifier_enum == General_Language_Translation::e_type_modifier_z) {
            cerr << "WARNING UNIMPLEMENTED: OptRoundOrTruncate - e_type_modifier_z in FloatingItemDescription \n";
         }
         else ROSE_ASSERT(false);
      }
   }
   else return ATfalse;

// precision
   if (ATmatch(t_precision, "no-precision()")) {
      // MATCHED no-precision
   }
   else if (traverse_NumericFormula(t_precision, sg_precision, precision)) {
      type->set_has_kind(true);
      type->set_type_kind(precision);

      sg_type->set_type_kind(sg_precision);
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_OptRoundOrTruncate(ATerm term, bool* has_round_or_truncate,
                                                                  General_Language_Translation::ExpressionKind* modifier_enum)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptRoundOrTruncate: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "no-round-or-truncate()")) {
      *has_round_or_truncate = false;
   }
   else if (ATmatch(term, "R()")) {
      *has_round_or_truncate = true;
      *modifier_enum = General_Language_Translation::e_type_modifier_round;
   }
   else if (ATmatch(term, "T()")) {
      *has_round_or_truncate = true;
      *modifier_enum = General_Language_Translation::e_type_modifier_truncate;
   }
   else if (ATmatch(term, "Z()")) {
      *has_round_or_truncate = true;
      *modifier_enum = General_Language_Translation::e_type_modifier_z;
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.1.3 FIXED TYPE DESCRIPTIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_FixedItemDescription(ATerm term, SgUntypedType* & type, SgUntypedExprListExpression* attr_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FixedItemDescription: %s\n", ATwriteToString(term));
#endif

   ATerm t_round_or_truncate, t_scale, t_fraction, t_scale_spec, t_frac_spec;
   bool has_round_or_truncate;  //, has_fraction_specifier;
   General_Language_Translation::ExpressionKind modifier_enum;
   SgUntypedExpression * scale, * fraction;
   SgExpression * sg_scale = nullptr, * sg_fraction = nullptr;

   if (ATmatch(term, "FixedItemDescription (<term>,<term>,<term>)", &t_round_or_truncate,&t_scale,&t_fraction)) {
      type = UntypedBuilder::buildType(SgUntypedType::e_unknown);
   }
   else return ATfalse;

   if (traverse_OptRoundOrTruncate(t_round_or_truncate, &has_round_or_truncate, &modifier_enum)) {
      // MATCHED OptRoundOrTruncate
      if (has_round_or_truncate) {
         if (modifier_enum == General_Language_Translation::e_type_modifier_round) {
            cerr << "WARNING UNIMPLEMENTED: OptRoundOrTruncate - e_type_modifier_round in FixedItemDescription \n";
         }
         else if (modifier_enum == General_Language_Translation::e_type_modifier_truncate) {
            cerr << "WARNING UNIMPLEMENTED: OptRoundOrTruncate - e_type_modifier_truncate in FixedItemDescription \n";
         }
         else if (modifier_enum == General_Language_Translation::e_type_modifier_z) {
            cerr << "WARNING UNIMPLEMENTED: OptRoundOrTruncate - e_type_modifier_z in FixedItemDescription \n";
         }
         else ROSE_ASSERT(false);
      }
   } else return ATfalse;

   if (ATmatch(t_scale, "ScaleSpecifier(<term>)", &t_scale_spec)) {
      if (traverse_NumericFormula(t_scale_spec, sg_scale, scale)) {
         type->set_has_kind(true);
         type->set_type_kind(scale);
      } else return ATfalse;
   } else return ATfalse;

   if (ATmatch(t_fraction, "no-fraction-specifier()")) {
      // MATCHED no-fraction-specifier
   }
   else if (ATmatch(t_fraction, "OptFractionSpecifier(<term>)", &t_frac_spec)) {
      if (traverse_FractionSpecifier(t_frac_spec, sg_fraction, fraction)) {
         cerr << "WARNING UNIMPLEMENTED: FractionSpecifier - in FixedItemDescription \n";

         //  CompileTimeNumericFormula   -> FractionSpecifier       {cons("FractionSpecifier")}
         //  ',' FractionSpecifier       -> OptFractionSpecifier    {cons("OptFractionSpecifier")}

      } else return ATfalse;
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_FractionSpecifier(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* & fraction)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FractionSpecifier: %s\n", ATwriteToString(term));
#endif

   ATerm t_fraction;
   fraction = NULL;

   if (ATmatch(term, "FractionSpecifier(<term>)", &t_fraction)) {
      if (traverse_NumericFormula(t_fraction, sg_expr, fraction)) {
        // MATCHED FixedFormula through NumericFormula
      } else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.1.4 BIT TYPE DESCRIPTIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_BitItemDescription(ATerm term, SgType* &sg_type, SgUntypedType* &type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BitItemDescription: %s\n", ATwriteToString(term));
#endif

   ATerm t_type_desc, t_size;
   bool has_size;
   SgUntypedExpression* size;
   SgExpression* sg_size = nullptr;

   if (ATmatch(term, "BitItemDescription(<term>,<term>)", &t_type_desc, &t_size)) {
    // Ignore the BitTypeDesc term t_type_desc.  It was placed in JovialLex and will be 'B' (just syntax)
       type = UntypedBuilder::buildType(SgUntypedType::e_bit);
   } else return ATfalse;

   if (traverse_OptItemSize(t_size, has_size, sg_size, size)) {
      // MATCHED OptItemSize
   } else return ATfalse;

   if (has_size) ROSE_ASSERT(sg_size);

   // TODO - is incorrect (may be closer to a hexadecimal or Hollerith, but used in boolean expressions)
   sg_type = SageBuilder::buildBoolType(sg_size);
   ROSE_ASSERT(sg_type);

   return ATtrue;
}

//========================================================================================
// 2.1.1.5 CHARACTER TYPE DESCRIPTIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_CharacterLiteral(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CharacterLiteral: %s\n", ATwriteToString(term));
#endif

   char* name;
   Jovial_ROSE_Translation::ExpressionKind expr_enum = Jovial_ROSE_Translation::e_referenceExpression;

   if (ATmatch(term, "CharacterLiteral(<str>)", &name)) {
      expr = new SgUntypedReferenceExpression(expr_enum, name);
      setSourcePosition(expr, term);
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_CharacterItemDescription(ATerm term, SgUntypedType* & type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CharacterItemDescription: %s\n", ATwriteToString(term));
#endif

   ATerm t_size;
   bool has_size;
   SgUntypedExpression* size;
   SgExpression* sg_size;

   if (ATmatch(term, "CharacterItemDescription(<term>)", &t_size)) {
      // MATCHED CharacterItemDescription
   } else return ATfalse;

   if (traverse_OptItemSize(t_size, has_size, sg_size, size)) {
      if (has_size) {
         type = UntypedBuilder::buildType(SgUntypedType::e_string);
         type->set_char_length_expression(size);
      }
      else {
         type = UntypedBuilder::buildType(SgUntypedType::e_char);
      }
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.1.6 STATUS TYPE DESCRIPTIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_StatusConstant(ATerm term, SgUntypedInitializedName* & init_name)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_StatusConstant: %s\n", ATwriteToString(term));
#endif

// NOTE: Name -> StatusConstant , Letter -> StatusConstant, ReservedWord -> StatusConstant
   char* name;

   init_name = NULL;

   if (ATmatch(term, "StatusConstant(<str>)", &name)) {
      SgUntypedType* type = UntypedBuilder::buildType(SgUntypedType::e_int);
      ROSE_ASSERT(type);

      init_name = new SgUntypedInitializedName(type, name);
      ROSE_ASSERT(init_name);
      setSourcePosition(init_name, term);
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_StatusItemDescription(ATerm term, SgUntypedInitializedNameList* & status_list, bool & has_size, SgUntypedExpression* & size)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_StatusItemDescription: %s\n", ATwriteToString(term));
#endif

   ATerm t_size, t_sublist;

   size = NULL;
   status_list = NULL;
   SgUntypedExpression* item_size;
   SgExpression* sg_item_size = nullptr;

   if (ATmatch(term, "StatusItemDescription(<term>,<term>)", &t_size, &t_sublist)) {
      if (traverse_OptItemSize(t_size, has_size, sg_item_size, item_size)) {
         // In grammar (2.1.1.6), uses OptStatusSize and StatusSize but cons is ItemSize, so used that traversal
         // MATCHED OptItemSize
      } else return ATfalse;

      status_list = new SgUntypedInitializedNameList();
      ROSE_ASSERT(status_list);
      setSourcePosition(status_list, t_sublist);

      if (traverse_DefaultSublist(t_sublist, status_list)) {
         // MATCHED DefaultSublist
      }
      else if (traverse_StatusList(t_sublist, status_list)) {
         // MATCHED StatusList
      }
      else return ATfalse;
   }
   else return ATfalse;

   ROSE_ASSERT(status_list);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_DefaultSublist(ATerm term, SgUntypedInitializedNameList* status_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_DefaultSublist: %s\n", ATwriteToString(term));
#endif

   ATerm t_sublist;
   SgUntypedInitializedName* initialized_name;

   if (ATmatch(term, "DefaultSublist(<term>)", &t_sublist)) {
      ATermList tail = (ATermList) ATmake("<term>", t_sublist);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_StatusConstant(head, initialized_name)) {
            status_list->get_name_list().push_back(initialized_name);
         } else return ATfalse;
      }
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_OptDefaultSublist(ATerm term, SgUntypedInitializedNameList* status_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptDefaultSublist: %s\n", ATwriteToString(term));
#endif

   ATerm t_sublist;

   if (ATmatch(term, "no-default-sublist()")) {
      // MATCHED no default sublist
   }
   else if (ATmatch(term, "OptDefaultSublist(<term>)", &t_sublist)) {
      if (traverse_DefaultSublist(t_sublist, status_list)) {
         // MATCHED DefaultSublist
      } else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_StatusList(ATerm term, SgUntypedInitializedNameList* status_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_StatusList: %s\n", ATwriteToString(term));
#endif

   ATerm t_sublist, t_specified;

   // 1. default initializer used for first portion of list
   // 2. one specified initializer is used as start for rest of sublist
   //
   // WARNING: do not create multiple lists !!!

   if (ATmatch(term, "StatusList(<term>,<term>)", &t_sublist, &t_specified)) {
      if (traverse_OptDefaultSublist(t_sublist, status_list)) {
         // MATCHED OptDefaultSublist
      } else return ATfalse;

      ATermList tail = (ATermList) ATmake("<term>", t_specified);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_SpecifiedSublist(head, status_list)) {
           // MATCHED SpecifiedSublist
         } else return ATfalse;
      }
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_SpecifiedSublist(ATerm term, SgUntypedInitializedNameList* status_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SpecifiedSublist: %s\n", ATwriteToString(term));
#endif

   ATerm t_formula, t_constant;
   SgUntypedInitializedName* initialized_name;
   SgUntypedExpression* initializer;
   SgExpression* sg_initializer = nullptr;
   bool has_initializer = true;

   if (ATmatch(term, "SpecifiedSublist(<term>,<term>)", &t_formula, &t_constant)) {

      if (traverse_NumericFormula(t_formula, sg_initializer, initializer)) {
         // MATCHED NumericFormula
      } else return ATfalse;

      ATermList tail = (ATermList) ATmake("<term>", t_constant);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_StatusConstant(head, initialized_name)) {
            if (has_initializer) {
               initialized_name->set_has_initializer(true);
               initialized_name->set_initializer(initializer);
               // the one initializer has been used
               has_initializer = false;
            }
            status_list->get_name_list().push_back(initialized_name);
         }
         else return ATfalse;
      }
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.1.7 POINTER TYPE DESCRIPTIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_PointerItemDescription(ATerm term, SgUntypedType* & type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_PointerItemDescription: %s\n", ATwriteToString(term));
#endif

   char* pntr;
   ATerm t_type_name;
   std::string type_name;

   if (ATmatch(term, "PointerItemDescription(<str>,<term>)", &pntr, &t_type_name)) {

      if (traverse_OptTypeName(t_type_name, type, type_name)) {
         // MATCHED OptTypeName
      } else return ATfalse;
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_OptTypeName(ATerm term, SgUntypedType* & type, std::string & name)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptTypeName: %s\n", ATwriteToString(term));
#endif

   ATerm t_type_name;

   if (ATmatch(term, "no-type-name()")) {
      // MATCHED no-type-name
   } else if (ATmatch(term, "TypeName(<term>)", &t_type_name)) {
      if (traverse_Name(t_type_name, name)) {
         cerr << "WARNING UNIMPLEMENTED: TypeName \n";
         // MATCHED TypeName
         type = UntypedBuilder::buildType(SgUntypedType::e_user_defined, name);
         ROSE_ASSERT(type);
      } else return ATfalse;
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.2 TABLE DECLARATION
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_TableDeclaration(ATerm term, int def_or_ref)
{
#if PRINT_ATERM_TRAVERSAL
   printf("\n... traverse_TableDeclaration: %s\n", ATwriteToString(term));
#endif

   ATerm t_name, t_alloc, t_dim_list, t_table_desc;
   char* name;

// A TableDeclaration is a variable declaration.  However, it may also (usually) define a type
// as well, in which case the type will be anonymous, unless only a table type name if given.

   SgUntypedTableType* table_type = NULL;
   SgUntypedVariableDeclaration* variable_decl = NULL;

   SgUntypedStructureDeclaration* table_decl = NULL;
   SgUntypedStructureDefinition*  table_desc = NULL;
   SgUntypedExprListExpression*    attr_list = NULL;
   SgUntypedExprListExpression*     dim_info = NULL;
   SgUntypedExprListExpression*       preset = NULL;

// Begin SageTreeBuilder
   SgJovialTableStatement* sg_table_decl = nullptr;
   SgType* sg_base_type = nullptr;
   SgType* sg_type = nullptr;
   SgExprListExp* sg_dim_info = nullptr;
   SgInitializer* sg_preset = nullptr;
   std::string table_var_name, table_type_name, anon_type_name;
   bool is_type_inherited = false;

   if (ATmatch(term, "TableDeclaration(<term>,<term>,<term>,<term>)", &t_name,&t_alloc,&t_dim_list,&t_table_desc)) {
      std::string label = "";

      sg_dim_info = new SgExprListExp();

      if (ATmatch(t_name, "<str>", &name)) {
         // MATCHED TableName
      } else return ATfalse;

      table_var_name = std::string(name);
      anon_type_name = std::string("_anon_typeof_") + table_var_name;

      if (traverse_OptAllocationSpecifier(t_alloc, attr_list)) {
         // MATCHED OptAllocationSpecifier
      } else return ATfalse;

      if (traverse_OptDimensionList(t_dim_list, sg_dim_info)) {
         // MATCHED OptDimensionList
      } else return ATfalse;

   // Look for a type name first (type will have already been declared by this point).
   // The type name is the name of the base type (this declaration inherits from the base/parent class)
      if (traverse_TableDescriptionName(t_table_desc, table_type_name, sg_type, sg_preset)) {
         ROSE_ASSERT(sg_type);
         is_type_inherited = true;

         std::cout << ".x. found inherited type, type name is " << table_type_name << std::endl;
         std::cout << ".x. found table type " << sg_type->class_name() << std::endl;
      }

   // Look for a base type (this is not inheritance, rather it is similar to the base type of an array type).
   // The base type is the table description and there will be no body.
      else if (traverse_TableDescriptionType(t_table_desc, sg_base_type, sg_preset)) {
         ROSE_ASSERT(sg_base_type);

         std::cout << ".x. found table base type " << sg_base_type->class_name() << std::endl;
         std::cout << ".x. will create type for table type name " << table_type_name << std::endl;

      // This must be anonymous as there is no explicit name for the type.
         SgName sg_name(anon_type_name);

         sg_type = SageBuilder::buildJovialTableType(sg_name, sg_base_type, sg_dim_info, SageBuilder::topScopeStack());
         ROSE_ASSERT(sg_type);
      }

   // Finally check for a table description body. This will need to create a table declaration
   // with a body for the table definition member variables. The declaration will be anonymous
   // and associated with the variable declaration (via baseTypeDefiningDeclaration).
      else if (traverse_TableDescriptionBody(t_table_desc, anon_type_name, sg_table_decl, table_desc)) {
         // MATCHED TableDescription with a structure body
         ROSE_ASSERT(sg_table_decl);

         std::cout << ".x. created table decl " << sg_table_decl << std::endl;

         sg_type = isSgJovialTableType(sg_table_decl->get_type());
         ROSE_ASSERT(sg_type);
      }
      else return ATfalse;
   }
   else return ATfalse;

// Begin SageTreeBuilder
   SgVariableDeclaration* sg_var_decl = nullptr;
   std::cout << ".x. TABLE DECLARATION for variable " << table_var_name << endl << endl;

   sage_tree_builder.Enter(sg_var_decl, table_var_name, sg_type, sg_preset);
   setSourcePosition(sg_var_decl, term);

// Begin language specific constructs
// TODO: CREATE a function for this
   if (def_or_ref == General_Language_Translation::e_storage_modifier_jovial_def) {
      sg_var_decl->get_declarationModifier().get_storageModifier().setJovialDef();
   }
   else if (def_or_ref == General_Language_Translation::e_storage_modifier_jovial_ref) {
      sg_var_decl->get_declarationModifier().get_storageModifier().setJovialRef();
   }

// TODO: The scope for the table definition needs to be case insensitive but this should be done
//       somewhere else (e.g., the builder function)
   SgJovialTableType* type = isSgJovialTableType(sg_type);
   ROSE_ASSERT(type);
   SgJovialTableStatement* decl = isSgJovialTableStatement(type->get_declaration());
   ROSE_ASSERT(decl);
   SgJovialTableStatement* def_decl = isSgJovialTableStatement(decl->get_definingDeclaration());
   ROSE_ASSERT(def_decl);

   if (is_type_inherited == false) {
      SageInterface::setBaseTypeDefiningDeclaration(sg_var_decl, def_decl);
   }

   SgClassDefinition* def = def_decl->get_definition();
   ROSE_ASSERT(def);
// TODO: This should be set someplace else
   def->setCaseInsensitive(true);

#if 0
   //   def_decl->get_scope()->setCaseInsensitive(true);
   //   sg_var_decl->get_scope()->setCaseInsensitive(true);
   cout << ".x. def scope sensitivity is " << def->isCaseInsensitive() << endl;
   def->setCaseInsensitive(true);
   cout << ".x. def scope sensitivity is " << def->isCaseInsensitive() << endl;

   cout << ".x. type is " << type << ": " << type->class_name() << endl;
   cout << ".x. decl is " << decl << ": " << decl->class_name() << endl;
   cout << ".x. def_decl is " << def_decl << ": " << def_decl->class_name() << endl;
   cout << ".x. def is " << def << ": " << def->class_name() << endl;
   cout << ".x. def_decl scope is " << def_decl->get_scope() << ": " << def_decl->get_scope()->isCaseInsensitive() << endl;
   cout << ".x. sg_var_decl scope is " << sg_var_decl->get_scope() << ": " << sg_var_decl->get_scope()->isCaseInsensitive() << endl;
   cout << ".x. def scope sensitivity is " << def->isCaseInsensitive() << endl;
#endif

   sage_tree_builder.Leave(sg_var_decl);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::
traverse_TableDescriptionName(ATerm term, std::string &type_name, SgType* &type, SgInitializer* &preset)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_TableDescriptionName: %s\n", ATwriteToString(term));
#endif

   ATerm t_name, t_preset;

   type = nullptr;
   preset = nullptr;

   if (ATmatch(term, "TableDescriptionName(<term>,<term>)", &t_name, &t_preset)) {
      if (traverse_Name(t_name, type_name)) {
         // MATCHED Name
      } else return ATfalse;

   // The name should be useful
      ROSE_ASSERT(type_name.length() > 0);

   // This type should have already been created by a type declaration statement
      SgClassSymbol* class_symbol = SageInterface::lookupClassSymbolInParentScopes(type_name, SageBuilder::topScopeStack());
      if (class_symbol != NULL) {
         type = class_symbol->get_type();
      }
      ROSE_ASSERT(type != nullptr);

   // TODO
      SgExprListExp* sg_preset_list = nullptr;
      ROSE_ASSERT(sg_preset_list);

      if (traverse_TablePreset(t_preset, sg_preset_list)) {
         // MATCHED TablePreset
         //DONE: cerr << "WARNING UNIMPLEMENTED: TableDescriptionName - table preset \n";
      } else return ATfalse;

   }
   else return ATfalse;

   ROSE_ASSERT(type != nullptr);

   return ATtrue;
}

// This table is array-like in that it doesn't have a table/structure body (but has a base type)
ATbool ATermToSageJovialTraversal::
traverse_TableDescriptionType(ATerm term, SgType* &sg_type, SgInitializer* &sg_preset)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_TableDescriptionType: %s\n", ATwriteToString(term));
#endif

   ATerm t_struct_spec, t_entry_spec;
   ATerm t_name, t_preset;
   std::string type_name;

   sg_type = nullptr;

   SgUntypedExprListExpression* attr_list = NULL;
#if 0
   SgUntypedExprListExpression* preset = NULL;
#endif

   if (ATmatch(term, "TableDescription(<term>,<term>)", &t_struct_spec, &t_entry_spec)) {

   // This is an EntrySpecifier without a body
      if (traverse_EntrySpecifierType(t_entry_spec, sg_type, sg_preset)) {
         // MATCHED EntrySpecifierType
      } else return ATfalse;

      if (traverse_OptStructureSpecifier(t_struct_spec, attr_list)) {
         // MATCHED OptStructureSpecifier
      } else return ATfalse;

   }
   else return ATfalse;

   ROSE_ASSERT(sg_type);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::
traverse_TableDescriptionBody(ATerm term, std::string &type_name, SgJovialTableStatement* &sg_table_decl, SgUntypedStructureDefinition* &table_desc)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_TableDescription: %s\n", ATwriteToString(term));
#endif

   ATerm t_struct_spec, t_entry_spec, t_name, t_preset;
   SgUntypedType* type = NULL;
   SgUntypedExprListExpression* preset = NULL;
   SgUntypedExprListExpression* attr_list = NULL;

   table_desc = NULL;
   sg_table_decl = nullptr;

   if (ATmatch(term, "TableDescription(<term>,<term>)", &t_struct_spec, &t_entry_spec)) {

   // Begin SageTreeBuilder
      Rose::builder::SourcePositionPair sources;
      sage_tree_builder.Enter(sg_table_decl, type_name, sources);

      table_desc = UntypedBuilder::buildJovialTableDescription();
      ROSE_ASSERT(table_desc);
      setSourcePosition(table_desc, term);

      attr_list = table_desc->get_modifiers();
      ROSE_ASSERT(attr_list);
      setSourcePosition(attr_list, t_struct_spec);

      if (traverse_OptStructureSpecifier(t_struct_spec, attr_list)) {
         // MATCHED OptStructureSpecifier
      } else return ATfalse;

      if (traverse_EntrySpecifierBody(t_entry_spec, sg_table_decl, table_desc)){
         // MATCHED EntrySpecifierBody
      } else return ATfalse;
   }
   else return ATfalse;

   ROSE_ASSERT(table_desc);
   table_desc->set_initializer(preset);

#if 0
   std::cout << "TABLE DESCRIPTION table_desc: " << table_desc << " : " << table_desc->class_name() << endl;
   std::cout << "TABLE DESCRIPTION scope: " << table_desc->get_scope() << endl;
   std::cout << "TABLE DESCRIPTION scope decl list: " << table_desc->get_scope()->get_declaration_list() << endl;
#endif

   ROSE_ASSERT(sg_table_decl);

// End SageTreeBuilder
   sage_tree_builder.Leave(sg_table_decl);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::
traverse_EntrySpecifierType(ATerm term, SgType* &sg_type, SgInitializer* &sg_preset)
{
   if (traverse_OrdinaryEntrySpecifierType(term, sg_type, sg_preset)) {
      // MATCHED OrdinaryEntrySpecifier -> EntrySpecifier with an item description (no structure body)
   }
   else if (traverse_SpecifiedEntrySpecifierType(term, sg_type, sg_preset)) {
      // MATCHED SpecifiedEntrySpecifier -> EntrySpecifier with an item description (no structure body)
   }
   else return ATfalse;

   return ATtrue;
}

#if 0 //DELETE_ME
ATbool ATermToSageJovialTraversal::
traverse_EntrySpecifier(ATerm term, SgJovialTableStatement* sg_table_decl,
                                    SgUntypedType* & type, SgUntypedExprListExpression* attr_list, SgUntypedExprListExpression* & preset)
{
   if (traverse_OrdinaryEntrySpecifier(term, sg_table_decl, type, attr_list, preset)) {
      // MATCHED OrdinaryEntrySpecifier -> EntrySpecifier with an item description (no structure body)
   }
   else if (traverse_SpecifiedEntrySpecifier(term, sg_table_decl, type, attr_list, preset)) {
      // MATCHED SpecifiedEntrySpecifier -> EntrySpecifier with an item description (no structure body)
   }
   else return ATfalse;

   return ATtrue;
}
#endif

ATbool ATermToSageJovialTraversal::
traverse_EntrySpecifierBody(ATerm term, SgJovialTableStatement* sg_table_decl, SgUntypedStructureDefinition* table_desc)
{
   if (traverse_OrdinaryEntrySpecifierBody(term, sg_table_decl, table_desc)) {
      // MATCHED OrdinaryEntrySpecifier -> EntrySpecifier with a structure body
   }
   else if (traverse_SpecifiedEntrySpecifierBody(term, sg_table_decl, table_desc)) {
      // MATCHED SpecifiedEntrySpecifier -> EntrySpecifier with a structure body
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.2.1 TABLE DIMENSION LISTS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_OptDimensionList(ATerm term, SgExprListExp* sg_shape)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptDimensionList: %s\n", ATwriteToString(term));
#endif

   ATerm t_dim_list;

   ROSE_ASSERT(sg_shape != nullptr);
   setSourcePosition(sg_shape, term);

   if (ATmatch(term, "no-dimension-list()")) {
   }
   else if (ATmatch(term, "DimensionList(<term>)" , &t_dim_list)) {
      ATermList tail = (ATermList) ATmake("<term>", t_dim_list);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_Dimension(head, sg_shape)) {
            // MATCHED Dimension
         }
      }
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_Dimension(ATerm term, SgExprListExp* sg_shape)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Dimension: %s\n", ATwriteToString(term));
#endif

   ATerm t_opt_lower_bound, t_lower_bound, t_upper_bound;

   SgUntypedExpression* lower_bound = NULL;
   SgUntypedExpression* upper_bound = NULL;
   SgUntypedExpression*      stride = NULL;
   SgUntypedSubscriptExpression* range = NULL;

   SgExpression* sg_lower_bound = nullptr;
   SgExpression* sg_upper_bound = nullptr;
   SgExpression*      sg_stride = nullptr;

   SgSubscriptExpression* sg_range = nullptr;

   General_Language_Translation::ExpressionKind expr_enum = General_Language_Translation::e_unknown;

   if (ATmatch(term, "Dimension(<term>,<term>)", &t_opt_lower_bound, &t_upper_bound)) {
      expr_enum = General_Language_Translation::e_explicit_dimension;

   // Lower bound (optional)
      if (ATmatch(t_opt_lower_bound, "no-lower-bound-option()")) {
         // lower bound has already been initialized to nullptr
      }
      else if (ATmatch(t_opt_lower_bound, "LowerBoundOption(<term>)", &t_lower_bound)) {
         if (traverse_NumericFormula(t_lower_bound, sg_lower_bound, lower_bound)) {
            // MATCHED NumericFormula
         } else return ATfalse;
      } else return ATfalse;

   // Upper bound
      if (traverse_NumericFormula(t_upper_bound, sg_upper_bound, upper_bound)) {
         // MATCHED NumericFormula
      } else return ATfalse;
   }
   else if (ATmatch(term, "DimensionSTAR()")) {
      expr_enum = General_Language_Translation::e_star_dimension;
   }
   else return ATfalse;

// Begin SageTreeBuilder
   if (sg_stride == nullptr) {
      // ROSE (Fortran) uses "1" rather than SgNullExpression
      sg_stride = SageBuilder::buildIntVal_nfi(std::string("1"));
   }

   sg_range = SageBuilder::buildSubscriptExpression_nfi(sg_lower_bound, sg_upper_bound, sg_stride);
   sg_shape->get_expressions().push_back(sg_range);

   return ATtrue;
}

//========================================================================================
// 2.1.2.2 TABLE STRUCTURE
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_OptStructureSpecifier(ATerm term, SgUntypedExprListExpression* attr_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptStructureSpecifier: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "no-structure-specifier()")) {
      // MATCHED no-structure-specifier
   }
   else {
      cerr << "WARNING UNIMPLEMENTED: StructureSpecifier \n";
      return ATtrue;

      ROSE_ASSERT(false);
   }

   return ATtrue;
}

//========================================================================================
// 2.1.2.3 ORDINARY TABLE ENTRIES
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::
traverse_OrdinaryEntrySpecifierType(ATerm term, SgType* &sg_type, SgInitializer* &sg_preset)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OrdinaryEntrySpecifierType: %s\n", ATwriteToString(term));
#endif

   ATerm t_pack_spec, t_item_desc, t_preset;
   bool has_size;

   sg_type = nullptr;
   sg_preset = nullptr;

   SgUntypedExpression* status_size;
   SgUntypedType* type = NULL;
   SgUntypedInitializedNameList* status_list = NULL;
   SgUntypedExprListExpression* attr_list = NULL;

   std::string label = "";

   if (ATmatch(term, "OrdinaryEntrySpecifier(<term>,<term>,<term>)", &t_pack_spec, &t_item_desc, &t_preset)) {

      if (traverse_OptPackingSpecifier(t_pack_spec, attr_list)) {
         // MATCHED OptPackingSpecifier
      } else return ATfalse;

      if (traverse_ItemTypeDescription(t_item_desc, sg_type, type, attr_list)) {
         // Found a base type for the table (not inheritance, likely a primitive type similar to an SgArrayType)
      }
      else if (traverse_StatusItemDescription(t_item_desc, status_list, has_size, status_size)) {
         // MATCHED StatusItemDescription

         // status item declarations have to be handled differently than other ItemTypeDescription terms

         // also assume an int is sufficient for status_size for now

         cerr << "WARNING UNIMPLEMENTED: OrdinaryEntrySpecifierType - StatusItemDescription\n";
#if 0
         ROSE_ASSERT(has_size == false);
         ROSE_ASSERT(status_list);

         SgUntypedEnumDeclaration* enum_decl = new SgUntypedEnumDeclaration(label, name, status_list);
         ROSE_ASSERT(enum_decl);
         setSourcePosition(enum_decl, term);

         // need a SgUntypedDeclarationStatementList to push the enum_decl onto
         //         decl_list->get_decl_list().push_back(enum_decl);
#endif
      }
      else return ATfalse;

   // TODO
      SgExprListExp* sg_preset_list = nullptr;
      ROSE_ASSERT(sg_preset_list);

      ROSE_ASSERT(sg_type);
      if (traverse_TablePreset(t_preset, sg_preset_list)) {
         // MATCHED TablePreset
         //DONE: cerr << "WARNING UNIMPLEMENTED: OrdinaryEntrySpecifierType - TablePreset";
      } else return ATfalse;
   }
   else return ATfalse;

   cout << "OrdinaryEntrySpecifierType - found sg_type " << sg_type << ": " << sg_type->class_name() << endl;
   ROSE_ASSERT(sg_type);

   return ATtrue;
}

#if 0 //DELETE_ME
ATbool ATermToSageJovialTraversal::
traverse_OrdinaryEntrySpecifier(ATerm term, SgJovialTableStatement* sg_table_decl, SgUntypedType* & type, SgUntypedExprListExpression* attr_list, SgUntypedExprListExpression* & preset)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OrdinaryEntrySpecifier: %s\n", ATwriteToString(term));
#endif

   ATerm t_pack_spec, t_item_desc, t_preset;

   ROSE_ASSERT(sg_table_decl);

   type = NULL;
   preset = NULL;

   SgType* sg_type = nullptr;

// For StatusItemDescription
   bool has_size;
   SgUntypedExpression* status_size;
   SgUntypedInitializedNameList* status_list = NULL;

   std::string label = "";

   if (ATmatch(term, "OrdinaryEntrySpecifier(<term>,<term>,<term>)", &t_pack_spec, &t_item_desc, &t_preset)) {

      if (traverse_OptPackingSpecifier(t_pack_spec, attr_list)) {
         // MATCHED OptPackingSpecifier
      } else return ATfalse;

      if (traverse_ItemTypeDescription(t_item_desc, sg_type, type, attr_list)) {
         // Found a base type for the table
         SgJovialTableType* sg_table_type = isSgJovialTableType(sg_table_decl->get_type());
         ROSE_ASSERT(sg_table_type);
         sg_table_type->set_base_type(sg_type);
      }
      else if (traverse_StatusItemDescription(t_item_desc, status_list, has_size, status_size)) {
         // MATCHED StatusItemDescription

         // status item declarations have to be handled differently than other ItemTypeDescription terms

         // also assume an int is sufficient for status_size for now

         cerr << "WARNING UNIMPLEMENTED: OrdinaryEntrySpecifier - StatusItemDescription\n";
#if 0
         ROSE_ASSERT(has_size == false);
         ROSE_ASSERT(status_list);

         SgUntypedEnumDeclaration* enum_decl = new SgUntypedEnumDeclaration(label, name, status_list);
         ROSE_ASSERT(enum_decl);
         setSourcePosition(enum_decl, term);

         // need a SgUntypedDeclarationStatementList to push the enum_decl onto
         //         decl_list->get_decl_list().push_back(enum_decl);
#endif
      }
      else return ATfalse;

      if (traverse_TablePreset(t_preset, preset)) {
         // MATCHED TablePreset
      } else return ATfalse;
   }
   else return ATfalse;

   ROSE_ASSERT(type);

   return ATtrue;
}
#endif

ATbool ATermToSageJovialTraversal::
traverse_OrdinaryEntrySpecifierBody(ATerm term, SgJovialTableStatement* sg_table_decl, SgUntypedStructureDefinition* table_desc)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OrdinaryEntrySpecifierBody: %s\n", ATwriteToString(term));
#endif

   ATerm t_pack_spec, t_preset, t_body;

   ROSE_ASSERT(table_desc);

   SgUntypedExprListExpression* attr_list    = table_desc->get_modifiers();

   SgUntypedScope* table_scope = table_desc->get_scope();
   ROSE_ASSERT(table_scope);

   SgUntypedDeclarationStatementList* decl_list = table_scope->get_declaration_list();
   ROSE_ASSERT(decl_list);

   SgExprListExp* sg_preset_list = nullptr;

   if (ATmatch(term, "OrdinaryEntrySpecifierBody(<term>,<term>,<term>)", &t_pack_spec, &t_preset, &t_body)) {

      if (traverse_OptPackingSpecifier(t_pack_spec, attr_list)) {
         // MATCHED OptPackingSpecifier
      } else return ATfalse;

// TODO ------------------------
      SgType* sg_type = nullptr;
      ROSE_ASSERT(sg_type);
      if (traverse_TablePreset(t_preset, sg_preset_list)) {
         // MATCHED TablePreset
      } else return ATfalse;

      if (traverse_OrdinaryTableBody(t_body, decl_list)) {
         // MATCHED OrdinaryTableBody
      } else return ATfalse;
   }
   else return ATfalse;

// TODO
   ROSE_ASSERT(false);
#if 0
   if (table_preset != NULL) {
      table_desc->set_initializer(table_preset);
   }
#endif

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_OrdinaryTableBody(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OrdinaryTableBody: %s\n", ATwriteToString(term));
#endif

   ROSE_ASSERT(decl_list);

   ATerm t_directives, t_table_option_list;

   if (ATmatch(term, "OrdinaryTableBody(<term>,<term>)", &t_directives, &t_table_option_list)) {

      if (traverse_DirectiveList(t_directives, decl_list)) {
         // MATCHED DirectiveList
      } else return ATfalse;

      ATermList tail = (ATermList) ATmake("<term>", t_table_option_list);
      // Match OrdinaryTableOptions
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_OrdinaryTableItemDeclaration(head, decl_list)) {
            // MATCHED OrdinaryTableItemDeclaration
         }
         else if (traverse_NullDeclaration(head, decl_list)) {
            // MATCHED NullDeclaration
         }
         else return ATfalse;
      }
   }
   else if (traverse_OrdinaryTableItemDeclaration(term, decl_list)) {
      // MATCHED OrdinaryTableItemDeclaration
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_OrdinaryTableItemDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OrdinaryTableItemDeclaration: %s\n", ATwriteToString(term));
#endif

   ROSE_ASSERT(decl_list);

   ATerm t_name, t_item_desc, t_pack_spec, t_preset;
   char* name;

   SgType* sg_item_type = nullptr;
   SgExprListExp* sg_preset_list = nullptr;

   SgUntypedType* item_type = NULL;
   SgUntypedVariableDeclaration* variable_decl = NULL;
   SgUntypedExprListExpression*  attr_list     = NULL;

// For StatusItemDescription
   bool has_size;
   SgUntypedExpression* status_size;
   SgUntypedInitializedNameList* status_list = NULL;

   std::string label = "";

   if (ATmatch(term, "OrdinaryTableItemDeclaration(<term>,<term>,<term>,<term>)", &t_name,&t_item_desc,&t_pack_spec,&t_preset)) {

      if (ATmatch(t_name, "<str>", &name)) {
         // MATCHED TableItemName
      } else return ATfalse;

      attr_list = new SgUntypedExprListExpression(General_Language_Translation::e_struct_item_modifier_list);
      ROSE_ASSERT(attr_list);
      setSourcePosition(attr_list, t_item_desc);

      if (traverse_ItemTypeDescription(t_item_desc, sg_item_type, item_type, attr_list)) {
         // MATCHED ItemTypeDescription
      } else if (traverse_StatusItemDescription(t_item_desc, status_list, has_size, status_size)) {
         // MATCHED StatusItemDescription

         // status item declarations have to be handled differently than other ItemTypeDescription terms

         // also assume an int is sufficient for status_size for now

         ROSE_ASSERT(has_size == false);
         ROSE_ASSERT(status_list);

         SgUntypedEnumDeclaration* enum_decl = new SgUntypedEnumDeclaration(label, name, status_list);
         ROSE_ASSERT(enum_decl);
         setSourcePosition(enum_decl, term);

         decl_list->get_decl_list().push_back(enum_decl);
      } else return ATfalse;

      if (traverse_TablePreset(t_preset, sg_preset_list)) {
         // MATCHED TablePreset
      } else return ATfalse;

   } else return ATfalse;

   // TODO - handle preset
   if (sg_preset_list != NULL) {
      cerr << "WARNING UNIMPLEMENTED: OrdinaryTableItemDeclaration - preset \n";
      ROSE_ASSERT(false);
      return ATtrue;
   }
   ROSE_ASSERT(sg_preset_list == NULL);

   ROSE_ASSERT(attr_list);
   if (item_type == NULL) {
      // ROSE_ASSERT(item_type);
      cerr << "WARNING UNIMPLEMENTED: OrdinaryTableItemDeclaration - item_type \n";
      return ATtrue;
   }

   if (item_type == NULL) {
      cerr << "WARNING UNIMPLEMENTED: OrdinaryTableItemDeclaration - type is null \n";
      return ATtrue;
   }

// Begin SageTreeBuilder
   SgVariableDeclaration* sg_var_decl;
   sage_tree_builder.Enter(sg_var_decl, std::string(name), sg_item_type, nullptr);
   setSourcePosition(sg_var_decl, term);

#if 0
   variable_decl = UntypedBuilder::buildVariableDeclaration(name, item_type, attr_list, preset);
   ROSE_ASSERT(variable_decl);
   setSourcePosition(variable_decl, term);

// Set the source positions
//
   SgUntypedInitializedNameList* var_name_list = variable_decl->get_variables();
   ROSE_ASSERT(var_name_list);
   setSourcePosition(var_name_list, t_name);

// There will be only one variable declared in Jovial
   SgUntypedInitializedName* initialized_name = var_name_list->get_name_list()[0];
   ROSE_ASSERT(initialized_name);
   setSourcePosition(initialized_name, t_name);

   decl_list->get_decl_list().push_back(variable_decl);
#endif

   sage_tree_builder.Leave(sg_var_decl);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_OptPackingSpecifier(ATerm term, SgUntypedExprListExpression* attr_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptPackingSpecifier: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "no-packing-specifier()")) {
      // MATCHED no-packing-specifier
   }
   else if (ATmatch(term, "PackingSpecifierN()")) {
      // TODO - add attribute
      cerr << "WARNING UNIMPLEMENTED: OptPackingSpecifier - N \n";
      return ATtrue;
   }
   else if (ATmatch(term, "PackingSpecifierM()")) {
      // TODO - add attribute
      cerr << "WARNING UNIMPLEMENTED: OptPackingSpecifier - M \n";
      return ATtrue;
   }
   else if (ATmatch(term, "PackingSpecifierD()")) {
      // TODO - add attribute
      cerr << "WARNING UNIMPLEMENTED: OptPackingSpecifier - D \n";
      return ATtrue;
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.2.4 SPECIFIED TABLE ENTRIES
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_SpecifiedEntrySpecifierType(ATerm term, SgType* &sg_type, SgInitializer* &sg_preset)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SpecifiedEntrySpecifierType: %s\n", ATwriteToString(term));
#endif

   ATerm t_words, t_item_desc, t_preset;

   sg_type = nullptr;
   sg_preset = nullptr;

//TODO: convert to sage typed system
#if 0
   ROSE_ASSERT(attr_list);

   type = NULL;
   preset = NULL;

   if (ATmatch(term, "SpecifiedEntrySpecifier(<term>,<term>,<term>)", &t_words, &t_item_desc, &t_preset)) {

      if (traverse_WordsPerEntry(t_words, sg_table_decl, attr_list)) {
         // MATCHED WordsPerEntry
      } else return ATfalse;

      if (traverse_SpecifiedItemDescription(t_item_desc, type, attr_list)) {
         // MATCHED SpecifiedItemDescription
      } else return ATfalse;
      
      if (traverse_TablePreset(t_preset, sg_preset, sg_type)) {
         // MATCHED TablePreset
      } else return ATfalse;
   }
   else return ATfalse;

   ROSE_ASSERT(type);
#endif

   return ATfalse;

   return ATtrue;
}

#if 0 //DELETE_ME
ATbool ATermToSageJovialTraversal::traverse_SpecifiedEntrySpecifier(ATerm term, SgJovialTableStatement* sg_table_decl, SgUntypedType* & type, SgUntypedExprListExpression* attr_list, SgUntypedExprListExpression* & preset)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SpecifiedEntrySpecifier: %s\n", ATwriteToString(term));
#endif

   ATerm t_words, t_item_desc, t_preset;

   ROSE_ASSERT(attr_list);

   type = NULL;
   preset = NULL;

   if (ATmatch(term, "SpecifiedEntrySpecifier(<term>,<term>,<term>)", &t_words, &t_item_desc, &t_preset)) {

      if (traverse_WordsPerEntry(t_words, sg_table_decl, attr_list)) {
         // MATCHED WordsPerEntry
      } else return ATfalse;

      if (traverse_SpecifiedItemDescription(t_item_desc, type, attr_list)) {
         // MATCHED SpecifiedItemDescription
      } else return ATfalse;

      if (traverse_TablePreset(t_preset, preset)) {
         // MATCHED TablePreset
      } else return ATfalse;
   }
   else return ATfalse;

   ROSE_ASSERT(type);

   return ATtrue;
}
#endif

ATbool ATermToSageJovialTraversal::
traverse_SpecifiedEntrySpecifierBody(ATerm term, SgJovialTableStatement* sg_table_decl, SgUntypedStructureDefinition* table_desc)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SpecifiedEntrySpecifierBody: %s\n", ATwriteToString(term));
#endif

   ATerm t_words, t_preset, t_body;

   ROSE_ASSERT(table_desc);

   SgUntypedExprListExpression* attr_list = table_desc->get_modifiers();

   SgUntypedScope* table_scope = table_desc->get_scope();
   ROSE_ASSERT(table_scope);

   SgUntypedDeclarationStatementList* decl_list = table_scope->get_declaration_list();
   ROSE_ASSERT(decl_list);

   SgExprListExp* sg_preset_list = nullptr;

   if (ATmatch(term, "SpecifiedEntrySpecifierBody(<term>,<term>,<term>)", &t_words, &t_preset, &t_body)) {

      if (traverse_WordsPerEntry(t_words, sg_table_decl, attr_list)) {
         // MATCHED WordsPerEntry
      } else return ATfalse;

      if (traverse_TablePreset(t_preset, sg_preset_list)) {
         // MATCHED TablePreset
      } else return ATfalse;

      if (traverse_SpecifiedTableBody(t_body, decl_list)) {
         // MATCHED SpecifiedTableBody
      } else return ATfalse;

   }
   else return ATfalse;

// TODO
// table_desc->set_initializer(table_preset);
   ROSE_ASSERT(false);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_SpecifiedItemDescription(ATerm term, SgUntypedType* & type, SgUntypedExprListExpression* attr_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SpecifiedItemDescription: %s\n", ATwriteToString(term));
#endif

   ATerm t_item_desc, t_loc_spec, t_start_bit, t_start_word;

   ROSE_ASSERT(attr_list);

   SgUntypedExpression* start_bit = NULL;
   SgUntypedExpression* start_word = NULL;
   SgUntypedExprListExpression* loc_attr = NULL;

// For StatusItemDescription
   bool has_size;
   SgUntypedExpression* status_size;
   SgUntypedInitializedNameList* status_list = NULL;

   SgType* sg_type = nullptr;

   std::string label = "";

   if (ATmatch(term, "SpecifiedItemDescription(<term>,<term>)", &t_item_desc, &t_loc_spec)) {

      if (traverse_ItemTypeDescription(t_item_desc, sg_type, type, attr_list)) {
         // MATCHED ItemTypeDescription
      }  else if (traverse_StatusItemDescription(t_item_desc, status_list, has_size, status_size)) {
         // MATCHED StatusItemDescription
         // status item declarations have to be handled differently than other ItemTypeDescription terms

         type = UntypedBuilder::buildType(SgUntypedType::e_status);
         ROSE_ASSERT(type);

         cerr << "WARNING UNIMPLEMENTED: SpecifiedItemDescription - StatusItemDescription\n";
#if 0
         // also assume an int is sufficient for status_size for now
         ROSE_ASSERT(has_size == false);
         ROSE_ASSERT(status_list);

         SgUntypedEnumDeclaration* enum_decl = new SgUntypedEnumDeclaration(label, name, status_list);
         ROSE_ASSERT(enum_decl);
         setSourcePosition(enum_decl, term);

         // need a SgUntypedDeclarationStatementList to push the enum_decl onto
         //         decl_list->get_decl_list().push_back(enum_decl);
#endif
      } else return ATfalse;

      // process location-specifier here (don't really need to call a function)
      if (ATmatch(t_loc_spec, "LocationSpecifier(<term>,<term>)", &t_start_bit, &t_start_word)) {

         loc_attr = new SgUntypedExprListExpression(General_Language_Translation::e_storage_modifier_location);
         ROSE_ASSERT(loc_attr);
         setSourcePosition(loc_attr, t_loc_spec);

      // Begin SageTreeBuilder
         SgExpression* sg_expr = nullptr;

         if (traverse_Formula(t_start_bit, sg_expr, start_bit)) {
            // MATCHED StartingBit
            loc_attr->get_expressions().push_back(start_bit);
         } else if (ATmatch(term, "StartingBitSTAR()")) {
            cerr << "WARNING UNIMPLEMENTED: SpecifiedItemDescription - StartingBitSTAR \n";
         } else return ATfalse;

         if (traverse_Formula(t_start_word, sg_expr, start_word)) {
            // MATCHED StartingWord
            loc_attr->get_expressions().push_back(start_word);
         } else return ATfalse;
      }

   } else return ATfalse;

   ROSE_ASSERT(loc_attr);
   ROSE_ASSERT(attr_list);

   attr_list->get_expressions().push_back(loc_attr);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_SpecifiedTableBody(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SpecifiedTableBody: %s\n", ATwriteToString(term));
#endif

   ROSE_ASSERT(decl_list);

   ATerm t_table_option_list;

   if (ATmatch(term, "SpecifiedTableBody(<term>)", &t_table_option_list)) {
      ATermList tail = (ATermList) ATmake("<term>", t_table_option_list);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_SpecifiedTableItemDeclaration(head, decl_list)) {
            // MATCHED SpecifiedTableItemDeclaration
         }
         else if (traverse_NullDeclaration(head, decl_list)) {
            // MATCHED NullDeclaration
         } else return ATfalse;
      }
   }

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_SpecifiedTableItemDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SpecifiedTableItemDeclaration: %s\n", ATwriteToString(term));
#endif

   ROSE_ASSERT(decl_list);

   ATerm t_spec_item_desc, t_amb, t_name, t_item_desc, t_preset;
   char* name;

   SgUntypedType* declared_type = NULL;
   SgUntypedExprListExpression* preset  = NULL;
   SgUntypedVariableDeclaration* variable_decl = NULL;
   SgUntypedExprListExpression*  attr_list     = NULL;

   SgExprListExp* sg_preset_list = nullptr;

   if (ATmatch(term, "SpecifiedTableItemDeclaration(<term>,<term>,<term>)", &t_name, &t_spec_item_desc, &t_preset)) {

      // SpecifiedTableItemDeclaration can have an ambiguity if an SpecifiedItemDescription has a type name starting with "a"
      if (ATmatch(t_spec_item_desc, "amb(<term>)", &t_amb)) {
         // MATCHED an ambiguity, choose the first one
         ATermList tail = (ATermList) ATmake("<term>", t_amb);
         t_item_desc = ATgetFirst(tail);
      }
      else {
         t_item_desc = t_spec_item_desc;
      }

      if (ATmatch(t_name, "<str>", &name)) {
         // MATCHED TableItemName
      } else return ATfalse;

      attr_list = new SgUntypedExprListExpression(General_Language_Translation::e_struct_item_modifier_list);
      ROSE_ASSERT(attr_list);
      setSourcePosition(attr_list, t_item_desc);

      if (traverse_SpecifiedItemDescription(t_item_desc, declared_type, attr_list)) {
         // MATCHED SpecifiedItemDescription
      } else return ATfalse;

      if (traverse_TablePreset(t_preset, sg_preset_list)) {
         // MATCHED TablePreset
      } else return ATfalse;

   } else return ATfalse;

   // TODO - handle preset
   //   ROSE_ASSERT(preset == NULL);
   ROSE_ASSERT(false);
   cerr << "WARNING UNIMPLEMENTED: SpecifiedTableItemDeclaration - preset\n";

   ROSE_ASSERT(attr_list);
   if (!declared_type) {
      cerr << "WARNING UNIMPLEMENTED: SpecifiedTableItemDeclaration - NOT declared_type\n";
      return ATtrue;
   }
   ROSE_ASSERT(declared_type);

   if (declared_type == NULL) {
      cerr << "WARNING UNIMPLEMENTED: SpecifiedTableItemDeclaration - type is null \n";
      return ATtrue;
   }

   variable_decl = UntypedBuilder::buildVariableDeclaration(name, declared_type, attr_list, preset);
   ROSE_ASSERT(variable_decl);
   setSourcePosition(variable_decl, term);

   // TODO - set source position of the initialized_name and variable_name_list

   decl_list->get_decl_list().push_back(variable_decl);

#if 0
   std::cout << "TABLE ITEM DECLARATION " << name << endl;
   std::cout << "TABLE ITEM DECLARATION attr_list: " << attr_list << " : size " << attr_list->get_expressions().size() << endl;
#endif

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_WordsPerEntry(ATerm term, SgJovialTableStatement* sg_table_decl, SgUntypedExprListExpression* attr_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_WordsPerEntry: %s\n", ATwriteToString(term));
#endif

   ATerm t_size;

   SgUntypedExpression* entry_size = NULL;
   SgExpression* sg_entry_size = nullptr;
   int entry_type = Jovial_ROSE_Translation::e_unknown;

   if (ATmatch(term, "WordsPerEntryW(<term>)", &t_size)) {
      if (ATmatch(term, "no-entry-size()")) {
         // MATCHED no-entry-size
      }
      else if (traverse_NumericFormula(t_size, sg_entry_size, entry_size)) {
         entry_type = Jovial_ROSE_Translation::e_words_per_entry_w;

         // An expr list is needed to contain the size expression and its type
         SgUntypedExprListExpression* entry_list = new SgUntypedExprListExpression(entry_type);
         ROSE_ASSERT(entry_list);
         setSourcePosition(entry_list, term);

         entry_list->get_expressions().push_back(entry_size);
         attr_list ->get_expressions().push_back(entry_list);

         sg_table_decl->set_has_table_entry_size(true);
         sg_table_decl->set_table_entry_size(sg_entry_size);
      }
      else return ATfalse;
   }
   else if (ATmatch(term, "WordsPerEntryV()")) {
      entry_type = Jovial_ROSE_Translation::e_words_per_entry_v;
      entry_size = new SgUntypedOtherExpression(entry_type);
      ROSE_ASSERT(entry_size);
      setSourcePosition(entry_size, term);

      attr_list->get_expressions().push_back(entry_size);
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.3 CONSTANT DECLARATIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_ConstantDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ConstantDeclaration: %s\n", ATwriteToString(term));
#endif

   ATerm t_name, t_type, t_preset, t_dim_list, t_table_desc;
   char* name;

   SgUntypedType* declared_type = NULL;
   SgType* sg_declared_type = nullptr;
   SgUntypedExpression* preset = NULL;
   SgExpression* sg_preset = nullptr;

   SgUntypedVariableDeclaration* variable_decl = NULL;
   SgUntypedExprListExpression*      attr_list = NULL;

// For StatusItemDescription
   bool has_size;
   SgUntypedExpression* status_size;
   SgUntypedInitializedNameList* status_list = NULL;

   std::string label = "";

   if (ATmatch(term, "ConstantItemDeclaration(<term>,<term>,<term>)", &t_name,&t_type,&t_preset)) {
      if (ATmatch(t_name, "<str>", &name)) {
         // MATCHED ItemName
      } else return ATfalse;

      attr_list = new SgUntypedExprListExpression(General_Language_Translation::e_type_modifier_list);
      ROSE_ASSERT(attr_list);
      setSourcePosition(attr_list, t_type);

      if (traverse_ItemTypeDescription(t_type, sg_declared_type, declared_type, attr_list)) {
         // MATCHED ItemTypeDescription
      } else if (traverse_StatusItemDescription(t_type, status_list, has_size, status_size)) {
         // MATCHED StatusItemDescription

         // status item declarations have to be handled differently than other ItemTypeDescription terms

         // also assume an int is sufficient for status_size for now

         ROSE_ASSERT(has_size == false);
         ROSE_ASSERT(status_list);

         SgUntypedEnumDeclaration* enum_decl = new SgUntypedEnumDeclaration(label, name, status_list);
         ROSE_ASSERT(enum_decl);
         setSourcePosition(enum_decl, term);

         decl_list->get_decl_list().push_back(enum_decl);
      } else return ATfalse;

      if (traverse_ItemPreset(t_preset, sg_preset, preset)) {
         // MATCHED ItemPreset
      } else return ATfalse;
   }

   else if (ATmatch(term, "ConstantTableDeclaration(<term>,<term>,<term>)", &t_name,&t_dim_list,&t_table_desc)) {
      cerr << "WARNING UNIMPLEMENTED: ConstantTableDeclaration \n";
      return ATtrue;
   }
   else return ATfalse;

   if (declared_type == NULL) {
      cerr << "WARNING UNIMPLEMENTED: ConstantDeclaration - type is null \n";
      return ATtrue;
   }

// Begin SageTreeBuilder
   SgVariableDeclaration* sg_var_decl;
   sage_tree_builder.Enter(sg_var_decl, std::string(name), sg_declared_type, sg_preset);
   setSourcePosition(sg_var_decl, term);

// This is a ConstanItemDeclaration
   sg_var_decl->get_declarationModifier().get_typeModifier().get_constVolatileModifier().setConst();

   variable_decl = UntypedBuilder::buildVariableDeclaration(name, declared_type, attr_list, preset);
   ROSE_ASSERT(variable_decl);
   setSourcePosition(variable_decl, term);

// Set the source positions
//
   SgUntypedInitializedNameList* var_name_list = variable_decl->get_variables();
   ROSE_ASSERT(var_name_list);
   setSourcePosition(var_name_list, t_name);

// There will be only one variable declared in Jovial
   SgUntypedInitializedName* initialized_name = var_name_list->get_name_list()[0];
   ROSE_ASSERT(initialized_name);
   setSourcePosition(initialized_name, t_name);

   SgUntypedOtherExpression* attr = new SgUntypedOtherExpression(General_Language_Translation::e_type_modifier_const);
   ROSE_ASSERT(attr);
   setSourcePositionUnknown(attr);

   attr_list->get_expressions().push_back(attr);
   decl_list->get_decl_list().push_back(variable_decl);

// End SageTreeBuilder
   sage_tree_builder.Leave(sg_var_decl);

   return ATtrue;
}

//========================================================================================
// 2.1.4 BLOCK DECLARATION
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_BlockDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list, int def_or_ref)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BlockDeclaration: %s\n", ATwriteToString(term));
#endif

   ATerm t_name, t_alloc, t_body, t_type_name, t_preset;
   std::string block_name, block_type_name;
   bool is_anon = false;

   SgUntypedStructureDeclaration* block_decl = NULL;

   // Begin SageTreeBuilder
   SgJovialTableStatement* sg_block_decl = nullptr;
   SgType* sg_type = nullptr;
   SgExprListExp* sg_preset_list = nullptr;
   std::string type_name;

   if (ATmatch(term, "BlockDeclarationBodyPart(<term>,<term>,<term>)", &t_name, &t_alloc, &t_body)) {
      // TODO list
      // 1. need block type declaration ("named anonymous")
      // 2. need variable declaration
      // 3. need source position information
      // 4. make sure STATIC works

      is_anon = true;

      if (traverse_Name(t_name, block_name)) {
         // MATCHED BlockName
      } else return ATfalse;

      // TODO: function to create anaonymous name
      block_type_name = "_anon_typeof_" + block_name;

      type_name = block_type_name;
      int struct_type = Jovial_ROSE_Translation::e_block_type_declaration;

      Rose::builder::SourcePositionPair sources;
      sage_tree_builder.Enter(sg_block_decl, type_name, sources, /*is_block*/ true);


// This portion could be moved to UntypedBuilder::
//--------------------------------------------------
      SgUntypedStructureDefinition* struct_def  = NULL;
      SgUntypedExprListExpression*    modifiers = NULL;
      SgUntypedExprListExpression*        shape = NULL;

      struct_def = UntypedBuilder::buildStructureDefinition(type_name, /*has_body*/true, /*scope*/NULL);
      ROSE_ASSERT(struct_def != NULL);
      SageInterface::setSourcePosition(struct_def);

      modifiers = new SgUntypedExprListExpression(General_Language_Translation::e_struct_modifier_list);
      ROSE_ASSERT(modifiers != NULL);
      SageInterface::setSourcePosition(modifiers);

      // There may be a shape if a Jovial table
      shape = new SgUntypedExprListExpression(General_Language_Translation::e_array_shape);
      ROSE_ASSERT(shape);
      SageInterface::setSourcePosition(shape);

      std::string label = "";
      block_decl = new SgUntypedStructureDeclaration(label, struct_type, type_name, modifiers, shape, struct_def);
      ROSE_ASSERT(block_decl);
      SageInterface::setSourcePosition(block_decl);
//--------------------------------------------------

      SgUntypedStructureDefinition* block_def = struct_def;

      ROSE_ASSERT(block_def != NULL);
      setSourcePosition(block_def, t_body);

      SgUntypedScope* block_scope = struct_def->get_scope();
      ROSE_ASSERT(block_scope != NULL);

      SgUntypedDeclarationStatementList* block_decl_list = block_scope->get_declaration_list();
      ROSE_ASSERT(block_decl_list);

      modifiers = block_decl->get_modifiers();
      ROSE_ASSERT(modifiers != NULL);

      if (traverse_OptAllocationSpecifier(t_alloc, modifiers)) {
         // MATCHED OptAllocationSpecifier
      } else return ATfalse;

      if (traverse_BlockBodyPart(t_body, sg_block_decl, block_decl_list)) {
         // MATCHED BlockBodyPart
      } else return ATfalse;

      sg_type = isSgJovialTableType(sg_block_decl->get_type());
      sage_tree_builder.Leave(sg_block_decl);
   }

   else if (ATmatch(term, "BlockDeclarationTypeName(<term>,<term>,<term>,<term>)", &t_name, &t_alloc, &t_type_name, &t_preset)) {

      if (traverse_Name(t_name, block_name)) {
         // MATCHED BlockName
      } else return ATfalse;

      if (traverse_Name(t_type_name, block_type_name)) {
         // MATCHED BlockTypeName
      } else return ATfalse;

//DONE: cerr << "WARNING UNIMPLEMENTED: BlockDeclarationTypeName\n";

      ROSE_ASSERT(block_type_name.length() > 0);

      // This type should have already been created by a type declaration statement
      SgClassSymbol* class_symbol = SageInterface::lookupClassSymbolInParentScopes(block_type_name, SageBuilder::topScopeStack());
      if (class_symbol != NULL) {
         sg_type = class_symbol->get_type();
      }
      ROSE_ASSERT(sg_type != nullptr);

      //      SgUntypedExprListExpression* modifiers = block_decl->get_modifiers();
      //      ROSE_ASSERT(modifiers != NULL);
      //      SageInterface::setSourcePosition(modifiers);

      // copied from other BlockDeclaration above
      SgUntypedExprListExpression* modifiers = new SgUntypedExprListExpression(General_Language_Translation::e_struct_modifier_list);
      ROSE_ASSERT(modifiers != NULL);
      SageInterface::setSourcePosition(modifiers);

      if (traverse_OptAllocationSpecifier(t_alloc, modifiers)) {
         // MATCHED OptAllocationSpecifier
      } else return ATfalse;

      if (traverse_BlockPreset(t_preset, sg_preset_list)) {
         // MATCHED BlockPreset
      } else return ATfalse;
   }
   else return ATfalse;

#if 0
//DONE: cerr << "WARNING UNIMPLEMENTED: BlockDeclarationBodyPart __implementing__ \n";

   // we have the type declaration, now we need a variable declaration
   ROSE_ASSERT(block_decl);
   SgUntypedExprListExpression* modifiers = block_decl->get_modifiers();

   // TODO: change this to SgUntypedType::e_block
   SgUntypedType* block_type = UntypedBuilder::buildType(SgUntypedType::e_table, block_type_name);

   // TODO: is modifiers the correct list?

   if (block_type == NULL) {
      cerr << "WARNING UNIMPLEMENTED: BlockDeclaration - type is null \n";
      return ATtrue;
   }

   var_decl = UntypedBuilder::buildVariableDeclaration(block_name, block_type, modifiers, preset);
   ROSE_ASSERT(var_decl != NULL);
   setSourcePosition(var_decl, term);

   // TODO: it seems like the block type should have a pointer to the block_decl?
   decl_list->get_decl_list().push_back(block_decl);
   decl_list->get_decl_list().push_back(var_decl);
#endif

   // Begin SageTreeBuilder
   SgVariableDeclaration* sg_var_decl = nullptr;

   std::cout << "BLOCK DECLARATION TYPE NAME" << block_name << " " << block_type_name << endl;
// TODO
   SgInitializer* sg_preset = nullptr;
   ROSE_ASSERT(sg_preset);

   sage_tree_builder.Enter(sg_var_decl, std::string(block_name), sg_type, sg_preset);
   setSourcePosition(sg_var_decl, term);

   // Begin language specific constructs
   // TODO: CREATE a function for this
   if (def_or_ref == General_Language_Translation::e_storage_modifier_jovial_def) {
      sg_var_decl->get_declarationModifier().get_storageModifier().setJovialDef();
   }
   else if (def_or_ref == General_Language_Translation::e_storage_modifier_jovial_ref) {
      sg_var_decl->get_declarationModifier().get_storageModifier().setJovialRef();
   }

   if (is_anon) {
      SgJovialTableStatement* def_decl = isSgJovialTableStatement(sg_block_decl->get_definingDeclaration());
      ROSE_ASSERT(def_decl);

      SageInterface::setBaseTypeDefiningDeclaration(sg_var_decl, def_decl);
   }

   sage_tree_builder.Leave(sg_var_decl);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_BlockBodyPart(ATerm term, SgJovialTableStatement* sg_block_decl, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BlockBodyPart: %s\n", ATwriteToString(term));
#endif

   ATerm t_dirs, t_body_options;

   if (ATmatch(term, "BlockBodyPart(<term>,<term>)", &t_dirs, &t_body_options)) {
      if (traverse_DirectiveList(t_dirs, decl_list)) {
         // MATCHED OrderDirective*
      } else return ATfalse;

      ATermList tail = (ATermList) ATmake("<term>", t_body_options);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_DataDeclaration(head, decl_list)) {
            // MATCHED DataDeclaration
         }
         else if (traverse_OverlayDeclaration(head, decl_list)) {
            // MATCHED OverlayDeclaration
         }
         else if (traverse_NullDeclaration(head, decl_list)) {
            // MATCHED NullDeclaration
         } else return ATfalse;
      }
   }
   else if (traverse_NullDeclaration(term, decl_list)) {
      // MATCHED NullDeclaration
   }
   else if (traverse_DataDeclaration(term, decl_list)) {
      // MATCHED DataDeclaration
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_BlockPreset(ATerm term, SgExprListExp* preset_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BlockPreset: %s\n", ATwriteToString(term));
#endif

   ATerm t_block_preset;

   if (ATmatch(term, "no-block-preset")) {
      // MATCHED no-block-preset
   }
   else if (ATmatch(term, "BlockPreset(<term>)", &t_block_preset)) {
      if (traverse_BlockPresetList(t_block_preset, preset_list)) {
         // MATCHED BlockPresetList
      } else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_BlockPresetList(ATerm term, SgExprListExp* preset_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BlockPresetList: %s\n", ATwriteToString(term));
#endif

   SgUntypedExpression* preset;
   SgExpression* sg_preset_expr;

   ATermList tail = (ATermList) ATmake("<term>", term);
   while (! ATisEmpty(tail)) {
      ATerm head = ATgetFirst(tail);
      tail = ATgetNext(tail);
   // TODO
      ROSE_ASSERT(false);
      if (traverse_PresetValuesOption(head, sg_preset_expr, preset)) {
         // MATCHED PresetValuesOption
      }
      else if (traverse_TablePresetList(head, preset_list)) {
         // MATCHED TablePresetList
      }
      else if (traverse_OptBlockPresetList(head, preset_list)) {
         // MATCHED OptBlockPresetList
      }
      else return ATfalse;
   }

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_OptBlockPresetList(ATerm term, SgExprListExp* preset_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptBlockPresetList: %s\n", ATwriteToString(term));
#endif

   ATerm t_list;

   if (ATmatch(term, "no-block-preset-list")) {
      // MATCHED no-block-preset-list
   }
   else if (ATmatch(term, "OptBlockPresetlist(<term>)", &t_list)) {
      if (traverse_BlockPresetList(t_list, preset_list)) {
         //Grammar:   '(' BlockPresetList ')' -> OptBlockPresetList  {cons("OptBlockPresetlist")}
      } else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.5 ALLOCATION OF DATA OBJECTS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_OptAllocationSpecifier(ATerm term, SgUntypedExprListExpression* attr_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptAllocationSpecifier: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "no-allocation-specifier()")) {
      // MATCHED no-allocation-specifier
   }
   else if (ATmatch(term, "STATIC()")) {
      ROSE_ASSERT(attr_list);
      SgUntypedExpression* attr = new SgUntypedOtherExpression(General_Language_Translation::e_storage_modifier_static);
      setSourcePosition(attr, term);
      attr_list->get_expressions().push_back(attr);
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.6 INITIALIZATION OF DATA OBJECTS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_ItemPreset(ATerm term, SgExpression* &sg_preset, SgUntypedExpression* &preset)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ItemPreset: %s\n", ATwriteToString(term));
#endif

   ATerm t_preset_value;

   sg_preset = NULL;

   if (ATmatch(term, "no-item-preset()")) {
      // MATCHED no-item-preset
   }
   else if (ATmatch(term, "ItemPreset(<term>)", &t_preset_value)) {
      if (traverse_ItemPresetValue(t_preset_value, sg_preset, preset)) {
         // MATCHED ItemPresetValue
         if (!preset) {
            cerr << "WARNING UNIMPLEMENTED: ItemPresetValue \n";
            return ATtrue;
         }
         ROSE_ASSERT(preset);
      } else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_OptItemPresetValue(ATerm term, SgExpression* &sg_preset, SgUntypedExpression* &preset)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptItemPresetValue: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "no-item-preset-value()")) {
      // MATCHED no-item-preset-value
   }
   else if (traverse_ItemPresetValue(term, sg_preset, preset)) {
      // MATCHED ItemPresetValue
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_ItemPresetValue(ATerm term, SgExpression* &sg_preset, SgUntypedExpression* &preset)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ItemPresetValue: %s\n", ATwriteToString(term));
#endif

   // CompileTimeFormula -> ItemPresetValue
   if (traverse_Formula(term, sg_preset, preset)) {
      // MATCHED CompileTimeFormula
   }
   // LocFunction -> ItemPresetValue
   else if (traverse_LocFunction(term, sg_preset, preset)) {
      // MATCHED LocFunction
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_TablePreset(ATerm term, SgExprListExp* &preset_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_TablePreset: %s\n", ATwriteToString(term));
#endif

   ATerm t_preset_list;

   preset_list = nullptr;

   if (ATmatch(term, "no-table-preset()")) {
      // MATCHED no-table-preset
   }
   else if (ATmatch(term, "TablePreset(<term>)", &t_preset_list)) {
   // TODO
      ROSE_ASSERT(false);
      SgExprListExp*  initializer_list = SageBuilder::buildExprListExp_nfi();
   // SgBracedInitializer* initializer = SageBuilder::buildBracedInitializer_nfi(initializer_list, type);

   // setSourcePosition(initializer, term);
      setSourcePosition(initializer_list, t_preset_list);

      if (traverse_TablePresetList(t_preset_list, initializer_list)) {
         // MATCHED TablePresetList
      } else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

#if 0 //DELETE_ME
ATbool ATermToSageJovialTraversal::traverse_TablePreset(ATerm term, SgUntypedExprListExpression* & preset)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_TablePreset: %s\n", ATwriteToString(term));
#endif

   ATerm t_preset_list;
   preset = NULL;

   if (ATmatch(term, "no-table-preset()")) {
      // MATCHED no-table-preset
   }
   else if (ATmatch(term, "TablePreset(<term>)", &t_preset_list)) {
      int expr_enum = General_Language_Translation::e_struct_initializer;
      preset = new SgUntypedExprListExpression(expr_enum);
      ROSE_ASSERT(preset);
      setSourcePosition(preset, t_preset_list);

      if (traverse_TablePresetList(t_preset_list, preset)) {
         // MATCHED TablePresetList
      } else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}
#endif

ATbool ATermToSageJovialTraversal::traverse_TablePresetList(ATerm term, SgExprListExp* preset_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_TablePresetList: %s\n", ATwriteToString(term));
#endif

   ATerm t_default_preset_list, t_spec_preset_list;

   ROSE_ASSERT(preset_list);

   if (traverse_DefaultPresetSublist(term, preset_list)) {
      // MATCHED DefaultPresetSublist
   }
   else if (ATmatch(term, "TablePresetList(<term>,<term>)", &t_default_preset_list, &t_spec_preset_list)) {
//TODO - break into two lists?
      if (traverse_DefaultPresetSublist(t_default_preset_list, preset_list)) {
         // MATCHED DefaultPresetSublist
      } else return ATfalse;

      ATermList tail = (ATermList) ATmake("<term>", t_spec_preset_list);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_SpecifiedPresetSublist(head, preset_list)) {
            // MATCHED SpecifiedPresetSublist
         } else return ATfalse;
      }
      cerr << "WARNING UNIMPLEMENTED: DefaultPresetSublist\n";
      ROSE_ASSERT(false);
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_DefaultPresetSublist(ATerm term, SgExprListExp* preset)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_DefaultPresetSublist: %s\n", ATwriteToString(term));
#endif

   ATerm t_default_preset_list;
   SgUntypedExpression* expr;
   SgExpression* sg_expr = nullptr;

   if (ATmatch(term, "DefaultPresetSublist(<term>)", &t_default_preset_list)) {
   // TODO
      cerr << "WARNING UNIMPLEMENTED: DefaultPresetSublist\n";
      ROSE_ASSERT(false);
      ATermList tail = (ATermList) ATmake("<term>", t_default_preset_list);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_PresetValuesOption(head, sg_expr, expr)) {
            // MATCHED PresetValuesOption: NOTE - this is optional so expr may be NULL
#if 0
            if (expr) {
               preset->get_expressions().push_back(expr);
            }
#endif
         } else return ATfalse;
      }
   }
   else if (ATmatch(term, "no-default-preset-sublist")) {
      // MATCHED no-default-preset-sublist
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_SpecifiedPresetSublist(ATerm term, SgExprListExp* preset)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SpecifiedPresetSublist: %s\n", ATwriteToString(term));
#endif

   ATerm t_preset_index_spec, t_preset_values_option;
   SgUntypedExpression* expr;
   SgExpression* sg_expr;

   if (ATmatch(term, "SpecifiedPresetSublist(<term>,<term>)", &t_preset_index_spec, &t_preset_values_option)) {
   // TODO
      SgInitializer* sg_preset = nullptr;
      cerr << "WARNING UNIMPLEMENTED: SpecifiedPresetSublist\n";
      ROSE_ASSERT(sg_preset);
      if (traverse_PresetIndexSpecifier(t_preset_index_spec, sg_preset)) {
         // MATCHED PresetIndexSpecifier
      } else return ATfalse;

      ATermList tail = (ATermList) ATmake("<term>", t_preset_values_option);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_PresetValuesOption(head, sg_expr, expr)) {
            // MATCHED PresetValuesOption, optional so ok if nullptr
            if (sg_expr != nullptr) {
               preset->get_expressions().push_back(sg_expr);
            }
         } else return ATfalse;
      }
   }

   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_PresetIndexSpecifier(ATerm term, SgInitializer* preset)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_PresetIndexSpecifier: %s\n", ATwriteToString(term));
#endif

   //  'POS' '(' {ConstantIndex ','}+ ')' ':'  -> PresetIndexSpecifier  {cons("PresetIndexSpecifier")}
   //  CompileTimeNumericFormula       -> ConstantIndex
   //  CompileTimeStatusFormula        -> ConstantIndex

   ATerm t_const_index;
   SgUntypedExpression* expr = NULL;

// Begin SageTreeBuilder
   SgExpression* sg_expr = nullptr;

   if (ATmatch(term, "PresetIndexSpecifier(<term>)", &t_const_index)) {
      ATermList tail = (ATermList) ATmake("<term>", t_const_index);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_NumericFormula(head, sg_expr, expr)) {
            // MATCHED CompileTimeNumericFormula
            ROSE_ASSERT(sg_expr);
//DELETE_ME preset->get_expressions().push_back(expr);
         } else if (traverse_StatusFormula(head, sg_expr, expr)) {
            // MATCHED CompileTimeStatusFormula
            ROSE_ASSERT(sg_expr);
//DELETE_ME preset->get_expressions().push_back(expr);
         } else return ATfalse;
      }
      cerr << "WARNING UNIMPLEMENTED: PresetIndexSpecifier\n";
      ROSE_ASSERT(false);
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_PresetValuesOption(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_PresetValuesOption: %s\n", ATwriteToString(term));
#endif

   ATerm t_rep_count, t_item_preset_value;

   if (ATmatch(term, "PresetValuesOption(<term>)", &t_item_preset_value)) {
      if (traverse_OptItemPresetValue(t_item_preset_value, sg_expr, expr)) {
         // MATCHED OptItemPresetValue
      } else return ATfalse;
   }
   else if (ATmatch(term, "PresetValuesOptionRep(<term>,<term>)", &t_rep_count, &t_item_preset_value)) {
      // TODO: Add traversal for RepetitionCount '(' {PresetValuesOption ','}+ ')' -> PresetValuesOption
      cerr << "WARNING UNIMPLEMENTED: PresetValuesOptionRep\n";
      return ATtrue;
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.2 TYPE DECLARATIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_TypeDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_TypeDeclaration: %s\n", ATwriteToString(term));
#endif

   if (traverse_ItemTypeDeclaration(term, decl_list)) {
      // MATCHED ItemTypeDeclaration
   }
   else if (traverse_TableTypeDeclaration(term, decl_list)) {
      // MATCHED TableTypeDeclaration
   }
   else if (traverse_BlockTypeDeclaration(term, decl_list)) {
      // MATCHED BlockTypeDeclaration
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_ItemTypeDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ItemTypeDeclaration: %s\n", ATwriteToString(term));
#endif

   ATerm t_name, t_type_desc;
   std::string name;

   SgUntypedType* declared_type = NULL;
   SgType* sg_declared_type = nullptr;
   SgUntypedExprListExpression* attr_list = NULL;

// For StatusItemDescription
   bool has_size;
   SgUntypedExpression* status_size;
   SgUntypedInitializedNameList* status_list = NULL;

   std::string label = "";

   if (ATmatch(term, "ItemTypeDeclaration(<term>,<term>)", &t_name, &t_type_desc)) {

      if (traverse_Name(t_name, name)) {
         // MATCHED ItemTypeName
      } else return ATfalse;

      if (traverse_ItemTypeDescription(t_type_desc, sg_declared_type, declared_type, attr_list)) {
         // MATCHED ItemTypeDescription

         ROSE_ASSERT(declared_type);

         SgUntypedTypedefDeclaration* type_def_decl = new SgUntypedTypedefDeclaration(name, declared_type);
         ROSE_ASSERT(type_def_decl);
         setSourcePosition(type_def_decl, term);

         decl_list->get_decl_list().push_back(type_def_decl);
      }
      else if (traverse_StatusItemDescription(t_type_desc, status_list, has_size, status_size)) {
         // MATCHED StatusItemDescription

         // status item declarations have to be handled differently than other ItemTypeDescription terms

         // also assume an int is sufficient for status_size for now
         if (has_size == true) {
            // ROSE_ASSERT(has_size == false);
            cerr << "WARNING UNIMPLEMENTED: ItemTypeDeclaration - has_size \n";
         }
         ROSE_ASSERT(status_list);

         SgUntypedEnumDeclaration* enum_decl = new SgUntypedEnumDeclaration(label, name, status_list);
         ROSE_ASSERT(enum_decl);
         setSourcePosition(enum_decl, term);

         decl_list->get_decl_list().push_back(enum_decl);
      }
      else return ATfalse;

   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_TableTypeDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_TableTypeDeclaration: %s\n", ATwriteToString(term));
#endif

   ATerm t_name, t_type_desc;
   std::string type_name;

   SgUntypedStructureDeclaration* struct_decl = NULL;
   SgJovialTableStatement* sg_table_decl = nullptr;

   if (ATmatch(term, "TableTypeDeclaration(<term>,<term>)", &t_name, &t_type_desc)) {
      if (traverse_Name(t_name, type_name)) {
         // MATCHED TableTypeName
      } else return ATfalse;

   // Begin SageTreeBuilder
      Rose::builder::SourcePositionPair sources;
      sage_tree_builder.Enter(sg_table_decl, type_name, sources);

// This portion could be moved to UntypedBuilder::
//--------------------------------------------------
      int struct_type = Jovial_ROSE_Translation::e_table_type_declaration;
      SgUntypedStructureDefinition* struct_def  = NULL;
      SgUntypedExprListExpression*    modifiers = NULL;
      SgUntypedExprListExpression*        shape = NULL;

      struct_def = UntypedBuilder::buildStructureDefinition(type_name, /*has_body*/true, /*scope*/NULL);
      ROSE_ASSERT(struct_def != NULL);
      SageInterface::setSourcePosition(struct_def);

      modifiers = new SgUntypedExprListExpression(General_Language_Translation::e_struct_modifier_list);
      ROSE_ASSERT(modifiers != NULL);
      SageInterface::setSourcePosition(modifiers);

      // There may be a shape if a Jovial table
      shape = new SgUntypedExprListExpression(General_Language_Translation::e_array_shape);
      ROSE_ASSERT(shape);
      SageInterface::setSourcePosition(shape);

      std::string label = "";
      struct_decl = new SgUntypedStructureDeclaration(label, struct_type, type_name, modifiers, shape, struct_def);
      ROSE_ASSERT(struct_decl);
      SageInterface::setSourcePosition(struct_decl);
//--------------------------------------------------

      ROSE_ASSERT(struct_def != NULL);
      setSourcePosition(struct_def, t_type_desc);

      if (traverse_TableTypeSpecifier(t_type_desc, sg_table_decl, struct_decl)) {
         // MATCHED TableTypeSpecifier
      } else return ATfalse;

   }
   else return ATfalse;

   ROSE_ASSERT(struct_decl != NULL);
   setSourcePosition(struct_decl, term);

   decl_list->get_decl_list().push_back(struct_decl);

// End SageTreeBuilder
   sage_tree_builder.Leave(sg_table_decl);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::
traverse_TableTypeSpecifier(ATerm term, SgJovialTableStatement* sg_table_decl, SgUntypedStructureDeclaration* table_decl)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_TableTypeSpecifier: %s\n", ATwriteToString(term));
#endif

// A TableTypeSpecifier looks a base class name or it is a primitive type
//
   SgJovialTableType* sg_parent_type = nullptr;

   ROSE_ASSERT(sg_table_decl);

   ATerm t_dim_list, t_struct_spec, t_like_option, t_entry_spec, t_type_name;
   std::string table_type_name, like_name;

   bool has_table_type_name = false;
   bool has_like_option = false;

   SgUntypedExprListExpression* attr_list = table_decl->get_modifiers();
   ROSE_ASSERT(attr_list);

   SgUntypedStructureDefinition* table_desc = table_decl->get_definition();
   ROSE_ASSERT(table_desc);

// Begin SageTreeBuilder
   SgJovialTableType* sg_table_type = isSgJovialTableType(sg_table_decl->get_type());
   ROSE_ASSERT(sg_table_type);

   SgExprListExp* sg_dim_info = sg_table_type->get_dim_info();

// TableTypeSpecifier with a name
   if (ATmatch(term, "TableTypeSpecifierName(<term>,<term>)", &t_dim_list, &t_type_name)) {

      if (sg_dim_info == nullptr) {
         sg_dim_info = SageBuilder::buildExprListExp_nfi();
      }

      if (traverse_OptDimensionList(t_dim_list, sg_dim_info)) {
         // MATCHED OptDimensionList
      } else return ATfalse;

      if (traverse_Name(t_type_name, table_type_name)) {
         // MATCHED TableTypeName
         has_table_type_name = true;
      } else return ATfalse;

//DONE: cerr << "\nWARNING UNIMPLEMENTED: TableTypeSpecifierName: " << table_type_name << endl;

   // This type should have already been created by a type declaration statement, find it
      SgClassSymbol* class_symbol = SageInterface::lookupClassSymbolInParentScopes(table_type_name, SageBuilder::topScopeStack());
      if (class_symbol != NULL) {
         sg_parent_type = isSgJovialTableType(class_symbol->get_type());
      }
      ROSE_ASSERT(sg_parent_type);

      // 1. The class declaration for the base class should be the first non-defining declaration of the base class
      // 2. The class definition should be the class definition of the derived class
      //
      SgClassDeclaration* base_class_decl = isSgClassDeclaration(sg_parent_type->get_declaration());
      ROSE_ASSERT(base_class_decl);
      
   // DQ (12/26/2011): The non defining declaration should not have a valid pointer to the class definition.
      ROSE_ASSERT(base_class_decl->get_definition() == NULL);

      SgClassDefinition* derived_class_def = sg_table_decl->get_definition();
      ROSE_ASSERT(derived_class_def);

      // The sage builder function sets the parent of the base class to the inherited class
      // definition and adds the base class to its inheritances list.
      SgBaseClass* base_class = SageBuilder::buildBaseClass(base_class_decl, derived_class_def, false, /*isDirect*/true);
      ROSE_ASSERT(base_class);
   }

// TableTypeSpecifier with four arguments where t_entry_spec specifies the type (could be primitive or anonymous)
   else if (ATmatch(term, "TableTypeSpecifier(<term>,<term>,<term>,<term>)",
                          &t_dim_list, &t_struct_spec, &t_like_option, &t_entry_spec)) {

      if (sg_dim_info == nullptr) {
         sg_dim_info = SageBuilder::buildExprListExp_nfi();
      }

      if (traverse_OptDimensionList(t_dim_list, sg_dim_info)) {
         // MATCHED OptDimensionList
      } else return ATfalse;

   // Structure specifier
      if (traverse_OptStructureSpecifier(t_struct_spec, /*FIXME*/NULL)) {
         // MATCHED OptStructureSpecifier
      } else return ATfalse;

   // Like option
      if (ATmatch(t_like_option, "no-like-option()")) {
         // MATCHED no-like-option
      }
      else if (ATmatch(t_like_option, "LikeOption(<term>)", &t_type_name)) {
         if (traverse_Name(t_type_name, table_type_name)) {
            // MATCHED Like option TableTypeName
            has_like_option = true;
            has_table_type_name = true;

         // TODO: like-option (apparently not needed at the moment)
            cerr << "WARNING UNIMPLEMENTED: LikeOption \n";
            return ATtrue;

            ROSE_ASSERT(false);
         } else return ATfalse;
      }
      else return ATfalse;

   // TODO - need to figure out type and preset (maybe preset can be null as it would be set in EntrySpecifierType)
      SgType* sg_base_type = nullptr;
      SgInitializer* sg_preset = nullptr;
      ROSE_ASSERT(sg_preset);

   // Entry specifier with or without a body
      if (traverse_EntrySpecifierType(t_entry_spec, sg_base_type, sg_preset)) {
         // MATCHED EntrySpecifier
      }
      else if (traverse_EntrySpecifierBody(t_entry_spec, sg_table_decl, table_desc)) {
         // MATCHED EntrySpecifierBody
      }
      else return ATfalse;
   }
   else return ATfalse;

   sg_table_type->set_dim_info(sg_dim_info);
   sg_dim_info->set_parent(sg_table_type);

#if 0
   std::cout << "TABLE TYPE SPEC rank is "     << sg_dim_info->get_expressions().size() << endl;
   std::cout << "TABLE TYPE SPEC dim_info: "   << sg_dim_info << endl;
   std::cout << "TABLE TYPE sg_table_type: "   << sg_table_type << ": " << sg_table_type->class_name() << endl;
#endif

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_BlockTypeDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BlockTypeDeclaration: %s\n", ATwriteToString(term));
#endif

   ATerm t_name, t_type_desc;
   std::string type_name;

   SgJovialTableStatement* sg_block_decl = nullptr;

   SgUntypedStructureDeclaration* struct_decl = NULL;

   if (ATmatch(term, "BlockTypeDeclaration(<term>,<term>)", &t_name, &t_type_desc)) {
      if (traverse_Name(t_name, type_name)) {
         // MATCHED BlockTypeName
      } else return ATfalse;

      // Begin SageTreeBuilder
      Rose::builder::SourcePositionPair sources;
      sage_tree_builder.Enter(sg_block_decl, type_name, sources, /*is_block*/ true);

      int struct_type = Jovial_ROSE_Translation::e_block_type_declaration;

// This portion could be moved to UntypedBuilder::
//--------------------------------------------------
      SgUntypedStructureDefinition* struct_def  = NULL;
      SgUntypedExprListExpression*    modifiers = NULL;
      SgUntypedExprListExpression*        shape = NULL;

      struct_def = UntypedBuilder::buildStructureDefinition(type_name, /*has_body*/true, /*scope*/NULL);
      ROSE_ASSERT(struct_def != NULL);
      SageInterface::setSourcePosition(struct_def);

      modifiers = new SgUntypedExprListExpression(General_Language_Translation::e_struct_modifier_list);
      ROSE_ASSERT(modifiers != NULL);
      SageInterface::setSourcePosition(modifiers);

      // There may be a shape if a Jovial table
      shape = new SgUntypedExprListExpression(General_Language_Translation::e_array_shape);
      ROSE_ASSERT(shape);
      SageInterface::setSourcePosition(shape);

      std::string label = "";
      struct_decl = new SgUntypedStructureDeclaration(label, struct_type, type_name, modifiers, shape, struct_def);
      ROSE_ASSERT(struct_decl);
      SageInterface::setSourcePosition(struct_decl);
//--------------------------------------------------

      ROSE_ASSERT(struct_def != NULL);
      setSourcePosition(struct_def, t_type_desc);

      SgUntypedScope* block_scope = struct_def->get_scope();
      ROSE_ASSERT(block_scope != NULL);

      SgUntypedDeclarationStatementList* block_decl_list = block_scope->get_declaration_list();
      ROSE_ASSERT(block_decl_list);

      if (traverse_BlockBodyPart(t_type_desc, sg_block_decl, block_decl_list)) {
         // MATCHED BlockBodyPart
      }
      else if (traverse_DataDeclaration(t_type_desc, block_decl_list)) {
         // MATCHED DataDeclaration -> BlockBodyPart
      }
      else if (traverse_NullDeclaration(t_type_desc, block_decl_list)) {
         // MATCHED NullDeclaration -> BlockBodyPart
      }
      else return ATfalse;

   }
   else return ATfalse;

   ROSE_ASSERT(struct_decl != NULL);
   setSourcePosition(struct_decl, term);

   decl_list->get_decl_list().push_back(struct_decl);

   // End SageTreeBuilder
   sage_tree_builder.Leave(sg_block_decl);

   return ATtrue;
}

//========================================================================================
// 2.3 STATEMENT NAME DECLARATIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_StatementNameDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list, int def_or_ref)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_StatementNameDeclaration: %s\n", ATwriteToString(term));
#endif

   //'LABEL' {StatementName ','}+ ';'  -> StatementNameDeclaration {cons("StatementNameDeclaration")}

   ATerm t_name;
   std::string name;

   if (ATmatch(term, "StatementNameDeclaration(<term>)", &t_name)) {
      cerr << "WARNING UNIMPLEMENTED: StatementNameDeclaration\n";

#if 0
      SgUntypedInitializedNameList* name_list = NULL;
      name_list = new SgUntypedInitializedNameList();
      ROSE_ASSERT(name_list);
      setSourcePosition(name_list, t_name);
#endif

      ATermList tail = (ATermList) ATmake("<term>", t_name);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_Name(head, name)) {
            // MATCHED Name

#if 0
            // type?
            SgUntypedType* type = UntypedBuilder::buildType(SgUntypedType::e_unknown);

            SgUntypedInitializedName* iname = new SgUntypedInitializedName(type, name);
            ROSE_ASSERT(iname);
            setSourcePosition(iname, head);

            // where to push it?
            name_list->get_name_list().push_back(iname);
#endif
         } else return ATfalse;
      }
   } else return ATfalse;

#if 0
   int stmt_enum = General_Language_Translation::e_unknown;
   std::string label_string = "";

   SgUntypedInitializedNameListDeclaration* name_list_decl = new SgUntypedInitializedNameListDeclaration(label_string, stmt_enum, name_list);
   ROSE_ASSERT(name_list_decl);
   setSourcePosition(name_list_decl, term); // correct source position?

   decl_list->get_decl_list().push_back(name_list_decl);
#endif

   return ATtrue;
}

//========================================================================================
// 2.4 DEFINE DECLARATIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_DefineDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_DefineDeclaration: %s\n", ATwriteToString(term));
#endif

   ATerm t_name, t_part;
   std::string name, params, def_string;

   if (ATmatch(term, "DefineDeclaration(<term>,<term>)", &t_name, &t_part)) {

      if (traverse_Name(t_name, name)) {
         // MATCHED DefineName
      } else return ATfalse;

      if (traverse_DefinitionPart(t_part, params, def_string)) {
      } else return ATfalse;

   } else return ATfalse;

   int stmt_enum = General_Language_Translation::e_define_directive_stmt;

   SgUntypedDirectiveDeclaration* define_decl = new SgUntypedDirectiveDeclaration("", stmt_enum, name +" " +params +" " +def_string);
   ROSE_ASSERT(define_decl);
   setSourcePosition(define_decl, term);

   decl_list->get_decl_list().push_back(define_decl);

   SgJovialDefineDeclaration* sg_define_decl = SageBuilder::buildJovialDefineDeclaration_nfi(name, params, def_string);
   setSourcePosition(sg_define_decl, term);

   SageInterface::appendStatement(sg_define_decl, SageBuilder::topScopeStack());

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_DefinitionPart(ATerm term, std::string & params, std::string & def_string)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_DefinitionPart: %s\n", ATwriteToString(term));
#endif

   ATerm t_param_list, t_def_string;

   if (ATmatch(term, "DefinitionPart(<term>,<term>)", &t_param_list, &t_def_string)) {
      if (traverse_FormalDefineParameterList(t_param_list, params)) {
         // MATCHED FormalDefineParameterList
      } else return ATfalse;

      if (traverse_DefineString(t_def_string, def_string)) {
         // MATCHED DefineString
      } else return ATfalse;
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_FormalDefineParameterList(ATerm term, std::string & params)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FormalDefineParameterList: %s\n", ATwriteToString(term));
#endif

   ATerm t_param_list;
   char* letter;

   if (ATmatch(term, "no-formal-define-parameter-list")) {
         // MATCHED no-formal-define-parameter-list
   }
   else if (ATmatch(term, "FormalDefineParameterList(<term>)", &t_param_list)) {
      // There is a FormalDefinParameterList.  For now just reassemble it to be
      // passed on to ROSE to be similar to #define in C.  Assume jovial front-end
      // compiler will handle macro substitution.
      //

      // add initial parens
      params += '(';

      bool first = true;
      ATermList tail = (ATermList) ATmake("<term>", t_param_list);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);

         // add comma separator
         if (first == false) params += ',';

         if (ATmatch(head, "FormalDefineParameter(<str>)", &letter)) {
            // MATCHED Letter
            params += letter;
         } else return ATfalse;
      }
   }
   else return ATfalse;

   // add terminating parens
   if (params.length() > 0) params += ')';

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_DefineString(ATerm term, std::string & def_string)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_DefineString: %s\n", ATwriteToString(term));
#endif

   // Need to remove the "?" at the beginning and end of each string
   char* str;

   if (ATmatch(term, "DefineString(<str>)", &str)) {
      // MATCHED DefineString
      def_string = str;
   } else return ATfalse;

   // To make lexing and parsing possible it was necessary to replace the
   // starting and terminating double quotes '"' with '?'.  Fix this
   // here.
   //
   unsigned int len = def_string.length();
   ROSE_ASSERT(len > 2);
   ROSE_ASSERT(def_string[0] == '?' && def_string[len-1] == '?');

   def_string[0]     = '"';
   def_string[len-1] = '"';

   return ATtrue;
}

//========================================================================================
// 2.5 EXTERNAL DECLARATIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_ExternalDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ExternalDeclaration: %s\n", ATwriteToString(term));
#endif

   if (traverse_SimpleDef(term, decl_list)) {
      // MATCHED SimpleDef -> DefSpecification
   } else if (traverse_CompoundDef(term, decl_list)) {
      // MATCHED CompoundDef -> DefSpecification
   } else if (traverse_SimpleRef(term, decl_list)) {
      // MATCHED SimpleRef -> RefSpecification

      //   } else if (traverse_CompoundRef(term, decl_list)) {
      // MATCHED CompoundRef -> RefSpecification
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.5.1 DEF SPECIFICATIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_SimpleDef(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SimpleDef: %s\n", ATwriteToString(term));
#endif

   ATerm t_simple_def, t_amb, t_def;

   if (ATmatch(term, "SimpleDef(<term>)", &t_simple_def)) {
      // MATCHED SimpleDef

      // SimpleDef can have an ambiguity if an ItemDescription has a type name starting with "a"
      if (ATmatch(t_simple_def, "amb(<term>)", &t_amb)) {
         // MATCHED an ambiguity, choose the first one
         ATermList tail = (ATermList) ATmake("<term>", t_amb);
         t_def = ATgetFirst(tail);
      }
      else {
         t_def = t_simple_def;
      }
   }
   else return ATfalse;

   if (traverse_DefSpecificationChoice(t_def, decl_list)) {
      // MATCHED DefSpecificationChoice
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_CompoundDef(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CompoundDef: %s\n", ATwriteToString(term));
#endif

   ATerm def_spec;

   if (ATmatch(term, "CompoundDef(<term>)" , &def_spec)) {
      ATermList tail = (ATermList) ATmake("<term>", def_spec);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_DefSpecificationChoice(head, decl_list)) {
            // MATCHED DefSpecificationChoice
         } else return ATfalse;
      }
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_DefSpecificationChoice(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_DefSpecificationChoice: %s\n", ATwriteToString(term));
#endif

   // This is an 'DEF' declaration
   int def_spec = General_Language_Translation::e_storage_modifier_jovial_def;

   if (traverse_NullDeclaration(term, decl_list, def_spec)) {
      // MATCHED NullDeclaration
   } else if (traverse_DataDeclaration(term, decl_list, def_spec)) {
      // MATCHED DataDeclaration
   } else if (traverse_StatementNameDeclaration(term, decl_list, def_spec)) {
      // MATCHED StatementNameDeclaration
   } else if (traverse_DefBlockInstantiation(term, decl_list)) {
      // MATCHED DefBlockInstantiation
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_DefBlockInstantiation(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_DefBlockInstantiation: %s\n", ATwriteToString(term));
#endif

   //   'BLOCK' 'INSTANCE'
   //    BlockName ';'                 -> DefBlockInstantiation   {cons("DefBlockInstantiation")}

   ATerm t_name;
   std::string name;

   if (ATmatch(term, "DefBlockInstantiation(<term>)", &t_name)) {
      if (traverse_Name(t_name, name)) {
         // MATCHED Name
      } else return ATfalse;
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.5.2 REF SPECIFICATIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_SimpleRef(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SimpleRef: %s\n", ATwriteToString(term));
#endif

   ATerm t_ref;

   if (ATmatch(term, "SimpleRef(<term>)", &t_ref)) {
      if (traverse_RefSpecificationChoice(t_ref, decl_list)) {
         // MATCHED RefSpecificationChoice
      } else return ATfalse;
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_CompoundRef(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CompoundRef: %s\n", ATwriteToString(term));
#endif

   return ATfalse;
}

ATbool ATermToSageJovialTraversal::traverse_RefSpecificationChoice(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_RefSpecificationChoice: %s\n", ATwriteToString(term));
#endif

   // This is an 'REF' declaration
   int ref_spec = General_Language_Translation::e_storage_modifier_jovial_ref;

   if (traverse_NullDeclaration(term, decl_list, ref_spec)) {
      // MATCHED NullDeclaration
   } else if (traverse_DataDeclaration(term, decl_list, ref_spec)) {
      // MATCHED DataDeclaration
   } else if (traverse_StatementNameDeclaration(term, decl_list, ref_spec)) {
      // MATCHED StatementNameDeclaration
   } else if (traverse_FunctionDeclaration(term, decl_list)) {
      // MATCHED FunctionDeclaration (is a SubroutineDeclaration in grammar)
   } else if (traverse_ProcedureDeclaration(term, decl_list)) {
      // MATCHED ProcedureDeclaration (is a SubroutineDeclaration in grammar)
   } else return ATfalse;


   return ATtrue;
}

//========================================================================================
// 2.6 OVERLAY DECLARATIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_OverlayDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OverlayDeclaration: %s\n", ATwriteToString(term));
#endif

   // 'OVERLAY'
   //   OptAbsoluteAddress
   //   OverlayExpression ';'         -> OverlayDeclaration   {cons("OverlayDeclaration")}

   ATerm t_addr, t_absolute_addr, t_expr;
   //   SgUntypedExpression* addr = NULL;
   SgUntypedExpression *addr, *expr = NULL;
   SgExpression *sg_addr = nullptr;

   if (ATmatch(term, "OverlayDeclaration(<term>,<term>)", &t_addr, &t_expr)) {
      cerr << "WARNING UNIMPLEMENTED: OverlayDeclaration\n";
      if (ATmatch(t_addr, "AbsoluteAddress(<term>)", &t_absolute_addr)) {
         // 'POS' '(' OverlayAddress ')'    -> AbsoluteAddress      {cons("AbsoluteAddress")}
         if (traverse_NumericFormula(t_absolute_addr, sg_addr, addr)) {
            // MATCHED OverlayAddress
            // CompileTimeNumericFormula       -> OverlayAddress
         } else return ATfalse;
      }
      else if (ATmatch(t_addr, "no-absolute-address")) {
         // MATCHED no-absolute-address
      }
      else return ATfalse;

      if (traverse_OverlayExpression(t_expr, expr)) {
         // MATCHED OverlayExpression
      } else return ATfalse;

   }
   else return ATfalse;

   return ATtrue;

}

ATbool ATermToSageJovialTraversal::traverse_OverlayExpression(ATerm term, SgUntypedExpression* & expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OverlayExpression: %s\n", ATwriteToString(term));
#endif

   //   {OverlayString ':'}+            -> OverlayExpression

   ATermList tail = (ATermList) ATmake("<term>", term);
   while (! ATisEmpty(tail)) {
      ATerm head = ATgetFirst(tail);
      tail = ATgetNext(tail);
      if (traverse_OverlayString(head, expr)) {
         // MATCHED OverlayString
      } else return ATfalse;
   }

   return ATtrue;

}

ATbool ATermToSageJovialTraversal::traverse_OverlayString(ATerm term, SgUntypedExpression* & expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OverlayString: %s\n", ATwriteToString(term));
#endif

   //  {OverlayElement ','}+           -> OverlayString

   ATermList tail = (ATermList) ATmake("<term>", term);
   while (! ATisEmpty(tail)) {
      ATerm head = ATgetFirst(tail);
      tail = ATgetNext(tail);
      if (traverse_OverlayElement(head, expr)) {
         // MATCHED OverlayElement
      } else return ATfalse;
   }

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_OverlayElement(ATerm term, SgUntypedExpression* & expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OverlayElement: %s\n", ATwriteToString(term));
#endif

   //  Spacer                          -> OverlayElement
   //  DataName                        -> OverlayElement
   //  '(' OverlayExpression ')'       -> OverlayElement       {cons("OverlayElement")}

   ATerm t_expr;
   std::string name;
   SgUntypedExpression *spacer; // *overlay_expr;

   if (ATmatch(term, "OverlayElement(<term>)", &t_expr)) {
      if (traverse_OverlayExpression(t_expr, expr)) {
         // MATCHED OverlayExpression
      } else return ATfalse;
   }
   else if (traverse_Spacer(term, spacer)) {
      // MATCHED Spacer
   }
   else if (traverse_Name(term, name)) {
      // MATCHED DataName
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_Spacer(ATerm term, SgUntypedExpression* & expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Spacer: %s\n", ATwriteToString(term));
#endif

   //   'W' CompileTimeNumericFormula   -> Spacer               {cons("Spacer")}

   ATerm t_num;
   SgUntypedExpression* num;
   // CHECK need to change function signature for this one?
   SgExpression* sg_num = nullptr;

   if (ATmatch(term, "Spacer(<term>)", &t_num)) {
      if (traverse_NumericFormula(t_num, sg_num, num)) {
         // MATCHED NumericFormula
      } else return ATfalse;
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 3.1 PROCEDURES
//----------------------------------------------------------------------------------------

ATbool ATermToSageJovialTraversal::traverse_ProcedureDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ProcedureDeclaration: %s\n", ATwriteToString(term));
#endif

   ATerm t_proc_heading, t_decl;

   std::string label, name;
   SgUntypedFunctionScope* function_scope = NULL;
   SgUntypedExprListExpression* modifiers = NULL;
   SgUntypedInitializedNameList* param_list = NULL;
   SgUntypedFunctionDeclaration* function_decl = NULL;

// "body" portion of the procedure declaration so that we can pick up parameter declaration
   SgUntypedDeclarationStatementList* param_decl_list = NULL;
   SgUntypedStatementList*            stmt_list = NULL;
   SgUntypedFunctionDeclarationList*  func_list = NULL;

   if (ATmatch(term, "ProcedureDeclaration(<term>,<term>)", &t_proc_heading, &t_decl)) {

      if (traverse_ProcedureHeading(t_proc_heading, name, modifiers, param_list)) {
         // MATCHED ProcedureHeading
      } else return ATfalse;

      param_decl_list = new SgUntypedDeclarationStatementList();
      ROSE_ASSERT(param_decl_list);
      setSourcePosition(param_decl_list, t_decl);

      if (traverse_Declaration(t_decl, param_decl_list)) {
         // MATCHED Declaration
      } else return ATfalse;

   }
   else return ATfalse;

   stmt_list = new SgUntypedStatementList();
   ROSE_ASSERT(stmt_list);
   setSourcePositionUnknown(stmt_list);

   func_list = new SgUntypedFunctionDeclarationList();
   ROSE_ASSERT(func_list);
   setSourcePositionUnknown(func_list);

   function_scope = UntypedBuilder::buildScope<SgUntypedFunctionScope>(label);
   ROSE_ASSERT(function_scope);
   setSourcePosition(function_scope, t_decl);

   int stmt_enum = General_Language_Translation::e_end_proc_ref_stmt;
   SgUntypedNamedStatement* end_proc_stmt = new SgUntypedNamedStatement(label, stmt_enum, "");
   ROSE_ASSERT(end_proc_stmt);
   setSourcePositionUnknown(end_proc_stmt);

// void type OK here because is not a function declaration
   SgUntypedType* type = UntypedBuilder::buildType(SgUntypedType::e_void);
   ROSE_ASSERT(type);

   ROSE_ASSERT(modifiers);

// create the function definition
   function_decl = new SgUntypedFunctionDeclaration(label, name, param_list, type,
                                                    function_scope, modifiers, end_proc_stmt);
   ROSE_ASSERT(function_decl);
   setSourcePosition(function_decl, term);

   decl_list->get_decl_list().push_back(function_decl);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_ProcedureDefinition(ATerm term, SgUntypedFunctionDeclarationList* func_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ProcedureDefinition: %s\n", ATwriteToString(term));
#endif

   ATerm t_proc_heading, t_proc_body;

   std::string label, name;
   SgUntypedFunctionScope* function_scope = NULL;
   SgUntypedExprListExpression* modifiers = NULL;
   SgUntypedInitializedNameList* param_list = NULL;
   SgUntypedFunctionDeclaration* function_decl;

   if (ATmatch(term, "ProcedureDefinition(<term>,<term>)", &t_proc_heading, &t_proc_body)) {

      if (traverse_ProcedureHeading(t_proc_heading, name, modifiers, param_list)) {
         // MATCHED ProcedureHeading
      } else return ATfalse;

      if (traverse_SubroutineBody(t_proc_body, function_scope)) {
         // MATCHED ProcedureBody (the production is actually a SubroutineBody)
      } else return ATfalse;

   }
   else return ATfalse;

   int stmt_enum = General_Language_Translation::e_end_proc_def_stmt;
   SgUntypedNamedStatement* end_proc_stmt = new SgUntypedNamedStatement(label, stmt_enum, "");
   ROSE_ASSERT(end_proc_stmt);
   setSourcePositionUnknown(end_proc_stmt);

// void type OK here because is not a function definition
   SgUntypedType* type = UntypedBuilder::buildType(SgUntypedType::e_void);
   ROSE_ASSERT(type);

   ROSE_ASSERT(modifiers);

// create the function definition
   function_decl = new SgUntypedFunctionDeclaration(label, name, param_list, type,
                                                    function_scope, modifiers, end_proc_stmt);
   ROSE_ASSERT(function_decl);
   setSourcePosition(function_decl, term);

   func_list->get_func_list().push_back(function_decl);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_ProcedureHeading(ATerm term, std::string & name,
                                                                SgUntypedExprListExpression* & attrs, SgUntypedInitializedNameList* & params)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ProcedureHeading: %s\n", ATwriteToString(term));
#endif

   using namespace General_Language_Translation;

   ATerm t_name, t_attr, t_params;
   SgUntypedExprListExpression* function_modifier_list;
   SgUntypedInitializedNameList* function_param_list;
   SgUntypedOtherExpression* function_modifier = NULL;

   attrs  = NULL;
   params = NULL;

   if (ATmatch(term, "ProcedureHeading(<term>,<term>,<term>)", &t_name, &t_attr, &t_params)) {

      if (traverse_Name(t_name, name)) {
         // MATCHED Name
      } else return ATfalse;

      if (traverse_SubroutineAttribute(t_attr, function_modifier)) {
         // MATCHED SubroutineAttribute
      } else return ATfalse;

      function_modifier_list = new SgUntypedExprListExpression(e_function_modifier_list);
      ROSE_ASSERT(function_modifier_list);
      setSourcePosition(function_modifier_list, t_attr);

      if (function_modifier != NULL) {
         function_modifier_list->get_expressions().push_back(function_modifier);
      }

      function_param_list = new SgUntypedInitializedNameList();
      ROSE_ASSERT(function_param_list);
      setSourcePosition(function_param_list, t_params);

      if (traverse_FormalParameterList(t_params, function_param_list)) {
         // MATCHED FormalParameterList
      } else return ATfalse;

   } else return ATfalse;


   if (function_modifier_list->get_expressions().size() != 0) {
      cerr << "WARNING UNIMPLEMENTED: ProcedureHeading - with function modifiers\n";
      //      return ATtrue;
   }
// not handling function modifiers for now
//   ROSE_ASSERT(function_modifier_list->get_expressions().size() == 0);

   attrs  = function_modifier_list;
   params = function_param_list;

   ROSE_ASSERT(attrs);
   ROSE_ASSERT(params);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_SubroutineAttribute(ATerm term, SgUntypedOtherExpression* & attr_expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SubroutineAttribute: %s\n", ATwriteToString(term));
#endif

   using namespace General_Language_Translation;

   attr_expr = NULL;

   if (ATmatch(term, "no-subroutine-attribute()")) {
      attr_expr = NULL;
      return ATtrue;
   }

   // note that this function will return NULL if there is no attribute (see above)
   //

   if (ATmatch(term, "REC()")) {
      attr_expr = new SgUntypedOtherExpression(e_function_modifier_recursive);
   }
   else if (ATmatch(term, "RENT()")) {
      attr_expr = new SgUntypedOtherExpression(e_function_modifier_reentrant);
   }
   else return ATfalse;

   ROSE_ASSERT(attr_expr);
   setSourcePosition(attr_expr, term);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_SubroutineBody(ATerm term, SgUntypedFunctionScope* & function_scope)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SubroutineBody: %s\n", ATwriteToString(term));
#endif

   ATerm t_stmt;
   ATerm t_decls, t_stmts, t_funcs, t_labels;
   std::vector<std::string> labels;
   std::vector<PosInfo> locations;
   std::string temp_label = "";

   SgUntypedDeclarationStatementList* decl_list = NULL;
   SgUntypedStatementList*            stmt_list = NULL;
   SgUntypedFunctionDeclarationList*  func_list = NULL;

   function_scope = NULL;

   if (ATmatch(term, "SubroutineSimpleBody(<term>)", &t_stmt)) {

      function_scope = UntypedBuilder::buildScope<SgUntypedFunctionScope>(temp_label);
      ROSE_ASSERT(function_scope);
      setSourcePosition(function_scope, term);

      stmt_list = function_scope->get_statement_list();
      ROSE_ASSERT(stmt_list);

      if (traverse_Statement(t_stmt, stmt_list)) {
         // MATCHED Statement
      } else return ATfalse;
   }

   else if (ATmatch(term, "SubroutineBody(<term>,<term>,<term>,<term>)", &t_decls,&t_stmts,&t_funcs,&t_labels)) {

      function_scope = UntypedBuilder::buildScope<SgUntypedFunctionScope>(temp_label);
      ROSE_ASSERT(function_scope);
      setSourcePosition(function_scope, term);

      decl_list = function_scope->get_declaration_list();
      ROSE_ASSERT(decl_list);

      if (traverse_DeclarationList(t_decls, decl_list)) {
         // MATCHED DeclarationList
      } else return ATfalse;

      stmt_list = function_scope->get_statement_list();
      ROSE_ASSERT(stmt_list);

      if (traverse_StatementList(t_stmts, stmt_list)) {
         // MATCHED StatementList
      } else return ATfalse;

      func_list = function_scope->get_function_list();
      ROSE_ASSERT(func_list);

      if (traverse_SubroutineDefinitionList(t_funcs, func_list)) {
         // MATCHED SubroutineDefinitionList
      } else return ATfalse;

      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;

#if 0
      std::cout << "SUBROUTINE BODY\n";
      std::cout << "  # decls = " << decl_list->get_decl_list().size() << "\n";
      std::cout << "  # stmts = " << stmt_list->get_stmt_list().size() << "\n";
      std::cout << "  # funcs = " << func_list->get_func_list().size() << "\n";
      std::cout << "  #labels = " << labels.size() << "\n\n";
#endif

   // TODO - need list for labels in untyped IR
   //        can labels be on procedure definitions?
      assert(labels.size() <= 1);
      if (labels.size() == 1) temp_label = labels[0];
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 3.2 FUNCTIONS
//----------------------------------------------------------------------------------------

ATbool ATermToSageJovialTraversal::traverse_FunctionDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FunctionDeclaration: %s\n", ATwriteToString(term));
#endif

   ATerm t_func_heading, t_dirs, t_decl;

   std::string label, name;
   SgUntypedType* function_type = NULL;
   SgUntypedFunctionScope* function_scope = NULL;
   SgUntypedExprListExpression* modifiers = NULL;
   SgUntypedInitializedNameList* param_list = NULL;
   SgUntypedFunctionDeclaration* function_decl = NULL;

// "body" portion of the procedure declaration so that we can pick up parameter declaration
   SgUntypedDeclarationStatementList* param_decl_list = NULL;

   if (ATmatch(term, "FunctionDeclaration(<term>,<term>,<term>)", &t_func_heading, &t_dirs, &t_decl)) {

      if (traverse_FunctionHeading(t_func_heading, name, function_type, modifiers, param_list)) {
         // MATCHED FunctionHeading
      } else return ATfalse;

      param_decl_list = new SgUntypedDeclarationStatementList();
      ROSE_ASSERT(param_decl_list);
      setSourcePosition(param_decl_list, t_decl);

      if (traverse_DirectiveList(t_dirs, param_decl_list)) {
         // MATCHED ReducibleDirective*
         cerr << "WARNING UNIMPLEMENTED: ReducibleDirective* in FunctionDeclaration\n";
      } else return ATfalse;

      if (traverse_Declaration(t_decl, param_decl_list)) {
         // MATCHED Declaration
      } else return ATfalse;

   }
   else return ATfalse;

   function_scope = UntypedBuilder::buildScope<SgUntypedFunctionScope>(label);
   ROSE_ASSERT(function_scope);
   setSourcePosition(function_scope, t_decl);

   int stmt_enum = General_Language_Translation::e_end_proc_ref_stmt;
   SgUntypedNamedStatement* end_proc_stmt = new SgUntypedNamedStatement(label, stmt_enum, "");
   ROSE_ASSERT(end_proc_stmt);
   setSourcePositionUnknown(end_proc_stmt);

   ROSE_ASSERT(function_type);

   ROSE_ASSERT(modifiers);

// create the function definition
   function_decl = new SgUntypedFunctionDeclaration(label, name, param_list, function_type,
                                                    function_scope, modifiers, end_proc_stmt);
   ROSE_ASSERT(function_decl);
   setSourcePosition(function_decl, term);

   decl_list->get_decl_list().push_back(function_decl);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_FunctionDefinition(ATerm term, SgUntypedFunctionDeclarationList* func_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FunctionDefinition: %s\n", ATwriteToString(term));
#endif

   ATerm t_func_heading, t_dirs, t_proc_body;

   std::string label, name;
   SgUntypedType* function_type = NULL;
   SgUntypedFunctionScope* function_scope = NULL;
   SgUntypedExprListExpression* modifiers = NULL;
   SgUntypedInitializedNameList* param_list = NULL;

   if (ATmatch(term, "FunctionDefinition(<term>,<term>,<term>)", &t_func_heading, &t_dirs, &t_proc_body)) {
      cerr << "WARNING UNIMPLEMENTED: FunctionDefinition\n";

      if (traverse_FunctionHeading(t_func_heading, name, function_type, modifiers, param_list)) {
         // MATCHED FunctionHeading
      } else return ATfalse;

      // OUT OF ORDER to get function scope
      // fix this

      if (traverse_SubroutineBody(t_proc_body, function_scope)) {
         // MATCHED FunctionBody
      } else return ATfalse;

      SgUntypedDeclarationStatementList* dir_list = function_scope->get_declaration_list();

      if (traverse_DirectiveList(t_dirs, dir_list)) {
         // MATCHED ReducibleDirective*
      } else return ATfalse;

   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_FunctionHeading(ATerm term, std::string & name, SgUntypedType* & type,
                                                               SgUntypedExprListExpression* & attrs, SgUntypedInitializedNameList* & params)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FunctionHeading: %s\n", ATwriteToString(term));
#endif

   using namespace General_Language_Translation;

   ATerm t_name, t_type, t_attr, t_params;
   SgUntypedExprListExpression* function_modifier_list;
   SgUntypedInitializedNameList* function_param_list;
   SgUntypedOtherExpression* function_modifier = NULL;

   attrs  = NULL;
   params = NULL;

// For StatusItemDescription
   bool has_size;
   SgUntypedExpression* status_size;
   SgUntypedInitializedNameList* status_list = NULL;

   SgType* sg_type = nullptr;

   std::string label = "";

   if (ATmatch(term, "FunctionHeading(<term>,<term>,<term>,<term>)", &t_name, &t_attr, &t_params, &t_type)) {

      if (traverse_Name(t_name, name)) {
         // MATCHED Name
      } else return ATfalse;

      if (traverse_SubroutineAttribute(t_attr, function_modifier)) {
         // MATCHED SubroutineAttribute
      } else return ATfalse;

      function_modifier_list = new SgUntypedExprListExpression(e_function_modifier_list);
      ROSE_ASSERT(function_modifier_list);
      setSourcePosition(function_modifier_list, t_attr);

      if (function_modifier != NULL) {
         function_modifier_list->get_expressions().push_back(function_modifier);
      }

      function_param_list = new SgUntypedInitializedNameList();
      ROSE_ASSERT(function_param_list);
      setSourcePosition(function_param_list, t_params);

      if (traverse_FormalParameterList(t_params, function_param_list)) {
         // MATCHED FormalParameterList
      } else return ATfalse;

   // function type attributes (ItemType) are added to the function_modifier_list and will have to be sorted out later
      if (traverse_ItemTypeDescription(t_type, sg_type, type, function_modifier_list)) {
         // MATCHED ItemTypeDescription
      }
      else if (traverse_StatusItemDescription(t_type, status_list, has_size, status_size)) {
         // MATCHED StatusItemDescription

         // status item declarations have to be handled differently than other ItemTypeDescription terms

         // also assume an int is sufficient for status_size for now

         cerr << "WARNING UNIMPLEMENTED: FunctionHeading - StatusItemDescription\n";
#if 0
         ROSE_ASSERT(has_size == false);
         ROSE_ASSERT(status_list);

         SgUntypedEnumDeclaration* enum_decl = new SgUntypedEnumDeclaration(label, name, status_list);
         ROSE_ASSERT(enum_decl);
         setSourcePosition(enum_decl, term);

         // need a SgUntypedDeclarationStatementList to push the enum_decl onto
         //         decl_list->get_decl_list().push_back(enum_decl);
#endif
     } else return ATfalse;

   } else return ATfalse;

   if (function_modifier_list->get_expressions().size() != 0) {
      cerr << "WARNING UNIMPLEMENTED: ProcedureHeading - with function modifiers\n";
      //      return ATtrue;
   }
// not handling function modifiers for now
//   ROSE_ASSERT(function_modifier_list->get_expressions().size() == 0);

   attrs  = function_modifier_list;
   params = function_param_list;

   ROSE_ASSERT(attrs);
   ROSE_ASSERT(params);

   return ATtrue;
}

//========================================================================================
// 3.3 PARAMETERS OF PROCEDURES AND FUNCTIONS
//----------------------------------------------------------------------------------------

ATbool ATermToSageJovialTraversal::traverse_FormalParameterList(ATerm term, SgUntypedInitializedNameList* param_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FormalParameterList: %s\n", ATwriteToString(term));
#endif

   ATerm t_input, t_output;

   if (ATmatch(term, "no-formal-parameter-list()")) {
      // no formal parameters OK, return immediately
      return ATtrue;
   }

// There is a formal parameter list (if not will have returned)
//
   if (ATmatch(term, "FormalParameterList(<term>,<term>)", &t_input,&t_output)) {
   // Input parameters
      ATermList tail = (ATermList) ATmake("<term>", t_input);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_FormalInputParameter(head, param_list)) {
            // MATCHED FormalInputParameter
         } else return ATfalse;
      }

   // Output parameters
      if (traverse_FormalOutputParameters(t_output, param_list)) {
         // MATCHED FormalOutputParameter
      } else return ATfalse;

   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_FormalOutputParameters(ATerm term, SgUntypedInitializedNameList* param_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FormalOutputParameters: %s\n", ATwriteToString(term));
#endif

   ATerm t_output;

   if (ATmatch(term, "no-formal-output-parameters()")) {
      // no formal output parameters OK, return immediately
      return ATtrue;
   }

// There is a formal output parameter list (if not will have returned)
//
   if (ATmatch(term, "FormalOutputParameterList(<term>)", &t_output)) {

   // Output parameters
      ATermList tail = (ATermList) ATmake("<term>", t_output);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_FormalOutputParameter(head, param_list)) {
            // MATCHED FormalOutputParameter
         } else return ATfalse;
      }
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_FormalInputParameter(ATerm term, SgUntypedInitializedNameList* param_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FormalInputParameter: %s\n", ATwriteToString(term));
#endif

   ATerm t_binding, t_name;
   std::string name;
   SgUntypedOtherExpression* binding_expr = NULL;

   if (ATmatch(term, "no-formal-parameter-list()")) {
      // no input parameters OK, return immediately
      return ATtrue;
   }

// There are input parameters (if not will have returned)
//
   if (ATmatch(term, "FormalInputParameter(<term>,<term>)", &t_binding, &t_name)) {
      if (traverse_ParameterBinding(t_binding, binding_expr)) {
         // MATCHED ParameterBinding
      } else return ATfalse;
      if (traverse_Name(t_name, name)) {
         // MATCHED Name
      } else return ATfalse;
   }
   else return ATfalse;

   SgUntypedType* type = UntypedBuilder::buildType(SgUntypedType::e_unknown);
   ROSE_ASSERT(type);

   SgUntypedExprListExpression* modifier_list = type->get_modifiers();
   ROSE_ASSERT(modifier_list);
   setSourcePosition(modifier_list, t_binding);

   modifier_list->get_expressions().push_back(binding_expr);

   SgUntypedInitializedName* iname = new SgUntypedInitializedName(type, name);
   ROSE_ASSERT(iname);
   setSourcePosition(iname, t_name);

   param_list->get_name_list().push_back(iname);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_FormalOutputParameter(ATerm term, SgUntypedInitializedNameList* param_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FormalOutputParameter: %s\n", ATwriteToString(term));
#endif

   using namespace General_Language_Translation;

   ATerm t_binding, t_name;
   std::string name;
   SgUntypedOtherExpression* binding_expr = NULL;

   if (ATmatch(term, "no-formal-output-parameters()")) {
      // no output parameters OK, return immediately
      return ATtrue;
   }

// There are output parameters (if not will have returned)
//
   if (ATmatch(term, "FormalOutputParameter(<term>,<term>)", &t_binding, &t_name)) {
      if (traverse_ParameterBinding(t_binding, binding_expr)) {
         // MATCHED ParameterBinding
      } else return ATfalse;
      if (traverse_Name(t_name, name)) {
         // MATCHED Name
      } else return ATfalse;
   }
   else return ATfalse;

   SgUntypedType* type = UntypedBuilder::buildType(SgUntypedType::e_unknown);
   ROSE_ASSERT(type);

   SgUntypedOtherExpression* output_expr = new SgUntypedOtherExpression(e_type_modifier_intent_out);
   ROSE_ASSERT(type);
   setSourcePositionUnknown(output_expr);

   ROSE_ASSERT(output_expr);
   ROSE_ASSERT(binding_expr);

   SgUntypedExprListExpression* modifier_list = type->get_modifiers();
   ROSE_ASSERT(modifier_list);
   setSourcePosition(modifier_list, t_binding);

   modifier_list->get_expressions().push_back(output_expr);
   modifier_list->get_expressions().push_back(binding_expr);

   SgUntypedInitializedName* iname = new SgUntypedInitializedName(type, name);
   ROSE_ASSERT(iname);
   setSourcePosition(iname, t_name);

   param_list->get_name_list().push_back(iname);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_ParameterBinding(ATerm term, SgUntypedOtherExpression* & binding_expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ParameterBinding: %s\n", ATwriteToString(term));
#endif

   using namespace General_Language_Translation;

   binding_expr = NULL;

   if (ATmatch(term, "no-parameter-binding()")) {
      binding_expr = new SgUntypedOtherExpression(e_unknown);
   }
   else if (ATmatch(term, "BYVAL()")) {
      binding_expr = new SgUntypedOtherExpression(e_type_modifier_value);
   }
   else if (ATmatch(term, "BYREF()")) {
      binding_expr = new SgUntypedOtherExpression(e_type_modifier_reference);
   }
   else if (ATmatch(term, "BYRES()")) {
      binding_expr = new SgUntypedOtherExpression(e_type_modifier_result);
   }

   ROSE_ASSERT(binding_expr);
   setSourcePosition(binding_expr, term);

   return ATtrue;
}

//========================================================================================
// 3.4 INLINE DECLARATIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_InlineDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_InlineDeclaration: %s\n", ATwriteToString(term));
#endif

//  'INLINE'
//    {SubroutineName ','}+ ';'     -> InlineDeclaration        {cons("InlineDeclaration")}

   ATerm t_subroutine_name;
   std::string subroutine_name;

   if (ATmatch(term, "InlineDeclaration(<term>)", &t_subroutine_name)) {
      cerr << "WARNING UNIMPLEMENTED: InlineDeclaration\n";
      ATermList tail = (ATermList) ATmake("<term>", t_subroutine_name);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_Name(head, subroutine_name)) {
            // MATCHED SubroutineName
         } else return ATfalse;
      }
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 4.0 STATEMENTS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_Statement(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Statement: %s\n", ATwriteToString(term));
#endif

   if (traverse_SimpleStatement(term, stmt_list)) {
      // MATCHED SimpleStatement
   } else if (traverse_CompoundStatement(term, stmt_list)) {
      // MATCHED CompoundStatement
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_StatementList(ATerm term, SgUntypedStatementList* stmt_list)
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

ATbool ATermToSageJovialTraversal::traverse_SimpleStatement(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SimpleStatement: %s\n", ATwriteToString(term));
#endif

   ATerm t_labels, t_stmt, t_amb;
   std::vector<std::string> labels;
   std::vector<PosInfo> locations;

   if (ATmatch(term, "SimpleStatement(<term>,<term>)", &t_labels, &t_stmt)) {
      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;

      if (traverse_AssignmentStatement(t_stmt, labels, stmt_list)) {
         // MATCHED AssignmentStatement
      }

      else if (traverse_NullStatement(t_stmt, stmt_list)) {
         // MATCHED NullStatement
      }
      else if (traverse_NullBlockStatement(t_stmt, stmt_list)) {
         // MATCHED NullStatement
      }
      else return ATfalse;
   }

// This subsumes the labels in statements, eventually all SimpleStatements will take this path
   else if (ATmatch(term, "SimpleStatement(<term>)", &t_stmt)) {
      if (traverse_IfStatement(t_stmt, stmt_list)) {
         // MATCHED IfStatement
      }
      else if (traverse_AbortStatement(t_stmt, stmt_list)) {
         // MATCHED AbortStatement
      }
      else if (traverse_StopStatement(t_stmt, stmt_list)) {
         // MATCHED StopStatement
      }
      else if (traverse_ExitStatement(t_stmt, stmt_list)) {
         // MATCHED ExitStatement
      }
      else if (traverse_GotoStatement(t_stmt, stmt_list)) {
         // MATCHED GotoStatement
      }
      else if (traverse_ReturnStatement(t_stmt, stmt_list)) {
         // MATCHED ReturnStatement
      }
      else if (traverse_CaseStatement(t_stmt, stmt_list)) {
         // MATCHED CaseStatement
      }
      else if (traverse_WhileStatement(t_stmt, stmt_list)) {
         // MATCHED WhileStatement
      }
      else if (traverse_ForStatement(t_stmt, stmt_list)) {
         // MATCHED ForStatement
      }
      else if (traverse_ProcedureCallStatement(t_stmt, stmt_list)) {
         // MATCHED ProcedureCallStatement
      } else if (ATmatch(t_stmt, "amb(<term>)", &t_amb)) {
         // MATCHED amb
         ATermList tail = (ATermList) ATmake("<term>", t_amb);
         ATerm head = ATgetFirst(tail);
         // chose first amb path, now traverse it

         if (traverse_IfStatement(head, stmt_list)) {
            // MATCHED IfStatement
         }
         else if (traverse_AbortStatement(head, stmt_list)) {
            // MATCHED AbortStatement
         }
         else if (traverse_StopStatement(head, stmt_list)) {
            // MATCHED StopStatement
         }
         else if (traverse_ExitStatement(head, stmt_list)) {
            // MATCHED ExitStatement
         }
         else if (traverse_GotoStatement(head, stmt_list)) {
            // MATCHED GotoStatement
         }
         else if (traverse_ReturnStatement(head, stmt_list)) {
            // MATCHED ReturnStatement
         }
         else if (traverse_CaseStatement(head, stmt_list)) {
            // MATCHED CaseStatement
         }
         else if (traverse_WhileStatement(head, stmt_list)) {
            // MATCHED WhileStatement
         }
         else if (traverse_ForStatement(head, stmt_list)) {
            // MATCHED ForStatement
         }
         else if (traverse_ProcedureCallStatement(head, stmt_list)) {
            // MATCHED ProcedureCallStatement
         } else return ATfalse;
      }
      else return ATfalse;
   }

   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_CompoundStatement(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CompoundStatement: %s\n", ATwriteToString(term));
#endif

   ATerm t_labels, t_stmt, t_labels2;
   std::vector<std::string> labels, labels2;
   std::vector<PosInfo> locations, locations2;
   SgUntypedBlockStatement* block_stmt = NULL;
   SgUntypedStatementList* new_stmt_list = NULL;

   if (ATmatch(term, "CompoundStatement(<term>,<term>,<term>)", &t_labels,&t_stmt,&t_labels2)) {
      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;

      block_stmt = SageBuilder::buildUntypedBlockStatement("");
      ROSE_ASSERT(block_stmt);

      new_stmt_list = block_stmt->get_scope()->get_statement_list();

      if (traverse_StatementList(t_stmt, new_stmt_list)) {
         // MATCHED StatementList
      } else return ATfalse;

      if (traverse_LabelList(t_labels2, labels2, locations2)) {
         // MATCHED LabelList
      } else return ATfalse;

   }
   else return ATfalse;

   stmt_list->get_stmt_list().push_back(block_stmt);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_NullStatement(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_NullStatement: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "NullStatement()")) {
      SgUntypedNullStatement* null_stmt = new SgUntypedNullStatement("");
      setSourcePosition(null_stmt, term);
      stmt_list->get_stmt_list().push_back(null_stmt);
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_NullBlockStatement(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_NullBlockStatement: %s\n", ATwriteToString(term));
#endif

   ATerm t_labels;
   std::vector<std::string> labels;
   std::vector<PosInfo> locations;
   SgUntypedStatement* stmt;

   if (ATmatch(term, "NullBlockStatement(<term>)", &t_labels)) {
      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;

      SgUntypedNullStatement* null_block_stmt = new SgUntypedNullStatement("");
      setSourcePosition(null_block_stmt, term);

      stmt = convert_Labels(labels, locations, null_block_stmt);
   }
   else return ATfalse;

   stmt_list->get_stmt_list().push_back(stmt);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_LabelList(ATerm term, std::vector<std::string> & labels, std::vector<PosInfo> & locations)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_LabelList: %s\n", ATwriteToString(term));
#endif

   ATerm t_labels;
   char * label;

   if (ATmatch(term, "LabelList(<term>)" , &t_labels)) {
      ATermList tail = (ATermList) ATmake("<term>", t_labels);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (ATmatch(head, "Label(<str>)", &label)) {
            labels.push_back(label);
            locations.push_back(getLocation(head));
         } else return ATfalse;
      }
   } else return ATfalse;

   if (labels.size() > 1) {
      cerr << "WARNING UNIMPLEMENTED: LabelList - with multiple labels\n";
   }

   return ATtrue;
}

//========================================================================================
// 4.1 ASSIGNMENT STATEMENTS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_AssignmentStatement(ATerm term, std::vector<std::string> & labels, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_AssignmentStatement: %s\n", ATwriteToString(term));
#endif

   ATerm t_vars, t_expr;
   std::string temp_label = "";
   std::vector<SgUntypedExpression*> vars;
   SgUntypedExpression * expr = NULL;

// Begin SageTreeBuilder
   SgExpression* sg_expr = nullptr;
   std::vector<SgExpression*> sg_vars;

   if (ATmatch(term, "AssignmentStatement(<term>,<term>)", &t_vars,&t_expr)) {

      if (traverse_VariableList(t_vars, sg_vars, vars)) {
         // MATCHED VariableList
      } else return ATfalse;

      if (traverse_Formula(t_expr, sg_expr, expr)) {
         // MATCHED Formula
      } else return ATfalse;

      ROSE_ASSERT (labels.size() <= 1);

      if (vars.size() > 1) {
         cerr << "WARNING UNIMPLEMENTED: AssignmentStatement - with multiple variables\n";
      }

      if (!expr) {
         cerr << "WARNING UNIMPLEMENTED: AssignmentStatement - could be FunctionCall, or StatusConstant, or PointerLiteral, etc.\n";
      }
      else {
      // This assertion probably should remain after implementation
         ROSE_ASSERT (expr);
      }

   // TODO - need list for labels in untyped IR
      if (labels.size() == 1) temp_label = labels[0];

      SgUntypedAssignmentStatement* assign_stmt = new SgUntypedAssignmentStatement(temp_label,vars[0],expr);
      setSourcePosition(assign_stmt, term);

      stmt_list->get_stmt_list().push_back(assign_stmt);

   // Begin SageTreeTraversal
      SgAssignOp* sg_assign_op = SageBuilder::buildBinaryExpression_nfi<SgAssignOp>(sg_vars[0], sg_expr);
      setSourcePosition(sg_assign_op, term);

      SgExprStatement* sg_assign_stmt = SageBuilder::buildExprStatement(sg_assign_op);
      ROSE_ASSERT(sg_assign_stmt != nullptr);
      setSourcePosition(sg_assign_stmt, term);

      SageInterface::appendStatement(sg_assign_stmt, SageBuilder::topScopeStack());

   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 4.2 LOOP STATEMENTS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_WhileStatement(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_WhileStatement: %s\n", ATwriteToString(term));
#endif

   ATerm t_labels, t_clause, t_stmt, t_formula;
   std::vector<std::string> labels;
   std::vector<PosInfo> locations;

   SgUntypedStatementList* while_body_list = NULL;
   SgUntypedExpression* condition = NULL;
   SgExpression* sg_condition = nullptr;

   if (ATmatch(term, "WhileStatement(<term>,<term>,<term>)", &t_labels, &t_clause, &t_stmt)) {
      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;

      if (ATmatch(t_clause, "WhileClause(<term>)", &t_formula)) {
         // MATCHED WhileClause
         if (traverse_BitFormula(t_formula, sg_condition, condition)) {
            // MATCHED BitFormula
         } else return ATfalse;
      }

      while_body_list = new SgUntypedStatementList();

      // Match ControlledStatement -- it is a Statement
      if (traverse_Statement(t_stmt, while_body_list)) {
         // MATCHED Statement
      } else return ATfalse;

   // List will either contain one simple statement or one block statement
      ROSE_ASSERT(while_body_list->get_stmt_list().size() == 1);
   }
   else return ATfalse;

   SgUntypedStatement * body = while_body_list->get_stmt_list().back();
   while_body_list->get_stmt_list().pop_back();
   delete while_body_list;

   SgUntypedWhileStatement* while_stmt = new SgUntypedWhileStatement("", condition, body);
   ROSE_ASSERT(while_stmt);
   setSourcePosition(while_stmt, term);

   stmt_list->get_stmt_list().push_back(while_stmt);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_ForStatement(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ForStatement: %s\n", ATwriteToString(term));
#endif

   using namespace Jovial_ROSE_Translation;

   ATerm t_labels, t_clause, t_stmt;
   std::vector<std::string> labels;
   std::vector<PosInfo> locations;

   SgUntypedExpression* var_ref = NULL;
   SgUntypedExpression* init    = NULL;
   SgUntypedExpression* phrase1 = NULL;
   SgUntypedExpression* phrase2 = NULL;

   int phrase1_enum = e_unknown;
   int phrase2_enum = e_unknown;
   int stmt_enum    = e_unknown;

   SgUntypedForStatement* for_stmt = NULL;
   SgUntypedStatementList* for_body_list = NULL;
   SgUntypedStatement* body = NULL;

   if (ATmatch(term, "ForStatement(<term>,<term>,<term>)", &t_labels, &t_clause, &t_stmt)) {
      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;

      if (traverse_ForClause(t_clause, var_ref, init, phrase1, phrase2, phrase1_enum, phrase2_enum)) {
         // MATCHED ForClause
      } else return ATfalse;

      for_body_list = new SgUntypedStatementList();

      // Match ControlledStatement which is a Statement
      if (traverse_Statement(t_stmt, for_body_list)) {
         // MATCHED Statement
      } else return ATfalse;
   }
   else return ATfalse;

   //cout << ".x. loop body size is " << for_body_list->get_stmt_list().size() << endl;
   //WHY????   ROSE_ASSERT(for_body_list->get_stmt_list().size() > 0);

   if (for_body_list->get_stmt_list().size() > 0) {
      body = for_body_list->get_stmt_list().back();
      for_body_list->get_stmt_list().pop_back();
   }

   int op_enum = General_Language_Translation::e_operator_assign;
   SgUntypedBinaryOperator* initialization = new SgUntypedBinaryOperator(op_enum,"assign",var_ref,init);
   ROSE_ASSERT(initialization);
   setSourcePosition(initialization, t_clause);

// WHILE then optional BY or THEN (increment expression)
   if (phrase1_enum == e_while_phrase_expr) {
      if (phrase2_enum == e_by_phrase_expr) {
         stmt_enum = e_for_while_by_stmt;
      }
      else if (phrase2_enum == e_then_phrase_expr) {
         stmt_enum = e_for_while_then_stmt;
      }
      else {
         // let the BY usage be the default as it matches C increment usage
         stmt_enum = e_for_while_by_stmt;
         phrase2 = UntypedBuilder::buildUntypedNullExpression();
      }
      for_stmt = new SgUntypedForStatement("", stmt_enum, initialization, phrase1, phrase2, body, "");
   }
// BY (increment expression) then optional WHILE
   else if (phrase1_enum == e_by_phrase_expr) {
      stmt_enum = e_for_by_while_stmt;
      if (phrase2_enum != e_while_phrase_expr) {
         phrase2 = UntypedBuilder::buildUntypedNullExpression();
      }
      for_stmt = new SgUntypedForStatement("", stmt_enum, initialization, phrase2, phrase1, body, "");
   }
// THEN (increment expression) then optional WHILE
   else if (phrase1_enum == e_then_phrase_expr) {
      stmt_enum = e_for_then_while_stmt;
      if (phrase2_enum != e_while_phrase_expr) {
         phrase2 = UntypedBuilder::buildUntypedNullExpression();
      }
      for_stmt = new SgUntypedForStatement("", stmt_enum, initialization, phrase2, phrase1, body, "");
   }

   ROSE_ASSERT(for_stmt);
   setSourcePosition(for_stmt, term);

   stmt_list->get_stmt_list().push_back(for_stmt);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_ForClause(ATerm term, SgUntypedExpression* & var_ref, SgUntypedExpression* & init,
                                                                     SgUntypedExpression* & phrase1, SgUntypedExpression* & phrase2,
                                                                     int & phrase1_enum, int & phrase2_enum)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ForClause: %s\n", ATwriteToString(term));
#endif

   ATerm t_item, t_clause;
   char* name;

   init = NULL;
   var_ref = NULL;
   phrase1 = NULL;
   phrase2 = NULL;
   phrase1_enum = Jovial_ROSE_Translation::e_unknown;
   phrase2_enum = Jovial_ROSE_Translation::e_unknown;

   if (ATmatch(term, "ForClause(<term>,<term>)", &t_item, &t_clause)) {
      // MATCHED ForClause

      if (ATmatch(t_item, "<str>" , &name)) {
         // MATCHED ControlItem
         int expr_enum = Jovial_ROSE_Translation::e_referenceExpression;
         var_ref = new SgUntypedReferenceExpression(expr_enum, name);
         ROSE_ASSERT(var_ref);
         setSourcePosition(var_ref, t_item);
      } else return ATfalse;

      if (traverse_ControlClause(t_clause, init, phrase1, phrase2, phrase1_enum, phrase2_enum)) {
         // MATCHED ControlClause
      } else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_ControlClause(ATerm term, SgUntypedExpression* & initial_value,
                                                             SgUntypedExpression* & phrase1, SgUntypedExpression* & phrase2,
                                                             int & phrase1_enum, int & phrase2_enum)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ControlClause: %s\n", ATwriteToString(term));
#endif

   ATerm t_value, t_continuation;

   initial_value = NULL;
   phrase1 = NULL;
   phrase2 = NULL;
   phrase1_enum = Jovial_ROSE_Translation::e_unknown;
   phrase2_enum = Jovial_ROSE_Translation::e_unknown;

// Begin SageTreeBuilder
   SgExpression* sg_expr = nullptr;

   if (ATmatch(term, "ControlClause(<term>,<term>)", &t_value, &t_continuation)) {
      // MATCHED ControlClause
      if (traverse_Formula(t_value, sg_expr, initial_value)) {
         // MATCHED InitialValue
      } else return ATfalse;
      if (traverse_OptContinuation(t_continuation, phrase1, phrase2, phrase1_enum, phrase2_enum)) {
         // MATCHED OptContinuation
      } else return ATfalse;

   return ATtrue;

   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_OptContinuation(ATerm term, SgUntypedExpression* & phrase1, SgUntypedExpression* & phrase2,
                                                               int & phrase_enum1, int & phrase_enum2)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptContinuation: %s\n", ATwriteToString(term));
#endif

   phrase1 = NULL;
   phrase2 = NULL;
   phrase_enum1 = Jovial_ROSE_Translation::e_unknown;
   phrase_enum2 = Jovial_ROSE_Translation::e_unknown;

   if (ATmatch(term, "no-continuation")) {
   } else if (traverse_Continuation(term, phrase1, phrase2, phrase_enum1, phrase_enum2)) {
      // MATCHED Continuation
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_Continuation(ATerm term, SgUntypedExpression* & phrase1, SgUntypedExpression* & phrase2,
                                                            int & phrase_enum_1, int & phrase_enum_2)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Continuation: %s\n", ATwriteToString(term));
#endif

   ATerm t_phrase1, t_phrase2;

   if (ATmatch(term, "Continuation(<term>,<term>)", &t_phrase1, &t_phrase2)) {
      if (traverse_Phrase(t_phrase1, phrase1, phrase_enum_1)) {
         // MATCHED Phrase
      } else return ATfalse;

      if (ATmatch(t_phrase2, "no-while-phrase")) {
         // MATCHED no-while-phrase
      }
      else if (ATmatch(t_phrase2, "no-by-or-then-phrase")) {
         // MATCHED no-by-or-then-phrase
      }
      else if (traverse_Phrase(t_phrase2, phrase2, phrase_enum_2)) {
         // MATCHED Phrase
      } else return ATfalse;

   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_Phrase(ATerm term, SgUntypedExpression* & expr, int & phrase_enum)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Phrase: %s\n", ATwriteToString(term));
#endif

   ATerm t_formula;

   expr = NULL;
   phrase_enum = Jovial_ROSE_Translation::e_unknown;

// Begin SageTreeBuilder
   SgExpression* sg_expr = nullptr;

   if (ATmatch(term, "ByPhrase(<term>)", &t_formula)) {
      // MATCHED ByPhrase
      if (traverse_NumericFormula(t_formula, sg_expr, expr)){
         // MATCHED NumericFormula
         phrase_enum = Jovial_ROSE_Translation::e_by_phrase_expr;
      } else return ATfalse;
   } else if (ATmatch(term, "ThenPhrase(<term>)", &t_formula)) {
      // MATCHED ThenPhrase
      if (traverse_Formula(t_formula, sg_expr, expr)){
         // MATCHED Formula
         phrase_enum = Jovial_ROSE_Translation::e_then_phrase_expr;
      } else return ATfalse;
   } else if (ATmatch(term, "WhilePhrase(<term>)", &t_formula)) {
      // MATCHED WhilePhrase
      if (traverse_BitFormula(t_formula, sg_expr, expr)){
         // BooleanFormula defaults to BitFormula
         // MATCHED BitFormula
         phrase_enum = Jovial_ROSE_Translation::e_while_phrase_expr;
      } else return ATfalse;
   }

   return ATtrue;
}

//========================================================================================
// 4.3 IF STATEMENTS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_IfStatement(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_IfStatement: %s\n", ATwriteToString(term));
#endif

   ATerm t_labels, t_cond, t_else, t_true, t_false;
   std::vector<std::string> labels;
   std::vector<PosInfo> locations;
   SgUntypedExpression* conditional;
   SgExpression* sg_conditional = nullptr;
   SgUntypedStatement *stmt, *true_body, *false_body;

   if (ATmatch(term, "IfStatement(<term>,<term>,<term>,<term>)", &t_labels,&t_cond,&t_true,&t_else)) {

      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;

      if (traverse_BitFormula(t_cond, sg_conditional, conditional)) {
         // MATCHED BitFormula
      } else return ATfalse;

      if (traverse_Statement(t_true, stmt_list)) {
         true_body = stmt_list->get_stmt_list().back();
         stmt_list->get_stmt_list().pop_back();
      } else return ATfalse;

      if (ATmatch(t_else, "no-else-clause()")) {
         false_body = NULL;
      }
      else if (ATmatch(t_else, "ElseClause(<term>)", &t_false)) {
         if (traverse_Statement(t_false, stmt_list)) {
            false_body = stmt_list->get_stmt_list().back();
            stmt_list->get_stmt_list().pop_back();
         } else return ATfalse;
      }
      else return ATfalse;
   }
   else return ATfalse;

   int statement_enum = General_Language_Translation::e_unknown;
   //   SgUntypedIfStatement* if_stmt = SageBuilder::buildUntypedIfStatement("",statement_enum,conditional,true_body,false_body);
   SgUntypedIfStatement* if_stmt = new SgUntypedIfStatement("", statement_enum, conditional, true_body, false_body);
   setSourcePosition(if_stmt, term);

   stmt = convert_Labels(labels, locations, if_stmt);

   stmt_list->get_stmt_list().push_back(stmt);

   return ATtrue;
}

//========================================================================================
// 4.4 CASE STATEMENTS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_CaseStatement(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CaseStatement: %s\n", ATwriteToString(term));
#endif

   ATerm t_labels, t_formula, t_case_body, t_labels2;
   std::vector<std::string> labels, labels2;
   std::vector<PosInfo> locations, locations2;
   SgUntypedStatement* stmt;
   SgUntypedExpression* formula = NULL;
   SgUntypedStatement* body = NULL;

// Begin SageTreeBuilder
   SgExpression* sg_expr = nullptr;

   int stmt_enum = General_Language_Translation::e_switch_stmt;

   if (ATmatch(term, "CaseStatement(<term>,<term>,<term>,<term>)", &t_labels, &t_formula, &t_case_body, &t_labels2)) {
      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;

      if (traverse_Formula(t_formula, sg_expr, formula)) {
        // MATCHED Formula
      } else return ATfalse;

      if (traverse_CaseBody(t_case_body, &body)) {
        // MATCHED CaseBody
      } else return ATfalse;

      if (traverse_LabelList(t_labels2, labels2, locations2)) {
         // MATCHED LabelList
         ROSE_ASSERT(locations2.size() == 0);  // TODO
      } else return ATfalse;

      ROSE_ASSERT(formula != NULL);
      ROSE_ASSERT(body != NULL);

      SgUntypedCaseStatement* case_stmt = new SgUntypedCaseStatement("", stmt_enum, formula, body, "", true);
      setSourcePosition(case_stmt, term);

      stmt = convert_Labels(labels, locations, case_stmt);
   }
   else return ATfalse;

   stmt_list->get_stmt_list().push_back(stmt);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_CaseBody(ATerm term, SgUntypedStatement** case_body)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CaseBody: %s\n", ATwriteToString(term));
#endif

   SgUntypedBlockStatement* body = SageBuilder::buildUntypedBlockStatement("");
   ROSE_ASSERT(body != NULL);
   setSourcePosition(body, term);

   SgUntypedStatementList* my_stmt_list = body->get_scope()->get_statement_list();

   ATermList tail = (ATermList) ATmake("<term>", term);
   while (! ATisEmpty(tail)) {
      ATerm head = ATgetFirst(tail);
      tail = ATgetNext(tail);
      if (traverse_CaseAlternative(head, my_stmt_list)) {
         // MATCHED CaseAlternative
      } else if (traverse_DefaultOption(head, my_stmt_list)) {
         // MATCHED DefaultOption
      } else return ATfalse;
   }

   *case_body = body;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_CaseAlternative(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CaseAlternative: %s\n", ATwriteToString(term));
#endif

   ATerm t_case_index_group, t_stmt, t_fall_thru;
   bool fall_thru;
   SgUntypedBlockStatement* body = NULL;
   SgUntypedExprListExpression* case_index_group = NULL;

   int stmt_enum = General_Language_Translation::e_case_option_stmt;

   if (ATmatch(term, "CaseAlternative(<term>,<term>,<term>)", &t_case_index_group, &t_stmt, &t_fall_thru)) {

      body = SageBuilder::buildUntypedBlockStatement("");
      ROSE_ASSERT(body != NULL);
      setSourcePosition(body, term);

      SgUntypedStatementList* my_stmt_list = body->get_scope()->get_statement_list();

      if (traverse_CaseIndexGroup(t_case_index_group, case_index_group)) {
         // MATCHED CaseIndexGroup
      } else return ATfalse;

      if (traverse_Statement(t_stmt, my_stmt_list)) {
        // MATCHED Statement
      } else return ATfalse;

      if (ATmatch(t_fall_thru, "no-fall-thru()")) {
         // MATCHED no-fall-thru
         fall_thru = false;
      } else if (ATmatch(t_fall_thru, "FALLTHRU()")) {
         // MATCHED FALLTHRU
         fall_thru = true;
      } else return ATfalse;

   } else return ATfalse;

   if (!case_index_group) {
      cerr << "WARNING UNIMPLEMENTED: CaseAlternative - probably StatusConstant\n";
      return ATtrue;
   }

   ROSE_ASSERT(case_index_group != NULL);
   ROSE_ASSERT(body != NULL);

   SgUntypedCaseStatement* case_stmt = new SgUntypedCaseStatement("", stmt_enum, case_index_group, body, "", fall_thru);
   ROSE_ASSERT(case_stmt != NULL);
   setSourcePosition(case_stmt, term);

   stmt_list->get_stmt_list().push_back(case_stmt);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_DefaultOption(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_DefaultOption: %s\n", ATwriteToString(term));
#endif

   ATerm t_stmt, t_fall_thru;
   SgUntypedBlockStatement* body = NULL;

   int stmt_enum = General_Language_Translation::e_case_default_option_stmt;
   bool fall_thru = false;

   if (ATmatch(term, "DefaultOption(<term>,<term>)", &t_stmt, &t_fall_thru)) {

      body = SageBuilder::buildUntypedBlockStatement("");
      ROSE_ASSERT(body != NULL);
      setSourcePosition(body, term);

      SgUntypedStatementList* my_stmt_list = body->get_scope()->get_statement_list();

      if (traverse_Statement(t_stmt, my_stmt_list)) {
         // MATCHED Statement
      } else return ATfalse;

      if (ATmatch(t_fall_thru, "no-fall-thru()")) {
         // MATCHED no-fall-thru
         fall_thru = false;
      } else if (ATmatch(t_fall_thru, "FALLTHRU()")) {
         // MATCHED FALLTHRU
         fall_thru = true;
      } else return ATfalse;

   } else return ATfalse;

   ROSE_ASSERT(body != NULL);

   SgUntypedCaseStatement* case_stmt = new SgUntypedCaseStatement("", stmt_enum, NULL, body, "", fall_thru);
   ROSE_ASSERT(case_stmt != NULL);
   setSourcePosition(case_stmt, term);

   stmt_list->get_stmt_list().push_back(case_stmt);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_CaseIndexGroup(ATerm term, SgUntypedExprListExpression* & case_index_group)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CaseIndexGroup: %s\n", ATwriteToString(term));
#endif

   ATerm t_case_index;
   SgUntypedExpression* case_index;
   SgUntypedExprListExpression* index_group;

   case_index_group = NULL;

   if (ATmatch(term, "CaseIndexGroup(<term>)", &t_case_index)) {
      index_group = new SgUntypedExprListExpression(General_Language_Translation::e_case_selector);
      setSourcePosition(index_group, term);

      ATermList tail = (ATermList) ATmake("<term>", t_case_index);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_CaseIndex(head, case_index)) {
            // MATCHED CaseIndex
            if (!case_index) {
               cerr << "WARNING UNIMPLEMENTED: CaseIndexGroup - probably Status Constant\n";
               return ATtrue;
            }
            ROSE_ASSERT(case_index);
            index_group->get_expressions().push_back(case_index);
         } else return ATfalse;
      }
   } else return ATfalse;

   ROSE_ASSERT(index_group != NULL);
   case_index_group = index_group;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_CaseIndex(ATerm term, SgUntypedExpression* & case_index)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_CaseIndex: %s\n", ATwriteToString(term));
#endif

   ATerm t_formula1, t_formula2;
   SgUntypedSubscriptExpression* range = NULL;
   SgUntypedExpression* lower_bound = NULL;
   SgUntypedExpression* upper_bound = NULL;
   SgUntypedExpression* stride = NULL;
   SgUntypedExpression* value = NULL;

// Begin SageTreeBuilder
   SgExpression* sg_expr = nullptr;

   case_index = NULL;

   if (ATmatch(term, "CaseIndex(<term>)", &t_formula1)) {
     // This case is needed to traverse CompileTimeFormula -> CaseIndex
      if (traverse_Formula(t_formula1, sg_expr, value)) {
         // MATCHED Formula
      } else return ATfalse;

   } else if (ATmatch(term, "CaseIndex(<term>,<term>)", &t_formula1, &t_formula2)) {
     // This case is needed to traverse LowerBound : UpperBound -> CaseIndex
      if (traverse_Formula(t_formula1, sg_expr, lower_bound)) {
         // MATCHED Formula
      } else return ATfalse;
      if (traverse_Formula(t_formula2, sg_expr, upper_bound)) {
         // MATCHED Formula
      } else return ATfalse;
   }
   else return ATfalse;

   if (value) {
      case_index = value;
   }
   else if (lower_bound && upper_bound) {
      int expr_enum = General_Language_Translation::e_case_range;
      stride = UntypedBuilder::buildUntypedNullExpression();
      setSourcePositionUnknown(stride);
      range = new SgUntypedSubscriptExpression(expr_enum, lower_bound, upper_bound, stride);
      setSourcePosition(range, term);
      case_index = range;
   }
   else {
      cerr << "WARNING UNIMPLEMENTED: CaseIndex - probably StatusConstant in lower_bound or upper_bound\n";
      return ATtrue;
      ROSE_ASSERT(0);
   }
   ROSE_ASSERT(case_index);

   return ATtrue;
}

//========================================================================================
// 4.5 PROCEDURE CALL STATEMENTS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_ProcedureCallStatement(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ProcedureCallStatement: %s\n", ATwriteToString(term));
#endif

   ATerm t_labels, t_proc_name, t_arg_list, t_abort_phrase, t_abort_name;
   std::vector<std::string> labels;
   std::vector<PosInfo> locations;
   std::string proc_name;
   std::string abort_stmt_name;
   SgUntypedExprListExpression* arg_list;

   SgUntypedStatement* stmt = NULL;

   if (ATmatch(term, "ProcedureCallStatement(<term>,<term>,<term>,<term>)", &t_labels, &t_proc_name, &t_arg_list, &t_abort_phrase)) {
      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;

      if (traverse_Name(t_proc_name, proc_name)) {
         // MATCHED Name
      } else return ATfalse;

      arg_list = new SgUntypedExprListExpression(General_Language_Translation::e_argument_list);
      ROSE_ASSERT(arg_list);
      setSourcePosition(arg_list, t_arg_list);

      if (traverse_ActualParameterList(t_arg_list, arg_list)) {
         // MATCHED ActualParameterList
      } else return ATfalse;

      if (ATmatch(t_abort_phrase, "no-abort-phrase()")) {
         // No AbortPhrase
      } else if (ATmatch(t_abort_phrase, "AbortPhrase(<term>)", &t_abort_name)) {
         if (traverse_Name(t_abort_name, abort_stmt_name)) {
            // MATCHED AbortStatementName
         } else return ATfalse;
      } else return ATfalse;

      int expr_enum = General_Language_Translation::e_function_reference;
      int stmt_enum = General_Language_Translation::e_procedure_call;

      SgUntypedReferenceExpression* func_ref = new SgUntypedReferenceExpression(expr_enum, proc_name);
      ROSE_ASSERT(func_ref);
      setSourcePosition(func_ref, t_proc_name);

   // TODO - add abort statement name
      SgUntypedFunctionCallStatement* func_call_stmt = new SgUntypedFunctionCallStatement("",stmt_enum,func_ref,arg_list,""/*abort_name*/);
      ROSE_ASSERT(func_call_stmt);
      setSourcePosition(func_call_stmt, term);

      stmt = convert_Labels(labels, locations, func_call_stmt);
   }
   else return ATfalse;

   ROSE_ASSERT(stmt);

   stmt_list->get_stmt_list().push_back(stmt);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_ActualParameterList(ATerm term, SgUntypedExprListExpression* arg_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ActualParameterList: %s\n", ATwriteToString(term));
#endif

   ATerm t_arg_list, t_output;
   SgUntypedExpression* arg_expr;
   SgUntypedExprListExpression* param_list = NULL;

// Begin SageTreeBuilder
   SgExpression* sg_expr = nullptr;

   if (ATmatch(term, "no-actual-parameter-list()")) {
      // MATCHED no-actual-parameter-list
   }
   else if (ATmatch(term, "ActualParameterList(<term>,<term>)" , &t_arg_list, &t_output)) {
      ATermList tail = (ATermList) ATmake("<term>", t_arg_list);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_Formula(head, sg_expr, arg_expr)) {
            // MATCHED Formula
         } else return ATfalse;

         arg_list->get_expressions().push_back(arg_expr);
      }

      if (traverse_ActualOutputParameters(t_output, param_list)) {
         // MATCHED ActualOutputParameters
      } else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_ActualOutputParameters(ATerm term, SgUntypedExprListExpression* param_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ActualOutputParameters: %s\n", ATwriteToString(term));
#endif

   ATerm t_output_list, t_param;
   SgUntypedExpression* param;

// Begin SageTreeBuilder
   SgExpression* sg_param = nullptr;

   if (ATmatch(term, "no-actual-output-parameters()")) {
      // MATCHED no-actual-output-parameters
      std::cout << "NOTE:::: no-actual-output-parameters" << std::endl;
   }
   else if (ATmatch(term, "ActualOutputParameters(<term>)" , &t_output_list)) {
      ATermList tail = (ATermList) ATmake("<term>", t_output_list);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (ATmatch(head, "Variable(<term>)", &t_param)) {
            if (traverse_Variable(t_param, sg_param, param)) {
               // MATCHED Variable

               // Variable                     -> ActualOutputParameter    {cons("Variable"), prefer}
               // BlockReference               -> ActualOutputParameter    {cons("BlockReference")}
            }
         } else return ATfalse;
      }
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 4.6 RETURN STATEMENTS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_ReturnStatement(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ReturnStatement: %s\n", ATwriteToString(term));
#endif

   ATerm t_labels;
   std::vector<std::string> labels;
   std::vector<PosInfo> locations;
   SgUntypedStatement* stmt;

   if (ATmatch(term, "ReturnStatement(<term>)", &t_labels)) {
      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;

      SgUntypedNullExpression * return_code = UntypedBuilder::buildUntypedNullExpression();
      SgUntypedReturnStatement* return_stmt = new SgUntypedReturnStatement("", return_code);
      setSourcePosition(return_stmt, term);

      stmt = convert_Labels(labels, locations, return_stmt);
   }
   else return ATfalse;

   stmt_list->get_stmt_list().push_back(stmt);

   return ATtrue;
}

//========================================================================================
// 4.7 GOTO STATEMENTS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_GotoStatement(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_GotoStatement: %s\n", ATwriteToString(term));
#endif

   ATerm t_labels, t_name;
   std::vector<std::string> labels;
   std::vector<PosInfo> locations;
   std::string name;
   SgUntypedStatement* stmt;

    if (ATmatch(term, "GotoStatement(<term>,<term>)", &t_labels, &t_name)) {
       if (traverse_LabelList(t_labels, labels, locations)) {
          // MATCHED LabelList
       } else return ATfalse;

       if (traverse_Name(t_name, name)) {
          // MATCHED Name
       } else return ATfalse;

      SgUntypedGotoStatement* goto_stmt = new SgUntypedGotoStatement("", name);
      setSourcePosition(goto_stmt, term);

      stmt = convert_Labels(labels, locations, goto_stmt);
   }
   else return ATfalse;

   stmt_list->get_stmt_list().push_back(stmt);

   return ATtrue;

}

//========================================================================================
// 4.8 EXIT STATEMENTS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_ExitStatement(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ExitStatement: %s\n", ATwriteToString(term));
#endif

   ATerm t_labels;
   std::vector<std::string> labels;
   std::vector<PosInfo> locations;
   SgUntypedStatement* stmt;

   if (ATmatch(term, "ExitStatement(<term>)", &t_labels)) {
      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;

      SgUntypedExitStatement* exit_stmt = new SgUntypedExitStatement("");
      setSourcePosition(exit_stmt, term);

      stmt = convert_Labels(labels, locations, exit_stmt);
   }
   else return ATfalse;

   stmt_list->get_stmt_list().push_back(stmt);

   return ATtrue;
}

//========================================================================================
// 4.9 STOP STATEMENTS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_StopStatement(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_StopStatement: %s\n", ATwriteToString(term));
#endif

   ATerm t_labels, t_stop_code;
   std::vector<std::string> labels;
   std::vector<PosInfo> locations;
   SgUntypedExpression* stop_code = NULL;
   SgExpression* sg_stop_code = nullptr;
   SgUntypedStatement* stmt;

   if (ATmatch(term, "StopStatement(<term>,<term>)", &t_labels, &t_stop_code)) {
      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;

      if (ATmatch(t_stop_code, "no-integer-formula()")) {
         // No StopCode
         stop_code = UntypedBuilder::buildUntypedNullExpression();
      }
      else if (traverse_NumericFormula(t_stop_code, sg_stop_code, stop_code)) {
         // MATCHED NumericFormula
      } else return ATfalse;

      SgUntypedStopStatement* stop_stmt = new SgUntypedStopStatement("", stop_code);
      setSourcePosition(stop_stmt, term);

      stmt = convert_Labels(labels, locations, stop_stmt);
   }
   else return ATfalse;

   stmt_list->get_stmt_list().push_back(stmt);

   return ATtrue;
}

//========================================================================================
// 4.10 ABORT STATEMENTS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_AbortStatement(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_AbortStatement: %s\n", ATwriteToString(term));
#endif

   ATerm t_labels;
   std::vector<std::string> labels;
   std::vector<PosInfo> locations;
   SgUntypedStatement* stmt;

   if (ATmatch(term, "AbortStatement(<term>)", &t_labels)) {
      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;

      SgUntypedAbortStatement* abort_stmt = new SgUntypedAbortStatement("");
      setSourcePosition(abort_stmt, term);

      stmt = convert_Labels(labels, locations, abort_stmt);
   }
   else return ATfalse;

   stmt_list->get_stmt_list().push_back(stmt);

   return ATtrue;
}

//========================================================================================
// 5.0 FORMULAS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_Formula(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr)
{
   if (traverse_NumericFormula(term, sg_expr, expr)) {
      // MATCHED NumericFormula
   } else if (traverse_BitFormula(term, sg_expr, expr)) {
      // MATCHED BitFormula
   } else if (traverse_GeneralFormula(term, sg_expr, expr)) {
      // MATCHED GeneralFormula
   } else return ATfalse;

   //  TableFormula                -> Formula

   return ATtrue;
}

//========================================================================================
// 5.1 NUMERIC FORMULAS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_NumericFormula(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* & expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_NumericFormula: %s\n", ATwriteToString(term));
#endif

   ATerm t_sign, t_expr, t_lhs, t_op, t_rhs;

   // OptSign NumericTerm -> NumericFormula
   //
   if (ATmatch(term, "NumericFormula(<term>,<term>)", &t_sign, &t_expr)) {
      General_Language_Translation::ExpressionKind op_enum;

      if (traverse_OptSign(t_sign, op_enum)) {
         // MATCHED OptSign
      } else return ATfalse;
      if (traverse_NumericTerm(t_expr, sg_expr, expr)) {
         // MATCHED NumericTerm
      } else return ATfalse;
      if (op_enum == General_Language_Translation::e_operator_unary_minus) {
        expr = new SgUntypedUnaryOperator(op_enum, "-", expr);
        setSourcePosition(expr, t_sign);
      }
      else if (op_enum == General_Language_Translation::e_operator_unary_plus) {
        expr = new SgUntypedUnaryOperator(op_enum, "+", expr);
        setSourcePosition(expr, t_sign);
      }
   }

   // NumericFormula PlusOrMinus NumericTerm -> NumericFormula
   //
   else if (ATmatch(term, "NumericFormula(<term>,<term>,<term>)", &t_lhs,&t_op,&t_rhs)) {
      std::string op_name;
      General_Language_Translation::ExpressionKind op_enum;
      SgUntypedExpression * lhs, * rhs;
      SgExpression * sg_lhs = nullptr, * sg_rhs = nullptr;

      if (traverse_NumericFormula(t_lhs, sg_lhs, lhs)) {
         // MATCHED NumericFormula
      } else return ATfalse;

      if (traverse_NumericTerm(t_rhs, sg_rhs, rhs)) {
         // MATCHED NumericTerm
      } else return ATfalse;

      if (ATmatch(t_op, "AddOp()")) {
         op_enum = General_Language_Translation::e_operator_add;
         op_name = "+";

         sg_expr = new SgAddOp(sg_lhs, sg_rhs, NULL);
      }
      else if (ATmatch(t_op, "SubtractOp()")) {
         op_enum = General_Language_Translation::e_operator_subtract;
         op_name = "-";

         sg_expr = new SgSubtractOp(sg_lhs, sg_rhs, NULL);
      } else return ATfalse;

      expr = new SgUntypedBinaryOperator(op_enum,op_name,lhs,rhs);
      setSourcePosition(expr, term);
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_NumericPrimary(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_NumericPrimary: %s\n", ATwriteToString(term));
#endif

   ATerm t_num_var, t_formula, t_factor, t_num_term, t_conversion;
   char *literal, *name, *variable;
   SgUntypedType* type;
   SgUntypedExpression *conv, *num_term, *factor;
   SgExpression *sg_conv = nullptr, *sg_num_term = nullptr, *sg_factor = nullptr;

   Jovial_ROSE_Translation::ExpressionKind expr_enum = Jovial_ROSE_Translation::e_referenceExpression;

   expr = NULL;
   sg_expr = nullptr;

   if (ATmatch(term, "IntegerLiteral(<str>)", &literal)) {
      type = UntypedBuilder::buildType(SgUntypedType::e_int);
      expr_enum = Jovial_ROSE_Translation::e_literalExpression;
      expr = new SgUntypedValueExpression(expr_enum,literal,type);
      ROSE_ASSERT(expr);
      setSourcePosition(expr, term);

   // Begin SageTreeBuilder
      sg_expr = SageBuilder::buildIntVal_nfi(std::string(literal));
   }

   else if (traverse_FixedOrFloatingLiteral(term, sg_expr, expr)) {
         //FixedOrFloatingLiteral
   }

   else if (traverse_NumericMachineParameter(term, sg_expr, expr)) {
      // MATCHED NumericMachineParameter

      if (!expr) {
         cerr << "WARNING UNIMPLEMENTED: NumericPrimary - NumericMachineParameter\n";
      }
   }

   else if (ATmatch(term, "NumericVariable(<str>)", &variable)) {
      expr_enum = Jovial_ROSE_Translation::e_referenceExpression;
      expr = new SgUntypedReferenceExpression(expr_enum, variable);
      setSourcePosition(expr, term);
   }

   else if (ATmatch(term, "NumericVariable(<term>)", &t_num_var)) {
#if 0
      if (traverse_TableItem(t_table_item, table_item)){
         //MATCHED TableItem
         expr = table_item;
      } else return ATfalse;
      //      expr_enum = Jovial_ROSE_Translation::e_referenceExpression;
      //      expr = new SgUntypedReferenceExpression(expr_enum, table_item);
      //      setSourcePosition(expr, term);
#endif

      // need to traverse all possible Variables, including TableItem
      if (traverse_Variable(t_num_var, sg_expr, expr)) {
         if (!expr) {
            cerr << "WARNING UNIMPLEMENTED: NumericPrimary - NumericVariable\n";
         }
      } else return ATfalse;
   }

   else if (ATmatch(term, "<str>" , &name)) {
      expr_enum = Jovial_ROSE_Translation::e_referenceExpression;
      expr = new SgUntypedReferenceExpression(expr_enum, name);
      setSourcePosition(expr, term);
   }

   else if (ATmatch(term, "NumericPrimaryParens(<term>)", &t_formula)) {

      if (traverse_NumericFormula(t_formula, sg_expr, expr)) {
         //  '(' NumericFormula ')'      -> NumericPrimary         {cons("NumericPrimary")}
         // TODO: Add way to indicate parens?
         // expr.set_need_paren();
         if (!expr) {
            cerr << "WARNING UNIMPLEMENTED: NumericPrimary - Parens - maybe because of FunctionCall\n";
         }
      } else return ATfalse;

   }

   else if (ATmatch(term, "NumericPrimary(<term>,<term>)", &t_conversion, &t_formula)) {

      if (traverse_IntegerConversion(t_conversion, sg_conv, conv)) {
         //  IntegerConversion '(' Formula ')' -> IntegerPrimary  {cons("IntegerPrimary")}
         // MATCHED IntegerConversion
      } else if (traverse_GeneralConversion(t_conversion, sg_conv, conv)) {
         // MATCHED GeneralConversion
      } else if (traverse_FloatingConversion(t_conversion, sg_conv, conv)) {
         // MATCHED FloatingConversion
      } else if (traverse_FixedConversion(t_conversion, sg_conv, conv)) {
         // MATCHED FixedConversion
      } else return ATfalse;

      if (traverse_Formula(t_formula, sg_expr, expr)) {
         // MATCHED Formula
      } else return ATfalse;

   }

   else if (ATmatch(term, "NumericPrimary(<term>,<term>,<term>)", &t_conversion, &t_num_term, &t_factor)) {

      if (traverse_FixedConversion(t_conversion, sg_conv, conv)) {
      } else return ATfalse;
      if (traverse_NumericTerm(t_num_term, sg_num_term, num_term)) {
      } else return ATfalse;
      if (traverse_NumericFactor(t_factor, sg_factor, factor)) {
      } else return ATfalse;

   }

   else if (traverse_FunctionCall(term, sg_expr, expr)) {
      // MATCHED FunctionCall
      if (!expr) {
         cerr << "WARNING UNIMPLEMENTED: NumericPrimary - FunctionCall\n";
      }
   }

   else if (ATmatch(term, "ControlLetter(<str>)", &variable)) {
      expr_enum = Jovial_ROSE_Translation::e_referenceExpression;
      expr = new SgUntypedReferenceExpression(expr_enum, variable);
      setSourcePosition(expr, term);
   }

   else return ATfalse;

// TODO FIXME (temporarily return integer literal expression "13")
   if (expr == NULL) {
      cerr << "WARNING UNIMPLEMENTED: NumericPrimary - expr is null \n";
      SgUntypedType* type = UntypedBuilder::buildType(SgUntypedType::e_int);
      Jovial_ROSE_Translation::ExpressionKind expr_enum = Jovial_ROSE_Translation::e_literalExpression;
      expr = new SgUntypedValueExpression(expr_enum,"13",type);
      setSourcePosition(expr, term);
   }

   ROSE_ASSERT(expr);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_OptSign(ATerm term, General_Language_Translation::ExpressionKind & op_enum)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptSign: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "no-sign()")) {
      op_enum = General_Language_Translation::e_operator_unity;
   }
   else if (ATmatch(term, "PLUS()")) {
      op_enum = General_Language_Translation::e_operator_unary_plus;
   }
   else if (ATmatch(term, "MINUS()")) {
      op_enum = General_Language_Translation::e_operator_unary_minus;
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_NumericTerm(ATerm term, SgExpression* & sg_expr, SgUntypedExpression* & expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_NumericTerm: %s\n", ATwriteToString(term));
#endif

   ATerm t_lhs, t_op, t_rhs;
   std::string op_name;
   General_Language_Translation::ExpressionKind op_enum;
   SgUntypedExpression * lhs, * rhs;
   SgExpression *sg_lhs = nullptr, *sg_rhs = nullptr;


   if (ATmatch(term, "NumericTerm(<term>,<term>,<term>)", &t_lhs, &t_op, &t_rhs)) {
      if (traverse_NumericTerm(t_lhs, sg_lhs, lhs)) {
         // MATCHED NumericTerm
      } else return ATfalse;

      if (traverse_MultiplyDivideOrMod(t_op, op_enum, op_name)) {
         // MATCHED MultiplyDivideOrMod
      } else return ATfalse;

      if (traverse_NumericFactor(t_rhs, sg_rhs, rhs)) {
         // MATCHED NumericFactor
      } else return ATfalse;

      expr = new SgUntypedBinaryOperator(op_enum,op_name,lhs,rhs);
      setSourcePosition(expr, term);
   }
   else if (traverse_NumericFactor(term, sg_expr, expr)) {
         // MATCHED NumericFactor
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_NumericFactor(ATerm term, SgExpression* & sg_expr, SgUntypedExpression* & expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_NumericFactor: %s\n", ATwriteToString(term));
#endif

   // NEED TO DECIDE: whether to use this or use traverse_ExponentiationOp
   // or use both

   if (traverse_NumericPrimary(term, sg_expr, expr)) {
      // MATCHED NumericPrimary
   } else if (traverse_ExponentiationOp(term, sg_expr, expr)) {
         // MATCHED ExponentiationOp
   }

   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_ExponentiationOp(ATerm term, SgExpression* & sg_expr, SgUntypedExpression* & expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ExponentiationOp: %s\n", ATwriteToString(term));
#endif

   ATerm t_lhs, t_rhs;
   SgUntypedExpression * lhs, * rhs;
   SgExpression * sg_lhs = nullptr, * sg_rhs = nullptr;
   General_Language_Translation::ExpressionKind op_enum;
   std::string op_name;

   if (ATmatch(term, "ExponentiationOp(<term>,<term>)", &t_lhs, &t_rhs)) {
      if (traverse_NumericFactor(t_lhs, sg_lhs, lhs)) {
         // MATCHED NumericFactor
      } else return ATfalse;

      if (traverse_NumericPrimary(t_rhs, sg_rhs, rhs)) {
         // MATCHED NumericPrimary
      } else return ATfalse;

      op_enum = General_Language_Translation::e_operator_exponentiate;
      op_name = "**";
      expr = new SgUntypedBinaryOperator(op_enum,op_name,lhs,rhs);
      setSourcePosition(expr, term);
   }

   //   else if (traverse_NumericPrimary(term, expr)) {
      // MATCHED NumericPrimary
   //   }

   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_NumericMachineParameter(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_NumericMachineParameter: %s\n", ATwriteToString(term));
#endif

   if (traverse_IntegerMachineParameter(term, sg_expr, expr)) {
      // MATCHED IntegerMachineParameter
   } else if (traverse_FloatingMachineParameter(term, sg_expr, expr)) {
      // MATCHED FloatingMachineParameter
   } else if (traverse_FixedMachineParameter(term, sg_expr, expr)) {
      // MATCHED FixedMachineParameter
   }

   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 5.2 BIT FORMULAS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_BitFormula(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BitFormula: %s\n", ATwriteToString(term));
#endif

   ATerm t_operand, t_continuation, t_amb;
   SgUntypedExpression* continuation;
   SgExpression* sg_continuation = nullptr;

   expr = NULL;
   if (ATmatch(term, "BitFormula(<term>,<term>)", &t_operand, &t_continuation)) {
      if (ATmatch(t_operand, "amb(<term>)", &t_amb)) {
         ATermList tail = (ATermList) ATmake("<term>", t_amb);
         ATerm head = ATgetFirst(tail);
         // chose first amb path, now traverse it

         if (traverse_RelationalExpression(head, sg_expr, expr)) {
            // MATCHED RelationalExpression
         } else return ATfalse;
      } else if (traverse_LogicalOperand(t_operand, sg_expr, expr)) {
         // MATCHED LogicalOperand
      } else return ATfalse;

      if (traverse_OptLogicalContinuation(t_continuation, sg_continuation, continuation)) {
         // MATCHED OptLogicalContinuation
      // TODO
         if (continuation != NULL) {
            cerr << "WARNING UNIMPLEMENTED: BitFormula - with continuation\n";
         }
         //         ROSE_ASSERT(continuation == NULL);
      } else return ATfalse;

   } else if (ATmatch(term, "BitFormulaNOT(<term>)", &t_operand)) {
      if (traverse_LogicalOperand(t_operand, sg_expr, expr)) {
         // MATCHED LogicalOperand
      } else return ATfalse;

   } else if (ATmatch(term, "BitVariableFormula(<term>)", &t_operand)) {
      if (traverse_Variable(t_operand, sg_expr, expr)) {
         // MATCHED Variable
      } else return ATfalse;
   } else return ATfalse;

   if (!expr) {
      cerr << "WARNING UNIMPLEMENTED: BitFormula - BitPrimaryConversion or Dereference\n";
      return ATtrue;
   }

   ROSE_ASSERT(expr != NULL);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_OptLogicalContinuation(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptLogicalContinuation: %s\n", ATwriteToString(term));
#endif

   expr = NULL;
   if (ATmatch(term, "no-logical-continuation")) {
      // MATCHED no-logical-continuation
   } else {
      ATermList tail = (ATermList) ATmake("<term>", term);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_LogicalContinuation(head, sg_expr, expr)) {
            // MATCHED LogicalContinuation
         } else return ATfalse;
      }
   }

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_LogicalContinuation(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_LogicalContinuation: %s\n", ATwriteToString(term));
#endif

   ATerm t_operand;

   expr = NULL;

   if (ATmatch(term, "AndContinuation(<term>)", &t_operand)) {
      if (traverse_LogicalOperand(t_operand, sg_expr, expr)) {
      // MATCHED LogicalOperand
      } else return ATfalse;
   } else if (ATmatch(term, "OrContinuation(<term>)", &t_operand)) {
      if (traverse_LogicalOperand(t_operand, sg_expr, expr)) {
      // MATCHED LogicalOperand
      } else return ATfalse;
   } else if (ATmatch(term, "XorContinuation(<term>)", &t_operand)) {
      if (traverse_LogicalOperand(t_operand, sg_expr, expr)) {
      // MATCHED LogicalOperand
      } else return ATfalse;
   } else if (ATmatch(term, "EqvContinuation(<term>)", &t_operand)) {
      if (traverse_LogicalOperand(t_operand, sg_expr, expr)) {
      // MATCHED LogicalOperand
      } else return ATfalse;
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_LogicalOperand(ATerm term, SgExpression* & sg_expr, SgUntypedExpression* & expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_LogicalOperand: %s\n", ATwriteToString(term));
#endif

   expr = NULL;
   if (traverse_BitPrimary(term, sg_expr, expr)) {
      // MATCHED BitPrimary
      if (!expr) {
         cerr << "WARNING UNIMPLEMENTED: LogicalOperand - BitPrimary - probably BitPrimaryConversion\n";
         return ATtrue;
      }
   } else if (traverse_Variable(term, sg_expr, expr)) {
      // MATCHED Variable
   } else if (traverse_RelationalExpression(term, sg_expr, expr)) {
      // MATCHED RelationalExpression
   } else return ATfalse;

   ROSE_ASSERT(expr != NULL);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_BitPrimary(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BitPrimary: %s\n", ATwriteToString(term));
#endif

   ATerm t_bit;

   expr = NULL;
   if (traverse_BooleanLiteral(term, sg_expr, expr)) {
      // MATCHED BooleanLiteral
   }
   else if (ATmatch(term,"BitPrimaryParens(<term>)", &t_bit)) {
      // TODO: Add parentheses
      cerr << "WARNING UNIMPLEMENTED: BitPrimary - BitPrimaryParens\n";
      if (traverse_BitFormula(t_bit, sg_expr, expr)) {
      // MATCHED '(' BitFormula ')'
      } else return ATfalse;
   }
   else if (traverse_BitLiteral(term, sg_expr, expr)) {
      // MATCHED BitLiteral
   }
   else if (traverse_BitConversion(term, sg_expr, expr)) {
      // MATCHED BitPrimaryConversion
      if (!expr) {
         cerr << "WARNING UNIMPLEMENTED: BitPrimary - BitPrimaryConversion\n";
         return ATtrue;
      }
   }
   else return ATfalse;
      // TODO: create else if for following
      // BitVariable                   -> BitPrimary {cons("BitVariable")} (not currently working in tests)
      // NamedBitConstant              -> BitPrimary {cons("NamedBitConstant")} (rejected in grammar)
      // BitFunctionCall               -> BitPrimary (no cons)

   if (!expr) {
      cerr << "WARNING UNIMPLEMENTED: BitPrimary - possibly Dereference\n";
      return ATtrue;
   }

   ROSE_ASSERT(expr != NULL);

   return ATtrue;
}

//========================================================================================
// 5.2.1 RELATIONAL EXPRESSIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_RelationalExpression(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_RelationalExpression: %s\n", ATwriteToString(term));
#endif

   ATerm t_formula, t_operator, t_formula2;
   std::string op_name;
   General_Language_Translation::ExpressionKind op_enum;
   SgUntypedExpression * expr1, * expr2;

   expr = NULL;

// Begin SageTreeBuilder
   SgExpression *sg_expr1 = nullptr, *sg_expr2 = nullptr;

   if (ATmatch(term, "RelationalExpression(<term>,<term>,<term>)", &t_formula, &t_operator, &t_formula2)) {
      if (traverse_Formula(t_formula, sg_expr1, expr1)) {
         // MATCHED Formula
      } else return ATfalse;

      if (traverse_RelationalOperator(t_operator, op_enum, op_name)) {
         // MATCHED RelationalOperator
      } else return ATfalse;

      if (traverse_Formula(t_formula2, sg_expr2, expr2)) {
         // MATCHED Formula
      } else return ATfalse;
   } else return ATfalse;

   ROSE_ASSERT(expr1);
   if (!expr2) {
      cerr << "WARNING UNIMPLEMENTED: RelationalExpression - expr2 - maybe StatusConstant\n";
   }
   else {
      ROSE_ASSERT(expr2);   // will want to keep this assert after implementation is complete
   }

   expr = new SgUntypedBinaryOperator(op_enum, op_name, expr1, expr2);
   ROSE_ASSERT(expr);
   setSourcePosition(expr, term);

   return ATtrue;
}

//========================================================================================
// 5.3.0 GENERAL FORMULA
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_GeneralFormula(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* & expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_GeneralFormula: %s\n", ATwriteToString(term));
#endif

   ATerm t_func_const_or_var;
   char* variable;
   Jovial_ROSE_Translation::ExpressionKind expr_enum = Jovial_ROSE_Translation::e_referenceExpression;

   if (ATmatch(term, "GeneralFormula(<str>)", &variable)) {
      expr_enum = Jovial_ROSE_Translation::e_referenceExpression;
      expr = new SgUntypedReferenceExpression(expr_enum, variable);
      setSourcePosition(expr, term);
   } else if (ATmatch(term, "GeneralFormula(<term>)", &t_func_const_or_var)) {
      if (traverse_FunctionCall(t_func_const_or_var, sg_expr, expr)) {
         // MATCHED FunctionCall
      } else if (traverse_NamedConstant(t_func_const_or_var, sg_expr, expr)) {
         // MATCHED NamedConstant
      } else if (traverse_Variable(t_func_const_or_var, sg_expr, expr)) {
         // MATCHED Variable
      } else return ATfalse;
   } else if (traverse_CharacterFormula(term, sg_expr, expr)) {
      // MATCHED CharacterFormula
   } else if (traverse_StatusFormula(term, sg_expr, expr)) {
      // MATCHED StatusFormula
   } else if (traverse_PointerFormula(term, sg_expr, expr)) {
      // MATCHED PointerFormula
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 5.3 CHARACTER FORMULAS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_CharacterFormula(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CharacterFormula: %s\n", ATwriteToString(term));
#endif

   ATerm t_next , t_formula;

   expr = nullptr;
   sg_expr = nullptr;

   if (ATmatch(term, "CharacterFormula(<term>)", &t_next)) {
      if (traverse_CharacterLiteral(t_next, sg_expr, expr)) {
         // CharacterLiteral -> CharacterFormula
         // MATCHED CharacterLiteral
      } else return ATfalse;

   } else if (ATmatch(term, "CharacterFormulaParens(<term>)", &t_next)) {
      // '(' CharacterFormula ')' -> CharacterFormula
      if (traverse_CharacterFormula(t_next, sg_expr, expr)) {
         // MATCHED CharacterFormula
      } else return ATfalse;

   } else if (ATmatch(term, "CharacterFormulaConversion(<term>,<term>)", &t_next, &t_formula)) {
      if (traverse_CharacterConversion(t_next, sg_expr, expr)) {
         // CharacterConversion '(' Formula ')'  ->  CharacterFormula
         // MATCHED CharacterConversion
      } else if (traverse_CharacterConversionC(t_next, sg_expr, expr)) {
         // MATCHED CharacterConversionC
      } else return ATfalse;

      if (traverse_Formula(t_formula, sg_expr, expr)) {
         // MATCHED Formula
      }
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 5.4 STATUS FORMULAS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_StatusFormula(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_StatusFormula: %s\n", ATwriteToString(term));
#endif

   ATerm t_next , t_formula;
   SgUntypedInitializedName* initialized_name;

   if (ATmatch(term, "StatusFormula(<term>)", &t_next)) {
      if (traverse_StatusConstant(t_next, initialized_name)) {
         // StatusConstant -> StatusFormula
         // MATCHED StatusConstant

      // TODO - WARNING: FIXME: don't know what to do with this
         //         return ATfalse;
         cerr << "WARNING UNIMPLEMENTED: StatusFormula - StatusConstant\n";
      } else return ATfalse;

   } else if (ATmatch(term, "StatusFormulaParens(<term>)", &t_next)) {
      // '(' StatusFormula ')' -> StatusFormula
      if (traverse_StatusFormula(t_next, sg_expr, expr)) {
         // MATCHED StatusFormula
      } else return ATfalse;

   } else if (ATmatch(term, "StatusFormula(<term>,<term>)", &t_next, &t_formula)) {
      if (traverse_StatusConversion(t_next, sg_expr, expr)) {
         // StatusConversion '(' Formula ')'  ->  StatusFormula
         // MATCHED StatusConversion
      }
      if (traverse_Formula(t_formula, sg_expr, expr)) {
         // MATCHED Formula
      }
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 5.5 POINTER FORMULAS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_PointerFormula(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_PointerFormula: %s\n", ATwriteToString(term));
#endif

   ATerm t_next, t_formula;

   expr = NULL;

   if (ATmatch(term, "PointerFormula(<term>)", &t_next)) {
      if (traverse_PointerLiteral(t_next, sg_expr, expr)) {
         // PointerLiteral -> PointerFormula
         // MATCHED PointerLiteral
      } else return ATfalse;

   } else if (ATmatch(term, "PointerFormulaParens(<term>)", &t_next)) {
      // '(' PointerFormula ')' -> PointerFormula
      if (traverse_PointerFormula(t_next, sg_expr, expr)) {
         // MATCHED PointerFormula
      } else return ATfalse;

   } else if (ATmatch(term, "PointerFormulaConversion(<term>,<term>)", &t_next, &t_formula)) {
      if (traverse_PointerConversion(t_next, sg_expr, expr)) {
         // PointerConversion '(' Formula ')'  -> PointerFormula
         // MATCHED PointerConversion
      } else if (traverse_PointerConversionP(t_next, sg_expr, expr)) {
         // MATCHED PointerConversionP
      } else return ATfalse;

      if (traverse_Formula(t_formula, sg_expr, expr)) {
         // MATCHED Formula
      }
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 6.1 VARIABLE AND BLOCK REFERENCES
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_Variable(ATerm term, SgExpression* &sg_var, SgUntypedExpression* &var)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Variable: %s\n", ATwriteToString(term));
#endif

   char* name;
   Jovial_ROSE_Translation::ExpressionKind expr_enum = Jovial_ROSE_Translation::e_referenceExpression;

   if (ATmatch(term, "<str>" , &name)) {
      // MATCHED NamedVariable
      var = new SgUntypedReferenceExpression(expr_enum, name);
      setSourcePosition(var, term);
      // SageTreeBuilder
      sg_var = SageBuilder::buildVarRefExp(name, SageBuilder::topScopeStack());
      setSourcePosition(sg_var, term);
   } else if (traverse_Dereference(term, sg_var, var)) {
      // MATCHED ItemDereference/TableDereference -> Item/Table -> NamedVariable
   } else if (traverse_TableItem(term, sg_var, var)) {
      // MATCHED TableItem
   } else if (traverse_BitFunctionVariable(term, sg_var, var)) {
      // MATCHED BitFunctionVariable
   } else if (traverse_ByteFunctionVariable(term, sg_var, var)) {
      // MATCHED ByteFunctionVariable
   }
   else return ATfalse;

   //  RepFunctionVariable         -> Variable           {cons("RepFunctionVariable")}

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_VariableList(ATerm term, std::vector<SgExpression*> &sg_vars, std::vector<SgUntypedExpression*> &vars)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_VariableList: %s\n", ATwriteToString(term));
#endif

   ATerm t_labels;
   SgUntypedExpression* var;

// Begin SageTreeBuilder
   SgExpression* sg_var;

   if (ATmatch(term, "VariableList(<term>)" , &t_labels)) {
      ATermList tail = (ATermList) ATmake("<term>", t_labels);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);

         if (traverse_Variable(head, sg_var, var)) {
            vars.push_back(var);
            sg_vars.push_back(sg_var);
         } else return ATfalse;
      }
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_TableItem(ATerm term, SgExpression* &sg_var, SgUntypedExpression* &var)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_TableItem: %s\n", ATwriteToString(term));
#endif

   ATerm t_name, t_subscript, t_tblderef;
   char* name;
   std::vector<SgUntypedExpression*> subscript;
   SgUntypedExprListExpression* array_subscripts;

   var = NULL;

   if (ATmatch(term, "TableItem(<term>,<term>,<term>)" , &t_name, &t_subscript, &t_tblderef)) {
      if (ATmatch(t_name, "<str>" , &name)) {
         // MATCHED TableItemName
      // need reference expression
      } else return ATfalse;

      if (traverse_Subscript(t_subscript, subscript)) {
         // MATCHED Subscript
         if (subscript.size() > 1) {
            cerr << "WARNING UNIMPLEMENTED: TableItem - subscript.size() > 1 not fully implemented\n";
         }

         if (subscript.size() > 0) {
         // TODO - convert to SgUntypedExprListExpression
         ROSE_ASSERT(subscript.size() > 0);
         //         ROSE_ASSERT(subscript.size() == 1);
         ROSE_ASSERT(subscript[0]);

         array_subscripts = new SgUntypedExprListExpression(General_Language_Translation::e_array_subscripts);
         ROSE_ASSERT(array_subscripts);
         setSourcePosition(array_subscripts, term);

         array_subscripts->get_expressions().push_back(subscript[0]);

#if 0
         cout << ".x. found subscript # is " << subscript.size() << ": subscript is " << subscript[0] << endl;
#endif

         }
         else {
            cerr << "WARNING UNIMPLEMENTED: TableItem - has a subscript with size " << subscript.size() << std::endl;
            SgUntypedExpression* array_subscripts = UntypedBuilder::buildUntypedNullExpression();
            ROSE_ASSERT(array_subscripts);
         }

         SgUntypedExpression* coarray_subscripts = UntypedBuilder::buildUntypedNullExpression();
         ROSE_ASSERT(coarray_subscripts);

         int expr_enum = General_Language_Translation::e_array_reference;
         var = new SgUntypedArrayReferenceExpression(expr_enum, name, array_subscripts, coarray_subscripts);
         ROSE_ASSERT(var);
         setSourcePosition(var, term);

      } else return ATfalse;

      if (traverse_TableDereference(t_tblderef, sg_var, var)) {
         // MATCHED TableDereference
      } else return ATfalse;

   } else return ATfalse;

   ROSE_ASSERT(var);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_Subscript(ATerm term, std::vector<SgUntypedExpression*> & indexes)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Subscript: %s\n", ATwriteToString(term));
#endif

   ATerm t_index;
   SgUntypedExpression* index;

// Begin SageTreeBuilder
   SgExpression* sg_index;

   if (ATmatch(term, "no-subscript")) {
      // MATCHED no-subscript
   } else if (ATmatch(term, "Subscript(<term>)" , &t_index)) {
      ATermList tail = (ATermList) ATmake("<term>", t_index);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_Index(head, sg_index, index)) {
            // MATCHED Index
            indexes.push_back(index);
         } else return ATfalse;
      }
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_Index(ATerm term, SgExpression* &sg_formula, SgUntypedExpression* &formula)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Index: %s\n", ATwriteToString(term));
#endif

   if (traverse_NumericFormula(term, sg_formula, formula)) {
      // MATCHED NumericFormula
   } else if (traverse_StatusFormula(term, sg_formula, formula)) {
      // MATCHED StatusFormula
   } else return ATfalse;

   return ATtrue;
}


ATbool ATermToSageJovialTraversal::traverse_TableDereference(ATerm term, SgExpression* &sg_formula, SgUntypedExpression* &formula)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_TableDereference: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "no-table-dereference")) {
      // MATCHED no-table-dereference
   } else if (traverse_Dereference(term, sg_formula, formula)) {
      // MATCHED Dereference
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_Dereference(ATerm term, SgExpression* &sg_formula, SgUntypedExpression* &formula)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Dereference: %s\n", ATwriteToString(term));
#endif

   ATerm t_deref;
   char* name;
   //   SgUntypedExpression* formula;

   if (ATmatch(term, "Dereference(<term>)", &t_deref)) {
      if (ATmatch(t_deref, "<str>", &name)) {
         cerr << "WARNING UNIMPLEMENTED: Dereference -> PointerItemName\n";
         // MATCHED PointerItemName
      } else if (traverse_GeneralFormula(t_deref, sg_formula, formula)) {
         // MATCHED PointerFormula through GeneralFormula
      } else return ATfalse;
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_BitFunctionVariable(ATerm term, SgExpression* &sg_var, SgUntypedExpression* &var)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BitFunctionVariable: %s\n", ATwriteToString(term));
#endif

   ATerm t_bitvar, t_var, t_fbit, t_nbit, t_fbit_num, t_nbit_num;
   SgUntypedExpression * fbit, * nbit;
   SgExpression * sg_fbit, * sg_nbit;

   //  'BIT' '(' BitVariable ',' Fbit ',' Nbit ')' -> BitFunctionVariable   {cons("BitFunctionVariable"), prefer}
   //  'BIT' '(' BitFormula ','  Fbit ',' Nbit ')' -> BitFunctionVariable   {cons("BitFunctionVariable")}

   if (ATmatch(term, "BitFunctionVariable(<term>,<term>,<term>)", &t_bitvar, &t_fbit, &t_nbit)) {
      cerr << "WARNING UNIMPLEMENTED: BitFunctionVariable\n";

      if (ATmatch(t_bitvar, "BitVariable(<term>)", &t_var)) {
         if (traverse_Variable(t_var, sg_var, var)) {
            // MATCHED BitVariable -> Variable
         } else return ATfalse;
      } else if (traverse_BitFormula(t_bitvar, sg_var, var)) {
            // MATCHED BitFormula
      } else return ATfalse;

      if (ATmatch(t_fbit, "Fbit(<term>)", &t_fbit_num)) {
         if (traverse_NumericFormula(t_fbit_num, sg_fbit, fbit)) {
            // MATCHED NumericFormula
         } else return ATfalse;
      } else return ATfalse;

      if (ATmatch(t_nbit, "Nbit(<term>)", &t_nbit_num)) {
         if (traverse_NumericFormula(t_nbit_num, sg_nbit, nbit)) {
            // MATCHED NumericFormula
         } else return ATfalse;
      } else return ATfalse;
   }

   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_ByteFunctionVariable(ATerm term, SgExpression* &sg_var, SgUntypedExpression* &var)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ByteFunctionVariable: %s\n", ATwriteToString(term));
#endif

   ATerm t_var, t_fbit, t_nbit, t_fbit_num, t_nbit_num;
   SgUntypedExpression * fbit, * nbit;
   SgExpression *sg_fbit, *sg_nbit;

   //  'BYTE' '(' Variable ',' Fbit ',' Nbit ')' -> ByteFunctionVariable   {cons("ByteFunctionVariable")}

   if (ATmatch(term, "ByteFunctionVariable(<term>,<term>,<term>)", &t_var, &t_fbit, &t_nbit)) {
      cerr << "WARNING UNIMPLEMENTED: ByteFunctionVariable\n";
      if (traverse_Variable(t_var, sg_var, var)) {
         // MATCHED Variable
      } else return ATfalse;

      if (ATmatch(t_fbit, "Fbit(<term>)", &t_fbit_num)) {
         if (traverse_NumericFormula(t_fbit_num, sg_fbit, fbit)) {
            // MATCHED NumericFormula
         } else return ATfalse;
      } else return ATfalse;

      if (ATmatch(t_nbit, "Nbit(<term>)", &t_nbit_num)) {
         if (traverse_NumericFormula(t_nbit_num, sg_nbit, nbit)) {
            // MATCHED NumericFormula
         } else return ATfalse;
      } else return ATfalse;
   }

   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 6.2 NAMED CONSTANTS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_NamedConstant(ATerm term, SgExpression* &sg_var, SgUntypedExpression* &var)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_NamedConstant: %s\n", ATwriteToString(term));
#endif

   char* letter;

   if (ATmatch(term, "ControlLetter(<str>)" , &letter)) {
      // MATCHED ControlLetter
      cerr << "WARNING UNIMPLEMENTED: NamedConstant - ControlLetter " << letter << endl;
   } else return ATfalse;

      //  ConstantItemName            -> NamedConstant         {prefer}  %% ambiguous with ConstantTableName
      //  ConstantTableName           -> NamedConstant         {cons("ConstantTableName")}
      //  ConstantTableName Subscript -> NamedConstant         {cons("NamedConstant")}

   return ATtrue;
}

//========================================================================================
// 6.3 FUNCTION CALLS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_FunctionCall(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FunctionCall: %s\n", ATwriteToString(term));
#endif

   if (traverse_UserDefinedFunctionCall(term, sg_expr, expr)) {
      // MATCHED UserDefinedFunctionCall
   }
   else if (traverse_IntrinsicFunctionCall(term, sg_expr, expr)) {
      // MATCHED IntrinsicFunctionCall
   } else return ATfalse;

   //   MachineSpecificFunctionCall -> FunctionCall

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_UserDefinedFunctionCall(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_UserDefinedFunctionCall: %s\n", ATwriteToString(term));
#endif

   ATerm t_name, t_arg_list;
   std::string name;
   SgUntypedExprListExpression* arg_list = NULL;

   if (ATmatch(term, "UserDefinedFunctionCall(<term>,<term>)", &t_name, &t_arg_list)) {
      cerr << "WARNING UNIMPLEMENTED: UserDefinedFunctionCall\n";
      if (traverse_Name(t_name, name)) {
         // MATCHED FunctionName
      } else return ATfalse;

      arg_list = new SgUntypedExprListExpression(General_Language_Translation::e_argument_list);
      ROSE_ASSERT(arg_list);
      setSourcePosition(arg_list, t_arg_list);

      if (traverse_ActualParameterList(t_arg_list, arg_list)) {
         // MATCHED ActualParameterList
      } else return ATfalse;
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_IntrinsicFunctionCall(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_IntrinsicFunctionCall: %s\n", ATwriteToString(term));
#endif

   if (traverse_StatusInverseFunction(term, sg_expr, expr)) {
      // MATCHED StatusInverseFunction
   }
   else if (traverse_LocFunction(term, sg_expr, expr)) {
      // MATCHED LocFunction
   }
   else if (traverse_NextFunction(term, sg_expr, expr)) {
      // MATCHED NextFunction
   }
   else if (traverse_ByteFunction(term, sg_expr, expr)) {
      // MATCHED ByteFunction
   }

   //   BitFunction                 -> IntrinsicFunctionCall
   //   ShiftFunction               -> IntrinsicFunctionCall
   //   AbsFunction                 -> IntrinsicFunctionCall
   //   SignFunction                -> IntrinsicFunctionCall

   else if (traverse_SizeFunction(term, sg_expr, expr)) {
      // MATCHED SizeFunction
   }

   //   BoundsFunction              -> IntrinsicFunctionCall
   //   NwdsenFunction              -> IntrinsicFunctionCall
   //   NentFunction                -> IntrinsicFunctionCall

   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 6.3.1 LOC FUNCTIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_LocFunction(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_LocFunction: %s\n", ATwriteToString(term));
#endif

   ATerm t_argument;
   std::string loc_arg_str;
   SgUntypedExpression* loc_arg_expr;
   SgExpression* sg_loc_arg_expr;

   if (ATmatch(term, "LocFunction(<term>)", &t_argument)) {
      cerr << "WARNING UNIMPLEMENTED: LocFunction\n";
      if (traverse_Name(t_argument, loc_arg_str)) {
         // MATCHED LocArgument
      }
      else if (traverse_Variable(t_argument, sg_loc_arg_expr, loc_arg_expr)) {
         // MATCHED NamedVariable -> Variable
      } else return ATfalse;
   } else return ATfalse;

   //  BlockReference              -> LocArgument

   return ATtrue;
}

//========================================================================================
// 6.3.2 NEXT FUNCTIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_NextFunction(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_NextFunction: %s\n", ATwriteToString(term));
#endif

   ATerm t_argument, t_increment;
   SgUntypedExpression * next_arg, * increment;
   SgExpression * sg_next_arg, * sg_increment;

   if (ATmatch(term, "NextFunction(<term>, <term>)", &t_argument, &t_increment)) {
      cerr << "WARNING UNIMPLEMENTED: NextFunction\n";
      if (traverse_GeneralFormula(t_argument, sg_next_arg, next_arg)) {
         // MATCHED GeneralFormula
      }
      else if (traverse_StatusFormula(t_argument, sg_next_arg, next_arg)) {
         // MATCHED StatusFormula
      } else return ATfalse;

      if (traverse_NumericFormula(t_increment, sg_increment, increment)) {
         // MATCHED NumericFormula
      } else return ATfalse;
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 6.3.4 BYTE FUNCTIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_ByteFunction(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ByteFunction: %s\n", ATwriteToString(term));
#endif

   ATerm t_formula, t_fbyte, t_nbyte;
   SgUntypedExpression * formula, * fbyte, * nbyte;
   SgExpression * sg_formula, * sg_fbyte, * sg_nbyte;

   if (ATmatch(term, "ByteFunction(<term>, <term>,<term>)", &t_formula, &t_fbyte, &t_nbyte)) {
      cerr << "WARNING UNIMPLEMENTED: ByteFunction\n";
      if (traverse_CharacterFormula(t_formula, sg_formula, formula)) {
         // MATCHED CharacterFormula
      } else return ATfalse;

      if (traverse_NumericFormula(t_fbyte, sg_fbyte, fbyte)) {
         // MATCHED NumericFormula
      } else return ATfalse;

      if (traverse_NumericFormula(t_nbyte, sg_nbyte, nbyte)) {
         // MATCHED NumericFormula
      } else return ATfalse;
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 6.3.7 SIZE FUNCTIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_SizeFunction(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SizeFunction: %s\n", ATwriteToString(term));
#endif

   ATerm t_formula;
   SgUntypedExpression* formula;
   SgExpression* sg_formula;

   if (ATmatch(term, "SizeFunction(BITSIZE(),<term>)", &t_formula)) {
      cerr << "WARNING UNIMPLEMENTED: SizeFunction - BITSIZE \n";
      if (traverse_Formula(t_formula, sg_formula, formula)) {
         // MATCHED Formula
      } else return ATfalse;
   }
   else if (ATmatch(term, "SizeFunction(BYTESIZE(),<term>)", &t_formula)) {
      cerr << "WARNING UNIMPLEMENTED: SizeFunction - BYTESIZE \n";
      if (traverse_Formula(t_formula, sg_formula, formula)) {
         // MATCHED Formula
      } else return ATfalse;
   }
   else if (ATmatch(term, "SizeFunction(WORDSIZE(),<term>)", &t_formula)) {
      cerr << "WARNING UNIMPLEMENTED: SizeFunction - WORDSIZE \n";
      if (traverse_Formula(t_formula, sg_formula, formula)) {
         // MATCHED Formula
      } else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 6.3.11 STATUS INVERSE FUNCTIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_StatusInverseFunction(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_StatusInverseFunction: %s\n", ATwriteToString(term));
#endif

   ATerm t_argument;
   SgUntypedExpression* argument;
   SgExpression* sg_argument;
   std::string name;

   if (ATmatch(term, "StatusInverseFunctionFIRST(<term>)", &t_argument)) {
      cerr << "WARNING UNIMPLEMENTED: StatusInverseFunctionFIRST\n";
      if (traverse_StatusFormula(t_argument, sg_argument, argument)) {
         // MATCHED StatusFormula
      }
      else if (traverse_Name(t_argument, name)) {
         // MATCHED StatusTypeName
      }
      else return ATfalse;
   }
   else if (ATmatch(term, "StatusInverseFunctionLAST(<term>)", &t_argument)) {
      cerr << "WARNING UNIMPLEMENTED: StatusInverseFunctionLAST\n";
      if (traverse_StatusFormula(t_argument, sg_argument, argument)) {
         // MATCHED StatusFormula
      }
      else if (traverse_Name(t_argument, name)) {
         // MATCHED StatusTypeName
      }
      else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 7.0 TYPE MATCHING AND TYPE CONVERSIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_BitConversion(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BitConversion: %s\n", ATwriteToString(term));
#endif

   ATerm t_conv, t_formula, t_bit_type_desc;
   SgUntypedExpression *formula;
   SgExpression *sg_formula;
   std::string bit_type_name;

   if (ATmatch(term, "BitPrimaryConversion(<term>,<term>)", &t_conv, &t_formula)) {
      cerr << "WARNING UNIMPLEMENTED: BitPrimaryConversion\n";
      if (ATmatch(t_conv, "BitTypeConversion(<term>)", &t_bit_type_desc)) {
         // MATCHED BitTypeConversion
         cerr << "WARNING UNIMPLEMENTED: BitTypeConversion\n";
      } else if (ATmatch(t_conv, "BitTypeConversionB()")) {
         // MATCHED BitTypeConversionB
         cerr << "WARNING UNIMPLEMENTED: BitTypeConversion - B\n";
      } else if (traverse_Name(t_conv, bit_type_name)) {
         // MATCHED BitTypeName
         cerr << "WARNING UNIMPLEMENTED: BitTypeConversion - BitTypeName \n";
      } else return ATfalse;

      if (traverse_Formula(t_formula, sg_formula, formula)) {
         // MATCHED Formula
      } else return ATfalse;
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_IntegerConversion(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_IntegerConversion: %s\n", ATwriteToString(term));
#endif

   ATerm t_next;
   SgType* sg_type = nullptr;
   SgUntypedExprListExpression* attr_list = NULL;
   std::string type_name;

   expr = NULL;

   if (ATmatch(term, "IntegerConversion(<term>)", &t_next)) {

      attr_list = new SgUntypedExprListExpression();
      ROSE_ASSERT(attr_list);
      setSourcePosition(attr_list, t_next);

      if (traverse_IntegerItemDescription(t_next, sg_type, attr_list)) {
         // MATCHED IntegerItemDescription
      } else return ATfalse;

#if 0
      // Should be IntegerTypeDescription
      // No traversal of this yet
      // In Main.sdf, prefer on IntegerItemDescription -> IntegerTypeDescription
      if (traverse_IntegerTypeDescription(t_next, type)) {
         // MATCHED IntegerTypeDescription
      } else return ATfalse;
#endif
   } else if (ATmatch(term, "IntegerConversionS()")) {
      // MATCHED IntegerConversionS
   } else if (ATmatch(term, "IntegerConversionU()")) {
      // MATCHED IntegerConversionU
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_GeneralConversion(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_GeneralConversion: %s\n", ATwriteToString(term));
#endif

   ATerm t_next;
   std::string type_name;
   SgUntypedType* type;

   if (ATmatch(term, "GeneralConversion(<term>)", &t_next)) {
      cerr << "WARNING UNIMPLEMENTED: GeneralConversion\n";
      // MATCHED GeneralConversion
      if (traverse_OptTypeName(t_next, type, type_name)) {
         // MATCHED TypeName
      } else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_FloatingConversion(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FloatingConversion: %s\n", ATwriteToString(term));
#endif

   ATerm t_next;
   SgUntypedType* type; // TODO - QUICK DO SOMETHING!
   SgType* sg_type = nullptr;
   SgUntypedExprListExpression* attr_list = NULL;

   expr = NULL;

   if (ATmatch(term, "FloatingConversion(<term>)", &t_next)) {

      attr_list = new SgUntypedExprListExpression();
      ROSE_ASSERT(attr_list);
      setSourcePosition(attr_list, t_next);

      if (traverse_FloatingItemDescription(t_next, sg_type, type, attr_list)) {
         // MATCHED FloatingItemDescription
      } else return ATfalse;

#if 0
      // Should be FloatingTypeDescription
      // No traversal of this yet
      // In Main.sdf, prefer on FloatingItemDescription -> FloatingTypeDescription
      if (traverse_FloatingTypeDescription(t_next, type)) {
         // MATCHED FloatingTypeDescription
      } else return ATfalse;
#endif

   } else if (ATmatch(term, "FloatingConversionF()")) {
      // MATCHED FloatingConversionF
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_FixedConversion(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FixedConversion: %s\n", ATwriteToString(term));
#endif

   ATerm t_next;
   SgUntypedType* type; // TODO - QUICK DO SOMETHING!
   SgUntypedExprListExpression* attr_list = NULL;

   expr = NULL;
   sg_expr = nullptr;

   if (ATmatch(term, "FixedConversion(<term>)", &t_next)) {

      attr_list = new SgUntypedExprListExpression();
      ROSE_ASSERT(attr_list);
      setSourcePosition(attr_list, t_next);

      if (traverse_FixedItemDescription(t_next, type, attr_list)) {
         // MATCHED FixedItemDescription
      } else return ATfalse;
   } else return ATfalse;

#if 0
      // Should be FixedTypeDescription
      // No traversal of this yet
      // In Main.sdf, prefer on FixedItemDescription -> FixedTypeDescription
      if (traverse_FixedTypeDescription(t_next, type)) {
         // MATCHED FixedTypeDescription
      } else return ATfalse;
#endif

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_CharacterConversion(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CharacterConversion: %s\n", ATwriteToString(term));
#endif

   ATerm t_next;
   SgUntypedType* type; // TODO - QUICK DO SOMETHING!

   expr = NULL;

   if (ATmatch(term, "CharacterConversion(<term>)", &t_next)) {
      if (traverse_CharacterItemDescription(t_next, type)) {
         // MATCHED CharacterItemDescription
      } else return ATfalse;
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_CharacterConversionC(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CharacterConversionC: %s\n", ATwriteToString(term));
#endif

   expr = NULL;

   if (ATmatch(term, "CharacterConversionC()")) {
     std::cout << "Matched CharacterConversionC" << endl;
     // MATCHED CharacterConversionC
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_StatusConversion(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_StatusConversion: %s\n", ATwriteToString(term));
#endif

   ATerm t_next;
   bool has_size;
   SgUntypedExpression* status_size;
   SgUntypedInitializedNameList* status_list;

   expr = NULL;

   if (ATmatch(term, "StatusConversion(<term>)", &t_next)) {
      cerr << "WARNING UNIMPLEMENTED: StatusConversion \n";

      if (traverse_StatusItemDescription(t_next, status_list, has_size, status_size)) {
         // MATCHED StatusItemDescription
      } else return ATfalse;

   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_PointerConversion(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_PointerConversion: %s\n", ATwriteToString(term));
#endif

   ATerm t_next;
   SgUntypedType* type; // TODO - QUICK DO SOMETHING!

   expr = NULL;

   if (ATmatch(term, "PointerConversion(<term>)", &t_next)) {
      if (traverse_PointerItemDescription(t_next, type)) {
         // MATCHED PointerItemDescription
      } else return ATfalse;
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_PointerConversionP(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_PointerConversionP: %s\n", ATwriteToString(term));
#endif

   expr = NULL;

   if (ATmatch(term, "PointerConversionP()")) {
     std::cout << "Matched PointerConversionP" << endl;
     // MATCHED PointerConversionP
   } else return ATfalse;

   return ATtrue;
}


//========================================================================================
// 8.2.3 OPERATORS
//----------------------------------------------------------------------------------------

ATbool
ATermToSageJovialTraversal::traverse_MultiplyDivideOrMod(ATerm term, General_Language_Translation::ExpressionKind & op_enum, std::string & op_name)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_MultiplyDivideOrMod: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "MultiplyOp()")) {
      op_enum = General_Language_Translation::e_operator_multiply;
      op_name = "*";
   }
   else if (ATmatch(term, "DivideOp()")) {
      op_enum = General_Language_Translation::e_operator_divide;
      op_name = "/";
   }
   else if (ATmatch(term, "ModOp()")) {
      op_enum = General_Language_Translation::e_operator_mod;
      op_name = "MOD";
   }
   else {
      op_enum = General_Language_Translation::e_unknown;
      op_name = "Jovial_operator_unknown";
      return ATfalse;
   }

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_RelationalOperator(ATerm term, General_Language_Translation::ExpressionKind & op_enum, std::string & op_name)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_RelationalOperator: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "LessThanOp()")) {
      op_enum = General_Language_Translation::e_operator_less_than;
      op_name = "<";
   }
   else if (ATmatch(term, "GreaterThanOp()")) {
      op_enum = General_Language_Translation::e_operator_greater_than;
      op_name = ">";
   }
   else if (ATmatch(term, "LessOrEqualOp()")) {
      op_enum = General_Language_Translation::e_operator_less_than_or_equal;
      op_name = "<=";
   }
   else if (ATmatch(term, "GreaterOrEqualOp()")) {
      op_enum = General_Language_Translation::e_operator_greater_than_or_equal;
      op_name = ">=";
   }
   else if (ATmatch(term, "EqualityOp()")) {
      op_enum = General_Language_Translation::e_operator_equality;
      op_name = "=";
   }
   else if (ATmatch(term, "NotEqualOp()")) {
      op_enum = General_Language_Translation::e_operator_not_equal;
      op_name = "<>";
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 8.3.1 NUMERIC LITERAL
//----------------------------------------------------------------------------------------

ATbool ATermToSageJovialTraversal::traverse_FixedOrFloatingLiteral(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FixedOrFloatingLiteral: %s\n", ATwriteToString(term));
#endif

   ATerm t_frac_form, t_num1, t_num2, t_opt_exp;
   std::string literal, opt_exp;
   char* number;

   SgUntypedType* type;
   Jovial_ROSE_Translation::ExpressionKind expr_enum = Jovial_ROSE_Translation::e_literalExpression;

// FractionalForm OptExponent -> RealLiteral
   if (ATmatch(term, "RealLiteralFF(<term>,<term>)", &t_frac_form, &t_opt_exp)) {

      if (ATmatch(t_frac_form, "FractionalForm(<term>,<term>)", &t_num1, &t_num2)) {

         // NOTE: reconstruct the integer, ".", fractional part, exponent into one string

         // TODO - fix this
         cerr << "WARNING UNIMPLEMENTED: FixedOrFloatingLiteral, RealLiteralFF \n";
         literal = "13.13";

#if 0
         // integer part
         if (ATmatch(t_num1, "no-number()")) {
            // no integer part
         }
         else if (ATmatch(t_num1, "<str>", &number)) {
            literal += number;
         }
         else return ATfalse;

         // fractional part
         if (ATmatch(t_num2, "no-number()")) {
            // no fractional part
            literal += ".";
         }
         else if (ATmatch(t_num2, "<str>", &number)) {
            literal += ".";
            literal += number;
         }
         else return ATfalse;

         // optional exponent
         if (ATmatch(t_opt_exp, "no-exponent()")) {
            // no exponent
         }

         else if (traverse_Exponent(t_opt_exp, opt_exp)) {
            literal += opt_exp;
         }
#endif

      }

      if (literal == ".") {
         cerr << "ERROR in traverse_FixedOrFloatingLiteral, no literal, contains only: " << literal << endl;
         return ATfalse;
      }

      type = UntypedBuilder::buildType(SgUntypedType::e_float);
      expr = new SgUntypedValueExpression(expr_enum,literal,type);
      setSourcePosition(expr, term);
   }

   else if (ATmatch(term, "RealLiteralIE(<term>,<term>)", &t_num1, &t_opt_exp)) {
      if (ATmatch(t_num1, "<str>", &number)) {
         literal += number;
      } else return ATfalse;

      if (traverse_Exponent(t_opt_exp, opt_exp)) {
         literal += opt_exp;
      } else return ATfalse;

      type = UntypedBuilder::buildType(SgUntypedType::e_float);
      expr_enum = Jovial_ROSE_Translation::e_literalExpression;
      expr = new SgUntypedValueExpression(expr_enum,literal,type);
      setSourcePosition(expr, term);
   }

   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_Exponent(ATerm term, std::string & opt_exp)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Exponent: %s\n", ATwriteToString(term));
#endif

   ATerm t_sign, t_num;
   char* number;


   if (ATmatch(term, "Exponent(<term>,<term>)", &t_sign, &t_num)) {
      General_Language_Translation::ExpressionKind op_enum;
      if (traverse_OptSign(t_sign, op_enum)) {
         // MATCHED OptSign
      } else return ATfalse;

      if (op_enum == General_Language_Translation::e_operator_unary_minus) {
         opt_exp += "E";
         opt_exp += "-";
      }
      else if (op_enum == General_Language_Translation::e_operator_unary_plus) {
         opt_exp += "E";
         opt_exp += "+";
      }
      else if (op_enum == General_Language_Translation::e_operator_unity) {
         opt_exp += "E";
      }

      if (ATmatch(t_num, "<str>", &number)) {
         opt_exp += number;
      } else return ATfalse;
   }

   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 8.3.2 BIT LITERAL
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_BitLiteral(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BitLiteral: %s\n", ATwriteToString(term));
#endif

   //  BeadSize 'B' "'" Bead+ "'"  -> BitLiteral              {cons("BitLiteral")}

   // from JovialLex.sdf
   //  [1-5]                     -> BeadSize
   //  [A-V]                     -> Bead

   ATerm t_bead_size, t_bead;
   char * bead_size, *bead;
   std::string literal = "";

   if (ATmatch(term, "BitLiteral(<term>,<term>)", &t_bead_size, &t_bead)) {
      if (ATmatch(t_bead_size, "<str>", &bead_size)) {
         // MATCHED BeadSize
         literal += bead_size;
      } else return ATfalse;

      literal += "B'";

      ATermList tail = (ATermList) ATmake("<term>", t_bead);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (ATmatch(head, "<str>", &bead)) {
            // MATCHED Bead
            literal += bead;
         } else return ATfalse;
      }

      literal += "'";
      SgUntypedType *type = UntypedBuilder::buildType(SgUntypedType::e_bit);
      Jovial_ROSE_Translation::ExpressionKind expr_enum = Jovial_ROSE_Translation::e_literalExpression;
      expr = new SgUntypedValueExpression(expr_enum, literal, type);
      setSourcePosition(expr, term);

   } else return ATfalse;

   ROSE_ASSERT(expr);

   return ATtrue;
}

//========================================================================================
// 8.3.3 BOOLEAN LITERAL
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_BooleanLiteral(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BooleanLiteral: %s\n", ATwriteToString(term));
#endif

   std::string value;

   expr = NULL;
   sg_expr = nullptr;

   if (ATmatch(term, "True()")) {
      // MATCHED TRUE
      value += "TRUE";
   } else if (ATmatch(term, "False()")) {
      // MATCHED FALSE
      value += "FALSE";
   } else return ATfalse;

   int expression_enum = General_Language_Translation::e_literalExpression;
   SgUntypedType* type = UntypedBuilder::buildType(SgUntypedType::e_bool);

   expr = new SgUntypedValueExpression(expression_enum, value, type);
   ROSE_ASSERT(expr != NULL);
   setSourcePosition(expr, term);

   return ATtrue;
}

//========================================================================================
// 8.3.4 POINTER LITERAL
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_PointerLiteral(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_PointerLiteral: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "Null()")) {
     // MATCHED Null
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 9.0 DIRECTIVES
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_DirectiveList(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_DirectiveList: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "[]")) {
      // Matched an empty list
      return ATtrue;
   }

// At this point there must be a non-empty list to succeed
//
   ATermList tail = (ATermList) ATmake("<term>", term);
   if (! ATisEmpty(tail)) {
      // found a non-empty list
   } else return ATfalse;

   while (! ATisEmpty(tail)) {
      ATerm head = ATgetFirst(tail);
      tail = ATgetNext(tail);
      if (traverse_Directive(head, decl_list)) {
         // MATCHED Directive
      } else return ATfalse;
   }

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_Directive(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Directive: %s\n", ATwriteToString(term));
#endif

   if (traverse_CompoolDirective(term, decl_list)) {
      // MATCHED CompoolDirective
   }
   else if (traverse_OrderDirective(term, decl_list)) {
      // MATCHED OrderDirective
   }
   else if (traverse_ReducibleDirective(term, decl_list)) {
      // MATCHED ReducibleDirective
   }
   else return ATfalse;

   return ATtrue;

//  CopyDirective            -> Directive
//  SkipDirective            -> Directive
//  BeginDirective           -> Directive
//  EndDirective             -> Directive
//  LinkageDirective         -> Directive
//  TraceDirective           -> Directive
//  InterferenceDirective    -> Directive
//  NolistDirective          -> Directive
//  ListDirective            -> Directive
//  EjectDirective           -> Directive
//  ListinvDirective         -> Directive
//  ListexpDirective         -> Directive
//  ListbothDirective        -> Directive
//  BaseDirective            -> Directive
//  IsbaseDirective          -> Directive
//  DropDirective            -> Directive
//  LeftrightDirective       -> Directive
//  RearrangeDirective       -> Directive
//  InitializeDirective      -> Directive

}

//========================================================================================
// 9.1 COMPOOL DIRECTIVES
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_CompoolDirective(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CompoolDirective: %s\n", ATwriteToString(term));
#endif

   ATerm t_dir_list, t_file_name, t_decl_name;
   SgUntypedExpression* file_name = NULL;
   SgExpression* sg_file_name = nullptr;
   std::string decl_name, directive_string;

   if (ATmatch(term, "CompoolDirective(<term>)", &t_dir_list)) {

      if (ATmatch(t_dir_list, "CompoolDirectiveList(<term>)", &t_file_name)) {
         if (ATmatch(t_file_name, "no-compool-file-name")) {
            // MATCHED no-compool-file-name
         }
         else if (traverse_CharacterLiteral(t_file_name, sg_file_name, file_name)) {
            //  '(' OptCompoolFileName ')'    -> CompoolDirectiveList     {cons("CompoolDirectiveList")}
         } else return ATfalse;
      }
      else if (ATmatch(t_dir_list, "CompoolDirectiveList(<term>, <term>)", &t_file_name, &t_decl_name)) {
         if (ATmatch(t_file_name, "no-compool-file-name")) {
            // MATCHED no-compool-file-name
         }
         else if (traverse_CharacterLiteral(t_file_name, sg_file_name, file_name)) {
            //  '(' OptCompoolFileName ')'    -> CompoolDirectiveList     {cons("CompoolDirectiveList")}
         } else return ATfalse;

         ATermList tail = (ATermList) ATmake("<term>", t_decl_name);
         while (! ATisEmpty(tail)) {
            ATerm head = ATgetFirst(tail);
            tail = ATgetNext(tail);
            if (traverse_Name(head, decl_name)) {
               directive_string = decl_name;
            } else return ATfalse;
         }
      } else return ATfalse;
   }

   else return ATfalse;

   if (file_name != NULL) {
      SgUntypedReferenceExpression* file_string = isSgUntypedReferenceExpression(file_name);
      ROSE_ASSERT(file_string);
      directive_string = file_string->get_name();
      delete file_string;
   }

   int stmt_enum = Jovial_ROSE_Translation::e_compool_directive_stmt;
   SgUntypedDirectiveDeclaration* compool_directive = new SgUntypedDirectiveDeclaration(stmt_enum, directive_string);
   ROSE_ASSERT(compool_directive);
   setSourcePosition(compool_directive, term);

   decl_list->get_decl_list().push_back(compool_directive);

   return ATtrue;
}

//========================================================================================
// 9.6 REDUCIBLE DIRECTIVES
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_ReducibleDirective(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ReducibleDirective: %s\n", ATwriteToString(term));
#endif

   std::string directive_string = "";

   if (ATmatch(term, "ReducibleDirective()")) {
      // MATCHED ReducibleDirective
   }
   else return ATfalse;

   int stmt_enum = Jovial_ROSE_Translation::e_reducible_directive_stmt;
   SgUntypedDirectiveDeclaration* reducible_directive = new SgUntypedDirectiveDeclaration("", stmt_enum, directive_string);
   ROSE_ASSERT(reducible_directive);
   setSourcePosition(reducible_directive, term);

   decl_list->get_decl_list().push_back(reducible_directive);

   return ATtrue;
}

//========================================================================================
// 9.11 ALLOCATION ORDER DIRECTIVES
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_OrderDirective(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OrderDirective: %s\n", ATwriteToString(term));
#endif

   std::string directive_string = "";

   if (ATmatch(term, "OrderDirective()")) {
      // MATCHED OrderDirective
   }
   else return ATfalse;

   int stmt_enum = Jovial_ROSE_Translation::e_order_directive_stmt;
   SgUntypedDirectiveDeclaration* order_directive = new SgUntypedDirectiveDeclaration("", stmt_enum, directive_string);
   ROSE_ASSERT(order_directive);
   setSourcePosition(order_directive, term);

   decl_list->get_decl_list().push_back(order_directive);

   return ATtrue;
}

void ATermToSageJovialTraversal::
setSourcePositions(ATerm term, Rose::builder::SourcePosition &start, Rose::builder::SourcePosition &end)
{
   PosInfo pos = getLocation(term);

   start.path   = getCurrentFilename();
   start.line   = pos.getStartLine();
   start.column = pos.getStartCol();

   end.path   = getCurrentFilename();
   end.line   = pos.getStartLine();
   end.column = pos.getStartCol();
}
