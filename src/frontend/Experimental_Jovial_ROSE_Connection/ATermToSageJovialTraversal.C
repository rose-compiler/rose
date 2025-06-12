#include "sage3basic.h"

#include "Tokens.h"
#include "rose_config.h"
#include "SageTreeBuilder.h"
#include "Jovial_to_ROSE_translation.h"
#include "ATermToSageJovialTraversal.h"

#include <iostream>

#define PRINT_ATERM_TRAVERSAL 0
#if 0
inline int set_bit(int const v, permissions const p)
{
   return v | p;
}
#endif

using namespace ATermSupport;
using namespace Jovial_ROSE_Translation;
using namespace Rose::Diagnostics;
namespace SB = SageBuilder;
namespace SI = SageInterface;
namespace LT = LanguageTranslation;
namespace RB = Rose::builder;

void
ATermToSageJovialTraversal::setSourcePosition(SgLocatedNode* node, ATerm term, bool attach_comments)
{
  // Set source position as normal
  ATermTraversal::setSourcePosition(node, term);
  // and attach comments if they exist
  if (attach_comments) {
    sage_tree_builder.attachComments(node, getLocation(term));
  }
}

void
ATermToSageJovialTraversal::setLocationSpecifier(SgVariableDeclaration* var_decl, const LocationSpecifier &loc_spec)
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

/** Create a mangled name for a StatusConstant */
std::string ATermToSageJovialTraversal::mangleStatusConstantName(const std::string &name)
{
  std::string lcname{name};
  std::transform(name.begin(), name.end(), lcname.begin(), ::tolower);
  return std::string{"_V_"} + lcname;
}

/** Create a mangled name for an anonymous type */
std::string ATermToSageJovialTraversal::mangleAnonymousName(const std::string &name)
{
  std::string lcname{name};
  std::transform(name.begin(), name.end(), lcname.begin(), ::tolower);
  return std::string{"_anon_typeof_"} + lcname;
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
      setSourcePosition(sage_tree_scope, term, false);

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
   }
   else return ATfalse;

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
   RB::SourcePositionPair sources;
   std::vector<RB::Token> saved_comments{};
   SgGlobal* global{nullptr};
   SgJovialCompoolStatement* compool_stmt{nullptr};
   SgNamespaceDeclarationStatement* namespace_decl{nullptr};

   if (ATmatch(term, "CompoolModule(<term>,<term>,<term>)", &t_dirs, &t_name, &t_decls)) {
      // Attach comments preceding CompoolModule START
      global = isSgGlobal(SageBuilder::topScopeStack());
      sage_tree_builder.attachComments(global, PosInfo{global});

   // Traverse Name first to have it available
      if (traverse_Name(t_name, name)) {
         // MATCHED Name
      } else return ATfalse;

   // The directive list should be traversed before the namespace is created in case a compool
   // directive is found and then the loaded compool module can be placed in global scope
   // without creating the namespace. Compool directives should go in global scope.
      if (traverse_DirectiveList(t_dirs)) {
         // MATCHED DirectiveList
      } else return ATfalse;

      sage_tree_builder.Enter(namespace_decl, name, sources);
      setSourcePosition(namespace_decl, term, false);

      sage_tree_builder.Enter(compool_stmt, name, sources);
      sage_tree_builder.Leave(compool_stmt);

      // Let CompoolModule start with its name (rather than global scope). This
      // helps with comment placement.
      setSourcePositionIncludingTerm(compool_stmt, t_name, term);
      sage_tree_builder.attachComments(compool_stmt);

      if (traverse_DeclarationList(t_decls)) {
         // MATCHED DeclarationList and CompoolDeclarationList
      } else return ATfalse;

   } else return ATfalse;

   sage_tree_builder.Leave(namespace_decl);

   // Attach any remaining comments to the global scope
   sage_tree_builder.attachRemainingComments(global, PosInfo{global});

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

   ATerm t_decls, t_funcs;
   SgGlobal* global{nullptr};

   if (ATmatch(term, "ProcedureModule(<term>,<term>)", &t_decls, &t_funcs)) {
      // Attach comments preceding CompoolModule START
      global = isSgGlobal(SageBuilder::topScopeStack());
      sage_tree_builder.attachComments(global, PosInfo{global});

      if (traverse_DeclarationList(t_decls)) {
         // MATCHED DeclarationList
      } else return ATfalse;

      if (traverse_NonNestedSubroutineList(t_funcs)) {
         // MATCHED NonNestedSubroutineList
      } else return ATfalse;
   }
   else return ATfalse;

   // Attach any remaining comments to the global scope
   sage_tree_builder.attachRemainingComments(global, PosInfo{global});

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_DeclsAndStmts(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_DeclarationList: %s\n", ATwriteToString(term));
#endif
   ATerm t_stmts;

   if (ATmatch(term, "DeclsAndStmts(<term>)", &t_stmts)) {
      ATermList tail = (ATermList) ATmake("<term>", t_stmts);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_Declaration(head)) {
            // MATCHED Declaration & CompoolDeclaration
         }
         else if (traverse_Statement(head)) {
            // MATCHED Statement
         }
         else return ATfalse;
      }
   } else return ATfalse;

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
      SgEmptyDeclaration* null_decl = SB::buildEmptyDeclaration();
      setSourcePosition(null_decl, term);
      SI::appendStatement(null_decl, SB::topScopeStack());
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

   ATerm t_decls, t_name, t_body, t_funcs;
   SgGlobal* global{nullptr};
   std::vector<RB::Token> comments{};
   SgProgramHeaderStatement* program_decl{nullptr};

   if (ATmatch(term, "MainProgramModule(<term>,<term>,<term>,<term>)", &t_decls,&t_name,&t_body,&t_funcs)) {
      std::string name;
      std::vector<std::string> labels;

      // Attach comments preceding MainProgramModule START
      global = isSgGlobal(SageBuilder::topScopeStack());
      sage_tree_builder.attachComments(global, PosInfo{global});

      if (traverse_DeclarationList(t_decls)) {
         // MATCHED DeclarationList
      } else return ATfalse;

      if (traverse_Name(t_name, name)) {
         // MATCHED Name
      } else return ATfalse;

   // Note that MainProgramModule term start and end is not that of the function,
   // the Program name is the best we can do for a starting position.
      RB::SourcePosition name_start, name_end; // start and end of program name
      RB::SourcePosition body_start, body_end; // start and end of body
      setSourcePositions(t_name, name_start, name_end);
      setSourcePositions(t_body, body_start, body_end);
      RB::SourcePositions sources(name_start, body_start, body_end);

   // Enter SageTreeBuilder for SgProgramHeaderStatement
      sage_tree_builder.Enter(program_decl, boost::optional<std::string>(name), labels, sources, comments);

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

   // Attach any remaining comments to the global scope
   sage_tree_builder.attachRemainingComments(global, PosInfo{global});

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
   ATerm t_stmt, t_stmts, t_funcs, t_labels;

   if (ATmatch(term, "ProgramSimpleBody(<term>)", &t_stmt)) {
      if (traverse_Statement(t_stmt)) {
         // MATCHED Statement
      } else return ATfalse;
   }
   else if (ATmatch(term, "ProgramBody(<term>,<term>,<term>)", &t_stmts,&t_funcs,&t_labels)) {
      std::vector<std::string> labels;
      std::vector<PosInfo> locations;

      if (traverse_DeclsAndStmts(t_stmts)) {
         // MATCHED DeclarationList
      } else return ATfalse;

      if (traverse_SubroutineDefinitionList(t_funcs)) {
         // MATCHED SubroutineDefinitionList
      } else return ATfalse;

      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;

      if (labels.size() > 1) {
         mlog[WARN] << "UNIMPLEMENTED: ProgramBody - labels.size > 1\n";
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
ATbool ATermToSageJovialTraversal::traverse_IntegerMachineParameter(ATerm term, SgExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_IntegerMachineParameter: %s\n", ATwriteToString(term));
#endif

   ATerm t_precision, t_scale_spec, t_frac_spec, t_formula;
   expr = nullptr;

   // Put these variable references into the global scope to avoid problems in post processing.
   // Consider creating an instrinsic Compool module for the parameters?
   auto scope = SageBuilder::getGlobalScopeFromScopeStack();

   // BITSINBYTE, BITSINWORD, BITSINPOINTER, BYTEPOS, BYTESINWORD
   //
   if (ATmatch(term, "BITSINBYTE")) {
     expr = SageBuilder::buildVarRefExp("BITSINBYTE", scope);
   }
   else if (ATmatch(term, "BITSINWORD")) {
     expr = SageBuilder::buildVarRefExp("BITSINWORD", scope);
   }
   else if (ATmatch(term, "BITSINPOINTER")) {
     expr = SageBuilder::buildVarRefExp("BITSINPOINTER", scope);
   }
   else if (ATmatch(term, "BYTEPOS(<term>)", &t_formula)) {
     SgExpression* pp{nullptr};
     if (traverse_Formula(t_formula, pp)) {
        // MATCHED CompileTimeNumericFormula
     } else return ATfalse;
     auto params = SageBuilder::buildExprListExp_nfi();
     params->append_expression(pp);
     expr = buildIntrinsicFunctionCallExp_nfi(std::string{"BYTEPOS"}, params, scope);
   }
   else if (ATmatch(term, "BYTESINWORD")) {
     expr = SageBuilder::buildVarRefExp("BYTESINWORD", scope);
   }

   // FIXEDPRECISION, FLOATPRECISION, FLOATRADIX, FLOATRELPRECISION, INTPRECISION, LOCSINWORD
   //
   else if (ATmatch(term, "FIXEDPRECISION")) {
     expr = buildIntrinsicVarRefExp_nfi(std::string{"FIXEDPRECISION"}, scope);
   }
   else if (ATmatch(term, "FLOATPRECISION")) {
     expr = SageBuilder::buildVarRefExp("FLOATPRECISION", scope);
   }
   else if (ATmatch(term, "FLOATRADIX")) {
     expr = buildIntrinsicVarRefExp_nfi(std::string{"FLOATRADIX"}, scope);
   }
   else if (ATmatch(term, "FLOATRELPRECISION(<term>)", &t_formula)) {
     SgExpression* precision{nullptr};
     if (traverse_Formula(t_formula, precision)) {
        // MATCHED FloatingMachineParameter
     } else return ATfalse;
     auto params = SageBuilder::buildExprListExp_nfi();
     params->append_expression(precision);
     expr = buildIntrinsicFunctionCallExp_nfi(std::string{"FLOATRELPRECISION"}, params, scope);
   }
   else if (ATmatch(term, "FLOATUNDERFLOW(<term>)", &t_formula)) {
     SgExpression* precision{nullptr};
     if (traverse_Formula(t_formula, precision)) {
        // MATCHED FloatingMachineParameter
     } else return ATfalse;
     auto params = SageBuilder::buildExprListExp_nfi();
     params->append_expression(precision);
     expr = buildIntrinsicFunctionCallExp_nfi(std::string{"FLOATUNDERFLOW"}, params, scope);
   }

   // IMPLINTSIZE, INTPRECISION
   //
   else if (ATmatch(term, "IMPLINTSIZE(<term>)", &t_formula)) {
     // ItemSize is required
     Sawyer::Optional<SgExpression*> size;
     if (traverse_OptItemSize(t_formula, size)) {
       // MATCHED OptItemSize
     } else return ATfalse;
     auto params = SageBuilder::buildExprListExp_nfi();
     params->append_expression(*size);
     expr = buildIntrinsicFunctionCallExp_nfi(std::string{"IMPLINTSIZE"}, params, scope);
   }
   else if (ATmatch(term, "INTPRECISION")) {
     expr = buildIntrinsicVarRefExp_nfi(std::string{"INTPRECISION"}, scope);
   }
   else if (ATmatch(term, "LOCSINWORD")) {
     expr = SageBuilder::buildVarRefExp("LOCSINWORD", scope);
   }

   // MAXBYTES, MAXFLOATPRECISION, MAXFIXEDPRECISION, MAXINT, MAXINTSIZE, MAXSTOP, MAXTABLESIZE, MAXSIGNDIGITS
   //
   else if (ATmatch(term, "MAXBITS")) {
     expr = buildIntrinsicVarRefExp_nfi(std::string{"MAXBITS"}, scope);
   }
   else if (ATmatch(term, "MAXBYTES")) {
     expr = buildIntrinsicVarRefExp_nfi(std::string{"MAXBYTES"}, scope);
   }
   else if (ATmatch(term, "MAXFLOATPRECISION")) {
     expr = SageBuilder::buildVarRefExp("MAXFLOATPRECISION", scope);
   }
   else if (ATmatch(term, "MAXFIXEDPRECISION")) {
     expr = buildIntrinsicVarRefExp_nfi(std::string{"MAXFIXEDPRECISION"}, scope);
   }
   else if (ATmatch(term, "MAXINT(<term>)", &t_formula)) {
     // ItemSize is required
     Sawyer::Optional<SgExpression*> size;
     if (traverse_OptItemSize(t_formula, size)) {
       // MATCHED OptItemSize
     } else return ATfalse;
     if (size) {
       auto params = SageBuilder::buildExprListExp_nfi();
       params->append_expression(*size);
       expr = buildIntrinsicFunctionCallExp_nfi(std::string{"MAXINT"}, params, scope);
     }
     else return ATfalse;
   }
   else if (ATmatch(term, "MAXINTSIZE")) {
     expr = SageBuilder::buildVarRefExp("MAXINTSIZE", scope);
   }
   else if (ATmatch(term, "MAXSTOP")) {
     expr = buildIntrinsicVarRefExp_nfi(std::string{"MAXSTOP"}, scope);
   }
   else if (ATmatch(term, "MAXTABLESIZE")) {
     expr = buildIntrinsicVarRefExp_nfi(std::string{"MAXTABLESIZE"}, scope);
   }
   else if (ATmatch(term, "MAXSIGNDIGITS")) {
     expr = buildIntrinsicVarRefExp_nfi(std::string{"MAXSIGNDIGITS"}, scope);
   }

   // MINFRACTION, MININT, MINSCALE, MINSIZE, MINSTOP
   //
   else if (ATmatch(term, "MINFRACTION(<term>)", &t_formula)) {
      if (traverse_Formula(t_formula, expr)) {
         // MATCHED CompileTimeNumericFormula
      } else return ATfalse;
      auto params = SageBuilder::buildExprListExp_nfi();
      params->append_expression(expr);
      expr = buildIntrinsicFunctionCallExp_nfi(std::string{"MINFRACTION"}, params, scope);
   }
   else if (ATmatch(term, "MININT(<term>)", &t_formula)) {
     // ItemSize is required
     Sawyer::Optional<SgExpression*> size;
     if (traverse_OptItemSize(t_formula, size)) {
       // MATCHED OptItemSize
     } else return ATfalse;
     if (size) {
       auto params = SageBuilder::buildExprListExp_nfi();
       params->append_expression(*size);
       expr = buildIntrinsicFunctionCallExp_nfi(std::string{"MININT"}, params, scope);
     }
     else return ATfalse;
   }
   else if (ATmatch(term, "MINRELPRECISION(<term>)", &t_formula)) {
      if (traverse_Formula(t_formula, expr)) {
         // MATCHED CompileTimeNumericFormula
      } else return ATfalse;
      auto params = SageBuilder::buildExprListExp_nfi();
      params->append_expression(expr);
      expr = buildIntrinsicFunctionCallExp_nfi(std::string{"MINRELPRECISION"}, params, scope);
   }
   else if (ATmatch(term, "MINSCALE(<term>)", &t_formula)) {
      if (traverse_Formula(t_formula, expr)) {
         // MATCHED CompileTimeNumericFormula
      } else return ATfalse;
      auto params = SageBuilder::buildExprListExp_nfi();
      params->append_expression(expr);
      expr = buildIntrinsicFunctionCallExp_nfi(std::string{"MINSCALE"}, params, scope);
   }
   else if (ATmatch(term, "MINSIZE(<term>)", &t_formula)) {
      if (traverse_Formula(t_formula, expr)) {
         // MATCHED CompileTimeNumericFormula
      } else return ATfalse;
      auto params = SageBuilder::buildExprListExp_nfi();
      params->append_expression(expr);
      expr = buildIntrinsicFunctionCallExp_nfi(std::string{"MINSIZE"}, params, scope);
   }
   else if (ATmatch(term, "MINSTOP")) {
     expr = buildIntrinsicVarRefExp_nfi(std::string{"MINSTOP"}, scope);
   }

   else if (ATmatch(term, "IMPLFLOATPRECISION(<term>)", &t_precision)) {
      mlog[WARN] << "UNIMPLEMENTED: IntegerMachineParameter - IMPLFLOATPRECISION\n";
      // MATCHED IMPLFLOATPRECISION
      if (traverse_Formula(t_precision, expr)) {
         // MATCHED Precision
      } else return ATfalse;
   }
   else if (ATmatch(term, "IMPLFIXEDPRECISION(<term>,<term>)", &t_scale_spec, &t_frac_spec)) {
      mlog[WARN] << "UNIMPLEMENTED: IntegerMachineParameter - IMPLFIXEDPRECISION\n";
      // MATCHED IMPLFIXEDPRECISION
      if (traverse_Formula(t_scale_spec, expr)) {
         // MATCHED ScaleSpecifier
      } else return ATfalse;
      if (traverse_Formula(t_frac_spec, expr)) {
         // MATCHED FractionSpecifier
      } else return ATfalse;
   }

   //TODO:
   //      'MAXFLOAT'          '(' Precision ')'     -> FloatingMachineParameter {cons("MAXFLOAT")}
   //      'MINFLOAT'          '(' Precision ')'     -> FloatingMachineParameter {cons("MINFLOAT")}
   //      'MAXFIXED' '(' ScaleSpecifier',' FractionSpecifier ')'-> FixedMachineParameter {cons("MAXFIXED")}
   //      'MINFIXED' '(' ScaleSpecifier ','FractionSpecifier ')'-> FixedMachineParameter    {cons("MINFIXED")}

   else return ATfalse;

   ASSERT_not_null(expr);
   setSourcePosition(expr, term);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_NumericMachineParameter(ATerm term, SgExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_NumericMachineParameter: %s\n", ATwriteToString(term));
#endif

   if (traverse_IntegerMachineParameter(term, expr)) {
      // MATCHED IntegerMachineParameter
   }
   else return ATfalse;

   if (expr == nullptr) {
      mlog[WARN] << "UNIMPLEMENTED: NumericMachineParameter";
      ASSERT_not_null(expr);
   }

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
   else if (traverse_Directive(term)) {
      // MATCHED Directive
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
   else if (traverse_ConstantDeclaration(term, def_or_ref)) {
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
   SgEnumDeclaration* enum_decl = nullptr;
   bool is_anonymous = false;

   std::string label = "";

   if (ATmatch(term, "ItemDeclaration(<term>,<term>,<term>,<term>)", &t_name,&t_alloc,&t_type,&t_preset)) {
      if (ATmatch(t_name, "<str>", &name)) {
         // MATCHED ItemName
      } else return ATfalse;

      if (traverse_OptAllocationSpecifier(t_alloc, modifier_enum)) {
         // MATCHED OptAllocationSpecifier
      } else return ATfalse;

      if (match_StatusItemDescription(t_type)) {
         // Build EnumDecl so that StatusItemDescription traversal has it to use
         is_anonymous = true;
         std::string anon_type_name = mangleAnonymousName(name);

      // Begin SageTreeBuilder
         sage_tree_builder.Enter(enum_decl, anon_type_name);
         setSourcePosition(enum_decl, term);
      }

      if (traverse_ItemTypeDescription(t_type, declared_type)) {
         // MATCHED ItemTypeDescription without StatusItemDescription
      }
      else if (traverse_StatusItemDescription(t_type, enum_decl, status_size)) {
         // MATCHED StatusItemDescription: Note that they are handled differently
         // than other ItemTypeDescriptions because they require different arguments

         if (status_size) {
            SgType* field_type = SageBuilder::buildIntType(*status_size);
            enum_decl->set_field_type(field_type);
         }

         // End SageTreeBuilder
         sage_tree_builder.Leave(enum_decl);

         ASSERT_not_null(enum_decl);
         declared_type = isSgEnumType(enum_decl->get_type());
         ASSERT_not_null(declared_type);
      }
      else return ATfalse;

      // ItemPreset is optional
      if (traverse_ItemPreset(t_preset, preset)) {
         // MATCHED ItemPreset
      } else return ATfalse;
   }
   else return ATfalse;

   if (declared_type == nullptr) {
      mlog[WARN] << "ERROR: ItemDeclaration - variable type is null\n";
      ASSERT_not_null(declared_type);
   }

// Begin SageTreeBuilder
   SgVariableDeclaration* var_decl{nullptr};
   sage_tree_builder.Enter(var_decl, std::string(name), declared_type, preset, std::vector<std::string>{}/*labels*/);
   setSourcePosition(var_decl, term);

// Begin language specific constructs
   setDeclarationModifier(var_decl, def_or_ref);

   if (modifier_enum && *modifier_enum == e_storage_modifier_static) {
      // Set static on both modifier attributes, JovialStatic is used for unparsing
      var_decl->get_declarationModifier().setJovialStatic();
      var_decl->get_declarationModifier().get_storageModifier().setStatic();
   }

// Jovial block and table members are visible in parent scope so create an alias
// to the symbol if needed.
   sage_tree_builder.injectAliasSymbol(std::string(name));

   if (is_anonymous) {
      SgEnumType* enum_type = isSgEnumType(declared_type);
      ASSERT_not_null(enum_type);
      SgEnumDeclaration* decl = isSgEnumDeclaration(enum_type->get_declaration());
      ASSERT_not_null(decl);
      SgEnumDeclaration* def_decl = isSgEnumDeclaration(decl->get_definingDeclaration());
      ASSERT_not_null(def_decl);
      sage_tree_builder.setBaseTypeDefiningDeclaration(var_decl, def_decl);
   }

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

// StatusItemDescription is handled separately because it requires different arguments
   if (match_StatusItemDescription(term)) {
      return ATfalse;
   }

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
   else if (traverse_PointerItemDescription(term, type)) {
      // MATCHED PointerItemDescription
   }
   else if (traverse_Name(term, name)) {
      // MATCHED ItemTypeName
      SgSymbol* symbol = SI::lookupSymbolInParentScopes(name, SB::topScopeStack());

      if (symbol == nullptr) {
         mlog[WARN] << "ERROR: ItemTypeDescription - symbol lookup failed for ItemTypeName " << name << "\n";
      }
      ASSERT_not_null(symbol);
      type = symbol->get_type();
      ASSERT_not_null(type);
   }
   else return ATfalse;

   ASSERT_not_null(type);

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
   ASSERT_not_null(type);

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
      else ROSE_ABORT();

      type = mod_type;
   }
   ASSERT_not_null(type);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_OptItemSize(ATerm term, Sawyer::Optional<SgExpression*> &size)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptItemSize: %s\n", ATwriteToString(term));
#endif
   ATerm t_size;
   SgExpression* size_expr{nullptr};

   if (ATmatch(term, "no-item-size()")) {
      size = Sawyer::Nothing();
   }
   else if (ATmatch(term, "ItemSize(<term>)", &t_size)) {
      if (traverse_Formula(t_size, size_expr)) {
         ASSERT_not_null(size_expr);
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

   ATerm t_floating_type_desc, t_round_or_truncate, t_precision;
   Sawyer::Optional<LanguageTranslation::ExpressionKind> modifier_enum;
   SgExpression* precision = nullptr;

   if (ATmatch(term, "FloatingItemDescription(<term>,<term>,<term>)", &t_floating_type_desc, &t_round_or_truncate, &t_precision)) {
      // MATCHED FloatingItemDescription
   }
   else return ATfalse;

   // The first term, t_floating_type_desc, comes from the lexer and is direct user input: "F", "f".

   // rounding or truncate mode
   if (traverse_OptRoundOrTruncate(t_round_or_truncate, modifier_enum)) {
      // MATCHED OptRoundOrTruncate
   }
   else return ATfalse;

   // precision
   if (ATmatch(t_precision, "no-precision()")) {
      // MATCHED no-precision
   }
   else if (traverse_Formula(t_precision, precision)) {
      // MATCHED Formula
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
      else ROSE_ABORT();

      type = mod_type;
   }
   ASSERT_not_null(type);

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

   ATerm t_fixed_type_desc, t_round_or_truncate, t_scale, t_fraction, t_scale_spec, t_frac_spec;
   Sawyer::Optional<LanguageTranslation::ExpressionKind> modifier_enum;
   SgExpression * scale = nullptr, * fraction = nullptr;

   if (ATmatch(term, "FixedItemDescription(<term>,<term>,<term>,<term>)", &t_fixed_type_desc,&t_round_or_truncate,&t_scale,&t_fraction)) {
      // MATCHED FixedItemDescription
   }
   else return ATfalse;

// The first term, t_fixed_type_desc, comes from the lexer and is direct user input: "A", "a".

   if (traverse_OptRoundOrTruncate(t_round_or_truncate, modifier_enum)) {
      // MATCHED OptRoundOrTruncate
   } else return ATfalse;

   if (ATmatch(t_scale, "ScaleSpecifier(<term>)", &t_scale_spec)) {
      if (traverse_Formula(t_scale_spec, scale)) {
         // MATCHED Formula
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

   type = SageBuilder::buildFixedType(scale, fraction);

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
      else ROSE_ABORT();

      type = mod_type;
   }
   ASSERT_not_null(type);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_FractionSpecifier(ATerm term, SgExpression* &fraction)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FractionSpecifier: %s\n", ATwriteToString(term));
#endif

   ATerm t_fraction;

   if (ATmatch(term, "FractionSpecifier(<term>)", &t_fraction)) {
      if (traverse_Formula(t_fraction, fraction)) {
        // MATCHED Formula
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
   SgExpression* size{nullptr};

   type = nullptr;

   if (ATmatch(term, "BitItemDescription(<term>,<term>)", &t_type_desc, &t_size)) {
    // Ignore the BitTypeDesc term t_type_desc.  It was placed in JovialLex and will be 'B' (just syntax)
   } else return ATfalse;

   if (traverse_OptItemSize(t_size, opt_size)) {
      // MATCHED OptItemSize
   } else return ATfalse;

   if (opt_size) size = *opt_size;

   type = SageBuilder::buildJovialBitType(size);

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

   // The string is enclosed in single quotes, they need to be removed and added back
   // during unparsing.
   ASSERT_require(str_literal.size() >= 2);
   ASSERT_require(str_literal.at(0) == '\'');
   ASSERT_require(str_literal.at(str_literal.size()-1) == '\'');

   str_literal = str_literal.substr(1, str_literal.size()-2);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_CharacterItemDescription(ATerm term, SgType* & type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CharacterItemDescription: %s\n", ATwriteToString(term));
#endif

   ATerm t_char_type_desc, t_size;
   Sawyer::Optional<SgExpression*> size;

   if (ATmatch(term, "CharacterItemDescription(<term>,<term>)", &t_char_type_desc, &t_size)) {
      // MATCHED CharacterItemDescription
      // The first term, t_char_type_desc, comes from the lexer and is direct user input: "C", "c".
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
ATbool ATermToSageJovialTraversal::traverse_StatusConstant(ATerm term, SgEnumDeclaration* enum_decl, int value, SgCastExp* cast)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_StatusConstant: %s\n", ATwriteToString(term));
#endif

// NOTE: Name -> StatusConstant , Letter -> StatusConstant, ReservedWord -> StatusConstant
   char* name;

   if (ATmatch(term, "StatusConstant(<str>)", &name)) {
      std::string enum_name{mangleStatusConstantName(name)};

   // Begin SageTreeBuilder
      SgEnumVal* enum_val = nullptr;
      sage_tree_builder.Enter(enum_val, enum_name, enum_decl, value, cast);

   // There is either an SgEnumVal or an SgCastExp
      if (enum_val) {
         setSourcePosition(enum_val, term);
      }

   // End SageTreeBuilder
      sage_tree_builder.Leave(enum_val);
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_StatusConstant(ATerm term, SgExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_StatusConstant: %s\n", ATwriteToString(term));
#endif

// NOTE: Name -> StatusConstant , Letter -> StatusConstant, ReservedWord -> StatusConstant
   char* name;

   expr = nullptr;

   if (ATmatch(term, "StatusConstant(<str>)", &name)) {
      std::string constant_name{mangleStatusConstantName(name)};

      // An enumerator in Jovial is scoped! Therefore we have to worry about finding the correct one.
      // I think we don't have to worry about it here, perhaps only when using pointers?

      // Would be nice to have a lookupEnumFieldSymbol in SageInterface
      SgSymbol* symbol = SI::lookupSymbolInParentScopes(constant_name, SB::topScopeStack());
      SgEnumFieldSymbol* enum_field_symbol = isSgEnumFieldSymbol(symbol);
      ASSERT_not_null(enum_field_symbol);

      SgInitializedName* init_name = enum_field_symbol->get_declaration();
      SgAssignInitializer* initializer = isSgAssignInitializer(init_name->get_initializer());

      expr = isSgEnumVal(initializer->get_operand());
      if (expr == nullptr) {
        // Could be a cast (see regression test gitlab-issue-261.jov)
        expr = isSgCastExp(initializer->get_operand());
      }
   }
   else return ATfalse;

   ASSERT_not_null(expr);
   setSourcePosition(expr, term);

   return ATtrue;
}

// This match function is helpful because the arguments to traverse_StatusItemDescription
// are different than the other ItemTypeDescriptions
//
ATbool ATermToSageJovialTraversal::match_StatusItemDescription(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... match_StatusItemDescription: %s\n", ATwriteToString(term));
#endif

   ATerm t_size, t_sublist;

   if (ATmatch(term, "StatusItemDescription(<term>,<term>)", &t_size, &t_sublist)) {
      // MATCHED StatusItemDescription
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::
traverse_StatusItemDescription(ATerm term, SgEnumDeclaration* enum_decl, Sawyer::Optional<SgExpression*> &status_size)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_StatusItemDescription: %s\n", ATwriteToString(term));
#endif

   ATerm t_size, t_sublist;

   if (ATmatch(term, "StatusItemDescription(<term>,<term>)", &t_size, &t_sublist)) {
      if (traverse_OptItemSize(t_size, status_size)) {
         // MATCHED OptItemSize
      } else return ATfalse;

      if (traverse_DefaultSublist(t_sublist, enum_decl)) {
         // MATCHED DefaultSublist
      }
      else if (traverse_StatusList(t_sublist, enum_decl)) {
         // MATCHED StatusList
      }
      else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_DefaultSublist(ATerm term, SgEnumDeclaration* enum_decl)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_DefaultSublist: %s\n", ATwriteToString(term));
#endif

   ATerm t_sublist;
   int value = 0;

   if (ATmatch(term, "DefaultSublist(<term>)", &t_sublist)) {
      ATermList tail = (ATermList) ATmake("<term>", t_sublist);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);

         if (traverse_StatusConstant(head, enum_decl, value)) {
            // MATCHED StatusConstant
         } else return ATfalse;

         ++value;
      }
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_OptDefaultSublist(ATerm term, SgEnumDeclaration* enum_decl)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptDefaultSublist: %s\n", ATwriteToString(term));
#endif

   ATerm t_sublist;

   if (ATmatch(term, "no-default-sublist()")) {
      // MATCHED no default sublist
   }
   else if (ATmatch(term, "OptDefaultSublist(<term>)", &t_sublist)) {
      if (traverse_DefaultSublist(t_sublist, enum_decl)) {
         // MATCHED DefaultSublist
      } else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_StatusList(ATerm term, SgEnumDeclaration* enum_decl)
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
      if (traverse_OptDefaultSublist(t_sublist, enum_decl)) {
         // MATCHED OptDefaultSublist
      } else return ATfalse;

      ATermList tail = (ATermList) ATmake("<term>", t_specified);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_SpecifiedSublist(head, enum_decl)) {
           // MATCHED SpecifiedSublist
         } else return ATfalse;
      }
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_SpecifiedSublist(ATerm term, SgEnumDeclaration* enum_decl)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SpecifiedSublist: %s\n", ATwriteToString(term));
#endif

   ATerm t_formula, t_constant;
   SgCastExp* cast{nullptr};
   SgExpression* init_expr{nullptr};

   if (ATmatch(term, "SpecifiedSublist(<term>,<term>)", &t_formula, &t_constant)) {

      if (traverse_Formula(t_formula, init_expr)) {
         // MATCHED Formula
      } else return ATfalse;
      ASSERT_not_null(init_expr);

      int pass = 1;
      int value;

      ATermList tail = (ATermList) ATmake("<term>", t_constant);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);

         if (pass == 1) {
            if (auto intval = isSgIntVal(init_expr)) {
               value = intval->get_value();
               // The initialization expression for the enum is no longer needed
               delete init_expr;  init_expr = nullptr;
            }
            else {
               cast = isSgCastExp(init_expr);
            }
         }
         if (pass > 1) {
            ++value;
         }

         if (traverse_StatusConstant(head, enum_decl, value, cast)) {
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
ATbool ATermToSageJovialTraversal::traverse_PointerItemDescription(ATerm term, SgType* &type)
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

   type = sage_tree_builder.buildPointerType(type_name, type);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_OptTypeName(ATerm term, SgType* &type, std::string &type_name)
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
         // nullptr return is ok
         type = SI::lookupNamedTypeInParentScopes(type_name, SB::topScopeStack());
      }
      else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.2 TABLE DECLARATION
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_TableDeclaration(ATerm term, int def_or_ref, bool constant)
{
#if PRINT_ATERM_TRAVERSAL
   printf("\n... traverse_[Constant]TableDeclaration: %s\n", ATwriteToString(term));
#endif

   ATerm t_name, t_alloc, t_dim_list, t_table_desc;
   char* name;

// A TableDeclaration is a variable declaration.  However, it may also (usually) define a type
// as well, in which case the type will be anonymous, unless only a table type name if given.

// Begin SageTreeBuilder
   SgJovialTableStatement* table_decl = nullptr;
   SgJovialTableType* table_type = nullptr;
   SgType* base_type = nullptr;
   SgType* type = nullptr;
   SgExprListExp* attr_list = nullptr;
   SgExprListExp* dim_info = nullptr;
   SgExpression* preset = nullptr;
   bool table_needs_body = false;
   std::string table_var_name, table_type_name, anon_type_name;

   TableSpecifier table_spec;
   Sawyer::Optional<LanguageTranslation::ExpressionKind> modifier_enum;

// ConstantTableDeclaration is shared with TableDeclaration for reuse
   if (constant) {
      if (ATmatch(term, "ConstantTableDeclaration(<term>,<term>,<term>)", &t_name,&t_dim_list,&t_table_desc)) {
         // MATCHED ConstantTableDeclaration
      } else return ATfalse;
   }
   else {
      if (ATmatch(term, "TableDeclaration(<term>,<term>,<term>,<term>)", &t_name,&t_alloc,&t_dim_list,&t_table_desc)) {
         // MATCHED TableDeclaration
      } else return ATfalse;
   }

   std::string label = "";

   dim_info = SageBuilder::buildExprListExp();

   if (ATmatch(t_name, "<str>", &name)) {
      // MATCHED TableName
   } else return ATfalse;

   table_var_name = std::string(name);
   anon_type_name = mangleAnonymousName(table_var_name);

   if (!constant) {
      if (traverse_OptAllocationSpecifier(t_alloc, modifier_enum)) {
         // MATCHED OptAllocationSpecifier
      } else return ATfalse;
   }

   if (traverse_OptDimensionList(t_dim_list, dim_info)) {
      // MATCHED OptDimensionList
   } else return ATfalse;

// 1. Look for a type name first (there will exist a TYPE declaration for the base type)
//    The type name is the name of the base type.
//
   if (traverse_TableDescriptionName(t_table_desc, table_type_name, base_type, preset)) {
      if (dim_info->get_expressions().size() == 0) {
        table_type = isSgJovialTableType(base_type);
        ASSERT_not_null(table_type);
        // Make sure there isn't a dangling pointer
        delete dim_info;
        dim_info = nullptr;
      }
      else {
        table_type = SB::buildJovialTableType(table_type_name, base_type, dim_info, SB::topScopeStack());
      }
      type = table_type;
   }

// 2. Otherwise look for a primitive base type (similar to the base type of an array type).
//    The base type is the table description and there will be no body.
//
   else if (traverse_TableDescriptionType(t_table_desc, base_type, preset, attr_list, table_spec)) {
      // Check to see if this is a pointer type with SgTypeUnknown as base_type (and use its name, see gitlab-issue-395.jov)
      // Reset pointer base-type name so the base type can be replaced when it has been declared
      if (SgPointerType* pointer = isSgPointerType(base_type)) {
         if (SgJovialTableType* tableType = isSgJovialTableType(pointer->get_base_type())) {
            table_type_name = tableType->get_name();
         }
         else if (SgTypeUnknown* unknown = isSgTypeUnknown(pointer->get_base_type())) {
           table_type_name = unknown->get_type_name();
         }
      }
      table_type = SB::buildJovialTableType(table_type_name, base_type, dim_info, SB::topScopeStack());
      type = table_type;
   }

// 3. Finally check for a table description body. This will need to create a table declaration
//    with a body for the table definition member variables. The declaration will be anonymous
//    and associated with the variable declaration (via baseTypeDefiningDeclaration).
//
//    This is the first pass to create the table type from which a variable declaration
//    can be declared.  A second pass will be required to insert the body of the table.
//    The body can't be read before the variable is created because a table item may
//    reference the table name (in particular, UBOUND(table_name,0)).
//
   else if (traverse_TableDescriptionBody(t_table_desc, anon_type_name, table_decl)) {
      ASSERT_not_null(table_decl);
      setSourcePosition(table_decl, term);

      table_needs_body = true;

      table_type = isSgJovialTableType(table_decl->get_type());
      ASSERT_not_null(table_type);

      if (dim_info) {
         table_type->set_dim_info(dim_info);
         dim_info->set_parent(table_type);
      }
      type = table_type;
   }
   else return ATfalse;

   ASSERT_not_null(type);
   ASSERT_not_null(table_type);

   if (constant) {
   // Create const SgModifierType with declared_type as base_type
      type = SageBuilder::buildConstType(type);
   }

// Begin SageTreeBuilder
   SgVariableDeclaration* var_decl = nullptr;

   sage_tree_builder.Enter(var_decl, table_var_name, type, preset, std::vector<std::string>{}/*labels*/);
   setSourcePosition(var_decl, term);

// Begin language specific constructs
   setDeclarationModifier(var_decl, def_or_ref);

   SgJovialTableStatement* decl = isSgJovialTableStatement(table_type->get_declaration());
   ASSERT_not_null(decl);
   SgJovialTableStatement* def_decl = isSgJovialTableStatement(decl->get_definingDeclaration());
   ASSERT_not_null(def_decl);
   if (dim_info) {
     sage_tree_builder.setBaseTypeDefiningDeclaration(var_decl, def_decl);
   }

   SgClassDefinition* def = def_decl->get_definition();
   ASSERT_not_null(def);
   ASSERT_require(def->isCaseInsensitive());

// Status/enum declarations must be added to the class definition
   if (auto enum_type = isSgEnumType(base_type)) {
      if (auto enum_decl = enum_type->get_declaration()) {
         def->append_member(enum_decl);
         enum_decl->set_parent(def);
      }
   }

// Jovial block and table members are visible in parent scope so create an alias
// to the symbol if needed.
   sage_tree_builder.injectAliasSymbol(table_var_name);

   if (anon_type_name.size() > 0) {
     // The table type should have a symbol also
     sage_tree_builder.injectAliasTypeSymbol(anon_type_name);
   }

   sage_tree_builder.Leave(var_decl);

// Traverse the table description a second time (if required) to insert the table members
// from the table body. See description in comment #3 above as to why this is required.
   if (table_needs_body) {
     // There should not be a preset because it is read with the table body
     ASSERT_require(preset == nullptr);
     ASSERT_not_null(table_decl);
     ASSERT_not_null(var_decl);

     // Insert the table members and get the preset
     ASSERT_require( traverse_TableDescriptionBody(t_table_desc, table_decl, preset, table_spec) );

     // If there is a preset, an initializer must be set now because it wasn't possible when
     // the variable was declared.
     if (preset) {
       SgAssignInitializer* init_expr = SB::buildAssignInitializer_nfi(preset, type);
       SgInitializedName* init_name = var_decl->get_decl_item(table_var_name);
       ASSERT_not_null(init_name);

       init_name->set_initializer(init_expr);
       init_expr->set_parent(init_name);
       setSourcePositionFrom(init_expr, preset);
     }
   }

// Begin language specific constructs
   setDeclarationModifier(var_decl, def_or_ref);

   if (modifier_enum && *modifier_enum == e_storage_modifier_static) {
      // Set static on both modifier attributes, JovialStatic is used for unparsing
      var_decl->get_declarationModifier().setJovialStatic();
      var_decl->get_declarationModifier().get_storageModifier().setStatic();
   }

// Set the structure specifier if present
   const StructureSpecifier& struct_spec = table_spec.struct_spec;
   if (struct_spec.is_parallel) {
      table_type->set_structure_specifier(StrucSpecEnum::e_parallel);
   }
   else if (struct_spec.is_tight) {
      table_type->set_structure_specifier(StrucSpecEnum::e_tight);
      table_type->set_bits_per_entry(struct_spec.bits_per_entry);
   }

   if (table_spec.packing_spec != e_packing_spec_unknown) {
      SgStorageModifier& storage_mod = var_decl->get_declarationModifier().get_storageModifier();
      if      (table_spec.packing_spec == e_packing_spec_none)  storage_mod.setPackingNone();
      else if (table_spec.packing_spec == e_packing_spec_mixed) storage_mod.setPackingMixed();
      else if (table_spec.packing_spec == e_packing_spec_dense) storage_mod.setPackingDense();
   }

// TODO: DELETE_ME - StructureSpecifier information has been placed in SgJovialTableType
// If this works out, this code should be removed and the node deleted in ROSETTA
#if 0
   // Wrap the type in an SgStructureModifier if needed
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
#endif

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
      ASSERT_require(type_name.length() > 0);

   // This type should have already been created by a type declaration statement
      SgSymbol* symbol = SI::lookupSymbolInParentScopes(type_name, SB::topScopeStack());
      if (symbol != nullptr) {
         type = symbol->get_type();
      }
      if (type == nullptr) {
         mlog[WARN] << "ERROR: TableDescriptionName - class symbol is null for type name " << type_name << "\n";
      }

      if (traverse_TablePreset(t_preset, preset)) {
         // MATCHED TablePreset
      } else return ATfalse;

   }
   else return ATfalse;

   if (type == nullptr) {
      mlog[WARN] << "ERROR: TableDescriptionName - type == nullptr\n";
      ASSERT_not_null(type);
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

   LocationSpecifier location_spec; // TODO
   StructureSpecifier& struct_spec{table_spec.struct_spec};

   if (ATmatch(term, "TableDescription(<term>,<term>)", &t_struct_spec, &t_entry_spec)) {

   // This is an EntrySpecifier without a body, although STATUS/enum is a complex type
      if (traverse_EntrySpecifierType(t_entry_spec, type, location_spec, preset, attr_list, table_spec)) {
         // MATCHED EntrySpecifierType
      } else return ATfalse;

      if (traverse_OptStructureSpecifier(t_struct_spec, struct_spec)) {
         // MATCHED OptStructureSpecifier
      } else return ATfalse;
   }
   else return ATfalse;

   ASSERT_not_null(type);

   return ATtrue;
}

// First Pass: This version of the overloaded function creates the table type declaration
//
ATbool ATermToSageJovialTraversal::
traverse_TableDescriptionBody(ATerm term, std::string &type_name, SgJovialTableStatement* &table_decl)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_TableDescription(create table_decl phase): %s\n", ATwriteToString(term));
#endif
   ATerm t_struct_spec, t_entry_spec;
   table_decl = nullptr;

   if (ATmatch(term, "TableDescription(<term>,<term>)", &t_struct_spec, &t_entry_spec)) {
   // Begin SageTreeBuilder
      RB::SourcePositionPair sources;
      sage_tree_builder.Enter(table_decl, type_name, sources);
      ASSERT_not_null(table_decl);
      setSourcePosition(table_decl, term);

   // End SageTreeBuilder
      sage_tree_builder.Leave(table_decl);
   }
   else return ATfalse;

   return ATtrue;
}

// Second Pass: This version of the overloaded function traverses the table body members and preset
//
ATbool ATermToSageJovialTraversal::
traverse_TableDescriptionBody(ATerm term, SgJovialTableStatement* table_decl,
                                          SgExpression* &preset, TableSpecifier &table_spec)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_TableDescription(add body phase): %s\n", ATwriteToString(term));
#endif

   ATerm t_struct_spec, t_entry_spec;

   ASSERT_not_null(table_decl);
   StructureSpecifier& struct_spec = table_spec.struct_spec;

   if (ATmatch(term, "TableDescription(<term>,<term>)", &t_struct_spec, &t_entry_spec)) {

      if (traverse_OptStructureSpecifier(t_struct_spec, struct_spec)) {
         // MATCHED OptStructureSpecifier
      } else return ATfalse;

      SgClassDefinition* table_def = table_decl->get_definition();
      ASSERT_not_null(table_def);
      SageBuilder::pushScopeStack(table_def);

      if (traverse_EntrySpecifierBody(t_entry_spec, table_decl, preset, table_spec)) {
        // MATCHED EntrySpecifierBody
      } else return ATfalse;

      SageBuilder::popScopeStack();
   }
   else return ATfalse;

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

   ASSERT_not_null(shape);
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
         } else return ATfalse;
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

   if (ATmatch(term, "Dimension(<term>,<term>)", &t_opt_lower_bound, &t_upper_bound)) {

   // Lower bound (optional)
      if (ATmatch(t_opt_lower_bound, "no-lower-bound-option()")) {
         // lower bound has already been initialized to nullptr
      }
      else if (ATmatch(t_opt_lower_bound, "LowerBoundOption(<term>)", &t_lower_bound)) {
      // Lower bound can be either a (CompileTime) Formula or a StatusFormula
         if (traverse_Formula(t_lower_bound, lower_bound)) {
            // MATCHED Formula
         }
         else if (traverse_StatusFormula(t_lower_bound, lower_bound)) {
            // MATCHED StatusFormula
         }
         else return ATfalse;
      }
      else return ATfalse;

   // Upper bound can be either a (CompileTime) Formula or a StatusFormula
      if (traverse_Formula(t_upper_bound, upper_bound)) {
         // MATCHED Formula
      }
      else if (traverse_StatusFormula(t_upper_bound, upper_bound)) {
         // MATCHED StatusFormula
      }
      else return ATfalse;
   }
   else if (ATmatch(term, "DimensionSTAR()")) {
      lower_bound = SageBuilder::buildNullExpression_nfi();
      upper_bound = new SgAsteriskShapeExp();
      setSourcePosition(upper_bound, term);
   }
   else return ATfalse;

// Begin SageTreeBuilder
   if (stride == nullptr) {
      // ROSE (Fortran) uses "1" rather than SgNullExpression
      stride = SageBuilder::buildIntVal_nfi(std::string("1"));
   }

   range = SageBuilder::buildSubscriptExpression_nfi(lower_bound, upper_bound, stride);
   range->set_parent(shape);
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
      else if (traverse_Formula(t_bits_per_entry, bits_per_entry)) {
         SgIntVal* int_val = isSgIntVal(bits_per_entry);
         ASSERT_not_null(int_val);
         ASSERT_require(int_val->get_value() > 0);
         struct_spec.bits_per_entry = int_val->get_value();
         // the expression/formula for the bits_per_entry is no longer needed as the value is stored
         delete bits_per_entry;  bits_per_entry = nullptr;
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
   SgEnumDeclaration* enum_decl{nullptr};
   std::string label{""};

   if (ATmatch(term, "OrdinaryEntrySpecifier(<term>,<term>,<term>)", &t_pack_spec, &t_item_desc, &t_preset)) {

      if (traverse_OptPackingSpecifier(t_pack_spec, table_spec.packing_spec)) {
         // MATCHED OptPackingSpecifier
      } else return ATfalse;

      if (traverse_ItemTypeDescription(t_item_desc, type)) {
         // MATCHED ItemTypeDescription found a base type
      }
      else if (match_StatusItemDescription(t_item_desc)) {
         // MATCHED StatusItemDescription
         Sawyer::Optional<SgExpression*> status_size;

         // Pick a number to use in making a unique anonymous name
         std::string id{Rose::StringUtility::numberToString(reinterpret_cast<uintptr_t>(term))};

         // Create an SgEnumDeclaration with associated SgEnumType to receive status items.
         enum_decl = SageBuilder::buildEnumDeclaration_nfi(mangleAnonymousName(id), SB::topScopeStack());
         setSourcePosition(enum_decl, t_item_desc);

         // The enum declaration can be retrieved from the type in later processing
         type = enum_decl->get_type();

         if (traverse_StatusItemDescription(t_item_desc, enum_decl, status_size)) {
            // MATCHED StatusItemDescription
            if (status_size) {
               SgTypeInt* field_type = SageBuilder::buildIntType(*status_size);
               enum_decl->set_field_type(field_type);
            }
         }
      }
      else return ATfalse;

      if (traverse_TablePreset(t_preset, preset)) {
         // MATCHED TablePreset
      } else return ATfalse;
   }
   else return ATfalse;

   ASSERT_not_null(type);

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

   SgType* declared_type = nullptr;
   SgExpression* preset = nullptr;
   SgVariableDeclaration* var_decl = nullptr;
   Sawyer::Optional<SgExpression*> status_size;
   SgEnumDeclaration* enum_decl = nullptr;
   bool is_anonymous = false;

   std::string label = "";

   if (ATmatch(term, "OrdinaryTableItemDeclaration(<term>,<term>,<term>,<term>)", &t_name,&t_item_desc,&t_pack_spec,&t_preset)) {

      if (ATmatch(t_name, "<str>", &name)) {
         // MATCHED TableItemName
      } else return ATfalse;

      if (match_StatusItemDescription(t_item_desc)) {
         // Build an SgEnumDeclaration so that the StatusItemDescription traversal has it available

         is_anonymous = true;
         std::string anon_type_name = mangleAnonymousName(name);

      // Begin SageTreeBuilder for enum
         sage_tree_builder.Enter(enum_decl, anon_type_name);
         setSourcePosition(enum_decl, term);
      }

      if (traverse_ItemTypeDescription(t_item_desc, declared_type)) {
         // MATCHED ItemTypeDescription without StatusItemDescription
      }
      else if (traverse_StatusItemDescription(t_item_desc, enum_decl, status_size)) {
         // MATCHED StatusItemDescription
         if (status_size) {
            SgType* field_type = SageBuilder::buildIntType(*status_size);
            enum_decl->set_field_type(field_type);
         }

         // End SageTreeBuilder for enum
         sage_tree_builder.Leave(enum_decl);

         ASSERT_not_null(enum_decl);
         declared_type = isSgEnumType(enum_decl->get_type());
         ASSERT_not_null(declared_type);
      }
      else return ATfalse;

      if (traverse_TablePreset(t_preset, preset)) {
         // MATCHED TablePreset
      } else return ATfalse;

   } else return ATfalse;

   if (declared_type == nullptr) {
      mlog[WARN] << "UNIMPLEMENTED: OrdinaryTableItemDeclaration - declared_type is null\n";
      ROSE_ABORT();
   }

// Begin SageTreeBuilder for variable
   sage_tree_builder.Enter(var_decl, std::string(name), declared_type, preset, std::vector<std::string>{}/*labels*/);
   setSourcePosition(var_decl, term);

// Jovial block and table members are visible in parent scope so create an alias
// to the symbol if needed.
   sage_tree_builder.injectAliasSymbol(std::string(name));

   if (is_anonymous) {
      SgEnumType* enum_type = isSgEnumType(declared_type);
      ASSERT_not_null(enum_type);
      SgEnumDeclaration* decl = isSgEnumDeclaration(enum_type->get_declaration());
      ASSERT_not_null(decl);
      SgEnumDeclaration* def_decl = isSgEnumDeclaration(decl->get_definingDeclaration());
      ASSERT_not_null(def_decl);
      sage_tree_builder.setBaseTypeDefiningDeclaration(var_decl, def_decl);
   }

// End SageTreeBuilder for variable
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
   else if (ATmatch(term, "PackingSpecifier(\"N\")")) {
      packing_spec = e_packing_spec_none;
   }
   else if (ATmatch(term, "PackingSpecifier(\"M\")")) {
      packing_spec = e_packing_spec_mixed;
   }
   else if (ATmatch(term, "PackingSpecifier(\"D\")")) {
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

  Sawyer::Optional<SgExpression*> entry_size;
  WordsPerEntry wpe;

  if (ATmatch(term, "SpecifiedEntrySpecifier(<term>,<term>,<term>)", &t_words, &t_item_desc, &t_preset)) {

    if (traverse_WordsPerEntry(t_words, entry_size, wpe)) {
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

  if (entry_size) {
    mlog[WARN] << "UNIMPLEMENTED: entry_size is " << *entry_size << ": " << (*entry_size)->class_name() << "\n";
  }
  ROSE_ABORT();
}

ATbool ATermToSageJovialTraversal::
traverse_SpecifiedEntrySpecifierBody(ATerm term, SgJovialTableStatement* table_decl, SgExpression* &preset)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SpecifiedEntrySpecifierBody: %s\n", ATwriteToString(term));
#endif

   ATerm t_words, t_preset, t_body;
   Sawyer::Optional<SgExpression*> entry_size;
   WordsPerEntry wpe;

   preset = nullptr;

   if (ATmatch(term, "SpecifiedEntrySpecifierBody(<term>,<term>,<term>)", &t_words, &t_preset, &t_body)) {

      if (traverse_WordsPerEntry(t_words, entry_size, wpe)) {
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

   table_decl->set_words_per_entry(wpe);
   if (entry_size) {
     table_decl->set_has_table_entry_size(true);
     table_decl->set_table_entry_size(*entry_size);
     (*entry_size)->set_parent(table_decl);
   }
   else {
     table_decl->set_has_table_entry_size(false);
     table_decl->set_table_entry_size(nullptr);
   }

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_SpecifiedItemDescription(ATerm term, SgType* &type, LocationSpecifier &loc_spec, SgExprListExp* attr_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SpecifiedItemDescription: %s\n", ATwriteToString(term));
#endif

   ATerm t_item_desc, t_loc_spec, t_start_bit, t_start_word;

   ASSERT_not_null(attr_list);

   SgExpression* start_bit = nullptr;
   SgExpression* start_word = nullptr;

// For StatusItemDescription
   Sawyer::Optional<SgExpression*> status_size;
   SgEnumDeclaration* enum_decl = nullptr;

   type = nullptr;
   std::string label = "";

   if (ATmatch(term, "SpecifiedItemDescription(<term>,<term>)", &t_item_desc, &t_loc_spec)) {

      if (traverse_ItemTypeDescription(t_item_desc, type)) {
         // MATCHED ItemTypeDescription without StatusItemDescription
      }  else if (traverse_StatusItemDescription(t_item_desc, enum_decl, status_size)) {
         // MATCHED StatusItemDescription
         // status item declarations have to be handled differently than other ItemTypeDescription terms

         mlog[WARN] << "UNIMPLEMENTED: SpecifiedItemDescription with StatusItemDescription\n";
         ROSE_ABORT();
      } else return ATfalse;

      // process location-specifier here (don't really need to call a function)
      if (ATmatch(t_loc_spec, "LocationSpecifier(<term>,<term>)", &t_start_bit, &t_start_word)) {

         if (ATmatch(t_start_bit, "StartingBitSTAR()")) {
            start_bit = new SgAsteriskShapeExp();
            ASSERT_not_null(start_bit);
            setSourcePosition(start_bit, t_start_bit);
         }
         else if (traverse_Formula(t_start_bit, start_bit)) {
            // MATCHED StartingBit
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
   else if (traverse_SpecifiedTableItemDeclaration(term)) {
      // MATCHED SpecifiedTableItemDeclaration
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_SpecifiedTableItemDeclaration(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SpecifiedTableItemDeclaration: %s\n", ATwriteToString(term));
#endif

   ATerm t_name, t_item_desc, t_preset, t_item_desc2, t_loc_spec, t_start_bit, t_start_word;
   char* name;

   SgExpression *preset{nullptr}, *start_bit{nullptr}, *start_word{nullptr};
   SgType* item_type = nullptr;
   SgExprListExp* attr_list = nullptr;
   LocationSpecifier loc_spec(nullptr,nullptr);
   SgEnumDeclaration* enum_decl = nullptr;
   SgVariableDeclaration* var_decl = nullptr;
   bool is_anon = false;

   if (ATmatch(term, "SpecifiedTableItemDeclaration(<term>,<term>,<term>)", &t_name, &t_item_desc, &t_preset)) {
      if (ATmatch(t_name, "<str>", &name)) {
         // MATCHED TableItemName
      } else return ATfalse;

      attr_list = SageBuilder::buildExprListExp();
      setSourcePosition(attr_list, t_item_desc);

      if (ATmatch(t_item_desc, "SpecifiedItemDescription(<term>,<term>)", &t_item_desc2, &t_loc_spec)) {

      // 1. Look for a StatusItemDescription first
         if (match_StatusItemDescription(t_item_desc2)) {
            Sawyer::Optional<SgExpression*> status_size;

         // This is an anonymous status type because not ItemTypeDescription so will have body
            std::string enum_anon_name = mangleAnonymousName(name);
            is_anon = true;
            enum_decl = nullptr;

         // Begin SageTreeBuilder
            sage_tree_builder.Enter(enum_decl, enum_anon_name);
            setSourcePosition(enum_decl, term);
            if (traverse_StatusItemDescription(t_item_desc2, enum_decl, status_size)) {
               if (status_size) {
                  SgTypeInt* field_type = SageBuilder::buildIntType(*status_size);
                  enum_decl->set_field_type(field_type);
               }

            // End SageTreeBuilder
               sage_tree_builder.Leave(enum_decl);

               item_type = enum_decl->get_type();
            }
            else {
               mlog[ERROR] << "matched an StatusItemDescription but failed in building an SgEnumDeclaration \n";
               ROSE_ABORT();
            }
         }

      // 2. This will be a named or intrinsic type
         else if (traverse_ItemTypeDescription(t_item_desc2, item_type)) {
            // MATCHED ItemTypeDescription without StatusItemDescription
         }
         else return ATfalse;

      // process location-specifier here (don't really need to call a function)
         if (ATmatch(t_loc_spec, "LocationSpecifier(<term>,<term>)", &t_start_bit, &t_start_word)) {

           if (ATmatch(t_start_bit, "StartingBitSTAR()")) {
              start_bit = new SgAsteriskShapeExp();
              ASSERT_not_null(start_bit);
              setSourcePosition(start_bit, t_start_bit);
           }
           else if (traverse_Formula(t_start_bit, start_bit)) {
              // MATCHED StartingBit
           } else return ATfalse;

           if (traverse_Formula(t_start_word, start_word)) {
              // MATCHED StartingWord
           } else return ATfalse;

           loc_spec = LocationSpecifier(start_bit, start_word);
         }

      } else return ATfalse;

      if (traverse_TablePreset(t_preset, preset)) {
         // MATCHED TablePreset
      } else return ATfalse;

   } else return ATfalse;

   ASSERT_not_null(attr_list);

   if (item_type == nullptr) {
      mlog[WARN] << "UNIMPLEMENTED: SpecifiedTableItemDeclaration - declared type is null\n";
      return ATtrue;
   }

// Begin SageTreeBuilder
   sage_tree_builder.Enter(var_decl, std::string(name), item_type, preset, std::vector<std::string>{}/*labels*/);
   setSourcePosition(var_decl, term);

// The bitfield is used to contain both the start_bit and start_word as an expression list
   setLocationSpecifier(var_decl, loc_spec);

   if (is_anon) {
      SgEnumDeclaration* def_decl = isSgEnumDeclaration(enum_decl->get_definingDeclaration());
      ASSERT_not_null(def_decl);
      sage_tree_builder.setBaseTypeDefiningDeclaration(var_decl, def_decl);
   }

// Jovial block and table members are visible in parent scope so create an alias
// to the symbol if needed.
   sage_tree_builder.injectAliasSymbol(std::string(name));

// End SageTreeBuilder
   sage_tree_builder.Leave(var_decl);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_WordsPerEntry(ATerm term, Sawyer::Optional<SgExpression*> &entry_size, WordsPerEntry &wpe)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_WordsPerEntry: %s\n", ATwriteToString(term));
#endif

   ATerm t_size;

   SgExpression* table_entry_size = nullptr;
   entry_size = Sawyer::Nothing();
   wpe = WordsPerEntry::e_default;

   if (ATmatch(term, "WordsPerEntryW(<term>)", &t_size)) {
      wpe = WordsPerEntry::e_fixed_length;
      if (ATmatch(t_size, "no-entry-size()")) {
         // MATCHED no-entry-size
      }
      else if (traverse_Formula(t_size, table_entry_size)) {
         ASSERT_not_null(table_entry_size);
         entry_size = Sawyer::Optional<SgExpression*>(table_entry_size);
      }
      else return ATfalse;
   }
   else if (ATmatch(term, "WordsPerEntryV()")) {
      wpe = WordsPerEntry::e_variable_length;
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.3 CONSTANT DECLARATIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_ConstantDeclaration(ATerm term, int defOrRef)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ConstantDeclaration: %s\n", ATwriteToString(term));
#endif

   ATerm t_name, t_type, t_preset, t_dim_list, t_table_desc;
   char* name;

   bool isAnon{false};
   std::string label{};

   SgType* declaredType{nullptr};
   SgType* constType{nullptr};
   SgExpression* preset{nullptr};
   SgEnumDeclaration* enumDecl{nullptr};
   Sawyer::Optional<SgExpression*> statusSize;

   if (ATmatch(term, "ConstantTableDeclaration(<term>,<term>,<term>)", &t_name,&t_dim_list,&t_table_desc)) {
   // Almost everything in ConstantTableDeclaration is shared with TableDeclaration so reuse it
      if (traverse_TableDeclaration(term, defOrRef, /*constant*/true)) {
         // MATCHED ConstantTableDeclaration
      }
      else return ATfalse;

   // The rest (variable declaration part) has been completed by traverse_TableDeclaration
      return ATtrue;
   }

   else if (ATmatch(term, "ConstantItemDeclaration(<term>,<term>,<term>)", &t_name, &t_type, &t_preset)) {
      if (ATmatch(t_name, "<str>", &name)) {
         // MATCHED ItemName
      } else return ATfalse;

      if (match_StatusItemDescription(t_type)) {
         // Build EnumDecl so that StatusItemDescription traversal has it to use
         isAnon = true;
         std::string anonTypeName = mangleAnonymousName(name);

         // Begin SageTreeBuilder for enum
         sage_tree_builder.Enter(enumDecl, anonTypeName);
         setSourcePosition(enumDecl, t_type);
      }

      if (traverse_ItemTypeDescription(t_type, declaredType)) {
         // MATCHED ItemTypeDescription without StatusItemDescription

         // Create const SgModifierType with declared_type as base_type
         constType = SageBuilder::buildConstType(declaredType);
      }
      else if (traverse_StatusItemDescription(t_type, enumDecl, statusSize)) {
         // MATCHED StatusItemDescription: Note that they are handled differently
         // than other ItemTypeDescriptions because they require different arguments

         if (statusSize) {
            SgType* fieldType{SageBuilder::buildIntType(*statusSize)};
            enumDecl->set_field_type(fieldType);
         }

         // End SageTreeBuilder
         sage_tree_builder.Leave(enumDecl);

         // Create const SgModifierType with declared_type as base_type
         declaredType = isSgEnumType(enumDecl->get_type());
         constType = SageBuilder::buildConstType(declaredType);
      }
      else return ATfalse;

      if (traverse_ItemPreset(t_preset, preset)) {
         // MATCHED ItemPreset
      } else return ATfalse;
   }
   else return ATfalse;

   if (constType == nullptr) {
      mlog[ERROR] << "UNIMPLEMENTED: ConstantDeclaration - type is null";
      ROSE_ABORT();
   }

   // Begin SageTreeBuilder for variable declaration
   SgVariableDeclaration* varDecl{nullptr};
   sage_tree_builder.Enter(varDecl, std::string(name), constType, preset, std::vector<std::string>{}/*labels*/);
   setSourcePosition(varDecl, term);

   if (preset) {
      // Set source position of initializer before var_decl for comment handling
      ASSERT_not_null(varDecl->get_decl_item(name));
      SgInitializer* initializer = varDecl->get_decl_item(name)->get_initializer();
      ASSERT_not_null(initializer);
      setSourcePosition(initializer, t_preset);
   }

   // Jovial block and table members are visible in parent scope so create an alias
   // to the symbol if needed.
   sage_tree_builder.injectAliasSymbol(std::string(name));

   if (isAnon) {
      SgEnumType* enumType = isSgEnumType(declaredType);
      ASSERT_not_null(enumType);
      SgEnumDeclaration* decl = isSgEnumDeclaration(enumType->get_declaration());
      ASSERT_not_null(decl);
      SgEnumDeclaration* defDecl = isSgEnumDeclaration(decl->get_definingDeclaration());
      ASSERT_not_null(defDecl);
      sage_tree_builder.setBaseTypeDefiningDeclaration(varDecl, defDecl);
   }

   // End SageTreeBuilder for variable declaration
   sage_tree_builder.Leave(varDecl);

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
   SgExpression* preset = nullptr;
   SgType* type = nullptr;
   std::string type_name;

   if (ATmatch(term, "BlockDeclarationBodyPart(<term>,<term>,<term>)", &t_name, &t_alloc, &t_body)) {
      is_anon = true;

      if (traverse_Name(t_name, block_name)) {
         // MATCHED BlockName
      } else return ATfalse;

      block_type_name = mangleAnonymousName(block_name);
      type_name = block_type_name;

      // Begin SageTreeBuilder for type declaration
      RB::SourcePositionPair sources;
      sage_tree_builder.Enter(block_decl, type_name, sources, /*is_block*/ true);
      setSourcePosition(block_decl, term);

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

      ASSERT_require(block_type_name.length() > 0);

      // This type should have already been created by a type declaration statement
      SgClassSymbol* class_symbol = SI::lookupClassSymbolInParentScopes(block_type_name, SB::topScopeStack());
      if (class_symbol != nullptr) {
         type = class_symbol->get_type();
      }
      ASSERT_not_null(type);

      if (traverse_OptAllocationSpecifier(t_alloc, modifier_enum)) {
         // MATCHED OptAllocationSpecifier
      } else return ATfalse;

      if (traverse_BlockPreset(t_preset, preset)) {
         // MATCHED BlockPreset
      } else return ATfalse;
   }
   else return ATfalse;

   // Begin SageTreeBuilder for variable declaration
   SgVariableDeclaration* var_decl = nullptr;
   sage_tree_builder.Enter(var_decl, std::string(block_name), type, preset, std::vector<std::string>{}/*labels*/);
   setSourcePosition(var_decl, term);

   // Begin language specific constructs
   setDeclarationModifier(var_decl, def_or_ref);

   if (modifier_enum && *modifier_enum == e_storage_modifier_static) {
      // Set static on both modifier attributes, JovialStatic is used for unparsing
      var_decl->get_declarationModifier().setJovialStatic();
      var_decl->get_declarationModifier().get_storageModifier().setStatic();
   }

   if (is_anon) {
      SgJovialTableStatement* def_decl = isSgJovialTableStatement(block_decl->get_definingDeclaration());
      ASSERT_not_null(def_decl);
      sage_tree_builder.setBaseTypeDefiningDeclaration(var_decl, def_decl);
   }

// Jovial block and table members are visible in parent scope so create an alias
// to the symbol if needed.
   sage_tree_builder.injectAliasSymbol(std::string(block_name));

  // End SageTreeBuilder for variable declaration
   sage_tree_builder.Leave(var_decl);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_BlockBodyPart(ATerm term, SgJovialTableStatement* /*blockDecl*/)
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

ATbool ATermToSageJovialTraversal::traverse_BlockPreset(ATerm term, SgExpression* &preset)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BlockPreset: %s\n", ATwriteToString(term));
#endif

   ATerm t_preset_list;

   preset = nullptr;

   if (ATmatch(term, "no-block-preset")) {
      // MATCHED no-block-preset
   }
   else if (ATmatch(term, "BlockPreset(<term>)", &t_preset_list)) {

      SgExprListExp* preset_list = SageBuilder::buildExprListExp_nfi();
      SgJovialTablePresetExp* block_preset = new SgJovialTablePresetExp(preset_list);
      ASSERT_not_null(block_preset);
      setSourcePosition(block_preset, term);

      preset = block_preset;

      if (traverse_BlockPresetList(t_preset_list, block_preset)) {
         // MATCHED BlockPresetList
      } else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_BlockPresetList(ATerm term, SgJovialTablePresetExp* block_preset)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BlockPresetList: %s\n", ATwriteToString(term));
#endif

   ATerm t_preset_value_list, t_table_preset_list;

   ASSERT_not_null(block_preset);
   SgExprListExp* preset_list = block_preset->get_preset_list();

   if (ATmatch(term, "BlockPresetList(<term>)", &t_preset_value_list)) {
      SgExpression* preset_value = nullptr;

      ATermList tail = (ATermList) ATmake("<term>", t_preset_value_list);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);

      // Check for parentheses
         if (ATmatch(head, "BlockPresetValueParens(<term>)", &t_table_preset_list)) {
           block_preset->set_need_paren(true);
           if (traverse_TablePresetList(t_table_preset_list, block_preset)) {
             // MATCHED a TablePresetList for block preset values
           }
           else {
             // There is likely something like "(TablePresetList), (BlockPresetList)"
             // This may require modifying grammar or multiple lists.
             mlog[WARN] << "UNIMPLEMENTED: BlockPresetList with ( BlockPresetList )\n";
             return ATfalse;
           }
         }
         else if (traverse_TablePresetValue(head, preset_value)) {
           // MATCHED PresetValue
           ASSERT_not_null(preset_value);
           preset_list->get_expressions().push_back(preset_value);
           preset_value->set_parent(preset_list);
         }
         else return ATfalse;
      }
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
      } else return ATfalse;

      ASSERT_not_null(preset);
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

   // In the process of building an initialization expression, setting this flag will
   // aid in disambiguation of undeclared variable and function symbols.
   sage_tree_builder.setInitializationContext(true);

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

   sage_tree_builder.setInitializationContext(false);

   ASSERT_not_null(preset);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_TablePreset(ATerm term, SgExpression* &preset)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_TablePreset: %s\n", ATwriteToString(term));
#endif

   ATerm t_preset_list;

   preset = nullptr;

   if (ATmatch(term, "no-table-preset()")) {
      // MATCHED no-table-preset
   }
   else if (ATmatch(term, "TablePreset(<term>)", &t_preset_list)) {
      SgExprListExp* preset_list = SageBuilder::buildExprListExp_nfi();
      SgJovialTablePresetExp* table_preset = new SgJovialTablePresetExp(preset_list);
      ASSERT_not_null(table_preset);

      preset = table_preset;

      if (traverse_TablePresetList(t_preset_list, table_preset)) {
         // MATCHED TablePresetList
      }
      else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_TablePresetList(ATerm term, SgJovialTablePresetExp* table_preset)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_TablePresetList: %s\n", ATwriteToString(term));
#endif

   ATerm t_preset_value_list;

   ASSERT_not_null(table_preset);
   SgExprListExp* preset_list = table_preset->get_preset_list();

   if (ATmatch(term, "TablePresetList(<term>)", &t_preset_value_list)) {
      SgExpression* preset_value = nullptr;

      ATermList tail = (ATermList) ATmake("<term>", t_preset_value_list);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_TablePresetValue(head, preset_value)) {
            // MATCHED PresetValue
            ASSERT_not_null(preset_value);
            preset_list->get_expressions().push_back(preset_value);
            preset_value->set_parent(preset_list);
         } else return ATfalse;
      }
   }
   else return ATfalse;

   // Wait to set the source position until the entire preset list has been processed
   setSourcePosition(table_preset, term, /*attach_comments*/true);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_PresetIndexSpecifier(ATerm term, SgExprListExp* index_specifier_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_PresetIndexSpecifier: %s\n", ATwriteToString(term));
#endif

   //  'POS' '(' {ConstantIndex ','}+ ')' ':'  -> PresetIndexSpecifier  {cons("PresetIndexSpecifier")}
   //  CompileTimeNumericFormula       -> ConstantIndex
   //  CompileTimeStatusFormula        -> ConstantIndex

   ATerm t_const_index;

   if (ATmatch(term, "PresetIndexSpecifier(<term>)", &t_const_index)) {
      SgExpression* constant_index;

      ATermList tail = (ATermList) ATmake("<term>", t_const_index);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);

         constant_index = nullptr;
         if (traverse_Formula(head, constant_index)) {
            // MATCHED CompileTimeFormula & CompileTimeStatusFormula
         } else return ATfalse;
         ASSERT_not_null(constant_index);

         index_specifier_list->get_expressions().push_back(constant_index);
         constant_index->set_parent(index_specifier_list);
      }
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_TablePresetValue(ATerm term, SgExpression* &preset)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_TablePresetValue: %s\n", ATwriteToString(term));
#endif

   ATerm t_rep_count, t_preset_value, t_preset_values, t_index_list;
   preset = nullptr;

   if (ATmatch(term, "PresetValueRep(<term>,<term>)", &t_rep_count, &t_preset_values)) {
      SgExpression* rep_count = nullptr;
      SgExprListExp* values_list = SageBuilder::buildExprListExp_nfi();
      setSourcePosition(values_list, t_preset_values);

      if (traverse_Formula(t_rep_count, rep_count)) {
         // MATCHED Formula for repetition count
      } else return ATfalse;

   // Fill list of PresetValues
      ATermList tail = (ATermList) ATmake("<term>", t_preset_values);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);

         SgExpression* preset_value = nullptr;
         if (traverse_TablePresetValue(head, preset_value)) {
            ASSERT_not_null(preset_value);
            values_list->get_expressions().push_back(preset_value);
            preset_value->set_parent(values_list);
         } else return ATfalse;
      }
      ASSERT_not_null(rep_count);

      preset = SageBuilder::buildReplicationOp_nfi(rep_count, values_list);
      setSourcePosition(preset, term);
   }
   else if (ATmatch(term, "PresetValuePositioner(<term>,<term>)", &t_index_list, &t_preset_value)) {
      SgExprListExp* index_list = SageBuilder::buildExprListExp_nfi();
      setSourcePosition(index_list, t_index_list);

      // Fill list of IndexValues
      ATermList tail = (ATermList) ATmake("<term>", t_index_list);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);

         SgExpression* index = nullptr;
         if (traverse_Formula(head, index)) {
            ASSERT_not_null(index);
            index_list->get_expressions().push_back(index);
            index->set_parent(index_list);
         } else return ATfalse;
      }

      SgExpression* preset_value = nullptr;
      if (traverse_TablePresetValue(t_preset_value, preset_value)) {
         // MATCHED TablePresetValue
      } else return ATfalse;
      ASSERT_not_null(preset_value);

      preset = new SgJovialPresetPositionExp(index_list, preset_value);
      ASSERT_not_null(preset);
      setSourcePosition(preset, term);
   }
   else if (ATmatch(term, "PresetValueNone()")) {
      preset = SageBuilder::buildNullExpression_nfi();
      setSourcePosition(preset, term);
   }
   else if (traverse_Formula(term, preset)) {
      // MATCHED Formula for preset value
   }
   else return ATfalse;

   ASSERT_not_null(preset);

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

   std::string label = "";

   if (ATmatch(term, "ItemTypeDeclaration(<term>,<term>)", &t_name, &t_type_desc)) {

      if (traverse_Name(t_name, name)) {
         // MATCHED ItemTypeName
      } else return ATfalse;

   // Look for a StatusItemDescription first
      if (match_StatusItemDescription(t_type_desc)) {
         SgEnumDeclaration* enum_decl = nullptr;
         Sawyer::Optional<SgExpression*> status_size;

      // Begin SageTreeBuilder
         sage_tree_builder.Enter(enum_decl, name);
         setSourcePosition(enum_decl, term);

         if (traverse_StatusItemDescription(t_type_desc, enum_decl, status_size)) {
            if (status_size) {
               SgTypeInt* field_type = SageBuilder::buildIntType(*status_size);
               enum_decl->set_field_type(field_type);
            }

         // End SageTreeBuilder
            sage_tree_builder.Leave(enum_decl);
         }
         else {
            mlog[ERROR] << "matched an StatusItemDescription but failed in building an SgEnumDeclaration \n";
            ROSE_ABORT();
         }
      }

   // If not a StatusItemDescription look for an ItemTypeDescription
      else if (traverse_ItemTypeDescription(t_type_desc, declared_type)) {
         // MATCHED ItemTypeDescription without StatusItemDescription

         ASSERT_not_null(declared_type);

      // Begin SageTreeBuilder
         SgTypedefDeclaration* type_def = nullptr;
         sage_tree_builder.Enter(type_def, name, declared_type);
         setSourcePosition(type_def, term);

      // End SageTreeBuilder
         sage_tree_builder.Leave(type_def);
      }
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
      RB::SourcePositionPair sources;
      sage_tree_builder.Enter(table_decl, type_name, sources);
      setSourcePosition(table_decl, term);

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
   SgType* base_type{nullptr};
   SgJovialTableType* parent_type{nullptr};

   ASSERT_not_null(table_decl);

   ATerm t_dim_list, t_struct_spec, t_like_option, t_entry_spec, t_type_name;
   std::string table_type_name;
   TableSpecifier table_spec;

   SgJovialTableType* table_type = isSgJovialTableType(table_decl->get_type());
   ASSERT_not_null(table_type);

   SgExprListExp* dim_info{table_type->get_dim_info()};

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
      } else return ATfalse;

   // This type should have already been created by a type declaration statement, find it
      SgClassSymbol* class_symbol = SI::lookupClassSymbolInParentScopes(table_type_name, SB::topScopeStack());
      if (class_symbol != nullptr) {
         parent_type = isSgJovialTableType(class_symbol->get_type());
      }
      ASSERT_not_null(parent_type);

      // 1. The class declaration for the base class should be the first non-defining declaration of the base class
      // 2. The class definition should be the class definition of the derived class
      //
      SgClassDeclaration* base_class_decl = isSgClassDeclaration(parent_type->get_declaration());
      ASSERT_not_null(base_class_decl);
      
   // DQ (12/26/2011): The non defining declaration should not have a valid pointer to the class definition.
      ASSERT_require(base_class_decl->get_definition() == nullptr);

      SgClassDefinition* derived_class_def = table_decl->get_definition();
      ASSERT_not_null(derived_class_def);

      // The sage builder function sets the parent of the base class to the inherited class
      // definition and adds the base class to its inheritances list.
      SgBaseClass* base_class = SB::buildBaseClass(base_class_decl, derived_class_def, false, /*isDirect*/true);
      ASSERT_not_null(base_class);
   }

// TableTypeSpecifier with four arguments where t_entry_spec specifies the type (could be primitive or anonymous)
   else if (ATmatch(term, "TableTypeSpecifier(<term>,<term>,<term>,<term>)",
                          &t_dim_list, &t_struct_spec, &t_like_option, &t_entry_spec)) {

      StructureSpecifier& struct_spec{table_spec.struct_spec};

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
            table_decl->set_has_like_option(true);
            table_decl->set_like_table_name(table_type_name);
         } else return ATfalse;
      }
      else return ATfalse;

   // TODO - something with these
      SgExpression* preset{nullptr};
      SgExprListExp* attr_list{nullptr};
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

      if (preset) {
         mlog[WARN] << "UNIMPLEMENTED: TableTypeSpecifier - preset (This is likely fixed, please confirm)\n";
         ASSERT_require(preset == nullptr);
      }
      if (attr_list) {
         mlog[WARN] << "UNIMPLEMENTED: TableTypeSpecifier - attr_list\n";
         ASSERT_require(attr_list == nullptr);
      }
   }
   else return ATfalse;

   table_type->set_dim_info(dim_info);
   dim_info->set_parent(table_type);

   if (base_type) {
      table_type->set_base_type(base_type);
      base_type->set_parent(table_type);
   }

// Set the structure specifier if present
   StructureSpecifier& struct_spec{table_spec.struct_spec};
   if (struct_spec.is_parallel) {
      table_type->set_structure_specifier(StrucSpecEnum::e_parallel);
   }
   else if (struct_spec.is_tight) {
      table_type->set_structure_specifier(StrucSpecEnum::e_tight);
      table_type->set_bits_per_entry(struct_spec.bits_per_entry);
   }

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_BlockTypeDeclaration(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BlockTypeDeclaration: %s\n", ATwriteToString(term));
#endif

   ATerm t_name, t_type_desc, t_order;
   std::string type_name;

   SgJovialTableStatement* block_decl = nullptr;
   bool hasBlockDirective = false;

   if (ATmatch(term, "BlockTypeDeclaration(<term>,<term>)", &t_name, &t_type_desc)) {
      hasBlockDirective = false;
   }
   else if (ATmatch(term, "BlockOrderedTypeDeclaration(<term>,<term>,<term>)", &t_name, &t_order, &t_type_desc)) {
      hasBlockDirective = true;
   }
   else return ATfalse;

   if (traverse_Name(t_name, type_name)) {
      // MATCHED BlockTypeName
   } else return ATfalse;

   // Begin SageTreeBuilder
   RB::SourcePositionPair sources;
   sage_tree_builder.Enter(block_decl, type_name, sources, /*is_block*/ true);
   setSourcePosition(block_decl, term);

   if (hasBlockDirective) {
      if (traverse_Directive(t_order)) {
         // MATCHED OrderDirective
      } else return ATfalse;
   }

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

   // Note: Can be a name list, not just a name.
   // Currently a separate SgLabelStatement is created for each name.
   //
   if (ATmatch(term, "StatementNameDeclaration(<term>)", &t_name)) {
      ATermList tail = (ATermList) ATmake("<term>", t_name);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_Name(head, name)) {
            auto labelType{SgLabelStatement::e_jovial_label_decl};
            if (def_or_ref == LanguageTranslation::e_storage_modifier_jovial_def) {
              labelType = SgLabelStatement::e_jovial_label_def;
            } else if (def_or_ref == LanguageTranslation::e_storage_modifier_jovial_ref) {
              labelType = SgLabelStatement::e_jovial_label_ref;
            }

            // Make a separate label statement for each name in the list
            //
            // WARNING: Do not use SageTreeBuilder!
            //   A StatementNameDeclaration should not create a symbol, it is effectively a noop,
            //   as it is not needed for normal label handling and may get in the way, see gitlab-issue.310.jov
            //   Thus statement creation has to be done here, can't even use SageBuilder.
            //
            //   Standard (MIL-STD-1589C) states that <statement-name-declaration> must either be a
            //   <formal-input-parameter> to the subroutine containing the <statement-name-declaration>
            //   or else must be used in a <label> in the immediate scope containing the
            //   <statement-name-declaration> (i.e., no including nested scopes), or else
            //   the <statement-name-declaration> must be a <ref-specification-choice>
            //
            // NOTE: Perhaps a symbol is created by later processing, to encourage correct label handling,
            //       see gitlab-issue-{310,343,345}, create an SgStringVal and save it for later. If not
            //       needed it will need to be deleted.
            //

            SgLabelStatement* labelStmt = new SgLabelStatement(name, /*statement*/nullptr);

            setSourcePosition(labelStmt, head);
            labelStmt->set_label_type(labelType);

            // Save an SgStringVal for later processing (if needed)
            auto stringVal = SB::buildStringVal_nfi(name);
            labelDecls_.insert(std::make_pair(name, stringVal));

            SI::appendStatement(labelStmt, SB::topScopeStack());
         }
         else return ATfalse;
      }
   }
   else return ATfalse;

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

   SgJovialDefineDeclaration* define_decl = SB::buildJovialDefineDeclaration_nfi(name, params, def_string);
   setSourcePosition(define_decl, term);

   SI::appendStatement(define_decl, SB::topScopeStack());

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_DefinitionPart(ATerm term, std::string &params, std::string &def_string)
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

ATbool ATermToSageJovialTraversal::traverse_FormalDefineParameterList(ATerm term, std::string &params)
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
         first = false;

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
   ASSERT_require(len > 1);
   ASSERT_require(def_string[0] == '?' && def_string[len-1] == '?');

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

   ATerm t_simple_def;

   if (ATmatch(term, "SimpleDef(<term>)", &t_simple_def)) {
      if (traverse_DefSpecificationChoice(t_simple_def)) {
         // MATCHED DefSpecificationChoice
      } else return ATfalse;
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
   } else if (traverse_OverlayDeclaration(term)) {
      // MATCHED OverlayDeclaration
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

   SgJovialOverlayDeclaration* overlay_decl = nullptr;

   if (ATmatch(term, "OverlayDeclaration(<term>,<term>)", &t_addr, &t_expr)) {
      SgExpression* address = nullptr;
      SgExprListExp* overlay_expr = nullptr;

      if (ATmatch(t_addr, "AbsoluteAddress(<term>)", &t_absolute_addr)) {
         // 'POS' '(' OverlayAddress ')'    -> AbsoluteAddress      {cons("AbsoluteAddress")}
         if (traverse_Formula(t_absolute_addr, address)) {
            // MATCHED OverlayAddress
            // CompileTimeNumericFormula       -> OverlayAddress
         } else return ATfalse;
      }
      else if (ATmatch(t_addr, "no-absolute-address")) {
         address = SageBuilder::buildNullExpression_nfi();
      }
      else return ATfalse;

      if (traverse_OverlayExpression(t_expr, overlay_expr)) {
         // MATCHED OverlayExpression
      } else return ATfalse;
      ASSERT_not_null(overlay_expr);

   // Begin SageTreeBuilder
      sage_tree_builder.Enter(overlay_decl, address, overlay_expr);
      setSourcePosition(overlay_decl, term);
   }
   else return ATfalse;

   ASSERT_not_null(overlay_decl);

// End SageTreeBuilder
   sage_tree_builder.Leave(overlay_decl);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_OverlayExpression(ATerm term, SgExprListExp* &overlay_expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OverlayExpression: %s\n", ATwriteToString(term));
#endif
   //   {OverlayString ':'}+            -> OverlayExpression

   SgExprListExp* overlay_string;
   overlay_expr = SageBuilder::buildExprListExp_nfi();
   setSourcePosition(overlay_expr, term);

   ATermList tail = (ATermList) ATmake("<term>", term);
   while (! ATisEmpty(tail)) {
      ATerm head = ATgetFirst(tail);
      tail = ATgetNext(tail);

      overlay_string = nullptr;
      if (traverse_OverlayString(head, overlay_string)) {
         // MATCHED OverlayString
      } else return ATfalse;
      ASSERT_not_null(overlay_string);

      overlay_expr->get_expressions().push_back(overlay_string);
      overlay_string->set_parent(overlay_expr);
   }

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_OverlayString(ATerm term, SgExprListExp* &overlay_string)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OverlayString: %s\n", ATwriteToString(term));
#endif

   //  {OverlayElement ','}+           -> OverlayString

   SgExpression* overlay_element;
   overlay_string = SageBuilder::buildExprListExp_nfi();
   setSourcePosition(overlay_string, term);

   ATermList tail = (ATermList) ATmake("<term>", term);
   while (! ATisEmpty(tail)) {
      ATerm head = ATgetFirst(tail);
      tail = ATgetNext(tail);

      overlay_element = nullptr;
      if (traverse_OverlayElement(head, overlay_element)) {
         // MATCHED OverlayElement
      } else return ATfalse;
      ASSERT_not_null(overlay_element);

      overlay_string->get_expressions().push_back(overlay_element);
      overlay_element->set_parent(overlay_string);
   }

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_OverlayElement(ATerm term, SgExpression* &overlay_element)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OverlayElement: %s\n", ATwriteToString(term));
#endif

   //  Spacer                          -> OverlayElement
   //  DataName                        -> OverlayElement
   //  '(' OverlayExpression ')'       -> OverlayElement       {cons("OverlayElement")}

   ATerm t_expr;
   std::string name;

   // Could make a new node, SgJovialOverlayElement. However, following convention used for now:
   //   1. Spacer is a SgExpression;
   //   2. DataName is a SgVarRefExp
   //   3. OverlayExpression is a SgExprListExp

   SgExpression* spacer = nullptr;
   SgVarRefExp* data_name = nullptr;
   SgExprListExp* overlay_expr = nullptr;

   if (ATmatch(term, "OverlayElement(<term>)", &t_expr)) {
      if (traverse_OverlayExpression(t_expr, overlay_expr)) {
         ASSERT_not_null(overlay_expr);
         setSourcePosition(overlay_expr, t_expr);
         overlay_element = overlay_expr;
      } else return ATfalse;
   }
   else if (traverse_Spacer(term, spacer)) {
      ASSERT_not_null(spacer);
      setSourcePosition(spacer, term);
      overlay_element = spacer;
   }
   else if (traverse_Name(term, name)) {
      data_name = SB::buildVarRefExp(name, SB::topScopeStack());
      setSourcePosition(data_name, term);
      overlay_element = data_name;
   } else return ATfalse;

   ASSERT_not_null(overlay_element);

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
      if (traverse_Formula(t_num, expr)) {
         // MATCHED Formula
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

   ATerm t_proc_heading, t_directives, t_decl;

   std::string name;
   std::list<FormalParameter> param_name_list;
   std::vector<RB::Token> comments{};

   SgFunctionDeclaration* function_decl{nullptr};
   SgFunctionParameterList* param_list{nullptr};
   SgScopeStatement* param_scope{nullptr};
   bool is_defining_decl{true};

   if (ATmatch(term, "ProcedureDeclaration(<term>,<term>,<term>)", &t_proc_heading, &t_directives, &t_decl)) {
      // Save any comments preceding ProcedureDeclaration
      sage_tree_builder.consumePrecedingComments(comments, getLocation(term));

      if (traverse_ProcedureHeading(t_proc_heading, name, param_name_list, modifiers)) {
         // MATCHED ProcedureHeading
      } else return ATfalse;

      is_defining_decl = ! sage_tree_builder.list_contains(modifiers, e_function_modifier_reference);

   // Enter SageTreeBuilder for SgFunctionParameterList
      sage_tree_builder.Enter(param_list, param_scope, name, nullptr, is_defining_decl);

      if (traverse_DirectiveList(t_directives)) {
         // MATCHED PostProcDirective*
      } else return ATfalse;

   // These declarations will stored in the function parameter scope
      if (traverse_Declaration(t_decl)) {
         // MATCHED Declaration
      } else return ATfalse;

   // Leave SageTreeBuilder for SgFunctionParameterList
      sage_tree_builder.Leave(param_list, param_scope, param_name_list);
   }
   else return ATfalse;

   RB::SourcePosition heading_start, heading_end; // start and end of ProcedureHeading
   RB::SourcePosition decl_start, decl_end;       // start and end of Declaration
   setSourcePositions(t_proc_heading, heading_start, heading_end);
   setSourcePositions(t_decl, decl_start, decl_end);

   RB::SourcePositions sources(heading_start, decl_start, decl_end);

// Enter SageTreeBuilder for SgFunctionDeclaration
   sage_tree_builder.Enter(function_decl, name, /*return_type*/nullptr,
                           param_list, modifiers, is_defining_decl, sources, comments);

// Leave SageTreeBuilder for SgFunctionDeclaration
   sage_tree_builder.Leave(function_decl, param_scope);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_ProcedureDefinition(ATerm term, LanguageTranslation::FunctionModifierList &modifiers)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ProcedureDefinition: %s\n", ATwriteToString(term));
#endif

   ATerm t_proc_heading, t_directives, t_proc_body;

   std::string name{};
   std::list<FormalParameter> param_name_list{};
   std::vector<RB::Token> comments{};

   SgFunctionDeclaration* function_decl{nullptr};
   SgFunctionParameterList* param_list{nullptr};
   SgScopeStatement* param_scope{nullptr};
   bool is_defining_decl{true};

   if (ATmatch(term, "ProcedureDefinition(<term>,<term>,<term>)", &t_proc_heading, &t_directives, &t_proc_body)) {
      // Save comments preceding body, otherwise they will be consumed by SubroutineBody
      sage_tree_builder.consumePrecedingComments(comments, getLocation(t_proc_body));

      if (traverse_ProcedureHeading(t_proc_heading, name, param_name_list, modifiers)) {
         // MATCHED ProcedureHeading
      } else return ATfalse;

   // Enter SageTreeBuilder for SgFunctionParameterList
      sage_tree_builder.Enter(param_list, param_scope, name, nullptr, is_defining_decl);

      if (traverse_DirectiveList(t_directives)) {
         // MATCHED PostProcDirective*
      } else return ATfalse;

   // These declarations will stored in the function parameter scope
      if (traverse_SubroutineBody(t_proc_body)) {
         // MATCHED ProcedureBody (the production is actually a SubroutineBody)
      } else return ATfalse;

   // Leave SageTreeBuilder for SgFunctionParameterList
      sage_tree_builder.Leave(param_list, param_scope, param_name_list);
   }
   else return ATfalse;

   RB::SourcePosition proc_start, proc_end; // start and end of ProcedureDefinition
   RB::SourcePosition body_start, body_end; // start and end of SubroutineBody
   setSourcePositions(term, proc_start, proc_end);
   setSourcePositions(t_proc_body, body_start, body_end);

   RB::SourcePositions sources(proc_start, body_start, proc_end);

// Enter SageTreeBuilder for SgFunctionDeclaration
   sage_tree_builder.Enter(function_decl, name, /*return_type*/nullptr,
                           param_list, modifiers, is_defining_decl, sources, comments);

// Leave SageTreeBuilder for SgFunctionDeclaration
   sage_tree_builder.Leave(function_decl, param_scope);

// Remaining source positions (last, comment processing may conflict with Leave() calls)
   setSourcePosition(param_list, t_proc_heading);

//TODO: Replace with a function
// There may be label strings that should be replaced by label references
   for (const auto &kvp: labelRefs_) {
     SgStringVal* strVal = kvp.second;

     // If strVal has no parent, it won't have been used, delete it
     // With labelDecls_ this should always be the case, delete labelDecls_ and stringVals
     ASSERT_not_null(strVal);
     ASSERT_not_null(strVal->get_parent());
#if 0
     if (strVal && strVal->get_parent() == nullptr) {
       delete strVal;
       continue;
     }
#endif

     if (strVal && strVal->get_parent() && strVal->get_value() == kvp.first) {
       // Replace original SgStringVal with an SgLabelRefExp
       SgScopeStatement* scope = function_decl->get_definition();
       auto labelSymbol = isSgLabelSymbol(SI::lookupSymbolInParentScopes(kvp.first, scope));
       if (labelSymbol) {
         auto labelRef = SB::buildLabelRefExp(labelSymbol);
         SI::setOneSourcePositionNull(labelRef);
         SI::replaceExpression(strVal, labelRef, /*keepOldExp*/false);
       }
     }
   }
   labelRefs_.clear();

   for (const auto &kvp: labelDecls_) {
     SgStringVal* strVal = kvp.second;
     delete strVal;
   }
   labelDecls_.clear();

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
   ATerm t_stmts, t_funcs, t_labels;
   std::vector<std::string> labels;
   std::vector<PosInfo> locations;
   std::string temp_label = "";

   if (ATmatch(term, "SubroutineSimpleBody(<term>)", &t_stmt)) {
      if (traverse_Statement(t_stmt)) {
         // MATCHED Statement
      } else return ATfalse;
   }
   else if (ATmatch(term, "SubroutineBody(<term>,<term>,<term>)", &t_stmts,&t_funcs,&t_labels)) {
      if (traverse_DeclsAndStmts(t_stmts)) {
         // MATCHED Declarations and Statements
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

ATbool ATermToSageJovialTraversal::
traverse_FunctionDeclaration(ATerm term, LanguageTranslation::FunctionModifierList &modifiers)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FunctionDeclaration: %s\n", ATwriteToString(term));
#endif

   ATerm t_func_heading, t_directives, t_decl;

   std::string name;
   SgType* return_type = nullptr;
   std::list<FormalParameter> param_name_list;
   std::vector<RB::Token> comments{};

   SgFunctionDeclaration* function_decl{nullptr};
   SgFunctionParameterList* param_list{nullptr};
   SgScopeStatement* param_scope{nullptr};
   bool is_defining_decl{true};

   if (ATmatch(term, "FunctionDeclaration(<term>,<term>,<term>)", &t_func_heading, &t_directives, &t_decl)) {

      if (traverse_FunctionHeading(t_func_heading, name, return_type, param_name_list, modifiers)) {
         // MATCHED FunctionHeading
      } else return ATfalse;

      is_defining_decl = ! sage_tree_builder.list_contains(modifiers, e_function_modifier_reference);

   // Enter SageTreeBuilder for SgFunctionParameterList
      sage_tree_builder.Enter(param_list, param_scope, name, return_type, is_defining_decl);

      if (traverse_DirectiveList(t_directives)) {
         // MATCHED PostProcDirective*
      } else return ATfalse;

      if (traverse_Declaration(t_decl)) {
         // MATCHED Declaration
      } else return ATfalse;

   // Leave SageTreeBuilder for SgFunctionParameterList
      sage_tree_builder.Leave(param_list, param_scope, param_name_list);
   }
   else return ATfalse;

   RB::SourcePosition heading_start, heading_end; // start and end of FunctionHeading
   RB::SourcePosition decl_start, decl_end;       // start and end of Declaration
   setSourcePositions(t_func_heading, heading_start, heading_end);
   setSourcePositions(t_decl, decl_start, decl_end);

   RB::SourcePositions sources(heading_start, decl_start, decl_end);

// Enter SageTreeBuilder for SgFunctionDeclaration
   sage_tree_builder.Enter(function_decl, name, return_type, param_list,
                           modifiers, is_defining_decl, sources, comments);

// Leave SageTreeBuilder for SgFunctionDeclaration
   sage_tree_builder.Leave(function_decl, param_scope);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_FunctionDefinition(ATerm term, LanguageTranslation::FunctionModifierList &modifiers)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FunctionDefinition: %s\n", ATwriteToString(term));
#endif

   ATerm t_func_heading, t_directives, t_func_body;

   std::string name;
   SgType* return_type = nullptr;
   std::list<FormalParameter> param_name_list;
   std::vector<RB::Token> comments{};

   SgFunctionDeclaration* function_decl{nullptr};
   SgFunctionParameterList* param_list{nullptr};
   SgScopeStatement* param_scope{nullptr};
   bool is_defining_decl{true};

   if (ATmatch(term, "FunctionDefinition(<term>,<term>,<term>)", &t_func_heading, &t_directives, &t_func_body)) {
      // Save comments preceding body, otherwise they will be consumed by SubroutineBody
      sage_tree_builder.consumePrecedingComments(comments, getLocation(t_func_body));

      if (traverse_FunctionHeading(t_func_heading, name, return_type, param_name_list, modifiers)) {
         // MATCHED FunctionHeading
      } else return ATfalse;

   // Enter SageTreeBuilder for SgFunctionParameterList
      sage_tree_builder.Enter(param_list, param_scope, name, return_type, is_defining_decl);

      if (traverse_DirectiveList(t_directives)) {
         // MATCHED PostProcDirective*
      } else return ATfalse;

      if (traverse_SubroutineBody(t_func_body)) {
         // MATCHED FunctionBody
      } else return ATfalse;

   // Leave SageTreeBuilder for SgFunctionParameterList
      sage_tree_builder.Leave(param_list, param_scope, param_name_list);
   }
   else return ATfalse;

   RB::SourcePosition heading_start, heading_end; // start and end of FunctionHeading
   RB::SourcePosition body_start, body_end;       // start and end of SubroutineBody
   setSourcePositions(t_func_heading, heading_start, heading_end);
   setSourcePositions(t_func_body, body_start, body_end);

   RB::SourcePositions sources(heading_start, body_start, body_end);

// Enter SageTreeBuilder for SgFunctionDeclaration
   sage_tree_builder.Enter(function_decl, name, return_type, param_list,
                           modifiers, is_defining_decl, sources, comments);

// Leave SageTreeBuilder for SgFunctionDeclaration
   sage_tree_builder.Leave(function_decl, param_scope);

//TODO: Replace with a function
// There may be label strings that should be replaced by label references
   for (const auto &kvp: labelRefs_) {
     SgStringVal* strVal = kvp.second;

     // If strVal has no parent, it won't have been used, delete it
     // With labelDecls_ this should always be the case, delete labelDecls_ and stringVals
     ASSERT_not_null(strVal);
     ASSERT_not_null(strVal->get_parent());
#if 0
     if (strVal && strVal->get_parent() == nullptr) {
       delete strVal;
       continue;
     }
#endif

     if (strVal && strVal->get_parent() && strVal->get_value() == kvp.first) {
       // Replace original SgStringVal with an SgLabelRefExp
       SgScopeStatement* scope = function_decl->get_definition();
       auto labelSymbol = isSgLabelSymbol(SI::lookupSymbolInParentScopes(kvp.first, scope));
       if (labelSymbol) {
         auto labelRef = SB::buildLabelRefExp(labelSymbol);
         SI::setOneSourcePositionNull(labelRef);
         SI::replaceExpression(strVal, labelRef, /*keepOldExp*/false);
       }
     }
   }
   labelRefs_.clear();

   for (const auto &kvp: labelDecls_) {
     SgStringVal* strVal = kvp.second;
     delete strVal;
   }
   labelDecls_.clear();

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
   SgEnumDeclaration* enum_decl = nullptr;

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
         // MATCHED ItemTypeDescription without StatusItemDescription
      }
      else if (traverse_StatusItemDescription(t_type, enum_decl, status_size)) {
         // MATCHED StatusItemDescription: must be handled differently than other ItemTypeDescriptions
         // because they require different arguments
         mlog[ERROR] << "UNIMPLEMENTED: FunctionHeading - StatusItemDescription";
         ROSE_ABORT();
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
      mlog[WARN] << "UNIMPLEMENTED: InlineDeclaration\n";

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
   } else if (traverse_EjectDirective(term)) {
      // MATCHED EjectDirective
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

   ATerm t_labels, t_stmt;
   std::vector<std::string> labels;
   std::vector<PosInfo> locations;

   if (ATmatch(term, "NullStatement()")) {
      // MATCHED NullStatement (SimpleStatement without a label)
   }

   else if (ATmatch(term, "SimpleStatement(<term>,<term>)", &t_labels, &t_stmt)) {
      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;

      if (traverse_AssignmentStatement(t_stmt, labels)) {
         // MATCHED AssignmentStatement
      }
      else if (traverse_NullStatement(t_stmt, labels)) {
         // MATCHED NullStatement
      }
      else if (traverse_NullBlockStatement(t_stmt)) {
         // MATCHED NullStatement
         if (labels.size() > 0) mlog[WARN] << "UNIMPLEMENTED labels: NullBlockStatement: labels.size() > 0\n";
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
      }
      else return ATfalse;
   }
   else return ATfalse;

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

   SgBasicBlock* block = nullptr;

   if (ATmatch(term, "CompoundStatement(<term>,<term>,<term>)", &t_labels, &t_stmt, &t_labels2)) {
      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;

   // Begin SageTreeBuilder
      sage_tree_builder.Enter(block, labels);
      setSourcePosition(block, term, false);

      if (traverse_StatementList(t_stmt)) {
         // MATCHED StatementList
      } else return ATfalse;

      if (traverse_LabelList(t_labels2, labels2, locations2)) {
         // MATCHED LabelList
      } else return ATfalse;

      // TODO: Move to Leave?
      sage_tree_builder.attachComments(block, getLocation(term), true);
   }
   else return ATfalse;

   ASSERT_not_null(block);

// End SageTreeBuilder
   sage_tree_builder.Leave(block, labels2);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_NullStatement(ATerm term, const std::vector<std::string> &labels)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_NullStatement: %s\n", ATwriteToString(term));
#endif
  ATerm t_labels;

  std::vector<std::string> localLabels;
  std::vector<PosInfo> locations;
  SgNullStatement* stmt{nullptr};

  if (ATmatch(term, "NullStatement()")) {
    if (labels.size() > 0) {
      localLabels = labels;
    }
  }
  else if (ATmatch(term, "LabeledNullStatement(<term>)", &t_labels)) {
    // The grammar is not optional for a NullStatement with a label, but
    // we have the grammar we have
    ASSERT_require(labels.size() == 0);
    if (traverse_LabelList(t_labels, localLabels, locations)) {
      // MATCHED LabelList
    } else return ATfalse;
  }
  else return ATfalse;

  sage_tree_builder.Enter(stmt);
  setSourcePosition(stmt, term);
  sage_tree_builder.Leave(stmt, localLabels);

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
      SI::appendStatement(null_block_stmt, SB::topScopeStack());

   // TODO - labels
   // stmt = convert_Labels(labels, locations, null_block_stmt);
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_LabelList(ATerm term, std::vector<std::string> &labels, std::vector<PosInfo> &locations)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_LabelList: %s\n", ATwriteToString(term));
#endif

   ATerm t_labels;
   char* label;

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

   return ATtrue;
}

//========================================================================================
// 4.1 ASSIGNMENT STATEMENTS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_AssignmentStatement(ATerm term, std::vector<std::string> &labels)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_AssignmentStatement: %s\n", ATwriteToString(term));
#endif
   ATerm t_vars, t_expr;
   SgExprStatement* assign_stmt{nullptr};

   if (ATmatch(term, "AssignmentStatement(<term>,<term>)", &t_vars, &t_expr)) {
      SgExpression* rhs{nullptr};
      std::vector<SgExpression*> vars;

      if (traverse_VariableList(t_vars, vars)) {
         // MATCHED VariableList
      } else return ATfalse;

      if (traverse_Formula(t_expr, rhs)) {
         // MATCHED Formula
      } else return ATfalse;

      // May need to be converted to a function call
      if (auto fref = isSgFunctionRefExp(rhs)) {
         auto params = SageBuilder::buildExprListExp_nfi();
         auto fcall = SageBuilder::buildFunctionCallExp(fref, params);
         setSourcePosition(fcall, t_expr);
         rhs = fcall;
      }

      if (rhs == nullptr) {
         mlog[WARN] << "UNIMPLEMENTED: AssignmentStatement "
                    << "- could be FunctionCall, or StatusConstant, or PointerLiteral, etc.\n";
         return ATtrue;
      }

   // Begin SageTreeBuilder
      sage_tree_builder.Enter(assign_stmt, rhs, vars);
      setSourcePosition(assign_stmt, term);

   } else return ATfalse;
   ASSERT_not_null(assign_stmt);

// End SageTreeBuilder
   sage_tree_builder.Leave(assign_stmt, labels);

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

   SgWhileStmt* while_stmt = nullptr;
   SgExpression* condition = nullptr;

   if (ATmatch(term, "WhileStatement(<term>,<term>,<term>)", &t_labels, &t_clause, &t_stmt)) {
      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;

      if (ATmatch(t_clause, "WhileClause(<term>)", &t_formula)) {
         if (traverse_Formula(t_formula, condition)) {
            // MATCHED Formula
         } else return ATfalse;
      }
      else return ATfalse;

   // Begin SageTreeBuilder
      sage_tree_builder.Enter(while_stmt, condition);
      setSourcePosition(while_stmt, term);

   // Match ControlledStatement -- this is the body
      if (traverse_Statement(t_stmt)) {
         // MATCHED Statement
      } else return ATfalse;
   }
   else return ATfalse;

// End SageTreeBuilder
   sage_tree_builder.Leave(while_stmt);

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

   SgExpression* var_ref = nullptr;
   SgExpression* init    = nullptr;
   SgExpression* phrase1 = nullptr;
   SgExpression* phrase2 = nullptr;

   int phrase1_enum = Jovial_ROSE_Translation::e_unknown;
   int phrase2_enum = Jovial_ROSE_Translation::e_unknown;
   SgJovialForThenStatement::loop_statement_type_enum loop_type_enum = SgJovialForThenStatement::e_unknown;

   SgExpression* while_expr = nullptr;
   SgExpression* by_or_then_expr = nullptr;
   SgJovialForThenStatement* for_stmt = nullptr;

   if (ATmatch(term, "ForStatement(<term>,<term>,<term>)", &t_labels, &t_clause, &t_stmt)) {
      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;

   // Begin SageTreeBuilder
      sage_tree_builder.Enter(for_stmt);
      setSourcePosition(for_stmt, term);

      // Control variable goes in the for statement scope, not in its body
      // The normal pattern of SageTreeBuilder is to push a statement scope
      // and then its body. Break that pattern here because the control
      // variable is seen after the for statement is constructed.
      SB::popScopeStack();  // for_stmt loop body

      if (traverse_ForClause(t_clause, var_ref, init, phrase1, phrase2, phrase1_enum, phrase2_enum)) {
         // MATCHED ForClause
      } else return ATfalse;

      ASSERT_not_null(var_ref);
      ASSERT_not_null(init);

      SgAssignOp* initialization = SageBuilder::buildAssignOp_nfi(var_ref, init);
      setSourcePosition(initialization, t_clause);

   // WHILE then optional BY or THEN (increment expression)
      if (phrase1_enum == e_while_phrase_expr) {
         while_expr = phrase1;
         by_or_then_expr = phrase2;
         if (phrase2_enum == e_by_phrase_expr) {
            loop_type_enum = SgJovialForThenStatement::e_for_while_by_stmt;
         }
         else if (phrase2_enum == e_then_phrase_expr) {
            loop_type_enum = SgJovialForThenStatement::e_for_while_then_stmt;
         }
         else if (phrase2_enum == Jovial_ROSE_Translation::e_unknown) {
            loop_type_enum = SgJovialForThenStatement::e_for_while_stmt;
         }
      }
   // BY (increment expression) then optional WHILE
      else if (phrase1_enum == e_by_phrase_expr) {
         while_expr = phrase2;
         by_or_then_expr = phrase1;
         loop_type_enum = SgJovialForThenStatement::e_for_by_while_stmt;
      }
   // THEN (increment expression) then optional WHILE
      else if (phrase1_enum == e_then_phrase_expr) {
         while_expr = phrase2;
         by_or_then_expr = phrase1;
         loop_type_enum = SgJovialForThenStatement::e_for_then_while_stmt;
      }
   // No WHILE, THEN, or BY expressions
      else {
         loop_type_enum = SgJovialForThenStatement::e_for_only_stmt;
      }
      ASSERT_require(loop_type_enum != SgJovialForThenStatement::e_unknown);
      ASSERT_not_null(for_stmt);

      for_stmt->set_initialization(initialization);
      for_stmt->set_while_expression(while_expr);
      for_stmt->set_by_or_then_expression(by_or_then_expr);
      for_stmt->set_loop_statement_type(loop_type_enum);

      if (initialization) initialization->set_parent(for_stmt);
      if (while_expr) while_expr->set_parent(for_stmt);
      if (by_or_then_expr) by_or_then_expr->set_parent(for_stmt);

      // Attach comments after ForClause and before body
      if (while_expr) {
        sage_tree_builder.attachComments(while_expr, getLocation(t_clause), /*at_end*/true);
      }
      sage_tree_builder.attachComments(for_stmt->get_loop_body(), getLocation(t_stmt));

      // Now the scope of the for loop body should be used
      SB::pushScopeStack(for_stmt->get_loop_body());

      // Match ControlledStatement (body of loop)
      if (traverse_Statement(t_stmt)) {
         // MATCHED Statement
      } else return ATfalse;
   }
   else return ATfalse;

// End SageTreeBuilder
   sage_tree_builder.Leave(for_stmt);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_ForClause(ATerm term, SgExpression* &var_ref, SgExpression* &init,
                                                                  SgExpression* &phrase1, SgExpression* &phrase2,
                                                                  int &phrase1_enum, int &phrase2_enum)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ForClause: %s\n", ATwriteToString(term));
#endif

   ATerm t_item, t_clause;
   char* var_name;

   init = nullptr;
   var_ref = nullptr;
   phrase1 = nullptr;
   phrase2 = nullptr;
   phrase1_enum = Jovial_ROSE_Translation::e_unknown;
   phrase2_enum = Jovial_ROSE_Translation::e_unknown;

   if (ATmatch(term, "ForClause(<term>,<term>)", &t_item, &t_clause)) {
      // MATCHED ForClause

      if (ATmatch(t_item, "<str>" , &var_name)) {
         // MATCHED ControlItem
         SgVarRefExp* typedVarRef{nullptr};
         sage_tree_builder.Enter(typedVarRef, var_name, true);
         sage_tree_builder.Leave(typedVarRef);

         var_ref = typedVarRef;
         setSourcePosition(var_ref, t_item);
      }
      else return ATfalse;

      if (traverse_ControlClause(t_clause, init, phrase1, phrase2, phrase1_enum, phrase2_enum)) {
         // MATCHED ControlClause
      } else return ATfalse;
   }
   else return ATfalse;

   ASSERT_not_null(var_ref);
   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_ControlClause(ATerm term, SgExpression* &initial_value,
                                                          SgExpression* &phrase1, SgExpression* &phrase2,
                                                          int &phrase1_enum, int &phrase2_enum)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ControlClause: %s\n", ATwriteToString(term));
#endif

   ATerm t_value, t_continuation;

   initial_value = nullptr;
   phrase1 = nullptr;
   phrase2 = nullptr;
   phrase1_enum = Jovial_ROSE_Translation::e_unknown;
   phrase2_enum = Jovial_ROSE_Translation::e_unknown;

   if (ATmatch(term, "ControlClause(<term>,<term>)", &t_value, &t_continuation)) {
      // MATCHED ControlClause
      if (traverse_Formula(t_value, initial_value)) {
         // MATCHED InitialValue
      } else return ATfalse;
      if (traverse_OptContinuation(t_continuation, phrase1, phrase2, phrase1_enum, phrase2_enum)) {
         // MATCHED OptContinuation
      } else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_OptContinuation(ATerm term, SgExpression* &phrase1, SgExpression* &phrase2,
                                                                        int &phrase_enum1, int &phrase_enum2)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptContinuation: %s\n", ATwriteToString(term));
#endif

   phrase1 = nullptr;
   phrase2 = nullptr;
   phrase_enum1 = Jovial_ROSE_Translation::e_unknown;
   phrase_enum2 = Jovial_ROSE_Translation::e_unknown;

   if (ATmatch(term, "no-continuation")) {
      // MATCHED no-continuation
   } else if (traverse_Continuation(term, phrase1, phrase2, phrase_enum1, phrase_enum2)) {
      // MATCHED Continuation
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_Continuation(ATerm term, SgExpression* &phrase1, SgExpression* &phrase2,
                                                                     int &phrase_enum_1, int &phrase_enum_2)
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

ATbool ATermToSageJovialTraversal::traverse_Phrase(ATerm term, SgExpression* &expr, int &phrase_enum)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Phrase: %s\n", ATwriteToString(term));
#endif

   ATerm t_formula;

   expr = nullptr;
   phrase_enum = Jovial_ROSE_Translation::e_unknown;

   if (ATmatch(term, "ByPhrase(<term>)", &t_formula)) {
      // MATCHED ByPhrase
      if (traverse_Formula(t_formula, expr)){
         // MATCHED Formula
         phrase_enum = Jovial_ROSE_Translation::e_by_phrase_expr;
      } else return ATfalse;
   } else if (ATmatch(term, "ThenPhrase(<term>)", &t_formula)) {
      // MATCHED ThenPhrase
      if (traverse_Formula(t_formula, expr)){
         // MATCHED Formula
         phrase_enum = Jovial_ROSE_Translation::e_then_phrase_expr;
      } else return ATfalse;
   } else if (ATmatch(term, "WhilePhrase(<term>)", &t_formula)) {
      // MATCHED WhilePhrase
      if (traverse_Formula(t_formula, expr)){
         // BooleanFormula generalized to Formula
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

   ATerm t_labels, t_if, t_cond, t_else, t_true, t_false;
   std::vector<std::string> labels{};
   std::vector<PosInfo> locations{};
   std::vector<RB::Token> before_if_comments{};
   SgExpression* conditional{nullptr};
   SgBasicBlock* true_body{nullptr};
   SgBasicBlock* false_body{nullptr};
   SgIfStmt* if_stmt{nullptr};

   bool hasElseClause;
   if (ATmatch(term, "IfSimpleStatement(<term>,<term>,<term>,<term>)", &t_labels,&t_if,&t_cond,&t_true)) {
     // MATCHED IfSimpleStatement
     hasElseClause = false;
   }
   else if (ATmatch(term, "IfElseStatement(<term>,<term>,<term>,<term>,<term>)", &t_labels,&t_if,&t_cond,&t_true,&t_else)) {
     // MATCHED IfElseStatement
     hasElseClause = true;
   }
   else return ATfalse;

   // Save any comments preceding IfStatement
   sage_tree_builder.consumePrecedingComments(before_if_comments, getLocation(term));

   if (traverse_LabelList(t_labels, labels, locations)) {
     // MATCHED LabelList
   } else return ATfalse;

   // t_if is the keyword 'IF'

   if (traverse_Formula(t_cond, conditional)) {
     // BooleanFormula generalized to Formula
   }
   else return ATfalse;

   // Create a basic block and push it on the scope stack so there is
   // a place for statements.
   true_body = SB::buildBasicBlock_nfi(SB::topScopeStack());
   setSourcePosition(true_body, t_true, /*attach_comments*/true);
   SageBuilder::pushScopeStack(true_body);

   if (traverse_Statement(t_true)) {
     // MATCHED Statement for the true body
   } else return ATfalse;

   // Attach comments above and at end of line of true_body
   sage_tree_builder.popScopeStack(/*attach_comments*/true);

   if (hasElseClause && ATmatch(t_else, "ElseClause(<term>)", &t_false)) {
     // Save and then attach comments before ELSE clause
     std::vector<RB::Token> before_else_comments{};
     sage_tree_builder.consumePrecedingComments(before_else_comments, getLocation(t_else));
     sage_tree_builder.attachComments(true_body, before_else_comments, /*at_end*/true);

     // There is a false body
     false_body = SB::buildBasicBlock_nfi(SB::topScopeStack());
     setSourcePosition(false_body, t_false, /*attach_comments*/true);
     SageBuilder::pushScopeStack(false_body);

     if (traverse_Statement(t_false)) {
       // MATCHED Statement for the false body
     } else return ATfalse;

     // Attach comments above and at end of line of true_body
     sage_tree_builder.popScopeStack(/*attach_comments*/true);
   }

// Begin SageTreeBuilder
   sage_tree_builder.Enter(if_stmt, conditional, true_body, false_body, before_if_comments);
   setSourcePosition(if_stmt, term, /*attach_comments*/false);

// End SageTreeBuilder
   sage_tree_builder.Leave(if_stmt, labels);

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

   RB::SourcePositionPair sources;

   SgExpression* selector = nullptr;
   SgSwitchStatement* switch_stmt = nullptr;

   if (ATmatch(term, "CaseStatement(<term>,<term>,<term>,<term>)", &t_labels, &t_formula, &t_case_body, &t_labels2)) {
      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;

      if (traverse_Formula(t_formula, selector)) {
        // MATCHED Formula
      } else return ATfalse;

   // Begin SageTreeBuilder
      sage_tree_builder.Enter(switch_stmt, selector, sources);
      setSourcePosition(switch_stmt, term);

      if (traverse_CaseBody(t_case_body)) {
        // MATCHED CaseBody
      } else return ATfalse;

      if (traverse_LabelList(t_labels2, labels2, locations2)) {
         // MATCHED LabelList
         ASSERT_require(locations2.size() == 0);  // TODO
      } else return ATfalse;
   }
   else return ATfalse;

// End SageTreeBuilder
   sage_tree_builder.Leave(switch_stmt);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_CaseBody(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CaseBody: %s\n", ATwriteToString(term));
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

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_CaseAlternative(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CaseAlternative: %s\n", ATwriteToString(term));
#endif

   ATerm t_case_index_group, t_stmt, t_fall_thru;
   bool fall_thru = false;

   SgCaseOptionStmt* case_option_stmt = nullptr;
   SgExprListExp* case_index_group = nullptr;

   if (ATmatch(term, "CaseAlternative(<term>,<term>,<term>)", &t_case_index_group, &t_stmt, &t_fall_thru)) {

      if (traverse_CaseIndexGroup(t_case_index_group, case_index_group)) {
         // MATCHED CaseIndexGroup
      } else return ATfalse;

   // Begin SageTreeBuilder
      sage_tree_builder.Enter(case_option_stmt, case_index_group);
      setSourcePosition(case_option_stmt, term);

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
      mlog[WARN] << "UNIMPLEMENTED: CaseAlternative - probably StatusConstant\n";
      ASSERT_not_null(case_index_group);
   }

   ASSERT_not_null(case_option_stmt);
   case_option_stmt->set_has_fall_through(fall_thru);

// End SageTreeBuilder
   sage_tree_builder.Leave(case_option_stmt);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_DefaultOption(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_DefaultOption: %s\n", ATwriteToString(term));
#endif

   ATerm t_stmt, t_fall_thru;
   SgDefaultOptionStmt* stmt{nullptr};
   bool fallThru{false};

   if (ATmatch(term, "DefaultOptionNoBlock()")) {
      sage_tree_builder.Enter(stmt);
      setSourcePosition(stmt, term);
   }
   else if (ATmatch(term, "DefaultOption(<term>,<term>)", &t_stmt, &t_fall_thru)) {
      sage_tree_builder.Enter(stmt);
      setSourcePosition(stmt, term);

      if (traverse_Statement(t_stmt)) {
         // MATCHED Statement
      } else return ATfalse;

      if (ATmatch(t_fall_thru, "no-fall-thru()")) {
         // MATCHED no-fall-thru
         fallThru = false;
      } else if (ATmatch(t_fall_thru, "FALLTHRU()")) {
         // MATCHED FALLTHRU
         fallThru = true;
      } else return ATfalse;
   }
   else return ATfalse;

   ASSERT_not_null(stmt);
   stmt->set_has_fall_through(fallThru);

// End SageTreeBuilder
   sage_tree_builder.Leave(stmt);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_CaseIndexGroup(ATerm term, SgExprListExp* &index_group)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CaseIndexGroup: %s\n", ATwriteToString(term));
#endif

   ATerm t_case_index;
   SgExpression* case_index;

   index_group = nullptr;

   if (ATmatch(term, "CaseIndexGroup(<term>)", &t_case_index)) {

      index_group = SageBuilder::buildExprListExp_nfi();
      setSourcePosition(index_group, term);

      ATermList tail = (ATermList) ATmake("<term>", t_case_index);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         case_index = nullptr;
         if (traverse_CaseIndex(head, case_index)) {
            // MATCHED CaseIndex
            ASSERT_not_null(case_index);
            index_group->get_expressions().push_back(case_index);
            case_index->set_parent(index_group);
         }
         else return ATfalse;
      }
   } else return ATfalse;

   ASSERT_not_null(index_group);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_CaseIndex(ATerm term, SgExpression* &case_index)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_CaseIndex: %s\n", ATwriteToString(term));
#endif

   ATerm t_formula1, t_formula2;
   SgExpression* lower_bound = nullptr;
   SgExpression* upper_bound = nullptr;

   case_index = nullptr;

// This case is needed to traverse CompileTimeFormula -> CaseIndex
   if (ATmatch(term, "CaseIndex(<term>)", &t_formula1)) {
      if (traverse_Formula(t_formula1, case_index)) {
         // MATCHED Formula
      } else return ATfalse;
   }

// This case is needed to traverse LowerBound : UpperBound -> CaseIndex
   else if (ATmatch(term, "CaseIndex(<term>,<term>)", &t_formula1, &t_formula2)) {
      if (traverse_Formula(t_formula1, lower_bound)) {
         // MATCHED Formula
      } else return ATfalse;
      if (traverse_Formula(t_formula2, upper_bound)) {
         // MATCHED Formula
      } else return ATfalse;

   // Perhaps SgRangeExp should be used instead for the stride and then won't need to insert literal "1"
      SgExpression* stride = SB::buildIntVal_nfi(std::string("1"));

      case_index = SB::buildSubscriptExpression_nfi(lower_bound, upper_bound, stride);
   }
   else return ATfalse;

   if (case_index == nullptr) {
      mlog[WARN] << "UNIMPLEMENTED: CaseIndex = nullptr, probably StatusConstant in lower_bound or upper_bound\n";
   }

   ASSERT_not_null(case_index);
   setSourcePosition(case_index, term);

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

   ATerm t_labels, t_proc_name, t_param_list, t_abort_phrase, t_abort_name;
   std::vector<std::string> labels;
   std::vector<PosInfo> locations;
   std::string proc_name;
   std::string abort_stmt_name;
   SgExprListExp* param_list;
   SgExprStatement* call_stmt = nullptr;

   if (ATmatch(term, "ProcedureCallStatement(<term>,<term>,<term>,<term>)", &t_labels, &t_proc_name, &t_param_list, &t_abort_phrase)) {
      // MATCHED UsedDefinedProcedureCall -> ProcedureCallStatement
      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;

      if (traverse_Name(t_proc_name, proc_name)) {
         // MATCHED Name
      } else return ATfalse;

      param_list = SageBuilder::buildExprListExp_nfi();

      if (traverse_ActualParameterList(t_param_list, param_list)) {
         // MATCHED ActualParameterList
      } else return ATfalse;

      if (ATmatch(t_abort_phrase, "no-abort-phrase()")) {
         // No AbortPhrase
      } else if (ATmatch(t_abort_phrase, "AbortPhrase(<term>)", &t_abort_name)) {
         if (traverse_Name(t_abort_name, abort_stmt_name)) {
            // MATCHED AbortStatementName
            mlog[WARN] << "UNIMPLEMENTED: ProcedureCallStatement AbortPhrase not handled";
            ROSE_ABORT();
         } else return ATfalse;
      } else return ATfalse;
   }
   else return ATfalse;

   // Begin SageTreeBuilder
   sage_tree_builder.Enter(call_stmt, proc_name, param_list, abort_stmt_name);
   setSourcePosition(call_stmt, term);

   // End SageTreeBuilder
   sage_tree_builder.Leave(call_stmt, labels);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_ActualParameterList(ATerm term, SgExprListExp* param_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ActualParameterList: %s\n", ATwriteToString(term));
#endif

   ATerm t_param_list, t_output;
   SgExpression* param = nullptr;

   if (ATmatch(term, "no-actual-parameter-list()")) {
      // MATCHED no-actual-parameter-list
   }
   else if (ATmatch(term, "ActualParameterList(<term>,<term>)" , &t_param_list, &t_output)) {
      ATermList tail = (ATermList) ATmake("<term>", t_param_list);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_Formula(head, param)) {
            // MATCHED Formula
         } else return ATfalse;

         ASSERT_not_null(param);
         param_list->get_expressions().push_back(param);
         param->set_parent(param_list);
      }

      if (traverse_ActualOutputParameters(t_output, param_list)) {
         // MATCHED ActualOutputParameters
      } else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_ActualOutputParameters(ATerm term, SgExprListExp* param_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ActualOutputParameters: %s\n", ATwriteToString(term));
#endif

   ATerm t_output_list, t_param;
   SgExpression* param;

   if (ATmatch(term, "no-actual-output-parameters()")) {
      // MATCHED no-actual-output-parameters
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
         } else if (traverse_UserDefinedFunctionCall(head, param)) {
            // MATCHED UserDefinedFunctionCall
         } else return ATfalse;

         ASSERT_not_null(param);
         param_list->get_expressions().push_back(param);
         param->set_parent(param_list);
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

   SgReturnStmt* returnStmt = nullptr;

   if (ATmatch(term, "ReturnStatement(<term>)", &t_labels)) {
      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;
   }
   else return ATfalse;

   // Begin SageTreeBuilder
   sage_tree_builder.Enter(returnStmt, boost::none);
   setSourcePosition(returnStmt, term);

   // End SageTreeBuilder
   sage_tree_builder.Leave(returnStmt, labels);

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

   SgGotoStatement* gotoStmt = nullptr;

   if (ATmatch(term, "GotoStatement(<term>,<term>)", &t_labels, &t_name)) {
       if (traverse_LabelList(t_labels, labels, locations)) {
          // MATCHED LabelList
       } else return ATfalse;

       if (traverse_Name(t_name, name)) {
          // MATCHED Name
       } else return ATfalse;
   }
   else return ATfalse;

   // Begin SageTreeBuilder
   sage_tree_builder.Enter(gotoStmt, name);
   setSourcePosition(gotoStmt, term);

   // End SageTreeBuilder
   sage_tree_builder.Leave(gotoStmt, labels);

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

   SgProcessControlStatement* exit_stmt = nullptr;

   if (ATmatch(term, "ExitStatement(<term>)", &t_labels)) {
      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;
   }
   else return ATfalse;

   // Begin SageTreeBuilder
   sage_tree_builder.Enter(exit_stmt, std::string("exit"), boost::none, boost::none);
   setSourcePosition(exit_stmt, term);

   // End SageTreeBuilder
   sage_tree_builder.Leave(exit_stmt, labels);

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

   SgExpression* stop_code{nullptr};
   SgProcessControlStatement* stop_stmt{nullptr};
   boost::optional<SgExpression*> opt_code{boost::none};

   if (ATmatch(term, "StopStatement(<term>,<term>)", &t_labels, &t_stop_code)) {
      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;

      if (ATmatch(t_stop_code, "no-integer-formula()")) {
         // no stop code
      }
      else if (traverse_Formula(t_stop_code, stop_code)) {
         // MATCHED Formula
         opt_code = boost::optional<SgExpression*>(stop_code);
      } else return ATfalse;
   }
   else return ATfalse;

   // Begin SageTreeBuilder
   sage_tree_builder.Enter(stop_stmt, std::string("stop"), opt_code, boost::none);
   setSourcePosition(stop_stmt, term);

   // End SageTreeBuilder
   sage_tree_builder.Leave(stop_stmt, labels);

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

   SgProcessControlStatement* abort_stmt = nullptr;

   if (ATmatch(term, "AbortStatement(<term>)", &t_labels)) {
      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;
   }
   else return ATfalse;

   // Begin SageTreeBuilder
   sage_tree_builder.Enter(abort_stmt, std::string("abort"), boost::none, boost::none);
   setSourcePosition(abort_stmt, term);

   // End SageTreeBuilder
   sage_tree_builder.Leave(abort_stmt, labels);

   return ATtrue;
}

//========================================================================================
// 5.0 FORMULAS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_Formula(ATerm term, SgExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_Formula: %s\n", ATwriteToString(term));
#endif

   if (traverse_Literal(term, expr)) {
      // MATCHED Literal
   }
   else if (traverse_Variable(term, expr)) {
      // MATCHED Variable
   }
   else if (traverse_Parens(term, expr)) {
      // MATCHED Parens
   }
   else if (traverse_UnaryExpression(term, expr)) {
      // MATCHED UnaryExpression
   }
   else if (traverse_BinaryExpression(term, expr)) {
      // MATCHED BinaryExpression
   }
   else if (traverse_NumericConversion(term, expr)) {
      // MATCHED NumericConversion
   }
   else if (traverse_BitOrByteFormula(term, expr)) {
      // BooleanFormula generalized to Formula
      // BitConversion still part of BitFormula
   }
   else if (traverse_CharacterFormula(term, expr)) {
      // MATCHED CharacterFormula
   }
   else if (traverse_StatusFormula(term, expr)) {
      // MATCHED StatusFormula
   }
   else if (traverse_PointerFormula(term, expr)) {
      // MATCHED PointerFormula
   } else return ATfalse;

   //  TableFormula                -> Formula

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_Literal(ATerm term, SgExpression* &expr)
{
  char* literal = nullptr;
  std::string str;

  if (ATmatch(term, "IntVal(<str>)", &literal)) {
    expr = SageBuilder::buildIntVal_nfi(std::string(literal));
  }
  else if (traverse_FixedOrFloatingLiteral(term, expr)) {
    // MATCHED fixed or float literal
  }
  else if (traverse_BooleanLiteral(term, expr)) {
    // MATCHED boolean literal (True/False)
  }
  else if (traverse_BitLiteral(term, expr)) {
    // MATCHED bit literal  (BeadSize 'B' "'" Bead+ "'")
  }
  else if (traverse_PointerLiteral(term, expr)) {
    // MATCHED pointer literal
  }
  else if (traverse_CharacterLiteral(term, str)) {
    // MATCHED character literal
     expr = SageBuilder::buildStringVal(str);
     setSourcePosition(expr, term);
  }
  else return ATfalse;

  ASSERT_not_null(expr);
  setSourcePosition(expr, term);

  return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_Parens(ATerm term, SgExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_Parens: %s\n", ATwriteToString(term));
#endif

  ATerm t_formula;
  expr = nullptr;

  if (ATmatch(term, "Parens(<term>)", &t_formula)) {
     if (traverse_Formula(t_formula, expr)) {
        //  MATCHED Formula
     } else return ATfalse;

  } else return ATfalse;

  ASSERT_not_null(expr);
  expr->set_need_paren(true); // Parentheses are not currently unparsing from this for SgIntVal (and maybe others)

  return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_UnaryExpression(ATerm term, SgExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_UnaryExpression: %s\n", ATwriteToString(term));
#endif

  ATerm t_formula;
  SgExpression* formula = nullptr;
  LanguageTranslation::ExpressionKind op = LT::e_none;

  if      (ATmatch(term, "UnaryPlus(<term>)",   &t_formula)) op = LT::e_operator_unary_plus;
  else if (ATmatch(term, "UnaryMinus(<term>)",  &t_formula)) op = LT::e_operator_unary_minus;
  else if (ATmatch(term, "NOT(<term>)",         &t_formula)) op = LT::e_operator_unary_not;
  else return ATfalse;

  if (traverse_Formula(t_formula, formula)) {
     // MATCHED Formula
  } else return ATfalse;

  ASSERT_not_null(formula);

  switch (op) {
    case e_operator_unary_plus:
      expr = new SgUnaryAddOp(formula, nullptr);
      break;
    case e_operator_unary_minus:
      expr = SageBuilder::buildMinusOp_nfi(formula, SgUnaryOp::prefix);
      break;
    case e_operator_unary_not:
      expr = SageBuilder::buildNotOp(formula);
      break;
    default:
      mlog[ERROR] << "unknown unary operator in traverse_UnaryExpression() \n";
      ROSE_ABORT();
  }

  ASSERT_not_null(expr);
  setSourcePosition(expr, term);

  return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_BinaryExpression(ATerm term, SgExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_BinaryExpression: %s\n", ATwriteToString(term));
#endif

  ATerm t_lhs, t_rhs, t_oper;
  LanguageTranslation::ExpressionKind op = LT::e_none;
  SgExpression* lhs = nullptr;
  SgExpression* rhs = nullptr;

  // Arithmetic operators
  if      (ATmatch(term, "Plus(<term>,<term>)",  &t_lhs, &t_rhs)) op = LT::e_operator_add;
  else if (ATmatch(term, "Minus(<term>,<term>)", &t_lhs, &t_rhs)) op = LT::e_operator_subtract;
  else if (ATmatch(term, "Mult(<term>,<term>)",  &t_lhs, &t_rhs)) op = LT::e_operator_multiply;
  else if (ATmatch(term, "Div(<term>,<term>)",   &t_lhs, &t_rhs)) op = LT::e_operator_divide;
  else if (ATmatch(term, "Power(<term>,<term>)", &t_lhs, &t_rhs)) op = LT::e_operator_exponentiate;

  // Relational operators
  else if (ATmatch(term, "LT(<term>,<term>)", &t_lhs, &t_rhs)) op = LT::e_operator_less_than;
  else if (ATmatch(term, "GT(<term>,<term>)", &t_lhs, &t_rhs)) op = LT::e_operator_greater_than;
  else if (ATmatch(term, "LE(<term>,<term>)", &t_lhs, &t_rhs)) op = LT::e_operator_less_or_equal;
  else if (ATmatch(term, "GE(<term>,<term>)", &t_lhs, &t_rhs)) op = LT::e_operator_greater_or_equal;
  else if (ATmatch(term, "EQ(<term>,<term>)", &t_lhs, &t_rhs)) op = LT::e_operator_equality;
  else if (ATmatch(term, "NE(<term>,<term>)", &t_lhs, &t_rhs)) op = LT::e_operator_not_equal;

  // Logical operators
  else if (ATmatch(term, "AND(<term>,<term>,<term>)", &t_lhs, &t_oper, &t_rhs)) op = LT::e_operator_and;
  else if (ATmatch(term,  "OR(<term>,<term>,<term>)", &t_lhs, &t_oper, &t_rhs)) op = LT::e_operator_or;
  else if (ATmatch(term, "XOR(<term>,<term>,<term>)", &t_lhs, &t_oper, &t_rhs)) op = LT::e_operator_xor;
  else if (ATmatch(term, "EQV(<term>,<term>,<term>)", &t_lhs, &t_oper, &t_rhs)) op = LT::e_operator_equiv;

  else if (ATmatch(term, "Mod(<term>,<term>,<term>)", &t_lhs, &t_oper, &t_rhs)) op = LT::e_operator_mod;

  else return ATfalse;

  if (traverse_Formula(t_lhs, lhs)) {
    // MATCHED lhs
  } else return ATfalse;
  if (traverse_Formula(t_rhs, rhs)) {
    // MATCHED rhs
  } else return ATfalse;

  ASSERT_not_null(lhs);
  ASSERT_not_null(rhs);

  switch (op) {
    // Arithmetic operators
    case e_operator_add:
      expr = SB::buildAddOp_nfi(lhs, rhs);
      break;
    case e_operator_subtract:
      expr = SB::buildSubtractOp_nfi(lhs, rhs);
      break;
    case e_operator_multiply:
      expr = SB::buildMultiplyOp_nfi(lhs, rhs);
      break;
    case e_operator_divide:
      expr = SB::buildDivideOp_nfi(lhs, rhs);
      break;
    case e_operator_exponentiate:
      expr = SB::buildExponentiationOp_nfi(lhs, rhs);
      break;

    // Relational operators
    case e_operator_less_than:
      expr = SB::buildLessThanOp_nfi(lhs, rhs);
      break;
    case e_operator_greater_than:
      expr = SB::buildGreaterThanOp_nfi(lhs, rhs);
      break;
    case e_operator_less_or_equal:
      expr = SB::buildLessOrEqualOp_nfi(lhs, rhs);
      break;
    case e_operator_greater_or_equal:
      expr = SB::buildGreaterOrEqualOp_nfi(lhs, rhs);
      break;
    case e_operator_equality:
      expr = SB::buildEqualityOp_nfi(lhs, rhs);
      break;
    case e_operator_not_equal:
      expr = SB::buildNotEqualOp_nfi(lhs, rhs);
      break;

    // Logical operators
    case e_operator_and:
      expr = SB::buildBitAndOp_nfi(lhs, rhs);
      break;
    case e_operator_or:
      expr = SB::buildBitOrOp_nfi(lhs, rhs);
      break;
    case e_operator_xor:
      expr = SB::buildBitXorOp_nfi(lhs, rhs);
      break;
    case e_operator_equiv:
      expr = SB::buildBinaryExpression_nfi<SgBitEqvOp>(lhs, rhs);
      break;

    case e_operator_mod:
      expr = SB::buildModOp_nfi(lhs, rhs);
      break;

    default:
      mlog[ERROR] << "unknown binary operator in traverse_BinaryExpression() \n";
      ROSE_ABORT();
  }

   ASSERT_not_null(expr);
   setSourcePosition(expr, term);

   return ATtrue;
}

//========================================================================================
// 5.1 NUMERIC FORMULAS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_NumericConversion(ATerm term, SgExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_NumericPrimary: %s\n", ATwriteToString(term));
#endif

   ATerm t_formula, t_conversion;

   SgExpression* cast_formula = nullptr;
   SgType* conv_type = nullptr;

   expr = nullptr;

   if (ATmatch(term, "NumericConversion(<term>,<term>)", &t_conversion, &t_formula)) {

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

      ASSERT_not_null(conv_type);
      ASSERT_not_null(cast_formula);
      //                                                      cast_enum? default? ctype? static? dynamic?
      SgCastExp* cast_expr = SB::buildCastExp_nfi(cast_formula, conv_type, SgCastExp::e_default);
      ASSERT_not_null(cast_expr);
      setSourcePosition(cast_expr, term);
      expr = cast_expr;
   }
   else return ATfalse;

   ASSERT_not_null(expr);

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

//========================================================================================
// 5.2 BIT FORMULAS
//----------------------------------------------------------------------------------------

// BitFormula has been removed from grammar as part of Formula generalization. But
// BitVariableFormula and ByteVariableFormula use it to get to FunctionCall (for different reasons).
ATbool ATermToSageJovialTraversal::traverse_BitOrByteFormula(ATerm term, SgExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BitOrByteFormula: %s\n", ATwriteToString(term));
#endif
   ATerm t_operand;

   expr = nullptr;

   // ByteVariableForumula uses LogicalOperand to get to FunctionCall (convoluted generalized Formula)
   if (traverse_LogicalOperand(term, expr)) {
      // MATCHED LogicalOperand
   }
   else if (ATmatch(term, "BitVariableFormula(<term>)", &t_operand)) {
      if (traverse_Variable(t_operand, expr)) {
         // MATCHED Variable
      }
      else return ATfalse;
   }
   else return ATfalse;

   ASSERT_not_null(expr);
   setSourcePosition(expr, term);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_LogicalOperand(ATerm term, SgExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_LogicalOperand: %s\n", ATwriteToString(term));
#endif

   // Path used by ByteFunctionVariable to get to function call? -> TableItem (I think essentially)
   if (traverse_BitPrimary(term, expr)) {
      // MATCHED BitPrimary
   }
   else return ATfalse;

   ASSERT_not_null(expr);
   setSourcePosition(expr, term);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_BitPrimary(ATerm term, SgExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BitPrimary: %s\n", ATwriteToString(term));
#endif

   ATerm t_conv_type, t_formula;

   expr = nullptr;

   if (ATmatch(term, "BitPrimaryConversion(<term>,<term>)", &t_conv_type, &t_formula)) {
      //          expr = SB::buildNullExpression_nfi();
      //          return ATtrue;
      //        }

      // MATCHED BitPrimaryConversion
      SgExpression* cast_formula = nullptr;
      SgType* conv_type = nullptr;

      if (traverse_BitConversion(t_conv_type, conv_type)) {
         // MATCHED BitConversion
      } else return ATfalse;

      if (traverse_Formula(t_formula, cast_formula)) {
         // MATCHED Formula
      } else return ATfalse;

      ASSERT_not_null(conv_type);
      ASSERT_not_null(cast_formula);

      //                                      cast_enum? default? ctype? static? dynamic?
      SgCastExp* cast_expr = SB::buildCastExp(cast_formula, conv_type, SgCastExp::e_default);
      ASSERT_not_null(cast_expr);
      expr = cast_expr;
   }
   // Path used by ByteFunctionVariable to get to function call -> TableItem (I think essentially)
   else if (traverse_FunctionCall(term, expr)) {
      // FunctionCall      -> BitFunctionCall
      // BitFunctionCall   -> BitPrimary (no cons)
   }
   else return ATfalse;

   // TODO: create else if for following (is this still the case, testing should inform)
   // NamedBitConstant       -> BitPrimary {cons("NamedBitConstant")} (rejected in grammar)

   ASSERT_not_null(expr);
   setSourcePosition(expr, term);

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
      } else return ATfalse;

      ASSERT_not_null(conv_type);
      ASSERT_not_null(cast_formula);
      //                                      cast_enum? default? ctype? static? dynamic?
      SgCastExp* cast_expr = SB::buildCastExp(cast_formula, conv_type, SgCastExp::e_default);
      ASSERT_not_null(cast_expr);
      setSourcePosition(cast_expr, term);
      expr = cast_expr;
   } else return ATfalse;

   ASSERT_not_null(expr);

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

      ASSERT_not_null(conv_type);
      ASSERT_not_null(cast_formula);
      //                                      cast_enum? default? ctype? static? dynamic?
      SgCastExp* cast_expr = SB::buildCastExp(cast_formula, conv_type, SgCastExp::e_default);
      ASSERT_not_null(cast_expr);
      setSourcePosition(cast_expr, term);
      expr = cast_expr;
   } else return ATfalse;

   ASSERT_not_null(expr);

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

   SgType* convType{nullptr};
   SgExpression* castFormula{nullptr};

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
      if (traverse_PointerConversion(t_conv_type, convType)) {
         // MATCHED PointerConversion
      } else return ATfalse;

      if (traverse_Formula(t_formula, castFormula)) {
         // MATCHED Formula
      } else return ATfalse;

      if (convType == nullptr) {
         // PointerConversion isa PointerConversionP term, get type from the formula
         if (auto cast = isSgCastExp(castFormula)) {
            convType = cast->get_type();
         }
         else if (isSgJovialBitVal(castFormula)) {
            convType = SB::buildJovialBitType(/*size*/nullptr);
         }
         else {
           // Don't have the type, punt and use nullptr
           convType = SB::buildNullptrType();
         }
      }
      ASSERT_not_null(convType);
      ASSERT_not_null(castFormula);

      //                                       cast_enum? default? ctype? static? dynamic?
      SgCastExp* castExpr = SB::buildCastExp(castFormula, convType, SgCastExp::e_default);
      ASSERT_not_null(castExpr);
      setSourcePosition(castExpr, term);
      expr = castExpr;
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
   char* name{nullptr};

   // Look for a variable (or could be a function call because of ambiguities)
   if (ATmatch(term, "<str>", &name)) {
     SgSymbol* symbol = SI::lookupSymbolInParentScopes(name);

     var = nullptr;

     // First look for a reference expression (coming from LOC function argument, need not technically be a variable)
     if (isSgLabelSymbol(symbol)) {
       var = SageBuilder::buildLabelRefExp(isSgLabelSymbol(symbol));
       // TODO: need _nfi
       SI::setOneSourcePositionNull(var);
     }
     else if (isSgFunctionSymbol(symbol)) {
       var = SageBuilder::buildFunctionRefExp_nfi(isSgFunctionSymbol(symbol));
     }
     else if (isSgClassSymbol(symbol) || isSgTypedefSymbol(symbol) || isSgEnumSymbol(symbol)) {
       var = SageBuilder::buildTypeExpression(symbol->get_type());
     }
     else if (isSgVariableSymbol(symbol) || symbol == nullptr) {
       var = sage_tree_builder.buildVarRefExp_nfi(std::string(name));
     }
     else {
       // TODO: look for a define and replace with define/macro name
     }
     ASSERT_not_null(var);
     setSourcePosition(var, term);

   // Look for other possibilities
   //
   } else if (traverse_Dereference(term, var)) {
     // MATCHED Dereference -> ItemDereference -> Item -> NamedVariable
   } else if (traverse_TableDereference(term, var)) {
     // MATCHED TableDereference
   } else if (traverse_TableItem(term, var)) {
     // MATCHED TableItem
   } else if (traverse_BitFunctionVariable(term, var)) {
     // MATCHED BitFunctionVariable
   } else if (traverse_ByteFunctionVariable(term, var)) {
     // MATCHED ByteFunctionVariable
   } else if (traverse_RepFunctionVariable(term, var)) {
     // MATCHED RepFunctionVariable
   } else if (traverse_NumericMachineParameter(term, var)) {
     // MATCHED NumericMachineParameter
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_VariableLVal(ATerm term, SgExpression* &var)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_VariableLVal: %s\n", ATwriteToString(term));
#endif

   ATerm t_var;
   var = nullptr;

   if (ATmatch(term, "VariableLVal(<term>)" , &t_var)) {
      if (traverse_Variable(t_var, var)) {
         // MATCHED Variable
      } else return ATfalse;
   }

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_VariableList(ATerm term, std::vector<SgExpression*> &vars)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_VariableList: %s\n", ATwriteToString(term));
#endif

   ATerm t_labels;
   SgExpression* var;

// The variables in the list are lvalues in an assignment statement
//
   if (ATmatch(term, "VariableList(<term>)" , &t_labels)) {
      ATermList tail = (ATermList) ATmake("<term>", t_labels);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);

         if (traverse_VariableLVal(head, var)) {
            vars.push_back(var);
         } else return ATfalse;
      }

   // Because of ambiguities in the grammar the lvalue member must be set here, it can't be
   // set in traverse_ByteFunctionVariable or traverse_BitFunctionVariable (perhaps in BitFunctionVariable)
      SgFunctionCallExp* func_call = isSgFunctionCallExp(var);
      if (func_call) {
        func_call->set_lvalue(true);
      }
   }
   else return ATfalse;

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

   SgVarRefExp* varRef{nullptr};
   SgExprListExp* arraySubscripts{nullptr};
   SgExpression* derefVar{nullptr};

   var = nullptr;

   if (ATmatch(term, "TableItem(<term>,<term>,<term>)" , &t_name, &t_subscript, &t_tblderef)) {
   // Shall have a name
      if (ATmatch(t_name, "<str>", &name)) {
        // MATCHED name
      } else return ATfalse;

   // May have a subscript(s)
      if (traverse_Subscript(t_subscript, subscript)) {
         if (subscript.size() > 0) {
            arraySubscripts = SageBuilder::buildExprListExp_nfi();
            setSourcePosition(arraySubscripts, t_subscript);

            for (SgExpression* expr : subscript) {
               arraySubscripts->get_expressions().push_back(expr);
               expr->set_parent(arraySubscripts);
            }
         }
      }

   // Grammar could be better designed (Dereference is a TableDereference here (may have changed?))
      if (traverse_TableDereference(t_tblderef, derefVar)) {
         // MATCHED TableDereference
      }
      else if (traverse_Dereference(t_tblderef, derefVar, false)) {
         // MATCHED Dereference
      }
      else return ATfalse;
   }
   else return ATfalse;

   // There may not be a symbol for the variable in this scope (name maybe from external CompoolModule)
   auto scope{SageBuilder::topScopeStack()};
   if (nullptr == SI::lookupVariableSymbolInParentScopes(name, scope)) {
     // Look for symbol in the scope of the dereferenced variable
     if (auto type = isSgPointerType(derefVar->get_type())) {
       if (auto baseType = isSgJovialTableType(type->get_base_type())) {
         if (auto decl = baseType->get_declaration()) {
           // Don't actually need the symbol here as the scope of the deref'd
           // variable will be used in the SB::buildVarRefExp() call below.
           scope = decl->get_scope();
         }
       }
     }
   }

   varRef = SageBuilder::buildVarRefExp(name, scope);
   ASSERT_not_null(varRef);
   setSourcePosition(varRef, t_name);

   if (isSgTypeUnknown(varRef->get_type())) {
     mlog[WARN] << "Type of a TableItem is unknown, may cause trouble downstream\n";
   }

   if (arraySubscripts) {
      var = SageBuilder::buildPntrArrRefExp_nfi(varRef, arraySubscripts);
   }
   else {
      var = varRef;
   }

   if (derefVar) {
      var = SageBuilder::buildAtOp_nfi(var, derefVar);
   }
   setSourcePosition(var, term);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_Subscript(ATerm term, std::vector<SgExpression*> & indexes)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Subscript: %s\n", ATwriteToString(term));
#endif

   ATerm t_index;
   SgExpression* index;

   if (ATmatch(term, "no-subscript")) {
      // MATCHED no-subscript
   } else if (ATmatch(term, "Subscript(<term>)" , &t_index)) {
      ATermList tail = (ATermList) ATmake("<term>", t_index);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_Formula(head, index)) {
            // MATCHED Index
            indexes.push_back(index);
         } else return ATfalse;
      }
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_TableDereference(ATerm term, SgExpression* &tableArrayRef)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_TableDereference: %s\n", ATwriteToString(term));
#endif

   ATerm t_deref, t_subscript, t_name_or_formula;
   char* name;
   std::vector<SgExpression*> subscript;
   SgExprListExp* subscripts{nullptr};
   SgExpression* deref{nullptr};

   tableArrayRef = nullptr;

   if (ATmatch(term, "no-table-dereference")) {
      // MATCHED no-table-dereference
      return ATtrue;
   }
   else if (ATmatch(term, "TableDereference(<term>,<term>)", &t_deref, &t_subscript)) {
      // There shall be a subscript
      if (traverse_Subscript(t_subscript, subscript)) {
         subscripts = SageBuilder::buildExprListExp_nfi();
         setSourcePosition(subscripts, t_subscript);
         for (SgExpression* expr : subscript) {
            subscripts->get_expressions().push_back(expr);
            expr->set_parent(subscripts);
         }
      } else return ATfalse;

      // Handle the dereferencing part
      if (ATmatch(t_deref, "Dereference(<term>)", &t_name_or_formula)) {
         // MATCHED Dereference
         if (traverse_PointerFormula(t_name_or_formula, deref)) {
            // MATCHED PointerFormula
            deref = SageBuilder::buildPointerDerefExp(deref);
            setSourcePosition(deref, t_name_or_formula);
            tableArrayRef = SageBuilder::buildPntrArrRefExp_nfi(deref, subscripts);
         }
         else if (ATmatch(t_name_or_formula, "<str>", &name)) {
            // The right-hand side of a dereference (SgAtOp) may be a function call
            if (auto funSymbol = SI::lookupFunctionSymbolInParentScopes(name, SB::topScopeStack())) {
              tableArrayRef = SageBuilder::buildFunctionCallExp(funSymbol, subscripts);
            }
            else {
              auto varRef = SageBuilder::buildVarRefExp(name, SB::topScopeStack());
              setSourcePosition(varRef, t_name_or_formula);
              deref = SageBuilder::buildPointerDerefExp(varRef);
              setSourcePosition(deref, t_name_or_formula);
              tableArrayRef = SageBuilder::buildPntrArrRefExp_nfi(deref, subscripts);
            }
         }
         else return ATfalse;
      }
      else {
         // Grammar has been changed (for GL-394), this path unlikely (not preferred in grammar)
         mlog[WARN] << "TableDereference with no Dereference to name, not attempting to recover\n";
         ROSE_ABORT();
      }
   } else return ATfalse;

   ASSERT_not_null(tableArrayRef);
   setSourcePosition(tableArrayRef, term);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_Dereference(ATerm term, SgExpression* &expr, bool build_ptr_ref)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Dereference: %s\n", ATwriteToString(term));
#endif
   ATerm t_deref;
   char* name{nullptr};
   SgExpression* formula{nullptr};

   expr = nullptr;

   if (ATmatch(term, "Dereference(<term>)", &t_deref)) {
      if (ATmatch(t_deref, "<str>", &name)) {
         // MATCHED PointerItemName
         formula = SB::buildVarRefExp(name, SB::topScopeStack());
         setSourcePosition(formula, term);
      } else if (traverse_Formula(t_deref, formula)) {
         // MATCHED PointerFormula through Formula
      } else return ATfalse;
   }
   else return ATfalse;

   ASSERT_not_null(formula);

   if (build_ptr_ref) {
     expr = SB::buildPointerDerefExp(formula);
   }
   else {
     expr = formula;
   }

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_BitFunctionVariable(ATerm term, SgExpression* &func_call)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BitFunctionVariable: %s\n", ATwriteToString(term));
#endif

   ATerm t_bitvar, t_var, t_fbit, t_nbit, t_first_bit, t_length;

   SgExpression* variable = nullptr;
   SgExpression* first_bit = nullptr;
   SgExpression* length = nullptr;
   SgType* return_type = nullptr;

   func_call = nullptr;

   // Grammar (this is an lvalue call expression, I don't think it can be an rvalue (BitFunction)
   //  'BIT' '(' Variable ',' Fbit ',' Nbit ')' -> BitFunctionVariable {cons("BitFunctionVariable"), prefer}
   //  'BIT' '(' Formula ','  Fbit ',' Nbit ')' -> BitFunctionVariable {cons("BitFunctionVariable")}
   //
   if (ATmatch(term, "BitFunctionVariable(<term>,<term>,<term>)", &t_bitvar, &t_fbit, &t_nbit)) {
     // Is it possible generate a Variable now or is Formula enough (try running all tests to see)
     // Formula may cover all (but only looking at rvalue at moment)
      if (ATmatch(t_bitvar, "Variable(<term>)", &t_var)) {
        if (traverse_Variable(t_var, variable)) {
         // MATCHED Variable
         // MATCHED BitVariable? -> Variable
        } else return ATfalse;
      } else if (traverse_Formula(t_bitvar, variable)) {
         // BooleanFormula generalized to Formula // checkout language spec if generalized (probably)
      } else return ATfalse;

      if (ATmatch(t_fbit, "Fbit(<term>)", &t_first_bit)) {
         if (traverse_Formula(t_first_bit, first_bit)) {
            // MATCHED Formula
         } else return ATfalse;
      } else return ATfalse;

      if (ATmatch(t_nbit, "Nbit(<term>)", &t_length)) {
         if (traverse_Formula(t_length, length)) {
            // MATCHED Formula
         } else return ATfalse;
      } else return ATfalse;
   }
   else return ATfalse;

   ASSERT_not_null(first_bit);
   ASSERT_not_null(length);

   // build the parameter list
   SgExprListExp* params = SageBuilder::buildExprListExp_nfi();
   params->append_expression(variable);
   params->append_expression(first_bit);
   params->append_expression(length);

   // get the variable type
   SgVarRefExp* var_ref = isSgVarRefExp(variable);
   if (var_ref) {
     // The return type is the type of the variable
     SgVariableSymbol* var_symbol = var_ref->get_symbol();
     ASSERT_not_null(var_symbol);
     return_type = var_symbol->get_type();
   }
   else {
     // Note: _assume_ that the return type is an intrinsic bit type
     SgExpression* size = nullptr;
     return_type = SB::buildJovialBitType(size);
   }
   ASSERT_not_null(return_type);

   func_call = SB::buildFunctionCallExp("BIT", return_type, params, SB::topScopeStack());
   ASSERT_not_null(func_call);
   setSourcePosition(func_call, term);

   func_call->set_lvalue(true);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_ByteFunctionVariable(ATerm term, SgExpression* &func_call)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ByteFunctionVariable: %s\n", ATwriteToString(term));
#endif

   ATerm t_variable, t_var, t_fbyte, t_nbyte;

   SgExpression* variable = nullptr;
   SgExpression* first_byte = nullptr;
   SgExpression* length = nullptr;
   SgType* return_type = nullptr;

   func_call = nullptr;

   // Grammar (this may be an lvalue call expression! or an rvalue depending on ambiguous context)
   //  'BYTE' '(' Variable ','
   //          Fbyte ',' Nbyte ')' -> ByteFunctionVariable  {cons("ByteFunctionVariable"), prefer}
   //  'BYTE' '(' Formula ','
   //          Fbyte ',' Nbyte ')' -> ByteFunctionVariable  {cons("ByteFunctionVariable")}


   // Grammar (this may be an lvalue call expression! or an rvalue depending on ambiguous context)
   //  'BYTE' '(' CharacterFormula/Variable ',' FirstByte ',' Length ')' -> ByteFunctionVariable
   //
   if (ATmatch(term, "ByteFunctionVariable(<term>,<term>,<term>)", &t_variable, &t_fbyte, &t_nbyte)) {
      if (ATmatch(t_variable, "Variable(<term>)", &t_var)) {
         if (traverse_Variable(t_var, variable)) {
            // MATCHED Variable
         } else return ATfalse;
      } else if (traverse_Formula(t_variable, variable)) {
         // BooleanFormula generalized to Formula (grammar changed from what)
         // MATCHED Formula
      } else return ATfalse;

      if (traverse_Formula(t_fbyte, first_byte)) {
            // MATCHED Formula
      } else return ATfalse;

      if (traverse_Formula(t_nbyte, length)) {
        // MATCHED Formula
      } else return ATfalse;
   }
   else return ATfalse;

   ASSERT_not_null(variable);
   ASSERT_not_null(first_byte);
   ASSERT_not_null(length);

   // build the parameter list
   SgExprListExp* params = SageBuilder::buildExprListExp_nfi();
   params->append_expression(variable);
   params->append_expression(first_byte);
   params->append_expression(length);

   // Create the return type.
   SgVarRefExp* var_ref = isSgVarRefExp(variable);

   if (var_ref) {
      SgVariableSymbol* var_symbol = var_ref->get_symbol();
      ASSERT_not_null(var_symbol);
      return_type = var_symbol->get_type();
   }
   else {
      // Create the return type. The length expression for the return_type can't be shared
      // so we need to either make a new one (if isSgIntVal) or a copy (if isSgVarRefExp).
      //
      SgExpression* new_length = nullptr;
      if (isSgIntVal(length)) {
        SgIntVal* length_val = isSgIntVal(length);
        new_length = SageBuilder::buildIntVal(length_val->get_value());
      }
      else {
        new_length = isSgVarRefExp(SI::deepCopyNode(length));
      }
      ASSERT_not_null(new_length);

      return_type = SageBuilder::buildStringType(new_length);
   }
   ASSERT_not_null(return_type);

   func_call = SB::buildFunctionCallExp("BYTE", return_type, params, SB::topScopeStack());
   ASSERT_not_null(func_call);
   setSourcePosition(func_call, term);

   func_call->set_lvalue(true);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_RepFunctionVariable(ATerm term, SgExpression* &func_call)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_RepFunctionVariable: %s\n", ATwriteToString(term));
#endif

   ATerm t_rep, t_name, t_subscript;
   char* name;
   std::vector<SgExpression*> subscript;
   SgExpression* var_ref = nullptr;
   bool has_subscript = false;

   func_call = nullptr;

   if (ATmatch(term, "RepFunctionVariable(<term>,<term>,<term>)", &t_rep, &t_name, &t_subscript)) {
     // MATCHED RepFunctionVariable with subscripts
     has_subscript = true;
   }
   else if (ATmatch(term, "RepFunctionVariable(<term>,<term>)", &t_rep, &t_name)) {
     // MATCHED RepFunctionVariable without subscripts
   }
   else return ATfalse;

   if (ATmatch(t_rep, "RepConversion()")) {
     // MATCHED "REP" grammar keyword
   } else return ATfalse;

   if (ATmatch(t_name, "<str>", &name)) {
     // MATCHED NamedVariable
     var_ref = SB::buildVarRefExp(name, SB::topScopeStack());
     setSourcePosition(var_ref, t_name);
   } else return ATfalse;

   if (has_subscript) {
     if (traverse_Subscript(t_subscript, subscript)) {
       SgExprListExp* array_subscripts = SB::buildExprListExp_nfi();
       setSourcePosition(array_subscripts, t_subscript);
       for (SgExpression* expr : subscript) {
          array_subscripts->get_expressions().push_back(expr);
          expr->set_parent(array_subscripts);
       }
       var_ref = SB::buildPntrArrRefExp_nfi(var_ref, array_subscripts);
       setSourcePosition(var_ref, term); // source position too broad because of 'REP'
     } else return ATfalse;
   }
   ASSERT_not_null(var_ref);

   // build the parameter list
   SgExprListExp* params = SB::buildExprListExp_nfi();
   params->append_expression(var_ref);

   // Create the return type. The language specifies the result is a bit string and
   // has an example assuming BITSINWORD is 16. Because WORD is mentioned it seems
   // that long is a good choice for a return type.
   SgType* result_type = SB::buildLongType();
   ASSERT_not_null(result_type);

   func_call = SB::buildFunctionCallExp("REP", result_type, params, SB::topScopeStack());
   ASSERT_not_null(func_call);
   setSourcePosition(func_call, term);

   func_call->set_lvalue(true);

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
   var = nullptr;

   if (ATmatch(term, "ControlLetter(<str>)" , &letter)) {
      SgVarRefExp* var_ref = nullptr;
      sage_tree_builder.Enter(var_ref, std::string(letter));
      sage_tree_builder.Leave(var_ref);
      ASSERT_not_null(var_ref);

      var = var_ref;
   }
   else return ATfalse;

   //  ConstantItemName            -> NamedConstant         {prefer}  %% ambiguous with ConstantTableName
   //  ConstantTableName           -> NamedConstant         {cons("ConstantTableName")}
   //  ConstantTableName Subscript -> NamedConstant         {cons("NamedConstant")}

   ASSERT_not_null(var);

   return ATtrue;
}

//========================================================================================
// 6.3 FUNCTION CALLS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_FunctionCall(ATerm term, SgExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FunctionCall: %s\n", ATwriteToString(term));
#endif

   SgFunctionCallExp* func_call = nullptr;

// UserDefinedFunctionCall is ambiguous with type conversions (casts) in Jovial,
// so a SgExpression is returned rather than a SgFunctionCallExp.
   if (traverse_UserDefinedFunctionCall(term, expr)) {
      // MATCHED UserDefinedFunctionCall
   }
   else if (traverse_IntrinsicFunctionCall(term, func_call)) {
      expr = func_call;
   }
   else return ATfalse;

   //   MachineSpecificFunctionCall -> FunctionCall

   ASSERT_not_null(expr);
   setSourcePosition(expr, term);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_UserDefinedFunctionCall(ATerm term, SgExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_UserDefinedFunctionCall: %s\n", ATwriteToString(term));
#endif

   ATerm t_name, t_param_list;
   std::string name;
   SgExprListExp* expr_list = nullptr;

   expr = nullptr;

   if (ATmatch(term, "UserDefinedFunctionCall(<term>,<term>)", &t_name, &t_param_list)) {
      if (traverse_Name(t_name, name)) {
         // MATCHED FunctionName
      } else return ATfalse;

   // The grammar (UserDefinedFunctionCall) treats this as a parameter list
   // but in general (because of ambiguities) it is just a general expression list
      expr_list = SageBuilder::buildExprListExp_nfi();

      if (traverse_ActualParameterList(t_param_list, expr_list)) {
         // MATCHED ActualParameterList
      } else return ATfalse;
   }
   else return ATfalse;

   // Several different options due to ambiguous grammar:
   //  1. Table reference argument of a function call not yet declared,
   //     e.g. LOC(table_var(5))
   //  2. Function call
   //     a. With a function symbol
   //     b. Without a function symbol (must build nondefining declaration)
   //  3. Type conversion
   //     a. General conversion (isSgTypedefSymbol)
   //     b. StatusConversion   (isSgEnumSymbol)
   //  4. Variable
   //     a. Table reference
   //     b. Table initialization replication operator

// The symbol is used to disambiguate the design of the grammar.
// For Jovial the symbol should be present, unfortuately this is not true for Fortran
//
   SgSymbol* symbol = SI::lookupSymbolInParentScopes(name, SB::topScopeStack());

// No symbol exists yet (perhaps a table variable declared later)
//
   if (!symbol && sage_tree_builder.isInitializationContext()) {
      // This will add a var ref to forward_var_refs_
      SgVarRefExp* var_ref = sage_tree_builder.buildVarRefExp_nfi(name);
      expr = SB::buildPntrArrRefExp_nfi(var_ref, expr_list); // table/array reference
   }

// Look for function call
//
   else if (isSgFunctionSymbol(symbol) || symbol == nullptr) {
      SgFunctionCallExp* func_call = nullptr;
      sage_tree_builder.Enter(func_call, name, expr_list);
      sage_tree_builder.Leave(func_call);
      expr = func_call;
   }

// Look for type conversion
//
   else if (isSgTypedefSymbol(symbol)) {
      SgCastExp* cast_expr = nullptr;

      ASSERT_require(expr_list->get_expressions().size() == 1);
      SgExpression* cast_operand = expr_list->get_expressions()[0];
      ASSERT_not_null(cast_operand);

      sage_tree_builder.Enter(cast_expr, name, cast_operand);
      sage_tree_builder.Leave(cast_expr);
      expr = cast_expr;
   }

// Look for StatusConversion
//
   else if (isSgEnumSymbol(symbol)) {
      SgCastExp* cast_expr = nullptr;

      ASSERT_require(expr_list->get_expressions().size() == 1);
      SgExpression* cast_operand = expr_list->get_expressions()[0];
      ASSERT_not_null(cast_operand);

      sage_tree_builder.Enter(cast_expr, name, cast_operand);
      sage_tree_builder.Leave(cast_expr);
      expr = cast_expr;
   }

// Look for table variable, table member, or table initialization replication operator
//
   else if (isSgVariableSymbol(symbol)) {
   // First look for a table type
      SgJovialTableType* table_type = nullptr;
      SgInitializedName* init_name = nullptr;
      SgExprListExp* dim_info = nullptr;
      SgVariableSymbol* var_sym = isSgVariableSymbol(symbol);

      if (var_sym) init_name = isSgInitializedName(var_sym->get_declaration());
      if (init_name) {
        if (SgModifierType* mod_type = isSgModifierType(init_name->get_type())) {
          table_type = isSgJovialTableType(mod_type->get_base_type());
        } else {
          table_type = isSgJovialTableType(init_name->get_type());
        }
      }
      if (!table_type) {
         // Variable could be a member of a table
         SgVariableDeclaration* var_decl = nullptr;
         SgClassDefinition* class_def = nullptr;
         SgJovialTableStatement* table_decl = nullptr;

         if (init_name) var_decl = isSgVariableDeclaration(init_name->get_parent());
         if (var_decl) class_def = isSgClassDefinition(var_decl->get_parent());
         if (class_def) {
           table_decl = isSgJovialTableStatement(class_def->get_parent());
           // Make sure this isn't a Block in disguise
           // TODO: make node SgJovialBlockStatement (or SgJovialBlockType?)
           if (table_decl && (table_decl->get_class_type() == SgClassDeclaration::e_jovial_block)) {
             table_decl = nullptr; // This a block not a table so don't allow table_type==true
           }
         }
         if (table_decl) table_type = isSgJovialTableType(table_decl->get_type());
      }

      if (table_type) {
         dim_info = table_type->get_dim_info();
         // Make sure the rank of the table is same as # of params in "function call"
         if (dim_info && (dim_info->get_expressions().size() == expr_list->get_expressions().size())) {
            SgVarRefExp* var_ref = nullptr;
            sage_tree_builder.Enter(var_ref, name);
            sage_tree_builder.Leave(var_ref);
            expr = SB::buildPntrArrRefExp_nfi(var_ref, expr_list);
         }
         else {
            mlog[ERROR] << "UserDefinedFunctionCall - variable reference ambiguous "
                        << "with table reference (and rank is incorrect) for " << name;
            ROSE_ABORT();
         }
      }
      else {
        // Since there is a variable and the type isn't a table, this must be
        // a replication operator (yes?).
        if (expr_list->get_expressions().size() != 1) {
           // It is not clear that a replication operator can't have expression size other than 1.
           mlog[ERROR] << "UserDefinedFunctionCall - variable reference ambiguous "
                       << "with replication operator and # expressions != 1 for variable " << name;
           ROSE_ABORT();
        }

        SgReplicationOp* rep_op = nullptr;
        SgExpression* value = expr_list->get_expressions()[0];
        ASSERT_not_null(value);

        sage_tree_builder.Enter(rep_op, name, value);
        sage_tree_builder.Leave(rep_op);
        expr = rep_op;
      }
   }
   ASSERT_not_null(expr);
   setSourcePosition(expr, term);

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
   else if (traverse_ShiftFunction(term, func_call)) {
      // MATCHED ShiftFunction
   }
   else if (traverse_AbsFunction(term, func_call)) {
      // MATCHED AbsFunction
   }
   else if (traverse_BitFunction(term, func_call)) {
      // MATCHED BitFunction
   }
   else if (traverse_SignFunction(term, func_call)) {
      // MATCHED SizeFunction
   }
   else if (traverse_SizeFunction(term, func_call)) {
      // MATCHED SizeFunction
   }
   else if (traverse_BoundsFunction(term, func_call)) {
      // MATCHED BoundsFunction
   }
   else if (traverse_NwdsenFunction(term, func_call)) {
      // MATCHED NwdsenFunction
   }
   else if (traverse_NentFunction(term, func_call)) {
      // MATCHED NentFunction
   }

   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 6.3.1 LOC FUNCTIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_LocFunction(ATerm term, SgFunctionCallExp* &funcCall)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_LocFunction: %s\n", ATwriteToString(term));
#endif

   ATerm t_argument;
   char* name;
   std::string locArgStr{};
   SgExpression* locArgExpr{nullptr};
   SgType* returnType{nullptr};

   funcCall = nullptr;

   if (ATmatch(term, "LocFunction(<term>)", &t_argument)) {
      if (traverse_Name(t_argument, locArgStr)) {
         locArgExpr = SB::buildVarRefExp(locArgStr, SB::topScopeStack());
         ASSERT_not_null(locArgExpr);
      }
      else if (ATmatch(t_argument, "<str>" , &name)) {
         if (!SI::lookupSymbolInParentScopes(name)) {
            // No symbol but perhaps a label statement already exists
            auto labels = sage_tree_builder.getLabels();
            if (labels.find(name) != labels.end()) {
               // The SgStringVal will later need to be converted to SgLabelRefExp (when we have a symbol)
               auto stringVal = SB::buildStringVal_nfi(name);
               labelRefs_.insert(std::make_pair(name, stringVal));
               locArgExpr = stringVal;
            }
            // Or perhaps there is a label DEF/REF declaration statement
            else {
              auto kvp = labelDecls_.find(name);
              if (kvp != labelDecls_.end()) {
                // TODO: duplication somehow with above, simplify
                // The SgStringVal will later need to be converted to SgLabelRefExp (when we have a symbol)
                auto stringVal = SB::buildStringVal_nfi(name);
                labelRefs_.insert(std::make_pair(name, stringVal));
                locArgExpr = stringVal;
              }
            }
         }
      }

      // Check for more complicated arguments
      if (!locArgExpr) {
         if (traverse_Variable(t_argument, locArgExpr)) {
            // MATCHED NamedVariable -> Variable
         }
         else if (traverse_UserDefinedFunctionCall(t_argument, locArgExpr)) {
            // MATCHED UserDefinedFunctionCall for when LocFunction argument is a TableItem
         }
         else return ATfalse;
      }
   }
   else return ATfalse;

   ASSERT_not_null(locArgExpr);

   // build the parameter list
   SgExprListExp* params = SageBuilder::buildExprListExp_nfi();
   params->append_expression(locArgExpr);

   // If the LOC arg is just a string, the function type should be void*
   if (isSgStringVal(locArgExpr)) {
      returnType = SB::buildNullptrType();
   }
   else {
      returnType = locArgExpr->get_type();
   }
   ASSERT_not_null(returnType);

   funcCall = SB::buildFunctionCallExp("LOC", returnType, params, SB::topScopeStack());
   ASSERT_not_null(funcCall);
   setSourcePosition(funcCall, term);

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
   SgExpression * next_arg, * increment;
   SgType* return_type = nullptr;

   func_call = nullptr;

   if (ATmatch(term, "NextFunction(<term>, <term>)", &t_argument, &t_increment)) {
      if (traverse_Formula(t_argument, next_arg)) {
         // NextArg will most likely be a variable of a PointerType or a StatusType
         // Formula will catch all the variables of either type here
         // If formula is a StatusConstant, which is probably never used,
         // this should still work
         ASSERT_not_null(next_arg);
         return_type = next_arg->get_type();
         ASSERT_not_null(return_type);
      } else return ATfalse;

      if (traverse_Formula(t_increment, increment)) {
         // MATCHED Formula
      } else return ATfalse;
   } else return ATfalse;

   // build the parameter list
   SgExprListExp* params = SageBuilder::buildExprListExp_nfi();
   params->append_expression(next_arg);
   params->append_expression(increment);

   func_call = SB::buildFunctionCallExp("NEXT", return_type, params, SB::topScopeStack());
   ASSERT_not_null(func_call);
   setSourcePosition(func_call, term);

   return ATtrue;
}

//========================================================================================
// 6.3.3 BIT FUNCTION
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_BitFunction(ATerm term, SgFunctionCallExp* &func_call)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BitFunction: %s\n", ATwriteToString(term));
#endif

   ATerm t_formula, t_fbit, t_nbit, t_fbit_formula, t_length;
   SgExpression* bit_formula, * first_bit, * length;
   SgType* return_type;

   func_call = nullptr;

   if (ATmatch(term, "BitFunction(<term>, <term>,<term>)", &t_formula, &t_fbit, &t_nbit)) {
      bit_formula = nullptr;
      first_bit = nullptr;
      length = nullptr;
      return_type = nullptr;

      if (traverse_Formula(t_formula, bit_formula)) {
         // BooleanFormula generalized to Formula
         // The return type is the same as the type of the Formula argument
         return_type = bit_formula->get_type();
      } else return ATfalse;

      if (ATmatch(t_fbit, "Fbit(<term>)", &t_fbit_formula)) {
         if (traverse_Formula(t_fbit_formula, first_bit)) {
            // MATCHED Formula
         } else return ATfalse;
      } else return ATfalse;

      if (ATmatch(t_nbit, "Nbit(<term>)", &t_length)) {
         if (traverse_Formula(t_length, length)) {
            // MATCHED Formula
         } else return ATfalse;
      } else return ATfalse;
   }
   else return ATfalse;

   ASSERT_not_null(bit_formula);
   ASSERT_not_null(first_bit);
   ASSERT_not_null(length);
   ASSERT_not_null(return_type);

   // build the parameter list
   SgExprListExp* params = SageBuilder::buildExprListExp_nfi();
   params->append_expression(bit_formula);
   params->append_expression(first_bit);
   params->append_expression(length);

   func_call = SB::buildFunctionCallExp("BIT", return_type, params, SB::topScopeStack());
   ASSERT_not_null(func_call);
   setSourcePosition(func_call, term);

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
   SgExpression * character_formula, * first_byte, * length;

   func_call = nullptr;

   // Grammar
   //
   //  'BYTE' '(' Formula
   //    ',' Fbyte ',' Nbyte ')'   -> ByteFunction         {cons("ByteFunction")}
#if 0
   //  'BYTE' '(' RepFunctionVariable
   //    ',' Fbyte ',' Nbyte ')'   -> ByteFunction         {cons("ByteFunction"), prefer}
#endif

   if (ATmatch(term, "ByteFunction(<term>, <term>,<term>)", &t_formula, &t_fbyte, &t_nbyte)) {
      if (traverse_Formula(t_formula, character_formula)) {
         // MATCHED Formula -> Will either be CharacterFormula or CharacterLiteral through Formula
      } else return ATfalse;

      if (traverse_Formula(t_fbyte, first_byte)) {
         // MATCHED Formula
      } else return ATfalse;

      if (traverse_Formula(t_nbyte, length)) {
         // MATCHED Formula
      } else return ATfalse;
   }
   else return ATfalse;

   // build the parameter list
   SgExprListExp* params = SageBuilder::buildExprListExp_nfi();
   params->append_expression(character_formula);
   params->append_expression(first_byte);
   params->append_expression(length);

   // Create the return type. The length expression for the return_type can't be shared
   // so we need to either make a new one (if isSgIntVal) or a copy otherwise.
   //
   SgExpression* new_length = nullptr;
   if (isSgIntVal(length)) {
     SgIntVal* length_val = isSgIntVal(length);
     new_length = SageBuilder::buildIntVal(length_val->get_value());
   }
   else {
     new_length = isSgExpression(SI::deepCopyNode(length));
   }
   ASSERT_not_null(new_length);

   SgType* return_type = SB::buildStringType(new_length);

   func_call = SB::buildFunctionCallExp("BYTE", return_type, params, SB::topScopeStack());
   ASSERT_not_null(func_call);
   setSourcePosition(func_call, term);

   return ATtrue;
}

//========================================================================================
// 6.3.5 SHIFT FUNCTIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_ShiftFunction(ATerm term, SgFunctionCallExp* &func_call)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ShiftFunction: %s\n", ATwriteToString(term));
#endif

   ATerm t_direction, t_formula, t_count;
   SgExpression * formula, * shift_count;
   std::string func_name;
   SgType* return_type = nullptr;

   func_call = nullptr;

   if (ATmatch(term, "ShiftFunction(<term>, <term>,<term>)", &t_direction, &t_formula, &t_count)) {
      if (ATmatch(t_direction, "SHIFTL")) {
         // MATCHED ShiftDirection Left
         func_name = "SHIFTL";
      }
      else if (ATmatch(t_direction, "SHIFTR")) {
         // MATCHED ShiftDirection Right
         func_name = "SHIFTR";
      } else return ATfalse;

      if (traverse_Formula(t_formula, formula)) {
         // BooleanFormula generalized to Formula
         // The return type is the same as the type of the Formula argument
         return_type = formula->get_type();
         ASSERT_not_null(return_type);
      } else return ATfalse;

      if (traverse_Formula(t_count, shift_count)) {
         // MATCHED Formula
      } else return ATfalse;
   } else return ATfalse;

   // build the parameter list
   SgExprListExp* params = SB::buildExprListExp_nfi();
   params->append_expression(formula);
   params->append_expression(shift_count);

   func_call = SB::buildFunctionCallExp(func_name, return_type, params, SB::topScopeStack());
   ASSERT_not_null(func_call);
   setSourcePosition(func_call, term);

   return ATtrue;
}

//========================================================================================
// 6.3.6 ABS FUNCTIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_AbsFunction(ATerm term, SgFunctionCallExp* &func_call)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_AbsFunction: %s\n", ATwriteToString(term));
#endif

   ATerm t_argument;
   SgExpression* argument;
   SgType* return_type = nullptr;

   func_call = nullptr;

   if (ATmatch(term, "AbsFunction(<term>)", &t_argument)) {
      if (traverse_Formula(t_argument, argument)) {
         // MATCHED Formula

         // The return type is the same as the type of the argument
         return_type = argument->get_type();
         ASSERT_not_null(return_type);
      } else return ATfalse;

   } else return ATfalse;

   // build the parameter list
   SgExprListExp* params = SB::buildExprListExp_nfi();
   params->append_expression(argument);

   func_call = SB::buildFunctionCallExp("ABS", return_type, params, SB::topScopeStack());
   ASSERT_not_null(func_call);
   setSourcePosition(func_call, term);

   return ATtrue;
}

//========================================================================================
// 6.3.7 SIGN FUNCTION
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_SignFunction(ATerm term, SgFunctionCallExp* &func_call)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SignFunction: %s\n", ATwriteToString(term));
#endif

   ATerm t_formula;

   func_call = nullptr;

   if (ATmatch(term, "SignFunction(<term>)", &t_formula)) {
      mlog[ERROR] << "UNIMPLEMENTED: SignFunction";
      ROSE_ABORT();
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 6.3.8 SIZE FUNCTIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_SizeFunction(ATerm term, SgFunctionCallExp* &func_call)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SizeFunction: %s\n", ATwriteToString(term));
#endif

   ATerm t_formula;
   SgExpression* size_argument;
   std::string func_name;

   func_call = nullptr;

   if (ATmatch(term, "SizeFunction(BITSIZE(),<term>)", &t_formula)) {
      func_name = "BITSIZE";
   }
   else if (ATmatch(term, "SizeFunction(BYTESIZE(),<term>)", &t_formula)) {
      func_name = "BYTESIZE";
   }
   else if (ATmatch(term, "SizeFunction(WORDSIZE(),<term>)", &t_formula)) {
      func_name = "WORDSIZE";
   }
   else return ATfalse;

   // get the function argument
   if (traverse_Formula(t_formula, size_argument)) {
     // MATCHED Formula
   } else return ATfalse;

   // build the parameter list
   SgExprListExp* params = SB::buildExprListExp_nfi();
   params->append_expression(size_argument);

   SgType* return_type = SB::buildUnsignedIntType();

   func_call = SB::buildFunctionCallExp(func_name, return_type, params, SB::topScopeStack());
   ASSERT_not_null(func_call);
   setSourcePosition(func_call, term);

   return ATtrue;
}

//========================================================================================
// 6.3.9 BOUNDS FUNCTIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_BoundsFunction(ATerm term, SgFunctionCallExp* &func_call)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BoundsFunction: %s\n", ATwriteToString(term));
#endif

   ATerm t_which, t_table_arg, t_dim;
   std::string function_name, table_or_type_name;
   SgExpression* table_arg = nullptr;
   SgExpression* dim_number = nullptr;

   func_call = nullptr;

   if (ATmatch(term, "BoundsFunction(<term>,<term>,<term>)", &t_which, &t_table_arg, &t_dim)) {
      if (ATmatch(t_which, "LBOUND()")) {
         function_name = "LBOUND";
      }
      else if (ATmatch(t_which, "UBOUND()")) {
         function_name = "UBOUND";
      } else return ATfalse;

      if (traverse_Name(t_table_arg, table_or_type_name)) {
        // MATCHED TableName
      } else return ATfalse;

      if (traverse_Formula(t_dim, dim_number)) {
         // MATCHED Formula
      } else return ATfalse;

   } else return ATfalse;

   // Find symbol and table name or table type name
   SgSymbol* symbol = SI::lookupSymbolInParentScopes(table_or_type_name, SB::topScopeStack());
   if (!symbol) {
      // could be anonymous type with variable declaration not seen yet
      std::string anon_type_name = mangleAnonymousName(table_or_type_name);
      symbol = SI::lookupSymbolInParentScopes(anon_type_name, SB::topScopeStack());
      // Not done yet, this is not the correct path
      ROSE_ABORT();
   }
   ASSERT_not_null(symbol);

   SgType* type = symbol->get_type();
   SgModifierType* mod_type = isSgModifierType(type);
   if (mod_type) {
     // unpack modifier wrapper (used for CONSTANT, for example)
     type = mod_type->get_base_type();
   }

   SgJovialTableType* table_type = isSgJovialTableType(type);
   ASSERT_not_null(table_type);

   switch (symbol->variantT())
      {
      case V_SgClassSymbol:
         {
            table_arg = SB::buildTypeExpression(table_type);
            break;
         }
      case V_SgVariableSymbol:
         {
            table_arg = SB::buildVarRefExp_nfi(isSgVariableSymbol(symbol));
            break;
         }
      default: ROSE_ABORT();
      }

   ASSERT_not_null(dim_number);
   ASSERT_not_null(table_arg);
   setSourcePosition(table_arg, t_table_arg);

   // build the parameter list
   SgExprListExp* params = SB::buildExprListExp_nfi();
   params->append_expression(table_arg);
   params->append_expression(dim_number);

//TODO: Return type may be a status type depending on the declaration of the table
   SgType* return_type = SB::buildSignedIntType();

   func_call = SB::buildFunctionCallExp(function_name, return_type, params, SB::topScopeStack());
   ASSERT_not_null(func_call);
   setSourcePosition(func_call, term);

   return ATtrue;
}

//========================================================================================
// 6.3.10 NWDSEN FUNCTIONS
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_NwdsenFunction(ATerm term, SgFunctionCallExp* &func_call)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_NwdsenFunction: %s\n", ATwriteToString(term));
#endif

   ATerm t_argument;
   std::string name;
   SgExpression* table_arg = nullptr;

   func_call = nullptr;

   if (ATmatch(term, "NwdsenFunction(<term>)", &t_argument)) {
      if (traverse_Name(t_argument, name)) {
         // MATCHED TableName or TableTypeName
      } else return ATfalse;
   } else return ATfalse;

   // Find symbol and jovial table type
   SgSymbol* symbol = SI::lookupSymbolInParentScopes(name, SB::topScopeStack());
   ASSERT_not_null(symbol);

   SgJovialTableType* table_type = isSgJovialTableType(symbol->get_type());
   if (table_type == nullptr) {
     if (SgModifierType* mod_type = isSgModifierType(symbol->get_type())) {
       table_type = isSgJovialTableType(mod_type->get_base_type());
     }
   }
   ASSERT_not_null(table_type);

   switch (symbol->variantT())
      {
      case V_SgClassSymbol:
         {
            table_arg = SB::buildTypeExpression(table_type);
            break;
         }
      case V_SgVariableSymbol:
         {
            table_arg = SB::buildVarRefExp_nfi(isSgVariableSymbol(symbol));
            break;
         }
      default: ROSE_ABORT();
      }

   ASSERT_not_null(table_arg);
   setSourcePosition(table_arg, t_argument);

   // build the parameter list
   SgExprListExp* params = SB::buildExprListExp_nfi();
   params->append_expression(table_arg);

   SgType* return_type = SB::buildSignedIntType();

   func_call = SB::buildFunctionCallExp("NWDSEN", return_type, params, SB::topScopeStack());
   ASSERT_not_null(func_call);
   setSourcePosition(func_call, term);

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
   SgExpression* param  = nullptr;

   func_call = nullptr;

   if (ATmatch(term, "StatusInverseFunctionFIRST(<term>)", &t_argument)) {
      if (traverse_Formula(t_argument, argument)) {
         // MATCHED StatusFormula
      }
      else if (traverse_Name(t_argument, var_name)) {
         // MATCHED StatusTypeName
      }
      else return ATfalse;
      function_name = "FIRST";
   }
   else if (ATmatch(term, "StatusInverseFunctionLAST(<term>)", &t_argument)) {
      if (traverse_Formula(t_argument, argument)) {
         // MATCHED StatusFormula
      }
      else if (traverse_Name(t_argument, var_name)) {
         // MATCHED StatusTypeName
      }
      else return ATfalse;
      function_name = "LAST";
   }
   else return ATfalse;

// Build the parameter list and then the variable reference or type expression
   SgExprListExp* params = SB::buildExprListExp_nfi();

// The variable may be a status type
   SgSymbol* symbol = SI::lookupSymbolInParentScopes(var_name, SB::topScopeStack());
   if (symbol) {
      SgEnumType* type = isSgEnumType(symbol->get_type());
      if (type) {
         param = SB::buildTypeExpression(type);
      }
   }
   if (!param) {
      // Parameter is not a type expression so it must be a variable
      param  = SB::buildVarRefExp(var_name, SB::topScopeStack());
   }
   ASSERT_not_null(param);
   params->append_expression(param);

   SgType* return_type = SB::buildIntType();

   func_call = SB::buildFunctionCallExp(function_name, return_type, params, SB::topScopeStack());

   ASSERT_not_null(func_call);

   return ATtrue;
}

//========================================================================================
// 6.3.12 NENT FUNCTION
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_NentFunction(ATerm term, SgFunctionCallExp* &func_call)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_NentFunction: %s\n", ATwriteToString(term));
#endif

   ATerm t_argument;
   std::string var_name;
   std::string function_name;
   SgExpression* param  = nullptr;

   func_call = nullptr;

   if (ATmatch(term, "NentFunction(<term>)", &t_argument)) {
     if (traverse_Name(t_argument, var_name)) {
       // MATCHED StatusTypeName
     } else return ATfalse;
   }
   else return ATfalse;

// Build the parameter list and then the variable reference or type expression
   SgExprListExp* params = SB::buildExprListExp_nfi();

// The variable may be a table name or table type name
   SgSymbol* symbol = SI::lookupSymbolInParentScopes(var_name, SB::topScopeStack());
   if (symbol) {
     if (SgVariableSymbol* var_sym = isSgVariableSymbol(symbol)) {
       param  = SB::buildVarRefExp(var_name, SB::topScopeStack());
     }
     else if (SgJovialTableType* type = isSgJovialTableType(symbol->get_type())) {
       if (type) {
         param = SB::buildTypeExpression(type);
       }
     }
   }
   ASSERT_not_null(param);
   params->append_expression(param);

   SgType* return_type = SB::buildIntType();
   func_call = SB::buildFunctionCallExp(SgName("NENT"), return_type, params, SB::topScopeStack());
   ASSERT_not_null(func_call);

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

   ATerm t_bit_type_desc;
   std::string bit_type_name;

   if (ATmatch(term, "BitTypeConversion(<term>)", &t_bit_type_desc)) {
      // MATCHED BitTypeConversion

      if (traverse_BitItemDescription(t_bit_type_desc, type)) {
         // MATCHED BitItemDescription
      } else return ATfalse;

   } else if (ATmatch(term, "BitTypeConversionB()")) {
      // MATCHED BitTypeConversionB

      SgExpression* size = nullptr;
      type = SB::buildJovialBitType(size);

   } else if (traverse_Name(term, bit_type_name)) {
      // MATCHED BitTypeName
      // BitTypeName shouldn't be able to happen (parses as UserDefinedFunctionCall)
      mlog[WARN] << "ERROR: BitTypeConversion - BitTypeName\n";
      ROSE_ABORT();
   }
   else return ATfalse;

   // Note: a RepConversion appears as part of RepFunctionVariable

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

   ASSERT_not_null(type);

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
         // Not optional here
         ASSERT_not_null(type);
      } else return ATfalse;
   }
   else return ATfalse;

   ASSERT_not_null(type);

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
   SgEnumDeclaration* enum_decl = nullptr;

   type = nullptr;

   if (ATmatch(term, "StatusConversion(<term>)", &t_type)) {
      mlog[WARN] << "UNIMPLEMENTED: StatusConversion\n";
      if (traverse_StatusItemDescription(t_type, enum_decl, status_size)) {
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

ATbool ATermToSageJovialTraversal::traverse_FixedOrFloatingLiteral(ATerm term, SgExpression* &expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FixedOrFloatingLiteral: %s\n", ATwriteToString(term));
#endif

   ATerm t_frac_form, t_num1, t_num2, t_opt_exp;
   std::string literal, opt_exp;
   char* number;

   expr = nullptr;

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
      mlog[WARN] << "ERROR in traverse_FixedOrFloatingLiteral, no float literal, contains only: " << literal << "\n";;
      ROSE_ABORT();
   }

   expr = SageBuilder::buildFloatVal_nfi(literal);
   setSourcePosition(expr, term);

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

      expr = SageBuilder::buildJovialBitVal_nfi(literal);
      setSourcePosition(expr, term);
   } else return ATfalse;

   ASSERT_not_null(expr);

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

   ASSERT_not_null(expr);
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
      expr = SageBuilder::buildNullptrValExp_nfi();
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
   else if (traverse_SkipDirective(term)) {
      // MATCHED SkipDirective
   }
   else if (traverse_BeginDirective(term)) {
      // MATCHED BeginDirective
   }
   else if (traverse_EndDirective(term)) {
      // MATCHED EndDirective
   }
   else if (traverse_LinkageDirective(term)) {
      // MATCHED LinkageDirective
   }
   else if (traverse_ReducibleDirective(term)) {
      // MATCHED ReducibleDirective
   }
   else if (traverse_NolistDirective(term)) {
      // MATCHED NolistDirective
   }
   else if (traverse_ListDirective(term)) {
      // MATCHED ListDirective
   }
   else if (traverse_EjectDirective(term)) {
      // MATCHED EjectDirective
   }
   else if (traverse_InitializeDirective(term)) {
      // MATCHED InitializeDirective
   }
   else if (traverse_OrderDirective(term)) {
      // MATCHED OrderDirective
   }
   else if (traverse_AlignDirective(term)) {
      // MATCHED AlignDirective
   }
   else if (traverse_AlwaysDirective(term)) {
      // MATCHED AlwaysDirective
   }
   else return ATfalse;

   return ATtrue;

//  CopyDirective            -> Directive
//  TraceDirective           -> Directive
//  InterferenceDirective    -> Directive
//  ListinvDirective         -> Directive
//  ListexpDirective         -> Directive
//  ListbothDirective        -> Directive
//  BaseDirective            -> Directive
//  IsbaseDirective          -> Directive
//  DropDirective            -> Directive
//  LeftrightDirective       -> Directive
//  RearrangeDirective       -> Directive

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
   std::string declared_name;
   std::string compool_name;
   std::vector<std::string> declared_name_list;
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
            if (traverse_Name(head, declared_name)) {
               // CompoolDeclaredName is a list of names to be used by the current module
               std::string lc_name{declared_name};
               // Convert to lower case to enable case insensitivity
               std::transform(declared_name.begin(), declared_name.end(), lc_name.begin(), ::tolower);
               declared_name_list.push_back(std::string(lc_name));
               mlog[DEBUG] << "appended declared compool name: " << lc_name << " compool " << compool_name <<"\n";
            } else return ATfalse;
         }
      } else return ATfalse;
   }
   else return ATfalse;

// Remove single quotes
   unsigned int len = compool_name.length();
   ASSERT_require(len > 2);
   if (compool_name[0] == '\'' && compool_name[len-1] == '\'') {
      compool_name = compool_name.substr(1,len-2);
   }

   sage_tree_builder.Enter(directive_stmt, compool_name, declared_name_list);
   directive_stmt->set_directive_type(SgJovialDirectiveStatement::e_compool);

   sage_tree_builder.Leave(directive_stmt);

   return ATtrue;
}

//========================================================================================
// 9.2.2 SKIP, BEGIN, AND END DIRECTIVES
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_SkipDirective(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SkipDirective: %s\n", ATwriteToString(term));
#endif

   ATerm t_opt_letter;
   SgJovialDirectiveStatement* directive_stmt = nullptr;
   std::string str = "";
   char* letter;

   if (ATmatch(term, "SkipDirective(<term>)", &t_opt_letter)) {
      // MATCHED SkipDirective

      if (ATmatch(t_opt_letter, "no-letter")) {
         // MATCHED no-letter
      }
      else if (ATmatch(t_opt_letter, "<str>", &letter)) {
         str = letter;
      } else return ATfalse;

   } else return ATfalse;

   sage_tree_builder.Enter(directive_stmt, str);
   directive_stmt->set_directive_type(SgJovialDirectiveStatement::e_skip);

   sage_tree_builder.Leave(directive_stmt);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_BeginDirective(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BeginDirective: %s\n", ATwriteToString(term));
#endif

   ATerm t_opt_letter;
   SgJovialDirectiveStatement* directive_stmt = nullptr;
   std::string str = "";
   char* letter;

   if (ATmatch(term, "BeginDirective(<term>)", &t_opt_letter)) {
      // MATCHED BeginDirective

      if (ATmatch(t_opt_letter, "no-letter")) {
         // MATCHED no-letter
      }
      else if (ATmatch(t_opt_letter, "<str>", &letter)) {
         str = letter;
      } else return ATfalse;

   } else return ATfalse;

   sage_tree_builder.Enter(directive_stmt, str);
   directive_stmt->set_directive_type(SgJovialDirectiveStatement::e_begin);

   sage_tree_builder.Leave(directive_stmt);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_EndDirective(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_EndDirective: %s\n", ATwriteToString(term));
#endif

   SgJovialDirectiveStatement* directive_stmt = nullptr;

   if (ATmatch(term, "EndDirective()")) {
      // MATCHED EndDirective
   }
   else return ATfalse;

   sage_tree_builder.Enter(directive_stmt, std::string(""));
   directive_stmt->set_directive_type(SgJovialDirectiveStatement::e_end);

   sage_tree_builder.Leave(directive_stmt);

   return ATtrue;
}

//========================================================================================
// 9.3 LINKAGE DIRECTIVES
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_LinkageDirective(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_LinkageDirective: %s\n", ATwriteToString(term));
#endif

   ATerm t_symbol;
   SgJovialDirectiveStatement* directive_stmt = nullptr;
   std::string str = "";
   SgExpression* expr = nullptr;

   if (ATmatch(term, "LinkageDirective(<term>)", &t_symbol)) {
      // MATCHED LinkageDirective

      ATermList tail = (ATermList) ATmake("<term>", t_symbol);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_Literal(head, expr)) {
            // MATCHED Literal
            SgIntVal* int_val = isSgIntVal(expr);
            if (int_val) {
               str = int_val->get_valueString();
               // the expression for int_val is no longer needed as the value is stored
               delete int_val;  int_val = nullptr;
            }
         } else return ATfalse;
      }
   }
   else return ATfalse;

   sage_tree_builder.Enter(directive_stmt, str);
   directive_stmt->set_directive_type(SgJovialDirectiveStatement::e_linkage);

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

   sage_tree_builder.Enter(directive_stmt, std::string(""));
   directive_stmt->set_directive_type(SgJovialDirectiveStatement::e_reducible);

   sage_tree_builder.Leave(directive_stmt);

   return ATtrue;
}

//========================================================================================
// 9.7.1 SOURCE-LISTING DIRECTIVES
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_NolistDirective(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_NolistDirective: %s\n", ATwriteToString(term));
#endif

   SgJovialDirectiveStatement* directive_stmt = nullptr;

   if (ATmatch(term, "NolistDirective()")) {
      // MATCHED NolistDirective
   }
   else return ATfalse;

   sage_tree_builder.Enter(directive_stmt, std::string(""));
   directive_stmt->set_directive_type(SgJovialDirectiveStatement::e_nolist);

   sage_tree_builder.Leave(directive_stmt);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_ListDirective(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ListDirective: %s\n", ATwriteToString(term));
#endif

   SgJovialDirectiveStatement* directive_stmt = nullptr;

   if (ATmatch(term, "ListDirective()")) {
      // MATCHED ListDirective
   }
   else return ATfalse;

   sage_tree_builder.Enter(directive_stmt, std::string(""));
   directive_stmt->set_directive_type(SgJovialDirectiveStatement::e_list);

   sage_tree_builder.Leave(directive_stmt);

   return ATtrue;
}

ATbool ATermToSageJovialTraversal::traverse_EjectDirective(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_EjectDirective: %s\n", ATwriteToString(term));
#endif

   SgJovialDirectiveStatement* directive_stmt = nullptr;

   if (ATmatch(term, "EjectDirective()")) {
      // MATCHED EjectDirective
   }
   else return ATfalse;

   sage_tree_builder.Enter(directive_stmt, std::string(""));
   directive_stmt->set_directive_type(SgJovialDirectiveStatement::e_eject);

   sage_tree_builder.Leave(directive_stmt);

   return ATtrue;
}

//========================================================================================
// 9.10 INITIALIZATION DIRECTIVES
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_InitializeDirective(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_InitializeDirective: %s\n", ATwriteToString(term));
#endif

   SgJovialDirectiveStatement* directive_stmt = nullptr;

   if (ATmatch(term, "InitializeDirective()")) {
      // MATCHED InitializeDirective
   }
   else return ATfalse;

   sage_tree_builder.Enter(directive_stmt, std::string(""));
   directive_stmt->set_directive_type(SgJovialDirectiveStatement::e_initialize);

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

   sage_tree_builder.Enter(directive_stmt, std::string(""));
   directive_stmt->set_directive_type(SgJovialDirectiveStatement::e_order);

   sage_tree_builder.Leave(directive_stmt);

   return ATtrue;
}

//========================================================================================
// ALIGN Directive: Non-standard, must be for odd compiler
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_AlignDirective(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_AlignDirective: %s\n", ATwriteToString(term));
#endif

   ATerm t_name;
   char* str;
   std::string name;
   SgJovialDirectiveStatement* directive_stmt = nullptr;

   if (ATmatch(term, "AlignDirective(<term>)", &t_name)) {
      // MATCHED AlignDirective
   }
   else return ATfalse;

   if (ATmatch(t_name, "<str>", &str)) {
      name = str;
   } else return ATfalse;

   sage_tree_builder.Enter(directive_stmt, name);
   directive_stmt->set_directive_type(SgJovialDirectiveStatement::e_align);

   sage_tree_builder.Leave(directive_stmt);

   return ATtrue;
}

//========================================================================================
// ALWAYS'STORE Directive: Non-standard, must be for odd compiler
//----------------------------------------------------------------------------------------
ATbool ATermToSageJovialTraversal::traverse_AlwaysDirective(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_AlwaysDirective: %s\n", ATwriteToString(term));
#endif

   ATerm t_names;
   char* str;
   bool first{true};
   std::string name, content;
   SgJovialDirectiveStatement* directive_stmt = nullptr;

   if (ATmatch(term, "AlwaysDirective(<term>)", &t_names)) {
      // MATCHED AlwaysDirective
   }
   else return ATfalse;

   ATermList tail = (ATermList) ATmake("<term>", t_names);
   while (! ATisEmpty(tail)) {
      ATerm head = ATgetFirst(tail);
      tail = ATgetNext(tail);
      if (ATmatch(head, "<str>", &str)) {
         name = str;
      } else return ATfalse;
      if (first) {
         first = false;
         content = name;
      }
      else {
         content = content + std::string{","} + name;
      }
   }

   sage_tree_builder.Enter(directive_stmt, content);
   directive_stmt->set_directive_type(SgJovialDirectiveStatement::e_always);

   sage_tree_builder.Leave(directive_stmt);

   return ATtrue;
}

void ATermToSageJovialTraversal::
setSourcePositions(ATerm term, RB::SourcePosition &start, RB::SourcePosition &end)
{
   PosInfo pos{getLocation(term)};

   start.path   = getCurrentFilename();
   start.line   = pos.getStartLine();
   start.column = pos.getStartCol();

   end.path   = getCurrentFilename();
   end.line   = pos.getEndLine();
   end.column = pos.getEndCol();
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

SgVarRefExp* ATermSupport::
buildIntrinsicVarRefExp_nfi(const std::string &name, SgScopeStatement* scope)
{
  // TODO: The type for the expression is SgUnknownType, make better
  return SageBuilder::buildVarRefExp(name, scope);
}

SgFunctionCallExp* ATermSupport::
buildIntrinsicFunctionCallExp_nfi(const std::string &name, SgExprListExp* params, SgScopeStatement* scope)
{
  SgType* type = nullptr;
  SgFunctionCallExp* callExpr = nullptr;

  if (!params) {
    params = SB::buildExprListExp_nfi();
  }
  if (!scope) {
    scope = SB::topScopeStack();
  }
  ASSERT_not_null(params);
  ASSERT_not_null(scope);

  // Create a return type based on the intrinsic name
  if (name == "BYTEPOS") {
    type = SageBuilder::buildIntType();
  }
  else if (name == "IMPLINTSIZE") {
    type = SageBuilder::buildIntType();
  }
  else if (name == "MAXINT") {
    type = SageBuilder::buildIntType();
  }
  else if (name == "MINFRACTION") {
    type = SageBuilder::buildIntType();
  }
  else if (name == "MININT") {
    type = SageBuilder::buildIntType();
  }
  else if (name == "MINRELPRECISION") {
    type = SageBuilder::buildIntType();
  }
  else if (name == "MINSCALE") {
    type = SageBuilder::buildIntType();
  }
  else if (name == "MINSIZE") {
    type = SageBuilder::buildIntType();
  }

  // Create double return type (Note: assumes double for max float)
  else if (name == "FLOATRELPRECISION") {
    type = SageBuilder::buildDoubleType();
  }
  else if (name == "FLOATUNDERFLOW") {
    type = SageBuilder::buildDoubleType();
  }

  else {
    type = SB::buildVoidType();
  }

  if (type) {
    callExpr = SB::buildFunctionCallExp(SgName(name), type, params, scope);
    ASSERT_not_null(callExpr);
    SI::setSourcePosition(callExpr);
  }

  return callExpr;
}
