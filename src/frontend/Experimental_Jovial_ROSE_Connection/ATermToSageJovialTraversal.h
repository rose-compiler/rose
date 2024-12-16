#ifndef ATERM_TO_SAGE_JOVIAL_TRAVERSAL_H_
#define ATERM_TO_SAGE_JOVIAL_TRAVERSAL_H_

#include "ATerm/ATermTraversal.h"
#include "general_language_translation.h"

namespace rb = Rose::builder;

namespace ATermSupport {

   using namespace LanguageTranslation;
   using StrucSpecEnum = SgJovialTableType::StructureSpecifier;
   using WordsPerEntry = SgJovialTableStatement::WordsPerEntry;
   using Language = rb::SageTreeBuilder::LanguageEnum;

class ATermToSageJovialTraversal : public ATermTraversal
{
 private:
   rb::SageTreeBuilder sage_tree_builder;
   std::multimap<const std::string, SgStringVal*> labelDecls_;
   std::multimap<const std::string, SgStringVal*> labelRefs_;

 protected:
   void setSourcePositions(ATerm term, Rose::builder::SourcePosition &start, Rose::builder::SourcePosition &end);
   void setDeclarationModifier(SgVariableDeclaration* var_decl, int def_or_ref);
   void setSourcePosition(SgLocatedNode* node, ATerm term, bool attach_comments=true);

   /** Create a mangled name for a StatusConstant */
   std::string mangleStatusConstantName(const std::string &name);

   /** Create a mangled name for an anonymous type */
   std::string mangleAnonymousName(const std::string &name);

 public:
   ATermToSageJovialTraversal() = delete;
   ATermToSageJovialTraversal(SgSourceFile* source, std::istringstream &tokens)
     : ATermTraversal{source}, sage_tree_builder{source, Language::Jovial, tokens}
     {
     }

   void setLocationSpecifier(SgVariableDeclaration* var_decl, const LocationSpecifier &loc_spec);

 public:
// 1.1
ATbool traverse_Module(ATerm term);

// 1.2.1 COMPOOL MODULES
ATbool traverse_CompoolModule(ATerm term);

// 1.2.2 PROCEDURE MODULES
ATbool traverse_ProcedureModule(ATerm term);
ATbool traverse_DeclsAndStmts(ATerm term);
ATbool traverse_DeclarationList(ATerm term);
ATbool traverse_NullDeclaration(ATerm term);

// 1.2.3 MAIN PROGRAM MODULES
ATbool traverse_MainProgramModule(ATerm term);
ATbool traverse_Name(ATerm term, std::string & name);
ATbool traverse_ProgramBody(ATerm term);
ATbool traverse_NonNestedSubroutineList(ATerm term);
ATbool traverse_SubroutineDefinitionList(ATerm term);

// 1.4 IMPLEMENTATION PARAMETERS
ATbool traverse_IntegerMachineParameter (ATerm term, SgExpression* &expr);
ATbool traverse_FloatingMachineParameter(ATerm term, SgExpression* &expr);
ATbool traverse_FixedMachineParameter   (ATerm term, SgExpression* &expr);
ATbool traverse_NumericMachineParameter (ATerm term, SgExpression* &expr);

// 2.0 DECLARATIONS
ATbool traverse_Declaration(ATerm term);

// 2.1 DATA DECLARATIONS
ATbool traverse_DataDeclaration(ATerm term, int def_or_ref = 0);

// 2.1.1 ITEM DECLARATION
ATbool traverse_ItemDeclaration(ATerm term, int def_or_ref = 0);
ATbool traverse_ItemTypeDescription(ATerm term, SgType* &type);

// 2.1.1.1 INTEGER TYPE DESCRIPTIONS
ATbool traverse_IntegerItemDescription(ATerm term, SgType* &type);
ATbool traverse_OptItemSize(ATerm term, Sawyer::Optional<SgExpression*> &size);

// 2.1.1.2 FLOATING TYPE DESCRIPTIONS
ATbool traverse_FloatingItemDescription(ATerm term, SgType* &type);
ATbool traverse_OptRoundOrTruncate(ATerm term, Sawyer::Optional<LanguageTranslation::ExpressionKind> &modifier_enum);

// 2.1.1.3 FIXED TYPE DESCRIPTIONS
ATbool traverse_FixedItemDescription(ATerm term, SgType* &type);
ATbool traverse_FractionSpecifier(ATerm term, SgExpression* &fraction);

// 2.1.1.4 BIT TYPE DESCRIPTIONS
ATbool traverse_BitItemDescription(ATerm term, SgType* &type);

// 2.1.1.5 CHARACTER TYPE DESCRIPTIONS
ATbool traverse_CharacterItemDescription (ATerm term, SgType* & type);
ATbool traverse_CharacterLiteral         (ATerm term, std::string &str_literal);

// 2.1.1.6 STATUS TYPE DESCRIPTIONS
ATbool    match_StatusItemDescription (ATerm term);
ATbool traverse_StatusItemDescription (ATerm term, SgEnumDeclaration* enum_decl, Sawyer::Optional<SgExpression*> &status_size);
ATbool traverse_StatusConstant        (ATerm term, SgEnumDeclaration* enum_decl, int value, SgCastExp* cast=nullptr);
ATbool traverse_StatusConstant        (ATerm term, SgExpression* &expr);
ATbool traverse_DefaultSublist        (ATerm term, SgEnumDeclaration* enum_decl);
ATbool traverse_OptDefaultSublist     (ATerm term, SgEnumDeclaration* enum_decl);
ATbool traverse_StatusList            (ATerm term, SgEnumDeclaration* enum_decl);
ATbool traverse_SpecifiedSublist      (ATerm term, SgEnumDeclaration* enum_decl);

// 2.1.1.7 POINTER TYPE DESCRIPTIONS
ATbool traverse_PointerItemDescription (ATerm term, SgType* & type);
ATbool traverse_OptTypeName            (ATerm term, SgType* & type, std::string & name);

// 2.1.2 TABLE DECLARATION
ATbool traverse_TableDeclaration       (ATerm term, int def_or_ref = 0, bool constant = false);
ATbool traverse_TableDescriptionName   (ATerm term, std::string &type_name, SgType* &sg_type, SgExpression* &preset);
ATbool traverse_TableDescriptionType   (ATerm term, SgType* &base_type, SgExpression* &preset,
                                                    SgExprListExp* attr_list, TableSpecifier &table_spec);
ATbool traverse_TableDescriptionBody   (ATerm term, std::string &type_name, SgJovialTableStatement* &sg_table_decl);
ATbool traverse_TableDescriptionBody   (ATerm term, SgJovialTableStatement* sg_table_decl,
                                                    SgExpression* &preset, TableSpecifier &table_spec);
ATbool traverse_EntrySpecifierType     (ATerm term, SgType* &type, LocationSpecifier &loc_spec, SgExpression* &preset,
                                                    SgExprListExp* attr_list, TableSpecifier &table_spec);
ATbool traverse_EntrySpecifierBody     (ATerm term, SgJovialTableStatement* table_decl,
                                                    SgExpression* &preset, TableSpecifier &table_spec);

// 2.1.2.3 ORDINARY TABLE ENTRIES
ATbool traverse_OrdinaryEntrySpecifierType   (ATerm term, SgType* &type, SgExpression* &preset, TableSpecifier &table_spec);
ATbool traverse_OrdinaryEntrySpecifierBody   (ATerm term, SgExpression* &preset, TableSpecifier &table_spec);
ATbool traverse_OrdinaryTableBody            (ATerm term);
ATbool traverse_OrdinaryTableItemDeclaration (ATerm term);
ATbool traverse_OptPackingSpecifier          (ATerm term, PackingSpecifier &packing_spec);

// 2.1.2.1 TABLE DIMENSION LISTS
ATbool traverse_OptDimensionList (ATerm term, SgExprListExp* sg_shape);
ATbool traverse_Dimension        (ATerm term, SgExprListExp* sg_shape);

// 2.1.2.2 TABLE STRUCTURE
ATbool traverse_OptStructureSpecifier(ATerm term, StructureSpecifier &struct_spec);

// 2.1.2.4 SPECIFIED TABLE ENTRIES
ATbool traverse_SpecifiedEntrySpecifierType   (ATerm term, SgType* &type, LocationSpecifier &loc_spec, SgExpression* &preset, SgExprListExp* attr_list);
ATbool traverse_SpecifiedEntrySpecifierBody   (ATerm term, SgJovialTableStatement* table_decl, SgExpression* &preset);
ATbool traverse_SpecifiedTableBody            (ATerm term);
ATbool traverse_SpecifiedTableItemDeclaration (ATerm term);
ATbool traverse_SpecifiedItemDescription      (ATerm term, SgType* &type, LocationSpecifier &loc_spec, SgExprListExp* attr_list);
ATbool traverse_WordsPerEntry                 (ATerm term, Sawyer::Optional<SgExpression*> &entry_size, WordsPerEntry &wpe);

// 2.1.3 CONSTANT DECLARATIONS
ATbool traverse_ConstantDeclaration(ATerm term, int def_or_ref = 0);

// 2.1.4 BLOCK DECLARATION
ATbool traverse_BlockDeclaration   (ATerm term, int def_or_ref = 0);
ATbool traverse_BlockBodyPart      (ATerm term, SgJovialTableStatement* block_decl);
ATbool traverse_BlockPreset        (ATerm term, SgExpression* &preset);
ATbool traverse_BlockPresetList    (ATerm term, SgJovialTablePresetExp* block_preset);

// 2.1.5 ALLOCATION OF DATA OBJECTS
ATbool traverse_OptAllocationSpecifier(ATerm term, Sawyer::Optional<LanguageTranslation::ExpressionKind> &modifier_enum);

// 2.1.6 INITIALIZATION OF DATA OBJECTS
ATbool traverse_ItemPreset            (ATerm term, SgExpression* &preset);
ATbool traverse_ItemPresetValue       (ATerm term, SgExpression* &preset);
ATbool traverse_OptItemPresetValue    (ATerm term, SgExpression* &preset);
ATbool traverse_TablePreset           (ATerm term, SgExpression* &preset);
ATbool traverse_TablePresetValue      (ATerm term, SgExpression* &preset);
ATbool traverse_TablePresetList       (ATerm term, SgJovialTablePresetExp* table_preset);
ATbool traverse_PresetIndexSpecifier  (ATerm term, SgExprListExp* index_specifier_list);

// 2.2 TYPE DECLARATIONS
ATbool traverse_TypeDeclaration       (ATerm term);
ATbool traverse_ItemTypeDeclaration   (ATerm term);
ATbool traverse_TableTypeDeclaration  (ATerm term);
ATbool traverse_TableTypeSpecifier    (ATerm term, SgJovialTableStatement* table_decl);
ATbool traverse_BlockTypeDeclaration  (ATerm term);

// 2.3 STATEMENT NAME DECLARATIONS
ATbool traverse_StatementNameDeclaration (ATerm term, int def_or_ref = 0);

// 2.4 DEFINE DECLARATIONS
ATbool traverse_DefineDeclaration        (ATerm term);
ATbool traverse_DefinitionPart           (ATerm term, std::string & params, std::string & def_string);
ATbool traverse_FormalDefineParameterList(ATerm term, std::string & params);
ATbool traverse_DefineString             (ATerm term, std::string & def_string);

// 2.5 EXTERNAL DECLARATIONS
ATbool traverse_ExternalDeclaration   (ATerm term);

// 2.5.1 DEF SPECIFICATIONS
ATbool traverse_SimpleDef             (ATerm term);
ATbool traverse_CompoundDef           (ATerm term);
ATbool traverse_DefSpecificationChoice(ATerm term);
ATbool traverse_DefBlockInstantiation (ATerm term);

// 2.5.2 REF SPECIFICATIONS
ATbool traverse_SimpleRef             (ATerm term);
ATbool traverse_CompoundRef           (ATerm term);
ATbool traverse_RefSpecificationChoice(ATerm term);

// 2.6 OVERLAY DECLARATIONS
ATbool traverse_OverlayDeclaration    (ATerm term);
ATbool traverse_OverlayExpression     (ATerm term, SgExprListExp* &overlay_expr);
ATbool traverse_OverlayString         (ATerm term, SgExprListExp* &overlay_string);
ATbool traverse_OverlayElement        (ATerm term, SgExpression*  &expr);
ATbool traverse_Spacer                (ATerm term, SgExpression*  &expr);

// 3.0 PROCEDURES AND FUNCTIONS
ATbool traverse_ProcedureDefinition(ATerm term, LanguageTranslation::FunctionModifierList &modifiers);
ATbool traverse_SubroutineAttribute(ATerm term, LanguageTranslation::FunctionModifierList &modifiers);
ATbool traverse_ProcedureHeading   (ATerm term, std::string & name, std::list<FormalParameter> &param_list,
                                                LanguageTranslation::FunctionModifierList &modifiers);
ATbool traverse_SubroutineBody     (ATerm term);

// 3.1 PROCEDURES
ATbool traverse_ProcedureDeclaration(ATerm term, LanguageTranslation::FunctionModifierList &modifiers);

// 3.2 FUNCTIONS
ATbool traverse_FunctionDeclaration(ATerm term, LanguageTranslation::FunctionModifierList &modifiers);
ATbool traverse_FunctionDefinition (ATerm term, LanguageTranslation::FunctionModifierList &modifiers);
ATbool traverse_FunctionHeading    (ATerm term, std::string & name, SgType* &type, std::list<FormalParameter> &param_list,
                                                LanguageTranslation::FunctionModifierList &modifiers);

// 3.3 PARAMETERS OF PROCEDURES AND FUNCTIONS
ATbool traverse_FormalParameterList   (ATerm term, std::list<FormalParameter> &param_list);
ATbool traverse_FormalInputParameter  (ATerm term, std::list<FormalParameter> &param_list);
ATbool traverse_FormalOutputParameters(ATerm term, std::list<FormalParameter> &param_list);
ATbool traverse_FormalOutputParameter (ATerm term, std::list<FormalParameter> &param_list);
ATbool traverse_ParameterBinding      (ATerm term, LanguageTranslation::ExpressionKind &binding);

// 3.4 INLINE DECLARATIONS
ATbool traverse_InlineDeclaration(ATerm term);

// 4.0 STATEMENTS
ATbool traverse_Statement(ATerm term);
ATbool traverse_StatementList(ATerm term);
ATbool traverse_SimpleStatement(ATerm term);
ATbool traverse_CompoundStatement(ATerm term);
ATbool traverse_NullStatement(ATerm term, const std::vector<std::string> &labels);
ATbool traverse_NullBlockStatement(ATerm term);
ATbool traverse_LabelList(ATerm term, std::vector<std::string> &labels, std::vector<PosInfo> &locations);

// 4.1 ASSIGNMENT STATEMENTS
ATbool traverse_AssignmentStatement(ATerm term, std::vector<std::string> &labels);

// 4.2 LOOP STATEMENTS
ATbool traverse_WhileStatement  (ATerm term);
ATbool traverse_ForStatement    (ATerm term);
ATbool traverse_ForClause       (ATerm term, SgExpression* &var_ref, SgExpression* &init,
                                             SgExpression* &phrase1, SgExpression* &phrase2,
                                             int &phrase1_enum, int &phrase2_enum);
ATbool traverse_ControlClause   (ATerm term, SgExpression* &initial_value,
                                             SgExpression* &phrase1, SgExpression* &phrase2,
                                             int &phrase1_enum, int &phrase2_enum);
ATbool traverse_OptContinuation (ATerm term, SgExpression* &phrase1, SgExpression* &phrase2,
                                             int &phrase1_enum, int &phrase2_enum);
ATbool traverse_Continuation    (ATerm term, SgExpression* &phrase1, SgExpression* &phrase2,
                                             int &phrase1_enum, int &phrase2_enum);
ATbool traverse_Phrase          (ATerm term, SgExpression* &expr, int &phrase_enum);

// 4.3 IF STATEMENTS
ATbool traverse_IfStatement     (ATerm term);

// 4.4 CASE STATEMENTS
ATbool traverse_CaseStatement   (ATerm term);
ATbool traverse_CaseBody        (ATerm term);
ATbool traverse_CaseAlternative (ATerm term);
ATbool traverse_DefaultOption   (ATerm term);
ATbool traverse_CaseIndexGroup  (ATerm term, SgExprListExp* &key);
ATbool traverse_CaseIndex       (ATerm term, SgExpression*  &index);

// 4.5 PROCEDURE CALL STATEMENTS
ATbool traverse_ProcedureCallStatement (ATerm term);
ATbool traverse_ActualParameterList    (ATerm term, SgExprListExp* param_list);
ATbool traverse_ActualOutputParameters (ATerm term, SgExprListExp* param_list);

// 4.6 RETURN STATEMENTS
ATbool traverse_ReturnStatement (ATerm term);

// 4.7 GOTO STATEMENTS
ATbool traverse_GotoStatement   (ATerm term);

// 4.8 EXIT STATEMENTS
ATbool traverse_ExitStatement   (ATerm term);

// 4.9 STOP STATEMENTS
ATbool traverse_StopStatement   (ATerm term);

// 4.10 ABORT STATEMENTS
ATbool traverse_AbortStatement  (ATerm term);

// 5.0 FORMULAS
ATbool traverse_Formula(ATerm term, SgExpression* &expr);

ATbool traverse_Literal(ATerm term, SgExpression* &expr);
ATbool traverse_Parens (ATerm term, SgExpression* &expr);
ATbool traverse_UnaryExpression (ATerm term, SgExpression* &expr);
ATbool traverse_BinaryExpression(ATerm term, SgExpression* &expr);

// 5.1 NUMERIC FORMULAS
ATbool traverse_NumericConversion(ATerm term, SgExpression* &expr);
ATbool traverse_OptSign        (ATerm term, LanguageTranslation::ExpressionKind & op_enum);

// 5.2 BIT FORMULAS
ATbool traverse_BitOrByteFormula       (ATerm term, SgExpression* &expr);
ATbool traverse_LogicalComponent       (ATerm term, SgExpression* &expr);
ATbool traverse_LogicalOperand         (ATerm term, SgExpression* &expr);
ATbool traverse_BitPrimary             (ATerm term, SgExpression* &expr);

// 5.3 CHARACTER FORMULAS
ATbool traverse_CharacterFormula (ATerm term, SgExpression* &expr);

// 5.4 STATUS FORMULAS
ATbool traverse_StatusFormula    (ATerm term, SgExpression* &expr);

// 5.5 POINTER FORMULAS
ATbool traverse_PointerFormula   (ATerm term, SgExpression* &expr);

// 6.1 VARIABLE AND BLOCK REFERENCES
ATbool traverse_Variable         (ATerm term, SgExpression* &var);
ATbool traverse_VariableLVal     (ATerm term, SgExpression* &var);
ATbool traverse_VariableList     (ATerm term, std::vector<SgExpression*> &vars);
ATbool traverse_TableItem        (ATerm term, SgExpression* &var);
ATbool traverse_Subscript        (ATerm term, std::vector<SgExpression*> & indexes);
ATbool traverse_TableDereference (ATerm term, SgExpression* &expr);
ATbool traverse_Dereference      (ATerm term, SgExpression* &expr, bool build_ptr_ref=true);
ATbool traverse_BitFunctionVariable  (ATerm term, SgExpression* &func_call);
ATbool traverse_ByteFunctionVariable (ATerm term, SgExpression* &func_call);
ATbool traverse_RepFunctionVariable  (ATerm term, SgExpression* &func_call);

// 6.2 NAMED CONSTANTS
ATbool traverse_NamedConstant        (ATerm term, SgExpression* &var);

// 6.3 FUNCTION CALLS
ATbool traverse_FunctionCall           (ATerm term, SgExpression* &expr);
ATbool traverse_UserDefinedFunctionCall(ATerm term, SgExpression* &expr);
ATbool traverse_IntrinsicFunctionCall  (ATerm term, SgFunctionCallExp* &func_call);

// 6.3.1 LOC FUNCTION
ATbool traverse_LocFunction          (ATerm term, SgFunctionCallExp* &func_call);

// 6.3.2 NEXT FUNCTION
ATbool traverse_NextFunction         (ATerm term, SgFunctionCallExp* &func_call);

// 6.3.3 BIT FUNCTION
ATbool traverse_BitFunction          (ATerm term, SgFunctionCallExp* &func_call);

// 6.3.4 BYTE FUNCTION
ATbool traverse_ByteFunction         (ATerm term, SgFunctionCallExp* &func_call);

// 6.3.5 SHIFT FUNCTION
ATbool traverse_ShiftFunction        (ATerm term, SgFunctionCallExp* &func_call);

// 6.3.6 ABS FUNCTIONS
ATbool traverse_AbsFunction          (ATerm term, SgFunctionCallExp* &func_call);

// 6.3.7 SIGN FUNCTION
ATbool traverse_SignFunction         (ATerm term, SgFunctionCallExp* &func_call);

// 6.3.8 SIZE FUNCTION
ATbool traverse_SizeFunction         (ATerm term, SgFunctionCallExp* &func_call);

// 6.3.9 BOUNDS FUNCTION
ATbool traverse_BoundsFunction       (ATerm term, SgFunctionCallExp* &func_call);

// 6.3.10 NWDSEN FUNCTION
ATbool traverse_NwdsenFunction       (ATerm term, SgFunctionCallExp* &func_call);

// 6.3.11 STATUS INVERSE FUNCTIONS
ATbool traverse_StatusInverseFunction(ATerm term, SgFunctionCallExp* &func_call);

// 6.3.12 NENT FUNCTION
ATbool traverse_NentFunction(ATerm term, SgFunctionCallExp* &func_call);

// 7.0 TYPE MATCHING AND TYPE CONVERSIONS
ATbool traverse_BitConversion        (ATerm term, SgType* &type);
ATbool traverse_IntegerConversion    (ATerm term, SgType* &type);
ATbool traverse_TypeNameConversion   (ATerm term, SgType* &type);
ATbool traverse_FloatingConversion   (ATerm term, SgType* &type);
ATbool traverse_FixedConversion      (ATerm term, SgType* &type);
ATbool traverse_CharacterConversion  (ATerm term, SgType* &type);
ATbool traverse_StatusConversion     (ATerm term, SgType* &type);
ATbool traverse_PointerConversion    (ATerm term, SgType* &type);

// 8.3.1 NUMERIC LITERAL
ATbool traverse_FixedOrFloatingLiteral (ATerm term, SgExpression* &expr);
ATbool traverse_Exponent               (ATerm term, std::string & opt_exp);

// 8.3.2 BIT LITERAL
ATbool traverse_BitLiteral     (ATerm term, SgExpression* &expr);

// 8.3.3 BOOLEAN LITERAL
ATbool traverse_BooleanLiteral (ATerm term, SgExpression* &expr);

// 8.3.4 POINTER LITERAL
ATbool traverse_PointerLiteral (ATerm term, SgExpression* &expr);

// 9.0 DIRECTIVES
ATbool traverse_DirectiveList      (ATerm term);
ATbool traverse_Directive          (ATerm term);

// 9.1 COMPOOL DIRECTIVES
ATbool traverse_CompoolDirective   (ATerm term);

// 9.2.2 SKIP, BEGIN, AND END DIRECTIVES
ATbool traverse_SkipDirective      (ATerm term);
ATbool traverse_BeginDirective     (ATerm term);
ATbool traverse_EndDirective       (ATerm term);

// 9.3 LINKAGE DIRECTIVES
ATbool traverse_LinkageDirective   (ATerm term);

// 9.6 REDUCIBLE DIRECTIVES
ATbool traverse_ReducibleDirective (ATerm term);

// 9.7.1 SOURCE-LISTING DIRECTIVES
ATbool traverse_NolistDirective    (ATerm term);
ATbool traverse_ListDirective      (ATerm term);
ATbool traverse_EjectDirective     (ATerm term);

// 9.10 INITIALIZATION DIRECTIVES
ATbool traverse_InitializeDirective(ATerm term);

// 9.11 ALLOCATION ORDER DIRECTIVES
ATbool traverse_OrderDirective     (ATerm term);

// ALWAYS'STORE Directive: non-standard, must be for odd compiler
ATbool traverse_AlignDirective     (ATerm term);
ATbool traverse_AlwaysDirective    (ATerm term);

}; // class ATermToSageJovialTraversal

  // Specifically for Jovial intrinsic functions
  SgVarRefExp* buildIntrinsicVarRefExp_nfi(const std::string &name, SgScopeStatement* scope);
  SgFunctionCallExp* buildIntrinsicFunctionCallExp_nfi(const std::string &name,
                                                       SgExprListExp* params=nullptr,
                                                       SgScopeStatement* scope=nullptr);

}  // namespace Jovial

#endif
