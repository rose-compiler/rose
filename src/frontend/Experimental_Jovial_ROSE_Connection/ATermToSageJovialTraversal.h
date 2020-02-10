#ifndef ATERM_TO_SAGE_JOVIAL_TRAVERSAL_H
#define ATERM_TO_SAGE_JOVIAL_TRAVERSAL_H

#include "ATerm/ATermToUntypedTraversal.h"
#include "general_language_translation.h"
#include "Jovial_to_ROSE_translation.h"
#include "sage-tree-builder.h"

#define USE_SAGE_TREE_BUILDER 1

namespace ATermSupport {

class ATermToSageJovialTraversal : public ATermToUntypedTraversal
{
 private:
#if USE_SAGE_TREE_BUILDER
   Rose::builder::SageTreeBuilder sage_tree_builder;
#else
   Rose::builder::SageTreeBuilderNull sage_tree_builder;
#endif

 protected:
   void setSourcePositions(ATerm term, Rose::builder::SourcePosition &start, Rose::builder::SourcePosition &end);

 public:
   ATermToSageJovialTraversal(SgSourceFile* source);
   virtual ~ATermToSageJovialTraversal();

 public:
// 1.1
ATbool traverse_Module(ATerm term);

// 1.2.1 COMPOOL MODULES
ATbool traverse_CompoolModule(ATerm term, SgUntypedScope* scope);

// 1.2.2 PROCEDURE MODULES
ATbool traverse_ProcedureModule(ATerm term, SgUntypedScope* scope);
ATbool traverse_DeclarationList(ATerm term, SgUntypedDeclarationStatementList* decl_list);
ATbool traverse_NullDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list, int def_or_ref = 0);

// 1.2.3 MAIN PROGRAM MODULES
ATbool traverse_MainProgramModule(ATerm term, SgUntypedGlobalScope* global_scope);
ATbool traverse_Name(ATerm term, std::string & name);
ATbool traverse_ProgramBody(ATerm term, SgUntypedFunctionScope* & function_scope);
ATbool traverse_NonNestedSubroutineList(ATerm term, SgUntypedScope* scope);
ATbool traverse_SubroutineDefinitionList(ATerm term, SgUntypedFunctionDeclarationList* func_list);

// 1.4 IMPLEMENTATION PARAMETERS
ATbool traverse_IntegerMachineParameter (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr);
ATbool traverse_FloatingMachineParameter(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr);
ATbool traverse_FixedMachineParameter   (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr);

// 2.0 DECLARATIONS
ATbool traverse_Declaration(ATerm term, SgUntypedDeclarationStatementList* decl_list);

// 2.1 DATA DECLARATIONS
ATbool traverse_DataDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list, int def_or_ref = 0);

// 2.1.1 ITEM DECLARATION
ATbool traverse_ItemDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list, int def_or_ref = 0);
ATbool traverse_ItemTypeDescription(ATerm term, SgType* &sg_type, SgUntypedType* &type, SgUntypedExprListExpression* attr_list);

// 2.1.1.1 INTEGER TYPE DESCRIPTIONS
ATbool traverse_IntegerItemDescription(ATerm term, SgType* &sg_type, SgUntypedType* &type, SgUntypedExprListExpression* attr_list);
ATbool traverse_OptItemSize(ATerm term, bool & has_size, SgExpression* &sg_size, SgUntypedExpression* &size);

// 2.1.1.2 FLOATING TYPE DESCRIPTIONS
ATbool traverse_FloatingItemDescription(ATerm term, SgType* &sg_type, SgUntypedType* &type, SgUntypedExprListExpression* attr_list);
ATbool traverse_OptRoundOrTruncate(ATerm term, bool* has_round_or_truncate,
                                   General_Language_Translation::ExpressionKind* modifier_enum);

// 2.1.1.3 FIXED TYPE DESCRIPTIONS
ATbool traverse_FixedItemDescription(ATerm term, SgUntypedType* & type, SgUntypedExprListExpression* attr_list);
ATbool traverse_FractionSpecifier(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* & fraction);

// 2.1.1.4 BIT TYPE DESCRIPTIONS
ATbool traverse_BitItemDescription(ATerm term, SgType* &sg_type, SgUntypedType* &type);

// 2.1.1.5 CHARACTER TYPE DESCRIPTIONS
ATbool traverse_CharacterItemDescription  (ATerm term, SgUntypedType* & type);
ATbool traverse_CharacterLiteral          (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr);

// 2.1.1.6 STATUS TYPE DESCRIPTIONS
ATbool traverse_StatusItemDescription (ATerm term, SgUntypedInitializedNameList* & status_list, bool & has_size, SgUntypedExpression* & size);
ATbool traverse_StatusConstant        (ATerm term, SgUntypedInitializedName* & init_name);
ATbool traverse_DefaultSublist        (ATerm term, SgUntypedInitializedNameList* status_list);
ATbool traverse_OptDefaultSublist     (ATerm term, SgUntypedInitializedNameList* status_list);
ATbool traverse_StatusList            (ATerm term, SgUntypedInitializedNameList* status_list);
ATbool traverse_SpecifiedSublist      (ATerm term, SgUntypedInitializedNameList* status_list);

// 2.1.1.7 POINTER TYPE DESCRIPTIONS
ATbool traverse_PointerItemDescription (ATerm term, SgUntypedType* & type);
ATbool traverse_OptTypeName            (ATerm term, SgUntypedType* & type, std::string & name);

// 2.1.2 TABLE DECLARATION
ATbool traverse_TableDeclaration       (ATerm term, int def_or_ref = 0);
ATbool traverse_TableDescription       (ATerm term, SgJovialTableStatement* sg_table_decl, SgUntypedStructureDefinition* & table_desc);
ATbool traverse_TableDescriptionName   (ATerm term, std::string & type_name, SgType* &sg_type, SgInitializer* &preset);
ATbool traverse_EntrySpecifierBody     (ATerm term, SgJovialTableStatement* sg_table_decl, SgUntypedStructureDefinition*   table_desc);
ATbool traverse_TableDescriptionType   (ATerm term, SgType* &base_type, SgInitializer* &preset);
ATbool traverse_EntrySpecifierType     (ATerm term, SgType* &base_type, SgInitializer* &preset);
ATbool traverse_EntrySpecifier         (ATerm term, SgJovialTableStatement* sg_table_decl,
                                                    SgUntypedType* & type, SgUntypedExprListExpression* attr_list, SgUntypedExprListExpression* &preset);

// 2.1.2.3 ORDINARY TABLE ENTRIES
ATbool traverse_OrdinaryEntrySpecifierType   (ATerm term, SgType* &type, SgInitializer* &preset);
ATbool traverse_OrdinaryEntrySpecifier       (ATerm term, SgJovialTableStatement* sg_table_decl,
                                                          SgUntypedType* & type, SgUntypedExprListExpression* attr_list, SgUntypedExprListExpression* & preset);
ATbool traverse_OrdinaryEntrySpecifierBody   (ATerm term,  SgJovialTableStatement* sg_table_decl, SgUntypedStructureDefinition* table_desc);
ATbool traverse_OrdinaryTableBody            (ATerm term, SgUntypedDeclarationStatementList* decl_list);
ATbool traverse_OrdinaryTableItemDeclaration (ATerm term, SgUntypedDeclarationStatementList* decl_list);
ATbool traverse_OptPackingSpecifier          (ATerm term, SgUntypedExprListExpression* attr_list);

// 2.1.2.1 TABLE DIMENSION LISTS
ATbool traverse_OptDimensionList (ATerm term, SgExprListExp* sg_shape);
ATbool traverse_Dimension        (ATerm term, SgExprListExp* sg_shape);

// 2.1.2.2 TABLE STRUCTURE
ATbool traverse_OptStructureSpecifier(ATerm term, SgUntypedExprListExpression* attr_list);

// 2.1.2.4 SPECIFIED TABLE ENTRIES
ATbool traverse_SpecifiedEntrySpecifierType   (ATerm term, SgType* &type, SgInitializer* &preset);
ATbool traverse_SpecifiedEntrySpecifier       (ATerm term, SgJovialTableStatement* sg_table_decl, SgUntypedType* & type,
                                                           SgUntypedExprListExpression* attr_list, SgUntypedExprListExpression* & preset);
ATbool traverse_SpecifiedEntrySpecifierBody   (ATerm term, SgJovialTableStatement* sg_table_decl, SgUntypedStructureDefinition* table_desc);
ATbool traverse_SpecifiedTableBody            (ATerm term, SgUntypedDeclarationStatementList* decl_list);
ATbool traverse_SpecifiedTableItemDeclaration (ATerm term, SgUntypedDeclarationStatementList* decl_list);
ATbool traverse_SpecifiedItemDescription      (ATerm term, SgUntypedType* & type, SgUntypedExprListExpression* attr_list);
ATbool traverse_WordsPerEntry                 (ATerm term, SgJovialTableStatement* sg_table_decl, SgUntypedExprListExpression* attr_list);

// 2.1.3 CONSTANT DECLARATIONS
ATbool traverse_ConstantDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list);

// 2.1.4 BLOCK DECLARATION
ATbool traverse_BlockDeclaration   (ATerm term, SgUntypedDeclarationStatementList* decl_list);
ATbool traverse_BlockBodyPart      (ATerm term, SgUntypedDeclarationStatementList* decl_list);
ATbool traverse_BlockPreset        (ATerm term, SgUntypedExprListExpression* preset_list);
ATbool traverse_BlockPresetList    (ATerm term, SgUntypedExprListExpression* preset_list);
ATbool traverse_OptBlockPresetList (ATerm term, SgUntypedExprListExpression* preset_list);

// 2.1.5 ALLOCATION OF DATA OBJECTS
ATbool traverse_OptAllocationSpecifier(ATerm term, SgUntypedExprListExpression* attr_list);

// 2.1.6 INITIALIZATION OF DATA OBJECTS
ATbool traverse_ItemPreset            (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr);
ATbool traverse_ItemPresetValue       (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr);
ATbool traverse_OptItemPresetValue    (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr);
ATbool traverse_TablePreset           (ATerm term, SgInitializer* &preset);
ATbool traverse_TablePreset           (ATerm term, SgUntypedExprListExpression* & preset);
ATbool traverse_TablePresetList       (ATerm term, SgUntypedExprListExpression* preset);
ATbool traverse_DefaultPresetSublist  (ATerm term, SgUntypedExprListExpression* preset);
ATbool traverse_SpecifiedPresetSublist(ATerm term, SgUntypedExprListExpression* preset);
ATbool traverse_PresetIndexSpecifier  (ATerm term, SgUntypedExprListExpression* preset);
ATbool traverse_PresetValuesOption    (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr);

// 2.2 TYPE DECLARATIONS
ATbool traverse_TypeDeclaration       (ATerm term, SgUntypedDeclarationStatementList* decl_list);
ATbool traverse_ItemTypeDeclaration   (ATerm term, SgUntypedDeclarationStatementList* decl_list);
ATbool traverse_TableTypeDeclaration  (ATerm term, SgUntypedDeclarationStatementList* decl_list);
ATbool traverse_TableTypeSpecifier    (ATerm term, SgJovialTableStatement* sg_table_decl, SgUntypedStructureDeclaration* table_decl);
ATbool traverse_BlockTypeDeclaration  (ATerm term, SgUntypedDeclarationStatementList* decl_list);

// 2.3 STATEMENT NAME DECLARATIONS
ATbool traverse_StatementNameDeclaration (ATerm term, SgUntypedDeclarationStatementList* decl_list, int def_or_ref = 0);

// 2.4 DEFINE DECLARATIONS
ATbool traverse_DefineDeclaration        (ATerm term, SgUntypedDeclarationStatementList* decl_list);
ATbool traverse_DefinitionPart           (ATerm term, std::string & params, std::string & def_string);
ATbool traverse_FormalDefineParameterList(ATerm term, std::string & params);
ATbool traverse_DefineString             (ATerm term, std::string & def_string);

// 2.5 EXTERNAL DECLARATIONS
ATbool traverse_ExternalDeclaration   (ATerm term, SgUntypedDeclarationStatementList* decl_list);

// 2.5.1 DEF SPECIFICATIONS
ATbool traverse_SimpleDef             (ATerm term, SgUntypedDeclarationStatementList* decl_list);
ATbool traverse_CompoundDef           (ATerm term, SgUntypedDeclarationStatementList* decl_list);
ATbool traverse_DefSpecificationChoice(ATerm term, SgUntypedDeclarationStatementList* decl_list);

// 2.5.2 REF SPECIFICATIONS
ATbool traverse_SimpleRef             (ATerm term, SgUntypedDeclarationStatementList* decl_list);
ATbool traverse_CompoundRef           (ATerm term, SgUntypedDeclarationStatementList* decl_list);
ATbool traverse_RefSpecificationChoice(ATerm term, SgUntypedDeclarationStatementList* decl_list);

// 2.6 OVERLAY DECLARATIONS
ATbool traverse_OverlayDeclaration    (ATerm term, SgUntypedDeclarationStatementList* decl_list);
ATbool traverse_OverlayExpression     (ATerm term, SgUntypedExpression* & expr);
ATbool traverse_OverlayString         (ATerm term, SgUntypedExpression* & expr);
ATbool traverse_OverlayElement        (ATerm term, SgUntypedExpression* & expr);
ATbool traverse_Spacer                (ATerm term, SgUntypedExpression* & expr);

// 3.0 PROCEDURES AND FUNCTIONS
ATbool traverse_ProcedureDefinition(ATerm term, SgUntypedFunctionDeclarationList* func_list);
ATbool traverse_SubroutineAttribute(ATerm term, SgUntypedOtherExpression* & attr_expr);
ATbool traverse_ProcedureHeading   (ATerm term, std::string & name,
                                    SgUntypedExprListExpression* & attrs, SgUntypedInitializedNameList* & params);
ATbool traverse_SubroutineBody     (ATerm term, SgUntypedFunctionScope* & function_scope);

// 3.1 PROCEDURES
ATbool traverse_ProcedureDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list);

// 3.2 FUNCTIONS
ATbool traverse_FunctionDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list);
ATbool traverse_FunctionDefinition (ATerm term, SgUntypedFunctionDeclarationList* func_list);
ATbool traverse_FunctionHeading    (ATerm term, std::string & name, SgUntypedType* & type,
                                    SgUntypedExprListExpression* & attrs, SgUntypedInitializedNameList* & params);

// 3.3 PARAMETERS OF PROCEDURES AND FUNCTIONS
ATbool traverse_FormalParameterList   (ATerm term, SgUntypedInitializedNameList* param_list);
ATbool traverse_FormalInputParameter  (ATerm term, SgUntypedInitializedNameList* param_list);
ATbool traverse_FormalOutputParameters(ATerm term, SgUntypedInitializedNameList* param_list);
ATbool traverse_FormalOutputParameter (ATerm term, SgUntypedInitializedNameList* param_list);
ATbool traverse_ParameterBinding      (ATerm term, SgUntypedOtherExpression* & binding_expr);

// 3.4 INLINE DECLARATIONS
ATbool traverse_InlineDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list);

// 4.0 STATEMENTS
ATbool traverse_Statement(ATerm term, SgUntypedStatementList* stmt_list);
ATbool traverse_StatementList(ATerm term, SgUntypedStatementList* stmt_list);
ATbool traverse_SimpleStatement(ATerm term, SgUntypedStatementList* stmt_list);
ATbool traverse_CompoundStatement(ATerm term, SgUntypedStatementList* stmt_list);
ATbool traverse_NullStatement(ATerm term, SgUntypedStatementList* stmt_list);
ATbool traverse_NullBlockStatement(ATerm term, SgUntypedStatementList* stmt_list);
ATbool traverse_LabelList(ATerm term, std::vector<std::string> & labels, std::vector<PosInfo> & locations);

// 4.1 ASSIGNMENT STATEMENTS
ATbool traverse_AssignmentStatement(ATerm term, std::vector<std::string> & labels, SgUntypedStatementList* stmt_list);

// 4.2 LOOP STATEMENTS
ATbool traverse_WhileStatement  (ATerm term, SgUntypedStatementList* stmt_list);
ATbool traverse_ForStatement    (ATerm term, SgUntypedStatementList* stmt_list);
ATbool traverse_ForClause       (ATerm term, SgUntypedExpression* & var_ref, SgUntypedExpression* & init,
                                             SgUntypedExpression* & phrase1, SgUntypedExpression* & phrase2,
                                             int & phrase1_enum, int & phrase2_enum);
ATbool traverse_ControlClause   (ATerm term, SgUntypedExpression* & initial_value,
                                             SgUntypedExpression* & phrase1, SgUntypedExpression* & phrase2,
                                             int & phrase1_enum, int & phrase2_enum);
ATbool traverse_OptContinuation (ATerm term, SgUntypedExpression* & phrase1, SgUntypedExpression* & phrase2,
                                             int & phrase1_enum, int & phrase2_enum);
ATbool traverse_Continuation    (ATerm term, SgUntypedExpression* & phrase1, SgUntypedExpression* & phrase2,
                                             int & phrase1_enum, int & phrase2_enum);
ATbool traverse_Phrase          (ATerm term, SgUntypedExpression* & expr, int & phrase_enum);

// 4.3 IF STATEMENTS
ATbool traverse_IfStatement   (ATerm term, SgUntypedStatementList* stmt_list);

// 4.4 CASE STATEMENTS
ATbool traverse_CaseStatement   (ATerm term, SgUntypedStatementList* stmt_list);
ATbool traverse_CaseBody        (ATerm term, SgUntypedStatement**    case_body);
ATbool traverse_CaseAlternative (ATerm term, SgUntypedStatementList* stmt_list);
ATbool traverse_DefaultOption   (ATerm term, SgUntypedStatementList* stmt_list);
ATbool traverse_CaseIndexGroup  (ATerm term, SgUntypedExprListExpression* & case_index_group);
ATbool traverse_CaseIndex       (ATerm term, SgUntypedExpression* & case_index);

// 4.5 PROCEDURE CALL STATEMENTS
ATbool traverse_ProcedureCallStatement (ATerm term, SgUntypedStatementList* stmt_list);
ATbool traverse_ActualParameterList    (ATerm term, SgUntypedExprListExpression* arg_list);
ATbool traverse_ActualOutputParameters (ATerm term, SgUntypedExprListExpression* param_list);

// 4.6 RETURN STATEMENTS
ATbool traverse_ReturnStatement (ATerm term, SgUntypedStatementList* stmt_list);

// 4.7 GOTO STATEMENTS
ATbool traverse_GotoStatement   (ATerm term, SgUntypedStatementList* stmt_list);

// 4.8 EXIT STATEMENTS
ATbool traverse_ExitStatement   (ATerm term, SgUntypedStatementList* stmt_list);

// 4.9 STOP STATEMENTS
ATbool traverse_StopStatement   (ATerm term, SgUntypedStatementList* stmt_list);

// 4.10 ABORT STATEMENTS
ATbool traverse_AbortStatement  (ATerm term, SgUntypedStatementList* stmt_list);

// 5.0 FORMULAS
ATbool traverse_Formula(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &ut_expr);

// 5.1 NUMERIC FORMULAS
ATbool traverse_NumericFormula (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &ut_expr);
ATbool traverse_NumericTerm    (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &ut_expr);
ATbool traverse_NumericFactor  (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &ut_expr);
ATbool traverse_NumericPrimary (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &ut_expr);
ATbool traverse_OptSign        (ATerm term, General_Language_Translation::ExpressionKind & op_enum);
ATbool traverse_ExponentiationOp(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &ut_expr);
ATbool traverse_NumericMachineParameter(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &ut_expr);

// 5.2 BIT FORMULAS
ATbool traverse_BitFormula             (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &ut_expr);
ATbool traverse_OptLogicalContinuation (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &ut_expr);
ATbool traverse_LogicalContinuation    (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &ut_expr);
ATbool traverse_LogicalComponent       (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &ut_expr);
ATbool traverse_LogicalOperand         (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &ut_expr);
ATbool traverse_BitPrimary             (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &ut_expr);

// 5.2.1 RELATIONAL EXPRESSIONS
ATbool traverse_RelationalExpression   (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr);

// 5.3.0 GENERAL FORMULAS
ATbool traverse_GeneralFormula   (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr);

// 5.3 CHARACTER FORMULAS
ATbool traverse_CharacterFormula (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr);

// 5.4 STATUS FORMULAS
ATbool traverse_StatusFormula    (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr);

// 5.5 POINTER FORMULAS
ATbool traverse_PointerFormula   (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr);

// 6.1 VARIABLE AND BLOCK REFERENCES
ATbool traverse_Variable         (ATerm term, SgExpression* &sg_var, SgUntypedExpression* &var);
ATbool traverse_VariableList     (ATerm term, std::vector<SgExpression*> &sg_vars, std::vector<SgUntypedExpression*> &vars);
ATbool traverse_TableItem        (ATerm term, SgExpression* &sg_var, SgUntypedExpression* &var);
ATbool traverse_Subscript        (ATerm term, std::vector<SgUntypedExpression*> & indexes);
ATbool traverse_Index            (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &formula);
ATbool traverse_TableDereference (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &formula);
ATbool traverse_Dereference      (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &formula);
ATbool traverse_BitFunctionVariable  (ATerm term, SgExpression* &sg_var, SgUntypedExpression* &var);
ATbool traverse_ByteFunctionVariable (ATerm term, SgExpression* &sg_var, SgUntypedExpression* &var);

// 6.2 NAMED CONSTANTS
ATbool traverse_NamedConstant        (ATerm term, SgExpression* &sg_var, SgUntypedExpression* &var);

// 6.3 FUNCTION CALLS
ATbool traverse_FunctionCall           (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr);
ATbool traverse_UserDefinedFunctionCall(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr);
ATbool traverse_IntrinsicFunctionCall  (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr);

// 6.3.1 LOC FUNCTION
ATbool traverse_LocFunction          (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr);

// 6.3.2 NEXT FUNCTION
ATbool traverse_NextFunction         (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr);

// 6.3.4 BYTE FUNCTION
ATbool traverse_ByteFunction(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr);

// 6.3.7 SIZE FUNCTION
ATbool traverse_SizeFunction(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr);

// 6.3.11 STATUS INVERSE FUNCTIONS
ATbool traverse_StatusInverseFunction(ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr);

// 7.0 TYPE MATCHING AND TYPE CONVERSIONS
ATbool traverse_BitConversion        (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr);
ATbool traverse_IntegerConversion    (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr);
ATbool traverse_GeneralConversion    (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr);
ATbool traverse_FloatingConversion   (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr);
ATbool traverse_FixedConversion      (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr);
ATbool traverse_CharacterConversion  (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr);
ATbool traverse_CharacterConversionC (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr);
ATbool traverse_StatusConversion     (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr);
ATbool traverse_PointerConversion    (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr);
ATbool traverse_PointerConversionP   (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr);

// 8.2.3 OPERATORS
ATbool traverse_MultiplyDivideOrMod(ATerm term, General_Language_Translation::ExpressionKind & op_enum, std::string & op_name);
ATbool traverse_RelationalOperator (ATerm term, General_Language_Translation::ExpressionKind & op_enum, std::string & op_name);

// 8.3.1 NUMERIC LITERAL
ATbool traverse_FixedOrFloatingLiteral (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr);
ATbool traverse_Exponent               (ATerm term, std::string & opt_exp);

// 8.3.2 BIT LITERAL
ATbool traverse_BitLiteral     (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr);

// 8.3.3 BOOLEAN LITERAL
ATbool traverse_BooleanLiteral (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr);

// 8.3.4 POINTER LITERAL
ATbool traverse_PointerLiteral (ATerm term, SgExpression* &sg_expr, SgUntypedExpression* &expr);

// 9.0 DIRECTIVES
ATbool traverse_DirectiveList     (ATerm term, SgUntypedDeclarationStatementList* decl_list);
ATbool traverse_Directive         (ATerm term, SgUntypedDeclarationStatementList* decl_list);

// 9.1 COMPOOL DIRECTIVES
ATbool traverse_CompoolDirective  (ATerm term, SgUntypedDeclarationStatementList* decl_list);

// 9.6 REDUCIBLE DIRECTIVES
ATbool traverse_ReducibleDirective(ATerm term, SgUntypedDeclarationStatementList* decl_list);

// 9.11 ALLOCATION ORDER DIRECTIVES
ATbool traverse_OrderDirective    (ATerm term, SgUntypedDeclarationStatementList* decl_list);

}; // class ATermToSageJovialTraversal
}  // namespace Jovial

#endif
