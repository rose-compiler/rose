#ifndef	OFP_TRAVERSAL_H
#define	OFP_TRAVERSAL_H

#undef  DEBUG_PRINT

#include <aterm2.h>

#include "OFPNodes.hpp"
#include "OFPUnparser.hpp"
#include "traverse.hpp"

extern OFP::Unparser* unparser;

#ifdef OBSOLETE
namespace OFP {
   // Terminals
   class Dop;
   class HexConstant;
   class OctalConstant;
   class BinaryConstant;
   class Rcon;
   class Scon;
   class Icon;
   class Letter;
   class StartCommentBlock;
}
#endif

enum DataType
{
   UNKNOWN,
   STRING,
   IDENT,
   LIST,
   OPTION,
   CONST,
   OR
};


/**                                                                                             
 * Section/Clause 2: Fortran concepts
 */

/* R201 */
ATbool ofp_traverse_Program(ATerm term, OFP::Program* Program);

/* R202 */
ATbool ofp_traverse_ProgramUnit(ATerm term, OFP::ProgramUnit* ProgramUnit);

/* R203 */
ATbool ofp_traverse_ExternalSubprogram(ATerm term, OFP::ExternalSubprogram* ExternalSubprogram);

/* R204 */
ATbool ofp_traverse_InitialSpecPart(ATerm term, OFP::InitialSpecPart* InitialSpecPart);
ATbool ofp_traverse_SpecificationPart(ATerm term, OFP::SpecificationPart* SpecificationPart);

/* R205 */
ATbool ofp_traverse_ImplicitPart(ATerm term, OFP::ImplicitPart* ImplicitPart);

/* R206 */
ATbool ofp_traverse_ImplicitPartStmt(ATerm term, OFP::ImplicitPartStmt* ImplicitPartStmt);

/* R207 */
ATbool ofp_traverse_DeclarationConstruct(ATerm term, OFP::DeclarationConstruct* DeclarationConstruct);

/* R208 */
ATbool ofp_traverse_SpecAndExecPart(ATerm term, OFP::SpecAndExecPart* SpecAndExecPart);
ATbool ofp_traverse_ExecutionPart(ATerm term, OFP::ExecutionPart* ExecutionPart);

/* R209 */
ATbool ofp_traverse_ExecutionPartConstruct(ATerm term, OFP::ExecutionPartConstruct* ExecutionPartConstruct);

/* R210 */
ATbool ofp_traverse_InternalSubprogramPart(ATerm term, OFP::InternalSubprogramPart* InternalSubprogramPart);

/* R211 */
ATbool ofp_traverse_InternalSubprogram(ATerm term, OFP::InternalSubprogram* InternalSubprogram);

/* R212 */
ATbool ofp_traverse_OtherSpecificationStmt(ATerm term, OFP::OtherSpecificationStmt* OtherSpecificationStmt);

/* R213 */
ATbool ofp_traverse_ExecutableConstruct(ATerm term, OFP::ExecutableConstruct* ExecutableConstruct);

/* R214 */
ATbool ofp_traverse_ActionStmt(ATerm term, OFP::ActionStmt* ActionStmt);

/* R215 */
ATbool ofp_traverse_Keyword(ATerm term, OFP::Keyword* Keyword);

/**                                                                                             
 * Section/Clause 3: Lexical tokens and source form
 */

/* R304 */
ATbool ofp_traverse_Constant(ATerm term, OFP::Constant* Constant);

/* R305 */
ATbool ofp_traverse_LiteralConstant(ATerm term, OFP::LiteralConstant* LiteralConstant);

/* R306 */
ATbool ofp_traverse_NamedConstant(ATerm term, OFP::NamedConstant* NamedConstant);

/* R312 */
ATbool ofp_traverse_LblRef(ATerm term, OFP::LblRef* LblRef);

/**                                                                                             
 * Section/Clause 4: Types                                                                      
 */

/* R401 */
ATbool ofp_traverse_TypeParamValue(ATerm term, OFP::TypeParamValue* TypeParamValue);

/* R402 */
ATbool ofp_traverse_TypeSpec(ATerm term, OFP::TypeSpec* TypeSpec);

/* R403 */
ATbool ofp_traverse_DeclarationTypeSpec(ATerm term, OFP::DeclarationTypeSpec* DeclarationTypeSpec);

/* R404 */
ATbool ofp_traverse_IntrinsicTypeSpec(ATerm term, OFP::IntrinsicTypeSpec* IntrinsicTypeSpec);

/* R405 */
ATbool ofp_traverse_KindSelector(ATerm term, OFP::KindSelector* KindSelector);

/* R406 */
ATbool ofp_traverse_SignedIntLiteralConstant(ATerm term, OFP::SignedIntLiteralConstant* SignedIntLiteralConstant);

/* R407 */
ATbool ofp_traverse_IntLiteralConstant(ATerm term, OFP::IntLiteralConstant* IntLiteralConstant);

/* R408 */
ATbool ofp_traverse_KindParam(ATerm term, OFP::KindParam* KindParam);

/* R409 */

/* R410 */
ATbool ofp_traverse_DigitString(ATerm term, OFP::DigitString* DigitString);

/* R411 */
ATbool ofp_traverse_Sign(ATerm term, OFP::Sign* Sign);

/* R412 */
ATbool ofp_traverse_SignedRealLiteralConstant(ATerm term, OFP::SignedRealLiteralConstant* SignedRealLiteralConstant);

/* R413 */
ATbool ofp_traverse_RealLiteralConstant(ATerm term, OFP::RealLiteralConstant* RealLiteralConstant);

/* R417 */
ATbool ofp_traverse_ComplexLiteralConstant(ATerm term, OFP::ComplexLiteralConstant* ComplexLiteralConstant);

/* R418 */
ATbool ofp_traverse_RealPart(ATerm term, OFP::RealPart* RealPart);

/* R419 */
ATbool ofp_traverse_ImagPart(ATerm term, OFP::ImagPart* ImagPart);

/* R420 */
ATbool ofp_traverse_CharSelector(ATerm term, OFP::CharSelector* CharSelector);

/* R421 */
ATbool ofp_traverse_LengthSelector(ATerm term, OFP::LengthSelector* LengthSelector);

/* R422 */
ATbool ofp_traverse_CharLength(ATerm term, OFP::CharLength* CharLength);

/* R423 */
ATbool ofp_traverse_CharLiteralConstant(ATerm term, OFP::CharLiteralConstant* CharLiteralConstant);

/* R424 */
ATbool ofp_traverse_LogicalLiteralConstant(ATerm term, OFP::LogicalLiteralConstant* LogicalLiteralConstant);

/* R425 */
ATbool ofp_traverse_DerivedTypeDef(ATerm term, OFP::DerivedTypeDef* DerivedTypeDef);

/* R426 */
ATbool ofp_traverse_DerivedTypeStmt(ATerm term, OFP::DerivedTypeStmt* DerivedTypeStmt);
ATbool ofp_traverse_OptTypeAttrSpecList(ATerm term, OFP::OptTypeAttrSpecList* OptTypeAttrSpecList);
ATbool ofp_traverse_OptTypeParamNameList(ATerm term, OFP::OptTypeParamNameList* OptTypeParamNameList);

/* R427 */
ATbool ofp_traverse_TypeAttrSpec(ATerm term, OFP::TypeAttrSpec* TypeAttrSpec);
ATbool ofp_traverse_TypeAttrSpecList(ATerm term, OFP::TypeAttrSpecList* TypeAttrSpecList);
ATbool ofp_traverse_TypeParamNameList(ATerm term, OFP::TypeParamNameList* TypeParamNameList);

/* R428 */
ATbool ofp_traverse_PrivateOrSequence(ATerm term, OFP::PrivateOrSequence* PrivateOrSequence);

/* R429 */
ATbool ofp_traverse_EndTypeStmt(ATerm term, OFP::EndTypeStmt* EndTypeStmt);

/* R430 */
ATbool ofp_traverse_SequenceStmt(ATerm term, OFP::SequenceStmt* SequenceStmt);

/* R431 */
ATbool ofp_traverse_TypeParamDefStmt(ATerm term, OFP::TypeParamDefStmt* TypeParamDefStmt);

/* R432 */
ATbool ofp_traverse_TypeParamDecl(ATerm term, OFP::TypeParamDecl* TypeParamDecl);
ATbool ofp_traverse_TypeParamDeclList(ATerm term, OFP::TypeParamDeclList* TypeParamDeclList);

/* R433 */
ATbool ofp_traverse_TypeParamAttrSpec(ATerm term, OFP::TypeParamAttrSpec* TypeParamAttrSpec);

/* R434 */
ATbool ofp_traverse_ComponentPart(ATerm term, OFP::ComponentPart* ComponentPart);

/* R435 */
ATbool ofp_traverse_ComponentDefStmt(ATerm term, OFP::ComponentDefStmt* ComponentDefStmt);

/* R436 */
ATbool ofp_traverse_DataComponentDefStmt(ATerm term, OFP::DataComponentDefStmt* DataComponentDefStmt);
ATbool ofp_traverse_OptComponentAttrSpecList(ATerm term, OFP::OptComponentAttrSpecList* OptComponentAttrSpecList);

/* R437 */
ATbool ofp_traverse_ComponentAttrSpec(ATerm term, OFP::ComponentAttrSpec* ComponentAttrSpec);
ATbool ofp_traverse_ComponentAttrSpecList(ATerm term, OFP::ComponentAttrSpecList* ComponentAttrSpecList);

/* R438 */
ATbool ofp_traverse_ComponentDecl(ATerm term, OFP::ComponentDecl* ComponentDecl);
ATbool ofp_traverse_ComponentDeclList(ATerm term, OFP::ComponentDeclList* ComponentDeclList);

/* R439 */
ATbool ofp_traverse_ComponentArraySpec(ATerm term, OFP::ComponentArraySpec* ComponentArraySpec);

/* R440 */
ATbool ofp_traverse_ProcComponentDefStmt(ATerm term, OFP::ProcComponentDefStmt* ProcComponentDefStmt);

/* R441 */
ATbool ofp_traverse_ProcComponentAttrSpec(ATerm term, OFP::ProcComponentAttrSpec* ProcComponentAttrSpec);
ATbool ofp_traverse_ProcComponentAttrSpecList(ATerm term, OFP::ProcComponentAttrSpecList* ProcComponentAttrSpecList);

/* R442 */
ATbool ofp_traverse_ComponentInitialization(ATerm term, OFP::ComponentInitialization* ComponentInitialization);

/* R443 */
ATbool ofp_traverse_InitialDataTarget(ATerm term, OFP::InitialDataTarget* InitialDataTarget);

/* R444 */
ATbool ofp_traverse_PrivateComponentsStmt(ATerm term, OFP::PrivateComponentsStmt* PrivateComponentsStmt);

/* R445 */
ATbool ofp_traverse_TypeBoundProcedurePart(ATerm term, OFP::TypeBoundProcedurePart* TypeBoundProcedurePart);

/* R446 */
ATbool ofp_traverse_BindingPrivateStmt(ATerm term, OFP::BindingPrivateStmt* BindingPrivateStmt);

/* R447 */
ATbool ofp_traverse_TypeBoundProcBinding(ATerm term, OFP::TypeBoundProcBinding* TypeBoundProcBinding);

/* R448 */
ATbool ofp_traverse_TypeBoundProcedureStmt(ATerm term, OFP::TypeBoundProcedureStmt* TypeBoundProcedureStmt);
ATbool ofp_traverse_BindingNameList(ATerm term, OFP::BindingNameList* BindingNameList);
ATbool ofp_traverse_BindingAttrList(ATerm term, OFP::BindingAttrList* BindingAttrList);

/* R449 */
ATbool ofp_traverse_TypeBoundProcDecl(ATerm term, OFP::TypeBoundProcDecl* TypeBoundProcDecl);
ATbool ofp_traverse_TypeBoundProcDeclList(ATerm term, OFP::TypeBoundProcDeclList* TypeBoundProcDeclList);

/* R450 */
ATbool ofp_traverse_TypeBoundGenericStmt(ATerm term, OFP::TypeBoundGenericStmt* TypeBoundGenericStmt);

/* R451 */
ATbool ofp_traverse_BindingAttr(ATerm term, OFP::BindingAttr* BindingAttr);

/* R452 */
ATbool ofp_traverse_FinalProcedureStmt(ATerm term, OFP::FinalProcedureStmt* FinalProcedureStmt);
ATbool ofp_traverse_FinalSubroutineNameList(ATerm term, OFP::FinalSubroutineNameList* FinalSubroutineNameList);

/* R453 */
ATbool ofp_traverse_DerivedTypeSpec(ATerm term, OFP::DerivedTypeSpec* DerivedTypeSpec);

/* R454 */
ATbool ofp_traverse_TypeParamSpec(ATerm term, OFP::TypeParamSpec* TypeParamSpec);
ATbool ofp_traverse_TypeParamSpecList(ATerm term, OFP::TypeParamSpecList* TypeParamSpecList);

/* R455 */
ATbool ofp_traverse_StructureConstructor(ATerm term, OFP::StructureConstructor* StructureConstructor);

/* R456 */
ATbool ofp_traverse_ComponentSpec(ATerm term, OFP::ComponentSpec* ComponentSpec);

/* R457 */
ATbool ofp_traverse_ComponentDataSource(ATerm term, OFP::ComponentDataSource* ComponentDataSource);

/* R458 */
ATbool ofp_traverse_EnumDef(ATerm term, OFP::EnumDef* EnumDef);

/* R459 */
ATbool ofp_traverse_EnumDefStmt(ATerm term, OFP::EnumDefStmt* EnumDefStmt);

/* R460 */
ATbool ofp_traverse_EnumeratorDefStmt(ATerm term, OFP::EnumeratorDefStmt* EnumeratorDefStmt);

/* R461 */
ATbool ofp_traverse_Enumerator(ATerm term, OFP::Enumerator* Enumerator);
ATbool ofp_traverse_EnumeratorList(ATerm term, OFP::EnumeratorList* EnumeratorList);

/* R462 */
ATbool ofp_traverse_EndEnumStmt(ATerm term, OFP::EndEnumStmt* EndEnumStmt);

/* R463 */
ATbool ofp_traverse_BozLiteralConstant(ATerm term, OFP::BozLiteralConstant* BozLiteralConstant);

/* R464 */
/* R465 */
/* R466 */
/* R467 */

/* R468 */
ATbool ofp_traverse_ArrayConstructor(ATerm term, OFP::ArrayConstructor* ArrayConstructor);

/* R469 */
ATbool ofp_traverse_AcSpec(ATerm term, OFP::AcSpec* AcSpec);

/* R470 */
ATbool ofp_traverse_AcValue(ATerm term, OFP::AcValue* AcValue);

/* R471 */
ATbool ofp_traverse_AcValueList(ATerm term, OFP::AcValueList* AcValueList);

/* R472 */
ATbool ofp_traverse_AcImpliedDo(ATerm term, OFP::AcImpliedDo* AcImpliedDo);

/* R473 */
ATbool ofp_traverse_AcImpliedDoControl(ATerm term, OFP::AcImpliedDoControl* AcImpliedDoControl);

/* R474 */
ATbool ofp_traverse_AcDoVariable(ATerm term, OFP::AcDoVariable* AcDoVariable);

/* R501 */
ATbool ofp_traverse_TypeDeclarationStmt(ATerm term, OFP::TypeDeclarationStmt* TypeDeclarationStmt);

/* R502 */
ATbool ofp_traverse_AttrSpec(ATerm term, OFP::AttrSpec* AttrSpec);
ATbool ofp_traverse_AttrSpecList(ATerm term, OFP::AttrSpecList* AttrSpecList);
ATbool ofp_traverse_AttrSpecList(ATerm term, OFP::AttrSpecList* AttrSpecList);
ATbool ofp_traverse_OptAttrSpecList(ATerm term, OFP::OptAttrSpecList* OptAttrSpecList);
ATbool ofp_traverse_OptAttrSpecList(ATerm term, OFP::OptAttrSpecList* OptAttrSpecList);

/* R503 */
ATbool ofp_traverse_EntityDecl(ATerm term, OFP::EntityDecl* EntityDecl);
ATbool ofp_traverse_EntityDeclList(ATerm term, OFP::EntityDeclList* EntityDeclList);
ATbool ofp_traverse_Initialization(ATerm term, OFP::Initialization* Initialization);
ATbool ofp_traverse_NullInit(ATerm term, OFP::NullInit* NullInit);
ATbool ofp_traverse_AccessSpec(ATerm term, OFP::AccessSpec* AccessSpec);
ATbool ofp_traverse_LanguageBindingSpec(ATerm term, OFP::LanguageBindingSpec* LanguageBindingSpec);
ATbool ofp_traverse_CoarraySpec(ATerm term, OFP::CoarraySpec* CoarraySpec);
ATbool ofp_traverse_DeferredCoshapeSpec(ATerm term, OFP::DeferredCoshapeSpec* DeferredCoshapeSpec);
ATbool ofp_traverse_DeferredCoshapeSpecList(ATerm term, OFP::DeferredCoshapeSpecList* DeferredCoshapeSpecList);
ATbool ofp_traverse_ExplicitCoshapeSpec(ATerm term, OFP::ExplicitCoshapeSpec* ExplicitCoshapeSpec);
ATbool ofp_traverse_CoboundsEntry(ATerm term, OFP::CoboundsEntry* CoboundsEntry);
ATbool ofp_traverse_LowerCobound(ATerm term, OFP::LowerCobound* LowerCobound);
ATbool ofp_traverse_UpperCobound(ATerm term, OFP::UpperCobound* UpperCobound);
ATbool ofp_traverse_ArraySpec(ATerm term, OFP::ArraySpec* ArraySpec);
ATbool ofp_traverse_ExplicitShapeSpec(ATerm term, OFP::ExplicitShapeSpec* ExplicitShapeSpec);
ATbool ofp_traverse_ExplicitShapeSpecList(ATerm term, OFP::ExplicitShapeSpecList* ExplicitShapeSpecList);
ATbool ofp_traverse_LowerBound(ATerm term, OFP::LowerBound* LowerBound);
ATbool ofp_traverse_UpperBound(ATerm term, OFP::UpperBound* UpperBound);
ATbool ofp_traverse_AssumedShapeSpec(ATerm term, OFP::AssumedShapeSpec* AssumedShapeSpec);
ATbool ofp_traverse_AssumedShapeSpecList(ATerm term, OFP::AssumedShapeSpecList* AssumedShapeSpecList);
ATbool ofp_traverse_DeferredShapeSpec(ATerm term, OFP::DeferredShapeSpec* DeferredShapeSpec);
ATbool ofp_traverse_DeferredShapeSpecList(ATerm term, OFP::DeferredShapeSpecList* DeferredShapeSpecList);
ATbool ofp_traverse_AssumedSizeSpec(ATerm term, OFP::AssumedSizeSpec* AssumedSizeSpec);
ATbool ofp_traverse_AssumedRankSpec(ATerm term, OFP::AssumedRankSpec* AssumedRankSpec);
ATbool ofp_traverse_ImpliedShapeSpec(ATerm term, OFP::ImpliedShapeSpec* ImpliedShapeSpec);
ATbool ofp_traverse_ImpliedShapeSpecList(ATerm term, OFP::ImpliedShapeSpecList* ImpliedShapeSpecList);
ATbool ofp_traverse_IntentSpec(ATerm term, OFP::IntentSpec* IntentSpec);
ATbool ofp_traverse_AccessStmt(ATerm term, OFP::AccessStmt* AccessStmt);
ATbool ofp_traverse_AccessId(ATerm term, OFP::AccessId* AccessId);
ATbool ofp_traverse_AccessIdList(ATerm term, OFP::AccessIdList* AccessIdList);
ATbool ofp_traverse_AllocatableStmt(ATerm term, OFP::AllocatableStmt* AllocatableStmt);
ATbool ofp_traverse_AllocatableDecl(ATerm term, OFP::AllocatableDecl* AllocatableDecl);
ATbool ofp_traverse_AllocatableDeclList(ATerm term, OFP::AllocatableDeclList* AllocatableDeclList);
ATbool ofp_traverse_AsynchronousStmt(ATerm term, OFP::AsynchronousStmt* AsynchronousStmt);
ATbool ofp_traverse_BindStmt(ATerm term, OFP::BindStmt* BindStmt);
ATbool ofp_traverse_BindEntity(ATerm term, OFP::BindEntity* BindEntity);
ATbool ofp_traverse_BindEntityList(ATerm term, OFP::BindEntityList* BindEntityList);
ATbool ofp_traverse_CodimensionStmt(ATerm term, OFP::CodimensionStmt* CodimensionStmt);
ATbool ofp_traverse_CodimensionDecl(ATerm term, OFP::CodimensionDecl* CodimensionDecl);
ATbool ofp_traverse_CodimensionDeclList(ATerm term, OFP::CodimensionDeclList* CodimensionDeclList);
ATbool ofp_traverse_ContiguousStmt(ATerm term, OFP::ContiguousStmt* ContiguousStmt);
ATbool ofp_traverse_ObjectNameList(ATerm term, OFP::ObjectNameList* ObjectNameList);
ATbool ofp_traverse_DataStmt(ATerm term, OFP::DataStmt* DataStmt);
ATbool ofp_traverse_DataStmtSet(ATerm term, OFP::DataStmtSet* DataStmtSet);
ATbool ofp_traverse_DataStmtSetList(ATerm term, OFP::DataStmtSetList* DataStmtSetList);
ATbool ofp_traverse_DataStmtObject(ATerm term, OFP::DataStmtObject* DataStmtObject);
ATbool ofp_traverse_DataStmtObjectList(ATerm term, OFP::DataStmtObjectList* DataStmtObjectList);
ATbool ofp_traverse_DataImpliedDo(ATerm term, OFP::DataImpliedDo* DataImpliedDo);
ATbool ofp_traverse_DataIDoObject(ATerm term, OFP::DataIDoObject* DataIDoObject);
ATbool ofp_traverse_DataIDoObjectList(ATerm term, OFP::DataIDoObjectList* DataIDoObjectList);
ATbool ofp_traverse_DataIDoVariable(ATerm term, OFP::DataIDoVariable* DataIDoVariable);
ATbool ofp_traverse_DataStmtValue(ATerm term, OFP::DataStmtValue* DataStmtValue);
ATbool ofp_traverse_DataStmtValueList(ATerm term, OFP::DataStmtValueList* DataStmtValueList);
ATbool ofp_traverse_DataStmtRepeat(ATerm term, OFP::DataStmtRepeat* DataStmtRepeat);
ATbool ofp_traverse_DataStmtConstant(ATerm term, OFP::DataStmtConstant* DataStmtConstant);
ATbool ofp_traverse_IntConstantSubobject(ATerm term, OFP::IntConstantSubobject* IntConstantSubobject);
ATbool ofp_traverse_ConstantSubobject(ATerm term, OFP::ConstantSubobject* ConstantSubobject);
ATbool ofp_traverse_DimensionStmt(ATerm term, OFP::DimensionStmt* DimensionStmt);
ATbool ofp_traverse_ArrayNameSpec(ATerm term, OFP::ArrayNameSpec* ArrayNameSpec);
ATbool ofp_traverse_ArrayNameSpecList(ATerm term, OFP::ArrayNameSpecList* ArrayNameSpecList);
ATbool ofp_traverse_IntentStmt(ATerm term, OFP::IntentStmt* IntentStmt);
ATbool ofp_traverse_DummyArgNameList(ATerm term, OFP::DummyArgNameList* DummyArgNameList);
ATbool ofp_traverse_OptionalStmt(ATerm term, OFP::OptionalStmt* OptionalStmt);
ATbool ofp_traverse_ParameterStmt(ATerm term, OFP::ParameterStmt* ParameterStmt);
ATbool ofp_traverse_NamedConstantDef(ATerm term, OFP::NamedConstantDef* NamedConstantDef);
ATbool ofp_traverse_NamedConstantDefList(ATerm term, OFP::NamedConstantDefList* NamedConstantDefList);
ATbool ofp_traverse_PointerStmt(ATerm term, OFP::PointerStmt* PointerStmt);
ATbool ofp_traverse_PointerDecl(ATerm term, OFP::PointerDecl* PointerDecl);
ATbool ofp_traverse_PointerDeclList(ATerm term, OFP::PointerDeclList* PointerDeclList);
ATbool ofp_traverse_ProtectedStmt(ATerm term, OFP::ProtectedStmt* ProtectedStmt);
ATbool ofp_traverse_EntityNameList(ATerm term, OFP::EntityNameList* EntityNameList);
ATbool ofp_traverse_SaveStmt(ATerm term, OFP::SaveStmt* SaveStmt);
ATbool ofp_traverse_SavedEntity(ATerm term, OFP::SavedEntity* SavedEntity);
ATbool ofp_traverse_SavedEntityList(ATerm term, OFP::SavedEntityList* SavedEntityList);
ATbool ofp_traverse_ProcPointerName(ATerm term, OFP::ProcPointerName* ProcPointerName);
ATbool ofp_traverse_TargetStmt(ATerm term, OFP::TargetStmt* TargetStmt);
ATbool ofp_traverse_TargetDecl(ATerm term, OFP::TargetDecl* TargetDecl);
ATbool ofp_traverse_TargetDeclList(ATerm term, OFP::TargetDeclList* TargetDeclList);
ATbool ofp_traverse_ValueStmt(ATerm term, OFP::ValueStmt* ValueStmt);
ATbool ofp_traverse_VolatileStmt(ATerm term, OFP::VolatileStmt* VolatileStmt);
ATbool ofp_traverse_ImplicitStmt(ATerm term, OFP::ImplicitStmt* ImplicitStmt);
ATbool ofp_traverse_ImplicitSpec(ATerm term, OFP::ImplicitSpec* ImplicitSpec);
ATbool ofp_traverse_ImplicitSpecList(ATerm term, OFP::ImplicitSpecList* ImplicitSpecList);
ATbool ofp_traverse_LetterSpec(ATerm term, OFP::LetterSpec* LetterSpec);
ATbool ofp_traverse_LetterSpecList(ATerm term, OFP::LetterSpecList* LetterSpecList);
ATbool ofp_traverse_NamelistStmt(ATerm term, OFP::NamelistStmt* NamelistStmt);
ATbool ofp_traverse_NamelistEntry(ATerm term, OFP::NamelistEntry* NamelistEntry);
ATbool ofp_traverse_NamelistEntryList(ATerm term, OFP::NamelistEntryList* NamelistEntryList);
ATbool ofp_traverse_NamelistGroupObject(ATerm term, OFP::NamelistGroupObject* NamelistGroupObject);
ATbool ofp_traverse_NamelistGroupObjectList(ATerm term, OFP::NamelistGroupObjectList* NamelistGroupObjectList);
ATbool ofp_traverse_EquivalenceStmt(ATerm term, OFP::EquivalenceStmt* EquivalenceStmt);
ATbool ofp_traverse_EquivalenceSet(ATerm term, OFP::EquivalenceSet* EquivalenceSet);
ATbool ofp_traverse_EquivalenceSetList(ATerm term, OFP::EquivalenceSetList* EquivalenceSetList);
ATbool ofp_traverse_EquivalenceObject(ATerm term, OFP::EquivalenceObject* EquivalenceObject);
ATbool ofp_traverse_EquivalenceObjectList(ATerm term, OFP::EquivalenceObjectList* EquivalenceObjectList);
ATbool ofp_traverse_CommonStmt(ATerm term, OFP::CommonStmt* CommonStmt);
ATbool ofp_traverse_CommonBlockEntry1(ATerm term, OFP::CommonBlockEntry1* CommonBlockEntry1);
ATbool ofp_traverse_CommonBlockEntry(ATerm term, OFP::CommonBlockEntry* CommonBlockEntry);
ATbool ofp_traverse_CommonBlockObject(ATerm term, OFP::CommonBlockObject* CommonBlockObject);
ATbool ofp_traverse_CommonBlockObjectList(ATerm term, OFP::CommonBlockObjectList* CommonBlockObjectList);

/* R601 */
ATbool ofp_traverse_Designator(ATerm term, OFP::Designator* Designator);

/* R602 */
ATbool ofp_traverse_Variable(ATerm term, OFP::Variable* Variable);

/* R603 */
ATbool ofp_traverse_VariableName(ATerm term, OFP::VariableName* VariableName);

/* R604 */
ATbool ofp_traverse_LogicalVariable(ATerm term, OFP::LogicalVariable* LogicalVariable);

/* R605 */
ATbool ofp_traverse_CharVariable(ATerm term, OFP::CharVariable* CharVariable);

/* R606 */
ATbool ofp_traverse_DefaultCharVariable(ATerm term, OFP::DefaultCharVariable* DefaultCharVariable);

/* R607 */
ATbool ofp_traverse_IntVariable(ATerm term, OFP::IntVariable* IntVariable);

/* R608 */
ATbool ofp_traverse_Substring(ATerm term, OFP::Substring* Substring);

/* R609 */
ATbool ofp_traverse_ParentString(ATerm term, OFP::ParentString* ParentString);

/* R610 */
ATbool ofp_traverse_SubstringRange(ATerm term, OFP::SubstringRange* SubstringRange);

/* R611 */
ATbool ofp_traverse_DataRef(ATerm term, OFP::DataRef* DataRef);

/* R612 */
ATbool ofp_traverse_PartRef(ATerm term, OFP::PartRef* PartRef);

/* R613 */
ATbool ofp_traverse_StructureComponent(ATerm term, OFP::StructureComponent* StructureComponent);

/* R614 */
ATbool ofp_traverse_CoindexedNamedObject(ATerm term, OFP::CoindexedNamedObject* CoindexedNamedObject);

/* R615 ComplexPartDesignator */

/* R616 */
ATbool ofp_traverse_TypeParamInquiry(ATerm term, OFP::TypeParamInquiry* TypeParamInquiry);

ATbool ofp_traverse_ArrayElement(ATerm term, OFP::ArrayElement* ArrayElement);
ATbool ofp_traverse_Subscript(ATerm term, OFP::Subscript* Subscript);
ATbool ofp_traverse_SectionSubscript(ATerm term, OFP::SectionSubscript* SectionSubscript);
ATbool ofp_traverse_SectionSubscriptList(ATerm term, OFP::SectionSubscriptList* SectionSubscriptList);
ATbool ofp_traverse_SubscriptTriplet(ATerm term, OFP::SubscriptTriplet* SubscriptTriplet);
ATbool ofp_traverse_Stride(ATerm term, OFP::Stride* Stride);
ATbool ofp_traverse_ImageSelector(ATerm term, OFP::ImageSelector* ImageSelector);
ATbool ofp_traverse_Cosubscript(ATerm term, OFP::Cosubscript* Cosubscript);
ATbool ofp_traverse_CosubscriptList(ATerm term, OFP::CosubscriptList* CosubscriptList);
ATbool ofp_traverse_AllocateStmt(ATerm term, OFP::AllocateStmt* AllocateStmt);
ATbool ofp_traverse_AllocOpt(ATerm term, OFP::AllocOpt* AllocOpt);
ATbool ofp_traverse_AllocOptList(ATerm term, OFP::AllocOptList* AllocOptList);
ATbool ofp_traverse_StatVariable(ATerm term, OFP::StatVariable* StatVariable);
ATbool ofp_traverse_ErrmsgVariable(ATerm term, OFP::ErrmsgVariable* ErrmsgVariable);
ATbool ofp_traverse_Allocation(ATerm term, OFP::Allocation* Allocation);
ATbool ofp_traverse_AllocationList(ATerm term, OFP::AllocationList* AllocationList);
ATbool ofp_traverse_AllocateObject(ATerm term, OFP::AllocateObject* AllocateObject);
ATbool ofp_traverse_AllocateObjectList(ATerm term, OFP::AllocateObjectList* AllocateObjectList);
ATbool ofp_traverse_AllocateShapeSpec(ATerm term, OFP::AllocateShapeSpec* AllocateShapeSpec);
ATbool ofp_traverse_AllocateShapeSpecList(ATerm term, OFP::AllocateShapeSpecList* AllocateShapeSpecList);
ATbool ofp_traverse_LowerBoundExpr(ATerm term, OFP::LowerBoundExpr* LowerBoundExpr);
ATbool ofp_traverse_UpperBoundExpr(ATerm term, OFP::UpperBoundExpr* UpperBoundExpr);
ATbool ofp_traverse_AllocateCoarraySpec(ATerm term, OFP::AllocateCoarraySpec* AllocateCoarraySpec);
ATbool ofp_traverse_AllocateCoshapeSpec(ATerm term, OFP::AllocateCoshapeSpec* AllocateCoshapeSpec);
ATbool ofp_traverse_AllocateCoshapeSpecList(ATerm term, OFP::AllocateCoshapeSpecList* AllocateCoshapeSpecList);
ATbool ofp_traverse_NullifyStmt(ATerm term, OFP::NullifyStmt* NullifyStmt);
ATbool ofp_traverse_PointerObject(ATerm term, OFP::PointerObject* PointerObject);
ATbool ofp_traverse_PointerObjectList(ATerm term, OFP::PointerObjectList* PointerObjectList);
ATbool ofp_traverse_DeallocateStmt(ATerm term, OFP::DeallocateStmt* DeallocateStmt);
ATbool ofp_traverse_DeallocOpt(ATerm term, OFP::DeallocOpt* DeallocOpt);
ATbool ofp_traverse_DeallocOptList(ATerm term, OFP::DeallocOptList* DeallocOptList);

ATbool ofp_traverse_IntrinsicOperator(ATerm term, OFP::IntrinsicOperator* IntrinsicOperator);
ATbool ofp_traverse_DefinedOperator(ATerm term, OFP::DefinedOperator* DefinedOperator);
ATbool ofp_traverse_ExtendedIntrinsicOp(ATerm term, OFP::ExtendedIntrinsicOp* ExtendedIntrinsicOp);
ATbool ofp_traverse_Primary(ATerm term, OFP::Primary* Primary);
ATbool ofp_traverse_DefinedUnaryOp(ATerm term, OFP::DefinedUnaryOp* DefinedUnaryOp);

/* R722 */
ATbool ofp_traverse_Expr(ATerm term, OFP::Expr* Expr);

ATbool ofp_traverse_DefinedBinaryOp(ATerm term, OFP::DefinedBinaryOp* DefinedBinaryOp);
ATbool ofp_traverse_LogicalExpr(ATerm term, OFP::LogicalExpr* LogicalExpr);
ATbool ofp_traverse_DefaultCharExpr(ATerm term, OFP::DefaultCharExpr* DefaultCharExpr);
ATbool ofp_traverse_IntExpr(ATerm term, OFP::IntExpr* IntExpr);
ATbool ofp_traverse_SpecificationExpr(ATerm term, OFP::SpecificationExpr* SpecificationExpr);
ATbool ofp_traverse_ConstantExpr(ATerm term, OFP::ConstantExpr* ConstantExpr);
ATbool ofp_traverse_IntConstantExpr(ATerm term, OFP::IntConstantExpr* IntConstantExpr);

/* R732 */
ATbool ofp_traverse_AssignmentStmt(ATerm term, OFP::AssignmentStmt* AssignmentStmt);

ATbool ofp_traverse_PointerAssignmentStmt(ATerm term, OFP::PointerAssignmentStmt* PointerAssignmentStmt);
ATbool ofp_traverse_DataPointerObject(ATerm term, OFP::DataPointerObject* DataPointerObject);
ATbool ofp_traverse_BoundsSpec(ATerm term, OFP::BoundsSpec* BoundsSpec);
ATbool ofp_traverse_BoundsSpecList(ATerm term, OFP::BoundsSpecList* BoundsSpecList);
ATbool ofp_traverse_BoundsRemapping(ATerm term, OFP::BoundsRemapping* BoundsRemapping);
ATbool ofp_traverse_BoundsRemappingList(ATerm term, OFP::BoundsRemappingList* BoundsRemappingList);
ATbool ofp_traverse_DataTarget(ATerm term, OFP::DataTarget* DataTarget);
ATbool ofp_traverse_ProcPointerObject(ATerm term, OFP::ProcPointerObject* ProcPointerObject);
ATbool ofp_traverse_ProcComponentRef(ATerm term, OFP::ProcComponentRef* ProcComponentRef);
ATbool ofp_traverse_ProcTarget(ATerm term, OFP::ProcTarget* ProcTarget);
ATbool ofp_traverse_WhereStmt(ATerm term, OFP::WhereStmt* WhereStmt);

/* R742 */
ATbool ofp_traverse_WhereConstruct(ATerm term, OFP::WhereConstruct* WhereConstruct);
ATbool ofp_traverse_MaskedElsewhereClause(ATerm term, OFP::MaskedElsewhereClause* MaskedElsewhereClause);
ATbool ofp_traverse_ElsewhereClause(ATerm term, OFP::ElsewhereClause* ElsewhereClause);

ATbool ofp_traverse_WhereConstructStmt(ATerm term, OFP::WhereConstructStmt* WhereConstructStmt);
ATbool ofp_traverse_WhereBodyConstruct(ATerm term, OFP::WhereBodyConstruct* WhereBodyConstruct);
ATbool ofp_traverse_WhereAssignmentStmt(ATerm term, OFP::WhereAssignmentStmt* WhereAssignmentStmt);
ATbool ofp_traverse_MaskExpr(ATerm term, OFP::MaskExpr* MaskExpr);
ATbool ofp_traverse_MaskedElsewhereStmt(ATerm term, OFP::MaskedElsewhereStmt* MaskedElsewhereStmt);
ATbool ofp_traverse_ElsewhereStmt(ATerm term, OFP::ElsewhereStmt* ElsewhereStmt);
ATbool ofp_traverse_EndWhereStmt(ATerm term, OFP::EndWhereStmt* EndWhereStmt);
ATbool ofp_traverse_ForallConstruct(ATerm term, OFP::ForallConstruct* ForallConstruct);
ATbool ofp_traverse_ForallConstructStmt(ATerm term, OFP::ForallConstructStmt* ForallConstructStmt);
ATbool ofp_traverse_ForallHeader(ATerm term, OFP::ForallHeader* ForallHeader);
ATbool ofp_traverse_ForallTripletSpec(ATerm term, OFP::ForallTripletSpec* ForallTripletSpec);
ATbool ofp_traverse_ForallTripletSpecList(ATerm term, OFP::ForallTripletSpecList* ForallTripletSpecList);
ATbool ofp_traverse_ForallLimit(ATerm term, OFP::ForallLimit* ForallLimit);
ATbool ofp_traverse_ForallStep(ATerm term, OFP::ForallStep* ForallStep);
ATbool ofp_traverse_ForallBodyConstruct(ATerm term, OFP::ForallBodyConstruct* ForallBodyConstruct);
ATbool ofp_traverse_ForallAssignmentStmt(ATerm term, OFP::ForallAssignmentStmt* ForallAssignmentStmt);
ATbool ofp_traverse_EndForallStmt(ATerm term, OFP::EndForallStmt* EndForallStmt);
ATbool ofp_traverse_ForallStmt(ATerm term, OFP::ForallStmt* ForallStmt);

/* R801 */
ATbool ofp_traverse_Block(ATerm term, OFP::Block* Block);

/* R802 */
ATbool ofp_traverse_AssociateConstruct(ATerm term, OFP::AssociateConstruct* AssociateConstruct);

/* R803 */
ATbool ofp_traverse_AssociateStmt(ATerm term, OFP::AssociateStmt* AssociateStmt);

/* R804 */
ATbool ofp_traverse_Association(ATerm term, OFP::Association* Association);
ATbool ofp_traverse_AssociationList(ATerm term, OFP::AssociationList* AssociationList);

/* R805 */
ATbool ofp_traverse_Selector(ATerm term, OFP::Selector* Selector);

/* R806 */
ATbool ofp_traverse_EndAssociateStmt(ATerm term, OFP::EndAssociateStmt* EndAssociateStmt);

/* R807 */
ATbool ofp_traverse_BlockConstruct(ATerm term, OFP::BlockConstruct* BlockConstruct);

/* R808 */
ATbool ofp_traverse_BlockStmt(ATerm term, OFP::BlockStmt* BlockStmt);

/* R809 */
ATbool ofp_traverse_EndBlockStmt(ATerm term, OFP::EndBlockStmt* EndBlockStmt);

/* R810 */
ATbool ofp_traverse_CriticalConstruct(ATerm term, OFP::CriticalConstruct* CriticalConstruct);

/* R811 */
ATbool ofp_traverse_CriticalStmt(ATerm term, OFP::CriticalStmt* CriticalStmt);

/* R812 */
ATbool ofp_traverse_EndCriticalStmt(ATerm term, OFP::EndCriticalStmt* EndCriticalStmt);

/* R813 */
/* R814 */
/* R815 */

/* R816 */
ATbool ofp_traverse_LabelDoStmt(ATerm term, OFP::LabelDoStmt* LabelDoStmt);

/* R817 */
ATbool ofp_traverse_NonlabelDoStmt(ATerm term, OFP::NonlabelDoStmt* NonlabelDoStmt);

/* R818 */
ATbool ofp_traverse_LoopControl(ATerm term, OFP::LoopControl* LoopControl);

/* R819 */
ATbool ofp_traverse_DoVariable(ATerm term, OFP::DoVariable* DoVariable);

/* R820 */
/* R821 */

/* R822 */
ATbool ofp_traverse_EndDoStmt(ATerm term, OFP::EndDoStmt* EndDoStmt);

/* R823 */
/* R824 */
/* R825 */
/* R826 */
/* R827 */
/* R828 */
/* R829 */
/* R830 */

/* R831 */
ATbool ofp_traverse_CycleStmt(ATerm term, OFP::CycleStmt* CycleStmt);

/* R832 */
ATbool ofp_traverse_IfConstruct(ATerm term, OFP::IfConstruct* IfConstruct);
ATbool ofp_traverse_ElseIfStmtAndBlock(ATerm term, OFP::ElseIfStmtAndBlock* ElseIfStmtAndBlock);
ATbool ofp_traverse_ElseStmtAndBlock(ATerm term, OFP::ElseStmtAndBlock* ElseStmtAndBlock);

/* R833 */
ATbool ofp_traverse_IfThenStmt(ATerm term, OFP::IfThenStmt* IfThenStmt);

/* R834 */
ATbool ofp_traverse_ElseIfStmt(ATerm term, OFP::ElseIfStmt* ElseIfStmt);

/* R835 */
ATbool ofp_traverse_ElseStmt(ATerm term, OFP::ElseStmt* ElseStmt);

/* R836 */
ATbool ofp_traverse_EndIfStmt(ATerm term, OFP::EndIfStmt* EndIfStmt);

/* R837 */
ATbool ofp_traverse_IfStmt(ATerm term, OFP::IfStmt* IfStmt);

/* R838 */
ATbool ofp_traverse_CaseConstruct(ATerm term, OFP::CaseConstruct* CaseConstruct);
ATbool ofp_traverse_CaseStmtAndBlock(ATerm term, OFP::CaseStmtAndBlock* CaseStmtAndBlock);

/* R839 */
ATbool ofp_traverse_SelectCaseStmt(ATerm term, OFP::SelectCaseStmt* SelectCaseStmt);

/* R840 */
ATbool ofp_traverse_CaseStmt(ATerm term, OFP::CaseStmt* CaseStmt);

/* R841 */
ATbool ofp_traverse_EndSelectStmt(ATerm term, OFP::EndSelectStmt* EndSelectStmt);

/* R842 */
ATbool ofp_traverse_CaseExpr(ATerm term, OFP::CaseExpr* CaseExpr);

/* R843 */
ATbool ofp_traverse_CaseSelector(ATerm term, OFP::CaseSelector* CaseSelector);

/* R844 */
ATbool ofp_traverse_CaseValueRange(ATerm term, OFP::CaseValueRange* CaseValueRange);
ATbool ofp_traverse_CaseValueRangeList(ATerm term, OFP::CaseValueRangeList* CaseValueRangeList);

/* R845 */
ATbool ofp_traverse_CaseValue(ATerm term, OFP::CaseValue* CaseValue);

/* R846 */
ATbool ofp_traverse_SelectTypeConstruct(ATerm term, OFP::SelectTypeConstruct* SelectTypeConstruct);
ATbool ofp_traverse_TypeGuardStmtAndBlock(ATerm term, OFP::TypeGuardStmtAndBlock* TypeGuardStmtAndBlock);

/* R847 */
ATbool ofp_traverse_SelectTypeStmt(ATerm term, OFP::SelectTypeStmt* SelectTypeStmt);

/* R848 */
ATbool ofp_traverse_TypeGuardStmt(ATerm term, OFP::TypeGuardStmt* TypeGuardStmt);

/* R849 */
ATbool ofp_traverse_EndSelectTypeStmt(ATerm term, OFP::EndSelectTypeStmt* EndSelectTypeStmt);

/* R850 */
ATbool ofp_traverse_ExitStmt(ATerm term, OFP::ExitStmt* ExitStmt);

/* R851 */
ATbool ofp_traverse_GotoStmt(ATerm term, OFP::GotoStmt* GotoStmt);

/* R852 */
ATbool ofp_traverse_ComputedGotoStmt(ATerm term, OFP::ComputedGotoStmt* ComputedGotoStmt);

/* R853 */
ATbool ofp_traverse_ArithmeticIfStmt(ATerm term, OFP::ArithmeticIfStmt* ArithmeticIfStmt);

/* R854 */
ATbool ofp_traverse_ContinueStmt(ATerm term, OFP::ContinueStmt* ContinueStmt);

/* R855 */
ATbool ofp_traverse_StopStmt(ATerm term, OFP::StopStmt* StopStmt);

/* R856 */
ATbool ofp_traverse_ErrorStopStmt(ATerm term, OFP::ErrorStopStmt* ErrorStopStmt);

/* R857 */
ATbool ofp_traverse_StopCode(ATerm term, OFP::StopCode* StopCode);

/* R858 */
ATbool ofp_traverse_SyncAllStmt(ATerm term, OFP::SyncAllStmt* SyncAllStmt);

/* R859 */
ATbool ofp_traverse_SyncStat(ATerm term, OFP::SyncStat* SyncStat);
ATbool ofp_traverse_SyncStatList(ATerm term, OFP::SyncStatList* SyncStatList);

/* R860 */
ATbool ofp_traverse_SyncImagesStmt(ATerm term, OFP::SyncImagesStmt* SyncImagesStmt);

/* R861 */
ATbool ofp_traverse_ImageSet(ATerm term, OFP::ImageSet* ImageSet);

/* R862 */
ATbool ofp_traverse_SyncMemoryStmt(ATerm term, OFP::SyncMemoryStmt* SyncMemoryStmt);

/* R863 */
ATbool ofp_traverse_LockStmt(ATerm term, OFP::LockStmt* LockStmt);

/* R864 */
ATbool ofp_traverse_LockStat(ATerm term, OFP::LockStat* LockStat);
ATbool ofp_traverse_LockStatList(ATerm term, OFP::LockStatList* LockStatList);

/* R865 */
ATbool ofp_traverse_UnlockStmt(ATerm term, OFP::UnlockStmt* UnlockStmt);

/* R866 */
ATbool ofp_traverse_LockVariable(ATerm term, OFP::LockVariable* LockVariable);

/* Obsolete feature */
ATbool ofp_traverse_PauseStmt(ATerm term, OFP::PauseStmt* PauseStmt);

/* R901 */
ATbool ofp_traverse_IoUnit(ATerm term, OFP::IoUnit* IoUnit);

/* R902 */
ATbool ofp_traverse_FileUnitNumber(ATerm term, OFP::FileUnitNumber* FileUnitNumber);

/* R904 */
ATbool ofp_traverse_OpenStmt(ATerm term, OFP::OpenStmt* OpenStmt);

/* R905 */
ATbool ofp_traverse_ConnectSpec(ATerm term, OFP::ConnectSpec* ConnectSpec);
ATbool ofp_traverse_ConnectSpecList(ATerm term, OFP::ConnectSpecList* ConnectSpecList);

/* R906 */
ATbool ofp_traverse_FileNameExpr(ATerm term, OFP::FileNameExpr* FileNameExpr);

/* R907 */
ATbool ofp_traverse_IomsgVariable(ATerm term, OFP::IomsgVariable* IomsgVariable);

/* R908 */
ATbool ofp_traverse_CloseStmt(ATerm term, OFP::CloseStmt* CloseStmt);

/* R909 */
ATbool ofp_traverse_CloseSpec(ATerm term, OFP::CloseSpec* CloseSpec);
ATbool ofp_traverse_CloseSpecList(ATerm term, OFP::CloseSpecList* CloseSpecList);

/* R910 */
ATbool ofp_traverse_ReadStmt(ATerm term, OFP::ReadStmt* ReadStmt);

/* R911 */
ATbool ofp_traverse_WriteStmt(ATerm term, OFP::WriteStmt* WriteStmt);

/* R912 */
ATbool ofp_traverse_PrintStmt(ATerm term, OFP::PrintStmt* PrintStmt);

/* R913 */
ATbool ofp_traverse_IoControlSpec(ATerm term, OFP::IoControlSpec* IoControlSpec);
ATbool ofp_traverse_IoControlSpecList(ATerm term, OFP::IoControlSpecList* IoControlSpecList);

/* R914 */
ATbool ofp_traverse_IdVariable(ATerm term, OFP::IdVariable* IdVariable);

/* R915 */
ATbool ofp_traverse_Format(ATerm term, OFP::Format* Format);

/* R916 */
ATbool ofp_traverse_InputItem(ATerm term, OFP::InputItem* InputItem);
ATbool ofp_traverse_InputItemList(ATerm term, OFP::InputItemList* InputItemList);

/* R917 */
ATbool ofp_traverse_OutputItem(ATerm term, OFP::OutputItem* OutputItem);
ATbool ofp_traverse_OutputItemList(ATerm term, OFP::OutputItemList* OutputItemList);

/* R918 */
ATbool ofp_traverse_IoImpliedDo(ATerm term, OFP::IoImpliedDo* IoImpliedDo);

/* R919 */
ATbool ofp_traverse_IoImpliedDoObject(ATerm term, OFP::IoImpliedDoObject* IoImpliedDoObject);
ATbool ofp_traverse_IoImpliedDoObjectList(ATerm term, OFP::IoImpliedDoObjectList* IoImpliedDoObjectList);

/* R921 */
ATbool ofp_traverse_IoImpliedDoControl(ATerm term, OFP::IoImpliedDoControl* IoImpliedDoControl);

/* R922 */
ATbool ofp_traverse_WaitStmt(ATerm term, OFP::WaitStmt* WaitStmt);

/* R923 */
ATbool ofp_traverse_WaitSpec(ATerm term, OFP::WaitSpec* WaitSpec);
ATbool ofp_traverse_WaitSpecList(ATerm term, OFP::WaitSpecList* WaitSpecList);

/* R924 */
ATbool ofp_traverse_BackspaceStmt(ATerm term, OFP::BackspaceStmt* BackspaceStmt);

/* R925 */
ATbool ofp_traverse_EndfileStmt(ATerm term, OFP::EndfileStmt* EndfileStmt);

/* R926 */
ATbool ofp_traverse_RewindStmt(ATerm term, OFP::RewindStmt* RewindStmt);

/* R927 */
ATbool ofp_traverse_PositionSpec(ATerm term, OFP::PositionSpec* PositionSpec);
ATbool ofp_traverse_PositionSpecList(ATerm term, OFP::PositionSpecList* PositionSpecList);

/* R928 */
ATbool ofp_traverse_FlushStmt(ATerm term, OFP::FlushStmt* FlushStmt);

/* R929 */
ATbool ofp_traverse_FlushSpec(ATerm term, OFP::FlushSpec* FlushSpec);
ATbool ofp_traverse_FlushSpecList(ATerm term, OFP::FlushSpecList* FlushSpecList);

/* R930 */
ATbool ofp_traverse_InquireStmt(ATerm term, OFP::InquireStmt* InquireStmt);

/* R931 */
ATbool ofp_traverse_InquireSpec(ATerm term, OFP::InquireSpec* InquireSpec);
ATbool ofp_traverse_InquireSpecList(ATerm term, OFP::InquireSpecList* InquireSpecList);

ATbool ofp_traverse_FormatStmt(ATerm term, OFP::FormatStmt* FormatStmt);
ATbool ofp_traverse_FormatSpecification(ATerm term, OFP::FormatSpecification* FormatSpecification);
ATbool ofp_traverse_FormatItems(ATerm term, OFP::FormatItems* FormatItems);
ATbool ofp_traverse_FormatItem(ATerm term, OFP::FormatItem* FormatItem);
ATbool ofp_traverse_UnlimitedFormatItem(ATerm term, OFP::UnlimitedFormatItem* UnlimitedFormatItem);
ATbool ofp_traverse_REditDesc(ATerm term, OFP::REditDesc* REditDesc);
ATbool ofp_traverse_DataEditDesc(ATerm term, OFP::DataEditDesc* DataEditDesc);
ATbool ofp_traverse_WEditDesc(ATerm term, OFP::WEditDesc* WEditDesc);
ATbool ofp_traverse_MEditDesc(ATerm term, OFP::MEditDesc* MEditDesc);
ATbool ofp_traverse_DEditDesc(ATerm term, OFP::DEditDesc* DEditDesc);
ATbool ofp_traverse_EEditDesc(ATerm term, OFP::EEditDesc* EEditDesc);
ATbool ofp_traverse_VEditDesc(ATerm term, OFP::VEditDesc* VEditDesc);
ATbool ofp_traverse_VEditDescList(ATerm term, OFP::VEditDescList* VEditDescList);
ATbool ofp_traverse_ControlEditDesc(ATerm term, OFP::ControlEditDesc* ControlEditDesc);
ATbool ofp_traverse_KEditDesc(ATerm term, OFP::KEditDesc* KEditDesc);
ATbool ofp_traverse_PositionEditDesc(ATerm term, OFP::PositionEditDesc* PositionEditDesc);
ATbool ofp_traverse_NEditDesc(ATerm term, OFP::NEditDesc* NEditDesc);
ATbool ofp_traverse_SignEditDesc(ATerm term, OFP::SignEditDesc* SignEditDesc);
ATbool ofp_traverse_BlankInterpEditDesc(ATerm term, OFP::BlankInterpEditDesc* BlankInterpEditDesc);
ATbool ofp_traverse_RoundEditDesc(ATerm term, OFP::RoundEditDesc* RoundEditDesc);
ATbool ofp_traverse_DecimalEditDesc(ATerm term, OFP::DecimalEditDesc* DecimalEditDesc);
ATbool ofp_traverse_CharStringEditDesc(ATerm term, OFP::CharStringEditDesc* CharStringEditDesc);

/* R1101 */
ATbool ofp_traverse_MainProgram(ATerm term, OFP::MainProgram* MainProgram);

/* R1102 */
ATbool ofp_traverse_ProgramStmt(ATerm term, OFP::ProgramStmt* ProgramStmt);

/* R1103 */
ATbool ofp_traverse_EndProgramStmt(ATerm term, OFP::EndProgramStmt* EndProgramStmt);

/* R1104 */
ATbool ofp_traverse_Module(ATerm term, OFP::Module* Module);

/* R1105 */
ATbool ofp_traverse_ModuleStmt(ATerm term, OFP::ModuleStmt* ModuleStmt);

/* R1106 */
ATbool ofp_traverse_EndModuleStmt(ATerm term, OFP::EndModuleStmt* EndModuleStmt);

/* R1107 */
ATbool ofp_traverse_ModuleSubprogramPart(ATerm term, OFP::ModuleSubprogramPart* ModuleSubprogramPart);

/* R1108 */
ATbool ofp_traverse_ModuleSubprogram(ATerm term, OFP::ModuleSubprogram* ModuleSubprogram);

/* R1109 */
ATbool ofp_traverse_UseStmt(ATerm term, OFP::UseStmt* UseStmt);

/* R1110 */
ATbool ofp_traverse_ModuleNature(ATerm term, OFP::ModuleNature* ModuleNature);

/* R1111 */
ATbool ofp_traverse_Rename(ATerm term, OFP::Rename* Rename);
ATbool ofp_traverse_RenameList(ATerm term, OFP::RenameList* RenameList);

/* R1112 */
ATbool ofp_traverse_Only(ATerm term, OFP::Only* Only);
ATbool ofp_traverse_OnlyList(ATerm term, OFP::OnlyList* OnlyList);

/* R1113 */
ATbool ofp_traverse_OnlyUseName(ATerm term, OFP::OnlyUseName* OnlyUseName);

/* R1114 */
ATbool ofp_traverse_LocalDefinedOperator(ATerm term, OFP::LocalDefinedOperator* LocalDefinedOperator);

/* R1115 */
ATbool ofp_traverse_UseDefinedOperator(ATerm term, OFP::UseDefinedOperator* UseDefinedOperator);

/* R1116 */
ATbool ofp_traverse_Submodule(ATerm term, OFP::Submodule* Submodule);

/* R1117 */
ATbool ofp_traverse_SubmoduleStmt(ATerm term, OFP::SubmoduleStmt* SubmoduleStmt);

/* R1118 */
ATbool ofp_traverse_ParentIdentifier(ATerm term, OFP::ParentIdentifier* ParentIdentifier);

/* R1119 */
ATbool ofp_traverse_EndSubmoduleStmt(ATerm term, OFP::EndSubmoduleStmt* EndSubmoduleStmt);

/* R1120 */
ATbool ofp_traverse_BlockData(ATerm term, OFP::BlockData* BlockData);

/* R1121 */
ATbool ofp_traverse_BlockDataStmt(ATerm term, OFP::BlockDataStmt* BlockDataStmt);

/* R1122 */
ATbool ofp_traverse_EndBlockDataStmt(ATerm term, OFP::EndBlockDataStmt* EndBlockDataStmt);

/* R1201 */
ATbool ofp_traverse_InterfaceBlock(ATerm term, OFP::InterfaceBlock* InterfaceBlock);

/* R1202 */
ATbool ofp_traverse_InterfaceSpecification(ATerm term, OFP::InterfaceSpecification* InterfaceSpecification);

/* R1203 */
ATbool ofp_traverse_InterfaceStmt(ATerm term, OFP::InterfaceStmt* InterfaceStmt);

/* R1204 */
ATbool ofp_traverse_EndInterfaceStmt(ATerm term, OFP::EndInterfaceStmt* EndInterfaceStmt);

/* R1205 */
ATbool ofp_traverse_InterfaceBody(ATerm term, OFP::InterfaceBody* InterfaceBody);

/* R1206 */
ATbool ofp_traverse_ProcedureStmt(ATerm term, OFP::ProcedureStmt* ProcedureStmt);
ATbool ofp_traverse_ProcedureNameList(ATerm term, OFP::ProcedureNameList* ProcedureNameList);

/* R1207 */
ATbool ofp_traverse_GenericSpec(ATerm term, OFP::GenericSpec* GenericSpec);

/* R1208 */
ATbool ofp_traverse_DefinedIoGenericSpec(ATerm term, OFP::DefinedIoGenericSpec* DefinedIoGenericSpec);

/* R1209 */
ATbool ofp_traverse_ImportStmt(ATerm term, OFP::ImportStmt* ImportStmt);
ATbool ofp_traverse_ImportNameList(ATerm term, OFP::ImportNameList* ImportNameList);

/* R1210 */
ATbool ofp_traverse_ExternalStmt(ATerm term, OFP::ExternalStmt* ExternalStmt);

/* R1211 */
ATbool ofp_traverse_ProcedureDeclarationStmt(ATerm term, OFP::ProcedureDeclarationStmt* ProcedureDeclarationStmt);

/* R1212 */
ATbool ofp_traverse_ProcInterface(ATerm term, OFP::ProcInterface* ProcInterface);

/* R1213 */
ATbool ofp_traverse_ProcAttrSpec(ATerm term, OFP::ProcAttrSpec* ProcAttrSpec);
ATbool ofp_traverse_ProcAttrSpecList(ATerm term, OFP::ProcAttrSpecList* ProcAttrSpecList);

/* R1214 */
ATbool ofp_traverse_ProcDecl(ATerm term, OFP::ProcDecl* ProcDecl);
ATbool ofp_traverse_ProcDeclList(ATerm term, OFP::ProcDeclList* ProcDeclList);

/* R1215 */
ATbool ofp_traverse_InterfaceName(ATerm term, OFP::InterfaceName* InterfaceName);

/* R1216 */
ATbool ofp_traverse_ProcPointerInit(ATerm term, OFP::ProcPointerInit* ProcPointerInit);

/* R1217 */
ATbool ofp_traverse_InitialProcTarget(ATerm term, OFP::InitialProcTarget* InitialProcTarget);

/* R1218 */
ATbool ofp_traverse_IntrinsicStmt(ATerm term, OFP::IntrinsicStmt* IntrinsicStmt);
ATbool ofp_traverse_IntrinsicProcedureNameList(ATerm term, OFP::IntrinsicProcedureNameList* IntrinsicProcedureNameList);

/* R1219 */
ATbool ofp_traverse_FunctionReference(ATerm term, OFP::FunctionReference* FunctionReference);

/* R1220 */
ATbool ofp_traverse_CallStmt(ATerm term, OFP::CallStmt* CallStmt);

/* R1221 */
ATbool ofp_traverse_ProcedureDesignator(ATerm term, OFP::ProcedureDesignator* ProcedureDesignator);

/* R1222 */
ATbool ofp_traverse_ActualArgSpec(ATerm term, OFP::ActualArgSpec* ActualArgSpec);
ATbool ofp_traverse_ActualArgSpecList(ATerm term, OFP::ActualArgSpecList* ActualArgSpecList);

/* R1223 */
ATbool ofp_traverse_ActualArg(ATerm term, OFP::ActualArg* ActualArg);

/* R1224 */
ATbool ofp_traverse_AltReturnSpec(ATerm term, OFP::AltReturnSpec* AltReturnSpec);

/* R1225 */
ATbool ofp_traverse_Prefix(ATerm term, OFP::Prefix* Prefix);

/* R1226 */
ATbool ofp_traverse_PrefixSpec(ATerm term, OFP::PrefixSpec* PrefixSpec);

/* R1227 */
ATbool ofp_traverse_FunctionSubprogram(ATerm term, OFP::FunctionSubprogram* FunctionSubprogram);

/* R1228 */
ATbool ofp_traverse_FunctionStmt(ATerm term, OFP::FunctionStmt* FunctionStmt);

/* R1229 */
ATbool ofp_traverse_ProcLanguageBindingSpec(ATerm term, OFP::ProcLanguageBindingSpec* ProcLanguageBindingSpec);

/* R1230 */
ATbool ofp_traverse_DummyArgName(ATerm term, OFP::DummyArgName* DummyArgName);

/* R1231 */
ATbool ofp_traverse_Suffix(ATerm term, OFP::Suffix* Suffix);

/* R1232 */
ATbool ofp_traverse_EndFunctionStmt(ATerm term, OFP::EndFunctionStmt* EndFunctionStmt);

/* R1233 */
ATbool ofp_traverse_SubroutineSubprogram(ATerm term, OFP::SubroutineSubprogram* SubroutineSubprogram);

/* R1234 */
ATbool ofp_traverse_SubroutineStmt(ATerm term, OFP::SubroutineStmt* SubroutineStmt);

/* R1235 */
ATbool ofp_traverse_DummyArg(ATerm term, OFP::DummyArg* DummyArg);
ATbool ofp_traverse_DummyArgList(ATerm term, OFP::DummyArgList* DummyArgList);

/* R1236 */
ATbool ofp_traverse_EndSubroutineStmt(ATerm term, OFP::EndSubroutineStmt* EndSubroutineStmt);

/* R1237 */
ATbool ofp_traverse_SeparateModuleSubprogram(ATerm term, OFP::SeparateModuleSubprogram* SeparateModuleSubprogram);

/* R1238 */
ATbool ofp_traverse_MpSubprogramStmt(ATerm term, OFP::MpSubprogramStmt* MpSubprogramStmt);

/* R1239 */
ATbool ofp_traverse_EndMpSubprogramStmt(ATerm term, OFP::EndMpSubprogramStmt* EndMpSubprogramStmt);

/* R1240 */
ATbool ofp_traverse_EntryStmt(ATerm term, OFP::EntryStmt* EntryStmt);

/* R1241 */
ATbool ofp_traverse_ReturnStmt(ATerm term, OFP::ReturnStmt* ReturnStmt);

/* R1242 */
ATbool ofp_traverse_ContainsStmt(ATerm term, OFP::ContainsStmt* ContainsStmt);

/* R1243 */
ATbool ofp_traverse_StmtFunctionStmt(ATerm term, OFP::StmtFunctionStmt* StmtFunctionStmt);


/** Identifier aliases
 */

ATbool ofp_traverse_Name(ATerm term, OFP::Name* Name);

#ifdef OBSOLETE
ATbool ofp_traverse_AncestorModuleName(ATerm term, OFP::AncestorModuleName* AncestorModuleName);
ATbool ofp_traverse_ArgName(ATerm term, OFP::ArgName* ArgName);
ATbool ofp_traverse_ArrayName(ATerm term, OFP::ArrayName* ArrayName);
ATbool ofp_traverse_AssociateConstructName(ATerm term, OFP::AssociateConstructName* AssociateConstructName);
ATbool ofp_traverse_AssociateName(ATerm term, OFP::AssociateName* AssociateName);
ATbool ofp_traverse_BindingName(ATerm term, OFP::BindingName* BindingName);
ATbool ofp_traverse_BlockConstructName(ATerm term, OFP::BlockConstructName* BlockConstructName);
ATbool ofp_traverse_BlockDataName(ATerm term, OFP::BlockDataName* BlockDataName);
ATbool ofp_traverse_CaseConstructName(ATerm term, OFP::CaseConstructName* CaseConstructName);
ATbool ofp_traverse_CoarrayName(ATerm term, OFP::CoarrayName* CoarrayName);
ATbool ofp_traverse_CommonBlockName(ATerm term, OFP::CommonBlockName* CommonBlockName);
ATbool ofp_traverse_ComponentName(ATerm term, OFP::ComponentName* ComponentName);
ATbool ofp_traverse_ConstructName(ATerm term, OFP::ConstructName* ConstructName);
ATbool ofp_traverse_CriticalConstructName(ATerm term, OFP::CriticalConstructName* CriticalConstructName);
ATbool ofp_traverse_DataPointerComponentName(ATerm term, OFP::DataPointerComponentName* DataPointerComponentName);
ATbool ofp_traverse_DoConstructName(ATerm term, OFP::DoConstructName* DoConstructName);
ATbool ofp_traverse_IfConstructName(ATerm term, OFP::IfConstructName* IfConstructName);
ATbool ofp_traverse_EntityName(ATerm term, OFP::EntityName* EntityName);
ATbool ofp_traverse_EntryName(ATerm term, OFP::EntryName* EntryName);
ATbool ofp_traverse_ExternalName(ATerm term, OFP::ExternalName* ExternalName);
ATbool ofp_traverse_FinalSubroutineName(ATerm term, OFP::FinalSubroutineName* FinalSubroutineName);
ATbool ofp_traverse_ForallConstructName(ATerm term, OFP::ForallConstructName* ForallConstructName);
ATbool ofp_traverse_FunctionName(ATerm term, OFP::FunctionName* FunctionName);
ATbool ofp_traverse_GenericName(ATerm term, OFP::GenericName* GenericName);
ATbool ofp_traverse_ImportName(ATerm term, OFP::ImportName* ImportName);
ATbool ofp_traverse_IndexName(ATerm term, OFP::IndexName* IndexName);
ATbool ofp_traverse_InterfaceName(ATerm term, OFP::InterfaceName* InterfaceName);
ATbool ofp_traverse_IntrinsicProcedureName(ATerm term, OFP::IntrinsicProcedureName* IntrinsicProcedureName);
ATbool ofp_traverse_LocalName(ATerm term, OFP::LocalName* LocalName);
ATbool ofp_traverse_ModuleName(ATerm term, OFP::ModuleName* ModuleName);
ATbool ofp_traverse_NamelistGroupName(ATerm term, OFP::NamelistGroupName* NamelistGroupName);
ATbool ofp_traverse_ObjectName(ATerm term, OFP::Name* ObjectName);
ATbool ofp_traverse_ObjectName(ATerm term, OFP::ObjectName* ObjectName);
ATbool ofp_traverse_ParentSubmoduleName(ATerm term, OFP::ParentSubmoduleName* ParentSubmoduleName);
ATbool ofp_traverse_ParentTypeName(ATerm term, OFP::ParentTypeName* ParentTypeName);
ATbool ofp_traverse_ProcedureComponentName(ATerm term, OFP::ProcedureComponentName* ProcedureComponentName);
ATbool ofp_traverse_ProcedureEntityName(ATerm term, OFP::ProcedureEntityName* ProcedureEntityName);
ATbool ofp_traverse_ProcedureName(ATerm term, OFP::ProcedureName* ProcedureName);
ATbool ofp_traverse_ProcEntityName(ATerm term, OFP::ProcEntityName* ProcEntityName);
ATbool ofp_traverse_Name(ATerm term, OFP::Name* Name);
ATbool ofp_traverse_ResultName(ATerm term, OFP::ResultName* ResultName);
ATbool ofp_traverse_ScalarIntConstantName(ATerm term, OFP::ScalarIntConstantName* ScalarIntConstantName);
ATbool ofp_traverse_ScalarIntVariableName(ATerm term, OFP::ScalarIntVariableName* ScalarIntVariableName);
ATbool ofp_traverse_ScalarVariableName(ATerm term, OFP::ScalarVariableName* ScalarVariableName);
ATbool ofp_traverse_SelectConstructName(ATerm term, OFP::SelectConstructName* SelectConstructName);
ATbool ofp_traverse_SubmoduleName(ATerm term, OFP::SubmoduleName* SubmoduleName);
ATbool ofp_traverse_SubroutineName(ATerm term, OFP::SubroutineName* SubroutineName);
ATbool ofp_traverse_TypeName(ATerm term, OFP::TypeName* TypeName);
ATbool ofp_traverse_TypeParamName(ATerm term, OFP::TypeParamName* TypeParamName);
ATbool ofp_traverse_UseName(ATerm term, OFP::UseName* UseName);
#endif

/** Lists
 */
ATbool ofp_traverse_ExternalNameList(ATerm term, OFP::ExternalNameList* ExternalNameList);
ATbool ofp_traverse_DummyArgNameList(ATerm term, OFP::DummyArgNameList* DummyArgNameList);
ATbool ofp_traverse_LabelList(ATerm term, OFP::LabelList* LabelList);

/** Terminals
 */
ATbool ofp_traverse_Dop(ATerm term, OFP::Dop* Dop);
ATbool ofp_traverse_HexConstant(ATerm term, OFP::HexConstant* HexConstant);
ATbool ofp_traverse_OctalConstant(ATerm term, OFP::OctalConstant* OctalConstant);
ATbool ofp_traverse_BinaryConstant(ATerm term, OFP::BinaryConstant* BinaryConstant);
ATbool ofp_traverse_Rcon(ATerm term, OFP::Rcon* Rcon);
ATbool ofp_traverse_Scon(ATerm term, OFP::Scon* Scon);
ATbool ofp_traverse_Icon(ATerm term, OFP::Icon* Icon);
ATbool ofp_traverse_Ident(ATerm term, OFP::Ident* Ident);
ATbool ofp_traverse_Letter(ATerm term, OFP::Letter* Letter);
ATbool ofp_traverse_Label(ATerm term, OFP::Label* Label);
ATbool ofp_traverse_LblRef(ATerm term, OFP::LblRef* LblRef);
ATbool ofp_traverse_StartCommentBlock(ATerm term, OFP::StartCommentBlock* StartCommentBlock);
ATbool ofp_traverse_EOS(ATerm term, OFP::EOS* EOS);

#endif /* OFP_TRAVERSAL_H */
