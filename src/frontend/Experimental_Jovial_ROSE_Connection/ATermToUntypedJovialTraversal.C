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
#if 0
   else if (traverse_StatusItemDescription(term, type)) {
      // MATCHED StatusItemDescription
   }
   else if (traverse_PointerItemDescription(term, type)) {
      // MATCHED PointerItemDescription
   }
#endif
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
      if (traverse_IntegerFormula(t_size, size)) {
        // MATCHED IntegerFormula
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
   bool has_round_or_truncate, has_fraction_specifier;
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

   if (traverse_IntegerFormula(t_scale, &scale)) {
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

   std::cout << "TABLE DECLARATION " << name << ", rank is " << dim_info->get_expressions().size() << "\n";

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
         lower_bound = new SgUntypedNullExpression();
         setSourcePositionUnknown(lower_bound);
      }
      else if (ATmatch(t_opt_lower_bound, "LowerBoundOption(<term>)", &t_lower_bound)) {
         if (traverse_IntegerFormula(t_lower_bound, &lower_bound)) {
            // MATCHED IntegerFormula
         } else return ATfalse;
      } else return ATfalse;

   // Upper bound
      if (traverse_IntegerFormula(t_upper_bound, &upper_bound)) {
         // MATCHED IntegerFormula
      } else return ATfalse;
   }
   else if (ATmatch(term, "DimensionSTAR()")) {
      expr_enum = General_Language_Translation::e_star_dimension;

      lower_bound = new SgUntypedNullExpression();
      upper_bound = new SgUntypedNullExpression();

      setSourcePositionUnknown(lower_bound);
      setSourcePositionUnknown(upper_bound);
   }
   else return ATfalse;

   stride = new SgUntypedNullExpression();
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
      //  IfStatement                 -> SimpleStatement
      //  CaseStatement               -> SimpleStatement
      //%%ProcedureCallStatement      -> SimpleStatement  %%AMBIGUOUS with AssignmentStatement

      else if (traverse_NullStatement(t_stmt, stmt_list)) {
         // MATCHED NullStatement
      }
      else return ATfalse;
   }

// This subsumes the labels in statements, eventually all SimpleStatements will take this path
   else if (ATmatch(term, "SimpleStatement(<term>)", &t_stmt)) {
      if (traverse_AbortStatement(t_stmt, stmt_list)) {
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
      else return ATfalse;
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
      else if (traverse_IntegerFormula(t_stop_code, &stop_code)) {
         // MATCHED IntegerFormula
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
ATbool ATermToUntypedJovialTraversal::traverse_NumericFormula(ATerm term, SgUntypedExpression** expr)
{
   if (traverse_IntegerFormula(term, expr)) {
      // MATCHED IntegerFormula
   }

   //  FloatingFormula             -> NumericFormula
   //  FixedFormula                -> NumericFormula
#if 0 //TODO - FixedOrFloatingFormula?
   else if (traverse_FloatingFormula(term, expr)) {
      // MATCHED FloatingFormula
   }
#endif

   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// 5.1.1 INTEGER FORMULAS
//----------------------------------------------------------------------------------------
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
         // MATCHED IntegerTerm
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

      // IntegerFormula PlusOrMinus IntegerTerm -> IntegerFormula

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

ATbool ATermToUntypedJovialTraversal::traverse_IntegerPrimary(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_IntegerPrimary: %s\n", ATwriteToString(term));
#endif

   char *literal, *name;
   SgUntypedType* type;
   Jovial_ROSE_Translation::ExpressionKind expr_enum = Jovial_ROSE_Translation::e_referenceExpression;

   if (ATmatch(term, "IntegerLiteral(<str>)", &literal)) {
      type = UntypedBuilder::buildType(SgUntypedType::e_int);
      expr_enum = Jovial_ROSE_Translation::e_literalExpression;
      *expr = new SgUntypedValueExpression(expr_enum,literal,type);
      std::cout << "INTEGER LITERAL is " << literal << "\n";
      setSourcePosition(*expr, term);
   }
   //  IntegerMachineParameter     -> IntegerPrimary
   //  IntegerVariable             -> IntegerPrimary
   else if (ATmatch(term, "<str>" , &name)) {
      std::cout << "VARIABLE " << name << "\n";
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

ATbool ATermToUntypedJovialTraversal::traverse_IntegerTerm(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_IntegerTerm: %s\n", ATwriteToString(term));
#endif

   ATerm t_lhs, t_op, t_rhs;
   std::string op_name;
   General_Language_Translation::ExpressionKind op_enum;
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
      setSourcePosition(*expr, term);
   }
   else if (traverse_IntegerFactor(term, expr)) {
         // MATCHED IntegerFactor
   }
   else return ATfalse;

   return ATtrue;
}

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

   cerr << "FIXED Formula expressions are not yet implemented! \n";

   return ATfalse;
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
      std::cout << "VARIABLE " << name << "\n";
      *var = new SgUntypedReferenceExpression(expr_enum, name);
      setSourcePosition(*var, term);

   } else return ATfalse;

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
