#include "sage3basic.h"
#include "untypedBuilder.h"

#include "ATermToUntypedJovialTraversal.h"
#include "Jovial_to_ROSE_translation.h"
#include <iostream>

#define PRINT_ATERM_TRAVERSAL 0
#define PRINT_SOURCE_POSITION 0

using namespace ATermSupport;
using std::cout;
using std::cerr;
using std::endl;

ATermToUntypedJovialTraversal::ATermToUntypedJovialTraversal(SgSourceFile* source) : ATermToUntypedTraversal(source)
{
   UntypedBuilder::set_language(SgFile::e_Jovial_language);
}

ATermToUntypedJovialTraversal::~ATermToUntypedJovialTraversal()
{
}

//========================================================================================
// 1.1 Module
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedJovialTraversal::traverse_Module(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Module: %s\n", ATwriteToString(term));
#endif

   ATerm module;
   SgUntypedGlobalScope* global_scope = get_scope();

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
ATbool ATermToUntypedJovialTraversal::traverse_CompoolModule(ATerm term, SgUntypedScope* scope)
{ 
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CompoolModule: %s\n", ATwriteToString(term));
#endif

   return ATfalse;
}

//========================================================================================
// 1.2.2 PROCEDURE MODULES
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedJovialTraversal::traverse_ProcedureModule(ATerm term, SgUntypedScope* scope)
{ 
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ProcedureModule: %s\n", ATwriteToString(term));
#endif

   return ATfalse;
}

ATbool ATermToUntypedJovialTraversal::traverse_DeclarationList(ATerm term, SgUntypedDeclarationStatementList* decl_list)
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
         } else if (traverse_NullDeclaration(head, decl_list)) {
            // MATCHED NullDeclaration
         } else return ATfalse;
      }
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_NullDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list)
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
ATbool ATermToUntypedJovialTraversal::traverse_MainProgramModule(ATerm term, SgUntypedGlobalScope* global_scope)
{ 
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_MainProgramModule: %s\n", ATwriteToString(term));
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

      std::string label = "";

      SgUntypedInitializedNameList* param_list = new SgUntypedInitializedNameList();
      SgUntypedExprListExpression* prefix_list = new SgUntypedExprListExpression();

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

ATbool ATermToUntypedJovialTraversal::traverse_Name(ATerm term, std::string & name)
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

ATbool ATermToUntypedJovialTraversal::traverse_ProgramBody(ATerm term, SgUntypedFunctionScope** function_scope)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ProgramBody: %s\n", ATwriteToString(term));
#endif

   ATerm t_stmt;
   ATerm t_decls, t_stmts, t_funcs, t_labels;
   std::vector<std::string> labels;
   std::vector<PosInfo> locations;
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
      assert(labels.size() <= 1);
      if (labels.size() == 1) temp_label = labels[0];

      *function_scope = new SgUntypedFunctionScope(temp_label,decl_list,stmt_list,func_list);
   }

   else return ATfalse;

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_NonNestedSubroutineList(ATerm term, SgUntypedScope* scope)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_NonNestedSubroutineList: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "NonNestedSubroutineList([])")) {
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_SubroutineDefinitionList(ATerm term, SgUntypedFunctionDeclarationList*)
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
ATbool ATermToUntypedJovialTraversal::traverse_DataDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_DataDeclaration: %s\n", ATwriteToString(term));
#endif

   if (traverse_ItemDeclaration(term, decl_list)) {
      // MATCHED ItemDeclaration
   } else if (traverse_TableDeclaration(term, decl_list)) {
      // MATCHED TableDeclaration -> DataDeclaration
   } else return ATfalse;

   //  ConstantDeclaration         -> DataDeclaration
   //  BlockDeclaration            -> DataDeclaration

   return ATtrue;
}

//========================================================================================
// 2.1.1 ITEM DECLARATION
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedJovialTraversal::traverse_ItemDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ItemDeclaration: %s\n", ATwriteToString(term));
#endif

   ATerm t_name, t_alloc, t_type, t_preset;
   char* name;

   SgUntypedType* declared_type;

   SgUntypedVariableDeclaration* variable_decl = NULL;
   SgUntypedInitializedNameList* var_name_list = new SgUntypedInitializedNameList();
   SgUntypedExprListExpression*      attr_list = new SgUntypedExprListExpression();

   if (ATmatch(term, "ItemDeclaration(<term>,<term>,<term>,<term>)", &t_name,&t_alloc,&t_type,&t_preset)) {
      if (ATmatch(t_name, "<str>", &name)) {
         // MATCHED ItemName
      } else return ATfalse;

      if (traverse_OptAllocationSpecifier(t_alloc, attr_list)) {
         // MATCHED OptAllocationSpecifier
      } else return ATfalse;

      if (traverse_ItemTypeDescription(t_type, &declared_type, attr_list)) {
         // MATCHED ItemTypeDescription
      } else return ATfalse;

      if (ATmatch(t_preset, "no-item-preset()")) {
         // MATCHED no-item-preset
      }
      else {
         // TODO - ItemPreset
         return ATfalse;
      }
   }
   else return ATfalse;

   std::cout << "ITEM DECLARATION " << name << "\n";

   std::string label = "";

   SgUntypedInitializedName* initialized_name = new SgUntypedInitializedName(declared_type, name);
   setSourcePosition(initialized_name, t_name);

// There will be only one variable declared in Jovial
   var_name_list->get_name_list().push_back(initialized_name);

   variable_decl = new SgUntypedVariableDeclaration(label, declared_type, attr_list, var_name_list);
   setSourcePosition(variable_decl, term);

   decl_list->get_decl_list().push_back(variable_decl);

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_ItemTypeDescription(ATerm term, SgUntypedType** type, SgUntypedExprListExpression* attr_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ItemTypeDescription: %s\n", ATwriteToString(term));
#endif

   if (traverse_IntegerItemDescription(term, type, attr_list)) {
      // MATCHED IntegerItemDescription
   }
   else if (traverse_FloatingItemDescription(term, type, attr_list)) {
      // MATCHED FloatingItemDescription
   }
   else if (traverse_FixedItemDescription(term, type, attr_list)) {
      // MATCHED FixedItemDescription
   }
   else if (traverse_BitItemDescription(term, type)) {
      // MATCHED BitItemDescription
   }
   else if (traverse_CharacterItemDescription(term, type)) {
      // MATCHED CharacterItemDescription
   }
   else if (traverse_StatusItemDescription(term, type)) {
      // MATCHED StatusItemDescription
   }
   else if (traverse_PointerItemDescription(term, type)) {
      // MATCHED PointerItemDescription
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.1.1 INTEGER TYPE DESCRIPTIONS
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedJovialTraversal::traverse_IntegerItemDescription(ATerm term, SgUntypedType** type, SgUntypedExprListExpression* attr_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_IntegerItemDescription: %s\n", ATwriteToString(term));
#endif

   ATerm t_type, t_round_or_truncate, t_size;
   bool has_round_or_truncate, has_size;
   SgUntypedExpression* size;
   General_Language_Translation::ExpressionKind modifier_enum;

// The first term, t_type, comes from the lexer and is direct user input: "S", "s", "U", "u".
// We disambiguate it via terms IntegerItemDescription or IntegerItemDescriptionU.
   if (ATmatch(term, "IntegerItemDescription(<term>,<term>,<term>)", &t_type,&t_round_or_truncate,&t_size)) {
      *type = UntypedBuilder::buildType(SgUntypedType::e_int);
   }
   else if (ATmatch(term, "IntegerItemDescriptionU(<term>,<term>,<term>)", &t_type,&t_round_or_truncate,&t_size)) {
      *type = UntypedBuilder::buildType(SgUntypedType::e_uint);
   }
   else return ATfalse;

   if (traverse_OptRoundOrTruncate(t_round_or_truncate, &has_round_or_truncate, &modifier_enum)) {
      // MATCHED OptRoundOrTruncate
      if (has_round_or_truncate) {
         if (modifier_enum == General_Language_Translation::e_type_modifier_round) {
            cerr << "WARNING: e_type_modifier_round information not forwarded from ATerm traversal \n";
         }
         else if (modifier_enum == General_Language_Translation::e_type_modifier_truncate) {
            cerr << "WARNING: e_type_modifier_truncate information not forwarded from ATerm traversal \n";
         }
         else if (modifier_enum == General_Language_Translation::e_type_modifier_z) {
            cerr << "WARNING: e_type_modifier_z information not forwarded from ATerm traversal \n";
         }
         else ROSE_ASSERT(false);
      }
   } else return ATfalse;

   if (traverse_OptItemSize(t_size, &has_size, &size)) {
      (*type)->set_has_kind(has_size);
      (*type)->set_type_kind(size);
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_OptItemSize(ATerm term, bool* has_size, SgUntypedExpression** size)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptItemSize: %s\n", ATwriteToString(term));
#endif

   ATerm t_size;

   *size = NULL;
   *has_size = false;

   if (ATmatch(term, "no-item-size()")) {
     // MATCHED no-item-size
   }
   else if (ATmatch(term, "ItemSize(<term>)", &t_size)) {
      if (traverse_NumericFormula(t_size, size)) {
        // MATCHED NumericFormula
      } else return ATfalse;
      *has_size = true;
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.1.2 FLOATING TYPE DESCRIPTIONS
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedJovialTraversal::traverse_FloatingItemDescription(ATerm term, SgUntypedType** type, SgUntypedExprListExpression* attr_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FloatingItemDescription: %s\n", ATwriteToString(term));
#endif

   ATerm t_round_or_truncate, t_precision;
   bool has_round_or_truncate;
   General_Language_Translation::ExpressionKind modifier_enum;
   SgUntypedExpression* precision;

   if (ATmatch(term, "FloatingItemDescription (<term>,<term>)", &t_round_or_truncate,&t_precision)) {
      *type = UntypedBuilder::buildType(SgUntypedType::e_float);
   }
   else return ATfalse;

   if (traverse_OptRoundOrTruncate(t_round_or_truncate, &has_round_or_truncate, &modifier_enum)) {
      // MATCHED OptRoundOrTruncate
      if (has_round_or_truncate) {
         if (modifier_enum == General_Language_Translation::e_type_modifier_round) {
            cerr << "WARNING: e_type_modifier_round information not forwarded from ATerm traversal \n";
         }
         else if (modifier_enum == General_Language_Translation::e_type_modifier_truncate) {
            cerr << "WARNING: e_type_modifier_truncate information not forwarded from ATerm traversal \n";
         }
         else if (modifier_enum == General_Language_Translation::e_type_modifier_z) {
            cerr << "WARNING: e_type_modifier_z information not forwarded from ATerm traversal \n";
         }
         else ROSE_ASSERT(false);
      }
   } else return ATfalse;

   if (traverse_FloatingFormula(t_precision, &precision)) {
      (*type)->set_has_kind(true);
      (*type)->set_type_kind(precision);
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_OptRoundOrTruncate(ATerm term, bool* has_round_or_truncate,
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
ATbool ATermToUntypedJovialTraversal::traverse_FixedItemDescription(ATerm term, SgUntypedType** type, SgUntypedExprListExpression* attr_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FixedItemDescription: %s\n", ATwriteToString(term));
#endif

   ATerm t_round_or_truncate, t_scale, t_fraction;
   bool has_round_or_truncate;  //, has_fraction_specifier;
   General_Language_Translation::ExpressionKind modifier_enum;
   SgUntypedExpression * scale, * fraction;

   if (ATmatch(term, "FixedItemDescription (<term>,<term>,<term>)", &t_round_or_truncate,&t_scale,&t_fraction)) {
      *type = UntypedBuilder::buildType(SgUntypedType::e_unknown);
   }
   else return ATfalse;

   if (traverse_OptRoundOrTruncate(t_round_or_truncate, &has_round_or_truncate, &modifier_enum)) {
      // MATCHED OptRoundOrTruncate
      if (has_round_or_truncate) {
         if (modifier_enum == General_Language_Translation::e_type_modifier_round) {
            cerr << "WARNING: e_type_modifier_round information not forwarded from ATerm traversal \n";
         }
         else if (modifier_enum == General_Language_Translation::e_type_modifier_truncate) {
            cerr << "WARNING: e_type_modifier_truncate information not forwarded from ATerm traversal \n";
         }
         else if (modifier_enum == General_Language_Translation::e_type_modifier_z) {
            cerr << "WARNING: e_type_modifier_z information not forwarded from ATerm traversal \n";
         }
         else ROSE_ASSERT(false);
      }
   } else return ATfalse;

   if (traverse_NumericFormula(t_scale, &scale)) {
      (*type)->set_has_kind(true);
      (*type)->set_type_kind(scale);
   } else return ATfalse;

   if (ATmatch(t_fraction, "no-fraction-specifier()")) {
      // MATCHED no-fraction-specifier
   }
   else if (traverse_FractionSpecifier(t_fraction, &fraction)) {
      cerr << "WARNING: fraction specifier has not been implemented \n";
      return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_FractionSpecifier(ATerm term, SgUntypedExpression** fraction)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FractionSpecifier: %s\n", ATwriteToString(term));
#endif

   ATerm t_fraction;
   *fraction = NULL;

   if (ATmatch(term, "FractionSpecifier(<term>)", &t_fraction)) {
      if (traverse_FixedFormula(t_fraction, fraction)) {
        // MATCHED FixedFormula
      } else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.1.4 BIT TYPE DESCRIPTIONS
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedJovialTraversal::traverse_BitItemDescription(ATerm term, SgUntypedType** type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BitItemDescription: %s\n", ATwriteToString(term));
#endif

   ATerm t_size;
   bool has_size;
   SgUntypedExpression* size;

   if (ATmatch(term, "BitItemDescription(<term>)", &t_size)) {
       *type = UntypedBuilder::buildType(SgUntypedType::e_bit);
   } else return ATfalse;

   if (traverse_OptItemSize(t_size, &has_size, &size)) {
      (*type)->set_has_kind(has_size);
      (*type)->set_type_kind(size);
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.1.5 CHARACTER TYPE DESCRIPTIONS
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedJovialTraversal::traverse_CharacterLiteral(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CharacterLiteral: %s\n", ATwriteToString(term));
#endif

   char* name;
   Jovial_ROSE_Translation::ExpressionKind expr_enum = Jovial_ROSE_Translation::e_referenceExpression;

   if (ATmatch(term, "CharacterLiteral(<str>)", &name)) {
      std::cout << "CharacterLiteral is " << name << endl;
      *expr = new SgUntypedReferenceExpression(expr_enum, name);
      setSourcePosition(*expr, term);

   } else return ATfalse;

   return ATtrue;
}

#if 0
ATbool ATermToUntypedJovialTraversal::traverse_CharacterItemDescription(ATerm term, SgUntypedType** type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CharacterItemDescription: %s\n", ATwriteToString(term));
#endif

   ATerm t_size;
   bool has_size;
   SgUntypedExpression* size;

   if (ATmatch(term, "CharacterItemDescription(<term>)", &t_size)) {
      // MATCHED CharacterItemDescription
   } else return ATfalse;

   if (traverse_OptItemSize(t_size, &has_size, &size)) {
      if (has_size) {
         *type = UntypedBuilder::buildType(SgUntypedType::e_string);
         (*type)->set_char_length_expression(size);
      }
      else {
         *type = UntypedBuilder::buildType(SgUntypedType::e_char);
      }
   } else return ATfalse;

   return ATtrue;
}
#endif

// Previous version of CharacterItemDescription is above
// Current and temporary version of CharacterItemDescription is below

ATbool ATermToUntypedJovialTraversal::traverse_CharacterItemDescription(ATerm term, SgUntypedType** type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CharacterItemDescription: %s\n", ATwriteToString(term));
#endif

   ATerm t_size;
   bool has_size;
   SgUntypedExpression* size;

   *type = NULL;

   if (ATmatch(term, "CharacterItemDescription(<term>)", &t_size)) {
      if (traverse_OptItemSize(t_size, &has_size, &size)) {
         // In grammar (2.1.1.5), uses OptCharacterSize and CharacterSize but cons is ItemSize, so used that traversal
         // MATCHED OptItemSize
      } else return ATfalse;
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.1.6 STATUS TYPE DESCRIPTIONS
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedJovialTraversal::traverse_StatusConstant(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_StatusConstant: %s\n", ATwriteToString(term));
#endif

   // Dealt with this as other strings have been dealt with
   // NOTE: Name -> StatusConstant , Letter -> StatusConstant, ReservedWord -> StatusConstant

   char* name;
   Jovial_ROSE_Translation::ExpressionKind expr_enum = Jovial_ROSE_Translation::e_referenceExpression;

   if (ATmatch(term, "StatusConstant(<str>)", &name)) {
      std::cout << "StatusConstant is " << name << endl;
      *expr = new SgUntypedReferenceExpression(expr_enum, name);
      setSourcePosition(*expr, term);

   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_StatusItemDescription(ATerm term, SgUntypedType** type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_StatusItemDescription: %s\n", ATwriteToString(term));
#endif

   ATerm t_size, t_sublist;
   bool has_size;
   SgUntypedExpression* size;

   if (ATmatch(term, "StatusItemDescription(<term>,<term>)", &t_size, &t_sublist)) {
      if (traverse_OptItemSize(t_size, &has_size, &size)) {
         // In grammar (2.1.1.6), uses OptStatusSize and StatusSize but cons is ItemSize, so used that traversal
         // MATCHED OptItemSize
      } else return ATfalse;
      if (traverse_DefaultSublist(t_sublist)) {
         // MATCHED DefaultSublist
      } else if (traverse_StatusList(t_sublist)) {
         // MATCHED StatusList
      } else return ATfalse;
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_DefaultSublist(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_DefaultSublist: %s\n", ATwriteToString(term));
#endif

   ATerm t_sublist;
   SgUntypedExpression* expr;

   if (ATmatch(term, "DefaultSublist(<term>)", &t_sublist)) {
      ATermList tail = (ATermList) ATmake("<term>", t_sublist);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_StatusConstant(head, &expr)) {
            // MATCHED StatusConstant
         } else return ATfalse;
      }
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_OptDefaultSublist(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptDefaultSublist: %s\n", ATwriteToString(term));
#endif

   ATerm t_sublist;

   if (ATmatch(term, "OptDefaultSublist(<term>)", &t_sublist)) {
      if (ATmatch(term, "no-default-sublist()")) {
         std::cout << "Matched no-default-sublist" << endl;
      } else if (traverse_DefaultSublist(t_sublist)) {
         // MATCHED DefaultSublist
      }
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_StatusList(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_StatusList: %s\n", ATwriteToString(term));
#endif

   ATerm t_sublist, t_specified;

   if (ATmatch(term, "StatusList(<term>,<term>)", &t_sublist, &t_specified)) {
      if (traverse_OptDefaultSublist(t_sublist)) {
         // MATCHED OptDefaultSublist
      }
      ATermList tail = (ATermList) ATmake("<term>", t_specified);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_SpecifiedSublist(head)) {
           // MATCHED SpecifiedSublist
         } else return ATfalse;
      }
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_SpecifiedSublist(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SpecifiedSublist: %s\n", ATwriteToString(term));
#endif

   ATerm t_formula, t_constant;
   SgUntypedExpression* expr;

   if (ATmatch(term, "SpecifiedSublist(<term>,<term>)", &t_formula, &t_constant)) {
     if (traverse_NumericFormula(t_formula, &expr)) {
         // MATCHED NumericFormula
      }
      ATermList tail = (ATermList) ATmake("<term>", t_constant);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_StatusConstant(head, &expr)) {
           // MATCHED StatusConstant
         } else return ATfalse;
      }
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.1.7 POINTER TYPE DESCRIPTIONS
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedJovialTraversal::traverse_PointerItemDescription(ATerm term, SgUntypedType** type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_PointerItemDescription: %s\n", ATwriteToString(term));
#endif

   char* pntr;
   ATerm type_name;

   if (ATmatch(term, "PointerItemDescription(<str>,<term>)", &pntr, &type_name)) {
      std::cout << "PointerTypeDesc is " << pntr << endl;

      if (traverse_OptTypeName(type_name)) {
         // MATCHED OptTypeName
      } else return ATfalse;
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_OptTypeName(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptTypeName: %s\n", ATwriteToString(term));
#endif

   char* type_name;

   if (ATmatch(term, "no-type-name()")) {
      // MATCHED no-type-name
      std::cout << "Matched no-type-name" << endl;
   } else if (ATmatch(term, "TypeName(<str>)", &type_name)) {
      std::cout << "TypeName is " << type_name << endl;
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.2 TABLE DECLARATION
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedJovialTraversal::traverse_TableDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_TableDeclaration: %s\n", ATwriteToString(term));
#endif

   ATerm t_name, t_alloc, t_dim_list, t_table_desc;
   char* name;

   SgUntypedType *declared_type, *actual_type;

   SgUntypedVariableDeclaration* variable_decl = NULL;
   SgUntypedInitializedNameList* var_name_list = NULL;
   SgUntypedExprListExpression*      attr_list = NULL;
   SgUntypedExprListExpression*       dim_info = NULL;

   if (ATmatch(term, "TableDeclaration(<term>,<term>,<term>,<term>)", &t_name,&t_alloc,&t_dim_list,&t_table_desc)) {
      var_name_list = new SgUntypedInitializedNameList();
      setSourcePosition(var_name_list, t_name);

      attr_list = new SgUntypedExprListExpression();
      setSourcePosition(attr_list, t_alloc);

      dim_info = new SgUntypedExprListExpression(General_Language_Translation::e_array_shape);
      setSourcePosition(dim_info, t_dim_list);

      if (ATmatch(t_name, "<str>", &name)) {
         // MATCHED ItemName
      } else return ATfalse;

      if (traverse_OptAllocationSpecifier(t_alloc, attr_list)) {
         // MATCHED OptAllocationSpecifier
      } else return ATfalse;

      if (traverse_OptDimensionList(t_dim_list, dim_info)) {
         // MATCHED OptDimensionList
      } else return ATfalse;

      if (traverse_TableDescription(t_table_desc, &declared_type, attr_list)) {
         // MATCHED TableDescription
      } else return ATfalse;

   } else return ATfalse;

   std::cout << "TABLE DECLARATION " << name << ", rank is " << dim_info->get_expressions().size() << endl;
   std::cout << "TABLE DECLARATION dim_info: " << dim_info << " attr_list: " << attr_list << endl;
   std::cout << "TABLE DECLARATION var_name: " << var_name_list << endl;

// TODO - label
   std::string label = "";

   actual_type = declared_type;
   if (dim_info->get_expressions().size() > 0) {
      int rank = dim_info->get_expressions().size();
      actual_type = UntypedBuilder::buildArrayType(declared_type->get_type_enum_id(),dim_info,rank);
   }

   SgUntypedInitializedName* initialized_name = new SgUntypedInitializedName(actual_type, name);
   setSourcePosition(initialized_name, t_name);

// There will be only one variable declared in Jovial
   var_name_list->get_name_list().push_back(initialized_name);

   variable_decl = new SgUntypedVariableDeclaration(label, declared_type, attr_list, var_name_list);
   setSourcePosition(variable_decl, term);

   decl_list->get_decl_list().push_back(variable_decl);

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_TableDescription(ATerm term, SgUntypedType** type, SgUntypedExprListExpression* attr_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_TableDescription: %s\n", ATwriteToString(term));
#endif

   ATerm t_struc_spec, t_entry_spec;

   if (ATmatch(term, "TableDescription(<term>,<term>)", &t_struc_spec,&t_entry_spec)) {

      if (traverse_OptStructureSpecifier(t_struc_spec, attr_list)) {
         // MATCHED OptStructureSpecifier
      } else return ATfalse;

      if (traverse_EntrySpecifier(t_entry_spec, type, attr_list)) {
         // MATCHED EntrySpecifier
      } else return ATfalse;

   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_EntrySpecifier(ATerm term, SgUntypedType** type, SgUntypedExprListExpression* attr_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_EntrySpecifier: %s\n", ATwriteToString(term));
#endif

   if (traverse_OrdinaryEntrySpecifier(term, type, attr_list)) {
      // MATCHED OrdinaryEntrySpecifier -> EntrySpecifier
   }
   else if (traverse_SpecifiedEntrySpecifier(term, type, attr_list)) {
      // MATCHED SpecifiedEntrySpecifier -> EntrySpecifier
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.2.1 TABLE DIMENSION LISTS
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedJovialTraversal::traverse_OptDimensionList(ATerm term, SgUntypedExprListExpression* dim_info)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptDimensionList: %s\n", ATwriteToString(term));
#endif

   ATerm t_dim_list;

   if (ATmatch(term, "no-dimension-list()")) {
   }
   else if (ATmatch(term, "DimensionList(<term>)" , &t_dim_list)) {
      ATermList tail = (ATermList) ATmake("<term>", t_dim_list);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_Dimension(head, dim_info)) {
            // MATCHED Dimension
         }
      }
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_Dimension(ATerm term, SgUntypedExprListExpression* dim_info)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Dimension: %s\n", ATwriteToString(term));
#endif

   ATerm t_opt_lower_bound, t_lower_bound, t_upper_bound;

   SgUntypedExpression* lower_bound = NULL;
   SgUntypedExpression* upper_bound = NULL;
   SgUntypedExpression*      stride = NULL;

   SgUntypedSubscriptExpression* range = NULL;

   General_Language_Translation::ExpressionKind expr_enum = General_Language_Translation::e_unknown;

   if (ATmatch(term, "Dimension(<term>,<term>)", &t_opt_lower_bound, &t_upper_bound)) {
      expr_enum = General_Language_Translation::e_explicit_dimension;

   // Lower bound (optional)
      if (ATmatch(t_opt_lower_bound, "no-lower-bound-option()")) {
         lower_bound = UntypedBuilder::buildUntypedNullExpression();
         setSourcePositionUnknown(lower_bound);
      }
      else if (ATmatch(t_opt_lower_bound, "LowerBoundOption(<term>)", &t_lower_bound)) {
         if (traverse_NumericFormula(t_lower_bound, &lower_bound)) {
            // MATCHED NumericFormula
         } else return ATfalse;
      } else return ATfalse;

   // Upper bound
      if (traverse_NumericFormula(t_upper_bound, &upper_bound)) {
         // MATCHED NumericFormula
      } else return ATfalse;
   }
   else if (ATmatch(term, "DimensionSTAR()")) {
      expr_enum = General_Language_Translation::e_star_dimension;

      lower_bound = UntypedBuilder::buildUntypedNullExpression();
      upper_bound = UntypedBuilder::buildUntypedNullExpression();

      setSourcePositionUnknown(lower_bound);
      setSourcePositionUnknown(upper_bound);
   }
   else return ATfalse;

   stride = UntypedBuilder::buildUntypedNullExpression();
   setSourcePositionUnknown(stride);

   range = new SgUntypedSubscriptExpression(expr_enum, lower_bound, upper_bound, stride);
   setSourcePosition(range, term);

   dim_info->get_expressions().push_back(range);

   return ATtrue;
}

//========================================================================================
// 2.1.2.2 TABLE STRUCTURE
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedJovialTraversal::traverse_OptStructureSpecifier(ATerm term, SgUntypedExprListExpression* attr_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptStructureSpecifier: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "no-structure-specifier()")) {
      // MATCHED no-structure-specifier
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.2.3 ORDINARY TABLE ENTRIES
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedJovialTraversal::traverse_OrdinaryEntrySpecifier(ATerm term, SgUntypedType** type, SgUntypedExprListExpression* attr_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OrdinaryEntrySpecifier: %s\n", ATwriteToString(term));
#endif

   ATerm t_pack_spec, t_item_desc, t_preset;

   if (ATmatch(term, "OrdinaryEntrySpecifier(<term>,<term>,<term>)", &t_pack_spec,&t_item_desc,&t_preset)) {

      if (traverse_OptPackingSpecifier(t_pack_spec, attr_list)) {
         // MATCHED OptStructureSpecifier
      } else return ATfalse;

      if (traverse_ItemTypeDescription(t_item_desc, type, attr_list)) {
         // MATCHED ItemTypeDescription
      } else return ATfalse;

   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_OptPackingSpecifier(ATerm term, SgUntypedExprListExpression* attr_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptPackingSpecifier: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "no-packing-specifier()")) {
      // MATCHED no-packing-specifier
   }
   else if (ATmatch(term, "N")) {
      // TODO - add attribute
      return ATfalse;
   }
   else if (ATmatch(term, "M")) {
      // TODO - add attribute
      return ATfalse;
   }
   else if (ATmatch(term, "D")) {
      // TODO - add attribute
      return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 2.1.2.4 SPECIFIED TABLE ENTRIES
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedJovialTraversal::traverse_SpecifiedEntrySpecifier(ATerm term, SgUntypedType** type, SgUntypedExprListExpression* attr_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SpecifiedEntrySpecifier: %s\n", ATwriteToString(term));
#endif

   return ATfalse;
}

//========================================================================================
// 2.1.5 ALLOCATION OF DATA OBJECTS
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedJovialTraversal::traverse_OptAllocationSpecifier(ATerm term, SgUntypedExprListExpression* attr_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptAllocationSpecifier: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "no-allocation-specifier()")) {
      // MATCHED no-allocation-specifier
   }
   else if (ATmatch(term, "STATIC()")) {
      SgUntypedExpression* attr = new SgUntypedOtherExpression(General_Language_Translation::e_storage_modifier_static);
      setSourcePosition(attr, term);
      attr_list->get_expressions().push_back(attr);
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 4.0 STATEMENTS
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedJovialTraversal::traverse_Statement(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Statement: %s\n", ATwriteToString(term));
#endif

   if (traverse_SimpleStatement(term, stmt_list)) {
      // MATCHED SimpleStatement
   } else if (traverse_CompoundStatement(term, stmt_list)) {
      // MATCHED CompoundStatement
   } else return ATfalse;

// TODO CompoundStatement

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_StatementList(ATerm term, SgUntypedStatementList* stmt_list)
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

ATbool ATermToUntypedJovialTraversal::traverse_SimpleStatement(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SimpleStatement: %s\n", ATwriteToString(term));
#endif

   ATerm t_labels, t_stmt;
   std::vector<std::string> labels;
   std::vector<PosInfo> locations;

   if (ATmatch(term, "SimpleStatement(<term>,<term>)", &t_labels,&t_stmt)) {
      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;

      if (traverse_AssignmentStatement(t_stmt, labels, stmt_list)) {
         // MATCHED AssignmentStatement
      }

      //  LoopStatement               -> SimpleStatement

      //%%ProcedureCallStatement      -> SimpleStatement  %%AMBIGUOUS with AssignmentStatement

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
      else return ATfalse;
   }

   else return ATfalse;

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_CompoundStatement(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CompoundStatement: %s\n", ATwriteToString(term));
#endif

   ATerm t_labels, t_stmt, t_labels2;
   std::vector<std::string> labels, labels2;
   std::vector<PosInfo> locations, locations2;
   SgUntypedStatementList* new_stmt_list = new SgUntypedStatementList();

   if (ATmatch(term, "CompoundStatement(<term>,<term>,<term>)", &t_labels,&t_stmt,&t_labels2)) {
      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;

      if (traverse_StatementList(t_stmt, new_stmt_list)) {
         // MATCHED StatementList
      } else return ATfalse;

      if (traverse_LabelList(t_labels2, labels2, locations2)) {
         // MATCHED LabelList
      } else return ATfalse;

   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_NullStatement(ATerm term, SgUntypedStatementList* stmt_list)
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

ATbool ATermToUntypedJovialTraversal::traverse_NullBlockStatement(ATerm term, SgUntypedStatementList* stmt_list)
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

ATbool ATermToUntypedJovialTraversal::traverse_LabelList(ATerm term, std::vector<std::string> & labels, std::vector<PosInfo> & locations)
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

   return ATtrue;
}

//========================================================================================
// 4.1 ASSIGNMENT STATEMENTS
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedJovialTraversal::traverse_AssignmentStatement(ATerm term, std::vector<std::string> & labels, SgUntypedStatementList* stmt_list)
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

      ROSE_ASSERT (labels.size() <= 1);
      ROSE_ASSERT (  vars.size() == 1);
      ROSE_ASSERT (expr);

   // TODO - need list for labels in untyped IR
      if (labels.size() == 1) temp_label = labels[0];

      SgUntypedAssignmentStatement* assign_stmt = new SgUntypedAssignmentStatement(temp_label,vars[0],expr);
      setSourcePosition(assign_stmt, term);

      stmt_list->get_stmt_list().push_back(assign_stmt);


   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 4.2 LOOP STATEMENTS
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedJovialTraversal::traverse_WhileStatement(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_WhileStatement: %s\n", ATwriteToString(term));
#endif

   ATerm t_labels, t_clause, t_stmt, t_formula;
   std::vector<std::string> labels;
   std::vector<PosInfo> locations;

   SgUntypedStatementList* while_body_list = NULL;
   SgUntypedExpression* condition = NULL;

   if (ATmatch(term, "WhileStatement(<term>,<term>,<term>)", &t_labels, &t_clause, &t_stmt)) {
      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;

      if (ATmatch(t_clause, "WhileClause(<term>)", &t_formula)) {
         // MATCHED WhileClause
         if (traverse_BitFormula(t_formula, &condition)) {
            // MATCHED BitFormula
         } else return ATfalse;
      }

      while_body_list = new SgUntypedStatementList();

      // Match ControlledStatement -- it is a Statement
      if (traverse_Statement(t_stmt, while_body_list)) {
         // MATCHED Statement
         cout << ".x. matched loop statement \n";
      } else return ATfalse;

      cout << ".x. loop body size is " << while_body_list->get_stmt_list().size() << endl;
      ROSE_ASSERT(while_body_list->get_stmt_list().size() > 0);
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

ATbool ATermToUntypedJovialTraversal::traverse_ForStatement(ATerm term, SgUntypedStatementList* stmt_list)
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

      if (traverse_ForClause(t_clause, &var_ref, &init, &phrase1, &phrase2, phrase1_enum, phrase2_enum)) {
         // MATCHED ForClause
      } else return ATfalse;

      for_body_list = new SgUntypedStatementList();

      // Match ControlledStatement which is a Statement
      if (traverse_Statement(t_stmt, for_body_list)) {
         // MATCHED Statement
      } else return ATfalse;
   }
   else return ATfalse;

   cout << ".x. loop body size is " << for_body_list->get_stmt_list().size() << endl;
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

ATbool ATermToUntypedJovialTraversal::traverse_ForClause(ATerm term, SgUntypedExpression** var_ref, SgUntypedExpression** init,
                                                                     SgUntypedExpression** phrase1, SgUntypedExpression** phrase2,
                                                                     int & phrase1_enum, int & phrase2_enum)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ForClause: %s\n", ATwriteToString(term));
#endif

   ATerm t_item, t_clause;
   char* name;

   *init = NULL;
   *var_ref = NULL;
   *phrase1 = NULL;
   *phrase2 = NULL;
   phrase1_enum = Jovial_ROSE_Translation::e_unknown;
   phrase2_enum = Jovial_ROSE_Translation::e_unknown;

   if (ATmatch(term, "ForClause(<term>,<term>)", &t_item, &t_clause)) {
      // MATCHED ForClause

      if (ATmatch(t_item, "<str>" , &name)) {
         // MATCHED ControlItem
         int expr_enum = Jovial_ROSE_Translation::e_referenceExpression;
         *var_ref = new SgUntypedReferenceExpression(expr_enum, name);
         ROSE_ASSERT(*var_ref);
         setSourcePosition(*var_ref, t_item);
      } else return ATfalse;

      if (traverse_ControlClause(t_clause, init, phrase1, phrase2, phrase1_enum, phrase2_enum)) {
         // MATCHED ControlClause
      } else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_ControlClause(ATerm term, SgUntypedExpression** initial_value,
                                                             SgUntypedExpression** phrase1, SgUntypedExpression** phrase2,
                                                             int & phrase1_enum, int & phrase2_enum)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ControlClause: %s\n", ATwriteToString(term));
#endif

   ATerm t_value, t_continuation;

   *initial_value = NULL;
   *phrase1 = NULL;
   *phrase2 = NULL;
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

   return ATtrue;

   }
   else return ATfalse;

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_OptContinuation(ATerm term, SgUntypedExpression** phrase1, SgUntypedExpression** phrase2,
                                                               int & phrase_enum1, int & phrase_enum2)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptContinuation: %s\n", ATwriteToString(term));
#endif

   *phrase1 = NULL;
   *phrase2 = NULL;
   phrase_enum1 = Jovial_ROSE_Translation::e_unknown;
   phrase_enum2 = Jovial_ROSE_Translation::e_unknown;

   if (ATmatch(term, "no-continuation")) {
   } else if (traverse_Continuation(term, phrase1, phrase2, phrase_enum1, phrase_enum2)) {
      // MATCHED Continuation
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_Continuation(ATerm term, SgUntypedExpression** phrase1, SgUntypedExpression** phrase2,
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

ATbool ATermToUntypedJovialTraversal::traverse_Phrase(ATerm term, SgUntypedExpression** expr, int & phrase_enum)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Phrase: %s\n", ATwriteToString(term));
#endif

   ATerm t_formula;

   *expr = NULL;
   phrase_enum = Jovial_ROSE_Translation::e_unknown;

   if (ATmatch(term, "ByPhrase(<term>)", &t_formula)) {
      // MATCHED ByPhrase
      if (traverse_NumericFormula(t_formula, expr)){
         // MATCHED NumericFormula
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
      if (traverse_BitFormula(t_formula, expr)){
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
ATbool ATermToUntypedJovialTraversal::traverse_IfStatement(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_IfStatement: %s\n", ATwriteToString(term));
#endif

   ATerm t_labels, t_cond, t_else, t_true, t_false;
   std::vector<std::string> labels;
   std::vector<PosInfo> locations;
   SgUntypedExpression* conditional;
   SgUntypedStatement *stmt, *true_body, *false_body;

   if (ATmatch(term, "IfStatement(<term>,<term>,<term>,<term>)", &t_labels,&t_cond,&t_true,&t_else)) {

      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;

      if (traverse_BitFormula(t_cond, &conditional)) {
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
ATbool ATermToUntypedJovialTraversal::traverse_CaseStatement(ATerm term, SgUntypedStatementList* stmt_list)
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

   int stmt_enum = General_Language_Translation::e_switch_stmt;

   if (ATmatch(term, "CaseStatement(<term>,<term>,<term>,<term>)", &t_labels, &t_formula, &t_case_body, &t_labels2)) {
      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;

      if (traverse_Formula(t_formula, &formula)) {
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

ATbool ATermToUntypedJovialTraversal::traverse_CaseBody(ATerm term, SgUntypedStatement** case_body)
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

ATbool ATermToUntypedJovialTraversal::traverse_CaseAlternative(ATerm term, SgUntypedStatementList* stmt_list)
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

      if (traverse_CaseIndexGroup(t_case_index_group, &case_index_group)) {
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

   ROSE_ASSERT(case_index_group != NULL);
   ROSE_ASSERT(body != NULL);

   SgUntypedCaseStatement* case_stmt = new SgUntypedCaseStatement("", stmt_enum, case_index_group, body, "", fall_thru);
   ROSE_ASSERT(case_stmt != NULL);
   setSourcePosition(case_stmt, term);

   stmt_list->get_stmt_list().push_back(case_stmt);

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_DefaultOption(ATerm term, SgUntypedStatementList* stmt_list)
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

ATbool ATermToUntypedJovialTraversal::traverse_CaseIndexGroup(ATerm term, SgUntypedExprListExpression** case_index_group)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CaseIndexGroup: %s\n", ATwriteToString(term));
#endif

   ATerm t_case_index;
   SgUntypedExpression* case_index;
   SgUntypedExprListExpression* index_group;

   *case_index_group = NULL;

   if (ATmatch(term, "CaseIndexGroup(<term>)", &t_case_index)) {
      index_group = new SgUntypedExprListExpression(General_Language_Translation::e_case_selector);
      setSourcePosition(index_group, term);

      ATermList tail = (ATermList) ATmake("<term>", t_case_index);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_CaseIndex(head, &case_index)) {
            // MATCHED CaseIndex
            ROSE_ASSERT(case_index);
            index_group->get_expressions().push_back(case_index);
         } else return ATfalse;
      }
   } else return ATfalse;

   ROSE_ASSERT(index_group != NULL);
   *case_index_group = index_group;

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_CaseIndex(ATerm term, SgUntypedExpression** case_index)
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

   *case_index = NULL;

   if (ATmatch(term, "CaseIndex(<term>)", &t_formula1)) {
     // This case is needed to traverse CompileTimeFormula -> CaseIndex
      if (traverse_Formula(t_formula1, &value)) {
         // MATCHED Formula
      } else return ATfalse;

   } else if (ATmatch(term, "CaseIndex(<term>,<term>)", &t_formula1, &t_formula2)) {
     // This case is needed to traverse LowerBound : UpperBound -> CaseIndex
      if (traverse_Formula(t_formula1, &lower_bound)) {
         // MATCHED Formula
      } else return ATfalse;
      if (traverse_Formula(t_formula2, &upper_bound)) {
         // MATCHED Formula
      } else return ATfalse;
   }
   else return ATfalse;

   if (value) {
      *case_index = value;
   }
   else if (lower_bound && upper_bound) {
      int expr_enum = General_Language_Translation::e_case_range;
      stride = UntypedBuilder::buildUntypedNullExpression();
      setSourcePositionUnknown(stride);
      range = new SgUntypedSubscriptExpression(expr_enum, lower_bound, upper_bound, stride);
      setSourcePosition(range, term);
      *case_index = range;
   }
   else {
      ROSE_ASSERT(0);
   }
   ROSE_ASSERT(*case_index);

   return ATtrue;
}

//========================================================================================
// 4.6 RETURN STATEMENTS
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedJovialTraversal::traverse_ReturnStatement(ATerm term, SgUntypedStatementList* stmt_list)
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
ATbool ATermToUntypedJovialTraversal::traverse_GotoStatement(ATerm term, SgUntypedStatementList* stmt_list)
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
ATbool ATermToUntypedJovialTraversal::traverse_ExitStatement(ATerm term, SgUntypedStatementList* stmt_list)
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
ATbool ATermToUntypedJovialTraversal::traverse_StopStatement(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_StopStatement: %s\n", ATwriteToString(term));
#endif

   ATerm t_labels, t_stop_code;
   std::vector<std::string> labels;
   std::vector<PosInfo> locations;
   SgUntypedExpression* stop_code = NULL;
   SgUntypedStatement* stmt;

   if (ATmatch(term, "StopStatement(<term>,<term>)", &t_labels, &t_stop_code)) {
      if (traverse_LabelList(t_labels, labels, locations)) {
         // MATCHED LabelList
      } else return ATfalse;

      if (ATmatch(t_stop_code, "no-integer-formula()")) {
         // No StopCode
         stop_code = UntypedBuilder::buildUntypedNullExpression();
      }
      else if (traverse_NumericFormula(t_stop_code, &stop_code)) {
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
ATbool ATermToUntypedJovialTraversal::traverse_AbortStatement(ATerm term, SgUntypedStatementList* stmt_list)
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
ATbool ATermToUntypedJovialTraversal::traverse_Formula(ATerm term, SgUntypedExpression** expr)
{
   if (traverse_NumericFormula(term, expr)) {
      // MATCHED NumericFormula
   } else if (traverse_BitFormula(term, expr)) {
      // MATCHED BitFormula
   } else if (traverse_CharacterFormula(term, expr)) {
      // MATCHED CharacterFormula
   } else if (traverse_StatusFormula(term, expr)) {
      // MATCHED StatusFormula
   } else if (traverse_PointerFormula(term, expr)) {
      // MATCHED PointerFormula
   } else return ATfalse;

   //  TableFormula                -> Formula

   return ATtrue;
}

//========================================================================================
// 5.1 NUMERIC FORMULAS
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedJovialTraversal::traverse_NumericFormula(ATerm term, SgUntypedExpression** expr)
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
      if (traverse_NumericTerm(t_expr, expr)) {
         // MATCHED NumericTerm
      } else return ATfalse;
      if (op_enum == General_Language_Translation::e_operator_unary_minus) {
        *expr = new SgUntypedUnaryOperator(op_enum, "-", *expr);
        setSourcePosition(*expr, t_sign);
      }
      else if (op_enum == General_Language_Translation::e_operator_unary_plus) {
        *expr = new SgUntypedUnaryOperator(op_enum, "+", *expr);
        setSourcePosition(*expr, t_sign);
      }
   }

   // NumericFormula PlusOrMinus NumericTerm -> NumericFormula
   //
   else if (ATmatch(term, "NumericFormula(<term>,<term>,<term>)", &t_lhs,&t_op,&t_rhs)) {
      std::string op_name;
      General_Language_Translation::ExpressionKind op_enum;
      SgUntypedExpression * lhs, * rhs;

      if (traverse_NumericFormula(t_lhs, &lhs)) {
         // MATCHED NumericFormula
      } else return ATfalse;

      if (ATmatch(t_op, "AddOp()")) {
         op_enum = General_Language_Translation::e_operator_add;
         op_name = "+";
      }
      else if (ATmatch(t_op, "SubtractOp()")) {
         op_enum = General_Language_Translation::e_operator_subtract;
         op_name = "-";
      } else return ATfalse;

      if (traverse_NumericTerm(t_rhs, &rhs)) {
         // MATCHED NumericTerm
      } else return ATfalse;

      *expr = new SgUntypedBinaryOperator(op_enum,op_name,lhs,rhs);
      setSourcePosition(*expr, term);
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 5.1.1 INTEGER FORMULAS
//----------------------------------------------------------------------------------------
#if DELETE_ME
ATbool ATermToUntypedJovialTraversal::traverse_IntegerFormula(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_IntegerFormula: %s\n", ATwriteToString(term));
#endif

   ATerm t_sign, t_expr, t_lhs, t_op, t_rhs;

   if (ATmatch(term, "IntegerFormula(<term>,<term>)", &t_sign,&t_expr)) {
      General_Language_Translation::ExpressionKind op_enum;

      // OptSign IntegerTerm -> IntegerFormula

      if (traverse_OptSign(t_sign, op_enum)) {
         // MATCHED OptSign
      } else return ATfalse;

      if (traverse_IntegerTerm(t_expr, expr)) {
         // MATCHED NumericTerm
      } else return ATfalse;

      if (op_enum == General_Language_Translation::e_operator_unary_minus) {
        *expr = new SgUntypedUnaryOperator(op_enum, "-", *expr);
        setSourcePosition(*expr, t_sign);
      }
      else if (op_enum == General_Language_Translation::e_operator_unary_plus) {
        *expr = new SgUntypedUnaryOperator(op_enum, "+", *expr);
        setSourcePosition(*expr, t_sign);
      }
   }

   else if (ATmatch(term, "IntegerFormula(<term>,<term>,<term>)", &t_lhs,&t_op,&t_rhs)) {

      // IntegerFormula PlusOrMinus NumericTerm -> IntegerFormula

      std::string op_name;
      General_Language_Translation::ExpressionKind op_enum;
      SgUntypedExpression * lhs, * rhs;

      if (traverse_IntegerFormula(t_lhs, &lhs)) {
         // MATCHED IntegerFormula
      } else return ATfalse;

      if (ATmatch(t_op, "PLUS()")) {
         op_enum = General_Language_Translation::e_operator_add;
         op_name = "+";
      }
      else if (ATmatch(t_op, "MINUS()")) {
         op_enum = General_Language_Translation::e_operator_subtract;
         op_name = "-";
      } else return ATfalse;

      if (traverse_IntegerTerm(t_rhs, &rhs)) {
         // MATCHED IntegerTerm
      } else return ATfalse;

      *expr = new SgUntypedBinaryOperator(op_enum,op_name,lhs,rhs);
      setSourcePosition(*expr, term);
   }

   else return ATfalse;

   return ATtrue;
}
#endif

ATbool ATermToUntypedJovialTraversal::traverse_NumericPrimary(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_NumericPrimary: %s\n", ATwriteToString(term));
#endif

   ATerm t_table_item;
   char *literal, *name, *variable;
   SgUntypedType* type;
   SgUntypedExpression* table_item;
   Jovial_ROSE_Translation::ExpressionKind expr_enum = Jovial_ROSE_Translation::e_referenceExpression;

   if (ATmatch(term, "IntegerLiteral(<str>)", &literal)) {
      type = UntypedBuilder::buildType(SgUntypedType::e_int);
      expr_enum = Jovial_ROSE_Translation::e_literalExpression;
      *expr = new SgUntypedValueExpression(expr_enum,literal,type);
      setSourcePosition(*expr, term);
   }

   // NumericMachineParameter -> NumericPrimary

   else if (ATmatch(term, "NumericVariable(<str>)", &variable)) {
      expr_enum = Jovial_ROSE_Translation::e_referenceExpression;
      *expr = new SgUntypedReferenceExpression(expr_enum, variable);
      setSourcePosition(*expr, term);
   }

   else if (ATmatch(term, "IntegerVariable(<term>)", &t_table_item)) {
      if (traverse_TableItem(t_table_item, &table_item)){
         //MATCHED TableItem
      }
      //      expr_enum = Jovial_ROSE_Translation::e_referenceExpression;
      //      *expr = new SgUntypedReferenceExpression(expr_enum, table_item);
      //      setSourcePosition(*expr, term);
   }

   else if (ATmatch(term, "<str>" , &name)) {
      expr_enum = Jovial_ROSE_Translation::e_referenceExpression;
      *expr = new SgUntypedReferenceExpression(expr_enum, name);
      setSourcePosition(*expr, term);
   } else return ATfalse;

   //  NamedIntegerConstant        -> NumericPrimary
   //  IntegerFunctionCall         -> NumericPrimary
   //  '(' IntegerFormula ')'      -> NumericPrimary         {cons("NumericPrimary")}
   //  IntegerConversion
   //    '(' Formula ')'           -> NumericPrimary         {cons("IntegerPrimary")}

   return ATtrue;
}

#if DELETE_ME
ATbool ATermToUntypedJovialTraversal::traverse_IntegerPrimary(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_IntegerPrimary: %s\n", ATwriteToString(term));
#endif

   ATerm t_table_item;
   char *literal, *name, *variable;
   SgUntypedType* type;
   SgUntypedExpression* table_item;
   Jovial_ROSE_Translation::ExpressionKind expr_enum = Jovial_ROSE_Translation::e_referenceExpression;

   if (ATmatch(term, "IntegerLiteral(<str>)", &literal)) {
      type = UntypedBuilder::buildType(SgUntypedType::e_int);
      expr_enum = Jovial_ROSE_Translation::e_literalExpression;
      *expr = new SgUntypedValueExpression(expr_enum,literal,type);
      setSourcePosition(*expr, term);
      std::cout << "INTEGER LITERAL is " << literal << " type (not deleted) is " << type << "\n";
   }

   //  IntegerMachineParameter     -> IntegerPrimary

   else if (ATmatch(term, "IntegerVariable(<str>)", &variable)) {
      expr_enum = Jovial_ROSE_Translation::e_referenceExpression;
      *expr = new SgUntypedReferenceExpression(expr_enum, variable);
      setSourcePosition(*expr, term);
   }

   else if (ATmatch(term, "IntegerVariable(<term>)", &t_table_item)) {
      if (traverse_TableItem(t_table_item, &table_item)){
         //MATCHED TableItem
      }
      //      expr_enum = Jovial_ROSE_Translation::e_referenceExpression;
      //      *expr = new SgUntypedReferenceExpression(expr_enum, table_item);
      //      setSourcePosition(*expr, term);
   }

   else if (ATmatch(term, "<str>" , &name)) {
      expr_enum = Jovial_ROSE_Translation::e_referenceExpression;
      *expr = new SgUntypedReferenceExpression(expr_enum, name);
      setSourcePosition(*expr, term);
   } else return ATfalse;

   //  NamedIntegerConstant        -> IntegerPrimary
   //  IntegerFunctionCall         -> IntegerPrimary
   //  '(' IntegerFormula ')'      -> IntegerPrimary         {cons("IntegerPrimary")}
   //  IntegerConversion
   //    '(' Formula ')'           -> IntegerPrimary         {cons("IntegerPrimary")}

   return ATtrue;
}
#endif

ATbool ATermToUntypedJovialTraversal::traverse_OptSign(ATerm term, General_Language_Translation::ExpressionKind & op_enum)
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

ATbool ATermToUntypedJovialTraversal::traverse_NumericTerm(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_NumericTerm: %s\n", ATwriteToString(term));
#endif

   ATerm t_lhs, t_op, t_rhs;
   std::string op_name;
   General_Language_Translation::ExpressionKind op_enum;
   SgUntypedExpression * lhs, * rhs;

   if (ATmatch(term, "NumericTerm(<term>,<term>,<term>)", &t_lhs,&t_op,&t_rhs)) {
      if (traverse_NumericTerm(t_lhs, &lhs)) {
         // MATCHED NumericTerm
      } else return ATfalse;

      if (traverse_MultiplyDivideOrMod(t_op, op_enum, op_name)) {
         // MATCHED MultiplyDivideOrMod
      } else return ATfalse;

      if (traverse_NumericFactor(t_rhs, &rhs)) {
         // MATCHED NumericTerm
      } else return ATfalse;

      *expr = new SgUntypedBinaryOperator(op_enum,op_name,lhs,rhs);
      setSourcePosition(*expr, term);
   }
   else if (traverse_NumericFactor(term, expr)) {
         // MATCHED NumericFactor
   }
   else return ATfalse;

   return ATtrue;
}

#if DELETE_ME
ATbool ATermToUntypedJovialTraversal::traverse_IntegerTerm(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_IntegerTerm: %s\n", ATwriteToString(term));
#endif

   ATerm t_lhs, t_op, t_rhs;
   std::string op_name;
   General_Language_Translation::ExpressionKind op_enum;
   SgUntypedExpression * lhs, * rhs;

   if (ATmatch(term, "NumericTerm(<term>,<term>,<term>)", &t_lhs,&t_op,&t_rhs)) {
      if (traverse_NumericTerm(t_lhs, &lhs)) {
         // MATCHED NumericTerm
      } else return ATfalse;

      if (traverse_MultiplyDivideOrMod(t_op, op_enum, op_name)) {
         // MATCHED MultiplyDivideOrMod
      } else return ATfalse;

      if (traverse_IntegerFactor(t_rhs, &rhs)) {
         // MATCHED NumericTerm
      } else return ATfalse;

      *expr = new SgUntypedBinaryOperator(op_enum,op_name,lhs,rhs);
      setSourcePosition(*expr, term);
   }
   else if (traverse_IntegerFactor(term, expr)) {
         // MATCHED IntegerFactor
   }
   else return ATfalse;

   return ATtrue;
}
#endif

ATbool ATermToUntypedJovialTraversal::traverse_NumericFactor(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_NumericFactor: %s\n", ATwriteToString(term));
#endif

   ATerm t_lhs, t_rhs;
   SgUntypedExpression * lhs, * rhs;

   if (ATmatch(term, "NumericFactor(<term>,<term>)", &t_lhs,&t_rhs)) {
      if (traverse_NumericFactor(t_lhs, &lhs)) {
         // MATCHED NumericFactor
      } else return ATfalse;

      if (traverse_NumericPrimary(t_rhs, &rhs)) {
         // MATCHED NumericPrimary
      } else return ATfalse;

      // TODO - create the expression
      // expr = new SgUntypedExpression()
   }

   else if (traverse_NumericPrimary(term, expr)) {
      // MATCHED NumericPrimary
   }

   else return ATfalse;

   return ATtrue;
}

#if DELETE_ME
ATbool ATermToUntypedJovialTraversal::traverse_IntegerFactor(ATerm term, SgUntypedExpression** expr)
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
#endif

//========================================================================================
// 5.1.2 FLOATING FORMULAS
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedJovialTraversal::traverse_FloatingFormula(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FloatingFormula: %s\n", ATwriteToString(term));
#endif

   ATerm t_sign, t_expr;
#if 0
   ATerm t_lhs, t_op, t_rhs;
#endif

   if (ATmatch(term, "FloatingFormula(<term>,<term>)", &t_sign,&t_expr)) {
      General_Language_Translation::ExpressionKind op_enum;

      // OptSign FloatingTerm -> FloatingFormula

      if (traverse_OptSign(t_sign, op_enum)) {
         // MATCHED OptSign
      } else return ATfalse;

      if (traverse_FloatingTerm(t_expr, expr)) {
         // MATCHED FloatingTerm
      } else return ATfalse;

      if (op_enum == General_Language_Translation::e_operator_unary_minus) {
        *expr = new SgUntypedUnaryOperator(op_enum, "-", *expr);
        setSourcePosition(*expr, t_sign);
      }
      else if (op_enum == General_Language_Translation::e_operator_unary_plus) {
        *expr = new SgUntypedUnaryOperator(op_enum, "+", *expr);
        setSourcePosition(*expr, t_sign);
      }
   }

#if 0
   else if (ATmatch(term, "FloatingFormula(<term>,<term>,<term>)", &t_lhs,&t_op,&t_rhs)) {

      // FloatingFormula PlusOrMinus FloatingTerm -> FloatingFormula

      std::string op_name;
      General_Language_Translation::ExpressionKind op_enum;
      SgUntypedExpression * lhs, * rhs;

      if (traverse_FloatingFormula(t_lhs, &lhs)) {
         // MATCHED FloatingFormula
      } else return ATfalse;

      if (ATmatch(t_op, "PLUS()")) {
         op_enum = General_Language_Translation::e_operator_add;
         op_name = "+";
      }
      else if (ATmatch(t_op, "MINUS()")) {
         op_enum = General_Language_Translation::e_operator_subtract;
         op_name = "-";
      } else return ATfalse;

      if (traverse_FloatingTerm(t_rhs, &rhs)) {
         // MATCHED FloatingTerm
      } else return ATfalse;

      *expr = new SgUntypedBinaryOperator(op_enum,op_name,lhs,rhs);
      setSourcePosition(*expr, term);
   }
#endif

   else return ATfalse;

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_FloatingTerm(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FloatingTerm: %s\n", ATwriteToString(term));
#endif

#if 0
   ATerm t_lhs, t_op, t_rhs;
   std::string op_name;
   General_Language_Translation::ExpressionKind op_enum;
   SgUntypedExpression * lhs, * rhs;

   if (ATmatch(term, "FloatingTerm(<term>,<term>,<term>)", &t_lhs,&t_op,&t_rhs)) {
      if (traverse_NumericTerm(t_lhs, &lhs)) {
         // MATCHED NumericTerm
      } else return ATfalse;

      if (traverse_MultiplyDivideOrMod(t_op, op_enum, op_name)) {
         // MATCHED MultiplyDivideOrMod
      } else return ATfalse;

      if (traverse_IntegerFactor(t_rhs, &rhs)) {
         // MATCHED NumericTerm
      } else return ATfalse;

      *expr = new SgUntypedBinaryOperator(op_enum,op_name,lhs,rhs);
      setSourcePosition(*expr, term);
   }
   else if FloatingFactor
#endif

   if (traverse_FloatingFactor(term, expr)) {
         // MATCHED FloatingFactor
   }

   else return ATfalse;

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_FloatingFactor(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FloatingFactor: %s\n", ATwriteToString(term));
#endif

#if 0
   ATerm t_lhs, t_rhs;
   SgUntypedExpression * lhs, * rhs;

   if (ATmatch(term, "FloatingFactor(<term>,<term>)", &t_lhs,&t_rhs)) {
      if (traverse_FloatingFactor(t_lhs, &lhs)) {
         // MATCHED FloatingFactor
      } else return ATfalse;

      if (traverse_FloatingPrimary(t_rhs, &rhs)) {
         // MATCHED FloatingPrimary
      } else return ATfalse;

      // TODO - create the expression
      // expr = new SgUntypedExpression()
   }
   else if FloatingPrimary
#endif

   if (traverse_FloatingPrimary(term, expr)) {
      // MATCHED FloatingPrimary
   }

   else return ATfalse;

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_FloatingPrimary(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FloatingPrimary: %s\n", ATwriteToString(term));
#endif

   if (traverse_FloatingLiteral(term, expr)) {
      // MATCHED FloatingLiteral
   }

#if 0
   // TODO - convert this to floating type
   //  IntegerMachineParameter     -> IntegerPrimary
   //  IntegerVariable             -> IntegerPrimary
   else if (ATmatch(term, "<str>" , &name)) {
      std::cout << "VARIABLE " << name << "\n";
      expr_enum = Jovial_ROSE_Translation::e_referenceExpression;
      *expr = new SgUntypedReferenceExpression(expr_enum, name);
      setSourcePosition(*expr, term);
   }

   //  NamedIntegerConstant        -> IntegerPrimary
   //  IntegerFunctionCall         -> IntegerPrimary
   //  '(' IntegerFormula ')'      -> IntegerPrimary         {cons("IntegerPrimary")}
   //  IntegerConversion
   //    '(' Formula ')'           -> IntegerPrimary         {cons("IntegerPrimary")}
#endif

   else return ATfalse;

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_FloatingLiteral(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FloatingLiteral: %s\n", ATwriteToString(term));
#endif

   ATerm t_frac_form, t_num1, t_num2, t_opt_exp;
   std::string literal;
   char* number;

   SgUntypedType* type;
   Jovial_ROSE_Translation::ExpressionKind expr_enum = Jovial_ROSE_Translation::e_literalExpression;

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

#if 0
         // TODO add exponent
         else if (traverse_Exponent(t_opt_exp, ...)) {
            literal += ...;
         }
#endif
         else return ATfalse;
      }

      if (literal == ".") {
         cerr << "ERROR in traverse_FloatingLiteral, no literal, contains only: " << literal << endl;
         return ATfalse;
      }

      type = UntypedBuilder::buildType(SgUntypedType::e_float);
      *expr = new SgUntypedValueExpression(expr_enum,literal,type);
      cout << "FLOATING LITERAL is " << literal << endl;
      setSourcePosition(*expr, term);
   }

#if 0
// Icon Exponent              -> RealLiteral

   else if (ATmatch(term, "RealLiteralIE(<str>,<term>)", &literal)) {
      type = UntypedBuilder::buildType(SgUntypedType::e_float);
      expr_enum = Jovial_ROSE_Translation::e_literalExpression;
      *expr = new SgUntypedValueExpression(expr_enum,literal,type);
      std::cout << "REAL LITERAL is " << literal << "\n";
      setSourcePosition(*expr, term);
   }
#endif

   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 5.1.3 FIXED FORMULAS
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedJovialTraversal::traverse_FixedFormula(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FixedFormula: %s\n", ATwriteToString(term));
#endif

   *expr = NULL;

   cerr << "FIXED Formula expressions are not yet implemented! \n";

   return ATfalse;
}

//========================================================================================
// 5.2 BIT FORMULAS
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedJovialTraversal::traverse_BitFormula(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BitFormula: %s\n", ATwriteToString(term));
#endif

   ATerm t_operand, t_continuation;
   SgUntypedExpression* continuation;

   *expr = NULL;
   if (ATmatch(term, "BitFormula(<term>,<term>)", &t_operand, &t_continuation)) {
      if (traverse_LogicalOperand(t_operand, expr)) {
         // MATCHED LogicalOperand
      } else return ATfalse;

      if (traverse_OptLogicalContinuation(t_continuation, &continuation)) {
         // MATCHED OptLogicalContinuation
      // TODO
         ROSE_ASSERT(continuation == NULL);
      } else return ATfalse;

   } else if (ATmatch(term, "BitFormulaNOT(<term>)", &t_operand)) {
      if (traverse_LogicalOperand(t_operand, expr)) {
         // MATCHED LogicalOperand
      } else return ATfalse;

   } else return ATfalse;

   ROSE_ASSERT(*expr != NULL);

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_OptLogicalContinuation(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptLogicalContinuation: %s\n", ATwriteToString(term));
#endif

   *expr = NULL;
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

ATbool ATermToUntypedJovialTraversal::traverse_LogicalContinuation(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_LogicalContinuation: %s\n", ATwriteToString(term));
#endif

   ATerm t_operand;

   *expr = NULL;

   if (ATmatch(term, "AndContinuation(<term>)", &t_operand)) {
      std::cout << "Matched AndContinuation" << endl;
      if (traverse_LogicalOperand(t_operand, expr)) {
      // MATCHED LogicalOperand
      } else return ATfalse;
   } else if (ATmatch(term, " OrContinuation(<term>)", &t_operand)) {
      std::cout << "Matched OrContinuation" << endl;
      if (traverse_LogicalOperand(t_operand, expr)) {
      // MATCHED LogicalOperand
      } else return ATfalse;
   } else if (ATmatch(term, "XorContinuation(<term>)", &t_operand)) {
      std::cout << "Matched XorContinuation" << endl;
      if (traverse_LogicalOperand(t_operand, expr)) {
      // MATCHED LogicalOperand
      } else return ATfalse;
   } else if (ATmatch(term, "EqvContinuation(<term>)", &t_operand)) {
      std::cout << "Matched EqvContinuation" << endl;
      if (traverse_LogicalOperand(t_operand, expr)) {
      // MATCHED LogicalOperand
      } else return ATfalse;
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_LogicalOperand(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_LogicalOperand: %s\n", ATwriteToString(term));
#endif

   *expr = NULL;
   if (traverse_BitPrimary(term, expr)) {
      // MATCHED BitPrimary
   } else if (traverse_RelationalExpression(term, expr)) {
      // MATCHED RelationalExpression
   } else return ATfalse;

   ROSE_ASSERT(*expr != NULL);

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_BitPrimary(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BitPrimary: %s\n", ATwriteToString(term));
#endif

   ATerm t_bit;

   *expr = NULL;
   if (traverse_BooleanLiteral(term, expr)) {
      // MATCHED BooleanLiteral
   } else if (ATmatch(term,"BitPrimary(<term>)", &t_bit)) {
      if (traverse_BitFormula(t_bit, expr)) {
      // MATCHED '(' BitFormula ')'
      } else return ATfalse;
   } else return ATfalse;
      // TODO: create else if for following
      // BitLiteral                    -> BitPrimary (no cons)
      // BitVariable                   -> BitPrimary {cons("BitVariable")} (not currently working in tests)
      // NamedBitConstant              -> BitPrimary {cons("NamedBitConstant")} (rejected in grammar)
      // BitFunctionCall               -> BitPrimary (no cons)
      // BitConversion '(' Formula ')' -> BitPrimary {cons("BitPrimary")}

   ROSE_ASSERT(*expr != NULL);

   return ATtrue;
}

//========================================================================================
// 5.2.1 RELATIONAL EXPRESSIONS
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedJovialTraversal::traverse_RelationalExpression(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_RelationalExpression: %s\n", ATwriteToString(term));
#endif

   ATerm t_formula, t_operator, t_formula2;
   std::string op_name;
   General_Language_Translation::ExpressionKind op_enum;
   SgUntypedExpression * expr1, * expr2;

   *expr = NULL;

   if (ATmatch(term, "RelationalExpression(<term>,<term>,<term>)", &t_formula, &t_operator, &t_formula2)) {
      if (traverse_Formula(t_formula, &expr1)) {
         // MATCHED Formula
      } else return ATfalse;

      if (traverse_RelationalOperator(t_operator, op_enum, op_name)) {
         // MATCHED RelationalOperator
      } else return ATfalse;

      if (traverse_Formula(t_formula2, &expr2)) {
         // MATCHED Formula
      } else return ATfalse;
   } else return ATfalse;

   ROSE_ASSERT(expr1);
   ROSE_ASSERT(expr2);

   *expr = new SgUntypedBinaryOperator(op_enum, op_name, expr1, expr2);
   ROSE_ASSERT(*expr);
   setSourcePosition(*expr, term);

   return ATtrue;
}

//========================================================================================
// 5.3 CHARACTER FORMULAS
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedJovialTraversal::traverse_CharacterFormula(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CharacterFormula: %s\n", ATwriteToString(term));
#endif

   ATerm t_next , t_formula;

   *expr = NULL;

   if (ATmatch(term, "CharacterFormula(<term>)", &t_next)) {
      if (traverse_CharacterLiteral(t_next, expr)) {
         // CharacterLiteral -> CharacterFormula
         // MATCHED CharacterLiteral
      }
      else if (traverse_CharacterFormula(t_next, expr)) {
         // '(' CharacterFormula ')' -> CharacterFormula
         // MATCHED CharacterFormula
      } else return ATfalse;

   } else if (ATmatch(term, "CharacterFormula(<term>,<term>)", &t_next, &t_formula)) {
      if (traverse_CharacterConversion(t_next, expr)) {
         // CharacterConversion '(' Formula ')'  ->  CharacterFormula
         // MATCHED CharacterConversion
      } else if (traverse_CharacterConversionC(t_next, expr)) {
         // MATCHED CharacterConversionC
      } else return ATfalse;

      if (traverse_Formula(t_formula, expr)) {
         // MATCHED Formula
      }
   } else return ATfalse;

   std::cout << "Got through CharacterFormula" << endl;

   return ATtrue;
}

//========================================================================================
// 5.4 STATUS FORMULAS
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedJovialTraversal::traverse_StatusFormula(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_StatusFormula: %s\n", ATwriteToString(term));
#endif

   ATerm t_next , t_formula;

   *expr = NULL;

   if (ATmatch(term, "StatusFormula(<term>)", &t_next)) {
      if (traverse_StatusConstant(t_next, expr)) {
         // StatusConstant -> StatusFormula
         // MATCHED StatusConstant
      }
      else if (traverse_StatusFormula(t_next, expr)) {
         // '(' StatusFormula ')' -> StatusFormula
         // MATCHED StatusFormula
      } else return ATfalse;

   } else if (ATmatch(term, "StatusFormula(<term>,<term>)", &t_next, &t_formula)) {
      if (traverse_StatusConversion(t_next, expr)) {
         // StatusConversion '(' Formula ')'  ->  StatusFormula
         // MATCHED StatusConversion
      }
      if (traverse_Formula(t_formula, expr)) {
         // MATCHED Formula
      }
   } else return ATfalse;

   std::cout << "Got through StatusFormula" << endl;

   return ATtrue;
}

//========================================================================================
// 5.5 POINTER FORMULAS
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedJovialTraversal::traverse_PointerFormula(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_PointerFormula: %s\n", ATwriteToString(term));
#endif

   ATerm t_next, t_formula;

   *expr = NULL;

   if (ATmatch(term, "PointerFormula(<term>)", &t_next)) {
      if (traverse_PointerLiteral(t_next, expr)) {
         // PointerLiteral -> PointerFormula
         // MATCHED PointerLiteral
      }
      else if (traverse_PointerFormula(t_next, expr)) {
         // '(' PointerFormula ')' -> PointerFormula
         // MATCHED PointerFormula
      } else return ATfalse;

   } else if (ATmatch(term, "PointerFormula(<term>,<term>)", &t_next, &t_formula)) {
      if (traverse_PointerConversion(t_next, expr)) {
         // PointerConversion '(' Formula ')'  -> PointerFormula
         // MATCHED PointerConversion
      } else if (traverse_PointerConversionP(t_next, expr)) {
         // MATCHED PointerConversionP
      } else return ATfalse;

      if (traverse_Formula(t_formula, expr)) {
         // MATCHED Formula
      }
   } else return ATfalse;

   std::cout << "Got through PointerFormula" << endl;

   return ATtrue;
}

//========================================================================================
// 6.1 VARIABLE AND BLOCK REFERENCES
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedJovialTraversal::traverse_Variable(ATerm term, SgUntypedExpression** var)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Variable: %s\n", ATwriteToString(term));
#endif

   char* name;
   Jovial_ROSE_Translation::ExpressionKind expr_enum = Jovial_ROSE_Translation::e_referenceExpression;

   if (ATmatch(term, "<str>" , &name)) {
      // MATCHED NamedVariable
      *var = new SgUntypedReferenceExpression(expr_enum, name);
      setSourcePosition(*var, term);

   } else if (traverse_TableItem(term, var)) {
     // MATCHED TableItem
   }
   else return ATfalse;

   //  BitFunctionVariable         -> Variable           {cons("BitFunctionVariable")}
   //  ByteFunctionVariable        -> Variable           {cons("ByteFunctionVariable")}
   //  RepFunctionVariable         -> Variable           {cons("RepFunctionVariable")}

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_VariableList(ATerm term, std::vector<SgUntypedExpression*> & vars)
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

ATbool ATermToUntypedJovialTraversal::traverse_TableItem(ATerm term, SgUntypedExpression** var)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_TableItem: %s\n", ATwriteToString(term));
#endif

   ATerm t_name, t_subscript, t_tblderef;
   char* name;
   std::vector<SgUntypedExpression*> subscript;

   if (ATmatch(term, "TableItem<term>,<term>,<term>" , &t_name, &t_subscript, &t_tblderef)) {
      if (ATmatch(t_name, "<str>" , &name)) {
         // MATCHED TableItemName
      } else if (traverse_Subscript(t_subscript, subscript)) {
         // MATCHED Subscript
      } else if (traverse_TableDereference(t_tblderef, var)) {
         // MATCHED TableDereference
      } else return ATfalse;
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_Subscript(ATerm term, std::vector<SgUntypedExpression*> & indexes)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Subscript: %s\n", ATwriteToString(term));
#endif

   ATerm t_index;
   SgUntypedExpression* index;

   if (ATmatch(term, "no-subscript")) {
      // MATCHED no-subscript
   } else if (ATmatch(term, "Subscript(<term>)" , &t_index)) {
      ATermList tail = (ATermList) ATmake("<term>", t_index);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_Index(head, &index)) {
            // MATCHED Index
            indexes.push_back(index);
         } else return ATfalse;
      }
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_Index(ATerm term, SgUntypedExpression** formula)
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


ATbool ATermToUntypedJovialTraversal::traverse_TableDereference(ATerm term, SgUntypedExpression** formula)
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

ATbool ATermToUntypedJovialTraversal::traverse_Dereference(ATerm term, SgUntypedExpression** formula)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Dereference: %s\n", ATwriteToString(term));
#endif

   ATerm t_deref;
   char* name;
   //   SgUntypedExpression* formula;

   if (ATmatch(term, "Dereference<term>", &t_deref)) {
      if (ATmatch(t_deref, "<str>", &name)) {
         // MATCHED PointerItemName
   } else if (traverse_PointerFormula(t_deref, formula)) {
         // MATCHED PointerFormula
      } else return ATfalse;
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 7.0 TYPE MATCHING AND TYPE CONVERSIONS
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedJovialTraversal::traverse_CharacterConversion(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CharacterConversion: %s\n", ATwriteToString(term));
#endif

   ATerm t_next;
   SgUntypedType* type; // TODO - QUICK DO SOMETHING!

   *expr = NULL;

   if (ATmatch(term, "CharacterConversion(<term>)", &t_next)) {
      if (traverse_CharacterItemDescription(t_next, &type)) {
         // MATCHED CharacterItemDescription
      } else return ATfalse;
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_CharacterConversionC(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CharacterConversionC: %s\n", ATwriteToString(term));
#endif

   *expr = NULL;

   if (ATmatch(term, "CharacterConversionC()")) {
     std::cout << "Matched CharacterConversionC" << endl;
     // MATCHED CharacterConversionC
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_StatusConversion(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_StatusConversion: %s\n", ATwriteToString(term));
#endif

   ATerm t_next;
   SgUntypedType* type; // TODO - QUICK DO SOMETHING!

   *expr = NULL;

   if (ATmatch(term, "StatusConversion(<term>)", &t_next)) {
      if (traverse_StatusItemDescription(t_next, &type)) {
         // MATCHED StatusItemDescription
      } else return ATfalse;
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_PointerConversion(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_PointerConversion: %s\n", ATwriteToString(term));
#endif

   ATerm t_next;
   SgUntypedType* type; // TODO - QUICK DO SOMETHING!

   *expr = NULL;

   if (ATmatch(term, "PointerConversion(<term>)", &t_next)) {
      if (traverse_PointerItemDescription(t_next, &type)) {
         // MATCHED PointerItemDescription
      } else return ATfalse;
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToUntypedJovialTraversal::traverse_PointerConversionP(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_PointerConversionP: %s\n", ATwriteToString(term));
#endif

   *expr = NULL;

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
ATermToUntypedJovialTraversal::traverse_MultiplyDivideOrMod(ATerm term, General_Language_Translation::ExpressionKind & op_enum, std::string & op_name)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_MultiplyDivideOrMod: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "TIMES()")) {
      op_enum = General_Language_Translation::e_operator_multiply;
      op_name = "*";
   }
   else if (ATmatch(term, "DIV()")) {
      op_enum = General_Language_Translation::e_operator_divide;
      op_name = "/";
   }
   else if (ATmatch(term, "MOD()")) {
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

ATbool ATermToUntypedJovialTraversal::traverse_RelationalOperator(ATerm term, General_Language_Translation::ExpressionKind & op_enum, std::string & op_name)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_RelationalOperator: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "LessThanOp()")) {
      op_enum = General_Language_Translation::e_operator_less_than;
      op_name = "<";
   }
   else if (ATmatch(term, "GT()")) {
      std::cout << "Matched GT: > " << endl;
      op_enum = General_Language_Translation::e_operator_greater_than;
      op_name = ">";
   }
   else if (ATmatch(term, "LE()")) {
      std::cout << "Matched LE: <= " << endl;
      op_enum = General_Language_Translation::e_operator_less_than_or_equal;
      op_name = "<=";
   }
   else if (ATmatch(term, "GE()")) {
      std::cout << "Matched GE: >= " << endl;
      op_enum = General_Language_Translation::e_operator_greater_than_or_equal;
      op_name = ">=";
   }
   else if (ATmatch(term, "EqOp()")) {
      std::cout << "Matched EqOp: = " << endl;
      op_enum = General_Language_Translation::e_operator_equality;
      op_name = "==";
      // Should this be the character used in Jovial or in the general langauges?
      //      op_name = "=";
   }
   else if (ATmatch(term, "NotEqOp()")) {
      std::cout << "Matched NotEqOp: <> " << endl;
      op_enum = General_Language_Translation::e_operator_not_equal;
      // Should this be the character used in Jovial or in the general langauges?
      op_name = "!=";
      //      op_name = "<>";
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 8.3.3 BOOLEAN LITERAL
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedJovialTraversal::traverse_BooleanLiteral(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BooleanLiteral: %s\n", ATwriteToString(term));
#endif

   std::string value;

   *expr = NULL;
   if (ATmatch(term, "True()")) {
      // MATCHED TRUE
      value += "TRUE";
      std::cout << "Matched TRUE" << endl;
   } else if (ATmatch(term, "False()")) {
      // MATCHED FALSE
      value += "FALSE";
      std::cout << "Matched FALSE" << endl;
   } else return ATfalse;

   int expression_enum = General_Language_Translation::e_literalExpression;
   SgUntypedType* type = UntypedBuilder::buildType(SgUntypedType::e_bool);

   *expr = new SgUntypedValueExpression(expression_enum, value, type);
   ROSE_ASSERT(*expr != NULL);
   setSourcePosition(*expr, term);

   return ATtrue;
}

//========================================================================================
// 8.3.4 POINTER LITERAL
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedJovialTraversal::traverse_PointerLiteral(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_PointerLiteral: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "Null()")) {
     std::cout << "Matched Null()" << endl;
     // MATCHED Null
   } else return ATfalse;

   return ATtrue;
}
