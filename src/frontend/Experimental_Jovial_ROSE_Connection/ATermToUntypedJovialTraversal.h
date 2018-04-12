#ifndef ATERM_TO_UNTYPED_JOVIAL_TRAVERSAL_H
#define ATERM_TO_UNTYPED_JOVIAL_TRAVERSAL_H

#include "ATerm/ATermToUntypedTraversal.h"
#include "general_language_translation.h"

namespace ATermSupport {

class ATermToUntypedJovialTraversal : public ATermToUntypedTraversal
{
 public:
   ATermToUntypedJovialTraversal(SgSourceFile* source);
   virtual ~ATermToUntypedJovialTraversal();

 public:
// 1.1
ATbool traverse_Module(ATerm term);

// 1.2.1 COMPOOL MODULES
ATbool traverse_CompoolModule(ATerm term, SgUntypedScope* scope);

// 1.2.2 PROCEDURE MODULES
ATbool traverse_ProcedureModule(ATerm term, SgUntypedScope* scope);
ATbool traverse_DeclarationList(ATerm term, SgUntypedDeclarationStatementList* decl_list);
ATbool traverse_NullDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list);

// 1.2.3 MAIN PROGRAM MODULES
ATbool traverse_MainProgramModule(ATerm term, SgUntypedGlobalScope* global_scope);
ATbool traverse_Name(ATerm term, std::string & name);
ATbool traverse_ProgramBody(ATerm term, SgUntypedFunctionScope** function_scope);
ATbool traverse_NonNestedSubroutineList(ATerm term, SgUntypedScope* scope);
ATbool traverse_SubroutineDefinitionList(ATerm term, SgUntypedFunctionDeclarationList*);

// 2.1 DATA DECLARATIONS
ATbool traverse_DataDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list);

// 2.1.1 ITEM DECLARATION
ATbool traverse_ItemDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list);
ATbool traverse_ItemTypeDescription(ATerm term, SgUntypedType** type, SgUntypedExprListExpression* attr_list);

// 2.1.1.1 INTEGER TYPE DESCRIPTIONS
ATbool traverse_IntegerItemDescription(ATerm term, SgUntypedType** type, SgUntypedExprListExpression* attr_list);
ATbool traverse_OptItemSize(ATerm term, bool* has_size, SgUntypedExpression** size);

// 2.1.1.2 FLOATING TYPE DESCRIPTIONS
ATbool traverse_FloatingItemDescription(ATerm term, SgUntypedType** type, SgUntypedExprListExpression* attr_list);
ATbool traverse_OptRoundOrTruncate(ATerm term, bool* has_round_or_truncate,
                                   General_Language_Translation::ExpressionKind* modifier_enum);

// 2.1.1.3 FIXED TYPE DESCRIPTIONS
ATbool traverse_FixedItemDescription(ATerm term, SgUntypedType** type, SgUntypedExprListExpression* attr_list);
ATbool traverse_FractionSpecifier(ATerm term, SgUntypedExpression** fraction);

// 2.1.1.4 BIT TYPE DESCRIPTIONS
ATbool traverse_BitItemDescription(ATerm term, SgUntypedType** type);

// 2.1.1.5 CHARACTER TYPE DESCRIPTIONS
ATbool traverse_CharacterItemDescription(ATerm term, SgUntypedType** type);

// 2.1.1.6 STATUS TYPE DESCRIPTIONS
ATbool traverse_StatusItemDescription(ATerm term, SgUntypedType** type);

// 2.1.1.7 POINTER TYPE DESCRIPTIONS
ATbool traverse_PointerItemDescription(ATerm term, SgUntypedType** type);

// 2.1.2 TABLE DECLARATION
ATbool traverse_TableDeclaration (ATerm term, SgUntypedDeclarationStatementList* decl_list);
ATbool traverse_TableDescription (ATerm term, SgUntypedType** type, SgUntypedExprListExpression* attr_list);
ATbool traverse_EntrySpecifier   (ATerm term, SgUntypedType** type, SgUntypedExprListExpression* attr_list);

// 2.1.2.3 ORDINARY TABLE ENTRIES
ATbool traverse_OrdinaryEntrySpecifier (ATerm term, SgUntypedType** type, SgUntypedExprListExpression* attr_list);
ATbool traverse_OptPackingSpecifier    (ATerm term, SgUntypedExprListExpression* attr_list);

// 2.1.2.4 SPECIFIED TABLE ENTRIES
ATbool traverse_SpecifiedEntrySpecifier(ATerm term, SgUntypedType** type, SgUntypedExprListExpression* attr_list);

// 2.1.2.1 TABLE DIMENSION LISTS
ATbool traverse_OptDimensionList(ATerm term, SgUntypedExprListExpression* dim_info);
ATbool traverse_Dimension(ATerm term, SgUntypedExprListExpression* dim_info);

// 2.1.2.2 TABLE STRUCTURE
ATbool traverse_OptStructureSpecifier(ATerm term, SgUntypedExprListExpression* attr_list);

// 2.1.5 ALLOCATION OF DATA OBJECTS
ATbool traverse_OptAllocationSpecifier(ATerm term, SgUntypedExprListExpression* attr_list);

// 4.0 STATEMENTS
ATbool traverse_Statement(ATerm term, SgUntypedStatementList* stmt_list);
ATbool traverse_StatementList(ATerm term, SgUntypedStatementList* stmt_list);
ATbool traverse_SimpleStatement(ATerm term, SgUntypedStatementList* stmt_list);
ATbool traverse_NullStatement(ATerm term, SgUntypedStatementList* stmt_list);
ATbool traverse_LabelList(ATerm term, std::vector<std::string> & labels);

// 4.1 ASSIGNMENT STATEMENTS
ATbool traverse_AssignmentStatement(ATerm term, std::vector<std::string> & labels, SgUntypedStatementList* stmt_list);

// 4.10 ABORT STATEMENTS
ATbool traverse_AbortStatement(ATerm term, SgUntypedStatementList* stmt_list);

// 5.0 FORMULAS
ATbool traverse_Formula(ATerm term, SgUntypedExpression** expr);

// 5.1 NUMERIC FORMULAS
ATbool traverse_NumericFormula(ATerm term, SgUntypedExpression** expr);

// 5.1.1 INTEGER FORMULAS
ATbool traverse_IntegerFormula (ATerm term, SgUntypedExpression** expr);
ATbool traverse_IntegerPrimary (ATerm term, SgUntypedExpression** expr);
ATbool traverse_OptSign        (ATerm term, General_Language_Translation::ExpressionKind & op_enum);
ATbool traverse_IntegerTerm    (ATerm term, SgUntypedExpression** expr);
ATbool traverse_IntegerFactor  (ATerm term, SgUntypedExpression** expr);

// 5.1.2 FLOATING FORMULAS
ATbool traverse_FloatingFormula (ATerm term, SgUntypedExpression** expr);
ATbool traverse_FloatingTerm    (ATerm term, SgUntypedExpression** expr);
ATbool traverse_FloatingFactor  (ATerm term, SgUntypedExpression** expr);
ATbool traverse_FloatingPrimary (ATerm term, SgUntypedExpression** expr);
ATbool traverse_FloatingLiteral (ATerm term, SgUntypedExpression** expr);

// 5.1.3 FIXED FORMULAS
ATbool traverse_FixedFormula (ATerm term, SgUntypedExpression** expr);

// 6.1 VARIABLE AND BLOCK REFERENCES
ATbool traverse_Variable(ATerm term, SgUntypedExpression** var);
ATbool traverse_VariableList(ATerm term, std::vector<SgUntypedExpression*> & vars);

// 8.2.3 OPERATORS
ATbool traverse_MultiplyDivideOrMod(ATerm term, General_Language_Translation::ExpressionKind & op_enum, std::string & op_name);

}; // class ATermToUntypedJovialTraversal
}  // namespace Jovial

#endif
