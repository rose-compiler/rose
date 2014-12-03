#define COMPILED_WITH_ROSE 1

#if COMPILED_WITH_ROSE
#include "sage3basic.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"
#endif

#include "OFPNodes.hpp"
#include "OFPExpr.hpp"
#include "traverse.hpp"

OFP::Program::~Program()
   {
      if (pStartCommentBlock) delete pStartCommentBlock;
      if (pProgramUnitList) delete pProgramUnitList;
   }

OFP::ProgramUnit::~ProgramUnit()
   {
      if (pBlockData) delete pBlockData;
      if (pSubmodule) delete pSubmodule;
      if (pModule) delete pModule;
      if (pExternalSubprogram) delete pExternalSubprogram;
      if (pMainProgram) delete pMainProgram;
   }

OFP::ExternalSubprogram::~ExternalSubprogram()
   {
      if (pSubroutineSubprogram) delete pSubroutineSubprogram;
      if (pFunctionSubprogram) delete pFunctionSubprogram;
   }

OFP::InitialSpecPart::~InitialSpecPart()
   {
      if (pStatementList) delete pStatementList;
   }

OFP::SpecificationPart::~SpecificationPart()
   {
      if (pUseStmtList) delete pUseStmtList;
      if (pImportStmtList) delete pImportStmtList;
      if (pImplicitPart) delete pImplicitPart;
      if (pDeclarationConstructList) delete pDeclarationConstructList;
   }

OFP::ImplicitPart::~ImplicitPart()
   {
      if (pImplicitPartStmtList) delete pImplicitPartStmtList;
      if (pImplicitStmt) delete pImplicitStmt;
   }

OFP::ImplicitPartStmt::~ImplicitPartStmt()
   {
      if (pEntryStmt) delete pEntryStmt;
      if (pFormatStmt) delete pFormatStmt;
      if (pParameterStmt) delete pParameterStmt;
      if (pImplicitStmt) delete pImplicitStmt;
   }

OFP::DeclarationConstruct::~DeclarationConstruct()
   {
      if (pStatement) delete pStatement;

      if (pStmtFunctionStmt) delete pStmtFunctionStmt;
      if (pOtherSpecificationStmt) delete pOtherSpecificationStmt;
      if (pProcedureDeclarationStmt) delete pProcedureDeclarationStmt;
      if (pParameterStmt) delete pParameterStmt;
      if (pInterfaceBlock) delete pInterfaceBlock;
      if (pFormatStmt) delete pFormatStmt;
      if (pEnumDef) delete pEnumDef;
      if (pEntryStmt) delete pEntryStmt;
      if (pDerivedTypeDef) delete pDerivedTypeDef;
   }

OFP::SpecAndExecPart::~SpecAndExecPart()
   {
      if (pStatementList) delete pStatementList;
   }

OFP::ExecutionPart::~ExecutionPart()
   {
      if (pExecutionPartConstructList) delete pExecutionPartConstructList;
   }

OFP::ExecutionPartConstruct::~ExecutionPartConstruct()
   {
      if (pDataStmt) delete pDataStmt;
      if (pEntryStmt) delete pEntryStmt;
      if (pFormatStmt) delete pFormatStmt;
      if (pExecutableConstruct) delete pExecutableConstruct;
   }

OFP::InternalSubprogramPart::~InternalSubprogramPart()
   {
      if (pContainsStmt) delete pContainsStmt;
      if (pInternalSubprogramList) delete pInternalSubprogramList;
   }

OFP::InternalSubprogram::~InternalSubprogram()
   {
      if (pSubroutineSubprogram) delete pSubroutineSubprogram;
      if (pFunctionSubprogram) delete pFunctionSubprogram;
   }

OFP::OtherSpecificationStmt::~OtherSpecificationStmt()
   {
      if (pValueStmt) delete pValueStmt;
      if (pVolatileStmt) delete pVolatileStmt;
      if (pTargetStmt) delete pTargetStmt;
      if (pSaveStmt) delete pSaveStmt;
      if (pProtectedStmt) delete pProtectedStmt;
      if (pPointerStmt) delete pPointerStmt;
      if (pOptionalStmt) delete pOptionalStmt;
      if (pNamelistStmt) delete pNamelistStmt;
      if (pIntrinsicStmt) delete pIntrinsicStmt;
      if (pIntentStmt) delete pIntentStmt;
      if (pExternalStmt) delete pExternalStmt;
      if (pEquivalenceStmt) delete pEquivalenceStmt;
      if (pDimensionStmt) delete pDimensionStmt;
      if (pDataStmt) delete pDataStmt;
      if (pCommonStmt) delete pCommonStmt;
      if (pContiguousStmt) delete pContiguousStmt;
      if (pCodimensionStmt) delete pCodimensionStmt;
      if (pBindStmt) delete pBindStmt;
      if (pAsynchronousStmt) delete pAsynchronousStmt;
      if (pAllocatableStmt) delete pAllocatableStmt;
      if (pAccessStmt) delete pAccessStmt;
   }

OFP::ExecutableConstruct::~ExecutableConstruct()
   {
   }

OFP::ActionStmt::~ActionStmt()
   {
   }

OFP::Keyword::~Keyword()
   {
      if (pName) delete pName;
   }

OFP::Constant::~Constant()
   {
      if (pLiteralConstant) delete pLiteralConstant;
   }

OFP::LiteralConstant::~LiteralConstant()
   {
      if (pBozLiteralConstant) delete pBozLiteralConstant;
      if (pCharLiteralConstant) delete pCharLiteralConstant;
      if (pLogicalLiteralConstant) delete pLogicalLiteralConstant;
      if (pComplexLiteralConstant) delete pComplexLiteralConstant;
      if (pRealLiteralConstant) delete pRealLiteralConstant;
      if (pIntLiteralConstant) delete pIntLiteralConstant;
   }

OFP::NamedConstant::~NamedConstant()
   {
      if (pName) delete pName;
   }

OFP::TypeParamValue::~TypeParamValue()
   {
      if (pExpr) delete pExpr;
   }

OFP::TypeSpec::~TypeSpec()
   {
      if (pDerivedTypeSpec) delete pDerivedTypeSpec;
      if (pIntrinsicTypeSpec) delete pIntrinsicTypeSpec;
   }

OFP::DeclarationTypeSpec::~DeclarationTypeSpec()
   {
      if (pDerivedTypeSpec) delete pDerivedTypeSpec;
      if (pIntrinsicTypeSpec) delete pIntrinsicTypeSpec;
   }

OFP::IntrinsicTypeSpec::~IntrinsicTypeSpec()
   {
      if (pKindSelector) delete pKindSelector;
      if (pCharSelector) delete pCharSelector;
   }

OFP::KindSelector::~KindSelector()
   {
      if (pIcon) delete pIcon;
      if (pExpr) delete pExpr;
   }

OFP::SignedIntLiteralConstant::~SignedIntLiteralConstant()
   {
      if (pSign) delete pSign;
      if (pIntLiteralConstant) delete pIntLiteralConstant;
   }

OFP::IntLiteralConstant::~IntLiteralConstant()
   {
      if (pDigitString) delete pDigitString;
      if (pKindParam) delete pKindParam;
   }

OFP::KindParam::~KindParam()
   {
      if (pScalarIntConstantName) delete pScalarIntConstantName;
      if (pDigitString) delete pDigitString;
   }

OFP::DigitString::~DigitString()
   {
      if (pIcon) delete pIcon;
   }

OFP::Sign::~Sign()
   {
   }

OFP::SignedRealLiteralConstant::~SignedRealLiteralConstant()
   {
      if (pSign) delete pSign;
      if (pRealLiteralConstant) delete pRealLiteralConstant;
   }

OFP::RealLiteralConstant::~RealLiteralConstant()
   {
      if (pRcon) delete pRcon;
      if (pKindParam) delete pKindParam;
   }

OFP::ComplexLiteralConstant::~ComplexLiteralConstant()
   {
      if (pRealPart) delete pRealPart;
      if (pImagPart) delete pImagPart;
   }

OFP::RealPart::~RealPart()
   {
      if (pNamedConstant) delete pNamedConstant;
      if (pSignedRealLiteralConstant) delete pSignedRealLiteralConstant;
      if (pSignedIntLiteralConstant) delete pSignedIntLiteralConstant;
   }

OFP::ImagPart::~ImagPart()
   {
      if (pNamedConstant) delete pNamedConstant;
      if (pSignedRealLiteralConstant) delete pSignedRealLiteralConstant;
      if (pSignedIntLiteralConstant) delete pSignedIntLiteralConstant;
   }

OFP::CharSelector::~CharSelector()
   {
      if (pExpr) delete pExpr;
      if (pTypeParamValue) delete pTypeParamValue;
      if (pLengthSelector) delete pLengthSelector;
   }

OFP::LengthSelector::~LengthSelector()
   {
      if (pCharLength) delete pCharLength;
      if (pTypeParamValue) delete pTypeParamValue;
   }

OFP::CharLength::~CharLength()
   {
      if (pIcon) delete pIcon;
      if (pTypeParamValue) delete pTypeParamValue;
   }

OFP::CharLiteralConstant::~CharLiteralConstant()
   {
      if (pKindParam) delete pKindParam;
      if (pScon) delete pScon;
   }

OFP::LogicalLiteralConstant::~LogicalLiteralConstant()
   {
      if (pKindParam) delete pKindParam;
   }

OFP::DerivedTypeDef::~DerivedTypeDef()
   {
      if (pDerivedTypeStmt) delete pDerivedTypeStmt;
      if (pTypeParamDefStmtList) delete pTypeParamDefStmtList;
      if (pPrivateOrSequenceList) delete pPrivateOrSequenceList;
      if (pComponentPart) delete pComponentPart;
      if (pTypeBoundProcedurePart) delete pTypeBoundProcedurePart;
      if (pEndTypeStmt) delete pEndTypeStmt;
   }

OFP::DerivedTypeStmt::~DerivedTypeStmt()
   {
      if (pLabel) delete pLabel;
      if (pOptTypeAttrSpecList) delete pOptTypeAttrSpecList;
      if (pTypeName) delete pTypeName;
      if (pOptTypeParamNameList) delete pOptTypeParamNameList;
      if (pEOS) delete pEOS;
   }

OFP::OptTypeAttrSpecList::~OptTypeAttrSpecList()
   {
      if (pTypeAttrSpecList) delete pTypeAttrSpecList;
   }

OFP::OptTypeParamNameList::~OptTypeParamNameList()
   {
      if (pTypeParamNameList) delete pTypeParamNameList;
   }

OFP::TypeAttrSpec::~TypeAttrSpec()
   {
      if (pParentTypeName) delete pParentTypeName;
      if (pAccessSpec) delete pAccessSpec;
   }

OFP::TypeAttrSpecList::~TypeAttrSpecList()
   {
      if (pTypeAttrSpecList) delete pTypeAttrSpecList;
   }

OFP::TypeParamNameList::~TypeParamNameList()
   {
      if (pTypeParamNameList) delete pTypeParamNameList;
   }

OFP::PrivateOrSequence::~PrivateOrSequence()
   {
      if (pSequenceStmt) delete pSequenceStmt;
      if (pPrivateComponentsStmt) delete pPrivateComponentsStmt;
   }

OFP::EndTypeStmt::~EndTypeStmt()
   {
      if (pLabel) delete pLabel;
      if (pTypeName) delete pTypeName;
      if (pEOS) delete pEOS;
   }

OFP::SequenceStmt::~SequenceStmt()
   {
      if (pLabel) delete pLabel;
      if (pEOS) delete pEOS;
   }

OFP::TypeParamDefStmt::~TypeParamDefStmt()
   {
      if (pLabel) delete pLabel;
      if (pKindSelector) delete pKindSelector;
      if (pTypeParamAttrSpec) delete pTypeParamAttrSpec;
      if (pTypeParamDeclList) delete pTypeParamDeclList;
      if (pEOS) delete pEOS;
   }

OFP::TypeParamDecl::~TypeParamDecl()
   {
      if (pTypeParamName) delete pTypeParamName;
      if (pExpr) delete pExpr;
   }

OFP::TypeParamDeclList::~TypeParamDeclList()
   {
      if (pTypeParamDeclList) delete pTypeParamDeclList;
   }

OFP::TypeParamAttrSpec::~TypeParamAttrSpec()
   {
   }

OFP::ComponentPart::~ComponentPart()
   {
      if (pComponentDefStmtList) delete pComponentDefStmtList;
   }

OFP::ComponentDefStmt::~ComponentDefStmt()
   {
      if (pProcComponentDefStmt) delete pProcComponentDefStmt;
      if (pDataComponentDefStmt) delete pDataComponentDefStmt;
   }

OFP::DataComponentDefStmt::~DataComponentDefStmt()
   {
      if (pLabel) delete pLabel;
      if (pDeclarationTypeSpec) delete pDeclarationTypeSpec;
      if (pOptComponentAttrSpecList) delete pOptComponentAttrSpecList;
      if (pComponentDeclList) delete pComponentDeclList;
      if (pEOS) delete pEOS;
   }

OFP::OptComponentAttrSpecList::~OptComponentAttrSpecList()
   {
      if (pComponentAttrSpecList) delete pComponentAttrSpecList;
   }

OFP::ComponentAttrSpec::~ComponentAttrSpec()
   {
      if (pComponentArraySpec) delete pComponentArraySpec;
      if (pCoarraySpec) delete pCoarraySpec;
      if (pAccessSpec) delete pAccessSpec;
   }

OFP::ComponentAttrSpecList::~ComponentAttrSpecList()
   {
      if (pComponentAttrSpecList) delete pComponentAttrSpecList;
   }

OFP::ComponentDecl::~ComponentDecl()
   {
      if (pComponentName) delete pComponentName;
      if (pComponentArraySpec) delete pComponentArraySpec;
      if (pCoarraySpec) delete pCoarraySpec;
      if (pCharLength) delete pCharLength;
      if (pComponentInitialization) delete pComponentInitialization;
   }

OFP::ComponentDeclList::~ComponentDeclList()
   {
      if (pComponentDeclList) delete pComponentDeclList;
   }

OFP::ComponentArraySpec::~ComponentArraySpec()
   {
      if (pDeferredShapeSpecList) delete pDeferredShapeSpecList;
      if (pExplicitShapeSpecList) delete pExplicitShapeSpecList;
   }

OFP::ProcComponentDefStmt::~ProcComponentDefStmt()
   {
      if (pLabel) delete pLabel;
      if (pProcInterface) delete pProcInterface;
      if (pProcComponentAttrSpecList) delete pProcComponentAttrSpecList;
      if (pProcDeclList) delete pProcDeclList;
      if (pEOS) delete pEOS;
   }

OFP::ProcComponentAttrSpec::~ProcComponentAttrSpec()
   {
      if (pAccessSpec) delete pAccessSpec;
   }

OFP::ProcComponentAttrSpecList::~ProcComponentAttrSpecList()
   {
      if (pProcComponentAttrSpecList) delete pProcComponentAttrSpecList;
   }

OFP::ComponentInitialization::~ComponentInitialization()
   {
      if (pInitialDataTarget) delete pInitialDataTarget;
      if (pNullInit) delete pNullInit;
      if (pExpr) delete pExpr;
   }

OFP::InitialDataTarget::~InitialDataTarget()
   {
      if (pDesignator) delete pDesignator;
   }

OFP::PrivateComponentsStmt::~PrivateComponentsStmt()
   {
      if (pLabel) delete pLabel;
      if (pEOS) delete pEOS;
   }

OFP::TypeBoundProcedurePart::~TypeBoundProcedurePart()
   {
      if (pContainsStmt) delete pContainsStmt;
      if (pBindingPrivateStmt) delete pBindingPrivateStmt;
      if (pTypeBoundProcBindingList) delete pTypeBoundProcBindingList;
   }

OFP::BindingPrivateStmt::~BindingPrivateStmt()
   {
      if (pLabel) delete pLabel;
      if (pEOS) delete pEOS;
   }

OFP::TypeBoundProcBinding::~TypeBoundProcBinding()
   {
      if (pFinalProcedureStmt) delete pFinalProcedureStmt;
      if (pTypeBoundGenericStmt) delete pTypeBoundGenericStmt;
      if (pTypeBoundProcedureStmt) delete pTypeBoundProcedureStmt;
   }

OFP::TypeBoundProcedureStmt::~TypeBoundProcedureStmt()
   {
      if (pLabel) delete pLabel;
      if (pInterfaceName) delete pInterfaceName;
      if (pBindingAttrList) delete pBindingAttrList;
      if (pBindingNameList) delete pBindingNameList;
      if (pEOS) delete pEOS;
      if (pTypeBoundProcDeclList) delete pTypeBoundProcDeclList;
   }

OFP::BindingNameList::~BindingNameList()
   {
      if (pBindingNameList) delete pBindingNameList;
   }

OFP::BindingAttrList::~BindingAttrList()
   {
      if (pBindingAttrList) delete pBindingAttrList;
   }

OFP::TypeBoundProcDecl::~TypeBoundProcDecl()
   {
      if (pBindingName) delete pBindingName;
      if (pProcedureName) delete pProcedureName;
   }

OFP::TypeBoundProcDeclList::~TypeBoundProcDeclList()
   {
      if (pTypeBoundProcDeclList) delete pTypeBoundProcDeclList;
   }

OFP::TypeBoundGenericStmt::~TypeBoundGenericStmt()
   {
      if (pLabel) delete pLabel;
      if (pAccessSpec) delete pAccessSpec;
      if (pGenericSpec) delete pGenericSpec;
      if (pBindingNameList) delete pBindingNameList;
      if (pEOS) delete pEOS;
   }

OFP::BindingAttr::~BindingAttr()
   {
      if (pAccessSpec) delete pAccessSpec;
      if (pArgName) delete pArgName;
   }

OFP::FinalProcedureStmt::~FinalProcedureStmt()
   {
      if (pLabel) delete pLabel;
      if (pFinalSubroutineNameList) delete pFinalSubroutineNameList;
      if (pEOS) delete pEOS;
   }

OFP::FinalSubroutineNameList::~FinalSubroutineNameList()
   {
      if (pFinalSubroutineNameList) delete pFinalSubroutineNameList;
   }

OFP::DerivedTypeSpec::~DerivedTypeSpec()
   {
      if (pTypeName) delete pTypeName;
      if (pTypeParamSpecList) delete pTypeParamSpecList;
   }

OFP::TypeParamSpec::~TypeParamSpec()
   {
      if (pKeyword) delete pKeyword;
      if (pTypeParamValue) delete pTypeParamValue;
   }

OFP::TypeParamSpecList::~TypeParamSpecList()
   {
      if (pTypeParamSpecList) delete pTypeParamSpecList;
   }

OFP::StructureConstructor::~StructureConstructor()
   {
      if (pDerivedTypeSpec) delete pDerivedTypeSpec;
      if (pComponentSpecList) delete pComponentSpecList;
   }

OFP::ComponentSpec::~ComponentSpec()
   {
      if (pKeyword) delete pKeyword;
      if (pComponentDataSource) delete pComponentDataSource;
   }

OFP::ComponentDataSource::~ComponentDataSource()
   {
      if (pProcTarget) delete pProcTarget;
      if (pDataTarget) delete pDataTarget;
      if (pExpr) delete pExpr;
   }

OFP::EnumDef::~EnumDef()
   {
      if (pEnumDefStmt) delete pEnumDefStmt;
      if (pEnumeratorDefStmtList) delete pEnumeratorDefStmtList;
      if (pEndEnumStmt) delete pEndEnumStmt;
   }

OFP::EnumDefStmt::~EnumDefStmt()
   {
      if (pLabel) delete pLabel;
      if (pEOS) delete pEOS;
   }

OFP::EnumeratorDefStmt::~EnumeratorDefStmt()
   {
      if (pLabel) delete pLabel;
      if (pEnumeratorList) delete pEnumeratorList;
      if (pEOS) delete pEOS;
   }

OFP::Enumerator::~Enumerator()
   {
      if (pNamedConstant) delete pNamedConstant;
      if (pIntConstantExpr) delete pIntConstantExpr;
   }

OFP::EnumeratorList::~EnumeratorList()
   {
      if (pEnumeratorList) delete pEnumeratorList;
   }

OFP::EndEnumStmt::~EndEnumStmt()
   {
      if (pLabel) delete pLabel;
      if (pEOS) delete pEOS;
   }

OFP::BozLiteralConstant::~BozLiteralConstant()
   {
      if (pHexConstant) delete pHexConstant;
      if (pOctalConstant) delete pOctalConstant;
      if (pBinaryConstant) delete pBinaryConstant;
   }

OFP::ArrayConstructor::~ArrayConstructor()
   {
      if (pAcSpec) delete pAcSpec;
   }

OFP::AcSpec::~AcSpec()
   {
      if (pTypeSpec) delete pTypeSpec;
      if (pAcValueList) delete pAcValueList;
   }

OFP::AcValue::~AcValue()
   {
      if (pAcImpliedDo) delete pAcImpliedDo;
      if (pExpr) delete pExpr;
   }

OFP::AcValueList::~AcValueList()
   {
      if (pAcValueList) delete pAcValueList;
   }

OFP::AcImpliedDo::~AcImpliedDo()
   {
      if (pAcValueList) delete pAcValueList;
      if (pAcImpliedDoControl) delete pAcImpliedDoControl;
   }

OFP::AcImpliedDoControl::~AcImpliedDoControl()
   {
      if (pAcDoVariable) delete pAcDoVariable;
      if (pExpr) delete pExpr;
   }

OFP::AcDoVariable::~AcDoVariable()
   {
      if (pDoVariable) delete pDoVariable;
   }

OFP::TypeDeclarationStmt::~TypeDeclarationStmt()
   {
      if (pLabel) delete pLabel;
      if (pDeclarationTypeSpec) delete pDeclarationTypeSpec;
      if (pAttrSpecList) delete pAttrSpecList;
      if (pEntityDeclList) delete pEntityDeclList;
      if (pEOS) delete pEOS;
   }

#ifdef OBSOLETE
OFP::OptAttrSpecList::~OptAttrSpecList()
   {
      if (pAttrSpecList) delete pAttrSpecList;
   }
#endif

OFP::AttrSpec::~AttrSpec()
   {
      if (pLanguageBindingSpec) delete pLanguageBindingSpec;
      if (pIntentSpec) delete pIntentSpec;
      if (pArraySpec) delete pArraySpec;
      if (pCoarraySpec) delete pCoarraySpec;
      if (pAccessSpec) delete pAccessSpec;
   }

OFP::AttrSpecList::~AttrSpecList()
   {
      if (pAttrSpecList) delete pAttrSpecList;
   }

OFP::EntityDecl::~EntityDecl()
   {
      if (pObjectName) delete pObjectName;
      if (pArraySpec) delete pArraySpec;
      if (pCoarraySpec) delete pCoarraySpec;
      if (pCharLength) delete pCharLength;
      if (pInitialization) delete pInitialization;
   }

OFP::EntityDeclList::~EntityDeclList()
   {
      if (pEntityDeclList) delete pEntityDeclList;
   }

OFP::Initialization::~Initialization()
   {
      if (pInitialDataTarget) delete pInitialDataTarget;
      if (pNullInit) delete pNullInit;
      if (pConstantExpr) delete pConstantExpr;
   }

OFP::NullInit::~NullInit()
   {
      if (pFunctionReference) delete pFunctionReference;
   }

OFP::AccessSpec::~AccessSpec()
   {
   }

OFP::LanguageBindingSpec::~LanguageBindingSpec()
   {
      if (pScon) delete pScon;
   }

OFP::CoarraySpec::~CoarraySpec()
   {
      if (pExplicitCoshapeSpec) delete pExplicitCoshapeSpec;
      if (pDeferredCoshapeSpecList) delete pDeferredCoshapeSpecList;
   }

OFP::DeferredCoshapeSpec::~DeferredCoshapeSpec()
   {
   }

OFP::DeferredCoshapeSpecList::~DeferredCoshapeSpecList()
   {
      if (pDeferredCoshapeSpecList) delete pDeferredCoshapeSpecList;
   }

OFP::ExplicitCoshapeSpec::~ExplicitCoshapeSpec()
   {
      if (pCoboundsEntryList) delete pCoboundsEntryList;
      if (pLowerCobound) delete pLowerCobound;
   }

OFP::CoboundsEntry::~CoboundsEntry()
   {
      if (pLowerCobound) delete pLowerCobound;
      if (pUpperCobound) delete pUpperCobound;
   }

OFP::LowerCobound::~LowerCobound()
   {
      if (pSpecificationExpr) delete pSpecificationExpr;
   }

OFP::UpperCobound::~UpperCobound()
   {
      if (pSpecificationExpr) delete pSpecificationExpr;
   }

OFP::ArraySpec::~ArraySpec()
   {
      if (pAssumedRankSpec) delete pAssumedRankSpec;
      if (pImpliedShapeSpecList) delete pImpliedShapeSpecList;
      if (pAssumedSizeSpec) delete pAssumedSizeSpec;
      if (pDeferredShapeSpecList) delete pDeferredShapeSpecList;
      if (pAssumedShapeSpecList) delete pAssumedShapeSpecList;
      if (pExplicitShapeSpecList) delete pExplicitShapeSpecList;
   }

OFP::ExplicitShapeSpec::~ExplicitShapeSpec()
   {
      if (pLowerBound) delete pLowerBound;
      if (pUpperBound) delete pUpperBound;
   }

OFP::ExplicitShapeSpecList::~ExplicitShapeSpecList()
   {
      if (pExplicitShapeSpecList) delete pExplicitShapeSpecList;
   }

OFP::LowerBound::~LowerBound()
   {
      if (pSpecificationExpr) delete pSpecificationExpr;
   }

OFP::UpperBound::~UpperBound()
   {
      if (pSpecificationExpr) delete pSpecificationExpr;
   }

OFP::AssumedShapeSpec::~AssumedShapeSpec()
   {
      if (pLowerBound) delete pLowerBound;
   }

OFP::AssumedShapeSpecList::~AssumedShapeSpecList()
   {
      if (pAssumedShapeSpecList) delete pAssumedShapeSpecList;
   }

OFP::DeferredShapeSpec::~DeferredShapeSpec()
   {
   }

OFP::DeferredShapeSpecList::~DeferredShapeSpecList()
   {
      if (pDeferredShapeSpecList) delete pDeferredShapeSpecList;
   }

OFP::AssumedSizeSpec::~AssumedSizeSpec()
   {
      if (pExplicitShapeSpecList) delete pExplicitShapeSpecList;
      if (pLowerBound) delete pLowerBound;
   }

OFP::AssumedRankSpec::~AssumedRankSpec()
   {
   }

OFP::ImpliedShapeSpec::~ImpliedShapeSpec()
   {
      if (pLowerBound) delete pLowerBound;
   }

OFP::ImpliedShapeSpecList::~ImpliedShapeSpecList()
   {
      if (pImpliedShapeSpecList) delete pImpliedShapeSpecList;
   }

OFP::IntentSpec::~IntentSpec()
   {
   }

OFP::AccessStmt::~AccessStmt()
   {
      if (pLabel) delete pLabel;
      if (pAccessSpec) delete pAccessSpec;
      if (pAccessIdList) delete pAccessIdList;
      if (pEOS) delete pEOS;
   }

OFP::AccessId::~AccessId()
   {
      if (pGenericSpec) delete pGenericSpec;
   }

OFP::AccessIdList::~AccessIdList()
   {
      if (pAccessIdList) delete pAccessIdList;
   }

OFP::AllocatableStmt::~AllocatableStmt()
   {
      if (pLabel) delete pLabel;
      if (pAllocatableDeclList) delete pAllocatableDeclList;
      if (pEOS) delete pEOS;
   }

OFP::AllocatableDecl::~AllocatableDecl()
   {
      if (pObjectName) delete pObjectName;
      if (pArraySpec) delete pArraySpec;
      if (pCoarraySpec) delete pCoarraySpec;
   }

OFP::AllocatableDeclList::~AllocatableDeclList()
   {
      if (pAllocatableDeclList) delete pAllocatableDeclList;
   }

OFP::AsynchronousStmt::~AsynchronousStmt()
   {
      if (pLabel) delete pLabel;
      if (pObjectNameList) delete pObjectNameList;
      if (pEOS) delete pEOS;
   }

OFP::BindStmt::~BindStmt()
   {
      if (pLabel) delete pLabel;
      if (pLanguageBindingSpec) delete pLanguageBindingSpec;
      if (pBindEntityList) delete pBindEntityList;
      if (pEOS) delete pEOS;
   }

OFP::BindEntity::~BindEntity()
   {
      if (pCommonBlockName) delete pCommonBlockName;
      if (pEntityName) delete pEntityName;
   }

OFP::BindEntityList::~BindEntityList()
   {
      if (pBindEntityList) delete pBindEntityList;
   }

OFP::CodimensionStmt::~CodimensionStmt()
   {
      if (pLabel) delete pLabel;
      if (pCodimensionDeclList) delete pCodimensionDeclList;
      if (pEOS) delete pEOS;
   }

OFP::CodimensionDecl::~CodimensionDecl()
   {
      if (pCoarrayName) delete pCoarrayName;
      if (pCoarraySpec) delete pCoarraySpec;
   }

OFP::CodimensionDeclList::~CodimensionDeclList()
   {
      if (pCodimensionDeclList) delete pCodimensionDeclList;
   }

OFP::ContiguousStmt::~ContiguousStmt()
   {
      if (pLabel) delete pLabel;
      if (pObjectNameList) delete pObjectNameList;
      if (pEOS) delete pEOS;
   }

OFP::ObjectNameList::~ObjectNameList()
   {
      if (pObjectNameList) delete pObjectNameList;
   }

OFP::DataStmt::~DataStmt()
   {
      if (pLabel) delete pLabel;
      if (pDataStmtSetList) delete pDataStmtSetList;
      if (pEOS) delete pEOS;
   }

OFP::DataStmtSet::~DataStmtSet()
   {
      if (pDataStmtObjectList) delete pDataStmtObjectList;
      if (pDataStmtValueList) delete pDataStmtValueList;
   }

OFP::DataStmtSetList::~DataStmtSetList()
   {
      if (pDataStmtSet) delete pDataStmtSet;
   }

OFP::DataStmtObject::~DataStmtObject()
   {
      if (pDataImpliedDo) delete pDataImpliedDo;
      if (pVariable) delete pVariable;
   }

OFP::DataStmtObjectList::~DataStmtObjectList()
   {
      if (pDataStmtObjectList) delete pDataStmtObjectList;
   }

OFP::DataImpliedDo::~DataImpliedDo()
   {
      if (pDataIDoObjectList) delete pDataIDoObjectList;
      if (pDataIDoVariable) delete pDataIDoVariable;
      if (pExpr) delete pExpr;
   }

OFP::DataIDoObject::~DataIDoObject()
   {
      if (pDataImpliedDo) delete pDataImpliedDo;
      if (pStructureComponent) delete pStructureComponent;
      if (pArrayElement) delete pArrayElement;
   }

OFP::DataIDoObjectList::~DataIDoObjectList()
   {
      if (pDataIDoObjectList) delete pDataIDoObjectList;
   }

OFP::DataIDoVariable::~DataIDoVariable()
   {
      if (pDoVariable) delete pDoVariable;
   }

OFP::DataStmtValue::~DataStmtValue()
   {
      if (pDataStmtRepeat) delete pDataStmtRepeat;
      if (pDataStmtConstant) delete pDataStmtConstant;
   }

OFP::DataStmtValueList::~DataStmtValueList()
   {
      if (pDataStmtValueList) delete pDataStmtValueList;
   }

OFP::DataStmtRepeat::~DataStmtRepeat()
   {
      if (pIntConstantSubobject) delete pIntConstantSubobject;
      if (pIcon) delete pIcon;
   }

OFP::DataStmtConstant::~DataStmtConstant()
   {
      if (pStructureConstructor) delete pStructureConstructor;
      if (pInitialDataTarget) delete pInitialDataTarget;
      if (pNullInit) delete pNullInit;
      if (pSignedRealLiteralConstant) delete pSignedRealLiteralConstant;
      if (pSignedIntLiteralConstant) delete pSignedIntLiteralConstant;
      if (pConstantSubobject) delete pConstantSubobject;
      if (pConstant) delete pConstant;
   }

OFP::IntConstantSubobject::~IntConstantSubobject()
   {
      if (pConstantSubobject) delete pConstantSubobject;
   }

OFP::ConstantSubobject::~ConstantSubobject()
   {
      if (pDesignator) delete pDesignator;
   }

OFP::DimensionStmt::~DimensionStmt()
   {
      if (pLabel) delete pLabel;
      if (pArrayNameSpecList) delete pArrayNameSpecList;
      if (pEOS) delete pEOS;
   }

OFP::ArrayNameSpec::~ArrayNameSpec()
   {
      if (pArrayName) delete pArrayName;
      if (pArraySpec) delete pArraySpec;
   }

OFP::ArrayNameSpecList::~ArrayNameSpecList()
   {
      if (pArrayNameSpecList) delete pArrayNameSpecList;
   }

OFP::IntentStmt::~IntentStmt()
   {
      if (pLabel) delete pLabel;
      if (pIntentSpec) delete pIntentSpec;
      if (pDummyArgNameList) delete pDummyArgNameList;
      if (pEOS) delete pEOS;
   }

OFP::DummyArgNameList::~DummyArgNameList()
   {
      if (pDummyArgNameList) delete pDummyArgNameList;
   }

OFP::OptionalStmt::~OptionalStmt()
   {
      if (pLabel) delete pLabel;
      if (pDummyArgNameList) delete pDummyArgNameList;
      if (pEOS) delete pEOS;
   }

OFP::ParameterStmt::~ParameterStmt()
   {
      if (pLabel) delete pLabel;
      if (pNamedConstantDefList) delete pNamedConstantDefList;
      if (pEOS) delete pEOS;
   }

OFP::NamedConstantDef::~NamedConstantDef()
   {
      if (pNamedConstant) delete pNamedConstant;
      if (pConstantExpr) delete pConstantExpr;
   }

OFP::NamedConstantDefList::~NamedConstantDefList()
   {
      if (pNamedConstantDefList) delete pNamedConstantDefList;
   }

OFP::PointerStmt::~PointerStmt()
   {
      if (pLabel) delete pLabel;
      if (pPointerDeclList) delete pPointerDeclList;
      if (pEOS) delete pEOS;
   }

OFP::PointerDecl::~PointerDecl()
   {
      if (pProcEntityName) delete pProcEntityName;
      if (pObjectName) delete pObjectName;
      if (pDeferredShapeSpecList) delete pDeferredShapeSpecList;
   }

OFP::PointerDeclList::~PointerDeclList()
   {
      if (pPointerDeclList) delete pPointerDeclList;
   }

OFP::ProtectedStmt::~ProtectedStmt()
   {
      if (pLabel) delete pLabel;
      if (pEntityNameList) delete pEntityNameList;
      if (pEOS) delete pEOS;
   }

OFP::EntityNameList::~EntityNameList()
   {
      if (pEntityNameList) delete pEntityNameList;
   }

OFP::SaveStmt::~SaveStmt()
   {
      if (pLabel) delete pLabel;
      if (pEOS) delete pEOS;
      if (pSavedEntityList) delete pSavedEntityList;
   }

OFP::SavedEntity::~SavedEntity()
   {
      if (pCommonBlockName) delete pCommonBlockName;
      if (pObjectName) delete pObjectName;
   }

OFP::SavedEntityList::~SavedEntityList()
   {
      if (pSavedEntityList) delete pSavedEntityList;
   }

OFP::ProcPointerName::~ProcPointerName()
   {
      if (pIdent) delete pIdent;
   }

OFP::TargetStmt::~TargetStmt()
   {
      if (pLabel) delete pLabel;
      if (pTargetDeclList) delete pTargetDeclList;
      if (pEOS) delete pEOS;
   }

OFP::TargetDecl::~TargetDecl()
   {
      if (pObjectName) delete pObjectName;
      if (pArraySpec) delete pArraySpec;
      if (pCoarraySpec) delete pCoarraySpec;
   }

OFP::TargetDeclList::~TargetDeclList()
   {
      if (pTargetDeclList) delete pTargetDeclList;
   }

OFP::ValueStmt::~ValueStmt()
   {
      if (pLabel) delete pLabel;
      if (pDummyArgNameList) delete pDummyArgNameList;
      if (pEOS) delete pEOS;
   }

OFP::VolatileStmt::~VolatileStmt()
   {
      if (pLabel) delete pLabel;
      if (pObjectNameList) delete pObjectNameList;
      if (pEOS) delete pEOS;
   }

OFP::ImplicitStmt::~ImplicitStmt()
   {
      if (pLabel) delete pLabel;
      if (pEOS) delete pEOS;
      if (pImplicitSpecList) delete pImplicitSpecList;
   }

OFP::ImplicitSpec::~ImplicitSpec()
   {
      if (pDeclarationTypeSpec) delete pDeclarationTypeSpec;
      if (pLetterSpecList) delete pLetterSpecList;
   }

OFP::ImplicitSpecList::~ImplicitSpecList()
   {
      if (pImplicitSpecList) delete pImplicitSpecList;
   }

OFP::LetterSpec::~LetterSpec()
   {
      if (pLetter) delete pLetter;
   }

OFP::LetterSpecList::~LetterSpecList()
   {
      if (pLetterSpecList) delete pLetterSpecList;
   }

OFP::NamelistStmt::~NamelistStmt()
   {
      if (pLabel) delete pLabel;
      if (pNamelistEntryList) delete pNamelistEntryList;
      if (pEOS) delete pEOS;
   }

OFP::NamelistEntry::~NamelistEntry()
   {
      if (pNamelistGroupName) delete pNamelistGroupName;
      if (pNamelistGroupObjectList) delete pNamelistGroupObjectList;
   }

OFP::NamelistEntryList::~NamelistEntryList()
   {
      if (pNamelistEntry) delete pNamelistEntry;
   }

OFP::NamelistGroupObject::~NamelistGroupObject()
   {
      if (pVariableName) delete pVariableName;
   }

OFP::NamelistGroupObjectList::~NamelistGroupObjectList()
   {
      if (pNamelistGroupObjectList) delete pNamelistGroupObjectList;
   }

OFP::EquivalenceStmt::~EquivalenceStmt()
   {
      if (pLabel) delete pLabel;
      if (pEquivalenceSetList) delete pEquivalenceSetList;
      if (pEOS) delete pEOS;
   }

OFP::EquivalenceSet::~EquivalenceSet()
   {
      if (pEquivalenceObject) delete pEquivalenceObject;
      if (pEquivalenceObjectList) delete pEquivalenceObjectList;
   }

OFP::EquivalenceSetList::~EquivalenceSetList()
   {
      if (pEquivalenceSetList) delete pEquivalenceSetList;
   }

OFP::EquivalenceObject::~EquivalenceObject()
   {
      if (pVariable) delete pVariable;
   }

OFP::EquivalenceObjectList::~EquivalenceObjectList()
   {
      if (pEquivalenceObjectList) delete pEquivalenceObjectList;
   }

OFP::CommonStmt::~CommonStmt()
   {
      if (pLabel) delete pLabel;
      if (pCommonBlockEntry1) delete pCommonBlockEntry1;
      if (pCommonBlockEntryList) delete pCommonBlockEntryList;
      if (pEOS) delete pEOS;
   }

OFP::CommonBlockEntry1::~CommonBlockEntry1()
   {
      if (pCommonBlockObjectList) delete pCommonBlockObjectList;
      if (pCommonBlockName) delete pCommonBlockName;
   }

OFP::CommonBlockEntry::~CommonBlockEntry()
   {
      if (pCommonBlockName) delete pCommonBlockName;
      if (pCommonBlockObjectList) delete pCommonBlockObjectList;
   }

OFP::CommonBlockObject::~CommonBlockObject()
   {
      if (pVariableName) delete pVariableName;
      if (pArraySpec) delete pArraySpec;
   }

OFP::CommonBlockObjectList::~CommonBlockObjectList()
   {
      if (pCommonBlockObjectList) delete pCommonBlockObjectList;
   }

OFP::Designator::~Designator()
   {
      if (pSubstring) delete pSubstring;
      if (pDataRef) delete pDataRef;
   }

OFP::Variable::~Variable()
   {
      if (pDesignator) delete pDesignator;
   }

OFP::VariableName::~VariableName()
   {
      if (pIdent) delete pIdent;
   }

OFP::LogicalVariable::~LogicalVariable()
   {
      if (pVariable) delete pVariable;
   }

OFP::CharVariable::~CharVariable()
   {
      if (pVariable) delete pVariable;
   }

OFP::DefaultCharVariable::~DefaultCharVariable()
   {
      if (pVariable) delete pVariable;
   }

OFP::IntVariable::~IntVariable()
   {
      if (pVariable) delete pVariable;
   }

OFP::Substring::~Substring()
   {
      if (pParentString) delete pParentString;
      if (pSubstringRange) delete pSubstringRange;
   }

OFP::ParentString::~ParentString()
   {
      if (pConstant) delete pConstant;
      if (pStructureComponent) delete pStructureComponent;
      if (pCoindexedNamedObject) delete pCoindexedNamedObject;
      if (pArrayElement) delete pArrayElement;
      if (pScalarVariableName) delete pScalarVariableName;
   }

OFP::SubstringRange::~SubstringRange()
   {
      if (pIntExpr) delete pIntExpr;
   }

OFP::DataRef::~DataRef()
   {
      if (pPartRefList) delete pPartRefList;
   }

OFP::PartRef::~PartRef()
   {
      if (pPartName) delete pPartName;
      if (pSectionSubscriptList) delete pSectionSubscriptList;
      if (pImageSelector) delete pImageSelector;
   }

OFP::StructureComponent::~StructureComponent()
   {
      if (pDataRef) delete pDataRef;
   }

OFP::CoindexedNamedObject::~CoindexedNamedObject()
   {
      if (pDataRef) delete pDataRef;
   }

OFP::TypeParamInquiry::~TypeParamInquiry()
   {
      if (pDesignator) delete pDesignator;
      if (pTypeParamName) delete pTypeParamName;
   }

OFP::ArrayElement::~ArrayElement()
   {
      if (pDataRef) delete pDataRef;
   }

OFP::Subscript::~Subscript()
   {
      if (pIntExpr) delete pIntExpr;
   }

OFP::SectionSubscript::~SectionSubscript()
   {
      if (pSubscriptTriplet) delete pSubscriptTriplet;
      if (pSubscript) delete pSubscript;
   }

OFP::SectionSubscriptList::~SectionSubscriptList()
   {
      if (pSectionSubscriptList) delete pSectionSubscriptList;
   }

OFP::SubscriptTriplet::~SubscriptTriplet()
   {
      if (pSubscript) delete pSubscript;
      if (pStride) delete pStride;
   }

OFP::Stride::~Stride()
   {
      if (pIntExpr) delete pIntExpr;
   }

OFP::ImageSelector::~ImageSelector()
   {
      if (pCosubscriptList) delete pCosubscriptList;
   }

OFP::Cosubscript::~Cosubscript()
   {
      if (pIntExpr) delete pIntExpr;
   }

OFP::CosubscriptList::~CosubscriptList()
   {
      if (pCosubscriptList) delete pCosubscriptList;
   }

OFP::AllocateStmt::~AllocateStmt()
   {
      if (pLabel) delete pLabel;
      if (pTypeSpec) delete pTypeSpec;
      if (pAllocationList) delete pAllocationList;
      if (pAllocOptList) delete pAllocOptList;
      if (pEOS) delete pEOS;
   }

OFP::AllocOpt::~AllocOpt()
   {
      if (pStatVariable) delete pStatVariable;
      if (pExpr) delete pExpr;
      if (pErrmsgVariable) delete pErrmsgVariable;
   }

OFP::AllocOptList::~AllocOptList()
   {
      if (pAllocOptList) delete pAllocOptList;
   }

OFP::StatVariable::~StatVariable()
   {
      if (pIntVariable) delete pIntVariable;
   }

OFP::ErrmsgVariable::~ErrmsgVariable()
   {
      if (pDefaultCharVariable) delete pDefaultCharVariable;
   }

OFP::Allocation::~Allocation()
   {
      if (pAllocateObject) delete pAllocateObject;
      if (pAllocateShapeSpecList) delete pAllocateShapeSpecList;
      if (pAllocateCoarraySpec) delete pAllocateCoarraySpec;
   }

OFP::AllocationList::~AllocationList()
   {
      if (pAllocationList) delete pAllocationList;
   }

OFP::AllocateObject::~AllocateObject()
   {
      if (pStructureComponent) delete pStructureComponent;
   }

OFP::AllocateObjectList::~AllocateObjectList()
   {
      if (pAllocateObjectList) delete pAllocateObjectList;
   }

OFP::AllocateShapeSpec::~AllocateShapeSpec()
   {
      if (pLowerBoundExpr) delete pLowerBoundExpr;
      if (pUpperBoundExpr) delete pUpperBoundExpr;
   }

OFP::AllocateShapeSpecList::~AllocateShapeSpecList()
   {
      if (pAllocateShapeSpecList) delete pAllocateShapeSpecList;
   }

OFP::LowerBoundExpr::~LowerBoundExpr()
   {
      if (pIntExpr) delete pIntExpr;
   }

OFP::UpperBoundExpr::~UpperBoundExpr()
   {
      if (pIntExpr) delete pIntExpr;
   }

OFP::AllocateCoarraySpec::~AllocateCoarraySpec()
   {
      if (pAllocateCoshapeSpecList) delete pAllocateCoshapeSpecList;
      if (pLowerBoundExpr) delete pLowerBoundExpr;
   }

OFP::AllocateCoshapeSpec::~AllocateCoshapeSpec()
   {
      if (pLowerBoundExpr) delete pLowerBoundExpr;
      if (pUpperBoundExpr) delete pUpperBoundExpr;
   }

OFP::AllocateCoshapeSpecList::~AllocateCoshapeSpecList()
   {
      if (pAllocateCoshapeSpecList) delete pAllocateCoshapeSpecList;
   }

OFP::NullifyStmt::~NullifyStmt()
   {
      if (pLabel) delete pLabel;
      if (pPointerObjectList) delete pPointerObjectList;
      if (pEOS) delete pEOS;
   }

OFP::PointerObject::~PointerObject()
   {
      if (pProcPointerName) delete pProcPointerName;
      if (pStructureComponent) delete pStructureComponent;
      if (pVariableName) delete pVariableName;
   }

OFP::PointerObjectList::~PointerObjectList()
   {
      if (pPointerObjectList) delete pPointerObjectList;
   }

OFP::DeallocateStmt::~DeallocateStmt()
   {
      if (pLabel) delete pLabel;
      if (pAllocateObjectList) delete pAllocateObjectList;
      if (pDeallocOptList) delete pDeallocOptList;
      if (pEOS) delete pEOS;
   }

OFP::DeallocOpt::~DeallocOpt()
   {
      if (pErrmsgVariable) delete pErrmsgVariable;
      if (pStatVariable) delete pStatVariable;
   }

OFP::DeallocOptList::~DeallocOptList()
   {
      if (pDeallocOptList) delete pDeallocOptList;
   }

#ifdef MOVED_TO_OFP_EXPR
OFP::IntrinsicOperator::~IntrinsicOperator()
   {
      if (pEquivOp) delete pEquivOp;
      if (pOrOp) delete pOrOp;
      if (pAndOp) delete pAndOp;
      if (pNotOp) delete pNotOp;
      if (pRelOp) delete pRelOp;
      if (pConcatOp) delete pConcatOp;
      if (pAddOp) delete pAddOp;
      if (pMultOp) delete pMultOp;
      if (pPowerOp) delete pPowerOp;
   }
#endif

OFP::DefinedOperator::~DefinedOperator()
   {
      if (pExtendedIntrinsicOp) delete pExtendedIntrinsicOp;
      if (pDefinedBinaryOp) delete pDefinedBinaryOp;
      if (pDefinedUnaryOp) delete pDefinedUnaryOp;
   }

OFP::ExtendedIntrinsicOp::~ExtendedIntrinsicOp()
   {
      if (pIntrinsicOperator) delete pIntrinsicOperator;
   }

OFP::Primary::~Primary()
   {
      if (pTypeParamInquiry) delete pTypeParamInquiry;
      if (pFunctionReference) delete pFunctionReference;
      if (pStructureConstructor) delete pStructureConstructor;
      if (pArrayConstructor) delete pArrayConstructor;
      if (pDesignator) delete pDesignator;
      if (pConstant) delete pConstant;
   }

OFP::DefinedUnaryOp::~DefinedUnaryOp()
   {
      if (pDop) delete pDop;
   }

#ifdef MOVED_TO_OFP_EXPR
OFP::Expr::~Expr()
   {
      if (pDefinedBinaryOp) delete pDefinedBinaryOp;
      if (pDefinedUnaryOp) delete pDefinedUnaryOp;
      if (pPrimary) delete pPrimary;
   }
#endif

OFP::DefinedBinaryOp::~DefinedBinaryOp()
   {
      if (pDop) delete pDop;
   }

OFP::LogicalExpr::~LogicalExpr()
   {
      if (pExpr) delete pExpr;
   }

OFP::DefaultCharExpr::~DefaultCharExpr()
   {
      if (pExpr) delete pExpr;
   }

OFP::IntExpr::~IntExpr()
   {
      if (pExpr) delete pExpr;
   }

OFP::SpecificationExpr::~SpecificationExpr()
   {
      if (pIntExpr) delete pIntExpr;
   }

OFP::ConstantExpr::~ConstantExpr()
   {
      if (pExpr) delete pExpr;
   }

OFP::IntConstantExpr::~IntConstantExpr()
   {
      if (pIntExpr) delete pIntExpr;
   }

OFP::AssignmentStmt::~AssignmentStmt()
   {
      if (pLabel) delete pLabel;
      if (pVariable) delete pVariable;
      if (pExpr) delete pExpr;
      if (pEOS) delete pEOS;
   }

OFP::PointerAssignmentStmt::~PointerAssignmentStmt()
   {
      if (pLabel) delete pLabel;
      if (pProcPointerObject) delete pProcPointerObject;
      if (pProcTarget) delete pProcTarget;
      if (pEOS) delete pEOS;
      if (pDataPointerObject) delete pDataPointerObject;
      if (pBoundsRemappingList) delete pBoundsRemappingList;
      if (pDataTarget) delete pDataTarget;
      if (pBoundsSpecList) delete pBoundsSpecList;
   }

OFP::DataPointerObject::~DataPointerObject()
   {
      if (pVariable) delete pVariable;
      if (pDataPointerComponentName) delete pDataPointerComponentName;
      if (pVariableName) delete pVariableName;
   }

OFP::BoundsSpec::~BoundsSpec()
   {
      if (pLowerBoundExpr) delete pLowerBoundExpr;
   }

OFP::BoundsSpecList::~BoundsSpecList()
   {
      if (pBoundsSpecList) delete pBoundsSpecList;
   }

OFP::BoundsRemapping::~BoundsRemapping()
   {
      if (pLowerBoundExpr) delete pLowerBoundExpr;
      if (pUpperBoundExpr) delete pUpperBoundExpr;
   }

OFP::BoundsRemappingList::~BoundsRemappingList()
   {
      if (pBoundsRemappingList) delete pBoundsRemappingList;
   }

OFP::DataTarget::~DataTarget()
   {
      if (pExpr) delete pExpr;
   }

OFP::ProcPointerObject::~ProcPointerObject()
   {
      if (pProcComponentRef) delete pProcComponentRef;
      if (pProcPointerName) delete pProcPointerName;
   }

OFP::ProcComponentRef::~ProcComponentRef()
   {
      if (pVariable) delete pVariable;
      if (pProcedureComponentName) delete pProcedureComponentName;
   }

OFP::ProcTarget::~ProcTarget()
   {
      if (pProcComponentRef) delete pProcComponentRef;
      if (pProcedureName) delete pProcedureName;
      if (pExpr) delete pExpr;
   }

OFP::WhereStmt::~WhereStmt()
   {
      if (pLabel) delete pLabel;
      if (pMaskExpr) delete pMaskExpr;
      if (pWhereAssignmentStmt) delete pWhereAssignmentStmt;
   }

OFP::WhereConstruct::~WhereConstruct()
   {
      if (pWhereConstructStmt) delete pWhereConstructStmt;
      if (pWhereBodyConstructList) delete pWhereBodyConstructList;
      if (pMaskedElsewhereClauseList) delete pMaskedElsewhereClauseList;
      if (pElsewhereClause) delete pElsewhereClause;
      if (pEndWhereStmt) delete pEndWhereStmt;
   }

OFP::MaskedElsewhereClause::~MaskedElsewhereClause()
   {
      if (pMaskedElsewhereStmt) delete pMaskedElsewhereStmt;
      if (pWhereBodyConstructList) delete pWhereBodyConstructList;
   }

OFP::ElsewhereClause::~ElsewhereClause()
   {
      if (pElsewhereStmt) delete pElsewhereStmt;
      if (pWhereBodyConstructList) delete pWhereBodyConstructList;
   }

OFP::WhereConstructStmt::~WhereConstructStmt()
   {
      if (pLabel) delete pLabel;
      if (pIdent) delete pIdent;
      if (pMaskExpr) delete pMaskExpr;
      if (pEOS) delete pEOS;
   }

OFP::WhereBodyConstruct::~WhereBodyConstruct()
   {
      if (pWhereConstruct) delete pWhereConstruct;
      if (pWhereStmt) delete pWhereStmt;
      if (pWhereAssignmentStmt) delete pWhereAssignmentStmt;
   }

OFP::WhereAssignmentStmt::~WhereAssignmentStmt()
   {
      if (pAssignmentStmt) delete pAssignmentStmt;
   }

OFP::MaskExpr::~MaskExpr()
   {
      if (pLogicalExpr) delete pLogicalExpr;
   }

OFP::MaskedElsewhereStmt::~MaskedElsewhereStmt()
   {
      if (pLabel) delete pLabel;
      if (pMaskExpr) delete pMaskExpr;
      if (pIdent) delete pIdent;
      if (pEOS) delete pEOS;
   }

OFP::ElsewhereStmt::~ElsewhereStmt()
   {
      if (pLabel) delete pLabel;
      if (pIdent) delete pIdent;
      if (pEOS) delete pEOS;
   }

OFP::EndWhereStmt::~EndWhereStmt()
   {
      if (pLabel) delete pLabel;
      if (pIdent) delete pIdent;
      if (pEOS) delete pEOS;
   }

OFP::ForallConstruct::~ForallConstruct()
   {
      if (pForallConstructStmt) delete pForallConstructStmt;
      if (pForallBodyConstructList) delete pForallBodyConstructList;
      if (pEndForallStmt) delete pEndForallStmt;
   }

OFP::ForallConstructStmt::~ForallConstructStmt()
   {
      if (pLabel) delete pLabel;
      if (pForallConstructName) delete pForallConstructName;
      if (pForallHeader) delete pForallHeader;
      if (pEOS) delete pEOS;
   }

OFP::ForallHeader::~ForallHeader()
   {
      if (pTypeSpec) delete pTypeSpec;
      if (pForallTripletSpecList) delete pForallTripletSpecList;
      if (pMaskExpr) delete pMaskExpr;
   }

OFP::ForallTripletSpec::~ForallTripletSpec()
   {
      if (pIndexName) delete pIndexName;
      if (pForallLimit) delete pForallLimit;
      if (pForallStep) delete pForallStep;
   }

OFP::ForallTripletSpecList::~ForallTripletSpecList()
   {
      if (pForallTripletSpecList) delete pForallTripletSpecList;
   }

OFP::ForallLimit::~ForallLimit()
   {
      if (pIntExpr) delete pIntExpr;
   }

OFP::ForallStep::~ForallStep()
   {
      if (pIntExpr) delete pIntExpr;
   }

OFP::ForallBodyConstruct::~ForallBodyConstruct()
   {
      if (pForallStmt) delete pForallStmt;
      if (pForallConstruct) delete pForallConstruct;
      if (pWhereConstruct) delete pWhereConstruct;
      if (pWhereStmt) delete pWhereStmt;
      if (pForallAssignmentStmt) delete pForallAssignmentStmt;
   }

OFP::ForallAssignmentStmt::~ForallAssignmentStmt()
   {
      if (pPointerAssignmentStmt) delete pPointerAssignmentStmt;
      if (pAssignmentStmt) delete pAssignmentStmt;
   }

OFP::EndForallStmt::~EndForallStmt()
   {
      if (pLabel) delete pLabel;
      if (pForallConstructName) delete pForallConstructName;
      if (pEOS) delete pEOS;
   }

OFP::ForallStmt::~ForallStmt()
   {
      if (pLabel) delete pLabel;
      if (pForallHeader) delete pForallHeader;
      if (pForallAssignmentStmt) delete pForallAssignmentStmt;
   }

OFP::Block::~Block()
   {
      if (pExecutionPartConstructList) delete pExecutionPartConstructList;
   }

OFP::AssociateConstruct::~AssociateConstruct()
   {
      if (pAssociateStmt) delete pAssociateStmt;
      if (pBlock) delete pBlock;
      if (pEndAssociateStmt) delete pEndAssociateStmt;
   }

OFP::AssociateStmt::~AssociateStmt()
   {
      if (pLabel) delete pLabel;
      if (pAssociateConstructName) delete pAssociateConstructName;
      if (pAssociationList) delete pAssociationList;
      if (pEOS) delete pEOS;
   }

OFP::Association::~Association()
   {
      if (pAssociateName) delete pAssociateName;
      if (pSelector) delete pSelector;
   }

OFP::AssociationList::~AssociationList()
   {
      if (pAssociationList) delete pAssociationList;
   }

OFP::Selector::~Selector()
   {
      if (pVariable) delete pVariable;
      if (pExpr) delete pExpr;
   }

OFP::EndAssociateStmt::~EndAssociateStmt()
   {
      if (pLabel) delete pLabel;
      if (pAssociateConstructName) delete pAssociateConstructName;
      if (pEOS) delete pEOS;
   }

OFP::BlockConstruct::~BlockConstruct()
   {
      if (pBlockStmt) delete pBlockStmt;
      if (pSpecificationPart) delete pSpecificationPart;
      if (pBlock) delete pBlock;
      if (pEndBlockStmt) delete pEndBlockStmt;
   }

OFP::BlockStmt::~BlockStmt()
   {
      if (pLabel) delete pLabel;
      if (pBlockConstructName) delete pBlockConstructName;
      if (pEOS) delete pEOS;
   }

OFP::EndBlockStmt::~EndBlockStmt()
   {
      if (pLabel) delete pLabel;
      if (pBlockConstructName) delete pBlockConstructName;
      if (pEOS) delete pEOS;
   }

OFP::CriticalConstruct::~CriticalConstruct()
   {
      if (pCriticalStmt) delete pCriticalStmt;
      if (pBlock) delete pBlock;
      if (pEndCriticalStmt) delete pEndCriticalStmt;
   }

OFP::CriticalStmt::~CriticalStmt()
   {
      if (pLabel) delete pLabel;
      if (pCriticalConstructName) delete pCriticalConstructName;
      if (pEOS) delete pEOS;
   }

OFP::EndCriticalStmt::~EndCriticalStmt()
   {
      if (pLabel) delete pLabel;
      if (pCriticalConstructName) delete pCriticalConstructName;
      if (pEOS) delete pEOS;
   }

OFP::LabelDoStmt::~LabelDoStmt()
   {
      if (pLabel) delete pLabel;
      if (pDoConstructName) delete pDoConstructName;
      if (pLblRef) delete pLblRef;
      if (pLoopControl) delete pLoopControl;
      if (pEOS) delete pEOS;
   }

OFP::NonlabelDoStmt::~NonlabelDoStmt()
   {
      if (pLabel) delete pLabel;
      if (pDoConstructName) delete pDoConstructName;
      if (pLoopControl) delete pLoopControl;
      if (pEOS) delete pEOS;
   }

OFP::LoopControl::~LoopControl()
   {
      if (pForallHeader) delete pForallHeader;
      if (pExpr) delete pExpr;
      if (pDoVariable) delete pDoVariable;
   }

OFP::DoVariable::~DoVariable()
   {
      if (pScalarIntVariableName) delete pScalarIntVariableName;
   }

OFP::EndDoStmt::~EndDoStmt()
   {
      if (pLabel) delete pLabel;
      if (pDoConstructName) delete pDoConstructName;
      if (pEOS) delete pEOS;
   }

OFP::CycleStmt::~CycleStmt()
   {
      if (pLabel) delete pLabel;
      if (pDoConstructName) delete pDoConstructName;
      if (pEOS) delete pEOS;
   }

OFP::IfConstruct::~IfConstruct()
   {
      if (pIfThenStmt) delete pIfThenStmt;
      if (pBlock) delete pBlock;
      if (pElseIfStmtAndBlockList) delete pElseIfStmtAndBlockList;
      if (pElseStmtAndBlock) delete pElseStmtAndBlock;
      if (pEndIfStmt) delete pEndIfStmt;
   }

OFP::ElseIfStmtAndBlock::~ElseIfStmtAndBlock()
   {
      if (pElseIfStmt) delete pElseIfStmt;
      if (pBlock) delete pBlock;
   }

OFP::ElseStmtAndBlock::~ElseStmtAndBlock()
   {
      if (pElseStmt) delete pElseStmt;
      if (pBlock) delete pBlock;
   }

OFP::IfThenStmt::~IfThenStmt()
   {
      if (pLabel) delete pLabel;
      if (pIfConstructName) delete pIfConstructName;
      if (pLogicalExpr) delete pLogicalExpr;
      if (pEOS) delete pEOS;
   }

OFP::ElseIfStmt::~ElseIfStmt()
   {
      if (pLabel) delete pLabel;
      if (pLogicalExpr) delete pLogicalExpr;
      if (pIfConstructName) delete pIfConstructName;
      if (pEOS) delete pEOS;
   }

OFP::ElseStmt::~ElseStmt()
   {
      if (pLabel) delete pLabel;
      if (pIfConstructName) delete pIfConstructName;
      if (pEOS) delete pEOS;
   }

OFP::EndIfStmt::~EndIfStmt()
   {
      if (pLabel) delete pLabel;
      if (pIfConstructName) delete pIfConstructName;
      if (pEOS) delete pEOS;
   }

OFP::IfStmt::~IfStmt()
   {
      if (pLabel) delete pLabel;
      if (pLogicalExpr) delete pLogicalExpr;
      if (pActionStmt) delete pActionStmt;
   }

OFP::CaseConstruct::~CaseConstruct()
   {
      if (pSelectCaseStmt) delete pSelectCaseStmt;
      if (pCaseStmtAndBlockList) delete pCaseStmtAndBlockList;
      if (pEndSelectStmt) delete pEndSelectStmt;
   }

OFP::CaseStmtAndBlock::~CaseStmtAndBlock()
   {
      if (pCaseStmt) delete pCaseStmt;
      if (pBlock) delete pBlock;
   }

OFP::SelectCaseStmt::~SelectCaseStmt()
   {
      if (pLabel) delete pLabel;
      if (pCaseConstructName) delete pCaseConstructName;
      if (pCaseExpr) delete pCaseExpr;
      if (pEOS) delete pEOS;
   }

OFP::CaseStmt::~CaseStmt()
   {
      if (pLabel) delete pLabel;
      if (pCaseSelector) delete pCaseSelector;
      if (pCaseConstructName) delete pCaseConstructName;
      if (pEOS) delete pEOS;
   }

OFP::EndSelectStmt::~EndSelectStmt()
   {
      if (pLabel) delete pLabel;
      if (pCaseConstructName) delete pCaseConstructName;
      if (pEOS) delete pEOS;
   }

OFP::CaseExpr::~CaseExpr()
   {
      if (pExpr) delete pExpr;
   }

OFP::CaseSelector::~CaseSelector()
   {
      if (pCaseValueRangeList) delete pCaseValueRangeList;
   }

OFP::CaseValueRange::~CaseValueRange()
   {
      if (pCaseValue) delete pCaseValue;
   }

OFP::CaseValueRangeList::~CaseValueRangeList()
   {
      if (pCaseValueRangeList) delete pCaseValueRangeList;
   }

OFP::CaseValue::~CaseValue()
   {
      if (pConstantExpr) delete pConstantExpr;
   }

OFP::SelectTypeConstruct::~SelectTypeConstruct()
   {
      if (pSelectTypeStmt) delete pSelectTypeStmt;
      if (pTypeGuardStmtAndBlockList) delete pTypeGuardStmtAndBlockList;
      if (pEndSelectTypeStmt) delete pEndSelectTypeStmt;
   }

OFP::TypeGuardStmtAndBlock::~TypeGuardStmtAndBlock()
   {
      if (pTypeGuardStmt) delete pTypeGuardStmt;
      if (pBlock) delete pBlock;
   }

OFP::SelectTypeStmt::~SelectTypeStmt()
   {
      if (pSelectConstructName) delete pSelectConstructName;
      if (pAssociateName) delete pAssociateName;
      if (pSelector) delete pSelector;
      if (pEOS) delete pEOS;
   }

OFP::TypeGuardStmt::~TypeGuardStmt()
   {
      if (pSelectConstructName) delete pSelectConstructName;
      if (pEOS) delete pEOS;
      if (pDerivedTypeSpec) delete pDerivedTypeSpec;
      if (pTypeSpec) delete pTypeSpec;
   }

OFP::EndSelectTypeStmt::~EndSelectTypeStmt()
   {
      if (pSelectConstructName) delete pSelectConstructName;
      if (pEOS) delete pEOS;
   }

OFP::ExitStmt::~ExitStmt()
   {
      if (pLabel) delete pLabel;
      if (pConstructName) delete pConstructName;
      if (pEOS) delete pEOS;
   }

OFP::GotoStmt::~GotoStmt()
   {
      if (pLabel) delete pLabel;
      if (pLblRef) delete pLblRef;
      if (pEOS) delete pEOS;
   }

OFP::ComputedGotoStmt::~ComputedGotoStmt()
   {
      if (pLabel) delete pLabel;
      if (pLabelList) delete pLabelList;
      if (pExpr) delete pExpr;
      if (pEOS) delete pEOS;
   }

OFP::ArithmeticIfStmt::~ArithmeticIfStmt()
   {
      if (pLabel) delete pLabel;
      if (pExpr) delete pExpr;
      if (pLblRef) delete pLblRef;
      if (pEOS) delete pEOS;
   }

OFP::ContinueStmt::~ContinueStmt()
   {
      if (pLabel) delete pLabel;
      if (pEOS) delete pEOS;
   }

OFP::StopStmt::~StopStmt()
   {
      if (pLabel) delete pLabel;
      if (pStopCode) delete pStopCode;
      if (pEOS) delete pEOS;
   }

OFP::ErrorStopStmt::~ErrorStopStmt()
   {
      if (pLabel) delete pLabel;
      if (pStopCode) delete pStopCode;
      if (pEOS) delete pEOS;
   }

OFP::StopCode::~StopCode()
   {
      if (pIcon) delete pIcon;
      if (pScon) delete pScon;
   }

OFP::SyncAllStmt::~SyncAllStmt()
   {
      if (pLabel) delete pLabel;
      if (pEOS) delete pEOS;
      if (pSyncStatList) delete pSyncStatList;
   }

OFP::SyncStat::~SyncStat()
   {
      if (pErrmsgVariable) delete pErrmsgVariable;
      if (pStatVariable) delete pStatVariable;
   }

OFP::SyncStatList::~SyncStatList()
   {
      if (pSyncStatList) delete pSyncStatList;
   }

OFP::SyncImagesStmt::~SyncImagesStmt()
   {
      if (pLabel) delete pLabel;
      if (pImageSet) delete pImageSet;
      if (pSyncStatList) delete pSyncStatList;
      if (pEOS) delete pEOS;
   }

OFP::ImageSet::~ImageSet()
   {
      if (pIntExpr) delete pIntExpr;
   }

OFP::SyncMemoryStmt::~SyncMemoryStmt()
   {
      if (pLabel) delete pLabel;
      if (pEOS) delete pEOS;
      if (pSyncStatList) delete pSyncStatList;
   }

OFP::LockStmt::~LockStmt()
   {
      if (pLabel) delete pLabel;
      if (pLockVariable) delete pLockVariable;
      if (pLockStatList) delete pLockStatList;
      if (pEOS) delete pEOS;
   }

OFP::LockStat::~LockStat()
   {
      if (pSyncStat) delete pSyncStat;
      if (pLogicalVariable) delete pLogicalVariable;
   }

OFP::LockStatList::~LockStatList()
   {
      if (pLockStatList) delete pLockStatList;
   }

OFP::UnlockStmt::~UnlockStmt()
   {
      if (pLabel) delete pLabel;
      if (pLockVariable) delete pLockVariable;
      if (pSyncStatList) delete pSyncStatList;
      if (pEOS) delete pEOS;
   }

OFP::LockVariable::~LockVariable()
   {
      if (pVariable) delete pVariable;
   }

OFP::PauseStmt::~PauseStmt()
   {
      if (pLabel) delete pLabel;
      if (pStopCode) delete pStopCode;
      if (pEOS) delete pEOS;
   }

OFP::IoUnit::~IoUnit()
   {
      if (pCharVariable) delete pCharVariable;
      if (pIntExpr) delete pIntExpr;
   }

OFP::FileUnitNumber::~FileUnitNumber()
   {
      if (pIntExpr) delete pIntExpr;
   }

OFP::OpenStmt::~OpenStmt()
   {
      if (pLabel) delete pLabel;
      if (pConnectSpecList) delete pConnectSpecList;
      if (pEOS) delete pEOS;
   }

OFP::ConnectSpec::~ConnectSpec()
   {
      if (pDefaultCharExpr) delete pDefaultCharExpr;
      if (pIntExpr) delete pIntExpr;
      if (pIntVariable) delete pIntVariable;
      if (pIomsgVariable) delete pIomsgVariable;
      if (pFileNameExpr) delete pFileNameExpr;
      if (pLblRef) delete pLblRef;
      if (pFileUnitNumber) delete pFileUnitNumber;
   }

OFP::ConnectSpecList::~ConnectSpecList()
   {
      if (pConnectSpecList) delete pConnectSpecList;
   }

OFP::FileNameExpr::~FileNameExpr()
   {
      if (pDefaultCharExpr) delete pDefaultCharExpr;
   }

OFP::IomsgVariable::~IomsgVariable()
   {
      if (pDefaultCharExpr) delete pDefaultCharExpr;
   }

OFP::CloseStmt::~CloseStmt()
   {
      if (pLabel) delete pLabel;
      if (pCloseSpecList) delete pCloseSpecList;
      if (pEOS) delete pEOS;
   }

OFP::CloseSpec::~CloseSpec()
   {
      if (pDefaultCharExpr) delete pDefaultCharExpr;
      if (pLblRef) delete pLblRef;
      if (pIomsgVariable) delete pIomsgVariable;
      if (pIntVariable) delete pIntVariable;
      if (pFileUnitNumber) delete pFileUnitNumber;
   }

OFP::CloseSpecList::~CloseSpecList()
   {
      if (pCloseSpecList) delete pCloseSpecList;
   }

OFP::ReadStmt::~ReadStmt()
   {
      if (pLabel) delete pLabel;
      if (pFormat) delete pFormat;
      if (pInputItemList) delete pInputItemList;
      if (pEOS) delete pEOS;
      if (pIoControlSpecList) delete pIoControlSpecList;
   }

OFP::WriteStmt::~WriteStmt()
   {
      if (pLabel) delete pLabel;
      if (pIoControlSpecList) delete pIoControlSpecList;
      if (pOutputItemList) delete pOutputItemList;
      if (pEOS) delete pEOS;
   }

OFP::PrintStmt::~PrintStmt()
   {
      if (pLabel) delete pLabel;
      if (pFormat) delete pFormat;
      if (pOutputItemList) delete pOutputItemList;
      if (pEOS) delete pEOS;
   }

OFP::IoControlSpec::~IoControlSpec()
   {
      if (pIntVariable) delete pIntVariable;
      if (pDefaultCharExpr) delete pDefaultCharExpr;
      if (pExpr) delete pExpr;
      if (pIomsgVariable) delete pIomsgVariable;
      if (pIdVariable) delete pIdVariable;
      if (pLblRef) delete pLblRef;
      if (pNamelistGroupName) delete pNamelistGroupName;
      if (pFormat) delete pFormat;
      if (pIoUnit) delete pIoUnit;
   }

OFP::IoControlSpecList::~IoControlSpecList()
   {
      if (pIoControlSpecList) delete pIoControlSpecList;
   }

OFP::IdVariable::~IdVariable()
   {
      if (pIntVariable) delete pIntVariable;
   }

OFP::Format::~Format()
   {
      if (pLabel) delete pLabel;
      if (pDefaultCharExpr) delete pDefaultCharExpr;
   }

OFP::InputItem::~InputItem()
   {
      if (pIoImpliedDo) delete pIoImpliedDo;
      if (pVariable) delete pVariable;
   }

OFP::InputItemList::~InputItemList()
   {
      if (pInputItemList) delete pInputItemList;
   }

OFP::OutputItem::~OutputItem()
   {
      if (pIoImpliedDo) delete pIoImpliedDo;
      if (pExpr) delete pExpr;
   }

OFP::OutputItemList::~OutputItemList()
   {
      if (pOutputItemList) delete pOutputItemList;
   }

OFP::IoImpliedDo::~IoImpliedDo()
   {
      if (pIoImpliedDoObjectList) delete pIoImpliedDoObjectList;
      if (pIoImpliedDoControl) delete pIoImpliedDoControl;
   }

OFP::IoImpliedDoObject::~IoImpliedDoObject()
   {
      if (pOutputItem) delete pOutputItem;
      if (pInputItem) delete pInputItem;
   }

OFP::IoImpliedDoObjectList::~IoImpliedDoObjectList()
   {
      if (pIoImpliedDoObjectList) delete pIoImpliedDoObjectList;
   }

OFP::IoImpliedDoControl::~IoImpliedDoControl()
   {
      if (pDoVariable) delete pDoVariable;
      if (pIntExpr) delete pIntExpr;
   }

OFP::WaitStmt::~WaitStmt()
   {
      if (pLabel) delete pLabel;
      if (pWaitSpecList) delete pWaitSpecList;
      if (pEOS) delete pEOS;
   }

OFP::WaitSpec::~WaitSpec()
   {
      if (pIntExpr) delete pIntExpr;
      if (pIomsgVariable) delete pIomsgVariable;
      if (pLblRef) delete pLblRef;
      if (pFileUnitNumber) delete pFileUnitNumber;
   }

OFP::WaitSpecList::~WaitSpecList()
   {
      if (pWaitSpecList) delete pWaitSpecList;
   }

OFP::BackspaceStmt::~BackspaceStmt()
   {
      if (pLabel) delete pLabel;
      if (pPositionSpecList) delete pPositionSpecList;
      if (pEOS) delete pEOS;
      if (pFileUnitNumber) delete pFileUnitNumber;
   }

OFP::EndfileStmt::~EndfileStmt()
   {
      if (pLabel) delete pLabel;
      if (pPositionSpecList) delete pPositionSpecList;
      if (pEOS) delete pEOS;
      if (pFileUnitNumber) delete pFileUnitNumber;
   }

OFP::RewindStmt::~RewindStmt()
   {
      if (pLabel) delete pLabel;
      if (pPositionSpecList) delete pPositionSpecList;
      if (pEOS) delete pEOS;
      if (pFileUnitNumber) delete pFileUnitNumber;
   }

OFP::PositionSpec::~PositionSpec()
   {
      if (pLblRef) delete pLblRef;
      if (pIomsgVariable) delete pIomsgVariable;
      if (pIntVariable) delete pIntVariable;
      if (pFileUnitNumber) delete pFileUnitNumber;
   }

OFP::PositionSpecList::~PositionSpecList()
   {
      if (pPositionSpecList) delete pPositionSpecList;
   }

OFP::FlushStmt::~FlushStmt()
   {
      if (pLabel) delete pLabel;
      if (pFlushSpecList) delete pFlushSpecList;
      if (pEOS) delete pEOS;
      if (pFileUnitNumber) delete pFileUnitNumber;
   }

OFP::FlushSpec::~FlushSpec()
   {
      if (pLblRef) delete pLblRef;
      if (pIomsgVariable) delete pIomsgVariable;
      if (pIntVariable) delete pIntVariable;
      if (pFileUnitNumber) delete pFileUnitNumber;
   }

OFP::FlushSpecList::~FlushSpecList()
   {
      if (pFlushSpecList) delete pFlushSpecList;
   }

OFP::InquireStmt::~InquireStmt()
   {
      if (pLabel) delete pLabel;
      if (pIntVariable) delete pIntVariable;
      if (pOutputItemList) delete pOutputItemList;
      if (pEOS) delete pEOS;
      if (pInquireSpecList) delete pInquireSpecList;
   }

OFP::InquireSpec::~InquireSpec()
   {
      if (pDefaultCharExpr) delete pDefaultCharExpr;
      if (pIntVariable) delete pIntVariable;
      if (pLogicalVariable) delete pLogicalVariable;
      if (pIomsgVariable) delete pIomsgVariable;
      if (pIntExpr) delete pIntExpr;
      if (pLblRef) delete pLblRef;
      if (pFileNameExpr) delete pFileNameExpr;
      if (pFileUnitNumber) delete pFileUnitNumber;
   }

OFP::InquireSpecList::~InquireSpecList()
   {
      if (pInquireSpecList) delete pInquireSpecList;
   }

OFP::FormatStmt::~FormatStmt()
   {
      if (pLabel) delete pLabel;
      if (pFormatSpecification) delete pFormatSpecification;
      if (pEOS) delete pEOS;
   }

OFP::FormatSpecification::~FormatSpecification()
   {
      if (pFormatItems) delete pFormatItems;
      if (pUnlimitedFormatItem) delete pUnlimitedFormatItem;
   }

OFP::FormatItems::~FormatItems()
   {
      if (pFormatItem) delete pFormatItem;
   }

OFP::FormatItem::~FormatItem()
   {
      if (pIcon) delete pIcon;
      if (pFormatItems) delete pFormatItems;
      if (pScon) delete pScon;
      if (pControlEditDesc) delete pControlEditDesc;
      if (pDataEditDesc) delete pDataEditDesc;
   }

OFP::UnlimitedFormatItem::~UnlimitedFormatItem()
   {
      if (pFormatItems) delete pFormatItems;
   }

OFP::REditDesc::~REditDesc()
   {
      if (pIcon) delete pIcon;
   }

OFP::DataEditDesc::~DataEditDesc()
   {
      if (pScon) delete pScon;
      if (pVEditDescList) delete pVEditDescList;
      if (pIcon) delete pIcon;
   }

OFP::WEditDesc::~WEditDesc()
   {
      if (pIcon) delete pIcon;
   }

OFP::MEditDesc::~MEditDesc()
   {
      if (pIcon) delete pIcon;
   }

OFP::DEditDesc::~DEditDesc()
   {
      if (pIcon) delete pIcon;
   }

OFP::EEditDesc::~EEditDesc()
   {
      if (pIcon) delete pIcon;
   }

OFP::VEditDesc::~VEditDesc()
   {
      if (pIcon) delete pIcon;
   }

OFP::VEditDescList::~VEditDescList()
   {
      if (pVEditDescList) delete pVEditDescList;
   }

OFP::ControlEditDesc::~ControlEditDesc()
   {
      if (pDecimalEditDesc) delete pDecimalEditDesc;
      if (pRoundEditDesc) delete pRoundEditDesc;
      if (pBlankInterpEditDesc) delete pBlankInterpEditDesc;
      if (pSignedIntLiteralConstant) delete pSignedIntLiteralConstant;
      if (pSignEditDesc) delete pSignEditDesc;
      if (pIcon) delete pIcon;
      if (pPositionEditDesc) delete pPositionEditDesc;
   }

OFP::KEditDesc::~KEditDesc()
   {
      if (pSignedIntLiteralConstant) delete pSignedIntLiteralConstant;
   }

OFP::PositionEditDesc::~PositionEditDesc()
   {
      if (pIcon) delete pIcon;
   }

OFP::NEditDesc::~NEditDesc()
   {
      if (pIcon) delete pIcon;
   }

OFP::SignEditDesc::~SignEditDesc()
   {
   }

OFP::BlankInterpEditDesc::~BlankInterpEditDesc()
   {
   }

OFP::RoundEditDesc::~RoundEditDesc()
   {
   }

OFP::DecimalEditDesc::~DecimalEditDesc()
   {
   }

OFP::CharStringEditDesc::~CharStringEditDesc()
   {
      if (pScon) delete pScon;
   }

OFP::MainProgram::~MainProgram()
   {
      if (pProgramStmt) delete pProgramStmt;
      if (pInitialSpecPart) delete pInitialSpecPart;
      if (pSpecAndExecPart) delete pSpecAndExecPart;
      if (pInternalSubprogramPart) delete pInternalSubprogramPart;
      if (pEndProgramStmt) delete pEndProgramStmt;
   }

OFP::ProgramStmt::~ProgramStmt()
   {
      if (pLabel) delete pLabel;
      if (pProgramName) delete pProgramName;
      if (pEOS) delete pEOS;
   }

OFP::EndProgramStmt::~EndProgramStmt()
   {
      if (pLabel) delete pLabel;
      if (pProgramName) delete pProgramName;
      if (pEOS) delete pEOS;
   }

OFP::Module::~Module()
   {
      if (pModuleStmt) delete pModuleStmt;
      if (pSpecificationPart) delete pSpecificationPart;
      if (pModuleSubprogramPart) delete pModuleSubprogramPart;
      if (pEndModuleStmt) delete pEndModuleStmt;
   }

OFP::ModuleStmt::~ModuleStmt()
   {
      if (pLabel) delete pLabel;
      if (pModuleName) delete pModuleName;
      if (pEOS) delete pEOS;
   }

OFP::EndModuleStmt::~EndModuleStmt()
   {
      if (pLabel) delete pLabel;
      if (pModuleName) delete pModuleName;
      if (pEOS) delete pEOS;
   }

OFP::ModuleSubprogramPart::~ModuleSubprogramPart()
   {
      if (pContainsStmt) delete pContainsStmt;
      if (pModuleSubprogramList) delete pModuleSubprogramList;
   }

OFP::ModuleSubprogram::~ModuleSubprogram()
   {
      if (pSeparateModuleSubprogram) delete pSeparateModuleSubprogram;
      if (pSubroutineSubprogram) delete pSubroutineSubprogram;
      if (pFunctionSubprogram) delete pFunctionSubprogram;
   }

OFP::UseStmt::~UseStmt()
   {
      if (pLabel) delete pLabel;
      if (pModuleNature) delete pModuleNature;
      if (pModuleName) delete pModuleName;
      if (pOnlyList) delete pOnlyList;
      if (pEOS) delete pEOS;
      if (pRenameList) delete pRenameList;
   }

OFP::ModuleNature::~ModuleNature()
   {
   }

OFP::Rename::~Rename()
   {
      if (pLocalDefinedOperator) delete pLocalDefinedOperator;
      if (pUseDefinedOperator) delete pUseDefinedOperator;
      if (pLocalName) delete pLocalName;
      if (pUseName) delete pUseName;
   }

OFP::RenameList::~RenameList()
   {
      if (pRenameList) delete pRenameList;
   }

OFP::Only::~Only()
   {
      if (pRename) delete pRename;
      if (pOnlyUseName) delete pOnlyUseName;
   }

OFP::OnlyList::~OnlyList()
   {
      if (pOnlyList) delete pOnlyList;
   }

OFP::OnlyUseName::~OnlyUseName()
   {
      if (pIdent) delete pIdent;
   }

OFP::LocalDefinedOperator::~LocalDefinedOperator()
   {
      if (pDefinedBinaryOp) delete pDefinedBinaryOp;
      if (pDefinedUnaryOp) delete pDefinedUnaryOp;
   }

OFP::UseDefinedOperator::~UseDefinedOperator()
   {
      if (pDefinedBinaryOp) delete pDefinedBinaryOp;
      if (pDefinedUnaryOp) delete pDefinedUnaryOp;
   }

OFP::Submodule::~Submodule()
   {
      if (pSubmoduleStmt) delete pSubmoduleStmt;
      if (pSpecificationPart) delete pSpecificationPart;
      if (pModuleSubprogramPart) delete pModuleSubprogramPart;
      if (pEndSubmoduleStmt) delete pEndSubmoduleStmt;
   }

OFP::SubmoduleStmt::~SubmoduleStmt()
   {
      if (pLabel) delete pLabel;
      if (pParentIdentifier) delete pParentIdentifier;
      if (pSubmoduleName) delete pSubmoduleName;
      if (pEOS) delete pEOS;
   }

OFP::ParentIdentifier::~ParentIdentifier()
   {
      if (pAncestorModuleName) delete pAncestorModuleName;
      if (pParentSubmoduleName) delete pParentSubmoduleName;
   }

OFP::EndSubmoduleStmt::~EndSubmoduleStmt()
   {
      if (pLabel) delete pLabel;
      if (pSubmoduleName) delete pSubmoduleName;
      if (pEOS) delete pEOS;
   }

OFP::BlockData::~BlockData()
   {
      if (pBlockDataStmt) delete pBlockDataStmt;
      if (pSpecificationPart) delete pSpecificationPart;
      if (pEndBlockDataStmt) delete pEndBlockDataStmt;
   }

OFP::BlockDataStmt::~BlockDataStmt()
   {
      if (pLabel) delete pLabel;
      if (pBlockDataName) delete pBlockDataName;
      if (pEOS) delete pEOS;
   }

OFP::EndBlockDataStmt::~EndBlockDataStmt()
   {
      if (pLabel) delete pLabel;
      if (pEOS) delete pEOS;
      if (pBlockDataName) delete pBlockDataName;
   }

OFP::InterfaceBlock::~InterfaceBlock()
   {
      if (pInterfaceStmt) delete pInterfaceStmt;
      if (pInterfaceSpecificationList) delete pInterfaceSpecificationList;
      if (pEndInterfaceStmt) delete pEndInterfaceStmt;
   }

OFP::InterfaceSpecification::~InterfaceSpecification()
   {
      if (pProcedureStmt) delete pProcedureStmt;
      if (pInterfaceBody) delete pInterfaceBody;
   }

OFP::InterfaceStmt::~InterfaceStmt()
   {
      if (pLabel) delete pLabel;
      if (pEOS) delete pEOS;
      if (pGenericSpec) delete pGenericSpec;
   }

OFP::EndInterfaceStmt::~EndInterfaceStmt()
   {
      if (pLabel) delete pLabel;
      if (pGenericSpec) delete pGenericSpec;
      if (pEOS) delete pEOS;
   }

OFP::InterfaceBody::~InterfaceBody()
   {
      if (pSubroutineStmt) delete pSubroutineStmt;
      if (pSpecificationPart) delete pSpecificationPart;
      if (pEndSubroutineStmt) delete pEndSubroutineStmt;
      if (pFunctionStmt) delete pFunctionStmt;
      if (pEndFunctionStmt) delete pEndFunctionStmt;
   }

OFP::ProcedureStmt::~ProcedureStmt()
   {
      if (pLabel) delete pLabel;
      if (pProcedureNameList) delete pProcedureNameList;
      if (pEOS) delete pEOS;
   }

OFP::ProcedureNameList::~ProcedureNameList()
   {
      if (pProcedureNameList) delete pProcedureNameList;
   }

OFP::GenericSpec::~GenericSpec()
   {
      if (pDefinedIoGenericSpec) delete pDefinedIoGenericSpec;
      if (pDefinedOperator) delete pDefinedOperator;
      if (pGenericName) delete pGenericName;
   }

OFP::DefinedIoGenericSpec::~DefinedIoGenericSpec()
   {
   }

OFP::ImportStmt::~ImportStmt()
   {
      if (pLabel) delete pLabel;
      if (pEOS) delete pEOS;
      if (pImportNameList) delete pImportNameList;
   }

OFP::ImportNameList::~ImportNameList()
   {
      if (pImportNameList) delete pImportNameList;
   }

OFP::ExternalStmt::~ExternalStmt()
   {
      if (pLabel) delete pLabel;
      if (pExternalNameList) delete pExternalNameList;
      if (pEOS) delete pEOS;
   }

OFP::ProcedureDeclarationStmt::~ProcedureDeclarationStmt()
   {
      if (pLabel) delete pLabel;
      if (pProcInterface) delete pProcInterface;
      if (pProcAttrSpecList) delete pProcAttrSpecList;
      if (pProcDeclList) delete pProcDeclList;
      if (pEOS) delete pEOS;
   }

OFP::ProcInterface::~ProcInterface()
   {
      if (pDeclarationTypeSpec) delete pDeclarationTypeSpec;
      if (pInterfaceName) delete pInterfaceName;
   }

OFP::ProcAttrSpec::~ProcAttrSpec()
   {
      if (pIntentSpec) delete pIntentSpec;
      if (pProcLanguageBindingSpec) delete pProcLanguageBindingSpec;
      if (pAccessSpec) delete pAccessSpec;
   }

OFP::ProcAttrSpecList::~ProcAttrSpecList()
   {
      if (pProcAttrSpecList) delete pProcAttrSpecList;
   }

OFP::ProcDecl::~ProcDecl()
   {
      if (pProcedureEntityName) delete pProcedureEntityName;
      if (pProcPointerInit) delete pProcPointerInit;
   }

OFP::ProcDeclList::~ProcDeclList()
   {
      if (pProcDeclList) delete pProcDeclList;
   }

OFP::InterfaceName::~InterfaceName()
   {
      if (pIdent) delete pIdent;
   }

OFP::ProcPointerInit::~ProcPointerInit()
   {
      if (pInitialProcTarget) delete pInitialProcTarget;
      if (pNullInit) delete pNullInit;
   }

OFP::InitialProcTarget::~InitialProcTarget()
   {
      if (pProcedureName) delete pProcedureName;
   }

OFP::IntrinsicStmt::~IntrinsicStmt()
   {
      if (pLabel) delete pLabel;
      if (pIntrinsicProcedureNameList) delete pIntrinsicProcedureNameList;
      if (pEOS) delete pEOS;
   }

OFP::IntrinsicProcedureNameList::~IntrinsicProcedureNameList()
   {
      if (pIntrinsicProcedureNameList) delete pIntrinsicProcedureNameList;
   }

OFP::FunctionReference::~FunctionReference()
   {
      if (pProcedureDesignator) delete pProcedureDesignator;
      if (pActualArgSpecList) delete pActualArgSpecList;
   }

OFP::CallStmt::~CallStmt()
   {
      if (pLabel) delete pLabel;
      if (pProcedureDesignator) delete pProcedureDesignator;
      if (pActualArgSpecList) delete pActualArgSpecList;
      if (pEOS) delete pEOS;
   }

OFP::ProcedureDesignator::~ProcedureDesignator()
   {
      if (pDataRef) delete pDataRef;
      if (pBindingName) delete pBindingName;
      if (pProcedureName) delete pProcedureName;
   }

OFP::ActualArgSpec::~ActualArgSpec()
   {
      if (pKeyword) delete pKeyword;
      if (pActualArg) delete pActualArg;
   }

OFP::ActualArgSpecList::~ActualArgSpecList()
   {
      if (pActualArgSpecList) delete pActualArgSpecList;
   }

OFP::ActualArg::~ActualArg()
   {
      if (pAltReturnSpec) delete pAltReturnSpec;
      if (pExpr) delete pExpr;
   }

OFP::AltReturnSpec::~AltReturnSpec()
   {
      if (pLabel) delete pLabel;
   }

OFP::Prefix::~Prefix()
   {
      if (pPrefixSpecList) delete pPrefixSpecList;
   }

OFP::PrefixSpec::~PrefixSpec()
   {
      if (pDeclarationTypeSpec) delete pDeclarationTypeSpec;
   }

OFP::FunctionSubprogram::~FunctionSubprogram()
   {
      if (pFunctionStmt) delete pFunctionStmt;
      if (pInitialSpecPart) delete pInitialSpecPart;
      if (pSpecAndExecPart) delete pSpecAndExecPart;
      if (pInternalSubprogramPart) delete pInternalSubprogramPart;
      if (pEndFunctionStmt) delete pEndFunctionStmt;
   }

OFP::FunctionStmt::~FunctionStmt()
   {
      if (pLabel) delete pLabel;
      if (pPrefix) delete pPrefix;
      if (pFunctionName) delete pFunctionName;
      if (pDummyArgNameList) delete pDummyArgNameList;
      if (pSuffix) delete pSuffix;
      if (pEOS) delete pEOS;
   }

OFP::ProcLanguageBindingSpec::~ProcLanguageBindingSpec()
   {
      if (pLanguageBindingSpec) delete pLanguageBindingSpec;
   }

OFP::DummyArgName::~DummyArgName()
   {
      if (pIdent) delete pIdent;
   }

OFP::Suffix::~Suffix()
   {
      if (pResultName) delete pResultName;
      if (pProcLanguageBindingSpec) delete pProcLanguageBindingSpec;
   }

OFP::EndFunctionStmt::~EndFunctionStmt()
   {
      if (pLabel) delete pLabel;
      if (pFunctionName) delete pFunctionName;
      if (pEOS) delete pEOS;
   }

OFP::SubroutineSubprogram::~SubroutineSubprogram()
   {
      if (pSubroutineStmt) delete pSubroutineStmt;
      if (pInitialSpecPart) delete pInitialSpecPart;
      if (pSpecAndExecPart) delete pSpecAndExecPart;
      if (pInternalSubprogramPart) delete pInternalSubprogramPart;
      if (pEndSubroutineStmt) delete pEndSubroutineStmt;
   }

OFP::SubroutineStmt::~SubroutineStmt()
   {
      if (pLabel) delete pLabel;
      if (pPrefix) delete pPrefix;
      if (pSubroutineName) delete pSubroutineName;
      if (pEOS) delete pEOS;
      if (pDummyArgList) delete pDummyArgList;
      if (pProcLanguageBindingSpec) delete pProcLanguageBindingSpec;
   }

OFP::DummyArg::~DummyArg()
   {
      if (pDummyArgName) delete pDummyArgName;
   }

OFP::DummyArgList::~DummyArgList()
   {
      if (pDummyArgList) delete pDummyArgList;
   }

OFP::EndSubroutineStmt::~EndSubroutineStmt()
   {
      if (pLabel) delete pLabel;
      if (pSubroutineName) delete pSubroutineName;
      if (pEOS) delete pEOS;
   }

OFP::SeparateModuleSubprogram::~SeparateModuleSubprogram()
   {
      if (pMpSubprogramStmt) delete pMpSubprogramStmt;
      if (pInitialSpecPart) delete pInitialSpecPart;
      if (pSpecAndExecPart) delete pSpecAndExecPart;
      if (pInternalSubprogramPart) delete pInternalSubprogramPart;
      if (pEndMpSubprogramStmt) delete pEndMpSubprogramStmt;
   }

OFP::MpSubprogramStmt::~MpSubprogramStmt()
   {
      if (pLabel) delete pLabel;
      if (pProcedureName) delete pProcedureName;
      if (pEOS) delete pEOS;
   }

OFP::EndMpSubprogramStmt::~EndMpSubprogramStmt()
   {
      if (pLabel) delete pLabel;
      if (pProcedureName) delete pProcedureName;
      if (pEOS) delete pEOS;
   }

OFP::EntryStmt::~EntryStmt()
   {
      if (pLabel) delete pLabel;
      if (pEntryName) delete pEntryName;
      if (pEOS) delete pEOS;
      if (pDummyArgList) delete pDummyArgList;
      if (pSuffix) delete pSuffix;
   }

OFP::ReturnStmt::~ReturnStmt()
   {
      if (pLabel) delete pLabel;
      if (pExpr) delete pExpr;
      if (pEOS) delete pEOS;
   }

OFP::ContainsStmt::~ContainsStmt()
   {
      if (pLabel) delete pLabel;
      if (pEOS) delete pEOS;
   }

OFP::StmtFunctionStmt::~StmtFunctionStmt()
   {
      if (pLabel) delete pLabel;
      if (pFunctionName) delete pFunctionName;
      if (pDummyArgNameList) delete pDummyArgNameList;
      if (pExpr) delete pExpr;
      if (pEOS) delete pEOS;
   }

OFP::Name::~Name()
   {
      if (pIdent) delete pIdent;
   }

#ifdef OBSOLETE
OFP::AncestorModuleName::~AncestorModuleName()
   {
      if (pIdent) delete pIdent;
   }

OFP::ArgName::~ArgName()
   {
      if (pIdent) delete pIdent;
   }

OFP::ArrayName::~ArrayName()
   {
      if (pIdent) delete pIdent;
   }

OFP::AssociateConstructName::~AssociateConstructName()
   {
      if (pIdent) delete pIdent;
   }

OFP::AssociateName::~AssociateName()
   {
      if (pIdent) delete pIdent;
   }

OFP::BindingName::~BindingName()
   {
      if (pIdent) delete pIdent;
   }

OFP::BlockConstructName::~BlockConstructName()
   {
      if (pIdent) delete pIdent;
   }

OFP::BlockDataName::~BlockDataName()
   {
      if (pIdent) delete pIdent;
   }

OFP::CaseConstructName::~CaseConstructName()
   {
      if (pIdent) delete pIdent;
   }

OFP::CoarrayName::~CoarrayName()
   {
      if (pIdent) delete pIdent;
   }

OFP::CommonBlockName::~CommonBlockName()
   {
      if (pIdent) delete pIdent;
   }

OFP::ComponentName::~ComponentName()
   {
      if (pIdent) delete pIdent;
   }

OFP::ConstructName::~ConstructName()
   {
      if (pIdent) delete pIdent;
   }

OFP::CriticalConstructName::~CriticalConstructName()
   {
      if (pIdent) delete pIdent;
   }

OFP::DataPointerComponentName::~DataPointerComponentName()
   {
      if (pIdent) delete pIdent;
   }

OFP::DoConstructName::~DoConstructName()
   {
      if (pIdent) delete pIdent;
   }

OFP::EntityName::~EntityName()
   {
      if (pIdent) delete pIdent;
   }

OFP::EntryName::~EntryName()
   {
      if (pIdent) delete pIdent;
   }

OFP::ExternalName::~ExternalName()
   {
      if (pIdent) delete pIdent;
   }

OFP::FinalSubroutineName::~FinalSubroutineName()
   {
      if (pIdent) delete pIdent;
   }

OFP::ForallConstructName::~ForallConstructName()
   {
      if (pIdent) delete pIdent;
   }

OFP::FunctionName::~FunctionName()
   {
      if (pIdent) delete pIdent;
   }

OFP::GenericName::~GenericName()
   {
      if (pIdent) delete pIdent;
   }

OFP::IfConstructName::~IfConstructName()
   {
      if (pIdent) delete pIdent;
   }

OFP::ImportName::~ImportName()
   {
      if (pIdent) delete pIdent;
   }

OFP::IndexName::~IndexName()
   {
      if (pIdent) delete pIdent;
   }

OFP::IntrinsicProcedureName::~IntrinsicProcedureName()
   {
      if (pIdent) delete pIdent;
   }

OFP::LocalName::~LocalName()
   {
      if (pIdent) delete pIdent;
   }

OFP::ModuleName::~ModuleName()
   {
      if (pIdent) delete pIdent;
   }

OFP::NamelistGroupName::~NamelistGroupName()
   {
      if (pIdent) delete pIdent;
   }

OFP::ObjectName::~ObjectName()
   {
      if (pIdent) delete pIdent;
   }

OFP::ParentSubmoduleName::~ParentSubmoduleName()
   {
      if (pIdent) delete pIdent;
   }

OFP::ParentTypeName::~ParentTypeName()
   {
      if (pIdent) delete pIdent;
   }

OFP::ProcedureComponentName::~ProcedureComponentName()
   {
      if (pIdent) delete pIdent;
   }

OFP::ProcedureEntityName::~ProcedureEntityName()
   {
      if (pIdent) delete pIdent;
   }

OFP::ProcedureName::~ProcedureName()
   {
      if (pIdent) delete pIdent;
   }

OFP::ProcEntityName::~ProcEntityName()
   {
      if (pIdent) delete pIdent;
   }

OFP::ResultName::~ResultName()
   {
      if (pIdent) delete pIdent;
   }

OFP::ScalarIntConstantName::~ScalarIntConstantName()
   {
      if (pIdent) delete pIdent;
   }

OFP::ScalarIntVariableName::~ScalarIntVariableName()
   {
      if (pIdent) delete pIdent;
   }

OFP::ScalarVariableName::~ScalarVariableName()
   {
      if (pIdent) delete pIdent;
   }

OFP::SelectConstructName::~SelectConstructName()
   {
      if (pIdent) delete pIdent;
   }

OFP::SubmoduleName::~SubmoduleName()
   {
      if (pIdent) delete pIdent;
   }

OFP::SubroutineName::~SubroutineName()
   {
      if (pIdent) delete pIdent;
   }

OFP::TypeName::~TypeName()
   {
      if (pIdent) delete pIdent;
   }

OFP::TypeParamName::~TypeParamName()
   {
      if (pIdent) delete pIdent;
   }

OFP::UseName::~UseName()
   {
      if (pIdent) delete pIdent;
   }
#endif

OFP::ExternalNameList::~ExternalNameList()
   {
      if (pExternalNameList) delete pExternalNameList;
   }

OFP::LabelList::~LabelList()
   {
      if (pLblRefList) delete pLblRefList;
   }

