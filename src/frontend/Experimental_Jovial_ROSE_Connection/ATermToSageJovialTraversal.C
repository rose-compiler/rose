#include "sage3basic.h"
#include "untypedBuilder.h"

#include "ATermToSageJovialTraversal.h"
#include "Jovial_to_ROSE_translation.h"
#include <iostream>

#define PRINT_ATERM_TRAVERSAL 0
#define PRINT_SOURCE_POSITION 0

using namespace ATermSupport;
using namespace Jovial_ROSE_Translation;
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

void ATermToSageJovialTraversal::setLocationSpecifier(SgVariableDeclaration* var_decl, const LocationSpecifier &loc_spec)
{
// The bitfield will contain both the start_bit and start_word as an expression list
   SgExprListExp* location_specifier = SageBuilder::buildExprListExp();

   SgExpression* start_bit  = loc_spec.start_bit;
   SgExpression* start_word = loc_spec.start_word;

   start_bit ->set_parent(location_specifier);
   start_word->set_parent(location_specifier);

   location_specifier->get_expressions().push_back(start_bit);
   location_specifier->get_expressions().push_back(start_word);

   var_decl->set_bitfield(location_specifier);
   location_specifier->set_parent(var_decl);
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

   if (ATmatch(term, "Module(<term>)", &module)) {

      SgScopeStatement* sage_tree_scope{nullptr};
      sage_tree_builder.Enter(sage_tree_scope);

      if (traverse_CompoolModule(module)) {
         // MATCHED CompoolModule
      }
      else if (traverse_ProcedureModule(module)) {
         // MATCHED ProcedureModule
      }
      else if (traverse_MainProgramModule(module)) {
         // MATCHED MainProgramModule
      } else return ATfalse;

      sage_tree_builder.Leave(sage_tree_scope);

   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 1.2.1 COMPOOL MODULES
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_CompoolModule(ATerm term)
{ 
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CompoolModule: %s\n", ATwriteToString(term));
#endif

   ATerm t_dirs, t_name, t_decls;
   std::string name;
   Rose::builder::SourcePositionPair sources;
   SgJovialCompoolStatement* compool_stmt = nullptr;

   if (ATmatch(term, "CompoolModule(<term>,<term>,<term>)", &t_dirs, &t_name, &t_decls)) {

      if (traverse_DirectiveList(t_dirs)) {
         // MATCHED DirectiveList
      } else return ATfalse;

      if (traverse_Name(t_name, name)) {
         // MATCHED Name
      } else return ATfalse;

      sage_tree_builder.Enter(compool_stmt, name, sources);
      sage_tree_builder.Leave(compool_stmt);

      if (traverse_DeclarationList(t_decls)) {
         // MATCHED DeclarationList and CompoolDeclarationList
      } else return ATfalse;

   } else return ATfalse;

   return ATtrue;

}

//========================================================================================
// 1.2.2 PROCEDURE MODULES
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_ProcedureModule(ATerm term)
{ 
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ProcedureModule: %s\n", ATwriteToString(term));
#endif

   ATerm t_dirs, t_decls, t_funcs;

   if (ATmatch(term, "ProcedureModule(<term>,<term>,<term>)", &t_dirs, &t_decls, &t_funcs)) {

      if (traverse_DirectiveList(t_dirs)) {
         // MATCHED DirectiveList
      } else return ATfalse;

      if (traverse_DeclarationList(t_decls)) {
         // MATCHED DeclarationList
      } else return ATfalse;

      if (traverse_NonNestedSubroutineList(t_funcs)) {
         // MATCHED NonNestedSubroutineList
      } else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_DeclarationList(ATerm term)
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
         if (traverse_Declaration(head)) {
            // MATCHED Declaration & CompoolDeclaration
         } else return ATfalse;
      }
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_NullDeclaration(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_NullDeclaration: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "NullDeclaration()")) {

#if 0
   // If this is a function parameter scope do not append the null declaration to it. It's
   // primarily in the grammar for decoration if there are no function formal parameters
   // and will fail when tried to insert in the parameter scope because it SgFunctionParameterScope
   // only contains declarations.

   // Conclusion: only append the null declaration (SgNullStatement) if scope is NOT SgFunctionParameterScope
      if (isSgBasicBlock(SageBuilder::topScopeStack()) != sage_tree_builder.get_context().actual_function_param_scope) {
         ROSE_ASSERT(false);
         SgNullStatement* null_decl = SageBuilder::buildNullStatement();
         setSourcePosition(null_decl, term);
         SageInterface::appendStatement(null_decl, SageBuilder::topScopeStack());
      }
#else
      SgNullStatement* null_decl = SageBuilder::buildNullStatement();
      setSourcePosition(null_decl, term);
      SageInterface::appendStatement(null_decl, SageBuilder::topScopeStack());
#endif
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 1.2.3 MAIN PROGRAM MODULES
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_MainProgramModule(ATerm term)
{ 
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_MainProgramModule: %s\n", ATwriteToString(term));
#endif

   ATerm t_dirs, t_decls, t_name, t_body, t_funcs;

   SgProgramHeaderStatement* program_decl = nullptr;

   if (ATmatch(term, "MainProgramModule(<term>,<term>,<term>,<term>,<term>)", &t_dirs, &t_decls,&t_name,&t_body,&t_funcs)) {
      std::string name;
      std::list<std::string> labels;

      if (traverse_DirectiveList(t_dirs)) {
         // MATCHED DirectiveList
      } else return ATfalse;

      if (traverse_DeclarationList(t_decls)) {
         // MATCHED DeclarationList
      } else return ATfalse;

      if (traverse_Name(t_name, name)) {
         // MATCHED Name
      } else return ATfalse;

      Rose::builder::SourcePosition prog_start, prog_end;  // start and end of program
      Rose::builder::SourcePosition dirs_start, dirs_end;  // start and end of directives
      setSourcePositions(term,   prog_start, prog_end);
      setSourcePositions(t_dirs, dirs_start, dirs_end);

   // Enter SageTreeBuilder for SgProgramHeaderStatement
      Rose::builder::SourcePositions sources(prog_start, dirs_start, prog_end);
      sage_tree_builder.Enter(program_decl, boost::optional<std::string>(name), labels, sources);

      if (traverse_ProgramBody(t_body)) {
         // MATCHED ProgramBody
      } else return ATfalse;

   // Leave SageTreeBuilder for SgProgramHeaderStatement
      sage_tree_builder.Leave(program_decl);

      if (traverse_NonNestedSubroutineList(t_funcs)) {
         // MATCHED NonNestedSubroutineList
      } else return ATfalse;
   }
   else return ATfalse;

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

ATbool ATermToSageJovialTraversal::traverse_ProgramBody(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ProgramBody: %s\n", ATwriteToString(term));
#endif

   ATerm t_stmt;
   ATerm t_decls, t_stmts, t_funcs, t_labels;

   if (ATmatch(term, "ProgramSimpleBody(<term>)", &t_stmt)) {
      if (traverse_Statement(t_stmt)) {
         // MATCHED Statement
      } else return ATfalse;
   }
   else if (ATmatch(term, "ProgramBody(<term>,<term>,<term>,<term>)", &t_decls,&t_stmts,&t_funcs,&t_labels)) {
      std::vector<std::string> labels;
      std::vector<PosInfo> locations;

      if (traverse_DeclarationList(t_decls)) {
         // MATCHED DeclarationList
      } else return ATfalse;

      if (traverse_StatementList(t_stmts)) {
         // MATCHED StatementList
      } else return ATfalse;

      if (traverse_SubroutineDefinitionList(t_funcs)) {
         // MATCHED SubroutineDefinitionList
      } else return ATfalse;

      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;

      if (labels.size() > 1) {
         cerr << "WARNING UNIMPLEMENTED: ProgramBody - labels.size > 1\n";
         return ATtrue;
      }
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_NonNestedSubroutineList(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_NonNestedSubroutineList: %s\n", ATwriteToString(term));
#endif

   ATerm t_subroutine_list, t_proc;
   LanguageTranslation::FunctionModifierList function_modifiers;

   if (ATmatch(term, "NonNestedSubroutineList(<term>)", &t_subroutine_list)) {
      ATermList tail = (ATermList) ATmake("<term>", t_subroutine_list);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);

      // Reuse the modifier list
         function_modifiers.clear();

         if (ATmatch(head, "NonNestedSubroutine(<term>)", &t_proc)) {
            // MATCHED NonNestedSubroutine
         }
         else if (ATmatch(head, "NonNestedSubroutineDEF(<term>)", &t_proc)) {
            function_modifiers.push_back(LanguageTranslation::e_function_modifier_definition);
         }
         else return ATfalse;

         if (traverse_ProcedureDefinition(t_proc, function_modifiers)) {
            // MATCHED ProcedureDefinition
         }
         else if (traverse_FunctionDefinition(t_proc, function_modifiers)) {
            // MATCHED FunctionDefinition
         }
         else return ATfalse;

      }
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_SubroutineDefinitionList(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SubroutineDefinitionList: %s\n", ATwriteToString(term));
#endif

   LanguageTranslation::FunctionModifierList function_modifiers;

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
      if (traverse_ProcedureDefinition(head, function_modifiers)) {
         // MATCHED ProcedureDefinition
      } else if (traverse_FunctionDefinition(head, function_modifiers)) {
         // MATCHED FunctionDefinition
      } else return ATfalse;
   }

   return ATtrue;
}

//========================================================================================
// 1.4 IMPLEMENTATION PARAMETERS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_IntegerMachineParameter(ATerm term, SgExpression* &sg_expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_IntegerMachineParameter: %s\n", ATwriteToString(term));
#endif

   ATerm t_precision, t_scale_spec, t_frac_spec, t_formula;

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
      if (traverse_NumericFormula(t_formula, sg_expr)) {
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
      if (traverse_NumericFormula(t_precision, sg_expr)) {
         // MATCHED Precision
      } else return ATfalse;
   }
   else if (ATmatch(term, "IMPLFIXEDPRECISION(<term>,<term>)", &t_scale_spec, &t_frac_spec)) {
      cerr << "WARNING UNIMPLEMENTED: IntegerMachineParameter - IMPLFIXEDPRECISION\n";
      // MATCHED IMPLFIXEDPRECISION
      if (traverse_NumericFormula(t_scale_spec, sg_expr)) {
         // MATCHED ScaleSpecifier
      } else return ATfalse;
      if (traverse_NumericFormula(t_frac_spec, sg_expr)) {
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

ATbool ATermToSageJovialTraversal::traverse_FloatingMachineParameter(ATerm term, SgExpression* &precision)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FloatingMachineParameter: %s\n", ATwriteToString(term));
#endif

   ATerm t_precision;

   if (ATmatch(term, "MAXFLOAT(<term>)", &t_precision)) {
      cerr << "WARNING UNIMPLEMENTED: FloatingMachineParameter - MAXFLOAT\n";
      // MATCHED MAXFLOAT
      if (traverse_NumericFormula(t_precision, precision)) {
         // MATCHED Precision
      } else return ATfalse;
   }
   else if (ATmatch(term, "MINFLOAT(<term>)", &t_precision)) {
      cerr << "WARNING UNIMPLEMENTED: FloatingMachineParameter - MINFLOAT\n";
      // MATCHED MINFLOAT
      if (traverse_NumericFormula(t_precision, precision)) {
         // MATCHED Precision
      } else return ATfalse;
   }
   else if (ATmatch(term, "FLOATRELPRECISION(<term>)", &t_precision)) {
      cerr << "WARNING UNIMPLEMENTED: FloatingMachineParameter - FLOATRELPRECISION\n";
      // MATCHED FLOATRELPRECISION
      if (traverse_NumericFormula(t_precision, precision)) {
         // MATCHED Precision
      } else return ATfalse;
   }
   else if (ATmatch(term, "FLOATUNDERFLOW(<term>)", &t_precision)) {
      cerr << "WARNING UNIMPLEMENTED: FloatingMachineParameter - FLOATUNDERFLOW\n";
      // MATCHED FLOATUNDERFLOW
      if (traverse_NumericFormula(t_precision, precision)) {
         // MATCHED Precision
      } else return ATfalse;
   }

   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_FixedMachineParameter(ATerm term, SgExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FixedMachineParameter: %s\n", ATwriteToString(term));
#endif

   ATerm t_spec1, t_spec2, t_scale_spec, t_frac_spec;
   SgExpression *scale_spec=nullptr, *frac_spec=nullptr;

   expr = nullptr;

   if (ATmatch(term, "MAXFIXED(<term>,<term>)", &t_spec1, &t_spec2)) {
      cerr << "WARNING UNIMPLEMENTED: FixedMachineParameter - MAXFIXED\n";
      // MATCHED MAXFIXED
      if (ATmatch(t_spec1, "ScaleSpecifier(<term>)", &t_scale_spec)) {
         if (traverse_NumericFormula(t_scale_spec, scale_spec)) {
            // MATCHED ScaleSpecifier
         } else return ATfalse;
      } else return ATfalse;

      if (ATmatch(t_spec2, "FractionSpecifier(<term>)", &t_frac_spec)) {
         if (traverse_NumericFormula(t_frac_spec, frac_spec)) {
            // MATCHED FractionSpecifier
         } else return ATfalse;
      } else return ATfalse;
   }
   else if (ATmatch(term, "MINFIXED(<term>,<term>)", &t_spec1, &t_spec2)) {
      cerr << "WARNING UNIMPLEMENTED: FixedMachineParameter - MINFIXED\n";
      // MATCHED MINFIXED
      if (ATmatch(t_spec1, "ScaleSpecifier(<term>)", &t_scale_spec)) {
         if (traverse_NumericFormula(t_scale_spec, scale_spec)) {
            // MATCHED ScaleSpecifier
         } else return ATfalse;
      } else return ATfalse;

      if (ATmatch(t_spec2, "FractionSpecifier(<term>)", &t_frac_spec)) {
         if (traverse_NumericFormula(t_frac_spec, frac_spec)) {
            // MATCHED FractionSpecifier
         } else return ATfalse;
      } else return ATfalse;
   }
   else return ATfalse;

// TODO - need to return an SgExprListExp containing the scale and fraction specifiers
// expr = SageBuilder::buildExprListExp_nfi();
// ROSE_ASSERT(scale_spec);
// ROSE_ASSERT(frac_spec);

   return ATtrue;
}

//========================================================================================
// 2.0 DECLARATIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_Declaration(ATerm term)
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
         if (traverse_Declaration(head)) {
            // MATCHED Declaration & CompoolDeclaration
         } else return ATfalse;
      }
   }
   else if (traverse_ExternalDeclaration(term)) {
      // MATCHED ExternalDeclaration
   }
   else if (traverse_DataDeclaration(term)) {
      // MATCHED DataDeclaration
   }
   else if (traverse_ConstantDeclaration(term)) {
      // MATCHED ConstantDeclaration
   }
   else if (traverse_TypeDeclaration(term)) {
      // MATCHED TypeDeclaration
   }
   else if (traverse_DefineDeclaration(term)) {
      // MATCHED DefineDeclaration
   }
   else if (traverse_OverlayDeclaration(term)) {
      // MATCHED OverlayDeclaration
   }
   else if (traverse_StatementNameDeclaration(term)) {
      // MATCHED StatementNameDeclaration
   }
   else if (traverse_InlineDeclaration(term)) {
      // MATCHED InlineDeclaration
   }
   else if (traverse_NullDeclaration(term)) {
      // MATCHED NullDeclaration
   }
   else return ATfalse;

   return ATtrue;

}

//========================================================================================
// 2.1 DATA DECLARATIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_DataDeclaration(ATerm term, int def_or_ref)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_DataDeclaration: %s\n", ATwriteToString(term));
#endif

   if (traverse_ItemDeclaration(term, def_or_ref)) {
      // MATCHED ItemDeclaration
   }
   else if (traverse_TableDeclaration(term, def_or_ref)) {
      // MATCHED TableDeclaration -> DataDeclaration
   }
   else if (traverse_ConstantDeclaration(term)) {
      // MATCHED ConstantDeclaration
   }
   else if (traverse_BlockDeclaration(term, def_or_ref)) {
      // MATCHED BlockDeclaration
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.1 ITEM DECLARATION
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_ItemDeclaration(ATerm term, int def_or_ref)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ItemDeclaration: %s\n", ATwriteToString(term));
#endif

   ATerm t_name, t_alloc, t_type, t_preset;
   char* name;

   SgType* declared_type = nullptr;
   SgExpression* preset = nullptr;
   Sawyer::Optional<SgExpression*> status_size;
   Sawyer::Optional<LanguageTranslation::ExpressionKind> modifier_enum;
   std::list<SgInitializedName*> status_list;

   std::string label = "";

   if (ATmatch(term, "ItemDeclaration(<term>,<term>,<term>,<term>)", &t_name,&t_alloc,&t_type,&t_preset)) {
      if (ATmatch(t_name, "<str>", &name)) {
         // MATCHED ItemName
      } else return ATfalse;

      if (traverse_OptAllocationSpecifier(t_alloc, modifier_enum)) {
         // MATCHED OptAllocationSpecifier
      } else return ATfalse;

      if (traverse_ItemTypeDescription(t_type, declared_type)) {
         // MATCHED ItemTypeDescription
      }
      else if (traverse_StatusItemDescription(t_type, status_list, status_size)) {
         // MATCHED StatusItemDescription

         // status item declarations have to be handled differently than other ItemTypeDescription terms

         // also assume an int is sufficient for status_size for now
         if (status_size) {
            cerr << "WARNING UNIMPLEMENTED: ItemDeclaration - StatusItemDescription with size \n";
            ROSE_ASSERT(false);
         }

         // Begin SageTreeBuilder
         SgEnumDeclaration* enum_decl = nullptr;
         sage_tree_builder.Enter(enum_decl, name, status_list);
         setSourcePosition(enum_decl, term);

         sage_tree_builder.Leave(enum_decl);
      } else return ATfalse;

      if (traverse_ItemPreset(t_preset, preset)) {
         // MATCHED ItemPreset
      } else return ATfalse;
   }
   else return ATfalse;

   if (declared_type == nullptr) {
      cerr << "WARNING UNIMPLEMENTED: ItemDeclaration - type is null \n";
      return ATtrue;
   }

// Begin SageTreeBuilder
   SgVariableDeclaration* var_decl;
   sage_tree_builder.Enter(var_decl, std::string(name), declared_type, preset);
   setSourcePosition(var_decl, term);

// Begin language specific constructs
   setDeclarationModifier(var_decl, def_or_ref);

// End SageTreeBuilder
   sage_tree_builder.Leave(var_decl);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_ItemTypeDescription(ATerm term, SgType* &type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ItemTypeDescription: %s\n", ATwriteToString(term));
#endif

   std::string name;

   type = nullptr;

   if (traverse_IntegerItemDescription(term, type)) {
      // MATCHED IntegerItemDescription
   }
   else if (traverse_FloatingItemDescription(term, type)) {
      // MATCHED FloatingItemDescription
   }
   else if (traverse_FixedItemDescription(term, type)) {
      // MATCHED FixedItemDescription
   }
   else if (traverse_BitItemDescription(term, type)) {
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
      SgSymbol* symbol = SageInterface::lookupSymbolInParentScopes(name, SageBuilder::topScopeStack());

      if (symbol == nullptr) {
         cerr << "WARNING UNIMPLEMENTED: ItemTypeDescription - symbol lookup failed for ItemTypeName " << name << "\n";
         return ATtrue;
      }
      ROSE_ASSERT(symbol);
      type = symbol->get_type();
      ROSE_ASSERT(type);
   }
   else return ATfalse;

   ROSE_ASSERT(type);

   return ATtrue;
}

//========================================================================================
// 2.1.1.1 INTEGER TYPE DESCRIPTIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_IntegerItemDescription(ATerm term, SgType* &type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_IntegerItemDescription: %s\n", ATwriteToString(term));
#endif

   ATerm t_type, t_round_or_truncate, t_size;
   bool is_signed;

   Sawyer::Optional<LanguageTranslation::ExpressionKind> modifier_enum;
   Sawyer::Optional<SgExpression*> opt_size;
   SgExpression* kind = nullptr;

// The first term, t_type, comes from the lexer and is direct user input: "S", "s", "U", "u".
// We disambiguate it via terms IntegerItemDescription or IntegerItemDescriptionU.
   if (ATmatch(term, "IntegerItemDescription(<term>,<term>,<term>)", &t_type,&t_round_or_truncate,&t_size)) {
      is_signed = true;
   }
   else if (ATmatch(term, "IntegerItemDescriptionU(<term>,<term>,<term>)", &t_type,&t_round_or_truncate,&t_size)) {
      is_signed = false;
   }
   else return ATfalse;

   if (traverse_OptRoundOrTruncate(t_round_or_truncate, modifier_enum)) {
      // MATCHED OptRoundOrTruncate
   } else return ATfalse;

   if (traverse_OptItemSize(t_size, opt_size)) {
      // MATCHED OptItemSize
   } else return ATfalse;

   if (opt_size) kind = *opt_size;

   if (is_signed) {
      type = SageBuilder::buildIntType(kind);
   } else {
      type = SageBuilder::buildUnsignedIntType(kind);
   }
   ROSE_ASSERT(type);

   if (modifier_enum) {
      SgModifierType* mod_type = SageBuilder::buildModifierType(type);

      if (*modifier_enum == LanguageTranslation::e_type_modifier_round) {
         mod_type->get_typeModifier().setRound();
      }
      else if (*modifier_enum == LanguageTranslation::e_type_modifier_truncate) {
         mod_type->get_typeModifier().setTruncate();
      }
      else if (*modifier_enum == LanguageTranslation::e_type_modifier_z) {
         mod_type->get_typeModifier().setTruncateTowardsZero();
      }
      else ROSE_ASSERT(false);

      type = mod_type;
   }
   ROSE_ASSERT(type != nullptr);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_OptItemSize(ATerm term, Sawyer::Optional<SgExpression*> &size)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptItemSize: %s\n", ATwriteToString(term));
#endif

   ATerm t_size;

   SgExpression* size_expr = nullptr;

   if (ATmatch(term, "no-item-size()")) {
      size = Sawyer::Nothing();
   }
   else if (ATmatch(term, "ItemSize(<term>)", &t_size)) {
      if (traverse_NumericFormula(t_size, size_expr)) {
         size = Sawyer::Optional<SgExpression*>(size_expr);
      } else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.1.2 FLOATING TYPE DESCRIPTIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_FloatingItemDescription(ATerm term, SgType* &type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FloatingItemDescription: %s\n", ATwriteToString(term));
#endif

   ATerm t_round_or_truncate, t_precision;
   Sawyer::Optional<LanguageTranslation::ExpressionKind> modifier_enum;
   SgExpression* precision = nullptr;

   if (ATmatch(term, "FloatingItemDescription(<term>,<term>)", &t_round_or_truncate, &t_precision)) {
      // MATCHED FloatingItemDescription
   }
   else return ATfalse;

// rounding or truncate mode
   if (traverse_OptRoundOrTruncate(t_round_or_truncate, modifier_enum)) {
      // MATCHED OptRoundOrTruncate
   }
   else return ATfalse;

// precision
   if (ATmatch(t_precision, "no-precision()")) {
      // MATCHED no-precision
   }
   else if (traverse_NumericFormula(t_precision, precision)) {
      // MATCHED NumericFormula
   }
   else return ATfalse;

   type = SageBuilder::buildFloatType(precision);

   if (modifier_enum) {
      SgModifierType* mod_type = SageBuilder::buildModifierType(type);

      if (*modifier_enum == LanguageTranslation::e_type_modifier_round) {
         mod_type->get_typeModifier().setRound();
      }
      else if (*modifier_enum == LanguageTranslation::e_type_modifier_truncate) {
         mod_type->get_typeModifier().setTruncate();
      }
      else if (*modifier_enum == LanguageTranslation::e_type_modifier_z) {
         mod_type->get_typeModifier().setTruncateTowardsZero();
      }
      else ROSE_ASSERT(false);

      type = mod_type;
   }
   ROSE_ASSERT(type != nullptr);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_OptRoundOrTruncate(ATerm term, Sawyer::Optional<LanguageTranslation::ExpressionKind> &modifier_enum)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptRoundOrTruncate: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "no-round-or-truncate()")) {
      modifier_enum = Sawyer::Nothing();
   }
   else if (ATmatch(term, "R()")) {
      modifier_enum = Sawyer::Optional<LanguageTranslation::ExpressionKind>(e_type_modifier_round);
   }
   else if (ATmatch(term, "T()")) {
      modifier_enum = Sawyer::Optional<LanguageTranslation::ExpressionKind>(e_type_modifier_truncate);
   }
   else if (ATmatch(term, "Z()")) {
      modifier_enum = Sawyer::Optional<LanguageTranslation::ExpressionKind>(e_type_modifier_z);
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.1.3 FIXED TYPE DESCRIPTIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_FixedItemDescription(ATerm term, SgType* & type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FixedItemDescription: %s\n", ATwriteToString(term));
#endif

   ATerm t_round_or_truncate, t_scale, t_fraction, t_scale_spec, t_frac_spec;
   Sawyer::Optional<LanguageTranslation::ExpressionKind> modifier_enum;
   SgExpression * scale = nullptr, * fraction = nullptr;

   if (ATmatch(term, "FixedItemDescription (<term>,<term>,<term>)", &t_round_or_truncate,&t_scale,&t_fraction)) {
      // MATCHED FixedItemDescription
   }
   else return ATfalse;

   if (traverse_OptRoundOrTruncate(t_round_or_truncate, modifier_enum)) {
      // MATCHED OptRoundOrTruncate
   } else return ATfalse;

   if (ATmatch(t_scale, "ScaleSpecifier(<term>)", &t_scale_spec)) {
      if (traverse_NumericFormula(t_scale_spec, scale)) {
         // MATCHED NumericFormula
      } else return ATfalse;
   } else return ATfalse;

   if (ATmatch(t_fraction, "no-fraction-specifier()")) {
      // MATCHED no-fraction-specifier
   }
   else if (ATmatch(t_fraction, "OptFractionSpecifier(<term>)", &t_frac_spec)) {
      if (traverse_FractionSpecifier(t_frac_spec, fraction)) {
      } else return ATfalse;
   }
   else return ATfalse;

// FIXME SageBuilder
#if 0
// TODO - for some reason this SageBuilder function fails in linker stage
   type = SageBuilder::buildFixedType(scale, fraction);
#else
   type = SgTypeFixed::createType(scale, fraction);
   ROSE_ASSERT(type);
   if (scale)    scale   ->set_parent(type);
   if (fraction) fraction->set_parent(type);
#endif

   if (modifier_enum) {
      SgModifierType* mod_type = SageBuilder::buildModifierType(type);

      if (*modifier_enum == LanguageTranslation::e_type_modifier_round) {
         mod_type->get_typeModifier().setRound();
      }
      else if (*modifier_enum == LanguageTranslation::e_type_modifier_truncate) {
         mod_type->get_typeModifier().setTruncate();
      }
      else if (*modifier_enum == LanguageTranslation::e_type_modifier_z) {
         mod_type->get_typeModifier().setTruncateTowardsZero();
      }
      else ROSE_ASSERT(false);

      type = mod_type;
   }
   ROSE_ASSERT(type != nullptr);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_FractionSpecifier(ATerm term, SgExpression* &fraction)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FractionSpecifier: %s\n", ATwriteToString(term));
#endif

   ATerm t_fraction;

   if (ATmatch(term, "FractionSpecifier(<term>)", &t_fraction)) {
      if (traverse_NumericFormula(t_fraction, fraction)) {
        // MATCHED FixedFormula through NumericFormula
      } else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.1.4 BIT TYPE DESCRIPTIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_BitItemDescription(ATerm term, SgType* &type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BitItemDescription: %s\n", ATwriteToString(term));
#endif

   ATerm t_type_desc, t_size;
   Sawyer::Optional<SgExpression*> opt_size;
   SgExpression* size = nullptr;

   if (ATmatch(term, "BitItemDescription(<term>,<term>)", &t_type_desc, &t_size)) {
    // Ignore the BitTypeDesc term t_type_desc.  It was placed in JovialLex and will be 'B' (just syntax)
   } else return ATfalse;

   if (traverse_OptItemSize(t_size, opt_size)) {
      // MATCHED OptItemSize
   } else return ATfalse;

   if (opt_size) size = *opt_size;

   // TODO - is incorrect (may be closer to a hexadecimal or Hollerith, but used in boolean expressions)
   type = SageBuilder::buildBoolType(size);

   return ATtrue;
}

//========================================================================================
// 2.1.1.5 CHARACTER TYPE DESCRIPTIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_CharacterLiteral(ATerm term, std::string &str_literal)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CharacterLiteral: %s\n", ATwriteToString(term));
#endif

   char* char_literal;

   if (ATmatch(term, "CharacterLiteral(<str>)", &char_literal)) {
      str_literal = char_literal;
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_CharacterItemDescription(ATerm term, SgType* & type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CharacterItemDescription: %s\n", ATwriteToString(term));
#endif

   ATerm t_size;
   Sawyer::Optional<SgExpression*> size;

   if (ATmatch(term, "CharacterItemDescription(<term>)", &t_size)) {
      // MATCHED CharacterItemDescription
   } else return ATfalse;

   if (traverse_OptItemSize(t_size, size)) {
      if (size) {
         type = SageBuilder::buildStringType(*size);
      }
      else {
         type = SageBuilder::buildCharType();
      }
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.1.6 STATUS TYPE DESCRIPTIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_StatusConstant(ATerm term, SgInitializedName* &init_name, SgType* &enum_type, SgExpression* init_expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_StatusConstant: %s\n", ATwriteToString(term));
#endif

// NOTE: Name -> StatusConstant , Letter -> StatusConstant, ReservedWord -> StatusConstant
   char* name;

   init_name = nullptr;

   if (ATmatch(term, "StatusConstant(<str>)", &name)) {
      std::string constant_name = name;
      constant_name.insert(0, "_V_");

      // This is an enumerator that is part of TypeDeclaration (SgEnumDeclaration).
      // The init_expr parameter will be the initialized value for the enumerator.
      ROSE_ASSERT(init_expr);

      // Only one SgEnumType should be created and shared between all enumerators
      if (!enum_type) {
         enum_type = new SgEnumType();
         ROSE_ASSERT(enum_type);
      }

      SgAssignInitializer* initializer = SageBuilder::buildAssignInitializer_nfi(init_expr, enum_type);
      init_name = SageBuilder::buildInitializedName_nfi(constant_name, enum_type, initializer);
   } else return ATfalse;

   ROSE_ASSERT(init_name);
   setSourcePosition(init_name, term);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_StatusConstant(ATerm term, SgExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_StatusConstant: %s\n", ATwriteToString(term));
#endif

// NOTE: Name -> StatusConstant , Letter -> StatusConstant, ReservedWord -> StatusConstant
   char* name;
   SgEnumVal* enum_val;

   expr = nullptr;

   if (ATmatch(term, "StatusConstant(<str>)", &name)) {
      std::string constant_name = name;
      constant_name.insert(0, "_V_");

      // An enumerator in Jovial is scoped! Therefore we have to worry about finding the correct one.
      // Build temporary EnumVal with no SgEnumDeclaration pointer to avoid name clashes with another SgEnumType.
      // The correct SgEnumVal will be built by SageTreeBuilder once we know the correct type.
      // In this case an expression is returned (for example the rhs of an assignment).

      enum_val = SageBuilder::buildEnumVal(-1, nullptr, constant_name);
   } else return ATfalse;

   ROSE_ASSERT(enum_val);
   setSourcePosition(enum_val, term);

   expr = enum_val;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::
traverse_StatusItemDescription(ATerm term, std::list<SgInitializedName*> &status_list, Sawyer::Optional<SgExpression*> &status_size)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_StatusItemDescription: %s\n", ATwriteToString(term));
#endif

   ATerm t_size, t_sublist;
   SgType* enum_type = nullptr;

   if (ATmatch(term, "StatusItemDescription(<term>,<term>)", &t_size, &t_sublist)) {
      if (traverse_OptItemSize(t_size, status_size)) {
         // MATCHED OptItemSize
      } else return ATfalse;

      if (traverse_DefaultSublist(t_sublist, status_list, enum_type)) {
         // MATCHED DefaultSublist
      }
      else if (traverse_StatusList(t_sublist, status_list, enum_type)) {
         // MATCHED StatusList
      }
      else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_DefaultSublist(ATerm term, std::list<SgInitializedName*> &status_list, SgType* &enum_type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_DefaultSublist: %s\n", ATwriteToString(term));
#endif

   ATerm t_sublist;
   SgInitializedName* init_name;
   SgExpression* init_expr = nullptr;
   int value = 0;

   if (ATmatch(term, "DefaultSublist(<term>)", &t_sublist)) {
      ATermList tail = (ATermList) ATmake("<term>", t_sublist);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);

         init_expr = SageBuilder::buildIntVal(value);
         ROSE_ASSERT(init_expr);
         SageInterface::setSourcePosition(init_expr);

         ++value;

         if (traverse_StatusConstant(head, init_name, enum_type, init_expr)) {
            status_list.push_back(init_name);
         } else return ATfalse;
      }
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_OptDefaultSublist(ATerm term, std::list<SgInitializedName*> &status_list, SgType* &enum_type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptDefaultSublist: %s\n", ATwriteToString(term));
#endif

   ATerm t_sublist;

   if (ATmatch(term, "no-default-sublist()")) {
      // MATCHED no default sublist
   }
   else if (ATmatch(term, "OptDefaultSublist(<term>)", &t_sublist)) {
      if (traverse_DefaultSublist(t_sublist, status_list, enum_type)) {
         // MATCHED DefaultSublist
      } else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_StatusList(ATerm term, std::list<SgInitializedName*> &status_list, SgType* &enum_type)
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
      if (traverse_OptDefaultSublist(t_sublist, status_list, enum_type)) {
         // MATCHED OptDefaultSublist
      } else return ATfalse;

      ATermList tail = (ATermList) ATmake("<term>", t_specified);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_SpecifiedSublist(head, status_list, enum_type)) {
           // MATCHED SpecifiedSublist
         } else return ATfalse;
      }
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_SpecifiedSublist(ATerm term, std::list<SgInitializedName*> &status_list, SgType* &enum_type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SpecifiedSublist: %s\n", ATwriteToString(term));
#endif

   ATerm t_formula, t_constant;

   SgInitializedName* init_name = nullptr;
   SgExpression* init_expr = nullptr;

   if (ATmatch(term, "SpecifiedSublist(<term>,<term>)", &t_formula, &t_constant)) {

      if (traverse_NumericFormula(t_formula, init_expr)) {
         // MATCHED NumericFormula
      } else return ATfalse;

      ROSE_ASSERT(init_expr);
      int pass = 1;
      int value;

      ATermList tail = (ATermList) ATmake("<term>", t_constant);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (pass == 2) {
            SgIntVal* intval = isSgIntVal(init_expr);
            ROSE_ASSERT(intval);
            value = intval->get_value();

            init_expr = SageBuilder::buildIntVal(++value);
         }
         if (pass > 2) {
            init_expr = SageBuilder::buildIntVal(++value);
         }

         if (traverse_StatusConstant(head, init_name, enum_type, init_expr)) {
            status_list.push_back(init_name);
            ++pass;
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
ATbool ATermToSageJovialTraversal::traverse_PointerItemDescription(ATerm term, SgType* & type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_PointerItemDescription: %s\n", ATwriteToString(term));
#endif

   char* pntr;
   ATerm t_type_name;
   std::string type_name;

   type = nullptr;

   if (ATmatch(term, "PointerItemDescription(<str>,<term>)", &pntr, &t_type_name)) {

      if (traverse_OptTypeName(t_type_name, type, type_name)) {
         // MATCHED OptTypeName
      } else return ATfalse;
   }
   else return ATfalse;

   ROSE_ASSERT(type);

   SgPointerType* pointer_type = SageBuilder::buildPointerType(type);
   ROSE_ASSERT(pointer_type);

   type = pointer_type;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_OptTypeName(ATerm term, SgType* & type, std::string & type_name)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptTypeName: %s\n", ATwriteToString(term));
#endif

   ATerm t_type_name;

   type = nullptr;

   if (ATmatch(term, "no-type-name()")) {
      // As there is no type name this will become a pointer to void (void*)
      type = SageBuilder::buildVoidType();
   }
   else if (ATmatch(term, "TypeName(<term>)", &t_type_name)) {
      if (traverse_Name(t_type_name, type_name)) {
         type = SageInterface::lookupNamedTypeInParentScopes(type_name, SageBuilder::topScopeStack());
         if (type == nullptr) {
            cerr << "WARNING UNIMPLEMENTED: OptTypeName for name: " << type_name << endl;
            //DELETE_ME - TEMPORARY - ensure that there is some type (what about primitive types?)
            type = SageBuilder::buildVoidType();
         }
      }
      else return ATfalse;
   }
   else return ATfalse;

   ROSE_ASSERT(type != nullptr);

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

// Begin SageTreeBuilder
   SgJovialTableStatement* table_decl = nullptr;
   SgType* base_type = nullptr;
   SgType* type = nullptr;
   SgExprListExp* attr_list = nullptr;
   SgExprListExp* dim_info = nullptr;
   SgExpression* preset = nullptr;
   std::string table_var_name, table_type_name, anon_type_name;
   bool is_type_inherited = false;

   TableSpecifier table_spec;
   Sawyer::Optional<LanguageTranslation::ExpressionKind> modifier_enum;

   if (ATmatch(term, "TableDeclaration(<term>,<term>,<term>,<term>)", &t_name,&t_alloc,&t_dim_list,&t_table_desc)) {
      std::string label = "";

      dim_info = SageBuilder::buildExprListExp();

      if (ATmatch(t_name, "<str>", &name)) {
         // MATCHED TableName
      } else return ATfalse;

      table_var_name = std::string(name);
      anon_type_name = std::string("_anon_typeof_") + table_var_name;

      if (traverse_OptAllocationSpecifier(t_alloc, modifier_enum)) {
         // MATCHED OptAllocationSpecifier
      } else return ATfalse;

      if (traverse_OptDimensionList(t_dim_list, dim_info)) {
         // MATCHED OptDimensionList
      } else return ATfalse;

// 1. Look for a type name first (type will have already been declared by this point).
//    The type name is the name of the base type (this declaration inherits from the base/parent class)
//
      if (traverse_TableDescriptionName(t_table_desc, table_type_name, type, preset)) {
         SgJovialTableType* table_type = isSgJovialTableType(type);
         if (table_type == nullptr) {
            cerr << "WARNING UNIMPLEMENTED: TableDeclaration - TableDescriptionName returns NULL type for name " << table_type_name << "\n";
         // TODO_COMPOOL
         // ROSE_ASSERT(type);
            return ATtrue;
         }
         type = SageBuilder::buildJovialTableType(table_type_name, table_type->get_base_type(), dim_info, SageBuilder::topScopeStack());
         ROSE_ASSERT(type);

         is_type_inherited = true;
      }

// 2. Otherwise look for a base type (this is not inheritance, rather it is similar to the base type of an array type).
//    The base type is the table description and there will be no body.
//
      else if (traverse_TableDescriptionType(t_table_desc, base_type, preset, attr_list, table_spec)) {
         ROSE_ASSERT(base_type);

      // This must be anonymous as there is no explicit name for the type.
         SgName name(anon_type_name);

         type = SageBuilder::buildJovialTableType(name, base_type, dim_info, SageBuilder::topScopeStack());
         ROSE_ASSERT(type);
      }

// 3. Finally check for a table description body. This will need to create a table declaration
//    with a body for the table definition member variables. The declaration will be anonymous
//    and associated with the variable declaration (via baseTypeDefiningDeclaration).
//
      else if (traverse_TableDescriptionBody(t_table_desc, anon_type_name, table_decl, preset, table_spec)) {
         ROSE_ASSERT(table_decl);

         SgJovialTableType* table_type = isSgJovialTableType(table_decl->get_type());
         ROSE_ASSERT(table_type);

         if (dim_info) {
            table_type->set_dim_info(dim_info);
            dim_info->set_parent(table_type);
         }
         type = table_type;
      }
      else return ATfalse;
   }
   else return ATfalse;

   SgJovialTableType* table_type = isSgJovialTableType(type);
   ROSE_ASSERT(table_type);

   // Wrap the type in an SgStructureModifier if needed
   StructureSpecifier& struct_spec = table_spec.struct_spec;
   if (struct_spec.is_parallel || struct_spec.is_tight) {
      SgModifierType* modifiers = SageBuilder::buildModifierType(type);
      if (struct_spec.is_parallel) {
         modifiers->get_typeModifier().get_structureModifier().setParallel();
      }
      else if (struct_spec.is_tight) {
         modifiers->get_typeModifier().get_structureModifier().setTight();
         modifiers->get_typeModifier().get_structureModifier().set_bits_per_entry(struct_spec.bits_per_entry);
      }

   // Reset the type to the SgModifierType wrapper
      type = modifiers;
   }

// Begin SageTreeBuilder
   SgVariableDeclaration* var_decl = nullptr;

   sage_tree_builder.Enter(var_decl, table_var_name, type, preset);
   setSourcePosition(var_decl, term);

// Begin language specific constructs
   setDeclarationModifier(var_decl, def_or_ref);

   SgJovialTableStatement* decl = isSgJovialTableStatement(table_type->get_declaration());
   ROSE_ASSERT(decl);
   SgJovialTableStatement* def_decl = isSgJovialTableStatement(decl->get_definingDeclaration());
   ROSE_ASSERT(def_decl);

   if (is_type_inherited == false) {
      SageInterface::setBaseTypeDefiningDeclaration(var_decl, def_decl);
   }

   if (table_spec.packing_spec != e_packing_spec_unknown) {
      SgStorageModifier& storage_mod = var_decl->get_declarationModifier().get_storageModifier();
      if      (table_spec.packing_spec == e_packing_spec_none)  storage_mod.setPackingNone();
      else if (table_spec.packing_spec == e_packing_spec_mixed) storage_mod.setPackingMixed();
      else if (table_spec.packing_spec == e_packing_spec_dense) storage_mod.setPackingDense();
   }

   SgClassDefinition* def = def_decl->get_definition();
   ROSE_ASSERT(def);
   ROSE_ASSERT(def->isCaseInsensitive());

#if 0
   //   def_decl->get_scope()->setCaseInsensitive(true);
   //   var_decl->get_scope()->setCaseInsensitive(true);
   cout << ".x. def scope sensitivity is " << def->isCaseInsensitive() << endl;
   def->setCaseInsensitive(true);
   cout << ".x. def scope sensitivity is " << def->isCaseInsensitive() << endl;

   cout << ".x. type is " << type << ": " << type->class_name() << endl;
   cout << ".x. decl is " << decl << ": " << decl->class_name() << endl;
   cout << ".x. def_decl is " << def_decl << ": " << def_decl->class_name() << endl;
   cout << ".x. def is " << def << ": " << def->class_name() << endl;
   cout << ".x. def_decl scope is " << def_decl->get_scope() << ": " << def_decl->get_scope()->isCaseInsensitive() << endl;
   cout << ".x. var_decl scope is " << var_decl->get_scope() << ": " << var_decl->get_scope()->isCaseInsensitive() << endl;
   cout << ".x. def scope sensitivity is " << def->isCaseInsensitive() << endl;
#endif

   sage_tree_builder.Leave(var_decl);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::
traverse_TableDescriptionName(ATerm term, std::string &type_name, SgType* &type, SgExpression* &preset)
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
      SgSymbol* symbol = SageInterface::lookupSymbolInParentScopes(type_name, SageBuilder::topScopeStack());
      if (symbol != NULL) {
         type = symbol->get_type();
      }
      if (type == nullptr) {
         cerr << "WARNING UNIMPLEMENTED: TableDescriptionName - class symbol is null for type name " << type_name <<  "\n";
      }

      if (traverse_TablePreset(t_preset, preset)) {
         // MATCHED TablePreset
         //DONE: cerr << "WARNING UNIMPLEMENTED: TableDescriptionName - table preset \n";
      } else return ATfalse;

   }
   else return ATfalse;

   if (type == nullptr) {
      cerr << "WARNING UNIMPLEMENTED: TableDescriptionName \n";
   // TODO_COMPOOL
   // ROSE_ASSERT(type != nullptr);
   }

   return ATtrue;
}

// This table is array-like in that it doesn't have a table/structure body (but has a base type)
ATbool ATermToSageJovialTraversal::
traverse_TableDescriptionType(ATerm term, SgType* &type, SgExpression* &preset,
                                          SgExprListExp* attr_list, TableSpecifier &table_spec)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_TableDescriptionType: %s\n", ATwriteToString(term));
#endif

   ATerm t_struct_spec, t_entry_spec;
   std::string type_name;

// TODO
   LocationSpecifier location_spec;

   StructureSpecifier& struct_spec = table_spec.struct_spec;

   if (ATmatch(term, "TableDescription(<term>,<term>)", &t_struct_spec, &t_entry_spec)) {

   // This is an EntrySpecifier without a body
      if (traverse_EntrySpecifierType(t_entry_spec, type, location_spec, preset, attr_list, table_spec)) {
         // MATCHED EntrySpecifierType
      } else return ATfalse;

      if (traverse_OptStructureSpecifier(t_struct_spec, struct_spec)) {
         // MATCHED OptStructureSpecifier
      } else return ATfalse;
   }
   else return ATfalse;

   ROSE_ASSERT(type);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::
traverse_TableDescriptionBody(ATerm term, std::string &type_name, SgJovialTableStatement* &table_decl,
                                          SgExpression* &preset, TableSpecifier &table_spec)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_TableDescription: %s\n", ATwriteToString(term));
#endif

   ATerm t_struct_spec, t_entry_spec;

   table_decl = nullptr;
   StructureSpecifier& struct_spec = table_spec.struct_spec;

   if (ATmatch(term, "TableDescription(<term>,<term>)", &t_struct_spec, &t_entry_spec)) {

   // Begin SageTreeBuilder
      Rose::builder::SourcePositionPair sources;
      sage_tree_builder.Enter(table_decl, type_name, sources);

      if (traverse_OptStructureSpecifier(t_struct_spec, struct_spec)) {
         // MATCHED OptStructureSpecifier
      } else return ATfalse;

      if (traverse_EntrySpecifierBody(t_entry_spec, table_decl, preset, table_spec)){
         // MATCHED EntrySpecifierBody
      } else return ATfalse;
   }
   else return ATfalse;

#if 0
   SgJovialTableType* table_type = isSgJovialTableType(table_decl->get_type());
   SgDeclarationModifier& decl_mod = table_decl->get_declarationModifier();
   SgTypeModifier& type_modifier = table_decl->get_declarationModifier().get_typeModifier();

   ROSE_ASSERT(table_type);
   std::cout << ".x. table_type is " << table_type << ": " << table_type->class_name() << std::endl;
   std::cout << ".x. table_decl is " << table_decl << ": " << table_decl->class_name() << std::endl;
   std::cout << ".x. type_modifier - isAllocatable " << type_modifier.isAllocatable() << std::endl;

   std::cout << "TABLE DESCRIPTION table_desc: " << table_desc << " : " << table_desc->class_name() << endl;
   std::cout << "TABLE DESCRIPTION scope: " << table_desc->get_scope() << endl;
   std::cout << "TABLE DESCRIPTION scope decl list: " << table_desc->get_scope()->get_declaration_list() << endl;
#endif

   ROSE_ASSERT(table_decl);

// End SageTreeBuilder
   sage_tree_builder.Leave(table_decl);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::
traverse_EntrySpecifierType(ATerm term, SgType* &type, LocationSpecifier &loc_spec, SgExpression* &preset,
                                        SgExprListExp* attr_list, TableSpecifier &table_spec)
{
   if (traverse_OrdinaryEntrySpecifierType(term, type, preset, table_spec)) {
      // MATCHED OrdinaryEntrySpecifier -> EntrySpecifier with an item description (no structure body)
   }
   else if (traverse_SpecifiedEntrySpecifierType(term, type, loc_spec, preset, attr_list)) {
      // MATCHED SpecifiedEntrySpecifier -> EntrySpecifier with an item description (no structure body)
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::
traverse_EntrySpecifierBody(ATerm term, SgJovialTableStatement* table_decl,
                                        SgExpression* &preset, TableSpecifier &table_spec)
{
   if (traverse_OrdinaryEntrySpecifierBody(term, preset, table_spec)) {
      // MATCHED OrdinaryEntrySpecifier -> EntrySpecifier with a structure body
   }
   else if (traverse_SpecifiedEntrySpecifierBody(term, table_decl, preset)) {
      // MATCHED SpecifiedEntrySpecifier -> EntrySpecifier with a structure body
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.2.1 TABLE DIMENSION LISTS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_OptDimensionList(ATerm term, SgExprListExp* shape)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptDimensionList: %s\n", ATwriteToString(term));
#endif

   ATerm t_dim_list;

   ROSE_ASSERT(shape != nullptr);
   setSourcePosition(shape, term);

   if (ATmatch(term, "no-dimension-list()")) {
   }
   else if (ATmatch(term, "DimensionList(<term>)" , &t_dim_list)) {
      ATermList tail = (ATermList) ATmake("<term>", t_dim_list);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_Dimension(head, shape)) {
            // MATCHED Dimension
         }
      }
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_Dimension(ATerm term, SgExprListExp* shape)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Dimension: %s\n", ATwriteToString(term));
#endif

   ATerm t_opt_lower_bound, t_lower_bound, t_upper_bound;

   SgExpression* lower_bound = nullptr;
   SgExpression* upper_bound = nullptr;
   SgExpression*      stride = nullptr;

   SgSubscriptExpression* range = nullptr;

   LanguageTranslation::ExpressionKind expr_enum = LanguageTranslation::e_unknown;

   if (ATmatch(term, "Dimension(<term>,<term>)", &t_opt_lower_bound, &t_upper_bound)) {
      expr_enum = LanguageTranslation::e_explicit_dimension;

   // Lower bound (optional)
      if (ATmatch(t_opt_lower_bound, "no-lower-bound-option()")) {
         // lower bound has already been initialized to nullptr
      }
      else if (ATmatch(t_opt_lower_bound, "LowerBoundOption(<term>)", &t_lower_bound)) {
         if (traverse_NumericFormula(t_lower_bound, lower_bound)) {
            // MATCHED NumericFormula
         } else return ATfalse;
      } else return ATfalse;

   // Upper bound
      if (traverse_NumericFormula(t_upper_bound, upper_bound)) {
         // MATCHED NumericFormula
      } else return ATfalse;
   }
   else if (ATmatch(term, "DimensionSTAR()")) {
      expr_enum = LanguageTranslation::e_star_dimension;
   }
   else return ATfalse;

// Begin SageTreeBuilder
   if (stride == nullptr) {
      // ROSE (Fortran) uses "1" rather than SgNullExpression
      stride = SageBuilder::buildIntVal_nfi(std::string("1"));
   }

   range = SageBuilder::buildSubscriptExpression_nfi(lower_bound, upper_bound, stride);
   shape->get_expressions().push_back(range);

   return ATtrue;
}

//========================================================================================
// 2.1.2.2 TABLE STRUCTURE
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_OptStructureSpecifier(ATerm term, StructureSpecifier &struct_spec)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptStructureSpecifier: %s\n", ATwriteToString(term));
#endif

   ATerm t_tight, t_bits_per_entry;
   SgExpression* bits_per_entry = nullptr;

// Default
   struct_spec.is_tight = false;
   struct_spec.is_parallel = false;
   struct_spec.bits_per_entry = 0;

   if (ATmatch(term, "no-structure-specifier()")) {
      // MATCHED no-structure-specifier
   }
   else if (ATmatch(term, "StructureSpecifier()")) {
   // PARALLEL option
      struct_spec.is_parallel = true;
   }
   else if (ATmatch(term, "StructureSpecifierT(<term>, <term>)", &t_tight, &t_bits_per_entry)) {
   // TIGHT option (t_tight term will always be "P")
      struct_spec.is_tight = true;
      if (ATmatch(t_bits_per_entry, "no-bits-per-entry()")) {
         // MATCHED StructureSpecifier with no-bits-per-entry
      }
      else if (traverse_NumericFormula(t_bits_per_entry, bits_per_entry)) {
         SgIntVal* int_val = isSgIntVal(bits_per_entry);
         ROSE_ASSERT(int_val != nullptr);
         ROSE_ASSERT(int_val->get_value() > 0);
         struct_spec.bits_per_entry = int_val->get_value();
      }
      else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.2.3 ORDINARY TABLE ENTRIES
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::
traverse_OrdinaryEntrySpecifierType(ATerm term, SgType* &type, SgExpression* &preset, TableSpecifier &table_spec)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OrdinaryEntrySpecifierType: %s\n", ATwriteToString(term));
#endif

   ATerm t_pack_spec, t_item_desc, t_preset;

   type = nullptr;
   preset = nullptr;

   Sawyer::Optional<SgExpression*> status_size;
   std::list<SgInitializedName*> status_list;

   std::string label = "";

   if (ATmatch(term, "OrdinaryEntrySpecifier(<term>,<term>,<term>)", &t_pack_spec, &t_item_desc, &t_preset)) {

      if (traverse_OptPackingSpecifier(t_pack_spec, table_spec.packing_spec)) {
         // MATCHED OptPackingSpecifier
      } else return ATfalse;

      if (traverse_ItemTypeDescription(t_item_desc, type)) {
         // Found a base type for the table (not inheritance, likely a primitive type similar to an SgArrayType)
      }
      else if (traverse_StatusItemDescription(t_item_desc, status_list, status_size)) {
         // MATCHED StatusItemDescription

         // status item declarations have to be handled differently than other ItemTypeDescription terms

         // also assume an int is sufficient for status_size for now

         cerr << "WARNING UNIMPLEMENTED: OrdinaryEntrySpecifierType - StatusItemDescription\n";
      }
      else return ATfalse;

      if (traverse_TablePreset(t_preset, preset)) {
         // MATCHED TablePreset
         cerr << "WARNING UNIMPLEMENTED: OrdinaryEntrySpecifierType - TablePreset \n";
         cout << "WARNING UNIMPLEMENTED: OrdinaryEntrySpecifierType - TablePreset: preset is " << preset << endl;
      } else return ATfalse;
   }
   else return ATfalse;

   ROSE_ASSERT(type);
   cout << "OrdinaryEntrySpecifierType - found type " << type << ": " << type->class_name() << endl;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::
traverse_OrdinaryEntrySpecifierBody(ATerm term, SgExpression* &preset, TableSpecifier &table_spec)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OrdinaryEntrySpecifierBody: %s\n", ATwriteToString(term));
#endif

   ATerm t_pack_spec, t_preset, t_body;

   if (ATmatch(term, "OrdinaryEntrySpecifierBody(<term>,<term>,<term>)", &t_pack_spec, &t_preset, &t_body)) {

      if (traverse_OptPackingSpecifier(t_pack_spec, table_spec.packing_spec)) {
         // MATCHED OptPackingSpecifier
      } else return ATfalse;

      if (traverse_TablePreset(t_preset, preset)) {
         // MATCHED TablePreset
      } else return ATfalse;

      if (traverse_OrdinaryTableBody(t_body)) {
         // MATCHED OrdinaryTableBody
      } else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_OrdinaryTableBody(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OrdinaryTableBody: %s\n", ATwriteToString(term));
#endif

   ATerm t_directives, t_table_option_list;

   if (ATmatch(term, "OrdinaryTableBody(<term>,<term>)", &t_directives, &t_table_option_list)) {

      if (traverse_DirectiveList(t_directives)) {
         // MATCHED DirectiveList
      } else return ATfalse;

      ATermList tail = (ATermList) ATmake("<term>", t_table_option_list);
      // Match OrdinaryTableOptions
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_OrdinaryTableItemDeclaration(head)) {
            // MATCHED OrdinaryTableItemDeclaration
         }
         else if (traverse_NullDeclaration(head)) {
            // MATCHED NullDeclaration
         }
         else return ATfalse;
      }
   }
   else if (traverse_OrdinaryTableItemDeclaration(term)) {
      // MATCHED OrdinaryTableItemDeclaration
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_OrdinaryTableItemDeclaration(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OrdinaryTableItemDeclaration: %s\n", ATwriteToString(term));
#endif

   ATerm t_name, t_item_desc, t_pack_spec, t_preset;
   char* name;

   SgType* item_type = nullptr;
   SgExpression* preset = nullptr;
   SgVariableDeclaration* var_decl = nullptr;
   Sawyer::Optional<SgExpression*> status_size;

   std::list<SgInitializedName*> status_list;

   std::string label = "";

   if (ATmatch(term, "OrdinaryTableItemDeclaration(<term>,<term>,<term>,<term>)", &t_name,&t_item_desc,&t_pack_spec,&t_preset)) {

      if (ATmatch(t_name, "<str>", &name)) {
         // MATCHED TableItemName
      } else return ATfalse;

      if (traverse_ItemTypeDescription(t_item_desc, item_type)) {
         // MATCHED ItemTypeDescription
      } else if (traverse_StatusItemDescription(t_item_desc, status_list, status_size)) {
         // MATCHED StatusItemDescription

         // status item declarations have to be handled differently than other ItemTypeDescription terms

         // also assume an int is sufficient for status_size for now
      } else return ATfalse;

      if (traverse_TablePreset(t_preset, preset)) {
         // MATCHED TablePreset
      } else return ATfalse;

   } else return ATfalse;

   if (item_type == NULL) {
      cerr << "WARNING UNIMPLEMENTED: OrdinaryTableItemDeclaration - item_type is null \n";
      ROSE_ASSERT(false);
   }

// Begin SageTreeBuilder
   sage_tree_builder.Enter(var_decl, std::string(name), item_type, preset);
   setSourcePosition(var_decl, term);

   sage_tree_builder.Leave(var_decl);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_OptPackingSpecifier(ATerm term, PackingSpecifier &packing_spec)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptPackingSpecifier: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "no-packing-specifier()")) {
      packing_spec = e_packing_spec_unknown;
   }
   else if (ATmatch(term, "PackingSpecifierN()")) {
      packing_spec = e_packing_spec_none;
   }
   else if (ATmatch(term, "PackingSpecifierM()")) {
      packing_spec = e_packing_spec_mixed;
   }
   else if (ATmatch(term, "PackingSpecifierD()")) {
      packing_spec = e_packing_spec_dense;
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.2.4 SPECIFIED TABLE ENTRIES
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::
traverse_SpecifiedEntrySpecifierType(ATerm term, SgType* &type, LocationSpecifier &loc_spec, SgExpression* &preset, SgExprListExp* attr_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SpecifiedEntrySpecifierType: %s\n", ATwriteToString(term));
#endif

   ATerm t_words, t_item_desc, t_preset;

   type = nullptr;
   preset = nullptr;

   Sawyer::Optional<SgExpression*> words_per_entry;

   if (ATmatch(term, "SpecifiedEntrySpecifier(<term>,<term>,<term>)", &t_words, &t_item_desc, &t_preset)) {

      if (traverse_WordsPerEntry(t_words, words_per_entry)) {
         // MATCHED WordsPerEntry
      } else return ATfalse;

      if (traverse_SpecifiedItemDescription(t_item_desc, type, loc_spec, attr_list)) {
         // MATCHED SpecifiedItemDescription
      } else return ATfalse;
      
      if (traverse_TablePreset(t_preset, preset)) {
         // MATCHED TablePreset
      } else return ATfalse;
   }
   else return ATfalse;

   if (words_per_entry) {
      cout << ".x. TODO_ADD_ME!!! words_per_entry is " << *words_per_entry << ": " << (*words_per_entry)->class_name() << endl;
   }
   ROSE_ASSERT(false);

   ROSE_ASSERT(type);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::
traverse_SpecifiedEntrySpecifierBody(ATerm term, SgJovialTableStatement* table_decl, SgExpression* &preset)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SpecifiedEntrySpecifierBody: %s\n", ATwriteToString(term));
#endif

   ATerm t_words, t_preset, t_body;
   Sawyer::Optional<SgExpression*> words_per_entry;

   preset = nullptr;

   if (ATmatch(term, "SpecifiedEntrySpecifierBody(<term>,<term>,<term>)", &t_words, &t_preset, &t_body)) {

      if (traverse_WordsPerEntry(t_words, words_per_entry)) {
         // MATCHED WordsPerEntry
      } else return ATfalse;

      if (traverse_TablePreset(t_preset, preset)) {
         // MATCHED TablePreset
      } else return ATfalse;

      if (traverse_SpecifiedTableBody(t_body)) {
         // MATCHED SpecifiedTableBody
      } else return ATfalse;

   }
   else return ATfalse;

   if (words_per_entry) {
      table_decl->set_has_table_entry_size(true);
      table_decl->set_table_entry_size(*words_per_entry);
      (*words_per_entry)->set_parent(table_decl);
   }

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_SpecifiedItemDescription(ATerm term, SgType* &type, LocationSpecifier &loc_spec, SgExprListExp* attr_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SpecifiedItemDescription: %s\n", ATwriteToString(term));
#endif

   ATerm t_item_desc, t_loc_spec, t_start_bit, t_start_word;

   ROSE_ASSERT(attr_list);

   SgExpression* start_bit = nullptr;
   SgExpression* start_word = nullptr;

// For StatusItemDescription
   Sawyer::Optional<SgExpression*> status_size;
   std::list<SgInitializedName*> status_list;

   type = nullptr;
   std::string label = "";

   if (ATmatch(term, "SpecifiedItemDescription(<term>,<term>)", &t_item_desc, &t_loc_spec)) {

      if (traverse_ItemTypeDescription(t_item_desc, type)) {
         // MATCHED ItemTypeDescription
      }  else if (traverse_StatusItemDescription(t_item_desc, status_list, status_size)) {
         // MATCHED StatusItemDescription
         // status item declarations have to be handled differently than other ItemTypeDescription terms

         cerr << "WARNING UNIMPLEMENTED: SpecifiedItemDescription - StatusItemDescription\n";
         ROSE_ASSERT(false);
      } else return ATfalse;

      // process location-specifier here (don't really need to call a function)
      if (ATmatch(t_loc_spec, "LocationSpecifier(<term>,<term>)", &t_start_bit, &t_start_word)) {

         if (traverse_Formula(t_start_bit, start_bit)) {
            // MATCHED StartingBit
         } else if (ATmatch(term, "StartingBitSTAR()")) {
            cerr << "WARNING UNIMPLEMENTED: SpecifiedItemDescription - StartingBitSTAR \n";
            ROSE_ASSERT(false);
         } else return ATfalse;

         if (traverse_Formula(t_start_word, start_word)) {
            // MATCHED StartingWord
         } else return ATfalse;

         loc_spec = LocationSpecifier(start_bit, start_word);
      }

   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_SpecifiedTableBody(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SpecifiedTableBody: %s\n", ATwriteToString(term));
#endif

   ATerm t_table_option_list;

   if (ATmatch(term, "SpecifiedTableBody(<term>)", &t_table_option_list)) {
      ATermList tail = (ATermList) ATmake("<term>", t_table_option_list);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_SpecifiedTableItemDeclaration(head)) {
            // MATCHED SpecifiedTableItemDeclaration
         }
         else if (traverse_NullDeclaration(head)) {
            // MATCHED NullDeclaration
         } else return ATfalse;
      }
   }

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_SpecifiedTableItemDeclaration(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SpecifiedTableItemDeclaration: %s\n", ATwriteToString(term));
#endif

   ATerm t_spec_item_desc, t_amb, t_name, t_item_desc, t_preset;
   char* name;

   SgType* item_type = nullptr;
   SgExpression* preset = nullptr;
   SgExprListExp* attr_list = nullptr;
   LocationSpecifier loc_spec(nullptr,nullptr);
   SgVariableDeclaration* var_decl = nullptr;

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

      attr_list = SageBuilder::buildExprListExp();
      setSourcePosition(attr_list, t_item_desc);

      if (traverse_SpecifiedItemDescription(t_item_desc, item_type, loc_spec, attr_list)) {
         // MATCHED SpecifiedItemDescription
      } else return ATfalse;

      if (traverse_TablePreset(t_preset, preset)) {
         // MATCHED TablePreset
      } else return ATfalse;

   } else return ATfalse;

   ROSE_ASSERT(attr_list);

   if (item_type == nullptr) {
      cerr << "WARNING UNIMPLEMENTED: SpecifiedTableItemDeclaration - declared type is null \n";
      return ATtrue;
   }

// Begin SageTreeBuilder
   sage_tree_builder.Enter(var_decl, std::string(name), item_type, preset);
   setSourcePosition(var_decl, term);

// The bitfield is used to contain both the start_bit and start_word as an expression list
   setLocationSpecifier(var_decl, loc_spec);

// End SageTreeBuilder
   sage_tree_builder.Leave(var_decl);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_WordsPerEntry(ATerm term, Sawyer::Optional<SgExpression*> &words_per_entry)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_WordsPerEntry: %s\n", ATwriteToString(term));
#endif

   ATerm t_size;

   SgExpression* entry_size = nullptr;

   if (ATmatch(term, "WordsPerEntryW(<term>)", &t_size)) {
      if (ATmatch(term, "no-entry-size()")) {
         // MATCHED no-entry-list
         words_per_entry = Sawyer::Nothing();
      }
      else if (traverse_NumericFormula(t_size, entry_size)) {
         words_per_entry = Sawyer::Optional<SgExpression*>(entry_size);
      }
      else return ATfalse;
   }
   else if (ATmatch(term, "WordsPerEntryV()")) {
   // TODO - fix ROSETTA so this doesn't depend on NULL for entry size, has_table_entry_size should be table_entry_enum (or some such)
      words_per_entry = Sawyer::Optional<SgExpression*>(nullptr);
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.3 CONSTANT DECLARATIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_ConstantDeclaration(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ConstantDeclaration: %s\n", ATwriteToString(term));
#endif

   ATerm t_name, t_type, t_preset, t_dim_list, t_table_desc;
   char* name;

   SgType* declared_type = nullptr;
   SgExpression* preset = nullptr;
   Sawyer::Optional<SgExpression*> status_size;
   std::list<SgInitializedName*> status_list;

   std::string label = "";

   if (ATmatch(term, "ConstantItemDeclaration(<term>,<term>,<term>)", &t_name, &t_type, &t_preset)) {
      if (ATmatch(t_name, "<str>", &name)) {
         // MATCHED ItemName
      } else return ATfalse;

      if (traverse_ItemTypeDescription(t_type, declared_type)) {
         // MATCHED ItemTypeDescription
      } else if (traverse_StatusItemDescription(t_type, status_list, status_size)) {
         cerr << "WARNING UNIMPLEMENTED: StatusItemDescription \n";
         // ROSE_ASSERT(false);
         return ATtrue;
         // status item declarations have to be handled differently than other ItemTypeDescription terms
         // also assume an int is sufficient for status_size for now
      } else return ATfalse;

      if (traverse_ItemPreset(t_preset, preset)) {
         // MATCHED ItemPreset
      } else return ATfalse;
   }

   else if (ATmatch(term, "ConstantTableDeclaration(<term>,<term>,<term>)", &t_name,&t_dim_list,&t_table_desc)) {
      cerr << "WARNING UNIMPLEMENTED: ConstantTableDeclaration \n";
      return ATtrue;
   }
   else return ATfalse;

   if (declared_type == nullptr) {
      cerr << "WARNING UNIMPLEMENTED: ConstantDeclaration - type is null \n";
      ROSE_ASSERT(false);
   }

// Begin SageTreeBuilder
   SgVariableDeclaration* var_decl;
   sage_tree_builder.Enter(var_decl, std::string(name), declared_type, preset);
   setSourcePosition(var_decl, term);

// This is a ConstanItemDeclaration
   var_decl->get_declarationModifier().get_typeModifier().get_constVolatileModifier().setConst();

// End SageTreeBuilder
   sage_tree_builder.Leave(var_decl);

   return ATtrue;
}

//========================================================================================
// 2.1.4 BLOCK DECLARATION
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_BlockDeclaration(ATerm term, int def_or_ref)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BlockDeclaration: %s\n", ATwriteToString(term));
#endif

   ATerm t_name, t_alloc, t_body, t_type_name, t_preset;
   std::string block_name, block_type_name;
   bool is_anon = false;

   // Begin SageTreeBuilder
   Sawyer::Optional<LanguageTranslation::ExpressionKind> modifier_enum;
   SgJovialTableStatement* block_decl = nullptr;
   SgExprListExp* preset_list = nullptr;
   SgType* type = nullptr;
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

      // Begin SageTreeBuilder for type declaration
      Rose::builder::SourcePositionPair sources;
      sage_tree_builder.Enter(block_decl, type_name, sources, /*is_block*/ true);

      if (traverse_OptAllocationSpecifier(t_alloc, modifier_enum)) {
         // MATCHED OptAllocationSpecifier
      } else return ATfalse;

      if (traverse_BlockBodyPart(t_body, block_decl)) {
         // MATCHED BlockBodyPart
      } else return ATfalse;

      // End SageTreeBuilder for type declaration
      type = isSgJovialTableType(block_decl->get_type());
      sage_tree_builder.Leave(block_decl);
   }

   else if (ATmatch(term, "BlockDeclarationTypeName(<term>,<term>,<term>,<term>)", &t_name, &t_alloc, &t_type_name, &t_preset)) {

      if (traverse_Name(t_name, block_name)) {
         // MATCHED BlockName
      } else return ATfalse;

      if (traverse_Name(t_type_name, block_type_name)) {
         // MATCHED BlockTypeName
      } else return ATfalse;

      ROSE_ASSERT(block_type_name.length() > 0);

      // This type should have already been created by a type declaration statement
      SgClassSymbol* class_symbol = SageInterface::lookupClassSymbolInParentScopes(block_type_name, SageBuilder::topScopeStack());
      if (class_symbol != NULL) {
         type = class_symbol->get_type();
      }
      ROSE_ASSERT(type != nullptr);

      if (traverse_OptAllocationSpecifier(t_alloc, modifier_enum)) {
         // MATCHED OptAllocationSpecifier
      } else return ATfalse;

      if (traverse_BlockPreset(t_preset, preset_list)) {
         // MATCHED BlockPreset
      } else return ATfalse;
   }
   else return ATfalse;

   // Begin SageTreeBuilder for variable declaration
   SgVariableDeclaration* var_decl = nullptr;
   sage_tree_builder.Enter(var_decl, std::string(block_name), type, preset_list);
   setSourcePosition(var_decl, term);

   // Begin language specific constructs
   setDeclarationModifier(var_decl, def_or_ref);
   if (modifier_enum) {
      if (*modifier_enum == e_storage_modifier_static) {
      // Set static on both modifier attributes, JovialStatic is used for unparsing
         var_decl->get_declarationModifier().setJovialStatic();
         var_decl->get_declarationModifier().get_storageModifier().setStatic();
      }
   }

   if (is_anon) {
      SgJovialTableStatement* def_decl = isSgJovialTableStatement(block_decl->get_definingDeclaration());
      ROSE_ASSERT(def_decl);
      SageInterface::setBaseTypeDefiningDeclaration(var_decl, def_decl);
   }

   // End SageTreeBuilder for variable declaration
   sage_tree_builder.Leave(var_decl);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_BlockBodyPart(ATerm term, SgJovialTableStatement* block_decl)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BlockBodyPart: %s\n", ATwriteToString(term));
#endif

   ATerm t_dirs, t_body_options;

   if (ATmatch(term, "BlockBodyPart(<term>,<term>)", &t_dirs, &t_body_options)) {
      if (traverse_DirectiveList(t_dirs)) {
         // MATCHED OrderDirective*
      } else return ATfalse;

      ATermList tail = (ATermList) ATmake("<term>", t_body_options);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_DataDeclaration(head)) {
            // MATCHED DataDeclaration
         }
         else if (traverse_OverlayDeclaration(head)) {
            // MATCHED OverlayDeclaration
         }
         else if (traverse_NullDeclaration(head)) {
            // MATCHED NullDeclaration
         } else return ATfalse;
      }
   }
   else if (traverse_NullDeclaration(term)) {
      // MATCHED NullDeclaration
   }
   else if (traverse_DataDeclaration(term)) {
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

   SgExpression* preset = nullptr;

   ATermList tail = (ATermList) ATmake("<term>", term);
   while (! ATisEmpty(tail)) {
      ATerm head = ATgetFirst(tail);
      tail = ATgetNext(tail);
   // TODO
      ROSE_ASSERT(false);
      if (traverse_PresetValuesOption(head, preset)) {
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
ATbool ATermToSageJovialTraversal::
traverse_OptAllocationSpecifier(ATerm term, Sawyer::Optional<LanguageTranslation::ExpressionKind> &modifier_enum)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptAllocationSpecifier: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "no-allocation-specifier()")) {
      modifier_enum = Sawyer::Nothing();
   }
   else if (ATmatch(term, "STATIC()")) {
      modifier_enum = Sawyer::Optional<LanguageTranslation::ExpressionKind>(e_storage_modifier_static);
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.6 INITIALIZATION OF DATA OBJECTS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_ItemPreset(ATerm term, SgExpression* &preset)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ItemPreset: %s\n", ATwriteToString(term));
#endif

   ATerm t_preset_value;
   preset = nullptr;

   if (ATmatch(term, "no-item-preset()")) {
      // MATCHED no-item-preset
   }
   else if (ATmatch(term, "ItemPreset(<term>)", &t_preset_value)) {
      if (traverse_ItemPresetValue(t_preset_value, preset)) {
         // MATCHED ItemPresetValue
         if (preset == nullptr) {
         // DELETE_ME - perhaps this is the LOC function
            cerr << "WARNING UNIMPLEMENTED: ItemPreset (perhaps this is LOC) \n";
            return ATtrue;
         }
      } else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_OptItemPresetValue(ATerm term, SgExpression* &preset)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptItemPresetValue: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "no-item-preset-value()")) {
      // MATCHED no-item-preset-value
   }
   else if (traverse_ItemPresetValue(term, preset)) {
      // MATCHED ItemPresetValue
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_ItemPresetValue(ATerm term, SgExpression* &preset)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ItemPresetValue: %s\n", ATwriteToString(term));
#endif

   preset = nullptr;
   SgFunctionCallExp* func_call = nullptr;

   // CompileTimeFormula -> ItemPresetValue
   if (traverse_Formula(term, preset)) {
      // MATCHED CompileTimeFormula
   }
   // LocFunction -> ItemPresetValue
   else if (traverse_LocFunction(term, func_call)) {
      // MATCHED LocFunction
      preset = func_call;
   }
   else return ATfalse;

 // DELETE_ME - perhaps this is the LOC function
   if (preset == nullptr) {
      cerr << "WARNING UNIMPLEMENTED: ItemPresetValue \n";
      return ATtrue;
   }
 // ROSE_ASSERT(preset);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_TablePreset(ATerm term, SgExpression* &table_preset)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_TablePreset: %s\n", ATwriteToString(term));
#endif

   ATerm t_preset_list;

   table_preset = nullptr;

   if (ATmatch(term, "no-table-preset()")) {
      // MATCHED no-table-preset
   }
   else if (ATmatch(term, "TablePreset(<term>)", &t_preset_list)) {
      SgExprListExp* preset_list = SageBuilder::buildExprListExp_nfi();

      if (traverse_TablePresetList(t_preset_list, preset_list)) {
         // MATCHED TablePresetList
      } else return ATfalse;

      table_preset = preset_list;
      setSourcePosition(table_preset, t_preset_list);
   }
   else return ATfalse;

   return ATtrue;
}

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
   // TODO_COMPOOL
      cerr << "WARNING UNIMPLEMENTED: DefaultPresetSublist\n";
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_DefaultPresetSublist(ATerm term, SgExprListExp* preset_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_DefaultPresetSublist: %s\n", ATwriteToString(term));
#endif

   ATerm t_default_preset_list;
   SgExpression* preset = nullptr;

   if (ATmatch(term, "DefaultPresetSublist(<term>)", &t_default_preset_list)) {
   // TODO
      cerr << "WARNING UNIMPLEMENTED: DefaultPresetSublist\n";

      ATermList tail = (ATermList) ATmake("<term>", t_default_preset_list);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_PresetValuesOption(head, preset)) {
            // MATCHED PresetValuesOption: NOTE - this is optional so preset expression may be NULL
            if (preset != nullptr) {
               preset_list->get_expressions().push_back(preset);
               preset->set_parent(preset_list);
            }
         } else return ATfalse;
      }
   }
   else if (ATmatch(term, "no-default-preset-sublist")) {
      // MATCHED no-default-preset-sublist
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_SpecifiedPresetSublist(ATerm term, SgExprListExp* preset_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SpecifiedPresetSublist: %s\n", ATwriteToString(term));
#endif

   ATerm t_preset_index_spec, t_preset_values_option;
   SgExpression* preset;

   if (ATmatch(term, "SpecifiedPresetSublist(<term>,<term>)", &t_preset_index_spec, &t_preset_values_option)) {
   // TODO_COMPOOL
      SgInitializer* sg_preset = nullptr;
      cerr << "WARNING UNIMPLEMENTED: SpecifiedPresetSublist\n";

      if (traverse_PresetIndexSpecifier(t_preset_index_spec, sg_preset)) {
         // MATCHED PresetIndexSpecifier
      } else return ATfalse;

      ATermList tail = (ATermList) ATmake("<term>", t_preset_values_option);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_PresetValuesOption(head, preset)) {
            // MATCHED PresetValuesOption, optional so ok if nullptr
            if (preset != nullptr) {
               preset_list->get_expressions().push_back(preset);
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

// Begin SageTreeBuilder
   SgExpression* sg_expr = nullptr;

   if (ATmatch(term, "PresetIndexSpecifier(<term>)", &t_const_index)) {
      ATermList tail = (ATermList) ATmake("<term>", t_const_index);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_NumericFormula(head, sg_expr)) {
            // MATCHED CompileTimeNumericFormula
         // TODO_COMPOOL
         // ROSE_ASSERT(sg_expr);
//DELETE_ME preset->get_expressions().push_back(expr);
         } else if (traverse_StatusFormula(head, sg_expr)) {
            // MATCHED CompileTimeStatusFormula
         // TODO_COMPOOL
         // ROSE_ASSERT(sg_expr);
//DELETE_ME preset->get_expressions().push_back(expr);
         } else return ATfalse;
      }
   // TODO_COMPOOL
      cerr << "WARNING UNIMPLEMENTED: PresetIndexSpecifier\n";
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_PresetValuesOption(ATerm term, SgExpression* &preset)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_PresetValuesOption: %s\n", ATwriteToString(term));
#endif

   ATerm t_rep_count, t_item_preset_value;

   preset = nullptr;

   if (ATmatch(term, "PresetValuesOption(<term>)", &t_item_preset_value)) {
      if (traverse_OptItemPresetValue(t_item_preset_value, preset)) {
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
ATbool ATermToSageJovialTraversal::traverse_TypeDeclaration(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_TypeDeclaration: %s\n", ATwriteToString(term));
#endif

   if (traverse_ItemTypeDeclaration(term)) {
      // MATCHED ItemTypeDeclaration
   }
   else if (traverse_TableTypeDeclaration(term)) {
      // MATCHED TableTypeDeclaration
   }
   else if (traverse_BlockTypeDeclaration(term)) {
      // MATCHED BlockTypeDeclaration
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_ItemTypeDeclaration(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ItemTypeDeclaration: %s\n", ATwriteToString(term));
#endif

   ATerm t_name, t_type_desc;
   std::string name;

   SgType* declared_type = nullptr;

// For StatusItemDescription
   Sawyer::Optional<SgExpression*> status_size;
   std::list<SgInitializedName*> status_list;

   std::string label = "";

   if (ATmatch(term, "ItemTypeDeclaration(<term>,<term>)", &t_name, &t_type_desc)) {

      if (traverse_Name(t_name, name)) {
         // MATCHED ItemTypeName
      } else return ATfalse;

      if (traverse_ItemTypeDescription(t_type_desc, declared_type)) {
         // MATCHED ItemTypeDescription

         if (declared_type == nullptr) {
            cerr << "WARNING UNIMPLEMENTED: ItemTypeDeclaration - declared_type is nullptr \n";
         // ROSE_ASSERT(false);
         // DELETE_ME
            return ATtrue;
         }

      // Begin SageTreeBuilder
         SgTypedefDeclaration* type_def = nullptr;
         sage_tree_builder.Enter(type_def, name, declared_type);
         setSourcePosition(type_def, term);

      // End SageTreeBuilder
         sage_tree_builder.Leave(type_def);
      }
      else if (traverse_StatusItemDescription(t_type_desc, status_list, status_size)) {
         // MATCHED StatusItemDescription

         // status item declarations have to be handled differently than other ItemTypeDescription terms

         // also assume an int is sufficient for status_size for now
         if (status_size) {
            cerr << "WARNING UNIMPLEMENTED: ItemTypeDeclaration - StatusItemDescription - has_size \n";
         // ROSE_ASSERT(false);
         // DELETE_ME
            return ATtrue;
         }

      // Begin SageTreeBuilder
         SgEnumDeclaration* enum_decl = nullptr;
         sage_tree_builder.Enter(enum_decl, name, status_list);
         setSourcePosition(enum_decl, term);

      // End SageTreeBuilder
         sage_tree_builder.Leave(enum_decl);
      }
      else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_TableTypeDeclaration(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_TableTypeDeclaration: %s\n", ATwriteToString(term));
#endif

   ATerm t_name, t_type_desc;
   std::string type_name;

   SgJovialTableStatement* table_decl = nullptr;

   if (ATmatch(term, "TableTypeDeclaration(<term>,<term>)", &t_name, &t_type_desc)) {
      if (traverse_Name(t_name, type_name)) {
         // MATCHED TableTypeName
      } else return ATfalse;

   // Begin SageTreeBuilder
      Rose::builder::SourcePositionPair sources;
      sage_tree_builder.Enter(table_decl, type_name, sources);

      if (traverse_TableTypeSpecifier(t_type_desc, table_decl)) {
         // MATCHED TableTypeSpecifier
      } else return ATfalse;

   }
   else return ATfalse;

// End SageTreeBuilder
   sage_tree_builder.Leave(table_decl);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::
traverse_TableTypeSpecifier(ATerm term, SgJovialTableStatement* table_decl)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_TableTypeSpecifier: %s\n", ATwriteToString(term));
#endif

// A TableTypeSpecifier looks a base class name or it is a primitive type
//
   SgType* base_type = nullptr;
   SgJovialTableType* parent_type = nullptr;

   ROSE_ASSERT(table_decl);

   ATerm t_dim_list, t_struct_spec, t_like_option, t_entry_spec, t_type_name;
   std::string table_type_name, like_name;

   bool has_table_type_name = false;
   bool has_like_option = false;

// Begin SageTreeBuilder
   SgJovialTableType* table_type = isSgJovialTableType(table_decl->get_type());
   ROSE_ASSERT(table_type);

   SgExprListExp* dim_info = table_type->get_dim_info();

// TableTypeSpecifier with a name
   if (ATmatch(term, "TableTypeSpecifierName(<term>,<term>)", &t_dim_list, &t_type_name)) {

      if (dim_info == nullptr) {
         dim_info = SageBuilder::buildExprListExp_nfi();
      }

      if (traverse_OptDimensionList(t_dim_list, dim_info)) {
         // MATCHED OptDimensionList
      } else return ATfalse;

      if (traverse_Name(t_type_name, table_type_name)) {
         // MATCHED TableTypeName
         has_table_type_name = true;
      } else return ATfalse;

   // This type should have already been created by a type declaration statement, find it
      SgClassSymbol* class_symbol = SageInterface::lookupClassSymbolInParentScopes(table_type_name, SageBuilder::topScopeStack());
      if (class_symbol != NULL) {
         parent_type = isSgJovialTableType(class_symbol->get_type());
      }
      ROSE_ASSERT(parent_type);

      // 1. The class declaration for the base class should be the first non-defining declaration of the base class
      // 2. The class definition should be the class definition of the derived class
      //
      SgClassDeclaration* base_class_decl = isSgClassDeclaration(parent_type->get_declaration());
      ROSE_ASSERT(base_class_decl);
      
   // DQ (12/26/2011): The non defining declaration should not have a valid pointer to the class definition.
      ROSE_ASSERT(base_class_decl->get_definition() == NULL);

      SgClassDefinition* derived_class_def = table_decl->get_definition();
      ROSE_ASSERT(derived_class_def);

      // The sage builder function sets the parent of the base class to the inherited class
      // definition and adds the base class to its inheritances list.
      SgBaseClass* base_class = SageBuilder::buildBaseClass(base_class_decl, derived_class_def, false, /*isDirect*/true);
      ROSE_ASSERT(base_class);
   }

// TableTypeSpecifier with four arguments where t_entry_spec specifies the type (could be primitive or anonymous)
   else if (ATmatch(term, "TableTypeSpecifier(<term>,<term>,<term>,<term>)",
                          &t_dim_list, &t_struct_spec, &t_like_option, &t_entry_spec)) {

      TableSpecifier table_spec;
      cerr << "WARNING UNIMPLEMENTED: TableTypeSpecifier - table_spec \n";

      StructureSpecifier& struct_spec = table_spec.struct_spec;

      if (dim_info == nullptr) {
         dim_info = SageBuilder::buildExprListExp_nfi();
      }

      if (traverse_OptDimensionList(t_dim_list, dim_info)) {
         // MATCHED OptDimensionList
      } else return ATfalse;

   // Structure specifier
      if (traverse_OptStructureSpecifier(t_struct_spec, struct_spec)) {
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
            ROSE_ASSERT(false);
         } else return ATfalse;
      }
      else return ATfalse;

   // TODO - something with these
      SgExpression* preset = nullptr;
      SgExprListExp* attr_list = nullptr;
      LocationSpecifier loc_spec;

   // Entry specifier without a body
      if (traverse_EntrySpecifierType(t_entry_spec, base_type, loc_spec, preset, attr_list, table_spec)) {
         // MATCHED EntrySpecifier
      }
   // Entry specifier with a body
      else if (traverse_EntrySpecifierBody(t_entry_spec, table_decl, preset, table_spec)) {
         // MATCHED EntrySpecifierBody
      }
      else return ATfalse;
   }
   else return ATfalse;

   table_type->set_dim_info(dim_info);
   dim_info->set_parent(table_type);

   if (base_type) {
      table_type->set_base_type(base_type);
      base_type->set_parent(table_type);
   }

#if 0
   std::cout << ".x. TABLE DECLARATION for type " << table_type_name << endl;
   std::cout << ".x. TABLE TYPE SPEC rank is "     << dim_info->get_expressions().size() << endl;
   std::cout << ".x. TABLE TYPE SPEC dim_info: "   << dim_info << endl;
   if (base_type) {
      std::cout << ".x. base_type is " << base_type << ": " << base_type->class_name() << endl;
   }
#endif

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_BlockTypeDeclaration(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BlockTypeDeclaration: %s\n", ATwriteToString(term));
#endif

   ATerm t_name, t_type_desc;
   std::string type_name;

   SgJovialTableStatement* block_decl = nullptr;

   if (ATmatch(term, "BlockTypeDeclaration(<term>,<term>)", &t_name, &t_type_desc)) {
      if (traverse_Name(t_name, type_name)) {
         // MATCHED BlockTypeName
      } else return ATfalse;

      // Begin SageTreeBuilder
      Rose::builder::SourcePositionPair sources;
      sage_tree_builder.Enter(block_decl, type_name, sources, /*is_block*/ true);

      if (traverse_BlockBodyPart(t_type_desc, block_decl)) {
         // MATCHED BlockBodyPart
      }
      else if (traverse_DataDeclaration(t_type_desc)) {
         // MATCHED DataDeclaration -> BlockBodyPart
      }
      else if (traverse_NullDeclaration(t_type_desc)) {
         // MATCHED NullDeclaration -> BlockBodyPart
      }
      else return ATfalse;

   }
   else return ATfalse;

   // End SageTreeBuilder
   sage_tree_builder.Leave(block_decl);

   return ATtrue;
}

//========================================================================================
// 2.3 STATEMENT NAME DECLARATIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_StatementNameDeclaration(ATerm term, int def_or_ref)
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
   int stmt_enum = LanguageTranslation::e_unknown;
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
ATbool ATermToSageJovialTraversal::traverse_DefineDeclaration(ATerm term)
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

   SgJovialDefineDeclaration* define_decl = SageBuilder::buildJovialDefineDeclaration_nfi(name, params, def_string);
   setSourcePosition(define_decl, term);

   SageInterface::appendStatement(define_decl, SageBuilder::topScopeStack());

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
ATbool ATermToSageJovialTraversal::traverse_ExternalDeclaration(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ExternalDeclaration: %s\n", ATwriteToString(term));
#endif

   if (traverse_SimpleDef(term)) {
      // MATCHED SimpleDef -> DefSpecification
   } else if (traverse_CompoundDef(term)) {
      // MATCHED CompoundDef -> DefSpecification
   } else if (traverse_SimpleRef(term)) {
      // MATCHED SimpleRef -> RefSpecification
   } else if (traverse_CompoundRef(term)) {
      // MATCHED CompoundRef -> RefSpecification
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.5.1 DEF SPECIFICATIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_SimpleDef(ATerm term)
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

   if (traverse_DefSpecificationChoice(t_def)) {
      // MATCHED DefSpecificationChoice
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_CompoundDef(ATerm term)
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
         if (traverse_DefSpecificationChoice(head)) {
            // MATCHED DefSpecificationChoice
         } else return ATfalse;
      }
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_DefSpecificationChoice(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_DefSpecificationChoice: %s\n", ATwriteToString(term));
#endif

   // This is a 'DEF' declaration
   int def_spec = LanguageTranslation::e_storage_modifier_jovial_def;

   if (traverse_NullDeclaration(term)) {
      // MATCHED NullDeclaration
   } else if (traverse_DataDeclaration(term, def_spec)) {
      // MATCHED DataDeclaration
   } else if (traverse_StatementNameDeclaration(term, def_spec)) {
      // MATCHED StatementNameDeclaration
   } else if (traverse_DefBlockInstantiation(term)) {
      // MATCHED DefBlockInstantiation
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_DefBlockInstantiation(ATerm term)
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
ATbool ATermToSageJovialTraversal::traverse_SimpleRef(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SimpleRef: %s\n", ATwriteToString(term));
#endif

   ATerm t_ref;

   if (ATmatch(term, "SimpleRef(<term>)", &t_ref)) {
      if (traverse_RefSpecificationChoice(t_ref)) {
         // MATCHED RefSpecificationChoice
      } else return ATfalse;
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_CompoundRef(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CompoundRef: %s\n", ATwriteToString(term));
#endif

   ATerm ref_spec;

   if (ATmatch(term, "CompoundRef(<term>)" , &ref_spec)) {
      ATermList tail = (ATermList) ATmake("<term>", ref_spec);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_RefSpecificationChoice(head)) {
            // MATCHED RefSpecificationChoice
         } else return ATfalse;
      }
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_RefSpecificationChoice(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_RefSpecificationChoice: %s\n", ATwriteToString(term));
#endif

   // This is an 'REF' declaration
   //
   int ref_spec = LanguageTranslation::e_storage_modifier_jovial_ref;

   LanguageTranslation::FunctionModifierList function_modifiers;
   function_modifiers.push_back(LanguageTranslation::e_function_modifier_reference);

   if (traverse_NullDeclaration(term)) {
      // MATCHED NullDeclaration
   } else if (traverse_DataDeclaration(term, ref_spec)) {
      // MATCHED DataDeclaration
   } else if (traverse_StatementNameDeclaration(term, ref_spec)) {
      // MATCHED StatementNameDeclaration
   } else if (traverse_FunctionDeclaration(term, function_modifiers)) {
      // MATCHED FunctionDeclaration (is a SubroutineDeclaration in grammar)
   } else if (traverse_ProcedureDeclaration(term, function_modifiers)) {
      // MATCHED ProcedureDeclaration (is a SubroutineDeclaration in grammar)
   } else return ATfalse;


   return ATtrue;
}

//========================================================================================
// 2.6 OVERLAY DECLARATIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_OverlayDeclaration(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OverlayDeclaration: %s\n", ATwriteToString(term));
#endif

   // 'OVERLAY'
   //   OptAbsoluteAddress
   //   OverlayExpression ';'         -> OverlayDeclaration   {cons("OverlayDeclaration")}

   ATerm t_addr, t_absolute_addr, t_expr;
   SgExpression *sg_addr = nullptr;
   SgExpression *expr = nullptr;

   if (ATmatch(term, "OverlayDeclaration(<term>,<term>)", &t_addr, &t_expr)) {
      cerr << "WARNING UNIMPLEMENTED: OverlayDeclaration\n";
      ROSE_ASSERT(false);
      if (ATmatch(t_addr, "AbsoluteAddress(<term>)", &t_absolute_addr)) {
         // 'POS' '(' OverlayAddress ')'    -> AbsoluteAddress      {cons("AbsoluteAddress")}
         if (traverse_NumericFormula(t_absolute_addr, sg_addr)) {
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

ATbool ATermToSageJovialTraversal::traverse_OverlayExpression(ATerm term, SgExpression* & expr)
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

ATbool ATermToSageJovialTraversal::traverse_OverlayString(ATerm term, SgExpression* & expr)
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

ATbool ATermToSageJovialTraversal::traverse_OverlayElement(ATerm term, SgExpression* & expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OverlayElement: %s\n", ATwriteToString(term));
#endif

   //  Spacer                          -> OverlayElement
   //  DataName                        -> OverlayElement
   //  '(' OverlayExpression ')'       -> OverlayElement       {cons("OverlayElement")}

   ATerm t_expr;
   std::string name;
   SgExpression *spacer; // *overlay_expr;

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

ATbool ATermToSageJovialTraversal::traverse_Spacer(ATerm term, SgExpression* & expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Spacer: %s\n", ATwriteToString(term));
#endif

   //   'W' CompileTimeNumericFormula   -> Spacer               {cons("Spacer")}

   ATerm t_num;

   if (ATmatch(term, "Spacer(<term>)", &t_num)) {
      if (traverse_NumericFormula(t_num, expr)) {
         // MATCHED NumericFormula
      } else return ATfalse;
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 3.1 PROCEDURES
//----------------------------------------------------------------------------------------

ATbool ATermToSageJovialTraversal::traverse_ProcedureDeclaration(ATerm term, LanguageTranslation::FunctionModifierList &modifiers)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ProcedureDeclaration: %s\n", ATwriteToString(term));
#endif

   ATerm t_proc_heading, t_decl;

   std::string name;
   std::list<FormalParameter> param_name_list;

   SgFunctionDeclaration* function_decl = nullptr;
   SgFunctionParameterList* param_list = nullptr;
   SgBasicBlock* param_scope = nullptr;

   if (ATmatch(term, "ProcedureDeclaration(<term>,<term>)", &t_proc_heading, &t_decl)) {

      if (traverse_ProcedureHeading(t_proc_heading, name, param_name_list, modifiers)) {
         // MATCHED ProcedureHeading
      } else return ATfalse;

   // Enter SageTreeBuilder for SgFunctionParameterList
      sage_tree_builder.Enter(param_list, param_scope);

   // These declarations will stored in the function parameter scope
      if (traverse_Declaration(t_decl)) {
         // MATCHED Declaration
      } else return ATfalse;

   // Leave SageTreeBuilder for SgFunctionParameterList
      sage_tree_builder.Leave(param_list, param_scope, param_name_list);
   }
   else return ATfalse;

// Enter SageTreeBuilder for SgFunctionDeclaration
   sage_tree_builder.Enter(function_decl, name, /*return_type*/nullptr, param_list, modifiers);

// Leave SageTreeBuilder for SgFunctionDeclaration
   sage_tree_builder.Leave(function_decl, param_scope);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_ProcedureDefinition(ATerm term, LanguageTranslation::FunctionModifierList &modifiers)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ProcedureDefinition: %s\n", ATwriteToString(term));
#endif

   ATerm t_proc_heading, t_proc_body;

   std::string name;
   std::list<FormalParameter> param_name_list;

   SgFunctionDeclaration* function_decl = nullptr;
   SgFunctionParameterList* param_list = nullptr;
   SgBasicBlock* param_scope = nullptr;

   if (ATmatch(term, "ProcedureDefinition(<term>,<term>)", &t_proc_heading, &t_proc_body)) {

      if (traverse_ProcedureHeading(t_proc_heading, name, param_name_list, modifiers)) {
         // MATCHED ProcedureHeading
      } else return ATfalse;

   // Enter SageTreeBuilder for SgFunctionParameterList
      sage_tree_builder.Enter(param_list, param_scope);

   // These declarations will stored in the function parameter scope
      if (traverse_SubroutineBody(t_proc_body)) {
         // MATCHED ProcedureBody (the production is actually a SubroutineBody)
      } else return ATfalse;

   // Leave SageTreeBuilder for SgFunctionParameterList
      sage_tree_builder.Leave(param_list, param_scope, param_name_list);

   }
   else return ATfalse;

// Enter SageTreeBuilder for SgFunctionDeclaration
   sage_tree_builder.Enter(function_decl, name, /*return_type*/nullptr, param_list, modifiers);

// Leave SageTreeBuilder for SgFunctionDeclaration
   sage_tree_builder.Leave(function_decl, param_scope);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::
traverse_ProcedureHeading(ATerm term, std::string &name, std::list<FormalParameter> &param_list,
                                      LanguageTranslation::FunctionModifierList &modifiers)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ProcedureHeading: %s\n", ATwriteToString(term));
#endif

   ATerm t_name, t_attr, t_params;

   if (ATmatch(term, "ProcedureHeading(<term>,<term>,<term>)", &t_name, &t_attr, &t_params)) {

      if (traverse_Name(t_name, name)) {
         // MATCHED Name
      } else return ATfalse;

      if (traverse_SubroutineAttribute(t_attr, modifiers)) {
         // MATCHED SubroutineAttribute
      } else return ATfalse;

      if (traverse_FormalParameterList(t_params, param_list)) {
         // MATCHED FormalParameterList
      } else return ATfalse;

   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_SubroutineAttribute(ATerm term, LanguageTranslation::FunctionModifierList &modifiers)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SubroutineAttribute: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "no-subroutine-attribute()")) {
      // no funtion modifiers
   }
   else if (ATmatch(term, "REC()")) {
      modifiers.push_back(LanguageTranslation::e_function_modifier_recursive);
   }
   else if (ATmatch(term, "RENT()")) {
      modifiers.push_back(LanguageTranslation::e_function_modifier_reentrant);
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_SubroutineBody(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SubroutineBody: %s\n", ATwriteToString(term));
#endif

   ATerm t_stmt;
   ATerm t_decls, t_stmts, t_funcs, t_labels;
   std::vector<std::string> labels;
   std::vector<PosInfo> locations;
   std::string temp_label = "";

   if (ATmatch(term, "SubroutineSimpleBody(<term>)", &t_stmt)) {
      if (traverse_Statement(t_stmt)) {
         // MATCHED Statement
      } else return ATfalse;
   }

   else if (ATmatch(term, "SubroutineBody(<term>,<term>,<term>,<term>)", &t_decls,&t_stmts,&t_funcs,&t_labels)) {
      if (traverse_DeclarationList(t_decls)) {
         // MATCHED DeclarationList
      } else return ATfalse;

      if (traverse_StatementList(t_stmts)) {
         // MATCHED StatementList
      } else return ATfalse;

      if (traverse_SubroutineDefinitionList(t_funcs)) {
         // MATCHED SubroutineDefinitionList
      } else return ATfalse;

      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 3.2 FUNCTIONS
//----------------------------------------------------------------------------------------

ATbool ATermToSageJovialTraversal::traverse_FunctionDeclaration(ATerm term, LanguageTranslation::FunctionModifierList &modifiers)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FunctionDeclaration: %s\n", ATwriteToString(term));
#endif

   ATerm t_func_heading, t_dirs, t_decl;

   std::string name;
   SgType* return_type = NULL;
   std::list<FormalParameter> param_name_list;

   SgFunctionDeclaration* function_decl = nullptr;
   SgFunctionParameterList* param_list = nullptr;
   SgBasicBlock* param_scope = nullptr;

   if (ATmatch(term, "FunctionDeclaration(<term>,<term>,<term>)", &t_func_heading, &t_dirs, &t_decl)) {

      if (traverse_FunctionHeading(t_func_heading, name, return_type, param_name_list, modifiers)) {
         // MATCHED FunctionHeading
      } else return ATfalse;

   // Enter SageTreeBuilder for SgFunctionParameterList
      sage_tree_builder.Enter(param_list, param_scope);

      if (traverse_DirectiveList(t_dirs)) {
         // MATCHED DirectiveList (grammar is ReducibleDirective*)
      } else return ATfalse;

      if (traverse_Declaration(t_decl)) {
         // MATCHED Declaration
      } else return ATfalse;

   // Leave SageTreeBuilder for SgFunctionParameterList
      sage_tree_builder.Leave(param_list, param_scope, param_name_list);
   }
   else return ATfalse;

// Enter SageTreeBuilder for SgFunctionDeclaration
   sage_tree_builder.Enter(function_decl, name, return_type, param_list, modifiers);

// Leave SageTreeBuilder for SgFunctionDeclaration
   sage_tree_builder.Leave(function_decl, param_scope);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_FunctionDefinition(ATerm term, LanguageTranslation::FunctionModifierList &modifiers)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FunctionDefinition: %s\n", ATwriteToString(term));
#endif

   ATerm t_func_heading, t_dirs, t_proc_body;

   std::string name;
   SgType* return_type = NULL;
   std::list<FormalParameter> param_list;

   if (ATmatch(term, "FunctionDefinition(<term>,<term>,<term>)", &t_func_heading, &t_dirs, &t_proc_body)) {
      cerr << "WARNING UNIMPLEMENTED: FunctionDefinition\n";
      ROSE_ASSERT(false);

      if (traverse_FunctionHeading(t_func_heading, name, return_type, param_list, modifiers)) {
         // MATCHED FunctionHeading
      } else return ATfalse;

      // OUT OF ORDER to get function scope
      // fix this

      if (traverse_SubroutineBody(t_proc_body)) {
         // MATCHED FunctionBody
      } else return ATfalse;

      if (traverse_DirectiveList(t_dirs)) {
         // MATCHED ReducibleDirective*
      } else return ATfalse;

   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::
traverse_FunctionHeading(ATerm term, std::string &name, SgType* &type, std::list<FormalParameter> &param_list,
                                     LanguageTranslation::FunctionModifierList &modifiers)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FunctionHeading: %s\n", ATwriteToString(term));
#endif

   ATerm t_name, t_type, t_attr, t_params;

// For StatusItemDescription
   Sawyer::Optional<SgExpression*> status_size;
   std::list<SgInitializedName*> status_list;

   type = nullptr;

   if (ATmatch(term, "FunctionHeading(<term>,<term>,<term>,<term>)", &t_name, &t_attr, &t_params, &t_type)) {
      if (traverse_Name(t_name, name)) {
         // MATCHED Name
      } else return ATfalse;

      if (traverse_SubroutineAttribute(t_attr, modifiers)) {
         // MATCHED SubroutineAttribute
      } else return ATfalse;

      if (traverse_FormalParameterList(t_params, param_list)) {
         // MATCHED FormalParameterList
      } else return ATfalse;

      if (traverse_ItemTypeDescription(t_type, type)) {
         // MATCHED ItemTypeDescription
      }
      else if (traverse_StatusItemDescription(t_type, status_list, status_size)) {
         // MATCHED StatusItemDescription

         // status item declarations have to be handled differently than other ItemTypeDescription terms
         cerr << "WARNING UNIMPLEMENTED: FunctionHeading - StatusItemDescription\n";
         ROSE_ASSERT(false);
      }
      else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 3.3 PARAMETERS OF PROCEDURES AND FUNCTIONS
//----------------------------------------------------------------------------------------

ATbool ATermToSageJovialTraversal::traverse_FormalParameterList(ATerm term, std::list<FormalParameter> &param_list)
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

ATbool ATermToSageJovialTraversal::traverse_FormalOutputParameters(ATerm term, std::list<FormalParameter> &param_list)
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

ATbool ATermToSageJovialTraversal::traverse_FormalInputParameter(ATerm term, std::list<FormalParameter> &param_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FormalInputParameter: %s\n", ATwriteToString(term));
#endif

   ATerm t_binding, t_name;
   std::string name;
   LanguageTranslation::ExpressionKind binding;

   if (ATmatch(term, "no-formal-parameter-list()")) {
      // no input parameters OK, return immediately
      return ATtrue;
   }

// There are input parameters (if not will have returned)
//
   if (ATmatch(term, "FormalInputParameter(<term>,<term>)", &t_binding, &t_name)) {
      if (traverse_ParameterBinding(t_binding, binding)) {
         // MATCHED ParameterBinding
      } else return ATfalse;
      if (traverse_Name(t_name, name)) {
         // MATCHED Name
      } else return ATfalse;
   }
   else return ATfalse;

   param_list.push_back(FormalParameter(name, false, binding));

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_FormalOutputParameter(ATerm term, std::list<FormalParameter> &param_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FormalOutputParameter: %s\n", ATwriteToString(term));
#endif

   ATerm t_binding, t_name;
   std::string name;
   LanguageTranslation::ExpressionKind binding;

   if (ATmatch(term, "no-formal-output-parameters()")) {
      // no output parameters OK, return immediately
      return ATtrue;
   }

// There are output parameters (if not will have returned)
//
   if (ATmatch(term, "FormalOutputParameter(<term>,<term>)", &t_binding, &t_name)) {
      if (traverse_ParameterBinding(t_binding, binding)) {
         // MATCHED ParameterBinding
      } else return ATfalse;
      if (traverse_Name(t_name, name)) {
         // MATCHED Name
      } else return ATfalse;
   }
   else return ATfalse;

   param_list.push_back(FormalParameter(name, true, binding));

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_ParameterBinding(ATerm term, LanguageTranslation::ExpressionKind &binding)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ParameterBinding: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "no-parameter-binding()")) {
      binding = e_none;
   }
   else if (ATmatch(term, "BYVAL()")) {
      binding = LanguageTranslation::e_param_binding_value;
   }
   else if (ATmatch(term, "BYREF()")) {
      binding = LanguageTranslation::e_param_binding_reference;
   }
   else if (ATmatch(term, "BYRES()")) {
      binding = LanguageTranslation::e_param_binding_result;
   }

   return ATtrue;
}

//========================================================================================
// 3.4 INLINE DECLARATIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_InlineDeclaration(ATerm term)
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
ATbool ATermToSageJovialTraversal::traverse_Statement(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Statement: %s\n", ATwriteToString(term));
#endif

   if (traverse_SimpleStatement(term)) {
      // MATCHED SimpleStatement
   } else if (traverse_CompoundStatement(term)) {
      // MATCHED CompoundStatement
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_StatementList(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_StatementList: %s\n", ATwriteToString(term));
#endif

   ATermList tail = (ATermList) ATmake("<term>", term);
   while (! ATisEmpty(tail)) {
      ATerm head = ATgetFirst(tail);
      tail = ATgetNext(tail);
      if (traverse_Statement(head)) {
         // MATCHED Statement
      } else return ATfalse;
   }

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_SimpleStatement(ATerm term)
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

      if (traverse_AssignmentStatement(t_stmt)) {
         // MATCHED AssignmentStatement
      }
      else if (traverse_NullStatement(t_stmt)) {
         // MATCHED NullStatement
      }
      else if (traverse_NullBlockStatement(t_stmt)) {
         // MATCHED NullStatement
      }
      else return ATfalse;
   }

// This subsumes the labels in statements, eventually all SimpleStatements will take this path
   else if (ATmatch(term, "SimpleStatement(<term>)", &t_stmt)) {
      if (traverse_IfStatement(t_stmt)) {
         // MATCHED IfStatement
      }
      else if (traverse_AbortStatement(t_stmt)) {
         // MATCHED AbortStatement
      }
      else if (traverse_StopStatement(t_stmt)) {
         // MATCHED StopStatement
      }
      else if (traverse_ExitStatement(t_stmt)) {
         // MATCHED ExitStatement
      }
      else if (traverse_GotoStatement(t_stmt)) {
         // MATCHED GotoStatement
      }
      else if (traverse_ReturnStatement(t_stmt)) {
         // MATCHED ReturnStatement
      }
      else if (traverse_CaseStatement(t_stmt)) {
         // MATCHED CaseStatement
      }
      else if (traverse_WhileStatement(t_stmt)) {
         // MATCHED WhileStatement
      }
      else if (traverse_ForStatement(t_stmt)) {
         // MATCHED ForStatement
      }
      else if (traverse_ProcedureCallStatement(t_stmt)) {
         // MATCHED ProcedureCallStatement
      } else if (ATmatch(t_stmt, "amb(<term>)", &t_amb)) {
         // MATCHED amb
         ATermList tail = (ATermList) ATmake("<term>", t_amb);
         ATerm head = ATgetFirst(tail);
         // chose first amb path, now traverse it

         if (traverse_IfStatement(head)) {
            // MATCHED IfStatement
         }
         else if (traverse_AbortStatement(head)) {
            // MATCHED AbortStatement
         }
         else if (traverse_StopStatement(head)) {
            // MATCHED StopStatement
         }
         else if (traverse_ExitStatement(head)) {
            // MATCHED ExitStatement
         }
         else if (traverse_GotoStatement(head)) {
            // MATCHED GotoStatement
         }
         else if (traverse_ReturnStatement(head)) {
            // MATCHED ReturnStatement
         }
         else if (traverse_CaseStatement(head)) {
            // MATCHED CaseStatement
         }
         else if (traverse_WhileStatement(head)) {
            // MATCHED WhileStatement
         }
         else if (traverse_ForStatement(head)) {
            // MATCHED ForStatement
         }
         else if (traverse_ProcedureCallStatement(head)) {
            // MATCHED ProcedureCallStatement
         } else return ATfalse;
      }
      else return ATfalse;
   }
   else return ATfalse;

   if (labels.size() > 0) {
      std::cerr << "WARNING UNIMPLEMENTED: SimpleStatement - labels.size() > 0 \n";
   }

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_CompoundStatement(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CompoundStatement: %s\n", ATwriteToString(term));
#endif

   ATerm t_labels, t_stmt, t_labels2;
   std::vector<std::string> labels, labels2;
   std::vector<PosInfo> locations, locations2;

   if (ATmatch(term, "CompoundStatement(<term>,<term>,<term>)", &t_labels, &t_stmt, &t_labels2)) {
      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;

//TODO_STATEMENTS
#if 0
      block_stmt = SageBuilder::buildUntypedBlockStatement("");
      ROSE_ASSERT(block_stmt);

      new_stmt_list = block_stmt->get_scope()->get_statement_list();

      if (traverse_StatementList(t_stmt)) {
         // MATCHED StatementList
      } else return ATfalse;

      if (traverse_LabelList(t_labels2, labels2, locations2)) {
         // MATCHED LabelList
      } else return ATfalse;
#endif

   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_NullStatement(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_NullStatement: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "NullStatement()")) {
      SgNullStatement* null_stmt = SageBuilder::buildNullStatement();
      setSourcePosition(null_stmt, term);
      SageInterface::appendStatement(null_stmt, SageBuilder::topScopeStack());
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_NullBlockStatement(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_NullBlockStatement: %s\n", ATwriteToString(term));
#endif

   ATerm t_labels;
   std::vector<std::string> labels;
   std::vector<PosInfo> locations;

   if (ATmatch(term, "NullBlockStatement(<term>)", &t_labels)) {
      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;

      SgNullStatement* null_block_stmt = SageBuilder::buildNullStatement();
      setSourcePosition(null_block_stmt, term);
      SageInterface::appendStatement(null_block_stmt, SageBuilder::topScopeStack());

   // TODO - labels
   // stmt = convert_Labels(labels, locations, null_block_stmt);
   }
   else return ATfalse;

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
ATbool ATermToSageJovialTraversal::traverse_AssignmentStatement(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_AssignmentStatement: %s\n", ATwriteToString(term));
#endif

   ATerm t_vars, t_expr;

   SgExprStatement* assign_stmt = nullptr;

   if (ATmatch(term, "AssignmentStatement(<term>,<term>)", &t_vars,&t_expr)) {
      SgExpression* rhs = nullptr;
      std::vector<SgExpression*> vars;

      if (traverse_VariableList(t_vars, vars)) {
         // MATCHED VariableList
      } else return ATfalse;

      if (traverse_Formula(t_expr, rhs)) {
         // MATCHED Formula
      } else return ATfalse;

      if (vars.size() > 1) {
         cerr << "WARNING UNIMPLEMENTED: AssignmentStatement - with multiple variables\n";
      }
      if (rhs == nullptr) {
         cerr << "WARNING UNIMPLEMENTED: AssignmentStatement - could be FunctionCall, or StatusConstant, or PointerLiteral, etc.\n";
         return ATtrue;
      }

   // Begin SageTreeBuilder
      sage_tree_builder.Enter(assign_stmt, rhs, vars, std::string());

   } else return ATfalse;
   ROSE_ASSERT(assign_stmt != nullptr);

// End SageTreeBuilder
   sage_tree_builder.Leave(assign_stmt);

   return ATtrue;
}

//========================================================================================
// 4.2 LOOP STATEMENTS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_WhileStatement(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_WhileStatement: %s\n", ATwriteToString(term));
#endif

   ATerm t_labels, t_clause, t_stmt, t_formula;
   std::vector<std::string> labels;
   std::vector<PosInfo> locations;

   SgExpression* condition = nullptr;

   if (ATmatch(term, "WhileStatement(<term>,<term>,<term>)", &t_labels, &t_clause, &t_stmt)) {
      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;

      if (ATmatch(t_clause, "WhileClause(<term>)", &t_formula)) {
         // MATCHED WhileClause
         if (traverse_BitFormula(t_formula, condition)) {
            // MATCHED BitFormula
         } else return ATfalse;
      }

//TODO_STATEMENTS
#if 0
      while_body_list = new SgUntypedStatementList();

      // Match ControlledStatement -- it is a Statement
      if (traverse_Statement(t_stmt)) {
         // MATCHED Statement
      } else return ATfalse;

   // List will either contain one simple statement or one block statement
      ROSE_ASSERT(while_body_list->get_stmt_list().size() == 1);
#endif
   }
   else return ATfalse;

//TODO_STATEMENTS
#if 0
   SgUntypedStatement * body = while_body_list->get_stmt_list().back();
   while_body_list->get_stmt_list().pop_back();
   delete while_body_list;

   SgUntypedWhileStatement* while_stmt = new SgUntypedWhileStatement("", condition, body);
   ROSE_ASSERT(while_stmt);
   setSourcePosition(while_stmt, term);

   stmt_list->get_stmt_list().push_back(while_stmt);
#endif

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_ForStatement(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ForStatement: %s\n", ATwriteToString(term));
#endif

   ATerm t_labels, t_clause, t_stmt;
   std::vector<std::string> labels;
   std::vector<PosInfo> locations;

   SgUntypedExpression* var_ref = NULL;
   SgUntypedExpression* init    = NULL;
   SgUntypedExpression* phrase1 = NULL;
   SgUntypedExpression* phrase2 = NULL;

   int phrase1_enum = Jovial_ROSE_Translation::e_unknown;
   int phrase2_enum = Jovial_ROSE_Translation::e_unknown;

//TODO_STATEMENTS
#if 0
   SgUntypedForStatement* for_stmt = NULL;
   SgUntypedStatement* body = NULL;
#endif

   if (ATmatch(term, "ForStatement(<term>,<term>,<term>)", &t_labels, &t_clause, &t_stmt)) {
      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;

      if (traverse_ForClause(t_clause, var_ref, init, phrase1, phrase2, phrase1_enum, phrase2_enum)) {
         // MATCHED ForClause
      } else return ATfalse;

      // Match ControlledStatement which is a Statement
      if (traverse_Statement(t_stmt)) {
         // MATCHED Statement
      } else return ATfalse;
   }
   else return ATfalse;

   //cout << ".x. loop body size is " << for_body_list->get_stmt_list().size() << endl;
   //WHY????   ROSE_ASSERT(for_body_list->get_stmt_list().size() > 0);

//TODO_STATEMENTS
#if 0
   if (for_body_list->get_stmt_list().size() > 0) {
      body = for_body_list->get_stmt_list().back();
      for_body_list->get_stmt_list().pop_back();
   }

   int op_enum = LanguageTranslation::e_operator_assign;
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
#endif

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

   SgExpression* expr = nullptr;

   if (ATmatch(term, "ControlClause(<term>,<term>)", &t_value, &t_continuation)) {
      // MATCHED ControlClause
      if (traverse_Formula(t_value, expr)) {
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
      if (traverse_NumericFormula(t_formula, sg_expr)){
         // MATCHED NumericFormula
         phrase_enum = Jovial_ROSE_Translation::e_by_phrase_expr;
      } else return ATfalse;
   } else if (ATmatch(term, "ThenPhrase(<term>)", &t_formula)) {
      // MATCHED ThenPhrase
      if (traverse_Formula(t_formula, sg_expr)){
         // MATCHED Formula
         phrase_enum = Jovial_ROSE_Translation::e_then_phrase_expr;
      } else return ATfalse;
   } else if (ATmatch(term, "WhilePhrase(<term>)", &t_formula)) {
      // MATCHED WhilePhrase
      if (traverse_BitFormula(t_formula, sg_expr)){
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
ATbool ATermToSageJovialTraversal::traverse_IfStatement(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_IfStatement: %s\n", ATwriteToString(term));
#endif

   ATerm t_labels, t_cond, t_else, t_true, t_false;
   std::vector<std::string> labels;
   std::vector<PosInfo> locations;
   SgExpression* conditional = nullptr;
   SgUntypedStatement *false_body;

   if (ATmatch(term, "IfStatement(<term>,<term>,<term>,<term>)", &t_labels,&t_cond,&t_true,&t_else)) {

      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;

      if (traverse_BitFormula(t_cond, conditional)) {
         // MATCHED BitFormula
      } else return ATfalse;

      if (traverse_Statement(t_true)) {
//TODO_STATEMENTS
#if 0
         true_body = stmt_list->get_stmt_list().back();
         stmt_list->get_stmt_list().pop_back();
#endif
      } else return ATfalse;

      if (ATmatch(t_else, "no-else-clause()")) {
         false_body = NULL;
      }
      else if (ATmatch(t_else, "ElseClause(<term>)", &t_false)) {
         if (traverse_Statement(t_false)) {
//TODO_STATEMENTS
#if 0
            false_body = stmt_list->get_stmt_list().back();
            stmt_list->get_stmt_list().pop_back();
#endif
         } else return ATfalse;
      }
      else return ATfalse;
   }
   else return ATfalse;

//TODO_STATEMENTS
#if 0
   int statement_enum = LanguageTranslation::e_unknown;
   //   SgUntypedIfStatement* if_stmt = SageBuilder::buildUntypedIfStatement("",statement_enum,conditional,true_body,false_body);
   SgUntypedIfStatement* if_stmt = new SgUntypedIfStatement("", statement_enum, conditional, true_body, false_body);
   setSourcePosition(if_stmt, term);

   stmt = convert_Labels(labels, locations, if_stmt);

   stmt_list->get_stmt_list().push_back(stmt);
#endif

   return ATtrue;
}

//========================================================================================
// 4.4 CASE STATEMENTS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_CaseStatement(ATerm term)
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

   int stmt_enum = LanguageTranslation::e_switch_stmt;

   if (ATmatch(term, "CaseStatement(<term>,<term>,<term>,<term>)", &t_labels, &t_formula, &t_case_body, &t_labels2)) {
      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;

      if (traverse_Formula(t_formula, sg_expr)) {
        // MATCHED Formula
      } else return ATfalse;

      if (traverse_CaseBody(t_case_body, body)) {
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

//TODO_STATEMENTS
#if 0
   stmt_list->get_stmt_list().push_back(stmt);
#endif

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_CaseBody(ATerm term, SgUntypedStatement* & case_body)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CaseBody: %s\n", ATwriteToString(term));
#endif

//TODO_STATEMENTS
#if 0
   SgUntypedBlockStatement* body = SageBuilder::buildUntypedBlockStatement("");
   ROSE_ASSERT(body != NULL);
   setSourcePosition(body, term);

   SgUntypedStatementList* my_stmt_list = body->get_scope()->get_statement_list();
#endif

   ATermList tail = (ATermList) ATmake("<term>", term);
   while (! ATisEmpty(tail)) {
      ATerm head = ATgetFirst(tail);
      tail = ATgetNext(tail);
      if (traverse_CaseAlternative(head)) {
         // MATCHED CaseAlternative
      } else if (traverse_DefaultOption(head)) {
         // MATCHED DefaultOption
      } else return ATfalse;
   }

//TODO_STATEMENTS
#if 0
   *case_body = body;
#endif

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_CaseAlternative(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CaseAlternative: %s\n", ATwriteToString(term));
#endif

   ATerm t_case_index_group, t_stmt, t_fall_thru;
   bool fall_thru;
   SgUntypedExprListExpression* case_index_group = NULL;

   if (ATmatch(term, "CaseAlternative(<term>,<term>,<term>)", &t_case_index_group, &t_stmt, &t_fall_thru)) {

//TODO_STATEMENTS
#if 0
      body = SageBuilder::buildUntypedBlockStatement("");
      ROSE_ASSERT(body != NULL);
      setSourcePosition(body, term);

      SgUntypedStatementList* my_stmt_list = body->get_scope()->get_statement_list();
#endif

      if (traverse_CaseIndexGroup(t_case_index_group, case_index_group)) {
         // MATCHED CaseIndexGroup
      } else return ATfalse;

      if (traverse_Statement(t_stmt)) {
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

//TODO_STATEMENTS
#if 0
   ROSE_ASSERT(case_index_group != NULL);
   ROSE_ASSERT(body != NULL);

   SgUntypedCaseStatement* case_stmt = new SgUntypedCaseStatement("", stmt_enum, case_index_group, body, "", fall_thru);
   ROSE_ASSERT(case_stmt != NULL);
   setSourcePosition(case_stmt, term);

   stmt_list->get_stmt_list().push_back(case_stmt);
#endif

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_DefaultOption(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_DefaultOption: %s\n", ATwriteToString(term));
#endif

   ATerm t_stmt, t_fall_thru;

//TODO_STATEMENTS
#if 0
   SgUntypedBlockStatement* body = NULL;
   int stmt_enum = LanguageTranslation::e_case_default_option_stmt;
#endif
   bool fall_thru = false;

   if (ATmatch(term, "DefaultOption(<term>,<term>)", &t_stmt, &t_fall_thru)) {

//TODO_STATEMENTS
#if 0
      body = SageBuilder::buildUntypedBlockStatement("");
      ROSE_ASSERT(body != NULL);
      setSourcePosition(body, term);

      SgUntypedStatementList* my_stmt_list = body->get_scope()->get_statement_list();
#endif

      if (traverse_Statement(t_stmt)) {
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

//TODO_STATEMENTS
#if 0
   ROSE_ASSERT(body != NULL);

   SgUntypedCaseStatement* case_stmt = new SgUntypedCaseStatement("", stmt_enum, NULL, body, "", fall_thru);
   ROSE_ASSERT(case_stmt != NULL);
   setSourcePosition(case_stmt, term);

   stmt_list->get_stmt_list().push_back(case_stmt);
#endif

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
      index_group = new SgUntypedExprListExpression(LanguageTranslation::e_case_selector);
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

// Begin SageTreeBuilder
   SgExpression* value = nullptr;
   SgExpression* lower_bound = nullptr;
   SgExpression* upper_bound = nullptr;

   case_index = NULL;

   if (ATmatch(term, "CaseIndex(<term>)", &t_formula1)) {
     // This case is needed to traverse CompileTimeFormula -> CaseIndex
      if (traverse_Formula(t_formula1, value)) {
         // MATCHED Formula
      } else return ATfalse;

   } else if (ATmatch(term, "CaseIndex(<term>,<term>)", &t_formula1, &t_formula2)) {
     // This case is needed to traverse LowerBound : UpperBound -> CaseIndex
      if (traverse_Formula(t_formula1, lower_bound)) {
         // MATCHED Formula
      } else return ATfalse;
      if (traverse_Formula(t_formula2, upper_bound)) {
         // MATCHED Formula
      } else return ATfalse;
   }
   else return ATfalse;
#if 0
   if (value) {
      case_index = value;
   }
   else if (lower_bound && upper_bound) {
      int expr_enum = LanguageTranslation::e_case_range;
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
#endif

   return ATtrue;
}

//========================================================================================
// 4.5 PROCEDURE CALL STATEMENTS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_ProcedureCallStatement(ATerm term)
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

      arg_list = new SgUntypedExprListExpression(LanguageTranslation::e_argument_list);
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

      int expr_enum = LanguageTranslation::e_function_reference;
      int stmt_enum = LanguageTranslation::e_procedure_call;

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

//TODO_STATEMENTS
#if 0
   ROSE_ASSERT(stmt);

   stmt_list->get_stmt_list().push_back(stmt);
#endif

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
         if (traverse_Formula(head, sg_expr)) {
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

// Begin SageTreeBuilder
   SgExpression* param = nullptr;

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
            if (traverse_Variable(t_param, param)) {
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
ATbool ATermToSageJovialTraversal::traverse_ReturnStatement(ATerm term)
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

//TODO_STATEMENTS
#if 0
   stmt_list->get_stmt_list().push_back(stmt);
#endif

   return ATtrue;
}

//========================================================================================
// 4.7 GOTO STATEMENTS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_GotoStatement(ATerm term)
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

//TODO_STATEMENTS
#if 0
   stmt_list->get_stmt_list().push_back(stmt);
#endif

   return ATtrue;

}

//========================================================================================
// 4.8 EXIT STATEMENTS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_ExitStatement(ATerm term)
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

//TODO_STATEMENTS
#if 0
   stmt_list->get_stmt_list().push_back(stmt);
#endif

   return ATtrue;
}

//========================================================================================
// 4.9 STOP STATEMENTS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_StopStatement(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_StopStatement: %s\n", ATwriteToString(term));
#endif

   ATerm t_labels, t_stop_code;
   std::vector<std::string> labels;
   std::vector<PosInfo> locations;

   SgStopOrPauseStatement* stop_stmt = nullptr;
   SgExpression* stop_code = nullptr;

   if (ATmatch(term, "StopStatement(<term>,<term>)", &t_labels, &t_stop_code)) {
      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;

#if 0
      if (ATmatch(t_stop_code, "no-integer-formula()")) {
         // no stop code
         // DELETE ME
         //         stop_code = UntypedBuilder::buildUntypedNullExpression();
      }
      else if (traverse_NumericFormula(t_stop_code, sg_stop_code)) {
         // MATCHED NumericFormula
      } else return ATfalse;

      // DELETE ME
      //      SgUntypedStopStatement* stop_stmt = new SgUntypedStopStatement("", stop_code);
      //      setSourcePosition(stop_stmt, term);

      //      stmt = convert_Labels(labels, locations, stop_stmt);
#endif
   }
   else return ATfalse;

//TODO_STATEMENTS
#if 0
   stmt_list->get_stmt_list().push_back(stmt);
#endif

   return ATtrue;
}

//========================================================================================
// 4.10 ABORT STATEMENTS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_AbortStatement(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_AbortStatement: %s\n", ATwriteToString(term));
#endif

   ATerm t_labels;
   std::vector<std::string> labels;
   std::vector<PosInfo> locations;

   SgStopOrPauseStatement* abort_stmt = nullptr;
   SgExpression* stop_code = nullptr;


   if (ATmatch(term, "AbortStatement(<term>)", &t_labels)) {
      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;

//TODO_STATEMENTS
#if 0

      SgUntypedAbortStatement* abort_stmt = new SgUntypedAbortStatement("");
      setSourcePosition(abort_stmt, term);
      stmt = convert_Labels(labels, locations, abort_stmt);
#endif
   }
   else return ATfalse;

#if 0
   // Begin SageTreeBuilder
   sage_tree_builder.Enter(abort_stmt, rhs, vars, std::string());

   } else return ATfalse;
   ROSE_ASSERT(assign_stmt != nullptr);

  // End SageTreeBuilder
  sage_tree_builder.Leave(assign_stmt, );
#endif

   return ATtrue;
}

//========================================================================================
// 5.0 FORMULAS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_Formula(ATerm term, SgExpression* &sg_expr)
{
   if (traverse_NumericFormula(term, sg_expr)) {
      // MATCHED NumericFormula
   } else if (traverse_BitFormula(term, sg_expr)) {
      // MATCHED BitFormula
   } else if (traverse_GeneralFormula(term, sg_expr)) {
      // MATCHED GeneralFormula
   } else return ATfalse;

   //  TableFormula                -> Formula

   return ATtrue;
}

//========================================================================================
// 5.1 NUMERIC FORMULAS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_NumericFormula(ATerm term, SgExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_NumericFormula: %s\n", ATwriteToString(term));
#endif

   ATerm t_sign, t_expr, t_lhs, t_op, t_rhs;

   // OptSign NumericTerm -> NumericFormula
   //
   if (ATmatch(term, "NumericFormula(<term>,<term>)", &t_sign, &t_expr)) {
      LanguageTranslation::ExpressionKind op_enum;

      if (traverse_OptSign(t_sign, op_enum)) {
         // MATCHED OptSign
      } else return ATfalse;
      if (traverse_NumericTerm(t_expr, expr)) {
         // MATCHED NumericTerm
      } else return ATfalse;
      if (op_enum == LanguageTranslation::e_operator_unary_minus) {
        expr = SageBuilder::buildMinusOp_nfi(expr, SgUnaryOp::prefix);
        setSourcePosition(expr, term);
      }
      else if (op_enum == LanguageTranslation::e_operator_unary_plus) {
        expr = new SgUnaryAddOp(expr, nullptr);
        ROSE_ASSERT(expr != nullptr);
        setSourcePosition(expr, term);
      }
   }

   // NumericFormula PlusOrMinus NumericTerm -> NumericFormula
   //
   else if (ATmatch(term, "NumericFormula(<term>,<term>,<term>)", &t_lhs,&t_op,&t_rhs)) {
      SgExpression * sg_lhs = nullptr, * sg_rhs = nullptr;

      if (traverse_NumericFormula(t_lhs, sg_lhs)) {
         // MATCHED NumericFormula
      } else return ATfalse;

      if (traverse_NumericTerm(t_rhs, sg_rhs)) {
         // MATCHED NumericTerm
      } else return ATfalse;

      if (ATmatch(t_op, "AddOp()")) {
         expr = new SgAddOp(sg_lhs, sg_rhs, NULL);
         ROSE_ASSERT(expr != nullptr);
         setSourcePosition(expr, term);
      }
      else if (ATmatch(t_op, "SubtractOp()")) {
         expr = new SgSubtractOp(sg_lhs, sg_rhs, NULL);
         ROSE_ASSERT(expr != nullptr);
         setSourcePosition(expr, term);
      } else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_NumericPrimary(ATerm term, SgExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_NumericPrimary: %s\n", ATwriteToString(term));
#endif

   ATerm t_num_var, t_formula, t_factor, t_num_term, t_conversion;
   char *literal=nullptr, *var_name=nullptr;

   SgExpression *sg_num_term = nullptr, *sg_factor = nullptr, *cast_formula = nullptr;
   SgFunctionCallExp* func_call = nullptr;
   SgType* conv_type = nullptr;

   expr = nullptr;

   if (ATmatch(term, "IntegerLiteral(<str>)", &literal)) {
      expr = SageBuilder::buildIntVal_nfi(std::string(literal));
      setSourcePosition(expr, term);
   }

   else if (traverse_FixedOrFloatingLiteral(term, expr)) {
         // MATCHED FixedOrFloatingLiteral
   }

   else if (traverse_NumericMachineParameter(term, expr)) {
      // MATCHED NumericMachineParameter

      if (!expr) {
         cerr << "WARNING UNIMPLEMENTED: NumericPrimary - NumericMachineParameter\n";
      }
   }

   else if (ATmatch(term, "NumericVariable(<term>)", &t_num_var)) {
#if 0
      if (traverse_TableItem(t_table_item, table_item)){
         //MATCHED TableItem
      } else return ATfalse;
#endif

      // need to traverse all possible Variables, including TableItem
      if (traverse_Variable(t_num_var, expr)) {
         if (!expr) {
            cerr << "WARNING UNIMPLEMENTED: NumericPrimary - NumericVariable\n";
            ROSE_ASSERT(false);
         }
      } else return ATfalse;
   }

   else if (ATmatch(term, "NumericPrimaryParens(<term>)", &t_formula)) {

      if (traverse_NumericFormula(t_formula, expr)) {
         //  '(' NumericFormula ')'      -> NumericPrimary         {cons("NumericPrimary")}
         // TODO: Add way to indicate parens?
         // expr.set_need_paren();
         if (!expr) {
            cerr << "WARNING UNIMPLEMENTED: NumericPrimary - Parens - maybe because of FunctionCall\n";
         }
      } else return ATfalse;

   }

   else if (ATmatch(term, "NumericPrimary(<term>,<term>)", &t_conversion, &t_formula)) {

      if (traverse_IntegerConversion(t_conversion, conv_type)) {
         // MATCHED IntegerConversion
      } else if (traverse_TypeNameConversion(t_conversion, conv_type)) {
         // MATCHED TypeNameConversion
      } else if (traverse_FloatingConversion(t_conversion, conv_type)) {
         // MATCHED FloatingConversion
      } else if (traverse_FixedConversion(t_conversion, conv_type)) {
         // MATCHED FixedConversion
      } else return ATfalse;

      if (traverse_Formula(t_formula, cast_formula)) {
         // MATCHED Formula
      } else return ATfalse;

      ROSE_ASSERT(conv_type);
      ROSE_ASSERT(cast_formula);
      //                                                      cast_enum? default? ctype? static? dynamic?
      SgCastExp* cast_expr = SageBuilder::buildCastExp_nfi(cast_formula, conv_type, SgCastExp::e_default);
      ROSE_ASSERT(cast_expr);
      setSourcePosition(cast_expr, term);
      expr = cast_expr;
   }

   else if (ATmatch(term, "NumericPrimary(<term>,<term>,<term>)", &t_conversion, &t_num_term, &t_factor)) {

      if (traverse_FixedConversion(t_conversion, conv_type)) {
      } else return ATfalse;
      if (traverse_NumericTerm(t_num_term, sg_num_term)) {
      } else return ATfalse;
      if (traverse_NumericFactor(t_factor, sg_factor)) {
      } else return ATfalse;

   }

   else if (traverse_FunctionCall(term, func_call)) {
      // MATCHED FunctionCall
      if (!func_call) {
         cerr << "WARNING UNIMPLEMENTED: NumericPrimary - FunctionCall\n";
      }
      expr = func_call;
   }

// Lastly handle names (variable identifiers)
   else if (ATmatch(term, "<str>", &var_name)) {
      // MATCHED an unnamed string
   }
   else if (ATmatch(term, "NumericVariable(<str>)", &var_name)) {
      // MATCHED NumericVariable
   }
   else if (ATmatch(term, "ControlLetter(<str>)", &var_name)) {
      // MATCHED ControlLetter
   }
   else return ATfalse;

   if (var_name != nullptr) {
      SgVariableSymbol* var_sym = SageInterface::lookupVariableSymbolInParentScopes(var_name, SageBuilder::topScopeStack());
      ROSE_ASSERT(var_sym);
      expr = SageBuilder::buildVarRefExp_nfi(var_sym);
      setSourcePosition(expr, term);
   }

// DELETE_ME (temporarily return integer literal expression "54321")
   if (expr == nullptr) {
      cerr << "WARNING UNIMPLEMENTED: NumericPrimary - expr is null \n";
      expr = SageBuilder::buildIntVal_nfi(std::string("54321"));
      return ATtrue;
   }

   ROSE_ASSERT(expr);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_OptSign(ATerm term, LanguageTranslation::ExpressionKind & op_enum)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptSign: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "no-sign()")) {
      op_enum = LanguageTranslation::e_operator_unity;
   }
   else if (ATmatch(term, "PLUS()")) {
      op_enum = LanguageTranslation::e_operator_unary_plus;
   }
   else if (ATmatch(term, "MINUS()")) {
      op_enum = LanguageTranslation::e_operator_unary_minus;
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_NumericTerm(ATerm term, SgExpression* & expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_NumericTerm: %s\n", ATwriteToString(term));
#endif

   ATerm t_lhs, t_op, t_rhs;
   SgExpression *lhs = nullptr, *rhs = nullptr;

   if (ATmatch(term, "NumericTerm(<term>,<term>,<term>)", &t_lhs, &t_op, &t_rhs)) {
      if (traverse_NumericTerm(t_lhs, lhs)) {
         // MATCHED NumericTerm
      } else return ATfalse;

      if (traverse_NumericFactor(t_rhs, rhs)) {
         // MATCHED NumericFactor
      } else return ATfalse;

      ROSE_ASSERT(lhs && rhs);

      if (ATmatch(t_op, "MultiplyOp()")) {
         expr = new SgMultiplyOp(lhs, rhs, NULL);
      }
      else if (ATmatch(t_op, "DivideOp()")) {
         expr = new SgDivideOp(lhs, rhs, NULL);
      }
      else if (ATmatch(t_op, "ModOp()")) {
         expr = new SgModOp(lhs, rhs, NULL);
      }
      else return ATfalse;

      ROSE_ASSERT(expr);
      setSourcePosition(expr, term);
   }
   else if (traverse_NumericFactor(term, expr)) {
         // MATCHED NumericFactor
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_NumericFactor(ATerm term, SgExpression* & expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_NumericFactor: %s\n", ATwriteToString(term));
#endif

   // NEED TO DECIDE: whether to use this or use traverse_ExponentiationOp
   // or use both

   if (traverse_NumericPrimary(term, expr)) {
      // MATCHED NumericPrimary
   } else if (traverse_ExponentiationOp(term, expr)) {
      // MATCHED ExponentiationOp
   }

   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_ExponentiationOp(ATerm term, SgExpression* & expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ExponentiationOp: %s\n", ATwriteToString(term));
#endif

   ATerm t_lhs, t_rhs;
   SgExpression * lhs = nullptr, * rhs = nullptr;
   std::string op_name;

   if (ATmatch(term, "ExponentiationOp(<term>,<term>)", &t_lhs, &t_rhs)) {
      if (traverse_NumericFactor(t_lhs, lhs)) {
         // MATCHED NumericFactor
      } else return ATfalse;

      if (traverse_NumericPrimary(t_rhs, rhs)) {
         // MATCHED NumericPrimary
      } else return ATfalse;

      // DELETE ME
      //      op_enum = LanguageTranslation::e_operator_exponentiate;
      //      op_name = "**";
      //      expr = new SgUntypedBinaryOperator(op_enum,op_name,lhs,rhs);
      //      setSourcePosition(expr, term);
   }

   //   else if (traverse_NumericPrimary(term, expr)) {
      // MATCHED NumericPrimary
   //   }

   else return ATfalse;

   ROSE_ASSERT(expr != nullptr);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_NumericMachineParameter(ATerm term, SgExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_NumericMachineParameter: %s\n", ATwriteToString(term));
#endif

   if (traverse_IntegerMachineParameter(term, expr)) {
      // MATCHED IntegerMachineParameter
   } else if (traverse_FloatingMachineParameter(term, expr)) {
      // MATCHED FloatingMachineParameter
   } else if (traverse_FixedMachineParameter(term, expr)) {
      // MATCHED FixedMachineParameter
   }

   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 5.2 BIT FORMULAS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_BitFormula(ATerm term, SgExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BitFormula: %s\n", ATwriteToString(term));
#endif

   ATerm t_operand, t_continuation, t_amb;
   SgExpression* sg_continuation = nullptr;

   expr = nullptr;

   if (ATmatch(term, "BitFormula(<term>,<term>)", &t_operand, &t_continuation)) {
      if (ATmatch(t_operand, "amb(<term>)", &t_amb)) {
         ATermList tail = (ATermList) ATmake("<term>", t_amb);
         ATerm head = ATgetFirst(tail);
         // chose first amb path, now traverse it

         if (traverse_RelationalExpression(head, expr)) {
            // MATCHED RelationalExpression
         } else return ATfalse;
      } else if (traverse_LogicalOperand(t_operand, expr)) {
         // MATCHED LogicalOperand
      } else return ATfalse;

      if (traverse_OptLogicalContinuation(t_continuation, sg_continuation)) {
         // MATCHED OptLogicalContinuation
         cerr << "WARNING UNIMPLEMENTED: BitFormula - with continuation\n";
      } else return ATfalse;

   } else if (ATmatch(term, "BitFormulaNOT(<term>)", &t_operand)) {
      if (traverse_LogicalOperand(t_operand, expr)) {
         // MATCHED LogicalOperand
      } else return ATfalse;

   } else if (ATmatch(term, "BitVariableFormula(<term>)", &t_operand)) {
      if (traverse_Variable(t_operand, expr)) {
         // MATCHED Variable
      } else return ATfalse;
   } else return ATfalse;

   if (expr == nullptr) {
      cerr << "WARNING UNIMPLEMENTED: BitFormula - BitPrimaryConversion or Dereference\n";
      return ATtrue;
   }

   ROSE_ASSERT(expr != NULL);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_OptLogicalContinuation(ATerm term, SgExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptLogicalContinuation: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "no-logical-continuation")) {
      // MATCHED no-logical-continuation
   } else {
      ATermList tail = (ATermList) ATmake("<term>", term);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_LogicalContinuation(head, expr)) {
            // MATCHED LogicalContinuation
         } else return ATfalse;
      }
   }

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_LogicalContinuation(ATerm term, SgExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_LogicalContinuation: %s\n", ATwriteToString(term));
#endif

   ATerm t_operand;

   if (ATmatch(term, "AndContinuation(<term>)", &t_operand)) {
      if (traverse_LogicalOperand(t_operand, expr)) {
      // MATCHED LogicalOperand
      } else return ATfalse;
   } else if (ATmatch(term, "OrContinuation(<term>)", &t_operand)) {
      if (traverse_LogicalOperand(t_operand, expr)) {
      // MATCHED LogicalOperand
      } else return ATfalse;
   } else if (ATmatch(term, "XorContinuation(<term>)", &t_operand)) {
      if (traverse_LogicalOperand(t_operand, expr)) {
      // MATCHED LogicalOperand
      } else return ATfalse;
   } else if (ATmatch(term, "EqvContinuation(<term>)", &t_operand)) {
      if (traverse_LogicalOperand(t_operand, expr)) {
      // MATCHED LogicalOperand
      } else return ATfalse;
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_LogicalOperand(ATerm term, SgExpression* & expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_LogicalOperand: %s\n", ATwriteToString(term));
#endif

   if (traverse_BitPrimary(term, expr)) {
      // MATCHED BitPrimary
      if (expr == nullptr) {
         cerr << "WARNING UNIMPLEMENTED: LogicalOperand - BitPrimary - probably BitPrimaryConversion\n";
         return ATtrue;
      }
   } else if (traverse_Variable(term, expr)) {
      // MATCHED Variable
   } else if (traverse_RelationalExpression(term, expr)) {
      // MATCHED RelationalExpression
   } else return ATfalse;

   ROSE_ASSERT(expr != NULL);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_BitPrimary(ATerm term, SgExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BitPrimary: %s\n", ATwriteToString(term));
#endif

   ATerm t_bit;

   expr = nullptr;
   SgType* type = nullptr;

   if (traverse_BooleanLiteral(term, expr)) {
      // MATCHED BooleanLiteral
   }
   else if (ATmatch(term,"BitPrimaryParens(<term>)", &t_bit)) {
      // TODO: Add parentheses
      cerr << "WARNING UNIMPLEMENTED: BitPrimary - BitPrimaryParens\n";
      if (traverse_BitFormula(t_bit, expr)) {
      // MATCHED '(' BitFormula ')'
      } else return ATfalse;
   }
   else if (traverse_BitLiteral(term, expr)) {
      // MATCHED BitLiteral
   }
   else if (traverse_BitConversion(term, type)) {
      // MATCHED BitPrimaryConversion
      cerr << "WARNING UNIMPLEMENTED: BitPrimary - BitPrimaryConversion\n";
      return ATtrue;
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
ATbool ATermToSageJovialTraversal::traverse_RelationalExpression(ATerm term, SgExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_RelationalExpression: %s\n", ATwriteToString(term));
#endif

   ATerm t_lhs, t_operator, t_rhs;

// Begin SageTreeBuilder
   SgExpression *lhs = nullptr, *rhs = nullptr;

   if (ATmatch(term, "RelationalExpression(<term>,<term>,<term>)", &t_lhs, &t_operator, &t_rhs)) {
      if (traverse_Formula(t_lhs, lhs)) {
         // MATCHED Formula
      } else return ATfalse;

      if (traverse_Formula(t_rhs, rhs)) {
         // MATCHED Formula
      } else return ATfalse;

      ROSE_ASSERT(lhs && rhs);

      if (ATmatch(t_operator, "LessThanOp()")) {
         expr = new SgLessThanOp(lhs, rhs, NULL);
      }
      else if (ATmatch(t_operator, "GreaterThanOp()")) {
         expr = new SgGreaterThanOp(lhs, rhs, NULL);
      }
      else if (ATmatch(t_operator, "LessOrEqualOp()")) {
         expr = new SgLessOrEqualOp(lhs, rhs, NULL);
      }
      else if (ATmatch(t_operator, "GreaterOrEqualOp()")) {
         expr = new SgGreaterOrEqualOp(lhs, rhs, NULL);
      }
      else if (ATmatch(t_operator, "EqualityOp()")) {
         expr = new SgEqualityOp(lhs, rhs, NULL);
      }
      else if (ATmatch(t_operator, "NotEqualOp()")) {
         expr = new SgNotEqualOp(lhs, rhs, NULL);
      } else return ATfalse;

   } else return ATfalse;

   ROSE_ASSERT(expr);
   setSourcePosition(expr, term);

   return ATtrue;
}

//========================================================================================
// 5.3.0 GENERAL FORMULA
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_GeneralFormula(ATerm term, SgExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_GeneralFormula: %s\n", ATwriteToString(term));
#endif

   ATerm t_func_const_or_var;
   char* variable;

   SgFunctionCallExp* func_call = nullptr;

   if (ATmatch(term, "GeneralFormula(<str>)", &variable)) {
#if 0
      expr_enum = Jovial_ROSE_Translation::e_referenceExpression;
      expr = new SgUntypedReferenceExpression(expr_enum, variable);
      setSourcePosition(expr, term);
#endif
   } else if (ATmatch(term, "GeneralFormula(<term>)", &t_func_const_or_var)) {
      if (traverse_FunctionCall(t_func_const_or_var, func_call)) {
         // MATCHED FunctionCall
         expr = func_call;
      } else if (traverse_NamedConstant(t_func_const_or_var, expr)) {
         // MATCHED NamedConstant
      } else if (traverse_Variable(t_func_const_or_var, expr)) {
         // MATCHED Variable
      } else return ATfalse;
   } else if (traverse_CharacterFormula(term, expr)) {
      // MATCHED CharacterFormula
   } else if (traverse_StatusFormula(term, expr)) {
      // MATCHED StatusFormula
   } else if (traverse_PointerFormula(term, expr)) {
      // MATCHED PointerFormula
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 5.3 CHARACTER FORMULAS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_CharacterFormula(ATerm term, SgExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CharacterFormula: %s\n", ATwriteToString(term));
#endif

   ATerm t_literal, t_formula, t_conv_type;

   std::string str_literal;
   SgExpression* cast_formula = nullptr;
   SgType* conv_type = nullptr;

   expr = nullptr;

   if (ATmatch(term, "CharacterFormula(<term>)", &t_literal)) {
      if (traverse_CharacterLiteral(t_literal, str_literal)) {
         // MATCHED CharacterLiteral -> CharacterFormula
         expr = SageBuilder::buildStringVal(str_literal);
         setSourcePosition(expr, t_literal);
      } else return ATfalse;

   } else if (ATmatch(term, "CharacterFormulaParens(<term>)", &t_formula)) {
      // '(' CharacterFormula ')' -> CharacterFormula
      if (traverse_CharacterFormula(t_formula, expr)) {
         // MATCHED CharacterFormula
      } else return ATfalse;

   } else if (ATmatch(term, "CharacterFormulaConversion(<term>,<term>)", &t_conv_type, &t_formula)) {
      if (traverse_CharacterConversion(t_conv_type, conv_type)) {
         // MATCHED CharacterConversion
      } else return ATfalse;

      if (traverse_Formula(t_formula, cast_formula)) {
         // MATCHED Formula
      }

      ROSE_ASSERT(conv_type);
      ROSE_ASSERT(cast_formula);
      //                                      cast_enum? default? ctype? static? dynamic?
      SgCastExp* cast_expr = SageBuilder::buildCastExp(cast_formula, conv_type, SgCastExp::e_default);
      ROSE_ASSERT(cast_expr);
      setSourcePosition(cast_expr, term);
      expr = cast_expr;
   } else return ATfalse;

   ROSE_ASSERT(expr);

   return ATtrue;
}

//========================================================================================
// 5.4 STATUS FORMULAS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_StatusFormula(ATerm term, SgExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_StatusFormula: %s\n", ATwriteToString(term));
#endif

   ATerm t_expr, t_conv, t_formula;

   SgExpression* cast_formula = nullptr;
   SgType* conv_type = nullptr;

   if (ATmatch(term, "StatusFormula(<term>)", &t_expr)) {
      if (traverse_StatusConstant(t_expr, expr)) {
         // MATCHED StatusConstant
      } else return ATfalse;

   } else if (ATmatch(term, "StatusFormulaParens(<term>)", &t_expr)) {
      // '(' StatusFormula ')' -> StatusFormula
      if (traverse_StatusFormula(t_expr, expr)) {
         // MATCHED StatusFormula
      } else return ATfalse;

   } else if (ATmatch(term, "StatusFormula(<term>,<term>)", &t_conv, &t_formula)) {
      if (traverse_StatusConversion(t_conv, conv_type)) {
         // StatusConversion '(' Formula ')'  ->  StatusFormula
         // MATCHED StatusConversion
      } else return ATfalse;

      if (traverse_Formula(t_formula, cast_formula)) {
         // MATCHED Formula
      } else return ATfalse;

      ROSE_ASSERT(conv_type);
      ROSE_ASSERT(cast_formula);
      //                                      cast_enum? default? ctype? static? dynamic?
      SgCastExp* cast_expr = SageBuilder::buildCastExp(cast_formula, conv_type, SgCastExp::e_default);
      ROSE_ASSERT(cast_expr);
      setSourcePosition(cast_expr, term);
      expr = cast_expr;
   } else return ATfalse;

   ROSE_ASSERT(expr);

   return ATtrue;
}

//========================================================================================
// 5.5 POINTER FORMULAS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_PointerFormula(ATerm term, SgExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_PointerFormula: %s\n", ATwriteToString(term));
#endif

   ATerm t_literal, t_formula, t_conv_type;

   SgExpression* cast_formula = nullptr;
   SgType* conv_type = nullptr;

   if (ATmatch(term, "PointerFormula(<term>)", &t_literal)) {
      if (traverse_PointerLiteral(t_literal, expr)) {
         // PointerLiteral -> PointerFormula
         // MATCHED PointerLiteral
      } else return ATfalse;

   } else if (ATmatch(term, "PointerFormulaParens(<term>)", &t_formula)) {
      // '(' PointerFormula ')' -> PointerFormula
      if (traverse_PointerFormula(t_formula, expr)) {
         // MATCHED PointerFormula
      } else return ATfalse;

   } else if (ATmatch(term, "PointerFormulaConversion(<term>,<term>)", &t_conv_type, &t_formula)) {
      if (traverse_PointerConversion(t_conv_type, conv_type)) {
         // MATCHED PointerConversion
      } else return ATfalse;

      if (traverse_Formula(t_formula, cast_formula)) {
         // MATCHED Formula
      } else return ATfalse;

      ROSE_ASSERT(conv_type);
      ROSE_ASSERT(cast_formula);
      //                                      cast_enum? default? ctype? static? dynamic?
      SgCastExp* cast_expr = SageBuilder::buildCastExp(cast_formula, conv_type, SgCastExp::e_default);
      ROSE_ASSERT(cast_expr);
      setSourcePosition(cast_expr, term);
      expr = cast_expr;
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 6.1 VARIABLE AND BLOCK REFERENCES
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_Variable(ATerm term, SgExpression* &var)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Variable: %s\n", ATwriteToString(term));
#endif

   char* name;

   if (ATmatch(term, "<str>" , &name)) {
      // MATCHED NamedVariable
      // SageTreeBuilder
      var = SageBuilder::buildVarRefExp(name, SageBuilder::topScopeStack());
      setSourcePosition(var, term);
   } else if (traverse_Dereference(term, var)) {
      // MATCHED ItemDereference/TableDereference -> Item/Table -> NamedVariable
   } else if (traverse_TableItem(term, var)) {
      // MATCHED TableItem
   } else if (traverse_BitFunctionVariable(term, var)) {
      // MATCHED BitFunctionVariable
   } else if (traverse_ByteFunctionVariable(term, var)) {
      // MATCHED ByteFunctionVariable
   }
   else return ATfalse;

   //  RepFunctionVariable         -> Variable           {cons("RepFunctionVariable")}

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_VariableList(ATerm term, std::vector<SgExpression*> &vars)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_VariableList: %s\n", ATwriteToString(term));
#endif

   ATerm t_labels;

// Begin SageTreeBuilder
   SgExpression* var;

   if (ATmatch(term, "VariableList(<term>)" , &t_labels)) {
      ATermList tail = (ATermList) ATmake("<term>", t_labels);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);

         if (traverse_Variable(head, var)) {
            vars.push_back(var);
         } else return ATfalse;
      }
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_TableItem(ATerm term, SgExpression* &var)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_TableItem: %s\n", ATwriteToString(term));
#endif

   ATerm t_name, t_subscript, t_tblderef;
   char* name;
   std::vector<SgExpression*> subscript;

   SgVarRefExp* var_ref = nullptr;
   SgPntrArrRefExp* array_ref = nullptr;
   SgExprListExp* array_subscripts = nullptr;

   var = nullptr;

   if (ATmatch(term, "TableItem(<term>,<term>,<term>)" , &t_name, &t_subscript, &t_tblderef)) {
   // Shall have a name
      if (ATmatch(t_name, "<str>" , &name)) {
         var_ref = SageBuilder::buildVarRefExp(name, SageBuilder::topScopeStack());
         ROSE_ASSERT(var_ref);
         setSourcePosition(var_ref, t_name);
      } else return ATfalse;

   // May have a subscript
      if (traverse_Subscript(t_subscript, subscript)) {
         array_subscripts = SageBuilder::buildExprListExp_nfi();
         setSourcePosition(array_subscripts, t_subscript);

         if (subscript.size() > 1) {
            cerr << "WARNING UNIMPLEMENTED: TableItem - subscript.size() > 1 not fully implemented\n";
            cerr << "--->    TableItem: name is " << name << endl;
            cerr << "--->    found subscript # is " << subscript.size() << ": subscript[0] is " << subscript[0] << endl;
         }

         array_subscripts->get_expressions().push_back(subscript[0]);

      }
      else {
         cerr << "WARNING UNIMPLEMENTED: TableItem - has a subscript with size (probably 0?) " << subscript.size() << std::endl;
      }

      if (traverse_TableDereference(t_tblderef, var)) {
         // MATCHED TableDereference
      } else return ATfalse;

   } else return ATfalse;

   ROSE_ASSERT(var_ref);
   ROSE_ASSERT(array_subscripts);

   array_ref = SageBuilder::buildPntrArrRefExp(var_ref, array_subscripts);
   ROSE_ASSERT(array_ref);
   setSourcePosition(array_ref, term);

   var = array_ref;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_Subscript(ATerm term, std::vector<SgExpression*> & indexes)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Subscript: %s\n", ATwriteToString(term));
#endif

   ATerm t_index;

// Begin SageTreeBuilder
   SgExpression* index;

   if (ATmatch(term, "no-subscript")) {
      // MATCHED no-subscript
   } else if (ATmatch(term, "Subscript(<term>)" , &t_index)) {
      ATermList tail = (ATermList) ATmake("<term>", t_index);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_Index(head, index)) {
            // MATCHED Index
            indexes.push_back(index);
         } else return ATfalse;
      }
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_Index(ATerm term, SgExpression* &formula)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Index: %s\n", ATwriteToString(term));
#endif

   if (traverse_NumericFormula(term, formula)) {
      // MATCHED NumericFormula
   } else if (traverse_StatusFormula(term, formula)) {
      // MATCHED StatusFormula
   } else return ATfalse;

   return ATtrue;
}


ATbool ATermToSageJovialTraversal::traverse_TableDereference(ATerm term, SgExpression* &formula)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_TableDereference: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "no-table-dereference")) {
      // MATCHED no-table-dereference
   } else if (traverse_Dereference(term, formula)) {
      // MATCHED Dereference
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_Dereference(ATerm term, SgExpression* &formula)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Dereference: %s\n", ATwriteToString(term));
#endif

   ATerm t_deref;
   char* name;

   if (ATmatch(term, "Dereference(<term>)", &t_deref)) {
      if (ATmatch(t_deref, "<str>", &name)) {
         cerr << "WARNING UNIMPLEMENTED: Dereference -> PointerItemName\n";
         // MATCHED PointerItemName
      } else if (traverse_GeneralFormula(t_deref, formula)) {
         // MATCHED PointerFormula through GeneralFormula
      } else return ATfalse;
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_BitFunctionVariable(ATerm term, SgExpression* &var)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BitFunctionVariable: %s\n", ATwriteToString(term));
#endif

   ATerm t_bitvar, t_var, t_fbit, t_nbit, t_fbit_num, t_nbit_num;
   SgExpression * fbit, * nbit;

   //  'BIT' '(' BitVariable ',' Fbit ',' Nbit ')' -> BitFunctionVariable   {cons("BitFunctionVariable"), prefer}
   //  'BIT' '(' BitFormula ','  Fbit ',' Nbit ')' -> BitFunctionVariable   {cons("BitFunctionVariable")}

   if (ATmatch(term, "BitFunctionVariable(<term>,<term>,<term>)", &t_bitvar, &t_fbit, &t_nbit)) {
      cerr << "WARNING UNIMPLEMENTED: BitFunctionVariable\n";

      if (ATmatch(t_bitvar, "BitVariable(<term>)", &t_var)) {
         if (traverse_Variable(t_var, var)) {
            // MATCHED BitVariable -> Variable
         } else return ATfalse;
      } else if (traverse_BitFormula(t_bitvar, var)) {
            // MATCHED BitFormula
      } else return ATfalse;

      if (ATmatch(t_fbit, "Fbit(<term>)", &t_fbit_num)) {
         if (traverse_NumericFormula(t_fbit_num, fbit)) {
            // MATCHED NumericFormula
         } else return ATfalse;
      } else return ATfalse;

      if (ATmatch(t_nbit, "Nbit(<term>)", &t_nbit_num)) {
         if (traverse_NumericFormula(t_nbit_num, nbit)) {
            // MATCHED NumericFormula
         } else return ATfalse;
      } else return ATfalse;
   }

   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_ByteFunctionVariable(ATerm term, SgExpression* &var)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ByteFunctionVariable: %s\n", ATwriteToString(term));
#endif

   ATerm t_var, t_fbit, t_nbit, t_fbit_num, t_nbit_num;
   SgExpression *fbit, *nbit;

   //  'BYTE' '(' Variable ',' Fbit ',' Nbit ')' -> ByteFunctionVariable   {cons("ByteFunctionVariable")}

   if (ATmatch(term, "ByteFunctionVariable(<term>,<term>,<term>)", &t_var, &t_fbit, &t_nbit)) {
      cerr << "WARNING UNIMPLEMENTED: ByteFunctionVariable\n";
      if (traverse_Variable(t_var, var)) {
         // MATCHED Variable
      } else return ATfalse;

      if (ATmatch(t_fbit, "Fbit(<term>)", &t_fbit_num)) {
         if (traverse_NumericFormula(t_fbit_num, fbit)) {
            // MATCHED NumericFormula
         } else return ATfalse;
      } else return ATfalse;

      if (ATmatch(t_nbit, "Nbit(<term>)", &t_nbit_num)) {
         if (traverse_NumericFormula(t_nbit_num, nbit)) {
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
ATbool ATermToSageJovialTraversal::traverse_NamedConstant(ATerm term, SgExpression* &var)
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
ATbool ATermToSageJovialTraversal::traverse_FunctionCall(ATerm term, SgFunctionCallExp* &func_call)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FunctionCall: %s\n", ATwriteToString(term));
#endif

   func_call = nullptr;

   if (traverse_UserDefinedFunctionCall(term, func_call)) {
      // MATCHED UserDefinedFunctionCall
   }
   else if (traverse_IntrinsicFunctionCall(term, func_call)) {
      // MATCHED IntrinsicFunctionCall
   } else return ATfalse;

   //   MachineSpecificFunctionCall -> FunctionCall

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_UserDefinedFunctionCall(ATerm term, SgFunctionCallExp* &func_call)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_UserDefinedFunctionCall: %s\n", ATwriteToString(term));
#endif

   ATerm t_name, t_arg_list;
   std::string name;

   func_call = nullptr;

   if (ATmatch(term, "UserDefinedFunctionCall(<term>,<term>)", &t_name, &t_arg_list)) {
      if (traverse_Name(t_name, name)) {
         // MATCHED FunctionName
      } else return ATfalse;
      cerr << "WARNING UNIMPLEMENTED: UserDefinedFunctionCall for function " << name << endl;

#if 0
      SgUntypedExprListExpression* arg_list = NULL;
      arg_list = new SgUntypedExprListExpression(LanguageTranslation::e_argument_list);
      ROSE_ASSERT(arg_list);
      setSourcePosition(arg_list, t_arg_list);

      if (traverse_ActualParameterList(t_arg_list, arg_list)) {
         // MATCHED ActualParameterList
      } else return ATfalse;
#endif
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_IntrinsicFunctionCall(ATerm term, SgFunctionCallExp* &func_call)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_IntrinsicFunctionCall: %s\n", ATwriteToString(term));
#endif

   func_call = nullptr;

   if (traverse_StatusInverseFunction(term, func_call)) {
      // MATCHED StatusInverseFunction
   }
   else if (traverse_LocFunction(term, func_call)) {
      // MATCHED LocFunction
   }
   else if (traverse_NextFunction(term, func_call)) {
      // MATCHED NextFunction
   }
   else if (traverse_ByteFunction(term, func_call)) {
      // MATCHED ByteFunction
   }

   //   BitFunction                 -> IntrinsicFunctionCall
   //   ShiftFunction               -> IntrinsicFunctionCall
   //   AbsFunction                 -> IntrinsicFunctionCall
   //   SignFunction                -> IntrinsicFunctionCall

   else if (traverse_SizeFunction(term, func_call)) {
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
ATbool ATermToSageJovialTraversal::traverse_LocFunction(ATerm term, SgFunctionCallExp* &func_call)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_LocFunction: %s\n", ATwriteToString(term));
#endif

   ATerm t_argument;
   std::string loc_arg_str;
   SgExpression* loc_arg_expr = nullptr;

   func_call = nullptr;

   if (ATmatch(term, "LocFunction(<term>)", &t_argument)) {
      if (traverse_Name(t_argument, loc_arg_str)) {
         loc_arg_expr = SageBuilder::buildVarRefExp(loc_arg_str, SageBuilder::topScopeStack());
         ROSE_ASSERT(loc_arg_expr);
      }
      else if (traverse_Variable(t_argument, loc_arg_expr)) {
         // MATCHED NamedVariable -> Variable
      } else return ATfalse;
   } else return ATfalse;

   // BlockReference              -> LocArgument
   // TODO
   // cerr << "WARNING UNIMPLEMENTED: LocFunction - BlockReference argument needs reproducer \n";

   ROSE_ASSERT(loc_arg_expr);

   // build the parameter list
   SgExprListExp* params = SageBuilder::buildExprListExp_nfi();
   params->append_expression(loc_arg_expr);

   SgType* return_type = SageBuilder::buildPointerType(SgTypeUnknown::createType());
   // TODO - this should be fixed in post processing, following is a comment in SageBuilder::buildVarRefExp
       // if not found: put fake init name and symbol here and
       // waiting for a postProcessing phase to clean it up
       // two features: no scope and unknown type for initializedName
   // cerr << "WARNING UNIMPLEMENTED: LocFunction - return type is pointer to SgTypeUnknown \n";

   func_call = SageBuilder::buildFunctionCallExp("LOC", return_type, params, SageBuilder::topScopeStack());
   ROSE_ASSERT(func_call);
   setSourcePosition(func_call, term);

   return ATtrue;
}

//========================================================================================
// 6.3.2 NEXT FUNCTIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_NextFunction(ATerm term, SgFunctionCallExp* &func_call)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_NextFunction: %s\n", ATwriteToString(term));
#endif

   ATerm t_argument, t_increment;
   SgExpression * sg_next_arg, * sg_increment;

   func_call = nullptr;

   if (ATmatch(term, "NextFunction(<term>, <term>)", &t_argument, &t_increment)) {
      cerr << "WARNING UNIMPLEMENTED: NextFunction\n";
      if (traverse_GeneralFormula(t_argument, sg_next_arg)) {
         // MATCHED GeneralFormula
      }
      else if (traverse_StatusFormula(t_argument, sg_next_arg)) {
         // MATCHED StatusFormula
      } else return ATfalse;

      if (traverse_NumericFormula(t_increment, sg_increment)) {
         // MATCHED NumericFormula
      } else return ATfalse;
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 6.3.4 BYTE FUNCTIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_ByteFunction(ATerm term, SgFunctionCallExp* &func_call)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ByteFunction: %s\n", ATwriteToString(term));
#endif

   ATerm t_formula, t_fbyte, t_nbyte;
   SgExpression * sg_formula, * sg_fbyte, * sg_nbyte;

   func_call = nullptr;

   if (ATmatch(term, "ByteFunction(<term>, <term>,<term>)", &t_formula, &t_fbyte, &t_nbyte)) {
      cerr << "WARNING UNIMPLEMENTED: ByteFunction\n";
      if (traverse_CharacterFormula(t_formula, sg_formula)) {
         // MATCHED CharacterFormula
      } else return ATfalse;

      if (traverse_NumericFormula(t_fbyte, sg_fbyte)) {
         // MATCHED NumericFormula
      } else return ATfalse;

      if (traverse_NumericFormula(t_nbyte, sg_nbyte)) {
         // MATCHED NumericFormula
      } else return ATfalse;
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 6.3.7 SIZE FUNCTIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_SizeFunction(ATerm term, SgFunctionCallExp* &func_call)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SizeFunction: %s\n", ATwriteToString(term));
#endif

   ATerm t_formula;
   SgExpression* sg_formula;

   func_call = nullptr;

   if (ATmatch(term, "SizeFunction(BITSIZE(),<term>)", &t_formula)) {
      cerr << "WARNING UNIMPLEMENTED: SizeFunction - BITSIZE \n";
      if (traverse_Formula(t_formula, sg_formula)) {
         // MATCHED Formula
      } else return ATfalse;
   }
   else if (ATmatch(term, "SizeFunction(BYTESIZE(),<term>)", &t_formula)) {
      cerr << "WARNING UNIMPLEMENTED: SizeFunction - BYTESIZE \n";
      if (traverse_Formula(t_formula, sg_formula)) {
         // MATCHED Formula
      } else return ATfalse;
   }
   else if (ATmatch(term, "SizeFunction(WORDSIZE(),<term>)", &t_formula)) {
      cerr << "WARNING UNIMPLEMENTED: SizeFunction - WORDSIZE \n";
      if (traverse_Formula(t_formula, sg_formula)) {
         // MATCHED Formula
      } else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 6.3.11 STATUS INVERSE FUNCTIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_StatusInverseFunction(ATerm term, SgFunctionCallExp* &func_call)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_StatusInverseFunction: %s\n", ATwriteToString(term));
#endif

   ATerm t_argument;
   SgExpression* argument;
   std::string function_name;
   std::string var_name;

   func_call = nullptr;

   if (ATmatch(term, "StatusInverseFunctionFIRST(<term>)", &t_argument)) {
      //DONE: cerr << "WARNING UNIMPLEMENTED: StatusInverseFunctionFIRST\n";
      if (traverse_StatusFormula(t_argument, argument)) {
         // MATCHED StatusFormula
      }
      else if (traverse_Name(t_argument, var_name)) {
         // MATCHED StatusTypeName
      }
      else return ATfalse;
      function_name = "FIRST";
   }
   else if (ATmatch(term, "StatusInverseFunctionLAST(<term>)", &t_argument)) {
      //DONE: cerr << "WARNING UNIMPLEMENTED: StatusInverseFunctionLAST\n";
      if (traverse_StatusFormula(t_argument, argument)) {
         // MATCHED StatusFormula
      }
      else if (traverse_Name(t_argument, var_name)) {
         // MATCHED StatusTypeName
      }
      else return ATfalse;
      function_name = "LAST";
   }
   else return ATfalse;

// build the parameter list
   SgExpression*  param  = SageBuilder::buildVarRefExp(var_name, SageBuilder::topScopeStack());
   SgExprListExp* params = SageBuilder::buildExprListExp_nfi();
   params->append_expression(param);

   SgType* return_type = SageBuilder::buildIntType();

   func_call = SageBuilder::buildFunctionCallExp(function_name, return_type, params, SageBuilder::topScopeStack());

   ROSE_ASSERT(func_call);

#if 0
   cout << ".x. Function argument name is " << var_name << endl;
   cout << ".x. Function return_type is  " << return_type << ": " << return_type->class_name() << endl;
   cout << ".x. Function param is  " << param << ": " << param->class_name() << endl;
   cout << ".x. Function param list is  " << params << ": " << params->get_expressions().size() << endl;
   cout << ".x. Function func_call is  " << func_call << ": " << func_call->class_name() << endl;
#endif

   return ATtrue;
}

//========================================================================================
// 7.0 TYPE MATCHING AND TYPE CONVERSIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_BitConversion(ATerm term, SgType* &type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BitConversion: %s\n", ATwriteToString(term));
#endif

   ATerm t_conv, t_formula, t_bit_type_desc;
   SgExpression *formula;
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

      if (traverse_Formula(t_formula, formula)) {
         // MATCHED Formula
      } else return ATfalse;
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_IntegerConversion(ATerm term, SgType* &type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_IntegerConversion: %s\n", ATwriteToString(term));
#endif

   ATerm t_type;
   std::string type_name;

   type = nullptr;

   if (ATmatch(term, "IntegerConversion(<term>)", &t_type)) {
      if (traverse_IntegerItemDescription(t_type, type)) {
         // MATCHED IntegerItemDescription
      } else return ATfalse;

   } else if (ATmatch(term, "IntegerConversionS()")) {
      // MATCHED IntegerConversionS
      type = SageBuilder::buildIntType();
   } else if (ATmatch(term, "IntegerConversionU()")) {
      // MATCHED IntegerConversionU
      type = SageBuilder::buildUnsignedIntType();
   } else return ATfalse;

   ROSE_ASSERT(type);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_TypeNameConversion(ATerm term, SgType* &type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_TypeNameConversion: %s\n", ATwriteToString(term));
#endif

   ATerm t_type_name;
   std::string type_name;
   type = nullptr;

   if (ATmatch(term, "TypeNameConversion(<term>)", &t_type_name)) {
      // MATCHED TypeNameConversion
      if (traverse_OptTypeName(t_type_name, type, type_name)) {
         // MATCHED TypeName
         ROSE_ASSERT(type);
      } else return ATfalse;
   }
   else return ATfalse;

   ROSE_ASSERT(type);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_FloatingConversion(ATerm term, SgType* &type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FloatingConversion: %s\n", ATwriteToString(term));
#endif

   ATerm t_type;
   type = nullptr;

   if (ATmatch(term, "FloatingConversion(<term>)", &t_type)) {

      if (traverse_FloatingItemDescription(t_type, type)) {
         // MATCHED FloatingItemDescription
      } else return ATfalse;

   } else if (ATmatch(term, "FloatingConversionF()")) {
      // MATCHED FloatingConversionF
      type = SageBuilder::buildFloatType();
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_FixedConversion(ATerm term, SgType* &type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FixedConversion: %s\n", ATwriteToString(term));
#endif

   ATerm t_type;

   type = nullptr;

   if (ATmatch(term, "FixedConversion(<term>)", &t_type)) {
      cerr << "WARNING UNIMPLEMENTED: FixedConversion \n";
      if (traverse_FixedItemDescription(t_type, type)) {
         // MATCHED FixedItemDescription
      } else return ATfalse;
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_CharacterConversion(ATerm term, SgType* &type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CharacterConversion: %s\n", ATwriteToString(term));
#endif

   ATerm t_type;

   type = nullptr;

   if (ATmatch(term, "CharacterConversion(<term>)", &t_type)) {
      if (traverse_CharacterItemDescription(t_type, type)) {
         // MATCHED CharacterItemDescription
      } else return ATfalse;
   } else if (ATmatch(term, "CharacterConversionC()")) {
     // MATCHED CharacterConversionC
      type = SageBuilder::buildCharType();
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_StatusConversion(ATerm term, SgType* &type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_StatusConversion: %s\n", ATwriteToString(term));
#endif

   ATerm t_type;
   Sawyer::Optional<SgExpression*> status_size;
   std::list<SgInitializedName*> status_list;

   type = nullptr;

   if (ATmatch(term, "StatusConversion(<term>)", &t_type)) {
      cerr << "WARNING UNIMPLEMENTED: StatusConversion \n";

      if (traverse_StatusItemDescription(t_type, status_list, status_size)) {
         // MATCHED StatusItemDescription
      } else return ATfalse;

   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_PointerConversion(ATerm term, SgType* &type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_PointerConversion: %s\n", ATwriteToString(term));
#endif

   ATerm t_type;

   type = nullptr;

   if (ATmatch(term, "PointerConversion(<term>)", &t_type)) {
      if (traverse_PointerItemDescription(t_type, type)) {
         // MATCHED PointerItemDescription
      } else return ATfalse;
   } else if (ATmatch(term, "PointerConversionP()")) {
     // MATCHED PointerConversionP
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 8.3.1 NUMERIC LITERAL
//----------------------------------------------------------------------------------------

ATbool ATermToSageJovialTraversal::traverse_FixedOrFloatingLiteral(ATerm term, SgExpression* &sg_expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FixedOrFloatingLiteral: %s\n", ATwriteToString(term));
#endif

   ATerm t_frac_form, t_num1, t_num2, t_opt_exp;
   std::string literal, opt_exp;
   char* number;

   sg_expr = nullptr;

// FractionalForm OptExponent -> RealLiteral
   if (ATmatch(term, "RealLiteralFF(<term>,<term>)", &t_frac_form, &t_opt_exp)) {

      if (ATmatch(t_frac_form, "FractionalForm(<term>,<term>)", &t_num1, &t_num2)) {

         // NOTE: reconstruct the integer, ".", fractional part, exponent into one string

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

      }
   }
   else if (ATmatch(term, "RealLiteralIE(<term>,<term>)", &t_num1, &t_opt_exp)) {
      if (ATmatch(t_num1, "<str>", &number)) {
         literal += number;
      } else return ATfalse;

      if (traverse_Exponent(t_opt_exp, opt_exp)) {
         literal += opt_exp;
      } else return ATfalse;
   }
   else return ATfalse;

   if (literal == "." || literal.size() < 1) {
      cerr << "ERROR in traverse_FixedOrFloatingLiteral, no float literal, contains only: " << literal << endl;
      ROSE_ASSERT(false);
      return ATfalse;
   }

   sg_expr = SageBuilder::buildFloatVal_nfi(literal);
   setSourcePosition(sg_expr, term);

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
      LanguageTranslation::ExpressionKind op_enum;
      if (traverse_OptSign(t_sign, op_enum)) {
         // MATCHED OptSign
      } else return ATfalse;

      if (op_enum == LanguageTranslation::e_operator_unary_minus) {
         opt_exp += "E";
         opt_exp += "-";
      }
      else if (op_enum == LanguageTranslation::e_operator_unary_plus) {
         opt_exp += "E";
         opt_exp += "+";
      }
      else if (op_enum == LanguageTranslation::e_operator_unity) {
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
ATbool ATermToSageJovialTraversal::traverse_BitLiteral(ATerm term, SgExpression* &expr)
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

   expr = nullptr;

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

      expr = SageBuilder::buildIntVal_nfi(0, literal);
      setSourcePosition(expr, term);
   } else return ATfalse;

   ROSE_ASSERT(expr);

   return ATtrue;
}

//========================================================================================
// 8.3.3 BOOLEAN LITERAL
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_BooleanLiteral(ATerm term, SgExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BooleanLiteral: %s\n", ATwriteToString(term));
#endif

   expr = nullptr;

   if (ATmatch(term, "True()")) {
      expr = SageBuilder::buildBoolValExp(1);
   } else if (ATmatch(term, "False()")) {
      expr = SageBuilder::buildBoolValExp(0);
   } else return ATfalse;

   ROSE_ASSERT(expr != nullptr);
   setSourcePosition(expr, term);

   return ATtrue;
}

//========================================================================================
// 8.3.4 POINTER LITERAL
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_PointerLiteral(ATerm term, SgExpression* &expr)
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
ATbool ATermToSageJovialTraversal::traverse_DirectiveList(ATerm term)
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
      if (traverse_Directive(head)) {
         // MATCHED Directive
      } else return ATfalse;
   }

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_Directive(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Directive: %s\n", ATwriteToString(term));
#endif

   if (traverse_CompoolDirective(term)) {
      // MATCHED CompoolDirective
   }
   else if (traverse_OrderDirective(term)) {
      // MATCHED OrderDirective
   }
   else if (traverse_ReducibleDirective(term)) {
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
ATbool ATermToSageJovialTraversal::traverse_CompoolDirective(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CompoolDirective: %s\n", ATwriteToString(term));
#endif

   ATerm t_dir_list, t_compool_name, t_decl_name;
   std::string compool_name;
   SgJovialDirectiveStatement* directive_stmt = nullptr;

   if (ATmatch(term, "CompoolDirective(<term>)", &t_dir_list)) {

      if (ATmatch(t_dir_list, "CompoolDirectiveList(<term>)", &t_compool_name)) {
         if (ATmatch(t_compool_name, "no-compool-file-name")) {
            // MATCHED no-compool-file-name
         }
         else if (traverse_CharacterLiteral(t_compool_name, compool_name)) {
            //  '(' OptCompoolFileName ')'    -> CompoolDirectiveList     {cons("CompoolDirectiveList")}
         } else return ATfalse;
      }
      else if (ATmatch(t_dir_list, "CompoolDirectiveList(<term>, <term>)", &t_compool_name, &t_decl_name)) {
         if (ATmatch(t_compool_name, "no-compool-file-name")) {
            // MATCHED no-compool-file-name
         }
         else if (traverse_CharacterLiteral(t_compool_name, compool_name)) {
            //  '(' OptCompoolFileName ')'    -> CompoolDirectiveList     {cons("CompoolDirectiveList")}
         } else return ATfalse;

         ATermList tail = (ATermList) ATmake("<term>", t_decl_name);
         while (! ATisEmpty(tail)) {
            ATerm head = ATgetFirst(tail);
            tail = ATgetNext(tail);
            if (traverse_Name(head, compool_name)) {
               // MATCHED Name
            } else return ATfalse;
         }
      } else return ATfalse;
   }
   else return ATfalse;

// Remove single quotes
// Return to C++11 usage when possible
// if (compool_name.back()  == '\'') compool_name.pop_back();
// if (compool_name.front() == '\'') compool_name = compool_name.substr(1);
   unsigned int len = compool_name.length();
   ROSE_ASSERT(len > 2);
   if (compool_name[0]  == '\'' && compool_name[len-1]) {
      compool_name = compool_name.substr(1,len-2);
   }

   sage_tree_builder.Enter(directive_stmt, compool_name, SgJovialDirectiveStatement::e_compool);
   sage_tree_builder.Leave(directive_stmt);

   return ATtrue;
}

//========================================================================================
// 9.6 REDUCIBLE DIRECTIVES
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_ReducibleDirective(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ReducibleDirective: %s\n", ATwriteToString(term));
#endif

   SgJovialDirectiveStatement* directive_stmt = nullptr;

   if (ATmatch(term, "ReducibleDirective()")) {
      // MATCHED ReducibleDirective
   }
   else return ATfalse;

   sage_tree_builder.Enter(directive_stmt, std::string(""), SgJovialDirectiveStatement::e_reducible);
   sage_tree_builder.Leave(directive_stmt);

   return ATtrue;
}

//========================================================================================
// 9.11 ALLOCATION ORDER DIRECTIVES
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_OrderDirective(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OrderDirective: %s\n", ATwriteToString(term));
#endif

   SgJovialDirectiveStatement* directive_stmt = nullptr;

   if (ATmatch(term, "OrderDirective()")) {
      // MATCHED OrderDirective
   }
   else return ATfalse;

   sage_tree_builder.Enter(directive_stmt, std::string(""), SgJovialDirectiveStatement::e_order);
   sage_tree_builder.Leave(directive_stmt);

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

void ATermToSageJovialTraversal::
setDeclarationModifier(SgVariableDeclaration* var_decl, int def_or_ref)
{
   using namespace LanguageTranslation;

   if (def_or_ref == e_storage_modifier_jovial_def) {
      var_decl->get_declarationModifier().setJovialDef();
   }
   else if (def_or_ref == e_storage_modifier_jovial_ref) {
      var_decl->get_declarationModifier().setJovialRef();
   }
}
