#ifndef	TRAVERSE_HPP
#define TRAVERSE_HPP

namespace OFP {

   class Program;
   class ProgramUnit;
   class ExternalSubprogram;
   class InitialSpecPart;
   class SpecificationPart;
   class ImplicitPart;
   class ImplicitPartStmt;
   class DeclarationConstruct;
   class SpecAndExecPart;
   class ExecutionPart;
   class ExecutionPartConstruct;
   class InternalSubprogramPart;
   class InternalSubprogram;
   class OtherSpecificationStmt;
   class ExecutableConstruct;
   class ActionStmt;
   class Keyword;
   class Constant;
   class LiteralConstant;
   class NamedConstant;
   class TypeParamValue;
   class TypeSpec;
   class DeclarationTypeSpec;
   class IntrinsicTypeSpec;
   class KindSelector;
   class SignedIntLiteralConstant;
   class IntLiteralConstant;
   class KindParam;
   class DigitString;
   class Sign;
   class SignedRealLiteralConstant;
   class RealLiteralConstant;
   class ComplexLiteralConstant;
   class RealPart;
   class ImagPart;
   class CharSelector;
   class LengthSelector;
   class CharLength;
   class CharLiteralConstant;
   class LogicalLiteralConstant;
   class DerivedTypeDef;
   class DerivedTypeStmt;
   class OptTypeAttrSpecList;
   class OptTypeParamNameList;
   class TypeAttrSpec;
   class TypeAttrSpecList;
   class TypeParamNameList;
   class PrivateOrSequence;
   class EndTypeStmt;
   class SequenceStmt;
   class TypeParamDefStmt;
   class TypeParamDecl;
   class TypeParamDeclList;
   class TypeParamAttrSpec;
   class ComponentPart;
   class ComponentDefStmt;
   class DataComponentDefStmt;
   class OptComponentAttrSpecList;
   class ComponentAttrSpec;
   class ComponentAttrSpecList;
   class ComponentDecl;
   class ComponentDeclList;
   class ComponentArraySpec;
   class ProcComponentDefStmt;
   class ProcComponentAttrSpec;
   class ProcComponentAttrSpecList;
   class ComponentInitialization;
   class InitialDataTarget;
   class PrivateComponentsStmt;
   class TypeBoundProcedurePart;
   class BindingPrivateStmt;
   class TypeBoundProcBinding;
   class TypeBoundProcedureStmt;
   class BindingNameList;
   class BindingAttrList;
   class TypeBoundProcDecl;
   class TypeBoundProcDeclList;
   class TypeBoundGenericStmt;
   class BindingAttr;
   class FinalProcedureStmt;
   class FinalSubroutineNameList;
   class DerivedTypeSpec;
   class TypeParamSpec;
   class TypeParamSpecList;
   class StructureConstructor;
   class ComponentSpec;
   class ComponentDataSource;
   class EnumDef;
   class EnumDefStmt;
   class EnumeratorDefStmt;
   class Enumerator;
   class EnumeratorList;
   class EndEnumStmt;
   class BozLiteralConstant;
   class ArrayConstructor;
   class AcSpec;
   class AcValue;
   class AcValueList;
   class AcImpliedDo;
   class AcImpliedDoControl;
   class AcDoVariable;
   class TypeDeclarationStmt;
   class OptAttrSpecList;
   class AttrSpec;
   class AttrSpecList;
   class EntityDecl;
   class EntityDeclList;
   class Initialization;
   class NullInit;
   class AccessSpec;
   class LanguageBindingSpec;
   class CoarraySpec;
   class DeferredCoshapeSpec;
   class DeferredCoshapeSpecList;
   class ExplicitCoshapeSpec;
   class CoboundsEntry;
   class LowerCobound;
   class UpperCobound;
   class ArraySpec;
   class ExplicitShapeSpec;
   class ExplicitShapeSpecList;
   class LowerBound;
   class UpperBound;
   class AssumedShapeSpec;
   class AssumedShapeSpecList;
   class DeferredShapeSpec;
   class DeferredShapeSpecList;
   class AssumedSizeSpec;
   class AssumedRankSpec;
   class ImpliedShapeSpec;
   class ImpliedShapeSpecList;
   class IntentSpec;
   class AccessStmt;
   class AccessId;
   class AccessIdList;
   class AllocatableStmt;
   class AllocatableDecl;
   class AllocatableDeclList;
   class AsynchronousStmt;
   class BindStmt;
   class BindEntity;
   class BindEntityList;
   class CodimensionStmt;
   class CodimensionDecl;
   class CodimensionDeclList;
   class ContiguousStmt;
   class ObjectNameList;
   class DataStmt;
   class DataStmtSet;
   class DataStmtSetList;
   class DataStmtObject;
   class DataStmtObjectList;
   class DataImpliedDo;
   class DataIDoObject;
   class DataIDoObjectList;
   class DataIDoVariable;
   class DataStmtValue;
   class DataStmtValueList;
   class DataStmtRepeat;
   class DataStmtConstant;
   class IntConstantSubobject;
   class ConstantSubobject;
   class DimensionStmt;
   class ArrayNameSpec;
   class ArrayNameSpecList;
   class IntentStmt;
   class DummyArgNameList;
   class OptionalStmt;
   class ParameterStmt;
   class NamedConstantDef;
   class NamedConstantDefList;
   class PointerStmt;
   class PointerDecl;
   class PointerDeclList;
   class ProtectedStmt;
   class EntityNameList;
   class SaveStmt;
   class SavedEntity;
   class SavedEntityList;
   class ProcPointerName;
   class TargetStmt;
   class TargetDecl;
   class TargetDeclList;
   class ValueStmt;
   class VolatileStmt;
   class ImplicitStmt;
   class ImplicitSpec;
   class ImplicitSpecList;
   class LetterSpec;
   class LetterSpecList;
   class NamelistStmt;
   class NamelistEntry;
   class NamelistEntryList;
   class NamelistGroupObject;
   class NamelistGroupObjectList;
   class EquivalenceStmt;
   class EquivalenceSet;
   class EquivalenceSetList;
   class EquivalenceObject;
   class EquivalenceObjectList;
   class CommonStmt;
   class CommonBlockEntry1;
   class CommonBlockEntry;
   class CommonBlockObject;
   class CommonBlockObjectList;
   class Designator;
   class Variable;
   class VariableName;
   class LogicalVariable;
   class CharVariable;
   class DefaultCharVariable;
   class IntVariable;
   class Substring;
   class ParentString;
   class SubstringRange;
   class DataRef;
   class PartRef;
   class StructureComponent;
   class CoindexedNamedObject;
   class TypeParamInquiry;
   class ArrayElement;
   class Subscript;
   class SectionSubscript;
   class SectionSubscriptList;
   class SubscriptTriplet;
   class Stride;
   class ImageSelector;
   class Cosubscript;
   class CosubscriptList;
   class AllocateStmt;
   class AllocOpt;
   class AllocOptList;
   class StatVariable;
   class ErrmsgVariable;
   class Allocation;
   class AllocationList;
   class AllocateObject;
   class AllocateObjectList;
   class AllocateShapeSpec;
   class AllocateShapeSpecList;
   class LowerBoundExpr;
   class UpperBoundExpr;
   class AllocateCoarraySpec;
   class AllocateCoshapeSpec;
   class AllocateCoshapeSpecList;
   class NullifyStmt;
   class PointerObject;
   class PointerObjectList;
   class DeallocateStmt;
   class DeallocOpt;
   class DeallocOptList;
   class IntrinsicOperator;
   class DefinedOperator;
   class ExtendedIntrinsicOp;
   class Primary;
   class DefinedUnaryOp;
   class Expr;
   class DefinedBinaryOp;
   class LogicalExpr;
   class DefaultCharExpr;
   class IntExpr;
   class SpecificationExpr;
   class ConstantExpr;
   class IntConstantExpr;
   class AssignmentStmt;
   class PointerAssignmentStmt;
   class DataPointerObject;
   class BoundsSpec;
   class BoundsSpecList;
   class BoundsRemapping;
   class BoundsRemappingList;
   class DataTarget;
   class ProcPointerObject;
   class ProcComponentRef;
   class ProcTarget;
   class WhereStmt;
   class WhereConstruct;
   class MaskedElsewhereClause;
   class ElsewhereClause;
   class WhereConstructStmt;
   class WhereBodyConstruct;
   class WhereAssignmentStmt;
   class MaskExpr;
   class MaskedElsewhereStmt;
   class ElsewhereStmt;
   class EndWhereStmt;
   class ForallConstruct;
   class ForallConstructStmt;
   class ForallHeader;
   class ForallTripletSpec;
   class ForallTripletSpecList;
   class ForallLimit;
   class ForallStep;
   class ForallBodyConstruct;
   class ForallAssignmentStmt;
   class EndForallStmt;
   class ForallStmt;
   class Block;
   class AssociateConstruct;
   class AssociateStmt;
   class Association;
   class AssociationList;
   class Selector;
   class EndAssociateStmt;
   class BlockConstruct;
   class BlockStmt;
   class EndBlockStmt;
   class CriticalConstruct;
   class CriticalStmt;
   class EndCriticalStmt;
   class LabelDoStmt;
   class NonlabelDoStmt;
   class LoopControl;
   class DoVariable;
   class EndDoStmt;
   class CycleStmt;
   class IfConstruct;
   class ElseIfStmtAndBlock;
   class ElseStmtAndBlock;
   class IfThenStmt;
   class ElseIfStmt;
   class ElseStmt;
   class EndIfStmt;
   class IfStmt;
   class CaseConstruct;
   class CaseStmtAndBlock;
   class SelectCaseStmt;
   class CaseStmt;
   class EndSelectStmt;
   class CaseExpr;
   class CaseSelector;
   class CaseValueRange;
   class CaseValueRangeList;
   class CaseValue;
   class SelectTypeConstruct;
   class TypeGuardStmtAndBlock;
   class SelectTypeStmt;
   class TypeGuardStmt;
   class EndSelectTypeStmt;
   class ExitStmt;
   class GotoStmt;
   class ComputedGotoStmt;
   class ArithmeticIfStmt;
   class ContinueStmt;
   class StopStmt;
   class ErrorStopStmt;
   class StopCode;
   class SyncAllStmt;
   class SyncStat;
   class SyncStatList;
   class SyncImagesStmt;
   class ImageSet;
   class SyncMemoryStmt;
   class LockStmt;
   class LockStat;
   class LockStatList;
   class UnlockStmt;
   class LockVariable;
   class PauseStmt;
   class IoUnit;
   class FileUnitNumber;
   class OpenStmt;
   class ConnectSpec;
   class ConnectSpecList;
   class FileNameExpr;
   class IomsgVariable;
   class CloseStmt;
   class CloseSpec;
   class CloseSpecList;
   class ReadStmt;
   class WriteStmt;
   class PrintStmt;
   class IoControlSpec;
   class IoControlSpecList;
   class IdVariable;
   class Format;
   class InputItem;
   class InputItemList;
   class OutputItem;
   class OutputItemList;
   class IoImpliedDo;
   class IoImpliedDoObject;
   class IoImpliedDoObjectList;
   class IoImpliedDoControl;
   class WaitStmt;
   class WaitSpec;
   class WaitSpecList;
   class BackspaceStmt;
   class EndfileStmt;
   class RewindStmt;
   class PositionSpec;
   class PositionSpecList;
   class FlushStmt;
   class FlushSpec;
   class FlushSpecList;
   class InquireStmt;
   class InquireSpec;
   class InquireSpecList;
   class FormatStmt;
   class FormatSpecification;
   class FormatItems;
   class FormatItem;
   class UnlimitedFormatItem;
   class REditDesc;
   class DataEditDesc;
   class WEditDesc;
   class MEditDesc;
   class DEditDesc;
   class EEditDesc;
   class VEditDesc;
   class VEditDescList;
   class ControlEditDesc;
   class KEditDesc;
   class PositionEditDesc;
   class NEditDesc;
   class SignEditDesc;
   class BlankInterpEditDesc;
   class RoundEditDesc;
   class DecimalEditDesc;
   class CharStringEditDesc;
   class MainProgram;
   class ProgramStmt;
   class EndProgramStmt;
   class Module;
   class ModuleStmt;
   class EndModuleStmt;
   class ModuleSubprogramPart;
   class ModuleSubprogram;
   class UseStmt;
   class ModuleNature;
   class Rename;
   class RenameList;
   class Only;
   class OnlyList;
   class OnlyUseName;
   class LocalDefinedOperator;
   class UseDefinedOperator;
   class Submodule;
   class SubmoduleStmt;
   class ParentIdentifier;
   class EndSubmoduleStmt;
   class BlockData;
   class BlockDataStmt;
   class EndBlockDataStmt;
   class InterfaceBlock;
   class InterfaceSpecification;
   class InterfaceStmt;
   class EndInterfaceStmt;
   class InterfaceBody;
   class ProcedureStmt;
   class ProcedureNameList;
   class GenericSpec;
   class DefinedIoGenericSpec;
   class ImportStmt;
   class ImportNameList;
   class ExternalStmt;
   class ProcedureDeclarationStmt;
   class ProcInterface;
   class ProcAttrSpec;
   class ProcAttrSpecList;
   class ProcDecl;
   class ProcDeclList;
   class InterfaceName;
   class ProcPointerInit;
   class InitialProcTarget;
   class IntrinsicStmt;
   class IntrinsicProcedureNameList;
   class FunctionReference;
   class CallStmt;
   class ProcedureDesignator;
   class ActualArgSpec;
   class ActualArgSpecList;
   class ActualArg;
   class AltReturnSpec;
   class Prefix;
   class PrefixSpec;
   class FunctionSubprogram;
   class FunctionStmt;
   class ProcLanguageBindingSpec;
   class DummyArgName;
   class Suffix;
   class EndFunctionStmt;
   class SubroutineSubprogram;
   class SubroutineStmt;
   class DummyArg;
   class DummyArgList;
   class EndSubroutineStmt;
   class SeparateModuleSubprogram;
   class MpSubprogramStmt;
   class EndMpSubprogramStmt;
   class EntryStmt;
   class ReturnStmt;
   class ContainsStmt;
   class StmtFunctionStmt;
   class Name;
#ifdef OBSOLETE
   class AncestorModuleName;
   class ArgName;
   class ArrayName;
   class AssociateConstructName;
   class AssociateName;
   class BindingName;
   class BlockConstructName;
   class BlockDataName;
   class CaseConstructName;
   class CoarrayName;
   class CommonBlockName;
   class ComponentName;
   class ConstructName;
   class CriticalConstructName;
   class DataPointerComponentName;
   class DoConstructName;
   class EntityName;
   class EntryName;
   class ExternalName;
   class FinalSubroutineName;
   class ForallConstructName;
   class FunctionName;
   class GenericName;
   class IfConstructName;
   class ImportName;
   class IndexName;
   class IntrinsicProcedureName;
   class LocalName;
   class ModuleName;
   class NamelistGroupName;
   class ObjectName;
   class ParentSubmoduleName;
   class ParentTypeName;
   class ProcedureComponentName;
   class ProcedureEntityName;
   class ProcedureName;
   class ProcEntityName;
   class ProgramName;
   class ResultName;
   class ScalarIntConstantName;
   class ScalarIntVariableName;
   class ScalarVariableName;
   class SelectConstructName;
   class SubmoduleName;
   class SubroutineName;
   class TypeName;
   class TypeParamName;
   class UseName;
#endif
   class ExternalNameList;
   class LabelList;

class Program : public Node
{
 public:
    Program()
      {
         pStartCommentBlock = NULL;
         pProgramUnitList = new std::vector<ProgramUnit*>();
      }
   virtual ~Program();

    Program* newProgram()
      {
         Program* node = new Program();
         node->pStartCommentBlock = pStartCommentBlock;  pStartCommentBlock = NULL;
         delete node->pProgramUnitList; node->pProgramUnitList = pProgramUnitList;  pProgramUnitList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    StartCommentBlock* getStartCommentBlock() {return pStartCommentBlock;}
    std::vector<ProgramUnit*>* getProgramUnitList() {return pProgramUnitList;}

    void setStartCommentBlock(StartCommentBlock* startcommentblock) {pStartCommentBlock = startcommentblock;}
    void appendProgramUnit(ProgramUnit* programunit) {pProgramUnitList->push_back(programunit);}

 private:
    StartCommentBlock* pStartCommentBlock;
    std::vector<ProgramUnit*>* pProgramUnitList;
};

class ProgramUnit : public Node
{
 public:
    ProgramUnit()
      {
         pBlockData = NULL;
         pSubmodule = NULL;
         pModule = NULL;
         pExternalSubprogram = NULL;
         pMainProgram = NULL;
      }
   virtual ~ProgramUnit();

   enum OptionType
     {
        DEFAULT = 0,
        ProgramUnit_BD,
        ProgramUnit_S,
        ProgramUnit_M,
        ProgramUnit_ES,
        ProgramUnit_MP
     };

    ProgramUnit* newProgramUnit()
      {
         ProgramUnit* node = new ProgramUnit();
         node->pBlockData = pBlockData;  pBlockData = NULL;
         node->pSubmodule = pSubmodule;  pSubmodule = NULL;
         node->pModule = pModule;  pModule = NULL;
         node->pExternalSubprogram = pExternalSubprogram;  pExternalSubprogram = NULL;
         node->pMainProgram = pMainProgram;  pMainProgram = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    BlockData* getBlockData() {return pBlockData;}
    Submodule* getSubmodule() {return pSubmodule;}
    Module* getModule() {return pModule;}
    ExternalSubprogram* getExternalSubprogram() {return pExternalSubprogram;}
    MainProgram* getMainProgram() {return pMainProgram;}

    void setBlockData(BlockData* blockdata) {pBlockData = blockdata;}
    void setSubmodule(Submodule* submodule) {pSubmodule = submodule;}
    void setModule(Module* module) {pModule = module;}
    void setExternalSubprogram(ExternalSubprogram* externalsubprogram) {pExternalSubprogram = externalsubprogram;}
    void setMainProgram(MainProgram* mainprogram) {pMainProgram = mainprogram;}

 private:
    BlockData* pBlockData;
    Submodule* pSubmodule;
    Module* pModule;
    ExternalSubprogram* pExternalSubprogram;
    MainProgram* pMainProgram;
};

class ExternalSubprogram : public Node
{
 public:
    ExternalSubprogram()
      {
         pSubroutineSubprogram = NULL;
         pFunctionSubprogram = NULL;
      }
   virtual ~ExternalSubprogram();

   enum OptionType
     {
        DEFAULT = 0,
        ExternalSubprogram_SS,
        ExternalSubprogram_FS
     };

    ExternalSubprogram* newExternalSubprogram()
      {
         ExternalSubprogram* node = new ExternalSubprogram();
         node->pSubroutineSubprogram = pSubroutineSubprogram;  pSubroutineSubprogram = NULL;
         node->pFunctionSubprogram = pFunctionSubprogram;  pFunctionSubprogram = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    SubroutineSubprogram* getSubroutineSubprogram() {return pSubroutineSubprogram;}
    FunctionSubprogram* getFunctionSubprogram() {return pFunctionSubprogram;}

    void setSubroutineSubprogram(SubroutineSubprogram* subroutinesubprogram) {pSubroutineSubprogram = subroutinesubprogram;}
    void setFunctionSubprogram(FunctionSubprogram* functionsubprogram) {pFunctionSubprogram = functionsubprogram;}

 private:
    SubroutineSubprogram* pSubroutineSubprogram;
    FunctionSubprogram* pFunctionSubprogram;
};

class InitialSpecPart : public Node
{
 public:
    InitialSpecPart()
      {
         pStatementList = new std::vector<Statement*>();
      }
   virtual ~InitialSpecPart();

    InitialSpecPart* newInitialSpecPart()
      {
         InitialSpecPart* node = new InitialSpecPart();
         delete node->pStatementList; node->pStatementList = pStatementList;  pStatementList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<Statement*>* getStatementList() {return pStatementList;}
    void appendStatement(Statement* stmt) {pStatementList->push_back(stmt);}

 private:
    std::vector<Statement*>* pStatementList;
};

class SpecificationPart : public Node
{
 public:
    SpecificationPart()
      {
         pUseStmtList = new std::vector<UseStmt*>();
         pImportStmtList = new std::vector<ImportStmt*>();
         pImplicitPart = NULL;
         pDeclarationConstructList = new std::vector<DeclarationConstruct*>();
      }
   virtual ~SpecificationPart();

    SpecificationPart* newSpecificationPart()
      {
         SpecificationPart* node = new SpecificationPart();
         delete node->pUseStmtList; node->pUseStmtList = pUseStmtList;  pUseStmtList = NULL;
         delete node->pImportStmtList; node->pImportStmtList = pImportStmtList;  pImportStmtList = NULL;
         node->pImplicitPart = pImplicitPart;  pImplicitPart = NULL;
         delete node->pDeclarationConstructList; node->pDeclarationConstructList = pDeclarationConstructList;  pDeclarationConstructList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<UseStmt*>* getUseStmtList() {return pUseStmtList;}
    std::vector<ImportStmt*>* getImportStmtList() {return pImportStmtList;}
    ImplicitPart* getImplicitPart() {return pImplicitPart;}
    std::vector<DeclarationConstruct*>* getDeclarationConstructList() {return pDeclarationConstructList;}

    void appendUseStmt(UseStmt* usestmt) {pUseStmtList->push_back(usestmt);}
    void appendImportStmt(ImportStmt* importstmt) {pImportStmtList->push_back(importstmt);}
    void setImplicitPart(ImplicitPart* implicitpart) {pImplicitPart = implicitpart;}
    void appendDeclarationConstruct(DeclarationConstruct* declarationconstruct) {pDeclarationConstructList->push_back(declarationconstruct);}

 private:
    std::vector<UseStmt*>* pUseStmtList;
    std::vector<ImportStmt*>* pImportStmtList;
    ImplicitPart* pImplicitPart;
    std::vector<DeclarationConstruct*>* pDeclarationConstructList;
};

class ImplicitPart : public Node
{
 public:
    ImplicitPart()
      {
         pImplicitPartStmtList = new std::vector<ImplicitPartStmt*>();
         pImplicitStmt = NULL;
      }
   virtual ~ImplicitPart();

    ImplicitPart* newImplicitPart()
      {
         ImplicitPart* node = new ImplicitPart();
         delete node->pImplicitPartStmtList; node->pImplicitPartStmtList = pImplicitPartStmtList;  pImplicitPartStmtList = NULL;
         node->pImplicitStmt = pImplicitStmt;  pImplicitStmt = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<ImplicitPartStmt*>* getImplicitPartStmtList() {return pImplicitPartStmtList;}
    ImplicitStmt* getImplicitStmt() {return pImplicitStmt;}

    void appendImplicitPartStmt(ImplicitPartStmt* implicitpartstmt) {pImplicitPartStmtList->push_back(implicitpartstmt);}
    void setImplicitStmt(ImplicitStmt* implicitstmt) {pImplicitStmt = implicitstmt;}

 private:
    std::vector<ImplicitPartStmt*>* pImplicitPartStmtList;
    ImplicitStmt* pImplicitStmt;
};

class ImplicitPartStmt : public Node
{
 public:
    ImplicitPartStmt()
      {
         pEntryStmt = NULL;
         pFormatStmt = NULL;
         pParameterStmt = NULL;
         pImplicitStmt = NULL;
      }
   virtual ~ImplicitPartStmt();

   enum OptionType
     {
        DEFAULT = 0,
        ImplicitPartStmt_ES,
        ImplicitPartStmt_FS,
        ImplicitPartStmt_PS,
        ImplicitPartStmt_IS
     };

    ImplicitPartStmt* newImplicitPartStmt()
      {
         ImplicitPartStmt* node = new ImplicitPartStmt();
         node->pEntryStmt = pEntryStmt;  pEntryStmt = NULL;
         node->pFormatStmt = pFormatStmt;  pFormatStmt = NULL;
         node->pParameterStmt = pParameterStmt;  pParameterStmt = NULL;
         node->pImplicitStmt = pImplicitStmt;  pImplicitStmt = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    EntryStmt* getEntryStmt() {return pEntryStmt;}
    FormatStmt* getFormatStmt() {return pFormatStmt;}
    ParameterStmt* getParameterStmt() {return pParameterStmt;}
    ImplicitStmt* getImplicitStmt() {return pImplicitStmt;}

    void setEntryStmt(EntryStmt* entrystmt) {pEntryStmt = entrystmt;}
    void setFormatStmt(FormatStmt* formatstmt) {pFormatStmt = formatstmt;}
    void setParameterStmt(ParameterStmt* parameterstmt) {pParameterStmt = parameterstmt;}
    void setImplicitStmt(ImplicitStmt* implicitstmt) {pImplicitStmt = implicitstmt;}

 private:
    EntryStmt* pEntryStmt;
    FormatStmt* pFormatStmt;
    ParameterStmt* pParameterStmt;
    ImplicitStmt* pImplicitStmt;
};

class DeclarationConstruct : public Node
{
 public:
    DeclarationConstruct()
      {
         pStatement = NULL;

         pStmtFunctionStmt = NULL;
         pOtherSpecificationStmt = NULL;
         pProcedureDeclarationStmt = NULL;
         pParameterStmt = NULL;
         pInterfaceBlock = NULL;
         pFormatStmt = NULL;
         pEnumDef = NULL;
         pEntryStmt = NULL;
         pDerivedTypeDef = NULL;
      }
   virtual ~DeclarationConstruct();

   enum OptionType
     {
        DEFAULT = 0,
        DeclarationConstruct_SFS,
        TypeDeclarationStmt_ot,
        DeclarationConstruct_OSS,
        DeclarationConstruct_PDS,
        DeclarationConstruct_PS,
        DeclarationConstruct_IB,
        DeclarationConstruct_FS,
        DeclarationConstruct_ED,
        DeclarationConstruct_ES,
        DeclarationConstruct_DTD
     };

    DeclarationConstruct* newDeclarationConstruct()
      {
         DeclarationConstruct* node = new DeclarationConstruct();

         node->pStatement = pStatement;  pStatement = NULL;

         node->pStmtFunctionStmt = pStmtFunctionStmt;  pStmtFunctionStmt = NULL;
         node->pOtherSpecificationStmt = pOtherSpecificationStmt;  pOtherSpecificationStmt = NULL;
         node->pProcedureDeclarationStmt = pProcedureDeclarationStmt;  pProcedureDeclarationStmt = NULL;
         node->pParameterStmt = pParameterStmt;  pParameterStmt = NULL;
         node->pInterfaceBlock = pInterfaceBlock;  pInterfaceBlock = NULL;
         node->pFormatStmt = pFormatStmt;  pFormatStmt = NULL;
         node->pEnumDef = pEnumDef;  pEnumDef = NULL;
         node->pEntryStmt = pEntryStmt;  pEntryStmt = NULL;
         node->pDerivedTypeDef = pDerivedTypeDef;  pDerivedTypeDef = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Statement* getStatement() {return pStatement;}

    StmtFunctionStmt* getStmtFunctionStmt() {return pStmtFunctionStmt;}
    OtherSpecificationStmt* getOtherSpecificationStmt() {return pOtherSpecificationStmt;}
    ProcedureDeclarationStmt* getProcedureDeclarationStmt() {return pProcedureDeclarationStmt;}
    ParameterStmt* getParameterStmt() {return pParameterStmt;}
    InterfaceBlock* getInterfaceBlock() {return pInterfaceBlock;}
    FormatStmt* getFormatStmt() {return pFormatStmt;}
    EnumDef* getEnumDef() {return pEnumDef;}
    EntryStmt* getEntryStmt() {return pEntryStmt;}
    DerivedTypeDef* getDerivedTypeDef() {return pDerivedTypeDef;}

    void setStatement(Statement* stmt) {pStatement = stmt;}

    void setStmtFunctionStmt(StmtFunctionStmt* stmtfunctionstmt) {pStmtFunctionStmt = stmtfunctionstmt;}
    void setOtherSpecificationStmt(OtherSpecificationStmt* otherspecificationstmt) {pOtherSpecificationStmt = otherspecificationstmt;}
    void setProcedureDeclarationStmt(ProcedureDeclarationStmt* proceduredeclarationstmt) {pProcedureDeclarationStmt = proceduredeclarationstmt;}
    void setParameterStmt(ParameterStmt* parameterstmt) {pParameterStmt = parameterstmt;}
    void setInterfaceBlock(InterfaceBlock* interfaceblock) {pInterfaceBlock = interfaceblock;}
    void setFormatStmt(FormatStmt* formatstmt) {pFormatStmt = formatstmt;}
    void setEnumDef(EnumDef* enumdef) {pEnumDef = enumdef;}
    void setEntryStmt(EntryStmt* entrystmt) {pEntryStmt = entrystmt;}
    void setDerivedTypeDef(DerivedTypeDef* derivedtypedef) {pDerivedTypeDef = derivedtypedef;}

 private:
    Statement* pStatement;

    StmtFunctionStmt* pStmtFunctionStmt;
    OtherSpecificationStmt* pOtherSpecificationStmt;
    ProcedureDeclarationStmt* pProcedureDeclarationStmt;
    ParameterStmt* pParameterStmt;
    InterfaceBlock* pInterfaceBlock;
    FormatStmt* pFormatStmt;
    EnumDef* pEnumDef;
    EntryStmt* pEntryStmt;
    DerivedTypeDef* pDerivedTypeDef;
};

class SpecAndExecPart : public Node
{
 public:
    SpecAndExecPart()
      {
         pStatementList = new std::vector<Statement*>();
      }
   virtual ~SpecAndExecPart();

    SpecAndExecPart* newSpecAndExecPart()
      {
         SpecAndExecPart* node = new SpecAndExecPart();
         delete node->pStatementList; node->pStatementList = pStatementList;  pStatementList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<Statement*>* getStatementList() {return pStatementList;}

   void appendStatement(Statement* stmt) {pStatementList->push_back(stmt);}

 private:
    std::vector<Statement*>* pStatementList;
};

class ExecutionPart : public Node
{
 public:
    ExecutionPart()
      {
         pExecutionPartConstructList = new std::vector<ExecutionPartConstruct*>();
      }
   virtual ~ExecutionPart();

    ExecutionPart* newExecutionPart()
      {
         ExecutionPart* node = new ExecutionPart();
         delete node->pExecutionPartConstructList; node->pExecutionPartConstructList = pExecutionPartConstructList;  pExecutionPartConstructList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<ExecutionPartConstruct*>* getExecutionPartConstructList() {return pExecutionPartConstructList;}

    void appendExecutionPartConstruct(ExecutionPartConstruct* executionpartconstruct) {pExecutionPartConstructList->push_back(executionpartconstruct);}

 private:
    std::vector<ExecutionPartConstruct*>* pExecutionPartConstructList;
};

class ExecutionPartConstruct : public Node
{
 public:
    ExecutionPartConstruct()
      {
         pDataStmt = NULL;
         pEntryStmt = NULL;
         pFormatStmt = NULL;
         pExecutableConstruct = NULL;
      }
   virtual ~ExecutionPartConstruct();

   enum OptionType
     {
        DEFAULT = 0,
        ExecutionPartConstruct_DS,
        ExecutionPartConstruct_ES,
        ExecutionPartConstruct_FS,
        ExecutionPartConstruct_EC
     };

    ExecutionPartConstruct* newExecutionPartConstruct()
      {
         ExecutionPartConstruct* node = new ExecutionPartConstruct();
         node->pDataStmt = pDataStmt;  pDataStmt = NULL;
         node->pEntryStmt = pEntryStmt;  pEntryStmt = NULL;
         node->pFormatStmt = pFormatStmt;  pFormatStmt = NULL;
         node->pExecutableConstruct = pExecutableConstruct;  pExecutableConstruct = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    DataStmt* getDataStmt() {return pDataStmt;}
    EntryStmt* getEntryStmt() {return pEntryStmt;}
    FormatStmt* getFormatStmt() {return pFormatStmt;}
    ExecutableConstruct* getExecutableConstruct() {return pExecutableConstruct;}

    void setDataStmt(DataStmt* datastmt) {pDataStmt = datastmt;}
    void setEntryStmt(EntryStmt* entrystmt) {pEntryStmt = entrystmt;}
    void setFormatStmt(FormatStmt* formatstmt) {pFormatStmt = formatstmt;}
    void setExecutableConstruct(ExecutableConstruct* executableconstruct) {pExecutableConstruct = executableconstruct;}

 private:
    DataStmt* pDataStmt;
    EntryStmt* pEntryStmt;
    FormatStmt* pFormatStmt;
    ExecutableConstruct* pExecutableConstruct;
};

class InternalSubprogramPart : public Node
{
 public:
    InternalSubprogramPart()
      {
         pContainsStmt = NULL;
         pInternalSubprogramList = new std::vector<InternalSubprogram*>();
      }
   virtual ~InternalSubprogramPart();

    InternalSubprogramPart* newInternalSubprogramPart()
      {
         InternalSubprogramPart* node = new InternalSubprogramPart();
         node->pContainsStmt = pContainsStmt;  pContainsStmt = NULL;
         delete node->pInternalSubprogramList; node->pInternalSubprogramList = pInternalSubprogramList;  pInternalSubprogramList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    ContainsStmt* getContainsStmt() {return pContainsStmt;}
    std::vector<InternalSubprogram*>* getInternalSubprogramList() {return pInternalSubprogramList;}

    void setContainsStmt(ContainsStmt* containsstmt) {pContainsStmt = containsstmt;}
    void appendInternalSubprogram(InternalSubprogram* internalsubprogram) {pInternalSubprogramList->push_back(internalsubprogram);}

 private:
    ContainsStmt* pContainsStmt;
    std::vector<InternalSubprogram*>* pInternalSubprogramList;
};

class InternalSubprogram : public Node
{
 public:
    InternalSubprogram()
      {
         pSubroutineSubprogram = NULL;
         pFunctionSubprogram = NULL;
      }
   virtual ~InternalSubprogram();

   enum OptionType
     {
        DEFAULT = 0,
        FunctionSubprogram_ot,
        SubroutineSubprogram_ot
     };

    InternalSubprogram* newInternalSubprogram()
      {
         InternalSubprogram* node = new InternalSubprogram();
         node->pSubroutineSubprogram = pSubroutineSubprogram;  pSubroutineSubprogram = NULL;
         node->pFunctionSubprogram = pFunctionSubprogram;  pFunctionSubprogram = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    SubroutineSubprogram* getSubroutineSubprogram() {return pSubroutineSubprogram;}
    FunctionSubprogram* getFunctionSubprogram() {return pFunctionSubprogram;}

    void setSubroutineSubprogram(SubroutineSubprogram* subroutinesubprogram) {pSubroutineSubprogram = subroutinesubprogram;}
    void setFunctionSubprogram(FunctionSubprogram* functionsubprogram) {pFunctionSubprogram = functionsubprogram;}

 private:
    SubroutineSubprogram* pSubroutineSubprogram;
    FunctionSubprogram* pFunctionSubprogram;
};

class OtherSpecificationStmt : public Node
{
 public:
    OtherSpecificationStmt()
      {
         pValueStmt = NULL;
         pVolatileStmt = NULL;
         pTargetStmt = NULL;
         pSaveStmt = NULL;
         pProtectedStmt = NULL;
         pPointerStmt = NULL;
         pOptionalStmt = NULL;
         pNamelistStmt = NULL;
         pIntrinsicStmt = NULL;
         pIntentStmt = NULL;
         pExternalStmt = NULL;
         pEquivalenceStmt = NULL;
         pDimensionStmt = NULL;
         pDataStmt = NULL;
         pCommonStmt = NULL;
         pContiguousStmt = NULL;
         pCodimensionStmt = NULL;
         pBindStmt = NULL;
         pAsynchronousStmt = NULL;
         pAllocatableStmt = NULL;
         pAccessStmt = NULL;
      }
   virtual ~OtherSpecificationStmt();

   enum OptionType
     {
        DEFAULT = 0,
        OtherSpecificationStmt_VS2,
        OtherSpecificationStmt_VS1,
        OtherSpecificationStmt_TS,
        OtherSpecificationStmt_SS,
        OtherSpecificationStmt_PS2,
        OtherSpecificationStmt_PS1,
        OtherSpecificationStmt_OS,
        OtherSpecificationStmt_NS,
        OtherSpecificationStmt_IS2,
        OtherSpecificationStmt_IS1,
        OtherSpecificationStmt_ES2,
        OtherSpecificationStmt_ES1,
        OtherSpecificationStmt_DS2,
        OtherSpecificationStmt_DS1,
        OtherSpecificationStmt_CS3,
        OtherSpecificationStmt_CS2,
        OtherSpecificationStmt_CS1,
        OtherSpecificationStmt_BS,
        OtherSpecificationStmt_AS3,
        OtherSpecificationStmt_AS2,
        OtherSpecificationStmt_AS1
     };

    OtherSpecificationStmt* newOtherSpecificationStmt()
      {
         OtherSpecificationStmt* node = new OtherSpecificationStmt();
         node->pValueStmt = pValueStmt;  pValueStmt = NULL;
         node->pVolatileStmt = pVolatileStmt;  pVolatileStmt = NULL;
         node->pTargetStmt = pTargetStmt;  pTargetStmt = NULL;
         node->pSaveStmt = pSaveStmt;  pSaveStmt = NULL;
         node->pProtectedStmt = pProtectedStmt;  pProtectedStmt = NULL;
         node->pPointerStmt = pPointerStmt;  pPointerStmt = NULL;
         node->pOptionalStmt = pOptionalStmt;  pOptionalStmt = NULL;
         node->pNamelistStmt = pNamelistStmt;  pNamelistStmt = NULL;
         node->pIntrinsicStmt = pIntrinsicStmt;  pIntrinsicStmt = NULL;
         node->pIntentStmt = pIntentStmt;  pIntentStmt = NULL;
         node->pExternalStmt = pExternalStmt;  pExternalStmt = NULL;
         node->pEquivalenceStmt = pEquivalenceStmt;  pEquivalenceStmt = NULL;
         node->pDimensionStmt = pDimensionStmt;  pDimensionStmt = NULL;
         node->pDataStmt = pDataStmt;  pDataStmt = NULL;
         node->pCommonStmt = pCommonStmt;  pCommonStmt = NULL;
         node->pContiguousStmt = pContiguousStmt;  pContiguousStmt = NULL;
         node->pCodimensionStmt = pCodimensionStmt;  pCodimensionStmt = NULL;
         node->pBindStmt = pBindStmt;  pBindStmt = NULL;
         node->pAsynchronousStmt = pAsynchronousStmt;  pAsynchronousStmt = NULL;
         node->pAllocatableStmt = pAllocatableStmt;  pAllocatableStmt = NULL;
         node->pAccessStmt = pAccessStmt;  pAccessStmt = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    ValueStmt* getValueStmt() {return pValueStmt;}
    VolatileStmt* getVolatileStmt() {return pVolatileStmt;}
    TargetStmt* getTargetStmt() {return pTargetStmt;}
    SaveStmt* getSaveStmt() {return pSaveStmt;}
    ProtectedStmt* getProtectedStmt() {return pProtectedStmt;}
    PointerStmt* getPointerStmt() {return pPointerStmt;}
    OptionalStmt* getOptionalStmt() {return pOptionalStmt;}
    NamelistStmt* getNamelistStmt() {return pNamelistStmt;}
    IntrinsicStmt* getIntrinsicStmt() {return pIntrinsicStmt;}
    IntentStmt* getIntentStmt() {return pIntentStmt;}
    ExternalStmt* getExternalStmt() {return pExternalStmt;}
    EquivalenceStmt* getEquivalenceStmt() {return pEquivalenceStmt;}
    DimensionStmt* getDimensionStmt() {return pDimensionStmt;}
    DataStmt* getDataStmt() {return pDataStmt;}
    CommonStmt* getCommonStmt() {return pCommonStmt;}
    ContiguousStmt* getContiguousStmt() {return pContiguousStmt;}
    CodimensionStmt* getCodimensionStmt() {return pCodimensionStmt;}
    BindStmt* getBindStmt() {return pBindStmt;}
    AsynchronousStmt* getAsynchronousStmt() {return pAsynchronousStmt;}
    AllocatableStmt* getAllocatableStmt() {return pAllocatableStmt;}
    AccessStmt* getAccessStmt() {return pAccessStmt;}

    void setValueStmt(ValueStmt* valuestmt) {pValueStmt = valuestmt;}
    void setVolatileStmt(VolatileStmt* volatilestmt) {pVolatileStmt = volatilestmt;}
    void setTargetStmt(TargetStmt* targetstmt) {pTargetStmt = targetstmt;}
    void setSaveStmt(SaveStmt* savestmt) {pSaveStmt = savestmt;}
    void setProtectedStmt(ProtectedStmt* protectedstmt) {pProtectedStmt = protectedstmt;}
    void setPointerStmt(PointerStmt* pointerstmt) {pPointerStmt = pointerstmt;}
    void setOptionalStmt(OptionalStmt* optionalstmt) {pOptionalStmt = optionalstmt;}
    void setNamelistStmt(NamelistStmt* nameliststmt) {pNamelistStmt = nameliststmt;}
    void setIntrinsicStmt(IntrinsicStmt* intrinsicstmt) {pIntrinsicStmt = intrinsicstmt;}
    void setIntentStmt(IntentStmt* intentstmt) {pIntentStmt = intentstmt;}
    void setExternalStmt(ExternalStmt* externalstmt) {pExternalStmt = externalstmt;}
    void setEquivalenceStmt(EquivalenceStmt* equivalencestmt) {pEquivalenceStmt = equivalencestmt;}
    void setDimensionStmt(DimensionStmt* dimensionstmt) {pDimensionStmt = dimensionstmt;}
    void setDataStmt(DataStmt* datastmt) {pDataStmt = datastmt;}
    void setCommonStmt(CommonStmt* commonstmt) {pCommonStmt = commonstmt;}
    void setContiguousStmt(ContiguousStmt* contiguousstmt) {pContiguousStmt = contiguousstmt;}
    void setCodimensionStmt(CodimensionStmt* codimensionstmt) {pCodimensionStmt = codimensionstmt;}
    void setBindStmt(BindStmt* bindstmt) {pBindStmt = bindstmt;}
    void setAsynchronousStmt(AsynchronousStmt* asynchronousstmt) {pAsynchronousStmt = asynchronousstmt;}
    void setAllocatableStmt(AllocatableStmt* allocatablestmt) {pAllocatableStmt = allocatablestmt;}
    void setAccessStmt(AccessStmt* accessstmt) {pAccessStmt = accessstmt;}

 private:
    ValueStmt* pValueStmt;
    VolatileStmt* pVolatileStmt;
    TargetStmt* pTargetStmt;
    SaveStmt* pSaveStmt;
    ProtectedStmt* pProtectedStmt;
    PointerStmt* pPointerStmt;
    OptionalStmt* pOptionalStmt;
    NamelistStmt* pNamelistStmt;
    IntrinsicStmt* pIntrinsicStmt;
    IntentStmt* pIntentStmt;
    ExternalStmt* pExternalStmt;
    EquivalenceStmt* pEquivalenceStmt;
    DimensionStmt* pDimensionStmt;
    DataStmt* pDataStmt;
    CommonStmt* pCommonStmt;
    ContiguousStmt* pContiguousStmt;
    CodimensionStmt* pCodimensionStmt;
    BindStmt* pBindStmt;
    AsynchronousStmt* pAsynchronousStmt;
    AllocatableStmt* pAllocatableStmt;
    AccessStmt* pAccessStmt;
};

class ExecutableConstruct : public Node
{
 public:
    ExecutableConstruct()
      {
         pStatement = NULL;

         pWhereConstruct = NULL;
         pSelectTypeConstruct = NULL;
         pIfConstruct = NULL;
         pForallConstruct = NULL;
         pCriticalConstruct = NULL;
         pCaseConstruct = NULL;
         pBlockConstruct = NULL;
         pAssociateConstruct = NULL;
         pActionStmt = NULL;
      }
   virtual ~ExecutableConstruct();

   enum OptionType
     {
        DEFAULT = 0,
        ExecutableConstruct_WC,
        ExecutableConstruct_STC,
        ExecutableConstruct_IC,
        ExecutableConstruct_FC,
        ExecutableConstruct_CC2,
        ExecutableConstruct_CC1,
        ExecutableConstruct_BC,
        ExecutableConstruct_AC,
        ExecutableConstruct_AS
     };

    ExecutableConstruct* newExecutableConstruct()
      {
         ExecutableConstruct* node = new ExecutableConstruct();

         node->pStatement = pStatement;  pStatement = NULL;

         node->pWhereConstruct = pWhereConstruct;  pWhereConstruct = NULL;
         node->pSelectTypeConstruct = pSelectTypeConstruct;  pSelectTypeConstruct = NULL;
         node->pIfConstruct = pIfConstruct;  pIfConstruct = NULL;
         node->pForallConstruct = pForallConstruct;  pForallConstruct = NULL;
         node->pCriticalConstruct = pCriticalConstruct;  pCriticalConstruct = NULL;
         node->pCaseConstruct = pCaseConstruct;  pCaseConstruct = NULL;
         node->pBlockConstruct = pBlockConstruct;  pBlockConstruct = NULL;
         node->pAssociateConstruct = pAssociateConstruct;  pAssociateConstruct = NULL;
         node->pActionStmt = pActionStmt;  pActionStmt = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Statement* getStatement() {return pStatement;}

    WhereConstruct* getWhereConstruct() {return pWhereConstruct;}
    SelectTypeConstruct* getSelectTypeConstruct() {return pSelectTypeConstruct;}
    IfConstruct* getIfConstruct() {return pIfConstruct;}
    ForallConstruct* getForallConstruct() {return pForallConstruct;}
    CriticalConstruct* getCriticalConstruct() {return pCriticalConstruct;}
    CaseConstruct* getCaseConstruct() {return pCaseConstruct;}
    BlockConstruct* getBlockConstruct() {return pBlockConstruct;}
    AssociateConstruct* getAssociateConstruct() {return pAssociateConstruct;}
    ActionStmt* getActionStmt() {return pActionStmt;}

    void setStatement(Statement* stmt) {pStatement = stmt;}

    void setWhereConstruct(WhereConstruct* whereconstruct) {pWhereConstruct = whereconstruct;}
    void setSelectTypeConstruct(SelectTypeConstruct* selecttypeconstruct) {pSelectTypeConstruct = selecttypeconstruct;}
    void setIfConstruct(IfConstruct* ifconstruct) {pIfConstruct = ifconstruct;}
    void setForallConstruct(ForallConstruct* forallconstruct) {pForallConstruct = forallconstruct;}
    void setCriticalConstruct(CriticalConstruct* criticalconstruct) {pCriticalConstruct = criticalconstruct;}
    void setCaseConstruct(CaseConstruct* caseconstruct) {pCaseConstruct = caseconstruct;}
    void setBlockConstruct(BlockConstruct* blockconstruct) {pBlockConstruct = blockconstruct;}
    void setAssociateConstruct(AssociateConstruct* associateconstruct) {pAssociateConstruct = associateconstruct;}
    void setActionStmt(ActionStmt* actionstmt) {pActionStmt = actionstmt;}

 private:
    Statement* pStatement;

    WhereConstruct* pWhereConstruct;
    SelectTypeConstruct* pSelectTypeConstruct;
    IfConstruct* pIfConstruct;
    ForallConstruct* pForallConstruct;
    CriticalConstruct* pCriticalConstruct;
    CaseConstruct* pCaseConstruct;
    BlockConstruct* pBlockConstruct;
    AssociateConstruct* pAssociateConstruct;
    ActionStmt* pActionStmt;
};

class ActionStmt : public Node
{
 public:
    ActionStmt()
      {
         pEndDoStmt = NULL;
         pNonlabelDoStmt = NULL;
         pLabelDoStmt = NULL;
         pComputedGotoStmt = NULL;
         pArithmeticIfStmt = NULL;
         pWriteStmt = NULL;
         pWhereStmt = NULL;
         pWaitStmt = NULL;
         pUnlockStmt = NULL;
         pSyncMemoryStmt = NULL;
         pSyncImagesStmt = NULL;
         pSyncAllStmt = NULL;
         pStopStmt = NULL;
         pRewindStmt = NULL;
         pReturnStmt = NULL;
         pReadStmt = NULL;
         pPrintStmt = NULL;
         pPointerAssignmentStmt = NULL;
         pOpenStmt = NULL;
         pNullifyStmt = NULL;
         pLockStmt = NULL;
         pInquireStmt = NULL;
         pIfStmt = NULL;
         pGotoStmt = NULL;
         pForallStmt = NULL;
         pFlushStmt = NULL;
         pExitStmt = NULL;
         pErrorStopStmt = NULL;
         pEndfileStmt = NULL;
         pEndSubroutineStmt = NULL;
         pEndProgramStmt = NULL;
         pEndMpSubprogramStmt = NULL;
         pEndFunctionStmt = NULL;
         pDeallocateStmt = NULL;
         pCycleStmt = NULL;
         pContinueStmt = NULL;
         pCloseStmt = NULL;
         pCallStmt = NULL;
         pBackspaceStmt = NULL;
         pStatement = NULL;
         pAllocateStmt = NULL;
         pPauseStmt = NULL;
      }
   virtual ~ActionStmt();

   enum OptionType
     {
        DEFAULT = 0,
        ActionStmt_EDS,
        ActionStmt_NDS,
        ActionStmt_LDS,
        ActionStmt_CGS,
        ActionStmt_AIS,
        ActionStmt_WS3,
        ActionStmt_WS2,
        ActionStmt_WS1,
        ActionStmt_US,
        ActionStmt_SMS,
        ActionStmt_SIS,
        ActionStmt_SAS,
        ActionStmt_SS,
        ActionStmt_RS3,
        ActionStmt_RS2,
        ActionStmt_RS1,
        ActionStmt_PS1,
        ActionStmt_PAS,
        ActionStmt_OS,
        ActionStmt_NS,
        ActionStmt_LS,
        ActionStmt_IS2,
        ActionStmt_IS1,
        ActionStmt_GS,
        ActionStmt_FS2,
        ActionStmt_FS1,
        ActionStmt_ES2,
        ActionStmt_ESS2,
        ActionStmt_ES1,
        ActionStmt_ESS1,
        ActionStmt_EPS,
        ActionStmt_EMSS,
        ActionStmt_EFS,
        ActionStmt_DS,
        ActionStmt_CS4,
        ActionStmt_CS3,
        ActionStmt_CS2,
        ActionStmt_CS1,
        ActionStmt_BS,
        AssignmentStmt_ot,
        ActionStmt_AS1,
        ActionStmt_PS2
     };

    ActionStmt* newActionStmt()
      {
         ActionStmt* node = new ActionStmt();
         node->pEndDoStmt = pEndDoStmt;  pEndDoStmt = NULL;
         node->pNonlabelDoStmt = pNonlabelDoStmt;  pNonlabelDoStmt = NULL;
         node->pLabelDoStmt = pLabelDoStmt;  pLabelDoStmt = NULL;
         node->pComputedGotoStmt = pComputedGotoStmt;  pComputedGotoStmt = NULL;
         node->pArithmeticIfStmt = pArithmeticIfStmt;  pArithmeticIfStmt = NULL;
         node->pWriteStmt = pWriteStmt;  pWriteStmt = NULL;
         node->pWhereStmt = pWhereStmt;  pWhereStmt = NULL;
         node->pWaitStmt = pWaitStmt;  pWaitStmt = NULL;
         node->pUnlockStmt = pUnlockStmt;  pUnlockStmt = NULL;
         node->pSyncMemoryStmt = pSyncMemoryStmt;  pSyncMemoryStmt = NULL;
         node->pSyncImagesStmt = pSyncImagesStmt;  pSyncImagesStmt = NULL;
         node->pSyncAllStmt = pSyncAllStmt;  pSyncAllStmt = NULL;
         node->pStopStmt = pStopStmt;  pStopStmt = NULL;
         node->pRewindStmt = pRewindStmt;  pRewindStmt = NULL;
         node->pReturnStmt = pReturnStmt;  pReturnStmt = NULL;
         node->pReadStmt = pReadStmt;  pReadStmt = NULL;
         node->pPrintStmt = pPrintStmt;  pPrintStmt = NULL;
         node->pPointerAssignmentStmt = pPointerAssignmentStmt;  pPointerAssignmentStmt = NULL;
         node->pOpenStmt = pOpenStmt;  pOpenStmt = NULL;
         node->pNullifyStmt = pNullifyStmt;  pNullifyStmt = NULL;
         node->pLockStmt = pLockStmt;  pLockStmt = NULL;
         node->pInquireStmt = pInquireStmt;  pInquireStmt = NULL;
         node->pIfStmt = pIfStmt;  pIfStmt = NULL;
         node->pGotoStmt = pGotoStmt;  pGotoStmt = NULL;
         node->pForallStmt = pForallStmt;  pForallStmt = NULL;
         node->pFlushStmt = pFlushStmt;  pFlushStmt = NULL;
         node->pExitStmt = pExitStmt;  pExitStmt = NULL;
         node->pErrorStopStmt = pErrorStopStmt;  pErrorStopStmt = NULL;
         node->pEndfileStmt = pEndfileStmt;  pEndfileStmt = NULL;
         node->pEndSubroutineStmt = pEndSubroutineStmt;  pEndSubroutineStmt = NULL;
         node->pEndProgramStmt = pEndProgramStmt;  pEndProgramStmt = NULL;
         node->pEndMpSubprogramStmt = pEndMpSubprogramStmt;  pEndMpSubprogramStmt = NULL;
         node->pEndFunctionStmt = pEndFunctionStmt;  pEndFunctionStmt = NULL;
         node->pDeallocateStmt = pDeallocateStmt;  pDeallocateStmt = NULL;
         node->pCycleStmt = pCycleStmt;  pCycleStmt = NULL;
         node->pContinueStmt = pContinueStmt;  pContinueStmt = NULL;
         node->pCloseStmt = pCloseStmt;  pCloseStmt = NULL;
         node->pCallStmt = pCallStmt;  pCallStmt = NULL;
         node->pBackspaceStmt = pBackspaceStmt;  pBackspaceStmt = NULL;
         node->pStatement = pStatement;  pStatement = NULL;
         node->pAllocateStmt = pAllocateStmt;  pAllocateStmt = NULL;
         node->pPauseStmt = pPauseStmt;  pPauseStmt = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    EndDoStmt* getEndDoStmt() {return pEndDoStmt;}
    NonlabelDoStmt* getNonlabelDoStmt() {return pNonlabelDoStmt;}
    LabelDoStmt* getLabelDoStmt() {return pLabelDoStmt;}
    ComputedGotoStmt* getComputedGotoStmt() {return pComputedGotoStmt;}
    ArithmeticIfStmt* getArithmeticIfStmt() {return pArithmeticIfStmt;}
    WriteStmt* getWriteStmt() {return pWriteStmt;}
    WhereStmt* getWhereStmt() {return pWhereStmt;}
    WaitStmt* getWaitStmt() {return pWaitStmt;}
    UnlockStmt* getUnlockStmt() {return pUnlockStmt;}
    SyncMemoryStmt* getSyncMemoryStmt() {return pSyncMemoryStmt;}
    SyncImagesStmt* getSyncImagesStmt() {return pSyncImagesStmt;}
    SyncAllStmt* getSyncAllStmt() {return pSyncAllStmt;}
    StopStmt* getStopStmt() {return pStopStmt;}
    RewindStmt* getRewindStmt() {return pRewindStmt;}
    ReturnStmt* getReturnStmt() {return pReturnStmt;}
    ReadStmt* getReadStmt() {return pReadStmt;}
    PrintStmt* getPrintStmt() {return pPrintStmt;}
    PointerAssignmentStmt* getPointerAssignmentStmt() {return pPointerAssignmentStmt;}
    OpenStmt* getOpenStmt() {return pOpenStmt;}
    NullifyStmt* getNullifyStmt() {return pNullifyStmt;}
    LockStmt* getLockStmt() {return pLockStmt;}
    InquireStmt* getInquireStmt() {return pInquireStmt;}
    IfStmt* getIfStmt() {return pIfStmt;}
    GotoStmt* getGotoStmt() {return pGotoStmt;}
    ForallStmt* getForallStmt() {return pForallStmt;}
    FlushStmt* getFlushStmt() {return pFlushStmt;}
    ExitStmt* getExitStmt() {return pExitStmt;}
    ErrorStopStmt* getErrorStopStmt() {return pErrorStopStmt;}
    EndfileStmt* getEndfileStmt() {return pEndfileStmt;}
    EndSubroutineStmt* getEndSubroutineStmt() {return pEndSubroutineStmt;}
    EndProgramStmt* getEndProgramStmt() {return pEndProgramStmt;}
    EndMpSubprogramStmt* getEndMpSubprogramStmt() {return pEndMpSubprogramStmt;}
    EndFunctionStmt* getEndFunctionStmt() {return pEndFunctionStmt;}
    DeallocateStmt* getDeallocateStmt() {return pDeallocateStmt;}
    CycleStmt* getCycleStmt() {return pCycleStmt;}
    ContinueStmt* getContinueStmt() {return pContinueStmt;}
    CloseStmt* getCloseStmt() {return pCloseStmt;}
    CallStmt* getCallStmt() {return pCallStmt;}
    BackspaceStmt* getBackspaceStmt() {return pBackspaceStmt;}
    Statement* getStatement() {return pStatement;}
    AllocateStmt* getAllocateStmt() {return pAllocateStmt;}
    PauseStmt* getPauseStmt() {return pPauseStmt;}

    void setEndDoStmt(EndDoStmt* enddostmt) {pEndDoStmt = enddostmt;}
    void setNonlabelDoStmt(NonlabelDoStmt* nonlabeldostmt) {pNonlabelDoStmt = nonlabeldostmt;}
    void setLabelDoStmt(LabelDoStmt* labeldostmt) {pLabelDoStmt = labeldostmt;}
    void setComputedGotoStmt(ComputedGotoStmt* computedgotostmt) {pComputedGotoStmt = computedgotostmt;}
    void setArithmeticIfStmt(ArithmeticIfStmt* arithmeticifstmt) {pArithmeticIfStmt = arithmeticifstmt;}
    void setWriteStmt(WriteStmt* writestmt) {pWriteStmt = writestmt;}
    void setWhereStmt(WhereStmt* wherestmt) {pWhereStmt = wherestmt;}
    void setWaitStmt(WaitStmt* waitstmt) {pWaitStmt = waitstmt;}
    void setUnlockStmt(UnlockStmt* unlockstmt) {pUnlockStmt = unlockstmt;}
    void setSyncMemoryStmt(SyncMemoryStmt* syncmemorystmt) {pSyncMemoryStmt = syncmemorystmt;}
    void setSyncImagesStmt(SyncImagesStmt* syncimagesstmt) {pSyncImagesStmt = syncimagesstmt;}
    void setSyncAllStmt(SyncAllStmt* syncallstmt) {pSyncAllStmt = syncallstmt;}
    void setStopStmt(StopStmt* stopstmt) {pStopStmt = stopstmt;}
    void setRewindStmt(RewindStmt* rewindstmt) {pRewindStmt = rewindstmt;}
    void setReturnStmt(ReturnStmt* returnstmt) {pReturnStmt = returnstmt;}
    void setReadStmt(ReadStmt* readstmt) {pReadStmt = readstmt;}
    void setPrintStmt(PrintStmt* printstmt) {pPrintStmt = printstmt;}
    void setPointerAssignmentStmt(PointerAssignmentStmt* pointerassignmentstmt) {pPointerAssignmentStmt = pointerassignmentstmt;}
    void setOpenStmt(OpenStmt* openstmt) {pOpenStmt = openstmt;}
    void setNullifyStmt(NullifyStmt* nullifystmt) {pNullifyStmt = nullifystmt;}
    void setLockStmt(LockStmt* lockstmt) {pLockStmt = lockstmt;}
    void setInquireStmt(InquireStmt* inquirestmt) {pInquireStmt = inquirestmt;}
    void setIfStmt(IfStmt* ifstmt) {pIfStmt = ifstmt;}
    void setGotoStmt(GotoStmt* gotostmt) {pGotoStmt = gotostmt;}
    void setForallStmt(ForallStmt* forallstmt) {pForallStmt = forallstmt;}
    void setFlushStmt(FlushStmt* flushstmt) {pFlushStmt = flushstmt;}
    void setExitStmt(ExitStmt* exitstmt) {pExitStmt = exitstmt;}
    void setErrorStopStmt(ErrorStopStmt* errorstopstmt) {pErrorStopStmt = errorstopstmt;}
    void setEndfileStmt(EndfileStmt* endfilestmt) {pEndfileStmt = endfilestmt;}
    void setEndSubroutineStmt(EndSubroutineStmt* endsubroutinestmt) {pEndSubroutineStmt = endsubroutinestmt;}
    void setEndProgramStmt(EndProgramStmt* endprogramstmt) {pEndProgramStmt = endprogramstmt;}
    void setEndMpSubprogramStmt(EndMpSubprogramStmt* endmpsubprogramstmt) {pEndMpSubprogramStmt = endmpsubprogramstmt;}
    void setEndFunctionStmt(EndFunctionStmt* endfunctionstmt) {pEndFunctionStmt = endfunctionstmt;}
    void setDeallocateStmt(DeallocateStmt* deallocatestmt) {pDeallocateStmt = deallocatestmt;}
    void setCycleStmt(CycleStmt* cyclestmt) {pCycleStmt = cyclestmt;}
    void setContinueStmt(ContinueStmt* continuestmt) {pContinueStmt = continuestmt;}
    void setCloseStmt(CloseStmt* closestmt) {pCloseStmt = closestmt;}
    void setCallStmt(CallStmt* callstmt) {pCallStmt = callstmt;}
    void setBackspaceStmt(BackspaceStmt* backspacestmt) {pBackspaceStmt = backspacestmt;}
    void setStatement(Statement* stmt) {pStatement = stmt;}
    void setAllocateStmt(AllocateStmt* allocatestmt) {pAllocateStmt = allocatestmt;}
    void setPauseStmt(PauseStmt* pausestmt) {pPauseStmt = pausestmt;}

 private:
    EndDoStmt* pEndDoStmt;
    NonlabelDoStmt* pNonlabelDoStmt;
    LabelDoStmt* pLabelDoStmt;
    ComputedGotoStmt* pComputedGotoStmt;
    ArithmeticIfStmt* pArithmeticIfStmt;
    WriteStmt* pWriteStmt;
    WhereStmt* pWhereStmt;
    WaitStmt* pWaitStmt;
    UnlockStmt* pUnlockStmt;
    SyncMemoryStmt* pSyncMemoryStmt;
    SyncImagesStmt* pSyncImagesStmt;
    SyncAllStmt* pSyncAllStmt;
    StopStmt* pStopStmt;
    RewindStmt* pRewindStmt;
    ReturnStmt* pReturnStmt;
    ReadStmt* pReadStmt;
    PrintStmt* pPrintStmt;
    PointerAssignmentStmt* pPointerAssignmentStmt;
    OpenStmt* pOpenStmt;
    NullifyStmt* pNullifyStmt;
    LockStmt* pLockStmt;
    InquireStmt* pInquireStmt;
    IfStmt* pIfStmt;
    GotoStmt* pGotoStmt;
    ForallStmt* pForallStmt;
    FlushStmt* pFlushStmt;
    ExitStmt* pExitStmt;
    ErrorStopStmt* pErrorStopStmt;
    EndfileStmt* pEndfileStmt;
    EndSubroutineStmt* pEndSubroutineStmt;
    EndProgramStmt* pEndProgramStmt;
    EndMpSubprogramStmt* pEndMpSubprogramStmt;
    EndFunctionStmt* pEndFunctionStmt;
    DeallocateStmt* pDeallocateStmt;
    CycleStmt* pCycleStmt;
    ContinueStmt* pContinueStmt;
    CloseStmt* pCloseStmt;
    CallStmt* pCallStmt;
    BackspaceStmt* pBackspaceStmt;
    Statement* pStatement;
    AllocateStmt* pAllocateStmt;
    PauseStmt* pPauseStmt;
};

class Keyword : public Node
{
 public:
    Keyword()
      {
         pName = NULL;
      }
   virtual ~Keyword();

    Keyword* newKeyword()
      {
         Keyword* node = new Keyword();
         node->pName = pName;  pName = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Name* getName() {return pName;}

    void setName(Name* name) {pName = name;}

 private:
    Name* pName;
};

class Constant : public Node
{
 public:
    Constant()
      {
         pLiteralConstant = NULL;
      }
   virtual ~Constant();

    Constant* newConstant()
      {
         Constant* node = new Constant();
         node->pLiteralConstant = pLiteralConstant;  pLiteralConstant = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    LiteralConstant* getLiteralConstant() {return pLiteralConstant;}

    void setLiteralConstant(LiteralConstant* literalconstant) {pLiteralConstant = literalconstant;}

 private:
    LiteralConstant* pLiteralConstant;
};

class LiteralConstant : public Node
{
 public:
    LiteralConstant()
      {
         pBozLiteralConstant = NULL;
         pCharLiteralConstant = NULL;
         pLogicalLiteralConstant = NULL;
         pComplexLiteralConstant = NULL;
         pRealLiteralConstant = NULL;
         pIntLiteralConstant = NULL;
      }
   virtual ~LiteralConstant();

   enum OptionType
     {
        DEFAULT = 0,
        LiteralConstant_BLC,
        LiteralConstant_CRLC,
        LiteralConstant_LLC,
        LiteralConstant_CXLC,
        LiteralConstant_RLC,
        IntLiteralConstant_ot
     };

    LiteralConstant* newLiteralConstant()
      {
         LiteralConstant* node = new LiteralConstant();
         node->pBozLiteralConstant = pBozLiteralConstant;  pBozLiteralConstant = NULL;
         node->pCharLiteralConstant = pCharLiteralConstant;  pCharLiteralConstant = NULL;
         node->pLogicalLiteralConstant = pLogicalLiteralConstant;  pLogicalLiteralConstant = NULL;
         node->pComplexLiteralConstant = pComplexLiteralConstant;  pComplexLiteralConstant = NULL;
         node->pRealLiteralConstant = pRealLiteralConstant;  pRealLiteralConstant = NULL;
         node->pIntLiteralConstant = pIntLiteralConstant;  pIntLiteralConstant = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    BozLiteralConstant* getBozLiteralConstant() {return pBozLiteralConstant;}
    CharLiteralConstant* getCharLiteralConstant() {return pCharLiteralConstant;}
    LogicalLiteralConstant* getLogicalLiteralConstant() {return pLogicalLiteralConstant;}
    ComplexLiteralConstant* getComplexLiteralConstant() {return pComplexLiteralConstant;}
    RealLiteralConstant* getRealLiteralConstant() {return pRealLiteralConstant;}
    IntLiteralConstant* getIntLiteralConstant() {return pIntLiteralConstant;}

    void setBozLiteralConstant(BozLiteralConstant* bozliteralconstant) {pBozLiteralConstant = bozliteralconstant;}
    void setCharLiteralConstant(CharLiteralConstant* charliteralconstant) {pCharLiteralConstant = charliteralconstant;}
    void setLogicalLiteralConstant(LogicalLiteralConstant* logicalliteralconstant) {pLogicalLiteralConstant = logicalliteralconstant;}
    void setComplexLiteralConstant(ComplexLiteralConstant* complexliteralconstant) {pComplexLiteralConstant = complexliteralconstant;}
    void setRealLiteralConstant(RealLiteralConstant* realliteralconstant) {pRealLiteralConstant = realliteralconstant;}
    void setIntLiteralConstant(IntLiteralConstant* intliteralconstant) {pIntLiteralConstant = intliteralconstant;}

 private:
    BozLiteralConstant* pBozLiteralConstant;
    CharLiteralConstant* pCharLiteralConstant;
    LogicalLiteralConstant* pLogicalLiteralConstant;
    ComplexLiteralConstant* pComplexLiteralConstant;
    RealLiteralConstant* pRealLiteralConstant;
    IntLiteralConstant* pIntLiteralConstant;
};

class NamedConstant : public Node
{
 public:
    NamedConstant()
      {
         pName = NULL;
      }
   virtual ~NamedConstant();

    NamedConstant* newNamedConstant()
      {
         NamedConstant* node = new NamedConstant();
         node->pName = pName;  pName = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Name* getName() {return pName;}

    void setName(Name* name) {pName = name;}

 private:
    Name* pName;
};

class TypeParamValue : public Node
{
 public:
    TypeParamValue()
      {
         pExpr = NULL;
      }
   virtual ~TypeParamValue();

   enum OptionType
     {
        DEFAULT = 0,
        TypeParamValue_COLON,
        TypeParamValue_STAR,
        TypeParamValue_E
     };

    TypeParamValue* newTypeParamValue()
      {
         TypeParamValue* node = new TypeParamValue();
         node->pExpr = pExpr;  pExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Expr* getExpr() {return pExpr;}

    void setExpr(Expr* expr) {pExpr = expr;}

 private:
    Expr* pExpr;
};

class TypeSpec : public Node
{
 public:
    TypeSpec()
      {
         pDerivedTypeSpec = NULL;
         pIntrinsicTypeSpec = NULL;
      }
   virtual ~TypeSpec();

   enum OptionType
     {
        DEFAULT = 0,
        TypeSpec_DTS,
        TypeSpec_ITS
     };

    TypeSpec* newTypeSpec()
      {
         TypeSpec* node = new TypeSpec();
         node->pDerivedTypeSpec = pDerivedTypeSpec;  pDerivedTypeSpec = NULL;
         node->pIntrinsicTypeSpec = pIntrinsicTypeSpec;  pIntrinsicTypeSpec = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    DerivedTypeSpec* getDerivedTypeSpec() {return pDerivedTypeSpec;}
    IntrinsicTypeSpec* getIntrinsicTypeSpec() {return pIntrinsicTypeSpec;}

    void setDerivedTypeSpec(DerivedTypeSpec* derivedtypespec) {pDerivedTypeSpec = derivedtypespec;}
    void setIntrinsicTypeSpec(IntrinsicTypeSpec* intrinsictypespec) {pIntrinsicTypeSpec = intrinsictypespec;}

 private:
    DerivedTypeSpec* pDerivedTypeSpec;
    IntrinsicTypeSpec* pIntrinsicTypeSpec;
};

class DeclarationTypeSpec : public Node
{
 public:
    DeclarationTypeSpec()
      {
         pDerivedTypeSpec = NULL;
         pIntrinsicTypeSpec = NULL;
      }
   virtual ~DeclarationTypeSpec();

   enum OptionType
     {
        DEFAULT = 0,
        DeclarationTypeSpec_T_STAR,
        DeclarationTypeSpec_C_STAR,
        DeclarationTypeSpec_C_DTS,
        DeclarationTypeSpec_T_DTS,
        IntrinsicType,
        DeclarationTypeSpec_ITS
     };

    DeclarationTypeSpec* newDeclarationTypeSpec()
      {
         DeclarationTypeSpec* node = new DeclarationTypeSpec();
         node->pDerivedTypeSpec = pDerivedTypeSpec;  pDerivedTypeSpec = NULL;
         node->pIntrinsicTypeSpec = pIntrinsicTypeSpec;  pIntrinsicTypeSpec = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    DerivedTypeSpec* getDerivedTypeSpec() {return pDerivedTypeSpec;}
    IntrinsicTypeSpec* getIntrinsicTypeSpec() {return pIntrinsicTypeSpec;}

    void setDerivedTypeSpec(DerivedTypeSpec* derivedtypespec) {pDerivedTypeSpec = derivedtypespec;}
    void setIntrinsicTypeSpec(IntrinsicTypeSpec* intrinsictypespec) {pIntrinsicTypeSpec = intrinsictypespec;}

 private:
    DerivedTypeSpec* pDerivedTypeSpec;
    IntrinsicTypeSpec* pIntrinsicTypeSpec;
};

class IntrinsicTypeSpec : public Node
{
 public:
    IntrinsicTypeSpec()
      {
         pKindSelector = NULL;
         pCharSelector = NULL;
      }
   virtual ~IntrinsicTypeSpec();

   enum OptionType
     {
        DEFAULT = 0,
        IntrinsicTypeSpec_DBL_CMPLX,
        LOGICAL,
        IntrinsicTypeSpec_CHAR,
        IntrinsicTypeSpec_CMPLX,
        IntrinsicTypeSpec_DBL_PREC,
        IntrinsicTypeSpec_REAL,
        INTEGER
     };

    IntrinsicTypeSpec* newIntrinsicTypeSpec()
      {
         IntrinsicTypeSpec* node = new IntrinsicTypeSpec();
         node->pKindSelector = pKindSelector;  pKindSelector = NULL;
         node->pCharSelector = pCharSelector;  pCharSelector = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    KindSelector* getKindSelector() {return pKindSelector;}
    CharSelector* getCharSelector() {return pCharSelector;}

    void setKindSelector(KindSelector* kindselector) {pKindSelector = kindselector;}
    void setCharSelector(CharSelector* charselector) {pCharSelector = charselector;}

 private:
    KindSelector* pKindSelector;
    CharSelector* pCharSelector;
};

class KindSelector : public Node
{
 public:
    KindSelector()
      {
         pIcon = NULL;
         pExpr = NULL;
      }
   virtual ~KindSelector();

   enum OptionType
     {
        DEFAULT = 0,
        KindSelector_STAR,
        KindSelector_E
     };

    KindSelector* newKindSelector()
      {
         KindSelector* node = new KindSelector();
         node->pIcon = pIcon;  pIcon = NULL;
         node->pExpr = pExpr;  pExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Icon* getIcon() {return pIcon;}
    Expr* getExpr() {return pExpr;}

    void setIcon(Icon* icon) {pIcon = icon;}
    void setExpr(Expr* expr) {pExpr = expr;}

 private:
    Icon* pIcon;
    Expr* pExpr;
};

class SignedIntLiteralConstant : public Node
{
 public:
    SignedIntLiteralConstant()
      {
         pSign = NULL;
         pIntLiteralConstant = NULL;
      }
   virtual ~SignedIntLiteralConstant();

    SignedIntLiteralConstant* newSignedIntLiteralConstant()
      {
         SignedIntLiteralConstant* node = new SignedIntLiteralConstant();
         node->pSign = pSign;  pSign = NULL;
         node->pIntLiteralConstant = pIntLiteralConstant;  pIntLiteralConstant = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Sign* getSign() {return pSign;}
    IntLiteralConstant* getIntLiteralConstant() {return pIntLiteralConstant;}

    void setSign(Sign* sign) {pSign = sign;}
    void setIntLiteralConstant(IntLiteralConstant* intliteralconstant) {pIntLiteralConstant = intliteralconstant;}

 private:
    Sign* pSign;
    IntLiteralConstant* pIntLiteralConstant;
};

class IntLiteralConstant : public Node
{
 public:
    IntLiteralConstant()
      {
         pDigitString = NULL;
         pKindParam = NULL;
      }
   virtual ~IntLiteralConstant();

    IntLiteralConstant* newIntLiteralConstant()
      {
         IntLiteralConstant* node = new IntLiteralConstant();
         node->pDigitString = pDigitString;  pDigitString = NULL;
         node->pKindParam = pKindParam;  pKindParam = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    DigitString* getDigitString() {return pDigitString;}
    KindParam* getKindParam() {return pKindParam;}

    void setDigitString(DigitString* digitstring) {pDigitString = digitstring;}
    void setKindParam(KindParam* kindparam) {pKindParam = kindparam;}

 private:
    DigitString* pDigitString;
    KindParam* pKindParam;
};

class KindParam : public Node
{
 public:
    KindParam()
      {
         pScalarIntConstantName = NULL;
         pDigitString = NULL;
      }
   virtual ~KindParam();

   enum OptionType
     {
        DEFAULT = 0,
        KindParam_SICN,
        DigitString_ot
     };

    KindParam* newKindParam()
      {
         KindParam* node = new KindParam();
         node->pScalarIntConstantName = pScalarIntConstantName;  pScalarIntConstantName = NULL;
         node->pDigitString = pDigitString;  pDigitString = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Name* getScalarIntConstantName() {return pScalarIntConstantName;}
    DigitString* getDigitString() {return pDigitString;}

    void setScalarIntConstantName(Name* scalarintconstantname) {pScalarIntConstantName = scalarintconstantname;}
    void setDigitString(DigitString* digitstring) {pDigitString = digitstring;}

 private:
    Name* pScalarIntConstantName;
    DigitString* pDigitString;
};

class DigitString : public Node
{
 public:
    DigitString()
      {
         pIcon = NULL;
      }
   virtual ~DigitString();

    DigitString* newDigitString()
      {
         DigitString* node = new DigitString();
         node->pIcon = pIcon;  pIcon = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Icon* getIcon() {return pIcon;}

    void setIcon(Icon* icon) {pIcon = icon;}

 private:
    Icon* pIcon;
};

class Sign : public Node
{
 public:
    Sign()
      {
      }
   virtual ~Sign();

   enum OptionType
     {
        DEFAULT = 0,
        Sign_MINUS,
        Sign_PLUS
     };

    Sign* newSign()
      {
         Sign* node = new Sign();
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

 private:
};

class SignedRealLiteralConstant : public Node
{
 public:
    SignedRealLiteralConstant()
      {
         pSign = NULL;
         pRealLiteralConstant = NULL;
      }
   virtual ~SignedRealLiteralConstant();

    SignedRealLiteralConstant* newSignedRealLiteralConstant()
      {
         SignedRealLiteralConstant* node = new SignedRealLiteralConstant();
         node->pSign = pSign;  pSign = NULL;
         node->pRealLiteralConstant = pRealLiteralConstant;  pRealLiteralConstant = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Sign* getSign() {return pSign;}
    RealLiteralConstant* getRealLiteralConstant() {return pRealLiteralConstant;}

    void setSign(Sign* sign) {pSign = sign;}
    void setRealLiteralConstant(RealLiteralConstant* realliteralconstant) {pRealLiteralConstant = realliteralconstant;}

 private:
    Sign* pSign;
    RealLiteralConstant* pRealLiteralConstant;
};

class RealLiteralConstant : public Node
{
 public:
    RealLiteralConstant()
      {
         pRcon = NULL;
         pKindParam = NULL;
      }
   virtual ~RealLiteralConstant();

    RealLiteralConstant* newRealLiteralConstant()
      {
         RealLiteralConstant* node = new RealLiteralConstant();
         node->pRcon = pRcon;  pRcon = NULL;
         node->pKindParam = pKindParam;  pKindParam = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Rcon* getRcon() {return pRcon;}
    KindParam* getKindParam() {return pKindParam;}

    void setRcon(Rcon* rcon) {pRcon = rcon;}
    void setKindParam(KindParam* kindparam) {pKindParam = kindparam;}

 private:
    Rcon* pRcon;
    KindParam* pKindParam;
};

class ComplexLiteralConstant : public Node
{
 public:
    ComplexLiteralConstant()
      {
         pRealPart = NULL;
         pImagPart = NULL;
      }
   virtual ~ComplexLiteralConstant();

    ComplexLiteralConstant* newComplexLiteralConstant()
      {
         ComplexLiteralConstant* node = new ComplexLiteralConstant();
         node->pRealPart = pRealPart;  pRealPart = NULL;
         node->pImagPart = pImagPart;  pImagPart = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    RealPart* getRealPart() {return pRealPart;}
    ImagPart* getImagPart() {return pImagPart;}

    void setRealPart(RealPart* realpart) {pRealPart = realpart;}
    void setImagPart(ImagPart* imagpart) {pImagPart = imagpart;}

 private:
    RealPart* pRealPart;
    ImagPart* pImagPart;
};

class RealPart : public Node
{
 public:
    RealPart()
      {
         pNamedConstant = NULL;
         pSignedRealLiteralConstant = NULL;
         pSignedIntLiteralConstant = NULL;
      }
   virtual ~RealPart();

   enum OptionType
     {
        DEFAULT = 0,
        RealPart_NC,
        RealPart_SRLC,
        RealPart_SILC
     };

    RealPart* newRealPart()
      {
         RealPart* node = new RealPart();
         node->pNamedConstant = pNamedConstant;  pNamedConstant = NULL;
         node->pSignedRealLiteralConstant = pSignedRealLiteralConstant;  pSignedRealLiteralConstant = NULL;
         node->pSignedIntLiteralConstant = pSignedIntLiteralConstant;  pSignedIntLiteralConstant = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    NamedConstant* getNamedConstant() {return pNamedConstant;}
    SignedRealLiteralConstant* getSignedRealLiteralConstant() {return pSignedRealLiteralConstant;}
    SignedIntLiteralConstant* getSignedIntLiteralConstant() {return pSignedIntLiteralConstant;}

    void setNamedConstant(NamedConstant* namedconstant) {pNamedConstant = namedconstant;}
    void setSignedRealLiteralConstant(SignedRealLiteralConstant* signedrealliteralconstant) {pSignedRealLiteralConstant = signedrealliteralconstant;}
    void setSignedIntLiteralConstant(SignedIntLiteralConstant* signedintliteralconstant) {pSignedIntLiteralConstant = signedintliteralconstant;}

 private:
    NamedConstant* pNamedConstant;
    SignedRealLiteralConstant* pSignedRealLiteralConstant;
    SignedIntLiteralConstant* pSignedIntLiteralConstant;
};

class ImagPart : public Node
{
 public:
    ImagPart()
      {
         pNamedConstant = NULL;
         pSignedRealLiteralConstant = NULL;
         pSignedIntLiteralConstant = NULL;
      }
   virtual ~ImagPart();

   enum OptionType
     {
        DEFAULT = 0,
        ImagPart_NC,
        ImagPart_SRLC,
        ImagPart_SILC
     };

    ImagPart* newImagPart()
      {
         ImagPart* node = new ImagPart();
         node->pNamedConstant = pNamedConstant;  pNamedConstant = NULL;
         node->pSignedRealLiteralConstant = pSignedRealLiteralConstant;  pSignedRealLiteralConstant = NULL;
         node->pSignedIntLiteralConstant = pSignedIntLiteralConstant;  pSignedIntLiteralConstant = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    NamedConstant* getNamedConstant() {return pNamedConstant;}
    SignedRealLiteralConstant* getSignedRealLiteralConstant() {return pSignedRealLiteralConstant;}
    SignedIntLiteralConstant* getSignedIntLiteralConstant() {return pSignedIntLiteralConstant;}

    void setNamedConstant(NamedConstant* namedconstant) {pNamedConstant = namedconstant;}
    void setSignedRealLiteralConstant(SignedRealLiteralConstant* signedrealliteralconstant) {pSignedRealLiteralConstant = signedrealliteralconstant;}
    void setSignedIntLiteralConstant(SignedIntLiteralConstant* signedintliteralconstant) {pSignedIntLiteralConstant = signedintliteralconstant;}

 private:
    NamedConstant* pNamedConstant;
    SignedRealLiteralConstant* pSignedRealLiteralConstant;
    SignedIntLiteralConstant* pSignedIntLiteralConstant;
};

class CharSelector : public Node
{
 public:
    CharSelector()
      {
         pExpr = NULL;
         pTypeParamValue = NULL;
         pLengthSelector = NULL;
      }
   virtual ~CharSelector();

   enum OptionType
     {
        DEFAULT = 0,
        CharSelector_KIND,
        CharSelector_KIND_LEN,
        CharSelector_LEN_KIND,
        CharSelector_LS
     };

    CharSelector* newCharSelector()
      {
         CharSelector* node = new CharSelector();
         node->pExpr = pExpr;  pExpr = NULL;
         node->pTypeParamValue = pTypeParamValue;  pTypeParamValue = NULL;
         node->pLengthSelector = pLengthSelector;  pLengthSelector = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Expr* getExpr() {return pExpr;}
    TypeParamValue* getTypeParamValue() {return pTypeParamValue;}
    LengthSelector* getLengthSelector() {return pLengthSelector;}

    void setExpr(Expr* expr) {pExpr = expr;}
    void setTypeParamValue(TypeParamValue* typeparamvalue) {pTypeParamValue = typeparamvalue;}
    void setLengthSelector(LengthSelector* lengthselector) {pLengthSelector = lengthselector;}

 private:
    Expr* pExpr;
    TypeParamValue* pTypeParamValue;
    LengthSelector* pLengthSelector;
};

class LengthSelector : public Node
{
 public:
    LengthSelector()
      {
         pCharLength = NULL;
         pTypeParamValue = NULL;
      }
   virtual ~LengthSelector();

   enum OptionType
     {
        DEFAULT = 0,
        LengthSelector_STAR,
        LengthSelector_LEN
     };

    LengthSelector* newLengthSelector()
      {
         LengthSelector* node = new LengthSelector();
         node->pCharLength = pCharLength;  pCharLength = NULL;
         node->pTypeParamValue = pTypeParamValue;  pTypeParamValue = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    CharLength* getCharLength() {return pCharLength;}
    TypeParamValue* getTypeParamValue() {return pTypeParamValue;}

    void setCharLength(CharLength* charlength) {pCharLength = charlength;}
    void setTypeParamValue(TypeParamValue* typeparamvalue) {pTypeParamValue = typeparamvalue;}

 private:
    CharLength* pCharLength;
    TypeParamValue* pTypeParamValue;
};

class CharLength : public Node
{
 public:
    CharLength()
      {
         pIcon = NULL;
         pTypeParamValue = NULL;
      }
   virtual ~CharLength();

   enum OptionType
     {
        DEFAULT = 0,
        CharLength_I,
        CharLength_TPV
     };

    CharLength* newCharLength()
      {
         CharLength* node = new CharLength();
         node->pIcon = pIcon;  pIcon = NULL;
         node->pTypeParamValue = pTypeParamValue;  pTypeParamValue = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Icon* getIcon() {return pIcon;}
    TypeParamValue* getTypeParamValue() {return pTypeParamValue;}

    void setIcon(Icon* icon) {pIcon = icon;}
    void setTypeParamValue(TypeParamValue* typeparamvalue) {pTypeParamValue = typeparamvalue;}

 private:
    Icon* pIcon;
    TypeParamValue* pTypeParamValue;
};

class CharLiteralConstant : public Node
{
 public:
    CharLiteralConstant()
      {
         pKindParam = NULL;
         pScon = NULL;
      }
   virtual ~CharLiteralConstant();

    CharLiteralConstant* newCharLiteralConstant()
      {
         CharLiteralConstant* node = new CharLiteralConstant();
         node->pKindParam = pKindParam;  pKindParam = NULL;
         node->pScon = pScon;  pScon = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    KindParam* getKindParam() {return pKindParam;}
    Scon* getScon() {return pScon;}

    void setKindParam(KindParam* kindparam) {pKindParam = kindparam;}
    void setScon(Scon* scon) {pScon = scon;}

 private:
    KindParam* pKindParam;
    Scon* pScon;
};

class LogicalLiteralConstant : public Node
{
 public:
    LogicalLiteralConstant()
      {
         pKindParam = NULL;
      }
   virtual ~LogicalLiteralConstant();

   enum OptionType
     {
        DEFAULT = 0,
        LogicalLiteralConstant_FALSE,
        LogicalLiteralConstant_TRUE
     };

    LogicalLiteralConstant* newLogicalLiteralConstant()
      {
         LogicalLiteralConstant* node = new LogicalLiteralConstant();
         node->pKindParam = pKindParam;  pKindParam = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    KindParam* getKindParam() {return pKindParam;}

    void setKindParam(KindParam* kindparam) {pKindParam = kindparam;}

 private:
    KindParam* pKindParam;
};

class DerivedTypeDef : public Node
{
 public:
    DerivedTypeDef()
      {
         pDerivedTypeStmt = NULL;
         pTypeParamDefStmtList = new std::vector<TypeParamDefStmt*>();
         pPrivateOrSequenceList = new std::vector<PrivateOrSequence*>();
         pComponentPart = NULL;
         pTypeBoundProcedurePart = NULL;
         pEndTypeStmt = NULL;
      }
   virtual ~DerivedTypeDef();

    DerivedTypeDef* newDerivedTypeDef()
      {
         DerivedTypeDef* node = new DerivedTypeDef();
         node->pDerivedTypeStmt = pDerivedTypeStmt;  pDerivedTypeStmt = NULL;
         delete node->pTypeParamDefStmtList; node->pTypeParamDefStmtList = pTypeParamDefStmtList;  pTypeParamDefStmtList = NULL;
         delete node->pPrivateOrSequenceList; node->pPrivateOrSequenceList = pPrivateOrSequenceList;  pPrivateOrSequenceList = NULL;
         node->pComponentPart = pComponentPart;  pComponentPart = NULL;
         node->pTypeBoundProcedurePart = pTypeBoundProcedurePart;  pTypeBoundProcedurePart = NULL;
         node->pEndTypeStmt = pEndTypeStmt;  pEndTypeStmt = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    DerivedTypeStmt* getDerivedTypeStmt() {return pDerivedTypeStmt;}
    std::vector<TypeParamDefStmt*>* getTypeParamDefStmtList() {return pTypeParamDefStmtList;}
    std::vector<PrivateOrSequence*>* getPrivateOrSequenceList() {return pPrivateOrSequenceList;}
    ComponentPart* getComponentPart() {return pComponentPart;}
    TypeBoundProcedurePart* getTypeBoundProcedurePart() {return pTypeBoundProcedurePart;}
    EndTypeStmt* getEndTypeStmt() {return pEndTypeStmt;}

    void setDerivedTypeStmt(DerivedTypeStmt* derivedtypestmt) {pDerivedTypeStmt = derivedtypestmt;}
    void appendTypeParamDefStmt(TypeParamDefStmt* typeparamdefstmt) {pTypeParamDefStmtList->push_back(typeparamdefstmt);}
    void appendPrivateOrSequence(PrivateOrSequence* privateorsequence) {pPrivateOrSequenceList->push_back(privateorsequence);}
    void setComponentPart(ComponentPart* componentpart) {pComponentPart = componentpart;}
    void setTypeBoundProcedurePart(TypeBoundProcedurePart* typeboundprocedurepart) {pTypeBoundProcedurePart = typeboundprocedurepart;}
    void setEndTypeStmt(EndTypeStmt* endtypestmt) {pEndTypeStmt = endtypestmt;}

 private:
    DerivedTypeStmt* pDerivedTypeStmt;
    std::vector<TypeParamDefStmt*>* pTypeParamDefStmtList;
    std::vector<PrivateOrSequence*>* pPrivateOrSequenceList;
    ComponentPart* pComponentPart;
    TypeBoundProcedurePart* pTypeBoundProcedurePart;
    EndTypeStmt* pEndTypeStmt;
};

class DerivedTypeStmt : public Statement
{
 public:
    DerivedTypeStmt()
      {
         pLabel = NULL;
         pOptTypeAttrSpecList = NULL;
         pTypeName = NULL;
         pOptTypeParamNameList = NULL;
         pEOS = NULL;
      }
   virtual ~DerivedTypeStmt();

    DerivedTypeStmt* newDerivedTypeStmt()
      {
         DerivedTypeStmt* node = new DerivedTypeStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pOptTypeAttrSpecList = pOptTypeAttrSpecList;  pOptTypeAttrSpecList = NULL;
         node->pTypeName = pTypeName;  pTypeName = NULL;
         node->pOptTypeParamNameList = pOptTypeParamNameList;  pOptTypeParamNameList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    OptTypeAttrSpecList* getOptTypeAttrSpecList() {return pOptTypeAttrSpecList;}
    Name* getTypeName() {return pTypeName;}
    OptTypeParamNameList* getOptTypeParamNameList() {return pOptTypeParamNameList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setOptTypeAttrSpecList(OptTypeAttrSpecList* opttypeattrspeclist) {pOptTypeAttrSpecList = opttypeattrspeclist;}
    void setTypeName(Name* type_name) {pTypeName = type_name;}
    void setOptTypeParamNameList(OptTypeParamNameList* opttypeparamnamelist) {pOptTypeParamNameList = opttypeparamnamelist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    OptTypeAttrSpecList* pOptTypeAttrSpecList;
    Name* pTypeName;
    OptTypeParamNameList* pOptTypeParamNameList;
    EOS* pEOS;
};

class OptTypeAttrSpecList : public Node
{
 public:
    OptTypeAttrSpecList()
      {
         pTypeAttrSpecList = NULL;
      }
   virtual ~OptTypeAttrSpecList();

    OptTypeAttrSpecList* newOptTypeAttrSpecList()
      {
         OptTypeAttrSpecList* node = new OptTypeAttrSpecList();
         node->pTypeAttrSpecList = pTypeAttrSpecList;  pTypeAttrSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    TypeAttrSpecList* getTypeAttrSpecList() {return pTypeAttrSpecList;}

    void setTypeAttrSpecList(TypeAttrSpecList* typeattrspeclist) {pTypeAttrSpecList = typeattrspeclist;}

 private:
    TypeAttrSpecList* pTypeAttrSpecList;
};

class OptTypeParamNameList : public Node
{
 public:
    OptTypeParamNameList()
      {
         pTypeParamNameList = NULL;
      }
   virtual ~OptTypeParamNameList();

    OptTypeParamNameList* newOptTypeParamNameList()
      {
         OptTypeParamNameList* node = new OptTypeParamNameList();
         node->pTypeParamNameList = pTypeParamNameList;  pTypeParamNameList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    TypeParamNameList* getTypeParamNameList() {return pTypeParamNameList;}

    void setTypeParamNameList(TypeParamNameList* typeparamnamelist) {pTypeParamNameList = typeparamnamelist;}

 private:
    TypeParamNameList* pTypeParamNameList;
};

class TypeAttrSpec : public Node
{
 public:
    TypeAttrSpec()
      {
         pParentTypeName = NULL;
         pAccessSpec = NULL;
      }
   virtual ~TypeAttrSpec();

   enum OptionType
     {
        DEFAULT = 0,
        TypeAttrSpec_EXTENDS,
        TypeAttrSpec_BIND,
        TypeAttrSpec_AS,
        TypeAttrSpec_ABSTRACT
     };

    TypeAttrSpec* newTypeAttrSpec()
      {
         TypeAttrSpec* node = new TypeAttrSpec();
         node->pParentTypeName = pParentTypeName;  pParentTypeName = NULL;
         node->pAccessSpec = pAccessSpec;  pAccessSpec = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Name* getParentTypeName() {return pParentTypeName;}
    AccessSpec* getAccessSpec() {return pAccessSpec;}

    void setParentTypeName(Name* parenttypename) {pParentTypeName = parenttypename;}
    void setAccessSpec(AccessSpec* accessspec) {pAccessSpec = accessspec;}

 private:
    Name* pParentTypeName;
    AccessSpec* pAccessSpec;
};

class TypeAttrSpecList : public Node
{
 public:
    TypeAttrSpecList()
      {
         pTypeAttrSpecList = new std::vector<TypeAttrSpec*>();
      }
   virtual ~TypeAttrSpecList();

    TypeAttrSpecList* newTypeAttrSpecList()
      {
         TypeAttrSpecList* node = new TypeAttrSpecList();
         delete node->pTypeAttrSpecList; node->pTypeAttrSpecList = pTypeAttrSpecList;  pTypeAttrSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<TypeAttrSpec*>* getTypeAttrSpecList() {return pTypeAttrSpecList;}

    void appendTypeAttrSpec(TypeAttrSpec* typeattrspec) {pTypeAttrSpecList->push_back(typeattrspec);}

 private:
    std::vector<TypeAttrSpec*>* pTypeAttrSpecList;
};

class TypeParamNameList : public Node
{
 public:
    TypeParamNameList()
      {
         pTypeParamNameList = new std::vector<Name*>();
      }
   virtual ~TypeParamNameList();

    TypeParamNameList* newTypeParamNameList()
      {
         TypeParamNameList* node = new TypeParamNameList();
         delete node->pTypeParamNameList; node->pTypeParamNameList = pTypeParamNameList;  pTypeParamNameList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<Name*>* getTypeParamNameList() {return pTypeParamNameList;}

    void appendTypeParamName(Name* typeparamname) {pTypeParamNameList->push_back(typeparamname);}

 private:
    std::vector<Name*>* pTypeParamNameList;
};

class PrivateOrSequence : public Node
{
 public:
    PrivateOrSequence()
      {
         pSequenceStmt = NULL;
         pPrivateComponentsStmt = NULL;
      }
   virtual ~PrivateOrSequence();

   enum OptionType
     {
        DEFAULT = 0,
        PrivateOrSequence_SS,
        PrivateOrSequence_PCS
     };

    PrivateOrSequence* newPrivateOrSequence()
      {
         PrivateOrSequence* node = new PrivateOrSequence();
         node->pSequenceStmt = pSequenceStmt;  pSequenceStmt = NULL;
         node->pPrivateComponentsStmt = pPrivateComponentsStmt;  pPrivateComponentsStmt = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    SequenceStmt* getSequenceStmt() {return pSequenceStmt;}
    PrivateComponentsStmt* getPrivateComponentsStmt() {return pPrivateComponentsStmt;}

    void setSequenceStmt(SequenceStmt* sequencestmt) {pSequenceStmt = sequencestmt;}
    void setPrivateComponentsStmt(PrivateComponentsStmt* privatecomponentsstmt) {pPrivateComponentsStmt = privatecomponentsstmt;}

 private:
    SequenceStmt* pSequenceStmt;
    PrivateComponentsStmt* pPrivateComponentsStmt;
};

class EndTypeStmt : public Statement
{
 public:
    EndTypeStmt()
      {
         pLabel = NULL;
         pTypeName = NULL;
         pEOS = NULL;
      }
   virtual ~EndTypeStmt();

    EndTypeStmt* newEndTypeStmt()
      {
         EndTypeStmt* node = new EndTypeStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pTypeName = pTypeName;  pTypeName = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Name* getTypeName() {return pTypeName;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setTypeName(Name* type_name) {pTypeName = type_name;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Name* pTypeName;
    EOS* pEOS;
};

class SequenceStmt : public Statement
{
 public:
    SequenceStmt()
      {
         pLabel = NULL;
         pEOS = NULL;
      }
   virtual ~SequenceStmt();

    SequenceStmt* newSequenceStmt()
      {
         SequenceStmt* node = new SequenceStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    EOS* pEOS;
};

class TypeParamDefStmt : public Statement
{
 public:
    TypeParamDefStmt()
      {
         pLabel = NULL;
         pKindSelector = NULL;
         pTypeParamAttrSpec = NULL;
         pTypeParamDeclList = NULL;
         pEOS = NULL;
      }
   virtual ~TypeParamDefStmt();

    TypeParamDefStmt* newTypeParamDefStmt()
      {
         TypeParamDefStmt* node = new TypeParamDefStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pKindSelector = pKindSelector;  pKindSelector = NULL;
         node->pTypeParamAttrSpec = pTypeParamAttrSpec;  pTypeParamAttrSpec = NULL;
         node->pTypeParamDeclList = pTypeParamDeclList;  pTypeParamDeclList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    KindSelector* getKindSelector() {return pKindSelector;}
    TypeParamAttrSpec* getTypeParamAttrSpec() {return pTypeParamAttrSpec;}
    TypeParamDeclList* getTypeParamDeclList() {return pTypeParamDeclList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setKindSelector(KindSelector* kindselector) {pKindSelector = kindselector;}
    void setTypeParamAttrSpec(TypeParamAttrSpec* typeparamattrspec) {pTypeParamAttrSpec = typeparamattrspec;}
    void setTypeParamDeclList(TypeParamDeclList* typeparamdecllist) {pTypeParamDeclList = typeparamdecllist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    KindSelector* pKindSelector;
    TypeParamAttrSpec* pTypeParamAttrSpec;
    TypeParamDeclList* pTypeParamDeclList;
    EOS* pEOS;
};

class TypeParamDecl : public Node
{
 public:
    TypeParamDecl()
      {
         pTypeParamName = NULL;
         pExpr = NULL;
      }
   virtual ~TypeParamDecl();

    TypeParamDecl* newTypeParamDecl()
      {
         TypeParamDecl* node = new TypeParamDecl();
         node->pTypeParamName = pTypeParamName;  pTypeParamName = NULL;
         node->pExpr = pExpr;  pExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Name* getTypeParamName() {return pTypeParamName;}
    Expr* getExpr() {return pExpr;}

    void setTypeParamName(Name* typeparamname) {pTypeParamName = typeparamname;}
    void setExpr(Expr* expr) {pExpr = expr;}

 private:
    Name* pTypeParamName;
    Expr* pExpr;
};

class TypeParamDeclList : public Node
{
 public:
    TypeParamDeclList()
      {
         pTypeParamDeclList = new std::vector<TypeParamDecl*>();
      }
   virtual ~TypeParamDeclList();

    TypeParamDeclList* newTypeParamDeclList()
      {
         TypeParamDeclList* node = new TypeParamDeclList();
         delete node->pTypeParamDeclList; node->pTypeParamDeclList = pTypeParamDeclList;  pTypeParamDeclList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<TypeParamDecl*>* getTypeParamDeclList() {return pTypeParamDeclList;}

    void appendTypeParamDecl(TypeParamDecl* typeparamdecl) {pTypeParamDeclList->push_back(typeparamdecl);}

 private:
    std::vector<TypeParamDecl*>* pTypeParamDeclList;
};

class TypeParamAttrSpec : public Node
{
 public:
    TypeParamAttrSpec()
      {
      }
   virtual ~TypeParamAttrSpec();

   enum OptionType
     {
        DEFAULT = 0,
        TypeParamAttrSpec_LEN,
        TypeParamAttrSpec_KIND
     };

    TypeParamAttrSpec* newTypeParamAttrSpec()
      {
         TypeParamAttrSpec* node = new TypeParamAttrSpec();
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

 private:
};

class ComponentPart : public Node
{
 public:
    ComponentPart()
      {
         pComponentDefStmtList = new std::vector<ComponentDefStmt*>();
      }
   virtual ~ComponentPart();

    ComponentPart* newComponentPart()
      {
         ComponentPart* node = new ComponentPart();
         delete node->pComponentDefStmtList; node->pComponentDefStmtList = pComponentDefStmtList;  pComponentDefStmtList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<ComponentDefStmt*>* getComponentDefStmtList() {return pComponentDefStmtList;}

    void appendComponentDefStmt(ComponentDefStmt* componentdefstmt) {pComponentDefStmtList->push_back(componentdefstmt);}

 private:
    std::vector<ComponentDefStmt*>* pComponentDefStmtList;
};

class ComponentDefStmt : public Node
{
 public:
    ComponentDefStmt()
      {
         pProcComponentDefStmt = NULL;
         pDataComponentDefStmt = NULL;
      }
   virtual ~ComponentDefStmt();

   enum OptionType
     {
        DEFAULT = 0,
        ComponentDefStmt_PCDS,
        ComponentDefStmt_DCDS
     };

    ComponentDefStmt* newComponentDefStmt()
      {
         ComponentDefStmt* node = new ComponentDefStmt();
         node->pProcComponentDefStmt = pProcComponentDefStmt;  pProcComponentDefStmt = NULL;
         node->pDataComponentDefStmt = pDataComponentDefStmt;  pDataComponentDefStmt = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    ProcComponentDefStmt* getProcComponentDefStmt() {return pProcComponentDefStmt;}
    DataComponentDefStmt* getDataComponentDefStmt() {return pDataComponentDefStmt;}

    void setProcComponentDefStmt(ProcComponentDefStmt* proccomponentdefstmt) {pProcComponentDefStmt = proccomponentdefstmt;}
    void setDataComponentDefStmt(DataComponentDefStmt* datacomponentdefstmt) {pDataComponentDefStmt = datacomponentdefstmt;}

 private:
    ProcComponentDefStmt* pProcComponentDefStmt;
    DataComponentDefStmt* pDataComponentDefStmt;
};

class DataComponentDefStmt : public Statement
{
 public:
    DataComponentDefStmt()
      {
         pLabel = NULL;
         pDeclarationTypeSpec = NULL;
         pOptComponentAttrSpecList = NULL;
         pComponentDeclList = NULL;
         pEOS = NULL;
      }
   virtual ~DataComponentDefStmt();

    DataComponentDefStmt* newDataComponentDefStmt()
      {
         DataComponentDefStmt* node = new DataComponentDefStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pDeclarationTypeSpec = pDeclarationTypeSpec;  pDeclarationTypeSpec = NULL;
         node->pOptComponentAttrSpecList = pOptComponentAttrSpecList;  pOptComponentAttrSpecList = NULL;
         node->pComponentDeclList = pComponentDeclList;  pComponentDeclList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    DeclarationTypeSpec* getDeclarationTypeSpec() {return pDeclarationTypeSpec;}
    OptComponentAttrSpecList* getOptComponentAttrSpecList() {return pOptComponentAttrSpecList;}
    ComponentDeclList* getComponentDeclList() {return pComponentDeclList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setDeclarationTypeSpec(DeclarationTypeSpec* declarationtypespec) {pDeclarationTypeSpec = declarationtypespec;}
    void setOptComponentAttrSpecList(OptComponentAttrSpecList* optcomponentattrspeclist) {pOptComponentAttrSpecList = optcomponentattrspeclist;}
    void setComponentDeclList(ComponentDeclList* componentdecllist) {pComponentDeclList = componentdecllist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    DeclarationTypeSpec* pDeclarationTypeSpec;
    OptComponentAttrSpecList* pOptComponentAttrSpecList;
    ComponentDeclList* pComponentDeclList;
    EOS* pEOS;
};

class OptComponentAttrSpecList : public Node
{
 public:
    OptComponentAttrSpecList()
      {
         pComponentAttrSpecList = NULL;
      }
   virtual ~OptComponentAttrSpecList();

    OptComponentAttrSpecList* newOptComponentAttrSpecList()
      {
         OptComponentAttrSpecList* node = new OptComponentAttrSpecList();
         node->pComponentAttrSpecList = pComponentAttrSpecList;  pComponentAttrSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    ComponentAttrSpecList* getComponentAttrSpecList() {return pComponentAttrSpecList;}

    void setComponentAttrSpecList(ComponentAttrSpecList* componentattrspeclist) {pComponentAttrSpecList = componentattrspeclist;}

 private:
    ComponentAttrSpecList* pComponentAttrSpecList;
};

class ComponentAttrSpec : public Node
{
 public:
    ComponentAttrSpec()
      {
         pComponentArraySpec = NULL;
         pCoarraySpec = NULL;
         pAccessSpec = NULL;
      }
   virtual ~ComponentAttrSpec();

   enum OptionType
     {
        DEFAULT = 0,
        ComponentAttrSpec_POINTER,
        ComponentAttrSpec_DIMENSION,
        ComponentAttrSpec_CONTIGUOUS,
        ComponentAttrSpec_CODIMENSION,
        ComponentAttrSpec_ALLOCATABLE,
        ComponentAttrSpec_AS
     };

    ComponentAttrSpec* newComponentAttrSpec()
      {
         ComponentAttrSpec* node = new ComponentAttrSpec();
         node->pComponentArraySpec = pComponentArraySpec;  pComponentArraySpec = NULL;
         node->pCoarraySpec = pCoarraySpec;  pCoarraySpec = NULL;
         node->pAccessSpec = pAccessSpec;  pAccessSpec = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    ComponentArraySpec* getComponentArraySpec() {return pComponentArraySpec;}
    CoarraySpec* getCoarraySpec() {return pCoarraySpec;}
    AccessSpec* getAccessSpec() {return pAccessSpec;}

    void setComponentArraySpec(ComponentArraySpec* componentarrayspec) {pComponentArraySpec = componentarrayspec;}
    void setCoarraySpec(CoarraySpec* coarrayspec) {pCoarraySpec = coarrayspec;}
    void setAccessSpec(AccessSpec* accessspec) {pAccessSpec = accessspec;}

 private:
    ComponentArraySpec* pComponentArraySpec;
    CoarraySpec* pCoarraySpec;
    AccessSpec* pAccessSpec;
};

class ComponentAttrSpecList : public Node
{
 public:
    ComponentAttrSpecList()
      {
         pComponentAttrSpecList = new std::vector<ComponentAttrSpec*>();
      }
   virtual ~ComponentAttrSpecList();

    ComponentAttrSpecList* newComponentAttrSpecList()
      {
         ComponentAttrSpecList* node = new ComponentAttrSpecList();
         delete node->pComponentAttrSpecList; node->pComponentAttrSpecList = pComponentAttrSpecList;  pComponentAttrSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<ComponentAttrSpec*>* getComponentAttrSpecList() {return pComponentAttrSpecList;}

    void appendComponentAttrSpec(ComponentAttrSpec* componentattrspec) {pComponentAttrSpecList->push_back(componentattrspec);}

 private:
    std::vector<ComponentAttrSpec*>* pComponentAttrSpecList;
};

class ComponentDecl : public Node
{
 public:
    ComponentDecl()
      {
         pComponentName = NULL;
         pComponentArraySpec = NULL;
         pCoarraySpec = NULL;
         pCharLength = NULL;
         pComponentInitialization = NULL;
      }
   virtual ~ComponentDecl();

    ComponentDecl* newComponentDecl()
      {
         ComponentDecl* node = new ComponentDecl();
         node->pComponentName = pComponentName;  pComponentName = NULL;
         node->pComponentArraySpec = pComponentArraySpec;  pComponentArraySpec = NULL;
         node->pCoarraySpec = pCoarraySpec;  pCoarraySpec = NULL;
         node->pCharLength = pCharLength;  pCharLength = NULL;
         node->pComponentInitialization = pComponentInitialization;  pComponentInitialization = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Name* getComponentName() {return pComponentName;}
    ComponentArraySpec* getComponentArraySpec() {return pComponentArraySpec;}
    CoarraySpec* getCoarraySpec() {return pCoarraySpec;}
    CharLength* getCharLength() {return pCharLength;}
    ComponentInitialization* getComponentInitialization() {return pComponentInitialization;}

    void setComponentName(Name* componentname) {pComponentName = componentname;}
    void setComponentArraySpec(ComponentArraySpec* componentarrayspec) {pComponentArraySpec = componentarrayspec;}
    void setCoarraySpec(CoarraySpec* coarrayspec) {pCoarraySpec = coarrayspec;}
    void setCharLength(CharLength* charlength) {pCharLength = charlength;}
    void setComponentInitialization(ComponentInitialization* componentinitialization) {pComponentInitialization = componentinitialization;}

 private:
    Name* pComponentName;
    ComponentArraySpec* pComponentArraySpec;
    CoarraySpec* pCoarraySpec;
    CharLength* pCharLength;
    ComponentInitialization* pComponentInitialization;
};

class ComponentDeclList : public Node
{
 public:
    ComponentDeclList()
      {
         pComponentDeclList = new std::vector<ComponentDecl*>();
      }
   virtual ~ComponentDeclList();

    ComponentDeclList* newComponentDeclList()
      {
         ComponentDeclList* node = new ComponentDeclList();
         delete node->pComponentDeclList; node->pComponentDeclList = pComponentDeclList;  pComponentDeclList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<ComponentDecl*>* getComponentDeclList() {return pComponentDeclList;}

    void appendComponentDecl(ComponentDecl* componentdecl) {pComponentDeclList->push_back(componentdecl);}

 private:
    std::vector<ComponentDecl*>* pComponentDeclList;
};

class ComponentArraySpec : public Node
{
 public:
    ComponentArraySpec()
      {
         pDeferredShapeSpecList = NULL;
         pExplicitShapeSpecList = NULL;
      }
   virtual ~ComponentArraySpec();

   enum OptionType
     {
        DEFAULT = 0,
        ComponentArraySpec_DSSL,
        ComponentArraySpec_ESSL
     };

    ComponentArraySpec* newComponentArraySpec()
      {
         ComponentArraySpec* node = new ComponentArraySpec();
         node->pDeferredShapeSpecList = pDeferredShapeSpecList;  pDeferredShapeSpecList = NULL;
         node->pExplicitShapeSpecList = pExplicitShapeSpecList;  pExplicitShapeSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    DeferredShapeSpecList* getDeferredShapeSpecList() {return pDeferredShapeSpecList;}
    ExplicitShapeSpecList* getExplicitShapeSpecList() {return pExplicitShapeSpecList;}

    void setDeferredShapeSpecList(DeferredShapeSpecList* deferredshapespeclist) {pDeferredShapeSpecList = deferredshapespeclist;}
    void setExplicitShapeSpecList(ExplicitShapeSpecList* explicitshapespeclist) {pExplicitShapeSpecList = explicitshapespeclist;}

 private:
    DeferredShapeSpecList* pDeferredShapeSpecList;
    ExplicitShapeSpecList* pExplicitShapeSpecList;
};

class ProcComponentDefStmt : public Statement
{
 public:
    ProcComponentDefStmt()
      {
         pLabel = NULL;
         pProcInterface = NULL;
         pProcComponentAttrSpecList = NULL;
         pProcDeclList = NULL;
         pEOS = NULL;
      }
   virtual ~ProcComponentDefStmt();

    ProcComponentDefStmt* newProcComponentDefStmt()
      {
         ProcComponentDefStmt* node = new ProcComponentDefStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pProcInterface = pProcInterface;  pProcInterface = NULL;
         node->pProcComponentAttrSpecList = pProcComponentAttrSpecList;  pProcComponentAttrSpecList = NULL;
         node->pProcDeclList = pProcDeclList;  pProcDeclList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    ProcInterface* getProcInterface() {return pProcInterface;}
    ProcComponentAttrSpecList* getProcComponentAttrSpecList() {return pProcComponentAttrSpecList;}
    ProcDeclList* getProcDeclList() {return pProcDeclList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setProcInterface(ProcInterface* procinterface) {pProcInterface = procinterface;}
    void setProcComponentAttrSpecList(ProcComponentAttrSpecList* proccomponentattrspeclist) {pProcComponentAttrSpecList = proccomponentattrspeclist;}
    void setProcDeclList(ProcDeclList* procdecllist) {pProcDeclList = procdecllist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    ProcInterface* pProcInterface;
    ProcComponentAttrSpecList* pProcComponentAttrSpecList;
    ProcDeclList* pProcDeclList;
    EOS* pEOS;
};

class ProcComponentAttrSpec : public Node
{
 public:
    ProcComponentAttrSpec()
      {
         pAccessSpec = NULL;
      }
   virtual ~ProcComponentAttrSpec();

   enum OptionType
     {
        DEFAULT = 0,
        ProcComponentAttrSpec_AS,
        ProcComponentAttrSpec_NOPASS,
        ProcComponentAttrSpec_PASS,
        ProcComponentAttrSpec_POINTER
     };

    ProcComponentAttrSpec* newProcComponentAttrSpec()
      {
         ProcComponentAttrSpec* node = new ProcComponentAttrSpec();
         node->pAccessSpec = pAccessSpec;  pAccessSpec = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    AccessSpec* getAccessSpec() {return pAccessSpec;}

    void setAccessSpec(AccessSpec* accessspec) {pAccessSpec = accessspec;}

 private:
    AccessSpec* pAccessSpec;
};

class ProcComponentAttrSpecList : public Node
{
 public:
    ProcComponentAttrSpecList()
      {
         pProcComponentAttrSpecList = new std::vector<ProcComponentAttrSpec*>();
      }
   virtual ~ProcComponentAttrSpecList();

    ProcComponentAttrSpecList* newProcComponentAttrSpecList()
      {
         ProcComponentAttrSpecList* node = new ProcComponentAttrSpecList();
         delete node->pProcComponentAttrSpecList; node->pProcComponentAttrSpecList = pProcComponentAttrSpecList;  pProcComponentAttrSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<ProcComponentAttrSpec*>* getProcComponentAttrSpecList() {return pProcComponentAttrSpecList;}

    void appendProcComponentAttrSpec(ProcComponentAttrSpec* proccomponentattrspec) {pProcComponentAttrSpecList->push_back(proccomponentattrspec);}

 private:
    std::vector<ProcComponentAttrSpec*>* pProcComponentAttrSpecList;
};

class ComponentInitialization : public Node
{
 public:
    ComponentInitialization()
      {
         pInitialDataTarget = NULL;
         pNullInit = NULL;
         pExpr = NULL;
      }
   virtual ~ComponentInitialization();

   enum OptionType
     {
        DEFAULT = 0,
        ComponentInitialization_IDT,
        ComponentInitialization_NI,
        ComponentInitialization_E
     };

    ComponentInitialization* newComponentInitialization()
      {
         ComponentInitialization* node = new ComponentInitialization();
         node->pInitialDataTarget = pInitialDataTarget;  pInitialDataTarget = NULL;
         node->pNullInit = pNullInit;  pNullInit = NULL;
         node->pExpr = pExpr;  pExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    InitialDataTarget* getInitialDataTarget() {return pInitialDataTarget;}
    NullInit* getNullInit() {return pNullInit;}
    Expr* getExpr() {return pExpr;}

    void setInitialDataTarget(InitialDataTarget* initialdatatarget) {pInitialDataTarget = initialdatatarget;}
    void setNullInit(NullInit* nullinit) {pNullInit = nullinit;}
    void setExpr(Expr* expr) {pExpr = expr;}

 private:
    InitialDataTarget* pInitialDataTarget;
    NullInit* pNullInit;
    Expr* pExpr;
};

class InitialDataTarget : public Node
{
 public:
    InitialDataTarget()
      {
         pDesignator = NULL;
      }
   virtual ~InitialDataTarget();

    InitialDataTarget* newInitialDataTarget()
      {
         InitialDataTarget* node = new InitialDataTarget();
         node->pDesignator = pDesignator;  pDesignator = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Designator* getDesignator() {return pDesignator;}

    void setDesignator(Designator* designator) {pDesignator = designator;}

 private:
    Designator* pDesignator;
};

class PrivateComponentsStmt : public Statement
{
 public:
    PrivateComponentsStmt()
      {
         pLabel = NULL;
         pEOS = NULL;
      }
   virtual ~PrivateComponentsStmt();

    PrivateComponentsStmt* newPrivateComponentsStmt()
      {
         PrivateComponentsStmt* node = new PrivateComponentsStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    EOS* pEOS;
};

class TypeBoundProcedurePart : public Node
{
 public:
    TypeBoundProcedurePart()
      {
         pContainsStmt = NULL;
         pBindingPrivateStmt = NULL;
         pTypeBoundProcBindingList = new std::vector<TypeBoundProcBinding*>();
      }
   virtual ~TypeBoundProcedurePart();

    TypeBoundProcedurePart* newTypeBoundProcedurePart()
      {
         TypeBoundProcedurePart* node = new TypeBoundProcedurePart();
         node->pContainsStmt = pContainsStmt;  pContainsStmt = NULL;
         node->pBindingPrivateStmt = pBindingPrivateStmt;  pBindingPrivateStmt = NULL;
         delete node->pTypeBoundProcBindingList; node->pTypeBoundProcBindingList = pTypeBoundProcBindingList;  pTypeBoundProcBindingList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    ContainsStmt* getContainsStmt() {return pContainsStmt;}
    BindingPrivateStmt* getBindingPrivateStmt() {return pBindingPrivateStmt;}
    std::vector<TypeBoundProcBinding*>* getTypeBoundProcBindingList() {return pTypeBoundProcBindingList;}

    void setContainsStmt(ContainsStmt* containsstmt) {pContainsStmt = containsstmt;}
    void setBindingPrivateStmt(BindingPrivateStmt* bindingprivatestmt) {pBindingPrivateStmt = bindingprivatestmt;}
    void appendTypeBoundProcBinding(TypeBoundProcBinding* typeboundprocbinding) {pTypeBoundProcBindingList->push_back(typeboundprocbinding);}

 private:
    ContainsStmt* pContainsStmt;
    BindingPrivateStmt* pBindingPrivateStmt;
    std::vector<TypeBoundProcBinding*>* pTypeBoundProcBindingList;
};

class BindingPrivateStmt : public Statement
{
 public:
    BindingPrivateStmt()
      {
         pLabel = NULL;
         pEOS = NULL;
      }
   virtual ~BindingPrivateStmt();

    BindingPrivateStmt* newBindingPrivateStmt()
      {
         BindingPrivateStmt* node = new BindingPrivateStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    EOS* pEOS;
};

class TypeBoundProcBinding : public Node
{
 public:
    TypeBoundProcBinding()
      {
         pFinalProcedureStmt = NULL;
         pTypeBoundGenericStmt = NULL;
         pTypeBoundProcedureStmt = NULL;
      }
   virtual ~TypeBoundProcBinding();

   enum OptionType
     {
        DEFAULT = 0,
        TypeBoundProcBinding_FPS,
        TypeBoundProcBinding_TBGS,
        TypeBoundProcBinding_TBPS
     };

    TypeBoundProcBinding* newTypeBoundProcBinding()
      {
         TypeBoundProcBinding* node = new TypeBoundProcBinding();
         node->pFinalProcedureStmt = pFinalProcedureStmt;  pFinalProcedureStmt = NULL;
         node->pTypeBoundGenericStmt = pTypeBoundGenericStmt;  pTypeBoundGenericStmt = NULL;
         node->pTypeBoundProcedureStmt = pTypeBoundProcedureStmt;  pTypeBoundProcedureStmt = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    FinalProcedureStmt* getFinalProcedureStmt() {return pFinalProcedureStmt;}
    TypeBoundGenericStmt* getTypeBoundGenericStmt() {return pTypeBoundGenericStmt;}
    TypeBoundProcedureStmt* getTypeBoundProcedureStmt() {return pTypeBoundProcedureStmt;}

    void setFinalProcedureStmt(FinalProcedureStmt* finalprocedurestmt) {pFinalProcedureStmt = finalprocedurestmt;}
    void setTypeBoundGenericStmt(TypeBoundGenericStmt* typeboundgenericstmt) {pTypeBoundGenericStmt = typeboundgenericstmt;}
    void setTypeBoundProcedureStmt(TypeBoundProcedureStmt* typeboundprocedurestmt) {pTypeBoundProcedureStmt = typeboundprocedurestmt;}

 private:
    FinalProcedureStmt* pFinalProcedureStmt;
    TypeBoundGenericStmt* pTypeBoundGenericStmt;
    TypeBoundProcedureStmt* pTypeBoundProcedureStmt;
};

class TypeBoundProcedureStmt : public Statement
{
 public:
    TypeBoundProcedureStmt()
      {
         pLabel = NULL;
         pInterfaceName = NULL;
         pBindingAttrList = NULL;
         pBindingNameList = NULL;
         pEOS = NULL;
         pTypeBoundProcDeclList = NULL;
      }
   virtual ~TypeBoundProcedureStmt();

   enum OptionType
     {
        DEFAULT = 0,
        TypeBoundProcedureStmt_3,
        TypeBoundProcedureStmt_2,
        TypeBoundProcedureStmt_1
     };

    TypeBoundProcedureStmt* newTypeBoundProcedureStmt()
      {
         TypeBoundProcedureStmt* node = new TypeBoundProcedureStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pInterfaceName = pInterfaceName;  pInterfaceName = NULL;
         node->pBindingAttrList = pBindingAttrList;  pBindingAttrList = NULL;
         node->pBindingNameList = pBindingNameList;  pBindingNameList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->pTypeBoundProcDeclList = pTypeBoundProcDeclList;  pTypeBoundProcDeclList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    InterfaceName* getInterfaceName() {return pInterfaceName;}
    BindingAttrList* getBindingAttrList() {return pBindingAttrList;}
    BindingNameList* getBindingNameList() {return pBindingNameList;}
    EOS* getEOS() {return pEOS;}
    TypeBoundProcDeclList* getTypeBoundProcDeclList() {return pTypeBoundProcDeclList;}

    void setLabel(Label* label) {pLabel = label;}
    void setInterfaceName(InterfaceName* interfacename) {pInterfaceName = interfacename;}
    void setBindingAttrList(BindingAttrList* bindingattrlist) {pBindingAttrList = bindingattrlist;}
    void setBindingNameList(BindingNameList* bindingnamelist) {pBindingNameList = bindingnamelist;}
    void setEOS(EOS* eos) {pEOS = eos;}
    void setTypeBoundProcDeclList(TypeBoundProcDeclList* typeboundprocdecllist) {pTypeBoundProcDeclList = typeboundprocdecllist;}

 private:
    Label* pLabel;
    InterfaceName* pInterfaceName;
    BindingAttrList* pBindingAttrList;
    BindingNameList* pBindingNameList;
    EOS* pEOS;
    TypeBoundProcDeclList* pTypeBoundProcDeclList;
};

class BindingNameList : public Node
{
 public:
    BindingNameList()
      {
         pBindingNameList = new std::vector<Name*>();
      }
   virtual ~BindingNameList();

    BindingNameList* newBindingNameList()
      {
         BindingNameList* node = new BindingNameList();
         delete node->pBindingNameList; node->pBindingNameList = pBindingNameList;  pBindingNameList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<Name*>* getBindingNameList() {return pBindingNameList;}

    void appendBindingName(Name* bindingname) {pBindingNameList->push_back(bindingname);}

 private:
    std::vector<Name*>* pBindingNameList;
};

class BindingAttrList : public Node
{
 public:
    BindingAttrList()
      {
         pBindingAttrList = new std::vector<BindingAttr*>();
      }
   virtual ~BindingAttrList();

    BindingAttrList* newBindingAttrList()
      {
         BindingAttrList* node = new BindingAttrList();
         delete node->pBindingAttrList; node->pBindingAttrList = pBindingAttrList;  pBindingAttrList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<BindingAttr*>* getBindingAttrList() {return pBindingAttrList;}

    void appendBindingAttr(BindingAttr* bindingattr) {pBindingAttrList->push_back(bindingattr);}

 private:
    std::vector<BindingAttr*>* pBindingAttrList;
};

class TypeBoundProcDecl : public Node
{
 public:
    TypeBoundProcDecl()
      {
         pBindingName = NULL;
         pProcedureName = NULL;
      }
   virtual ~TypeBoundProcDecl();

    TypeBoundProcDecl* newTypeBoundProcDecl()
      {
         TypeBoundProcDecl* node = new TypeBoundProcDecl();
         node->pBindingName = pBindingName;  pBindingName = NULL;
         node->pProcedureName = pProcedureName;  pProcedureName = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Name* getBindingName() {return pBindingName;}
    Name* getProcedureName() {return pProcedureName;}

    void setBindingName(Name* bindingname) {pBindingName = bindingname;}
    void setProcedureName(Name* procedurename) {pProcedureName = procedurename;}

 private:
    Name* pBindingName;
    Name* pProcedureName;
};

class TypeBoundProcDeclList : public Node
{
 public:
    TypeBoundProcDeclList()
      {
         pTypeBoundProcDeclList = new std::vector<TypeBoundProcDecl*>();
      }
   virtual ~TypeBoundProcDeclList();

    TypeBoundProcDeclList* newTypeBoundProcDeclList()
      {
         TypeBoundProcDeclList* node = new TypeBoundProcDeclList();
         delete node->pTypeBoundProcDeclList; node->pTypeBoundProcDeclList = pTypeBoundProcDeclList;  pTypeBoundProcDeclList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<TypeBoundProcDecl*>* getTypeBoundProcDeclList() {return pTypeBoundProcDeclList;}

    void appendTypeBoundProcDecl(TypeBoundProcDecl* typeboundprocdecl) {pTypeBoundProcDeclList->push_back(typeboundprocdecl);}

 private:
    std::vector<TypeBoundProcDecl*>* pTypeBoundProcDeclList;
};

class TypeBoundGenericStmt : public Statement
{
 public:
    TypeBoundGenericStmt()
      {
         pLabel = NULL;
         pAccessSpec = NULL;
         pGenericSpec = NULL;
         pBindingNameList = NULL;
         pEOS = NULL;
      }
   virtual ~TypeBoundGenericStmt();

    TypeBoundGenericStmt* newTypeBoundGenericStmt()
      {
         TypeBoundGenericStmt* node = new TypeBoundGenericStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pAccessSpec = pAccessSpec;  pAccessSpec = NULL;
         node->pGenericSpec = pGenericSpec;  pGenericSpec = NULL;
         node->pBindingNameList = pBindingNameList;  pBindingNameList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    AccessSpec* getAccessSpec() {return pAccessSpec;}
    GenericSpec* getGenericSpec() {return pGenericSpec;}
    BindingNameList* getBindingNameList() {return pBindingNameList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setAccessSpec(AccessSpec* accessspec) {pAccessSpec = accessspec;}
    void setGenericSpec(GenericSpec* genericspec) {pGenericSpec = genericspec;}
    void setBindingNameList(BindingNameList* bindingnamelist) {pBindingNameList = bindingnamelist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    AccessSpec* pAccessSpec;
    GenericSpec* pGenericSpec;
    BindingNameList* pBindingNameList;
    EOS* pEOS;
};

class BindingAttr : public Node
{
 public:
    BindingAttr()
      {
         pAccessSpec = NULL;
         pArgName = NULL;
      }
   virtual ~BindingAttr();

   enum OptionType
     {
        DEFAULT = 0,
        BindingAttr_AS,
        BindingAttr_DEFERRED,
        BindingAttr_NON_OVERRIDABLE,
        BindingAttr_NOPASS,
        BindingAttr_PASS
     };

    BindingAttr* newBindingAttr()
      {
         BindingAttr* node = new BindingAttr();
         node->pAccessSpec = pAccessSpec;  pAccessSpec = NULL;
         node->pArgName = pArgName;  pArgName = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    AccessSpec* getAccessSpec() {return pAccessSpec;}
    Name* getArgName() {return pArgName;}

    void setAccessSpec(AccessSpec* accessspec) {pAccessSpec = accessspec;}
    void setArgName(Name* argname) {pArgName = argname;}

 private:
    AccessSpec* pAccessSpec;
    Name* pArgName;
};

class FinalProcedureStmt : public Statement
{
 public:
    FinalProcedureStmt()
      {
         pLabel = NULL;
         pFinalSubroutineNameList = NULL;
         pEOS = NULL;
      }
   virtual ~FinalProcedureStmt();

    FinalProcedureStmt* newFinalProcedureStmt()
      {
         FinalProcedureStmt* node = new FinalProcedureStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pFinalSubroutineNameList = pFinalSubroutineNameList;  pFinalSubroutineNameList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    FinalSubroutineNameList* getFinalSubroutineNameList() {return pFinalSubroutineNameList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setFinalSubroutineNameList(FinalSubroutineNameList* finalsubroutinenamelist) {pFinalSubroutineNameList = finalsubroutinenamelist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    FinalSubroutineNameList* pFinalSubroutineNameList;
    EOS* pEOS;
};

class FinalSubroutineNameList : public Node
{
 public:
    FinalSubroutineNameList()
      {
         pFinalSubroutineNameList = new std::vector<Name*>();
      }
   virtual ~FinalSubroutineNameList();

    FinalSubroutineNameList* newFinalSubroutineNameList()
      {
         FinalSubroutineNameList* node = new FinalSubroutineNameList();
         delete node->pFinalSubroutineNameList; node->pFinalSubroutineNameList = pFinalSubroutineNameList;  pFinalSubroutineNameList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<Name*>* getFinalSubroutineNameList() {return pFinalSubroutineNameList;}

    void appendFinalSubroutineName(Name* finalsubroutinename) {pFinalSubroutineNameList->push_back(finalsubroutinename);}

 private:
    std::vector<Name*>* pFinalSubroutineNameList;
};

class DerivedTypeSpec : public Node
{
 public:
    DerivedTypeSpec()
      {
         pTypeName = NULL;
         pTypeParamSpecList = NULL;
      }
   virtual ~DerivedTypeSpec();

    DerivedTypeSpec* newDerivedTypeSpec()
      {
         DerivedTypeSpec* node = new DerivedTypeSpec();
         node->pTypeName = pTypeName;  pTypeName = NULL;
         node->pTypeParamSpecList = pTypeParamSpecList;  pTypeParamSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Name* getTypeName() {return pTypeName;}
    TypeParamSpecList* getTypeParamSpecList() {return pTypeParamSpecList;}

    void setTypeName(Name* type_name) {pTypeName = type_name;}
    void setTypeParamSpecList(TypeParamSpecList* typeparamspeclist) {pTypeParamSpecList = typeparamspeclist;}

 private:
    Name* pTypeName;
    TypeParamSpecList* pTypeParamSpecList;
};

class TypeParamSpec : public Node
{
 public:
    TypeParamSpec()
      {
         pKeyword = NULL;
         pTypeParamValue = NULL;
      }
   virtual ~TypeParamSpec();

    TypeParamSpec* newTypeParamSpec()
      {
         TypeParamSpec* node = new TypeParamSpec();
         node->pKeyword = pKeyword;  pKeyword = NULL;
         node->pTypeParamValue = pTypeParamValue;  pTypeParamValue = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Keyword* getKeyword() {return pKeyword;}
    TypeParamValue* getTypeParamValue() {return pTypeParamValue;}

    void setKeyword(Keyword* keyword) {pKeyword = keyword;}
    void setTypeParamValue(TypeParamValue* typeparamvalue) {pTypeParamValue = typeparamvalue;}

 private:
    Keyword* pKeyword;
    TypeParamValue* pTypeParamValue;
};

class TypeParamSpecList : public Node
{
 public:
    TypeParamSpecList()
      {
         pTypeParamSpecList = new std::vector<TypeParamSpec*>();
      }
   virtual ~TypeParamSpecList();

    TypeParamSpecList* newTypeParamSpecList()
      {
         TypeParamSpecList* node = new TypeParamSpecList();
         delete node->pTypeParamSpecList; node->pTypeParamSpecList = pTypeParamSpecList;  pTypeParamSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<TypeParamSpec*>* getTypeParamSpecList() {return pTypeParamSpecList;}

    void appendTypeParamSpec(TypeParamSpec* typeparamspec) {pTypeParamSpecList->push_back(typeparamspec);}

 private:
    std::vector<TypeParamSpec*>* pTypeParamSpecList;
};

class StructureConstructor : public Node
{
 public:
    StructureConstructor()
      {
         pDerivedTypeSpec = NULL;
         pComponentSpecList = new std::vector<ComponentSpec*>();
      }
   virtual ~StructureConstructor();

    StructureConstructor* newStructureConstructor()
      {
         StructureConstructor* node = new StructureConstructor();
         node->pDerivedTypeSpec = pDerivedTypeSpec;  pDerivedTypeSpec = NULL;
         delete node->pComponentSpecList; node->pComponentSpecList = pComponentSpecList;  pComponentSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    DerivedTypeSpec* getDerivedTypeSpec() {return pDerivedTypeSpec;}
    std::vector<ComponentSpec*>* getComponentSpecList() {return pComponentSpecList;}

    void setDerivedTypeSpec(DerivedTypeSpec* derivedtypespec) {pDerivedTypeSpec = derivedtypespec;}
    void appendComponentSpec(ComponentSpec* componentspec) {pComponentSpecList->push_back(componentspec);}

 private:
    DerivedTypeSpec* pDerivedTypeSpec;
    std::vector<ComponentSpec*>* pComponentSpecList;
};

class ComponentSpec : public Node
{
 public:
    ComponentSpec()
      {
         pKeyword = NULL;
         pComponentDataSource = NULL;
      }
   virtual ~ComponentSpec();

    ComponentSpec* newComponentSpec()
      {
         ComponentSpec* node = new ComponentSpec();
         node->pKeyword = pKeyword;  pKeyword = NULL;
         node->pComponentDataSource = pComponentDataSource;  pComponentDataSource = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Keyword* getKeyword() {return pKeyword;}
    ComponentDataSource* getComponentDataSource() {return pComponentDataSource;}

    void setKeyword(Keyword* keyword) {pKeyword = keyword;}
    void setComponentDataSource(ComponentDataSource* componentdatasource) {pComponentDataSource = componentdatasource;}

 private:
    Keyword* pKeyword;
    ComponentDataSource* pComponentDataSource;
};

class ComponentDataSource : public Node
{
 public:
    ComponentDataSource()
      {
         pProcTarget = NULL;
         pDataTarget = NULL;
         pExpr = NULL;
      }
   virtual ~ComponentDataSource();

   enum OptionType
     {
        DEFAULT = 0,
        ComponentDataSource_PT,
        ComponentDataSource_DT,
        ComponentDataSource_E
     };

    ComponentDataSource* newComponentDataSource()
      {
         ComponentDataSource* node = new ComponentDataSource();
         node->pProcTarget = pProcTarget;  pProcTarget = NULL;
         node->pDataTarget = pDataTarget;  pDataTarget = NULL;
         node->pExpr = pExpr;  pExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    ProcTarget* getProcTarget() {return pProcTarget;}
    DataTarget* getDataTarget() {return pDataTarget;}
    Expr* getExpr() {return pExpr;}

    void setProcTarget(ProcTarget* proctarget) {pProcTarget = proctarget;}
    void setDataTarget(DataTarget* datatarget) {pDataTarget = datatarget;}
    void setExpr(Expr* expr) {pExpr = expr;}

 private:
    ProcTarget* pProcTarget;
    DataTarget* pDataTarget;
    Expr* pExpr;
};

class EnumDef : public Node
{
 public:
    EnumDef()
      {
         pEnumDefStmt = NULL;
         pEnumeratorDefStmtList = new std::vector<EnumeratorDefStmt*>();
         pEndEnumStmt = NULL;
      }
   virtual ~EnumDef();

    EnumDef* newEnumDef()
      {
         EnumDef* node = new EnumDef();
         node->pEnumDefStmt = pEnumDefStmt;  pEnumDefStmt = NULL;
         delete node->pEnumeratorDefStmtList; node->pEnumeratorDefStmtList = pEnumeratorDefStmtList;  pEnumeratorDefStmtList = NULL;
         node->pEndEnumStmt = pEndEnumStmt;  pEndEnumStmt = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    EnumDefStmt* getEnumDefStmt() {return pEnumDefStmt;}
    std::vector<EnumeratorDefStmt*>* getEnumeratorDefStmtList() {return pEnumeratorDefStmtList;}
    EndEnumStmt* getEndEnumStmt() {return pEndEnumStmt;}

    void setEnumDefStmt(EnumDefStmt* enumdefstmt) {pEnumDefStmt = enumdefstmt;}
    void appendEnumeratorDefStmt(EnumeratorDefStmt* enumeratordefstmt) {pEnumeratorDefStmtList->push_back(enumeratordefstmt);}
    void setEndEnumStmt(EndEnumStmt* endenumstmt) {pEndEnumStmt = endenumstmt;}

 private:
    EnumDefStmt* pEnumDefStmt;
    std::vector<EnumeratorDefStmt*>* pEnumeratorDefStmtList;
    EndEnumStmt* pEndEnumStmt;
};

class EnumDefStmt : public Statement
{
 public:
    EnumDefStmt()
      {
         pLabel = NULL;
         pEOS = NULL;
      }
   virtual ~EnumDefStmt();

    EnumDefStmt* newEnumDefStmt()
      {
         EnumDefStmt* node = new EnumDefStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    EOS* pEOS;
};

class EnumeratorDefStmt : public Statement
{
 public:
    EnumeratorDefStmt()
      {
         pLabel = NULL;
         pEnumeratorList = NULL;
         pEOS = NULL;
      }
   virtual ~EnumeratorDefStmt();

    EnumeratorDefStmt* newEnumeratorDefStmt()
      {
         EnumeratorDefStmt* node = new EnumeratorDefStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pEnumeratorList = pEnumeratorList;  pEnumeratorList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    EnumeratorList* getEnumeratorList() {return pEnumeratorList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setEnumeratorList(EnumeratorList* enumeratorlist) {pEnumeratorList = enumeratorlist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    EnumeratorList* pEnumeratorList;
    EOS* pEOS;
};

class Enumerator : public Node
{
 public:
    Enumerator()
      {
         pNamedConstant = NULL;
         pIntConstantExpr = NULL;
      }
   virtual ~Enumerator();

    Enumerator* newEnumerator()
      {
         Enumerator* node = new Enumerator();
         node->pNamedConstant = pNamedConstant;  pNamedConstant = NULL;
         node->pIntConstantExpr = pIntConstantExpr;  pIntConstantExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    NamedConstant* getNamedConstant() {return pNamedConstant;}
    IntConstantExpr* getIntConstantExpr() {return pIntConstantExpr;}

    void setNamedConstant(NamedConstant* namedconstant) {pNamedConstant = namedconstant;}
    void setIntConstantExpr(IntConstantExpr* intconstantexpr) {pIntConstantExpr = intconstantexpr;}

 private:
    NamedConstant* pNamedConstant;
    IntConstantExpr* pIntConstantExpr;
};

class EnumeratorList : public Node
{
 public:
    EnumeratorList()
      {
         pEnumeratorList = new std::vector<Enumerator*>();
      }
   virtual ~EnumeratorList();

    EnumeratorList* newEnumeratorList()
      {
         EnumeratorList* node = new EnumeratorList();
         delete node->pEnumeratorList; node->pEnumeratorList = pEnumeratorList;  pEnumeratorList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<Enumerator*>* getEnumeratorList() {return pEnumeratorList;}

    void appendEnumerator(Enumerator* enumerator) {pEnumeratorList->push_back(enumerator);}

 private:
    std::vector<Enumerator*>* pEnumeratorList;
};

class EndEnumStmt : public Statement
{
 public:
    EndEnumStmt()
      {
         pLabel = NULL;
         pEOS = NULL;
      }
   virtual ~EndEnumStmt();

    EndEnumStmt* newEndEnumStmt()
      {
         EndEnumStmt* node = new EndEnumStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    EOS* pEOS;
};

class BozLiteralConstant : public Node
{
 public:
    BozLiteralConstant()
      {
         pHexConstant = NULL;
         pOctalConstant = NULL;
         pBinaryConstant = NULL;
      }
   virtual ~BozLiteralConstant();

   enum OptionType
     {
        DEFAULT = 0,
        BozLiteralConstant_HC,
        BozLiteralConstant_OC,
        BozLiteralConstant_BC
     };

    BozLiteralConstant* newBozLiteralConstant()
      {
         BozLiteralConstant* node = new BozLiteralConstant();
         node->pHexConstant = pHexConstant;  pHexConstant = NULL;
         node->pOctalConstant = pOctalConstant;  pOctalConstant = NULL;
         node->pBinaryConstant = pBinaryConstant;  pBinaryConstant = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    HexConstant* getHexConstant() {return pHexConstant;}
    OctalConstant* getOctalConstant() {return pOctalConstant;}
    BinaryConstant* getBinaryConstant() {return pBinaryConstant;}

    void setHexConstant(HexConstant* hexconstant) {pHexConstant = hexconstant;}
    void setOctalConstant(OctalConstant* octalconstant) {pOctalConstant = octalconstant;}
    void setBinaryConstant(BinaryConstant* binaryconstant) {pBinaryConstant = binaryconstant;}

 private:
    HexConstant* pHexConstant;
    OctalConstant* pOctalConstant;
    BinaryConstant* pBinaryConstant;
};

class ArrayConstructor : public Node
{
 public:
    ArrayConstructor()
      {
         pAcSpec = NULL;
      }
   virtual ~ArrayConstructor();

    ArrayConstructor* newArrayConstructor()
      {
         ArrayConstructor* node = new ArrayConstructor();
         node->pAcSpec = pAcSpec;  pAcSpec = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    AcSpec* getAcSpec() {return pAcSpec;}

    void setAcSpec(AcSpec* acspec) {pAcSpec = acspec;}

 private:
    AcSpec* pAcSpec;
};

class AcSpec : public Node
{
 public:
    AcSpec()
      {
         pTypeSpec = NULL;
         pAcValueList = NULL;
      }
   virtual ~AcSpec();

   enum OptionType
     {
        DEFAULT = 0,
        AcSpec_AVL,
        AcSpec_TS
     };

    AcSpec* newAcSpec()
      {
         AcSpec* node = new AcSpec();
         node->pTypeSpec = pTypeSpec;  pTypeSpec = NULL;
         node->pAcValueList = pAcValueList;  pAcValueList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    TypeSpec* getTypeSpec() {return pTypeSpec;}
    AcValueList* getAcValueList() {return pAcValueList;}

    void setTypeSpec(TypeSpec* typespec) {pTypeSpec = typespec;}
    void setAcValueList(AcValueList* acvaluelist) {pAcValueList = acvaluelist;}

 private:
    TypeSpec* pTypeSpec;
    AcValueList* pAcValueList;
};

class AcValue : public Node
{
 public:
    AcValue()
      {
         pAcImpliedDo = NULL;
         pExpr = NULL;
      }
   virtual ~AcValue();

   enum OptionType
     {
        DEFAULT = 0,
        AcValue_AID,
        AcValue_E
     };

    AcValue* newAcValue()
      {
         AcValue* node = new AcValue();
         node->pAcImpliedDo = pAcImpliedDo;  pAcImpliedDo = NULL;
         node->pExpr = pExpr;  pExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    AcImpliedDo* getAcImpliedDo() {return pAcImpliedDo;}
    Expr* getExpr() {return pExpr;}

    void setAcImpliedDo(AcImpliedDo* acimplieddo) {pAcImpliedDo = acimplieddo;}
    void setExpr(Expr* expr) {pExpr = expr;}

 private:
    AcImpliedDo* pAcImpliedDo;
    Expr* pExpr;
};

class AcValueList : public Node
{
 public:
    AcValueList()
      {
         pAcValueList = new std::vector<AcValue*>();
      }
   virtual ~AcValueList();

    AcValueList* newAcValueList()
      {
         AcValueList* node = new AcValueList();
         delete node->pAcValueList; node->pAcValueList = pAcValueList;  pAcValueList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<AcValue*>* getAcValueList() {return pAcValueList;}

    void appendAcValue(AcValue* acvalue) {pAcValueList->push_back(acvalue);}

 private:
    std::vector<AcValue*>* pAcValueList;
};

class AcImpliedDo : public Node
{
 public:
    AcImpliedDo()
      {
         pAcValueList = NULL;
         pAcImpliedDoControl = NULL;
      }
   virtual ~AcImpliedDo();

    AcImpliedDo* newAcImpliedDo()
      {
         AcImpliedDo* node = new AcImpliedDo();
         node->pAcValueList = pAcValueList;  pAcValueList = NULL;
         node->pAcImpliedDoControl = pAcImpliedDoControl;  pAcImpliedDoControl = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    AcValueList* getAcValueList() {return pAcValueList;}
    AcImpliedDoControl* getAcImpliedDoControl() {return pAcImpliedDoControl;}

    void setAcValueList(AcValueList* acvaluelist) {pAcValueList = acvaluelist;}
    void setAcImpliedDoControl(AcImpliedDoControl* acimplieddocontrol) {pAcImpliedDoControl = acimplieddocontrol;}

 private:
    AcValueList* pAcValueList;
    AcImpliedDoControl* pAcImpliedDoControl;
};

class AcImpliedDoControl : public Node
{
 public:
    AcImpliedDoControl()
      {
         pAcDoVariable = NULL;
         pExpr = NULL;
      }
   virtual ~AcImpliedDoControl();

    AcImpliedDoControl* newAcImpliedDoControl()
      {
         AcImpliedDoControl* node = new AcImpliedDoControl();
         node->pAcDoVariable = pAcDoVariable;  pAcDoVariable = NULL;
         node->pExpr = pExpr;  pExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    AcDoVariable* getAcDoVariable() {return pAcDoVariable;}
    Expr* getExpr() {return pExpr;}

    void setAcDoVariable(AcDoVariable* acdovariable) {pAcDoVariable = acdovariable;}
    void setExpr(Expr* expr) {pExpr = expr;}

 private:
    AcDoVariable* pAcDoVariable;
    Expr* pExpr;
};

class AcDoVariable : public Node
{
 public:
    AcDoVariable()
      {
         pDoVariable = NULL;
      }
   virtual ~AcDoVariable();

    AcDoVariable* newAcDoVariable()
      {
         AcDoVariable* node = new AcDoVariable();
         node->pDoVariable = pDoVariable;  pDoVariable = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    DoVariable* getDoVariable() {return pDoVariable;}

    void setDoVariable(DoVariable* dovariable) {pDoVariable = dovariable;}

 private:
    DoVariable* pDoVariable;
};

class TypeDeclarationStmt : public Statement
{
 public:
    TypeDeclarationStmt()
      {
         pLabel = NULL;
         pDeclarationTypeSpec = NULL;
         pAttrSpecList = NULL;
         pEntityDeclList = NULL;
         pEOS = NULL;
      }
   virtual ~TypeDeclarationStmt();

    TypeDeclarationStmt* newTypeDeclarationStmt()
      {
         TypeDeclarationStmt* node = new TypeDeclarationStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pDeclarationTypeSpec = pDeclarationTypeSpec;  pDeclarationTypeSpec = NULL;
         node->pAttrSpecList = pAttrSpecList;  pAttrSpecList = NULL;
         node->pEntityDeclList = pEntityDeclList;  pEntityDeclList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    DeclarationTypeSpec* getDeclarationTypeSpec() {return pDeclarationTypeSpec;}
    AttrSpecList* getAttrSpecList() {return pAttrSpecList;}
    EntityDeclList* getEntityDeclList() {return pEntityDeclList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setDeclarationTypeSpec(DeclarationTypeSpec* declarationtypespec) {pDeclarationTypeSpec = declarationtypespec;}
    void setAttrSpecList(AttrSpecList* attrspeclist) {pAttrSpecList = attrspeclist;}
    void setEntityDeclList(EntityDeclList* entitydecllist) {pEntityDeclList = entitydecllist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    DeclarationTypeSpec* pDeclarationTypeSpec;
    AttrSpecList* pAttrSpecList;
    EntityDeclList* pEntityDeclList;
    EOS* pEOS;
};

#ifdef OBSOLETE
class OptAttrSpecList : public Node
{
 public:
    OptAttrSpecList()
      {
         pAttrSpecList = NULL;
      }
   virtual ~OptAttrSpecList();

    OptAttrSpecList* newOptAttrSpecList()
      {
         OptAttrSpecList* node = new OptAttrSpecList();
         node->pAttrSpecList = pAttrSpecList;  pAttrSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    AttrSpecList* getAttrSpecList() {return pAttrSpecList;}

    void setAttrSpecList(AttrSpecList* attrspeclist) {pAttrSpecList = attrspeclist;}

 private:
    AttrSpecList* pAttrSpecList;
};
#endif

class AttrSpec : public Node
{
 public:
    AttrSpec()
      {
         pLanguageBindingSpec = NULL;
         pIntentSpec = NULL;
         pArraySpec = NULL;
         pCoarraySpec = NULL;
         pAccessSpec = NULL;
      }
   virtual ~AttrSpec();

   enum OptionType
     {
        DEFAULT = 0,
        AttrSpec_VOLATILE,
        AttrSpec_VALUE,
        AttrSpec_TARGET,
        AttrSpec_SAVE,
        AttrSpec_PROTECTED,
        AttrSpec_POINTER,
        AttrSpec_PARAMETER,
        AttrSpec_OPTIONAL,
        AttrSpec_LBS,
        AttrSpec_INTRINSIC,
        AttrSpec_INTENT,
        AttrSpec_EXTERNAL,
        AttrSpec_DIMENSION,
        AttrSpec_CONTIGUOUS,
        AttrSpec_CODIMENSION,
        AttrSpec_ASYNC,
        AttrSpec_ALLOCATABLE,
        AttrSpec_AS
     };

    AttrSpec* newAttrSpec()
      {
         AttrSpec* node = new AttrSpec();
         node->pLanguageBindingSpec = pLanguageBindingSpec;  pLanguageBindingSpec = NULL;
         node->pIntentSpec = pIntentSpec;  pIntentSpec = NULL;
         node->pArraySpec = pArraySpec;  pArraySpec = NULL;
         node->pCoarraySpec = pCoarraySpec;  pCoarraySpec = NULL;
         node->pAccessSpec = pAccessSpec;  pAccessSpec = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    LanguageBindingSpec* getLanguageBindingSpec() {return pLanguageBindingSpec;}
    IntentSpec* getIntentSpec() {return pIntentSpec;}
    ArraySpec* getArraySpec() {return pArraySpec;}
    CoarraySpec* getCoarraySpec() {return pCoarraySpec;}
    AccessSpec* getAccessSpec() {return pAccessSpec;}

    void setLanguageBindingSpec(LanguageBindingSpec* languagebindingspec) {pLanguageBindingSpec = languagebindingspec;}
    void setIntentSpec(IntentSpec* intentspec) {pIntentSpec = intentspec;}
    void setArraySpec(ArraySpec* arrayspec) {pArraySpec = arrayspec;}
    void setCoarraySpec(CoarraySpec* coarrayspec) {pCoarraySpec = coarrayspec;}
    void setAccessSpec(AccessSpec* accessspec) {pAccessSpec = accessspec;}

 private:
    LanguageBindingSpec* pLanguageBindingSpec;
    IntentSpec* pIntentSpec;
    ArraySpec* pArraySpec;
    CoarraySpec* pCoarraySpec;
    AccessSpec* pAccessSpec;
};

class AttrSpecList : public Node
{
 public:
    AttrSpecList()
      {
         pAttrSpecList = new std::vector<AttrSpec*>();
      }
   virtual ~AttrSpecList();

    AttrSpecList* newAttrSpecList()
      {
         AttrSpecList* node = new AttrSpecList();
         delete node->pAttrSpecList; node->pAttrSpecList = pAttrSpecList;  pAttrSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<AttrSpec*>* getAttrSpecList() {return pAttrSpecList;}

    void appendAttrSpec(AttrSpec* attrspec) {pAttrSpecList->push_back(attrspec);}

 private:
    std::vector<AttrSpec*>* pAttrSpecList;
};

class EntityDecl : public Node
{
 public:
    EntityDecl()
      {
         pObjectName = NULL;
         pArraySpec = NULL;
         pCoarraySpec = NULL;
         pCharLength = NULL;
         pInitialization = NULL;
      }
   virtual ~EntityDecl();

    EntityDecl* newEntityDecl()
      {
         EntityDecl* node = new EntityDecl();
         node->pObjectName = pObjectName;  pObjectName = NULL;
         node->pArraySpec = pArraySpec;  pArraySpec = NULL;
         node->pCoarraySpec = pCoarraySpec;  pCoarraySpec = NULL;
         node->pCharLength = pCharLength;  pCharLength = NULL;
         node->pInitialization = pInitialization;  pInitialization = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Name* getObjectName() {return pObjectName;}
    ArraySpec* getArraySpec() {return pArraySpec;}
    CoarraySpec* getCoarraySpec() {return pCoarraySpec;}
    CharLength* getCharLength() {return pCharLength;}
    Initialization* getInitialization() {return pInitialization;}

    void setObjectName(Name* objectname) {pObjectName = objectname;}
    void setArraySpec(ArraySpec* arrayspec) {pArraySpec = arrayspec;}
    void setCoarraySpec(CoarraySpec* coarrayspec) {pCoarraySpec = coarrayspec;}
    void setCharLength(CharLength* charlength) {pCharLength = charlength;}
    void setInitialization(Initialization* initialization) {pInitialization = initialization;}

 private:
    Name* pObjectName;
    ArraySpec* pArraySpec;
    CoarraySpec* pCoarraySpec;
    CharLength* pCharLength;
    Initialization* pInitialization;
};

class EntityDeclList : public Node
{
 public:
    EntityDeclList()
      {
         pEntityDeclList = new std::vector<EntityDecl*>();
      }
   virtual ~EntityDeclList();

    EntityDeclList* newEntityDeclList()
      {
         EntityDeclList* node = new EntityDeclList();
         delete node->pEntityDeclList; node->pEntityDeclList = pEntityDeclList;  pEntityDeclList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<EntityDecl*>* getEntityDeclList() {return pEntityDeclList;}

    void appendEntityDecl(EntityDecl* entitydecl) {pEntityDeclList->push_back(entitydecl);}

 private:
    std::vector<EntityDecl*>* pEntityDeclList;
};

class Initialization : public Node
{
 public:
    Initialization()
      {
         pInitialDataTarget = NULL;
         pNullInit = NULL;
         pConstantExpr = NULL;
      }
   virtual ~Initialization();

   enum OptionType
     {
        DEFAULT = 0,
        Initialization_IDT,
        Initialization_NI,
        Initialization_CE
     };

    Initialization* newInitialization()
      {
         Initialization* node = new Initialization();
         node->pInitialDataTarget = pInitialDataTarget;  pInitialDataTarget = NULL;
         node->pNullInit = pNullInit;  pNullInit = NULL;
         node->pConstantExpr = pConstantExpr;  pConstantExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    InitialDataTarget* getInitialDataTarget() {return pInitialDataTarget;}
    NullInit* getNullInit() {return pNullInit;}
    ConstantExpr* getConstantExpr() {return pConstantExpr;}

    void setInitialDataTarget(InitialDataTarget* initialdatatarget) {pInitialDataTarget = initialdatatarget;}
    void setNullInit(NullInit* nullinit) {pNullInit = nullinit;}
    void setConstantExpr(ConstantExpr* constantexpr) {pConstantExpr = constantexpr;}

 private:
    InitialDataTarget* pInitialDataTarget;
    NullInit* pNullInit;
    ConstantExpr* pConstantExpr;
};

class NullInit : public Node
{
 public:
    NullInit()
      {
         pFunctionReference = NULL;
      }
   virtual ~NullInit();

    NullInit* newNullInit()
      {
         NullInit* node = new NullInit();
         node->pFunctionReference = pFunctionReference;  pFunctionReference = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    FunctionReference* getFunctionReference() {return pFunctionReference;}

    void setFunctionReference(FunctionReference* functionreference) {pFunctionReference = functionreference;}

 private:
    FunctionReference* pFunctionReference;
};

class AccessSpec : public Node
{
 public:
    AccessSpec()
      {
      }
   virtual ~AccessSpec();

   enum OptionType
     {
        DEFAULT = 0,
        AccessSpec_PRIVATE,
        AccessSpec_PUBLIC
     };

    AccessSpec* newAccessSpec()
      {
         AccessSpec* node = new AccessSpec();
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

 private:
};

class LanguageBindingSpec : public Node
{
 public:
    LanguageBindingSpec()
      {
         pScon = NULL;
      }
   virtual ~LanguageBindingSpec();

    LanguageBindingSpec* newLanguageBindingSpec()
      {
         LanguageBindingSpec* node = new LanguageBindingSpec();
         node->pScon = pScon;  pScon = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Scon* getScon() {return pScon;}

    void setScon(Scon* scon) {pScon = scon;}

 private:
    Scon* pScon;
};

class CoarraySpec : public Node
{
 public:
    CoarraySpec()
      {
         pExplicitCoshapeSpec = NULL;
         pDeferredCoshapeSpecList = NULL;
      }
   virtual ~CoarraySpec();

   enum OptionType
     {
        DEFAULT = 0,
        CoarraySpec_ECS,
        CoarraySpec_DCSL
     };

    CoarraySpec* newCoarraySpec()
      {
         CoarraySpec* node = new CoarraySpec();
         node->pExplicitCoshapeSpec = pExplicitCoshapeSpec;  pExplicitCoshapeSpec = NULL;
         node->pDeferredCoshapeSpecList = pDeferredCoshapeSpecList;  pDeferredCoshapeSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    ExplicitCoshapeSpec* getExplicitCoshapeSpec() {return pExplicitCoshapeSpec;}
    DeferredCoshapeSpecList* getDeferredCoshapeSpecList() {return pDeferredCoshapeSpecList;}

    void setExplicitCoshapeSpec(ExplicitCoshapeSpec* explicitcoshapespec) {pExplicitCoshapeSpec = explicitcoshapespec;}
    void setDeferredCoshapeSpecList(DeferredCoshapeSpecList* deferredcoshapespeclist) {pDeferredCoshapeSpecList = deferredcoshapespeclist;}

 private:
    ExplicitCoshapeSpec* pExplicitCoshapeSpec;
    DeferredCoshapeSpecList* pDeferredCoshapeSpecList;
};

class DeferredCoshapeSpec : public Node
{
 public:
    DeferredCoshapeSpec()
      {
      }
   virtual ~DeferredCoshapeSpec();

    DeferredCoshapeSpec* newDeferredCoshapeSpec()
      {
         DeferredCoshapeSpec* node = new DeferredCoshapeSpec();
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

 private:
};

class DeferredCoshapeSpecList : public Node
{
 public:
    DeferredCoshapeSpecList()
      {
         pDeferredCoshapeSpecList = new std::vector<DeferredCoshapeSpec*>();
      }
   virtual ~DeferredCoshapeSpecList();

    DeferredCoshapeSpecList* newDeferredCoshapeSpecList()
      {
         DeferredCoshapeSpecList* node = new DeferredCoshapeSpecList();
         delete node->pDeferredCoshapeSpecList; node->pDeferredCoshapeSpecList = pDeferredCoshapeSpecList;  pDeferredCoshapeSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<DeferredCoshapeSpec*>* getDeferredCoshapeSpecList() {return pDeferredCoshapeSpecList;}

    void appendDeferredCoshapeSpec(DeferredCoshapeSpec* deferredcoshapespec) {pDeferredCoshapeSpecList->push_back(deferredcoshapespec);}

 private:
    std::vector<DeferredCoshapeSpec*>* pDeferredCoshapeSpecList;
};

class ExplicitCoshapeSpec : public Node
{
 public:
    ExplicitCoshapeSpec()
      {
         pCoboundsEntryList = new std::vector<CoboundsEntry*>();
         pLowerCobound = NULL;
      }
   virtual ~ExplicitCoshapeSpec();

    ExplicitCoshapeSpec* newExplicitCoshapeSpec()
      {
         ExplicitCoshapeSpec* node = new ExplicitCoshapeSpec();
         delete node->pCoboundsEntryList; node->pCoboundsEntryList = pCoboundsEntryList;  pCoboundsEntryList = NULL;
         node->pLowerCobound = pLowerCobound;  pLowerCobound = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<CoboundsEntry*>* getCoboundsEntryList() {return pCoboundsEntryList;}
    LowerCobound* getLowerCobound() {return pLowerCobound;}

    void appendCoboundsEntry(CoboundsEntry* coboundsentry) {pCoboundsEntryList->push_back(coboundsentry);}
    void setLowerCobound(LowerCobound* lowercobound) {pLowerCobound = lowercobound;}

 private:
    std::vector<CoboundsEntry*>* pCoboundsEntryList;
    LowerCobound* pLowerCobound;
};

class CoboundsEntry : public Node
{
 public:
    CoboundsEntry()
      {
         pLowerCobound = NULL;
         pUpperCobound = NULL;
      }
   virtual ~CoboundsEntry();

    CoboundsEntry* newCoboundsEntry()
      {
         CoboundsEntry* node = new CoboundsEntry();
         node->pLowerCobound = pLowerCobound;  pLowerCobound = NULL;
         node->pUpperCobound = pUpperCobound;  pUpperCobound = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    LowerCobound* getLowerCobound() {return pLowerCobound;}
    UpperCobound* getUpperCobound() {return pUpperCobound;}

    void setLowerCobound(LowerCobound* lowercobound) {pLowerCobound = lowercobound;}
    void setUpperCobound(UpperCobound* uppercobound) {pUpperCobound = uppercobound;}

 private:
    LowerCobound* pLowerCobound;
    UpperCobound* pUpperCobound;
};

class LowerCobound : public Node
{
 public:
    LowerCobound()
      {
         pSpecificationExpr = NULL;
      }
   virtual ~LowerCobound();

    LowerCobound* newLowerCobound()
      {
         LowerCobound* node = new LowerCobound();
         node->pSpecificationExpr = pSpecificationExpr;  pSpecificationExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    SpecificationExpr* getSpecificationExpr() {return pSpecificationExpr;}

    void setSpecificationExpr(SpecificationExpr* specificationexpr) {pSpecificationExpr = specificationexpr;}

 private:
    SpecificationExpr* pSpecificationExpr;
};

class UpperCobound : public Node
{
 public:
    UpperCobound()
      {
         pSpecificationExpr = NULL;
      }
   virtual ~UpperCobound();

    UpperCobound* newUpperCobound()
      {
         UpperCobound* node = new UpperCobound();
         node->pSpecificationExpr = pSpecificationExpr;  pSpecificationExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    SpecificationExpr* getSpecificationExpr() {return pSpecificationExpr;}

    void setSpecificationExpr(SpecificationExpr* specificationexpr) {pSpecificationExpr = specificationexpr;}

 private:
    SpecificationExpr* pSpecificationExpr;
};

class ArraySpec : public Node
{
 public:
    ArraySpec()
      {
         pAssumedRankSpec = NULL;
         pImpliedShapeSpecList = NULL;
         pAssumedSizeSpec = NULL;
         pDeferredShapeSpecList = NULL;
         pAssumedShapeSpecList = NULL;
         pExplicitShapeSpecList = NULL;
      }
   virtual ~ArraySpec();

   enum OptionType
     {
        DEFAULT = 0,
        ArraySpec_ARS,
        ArraySpec_ISSL,
        ArraySpec_ASS,
        ArraySpec_DSSL,
        ArraySpec_ASSL,
        ArraySpec_ESSL
     };

    ArraySpec* newArraySpec()
      {
         ArraySpec* node = new ArraySpec();
         node->pAssumedRankSpec = pAssumedRankSpec;  pAssumedRankSpec = NULL;
         node->pImpliedShapeSpecList = pImpliedShapeSpecList;  pImpliedShapeSpecList = NULL;
         node->pAssumedSizeSpec = pAssumedSizeSpec;  pAssumedSizeSpec = NULL;
         node->pDeferredShapeSpecList = pDeferredShapeSpecList;  pDeferredShapeSpecList = NULL;
         node->pAssumedShapeSpecList = pAssumedShapeSpecList;  pAssumedShapeSpecList = NULL;
         node->pExplicitShapeSpecList = pExplicitShapeSpecList;  pExplicitShapeSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    AssumedRankSpec* getAssumedRankSpec() {return pAssumedRankSpec;}
    ImpliedShapeSpecList* getImpliedShapeSpecList() {return pImpliedShapeSpecList;}
    AssumedSizeSpec* getAssumedSizeSpec() {return pAssumedSizeSpec;}
    DeferredShapeSpecList* getDeferredShapeSpecList() {return pDeferredShapeSpecList;}
    AssumedShapeSpecList* getAssumedShapeSpecList() {return pAssumedShapeSpecList;}
    ExplicitShapeSpecList* getExplicitShapeSpecList() {return pExplicitShapeSpecList;}

    void setAssumedRankSpec(AssumedRankSpec* assumedrankspec) {pAssumedRankSpec = assumedrankspec;}
    void setImpliedShapeSpecList(ImpliedShapeSpecList* impliedshapespeclist) {pImpliedShapeSpecList = impliedshapespeclist;}
    void setAssumedSizeSpec(AssumedSizeSpec* assumedsizespec) {pAssumedSizeSpec = assumedsizespec;}
    void setDeferredShapeSpecList(DeferredShapeSpecList* deferredshapespeclist) {pDeferredShapeSpecList = deferredshapespeclist;}
    void setAssumedShapeSpecList(AssumedShapeSpecList* assumedshapespeclist) {pAssumedShapeSpecList = assumedshapespeclist;}
    void setExplicitShapeSpecList(ExplicitShapeSpecList* explicitshapespeclist) {pExplicitShapeSpecList = explicitshapespeclist;}

 private:
    AssumedRankSpec* pAssumedRankSpec;
    ImpliedShapeSpecList* pImpliedShapeSpecList;
    AssumedSizeSpec* pAssumedSizeSpec;
    DeferredShapeSpecList* pDeferredShapeSpecList;
    AssumedShapeSpecList* pAssumedShapeSpecList;
    ExplicitShapeSpecList* pExplicitShapeSpecList;
};

class ExplicitShapeSpec : public Node
{
 public:
    ExplicitShapeSpec()
      {
         pLowerBound = NULL;
         pUpperBound = NULL;
      }
   virtual ~ExplicitShapeSpec();

    ExplicitShapeSpec* newExplicitShapeSpec()
      {
         ExplicitShapeSpec* node = new ExplicitShapeSpec();
         node->pLowerBound = pLowerBound;  pLowerBound = NULL;
         node->pUpperBound = pUpperBound;  pUpperBound = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    LowerBound* getLowerBound() {return pLowerBound;}
    UpperBound* getUpperBound() {return pUpperBound;}

    void setLowerBound(LowerBound* lowerbound) {pLowerBound = lowerbound;}
    void setUpperBound(UpperBound* upperbound) {pUpperBound = upperbound;}

 private:
    LowerBound* pLowerBound;
    UpperBound* pUpperBound;
};

class ExplicitShapeSpecList : public Node
{
 public:
    ExplicitShapeSpecList()
      {
         pExplicitShapeSpecList = new std::vector<ExplicitShapeSpec*>();
      }
   virtual ~ExplicitShapeSpecList();

    ExplicitShapeSpecList* newExplicitShapeSpecList()
      {
         ExplicitShapeSpecList* node = new ExplicitShapeSpecList();
         delete node->pExplicitShapeSpecList; node->pExplicitShapeSpecList = pExplicitShapeSpecList;  pExplicitShapeSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<ExplicitShapeSpec*>* getExplicitShapeSpecList() {return pExplicitShapeSpecList;}

    void appendExplicitShapeSpec(ExplicitShapeSpec* explicitshapespec) {pExplicitShapeSpecList->push_back(explicitshapespec);}

 private:
    std::vector<ExplicitShapeSpec*>* pExplicitShapeSpecList;
};

class LowerBound : public Node
{
 public:
    LowerBound()
      {
         pSpecificationExpr = NULL;
      }
   virtual ~LowerBound();

    LowerBound* newLowerBound()
      {
         LowerBound* node = new LowerBound();
         node->pSpecificationExpr = pSpecificationExpr;  pSpecificationExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    SpecificationExpr* getSpecificationExpr() {return pSpecificationExpr;}

    void setSpecificationExpr(SpecificationExpr* specificationexpr) {pSpecificationExpr = specificationexpr;}

 private:
    SpecificationExpr* pSpecificationExpr;
};

class UpperBound : public Node
{
 public:
    UpperBound()
      {
         pSpecificationExpr = NULL;
      }
   virtual ~UpperBound();

    UpperBound* newUpperBound()
      {
         UpperBound* node = new UpperBound();
         node->pSpecificationExpr = pSpecificationExpr;  pSpecificationExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    SpecificationExpr* getSpecificationExpr() {return pSpecificationExpr;}

    void setSpecificationExpr(SpecificationExpr* specificationexpr) {pSpecificationExpr = specificationexpr;}

 private:
    SpecificationExpr* pSpecificationExpr;
};

class AssumedShapeSpec : public Node
{
 public:
    AssumedShapeSpec()
      {
         pLowerBound = NULL;
      }
   virtual ~AssumedShapeSpec();

    AssumedShapeSpec* newAssumedShapeSpec()
      {
         AssumedShapeSpec* node = new AssumedShapeSpec();
         node->pLowerBound = pLowerBound;  pLowerBound = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    LowerBound* getLowerBound() {return pLowerBound;}

    void setLowerBound(LowerBound* lowerbound) {pLowerBound = lowerbound;}

 private:
    LowerBound* pLowerBound;
};

class AssumedShapeSpecList : public Node
{
 public:
    AssumedShapeSpecList()
      {
         pAssumedShapeSpecList = new std::vector<AssumedShapeSpec*>();
      }
   virtual ~AssumedShapeSpecList();

    AssumedShapeSpecList* newAssumedShapeSpecList()
      {
         AssumedShapeSpecList* node = new AssumedShapeSpecList();
         delete node->pAssumedShapeSpecList; node->pAssumedShapeSpecList = pAssumedShapeSpecList;  pAssumedShapeSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<AssumedShapeSpec*>* getAssumedShapeSpecList() {return pAssumedShapeSpecList;}

    void appendAssumedShapeSpec(AssumedShapeSpec* assumedshapespec) {pAssumedShapeSpecList->push_back(assumedshapespec);}

 private:
    std::vector<AssumedShapeSpec*>* pAssumedShapeSpecList;
};

class DeferredShapeSpec : public Node
{
 public:
    DeferredShapeSpec()
      {
      }
   virtual ~DeferredShapeSpec();

    DeferredShapeSpec* newDeferredShapeSpec()
      {
         DeferredShapeSpec* node = new DeferredShapeSpec();
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

 private:
};

class DeferredShapeSpecList : public Node
{
 public:
    DeferredShapeSpecList()
      {
         pDeferredShapeSpecList = new std::vector<DeferredShapeSpec*>();
      }
   virtual ~DeferredShapeSpecList();

    DeferredShapeSpecList* newDeferredShapeSpecList()
      {
         DeferredShapeSpecList* node = new DeferredShapeSpecList();
         delete node->pDeferredShapeSpecList; node->pDeferredShapeSpecList = pDeferredShapeSpecList;  pDeferredShapeSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<DeferredShapeSpec*>* getDeferredShapeSpecList() {return pDeferredShapeSpecList;}

    void appendDeferredShapeSpec(DeferredShapeSpec* deferredshapespec) {pDeferredShapeSpecList->push_back(deferredshapespec);}

 private:
    std::vector<DeferredShapeSpec*>* pDeferredShapeSpecList;
};

class AssumedSizeSpec : public Node
{
 public:
    AssumedSizeSpec()
      {
         pExplicitShapeSpecList = new std::vector<ExplicitShapeSpec*>();
         pLowerBound = NULL;
      }
   virtual ~AssumedSizeSpec();

    AssumedSizeSpec* newAssumedSizeSpec()
      {
         AssumedSizeSpec* node = new AssumedSizeSpec();
         delete node->pExplicitShapeSpecList; node->pExplicitShapeSpecList = pExplicitShapeSpecList;  pExplicitShapeSpecList = NULL;
         node->pLowerBound = pLowerBound;  pLowerBound = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<ExplicitShapeSpec*>* getExplicitShapeSpecList() {return pExplicitShapeSpecList;}
    LowerBound* getLowerBound() {return pLowerBound;}

    void appendExplicitShapeSpec(ExplicitShapeSpec* explicitshapespec) {pExplicitShapeSpecList->push_back(explicitshapespec);}
    void setLowerBound(LowerBound* lowerbound) {pLowerBound = lowerbound;}

 private:
    std::vector<ExplicitShapeSpec*>* pExplicitShapeSpecList;
    LowerBound* pLowerBound;
};

class AssumedRankSpec : public Node
{
 public:
    AssumedRankSpec()
      {
      }
   virtual ~AssumedRankSpec();

    AssumedRankSpec* newAssumedRankSpec()
      {
         AssumedRankSpec* node = new AssumedRankSpec();
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

 private:
};

class ImpliedShapeSpec : public Node
{
 public:
    ImpliedShapeSpec()
      {
         pLowerBound = NULL;
      }
   virtual ~ImpliedShapeSpec();

    ImpliedShapeSpec* newImpliedShapeSpec()
      {
         ImpliedShapeSpec* node = new ImpliedShapeSpec();
         node->pLowerBound = pLowerBound;  pLowerBound = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    LowerBound* getLowerBound() {return pLowerBound;}

    void setLowerBound(LowerBound* lowerbound) {pLowerBound = lowerbound;}

 private:
    LowerBound* pLowerBound;
};

class ImpliedShapeSpecList : public Node
{
 public:
    ImpliedShapeSpecList()
      {
         pImpliedShapeSpecList = new std::vector<ImpliedShapeSpec*>();
      }
   virtual ~ImpliedShapeSpecList();

    ImpliedShapeSpecList* newImpliedShapeSpecList()
      {
         ImpliedShapeSpecList* node = new ImpliedShapeSpecList();
         delete node->pImpliedShapeSpecList; node->pImpliedShapeSpecList = pImpliedShapeSpecList;  pImpliedShapeSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<ImpliedShapeSpec*>* getImpliedShapeSpecList() {return pImpliedShapeSpecList;}

    void appendImpliedShapeSpec(ImpliedShapeSpec* impliedshapespec) {pImpliedShapeSpecList->push_back(impliedshapespec);}

 private:
    std::vector<ImpliedShapeSpec*>* pImpliedShapeSpecList;
};

class IntentSpec : public Node
{
 public:
    IntentSpec()
      {
      }
   virtual ~IntentSpec();

   enum OptionType
     {
        DEFAULT = 0,
        IntentSpec_INOUT,
        IntentSpec_OUT,
        IntentSpec_IN
     };

    IntentSpec* newIntentSpec()
      {
         IntentSpec* node = new IntentSpec();
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

 private:
};

class AccessStmt : public Statement
{
 public:
    AccessStmt()
      {
         pLabel = NULL;
         pAccessSpec = NULL;
         pAccessIdList = NULL;
         pEOS = NULL;
      }
   virtual ~AccessStmt();

   enum OptionType
     {
        DEFAULT = 0,
        AccessStmt_AIL,
        AccessStmt_0
     };

    AccessStmt* newAccessStmt()
      {
         AccessStmt* node = new AccessStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pAccessSpec = pAccessSpec;  pAccessSpec = NULL;
         node->pAccessIdList = pAccessIdList;  pAccessIdList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    AccessSpec* getAccessSpec() {return pAccessSpec;}
    AccessIdList* getAccessIdList() {return pAccessIdList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setAccessSpec(AccessSpec* accessspec) {pAccessSpec = accessspec;}
    void setAccessIdList(AccessIdList* accessidlist) {pAccessIdList = accessidlist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    AccessSpec* pAccessSpec;
    AccessIdList* pAccessIdList;
    EOS* pEOS;
};

class AccessId : public Node
{
 public:
    AccessId()
      {
         pGenericSpec = NULL;
      }
   virtual ~AccessId();

    AccessId* newAccessId()
      {
         AccessId* node = new AccessId();
         node->pGenericSpec = pGenericSpec;  pGenericSpec = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    GenericSpec* getGenericSpec() {return pGenericSpec;}

    void setGenericSpec(GenericSpec* genericspec) {pGenericSpec = genericspec;}

 private:
    GenericSpec* pGenericSpec;
};

class AccessIdList : public Node
{
 public:
    AccessIdList()
      {
         pAccessIdList = new std::vector<AccessId*>();
      }
   virtual ~AccessIdList();

    AccessIdList* newAccessIdList()
      {
         AccessIdList* node = new AccessIdList();
         delete node->pAccessIdList; node->pAccessIdList = pAccessIdList;  pAccessIdList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<AccessId*>* getAccessIdList() {return pAccessIdList;}

    void appendAccessId(AccessId* accessid) {pAccessIdList->push_back(accessid);}

 private:
    std::vector<AccessId*>* pAccessIdList;
};

class AllocatableStmt : public Statement
{
 public:
    AllocatableStmt()
      {
         pLabel = NULL;
         pAllocatableDeclList = NULL;
         pEOS = NULL;
      }
   virtual ~AllocatableStmt();

    AllocatableStmt* newAllocatableStmt()
      {
         AllocatableStmt* node = new AllocatableStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pAllocatableDeclList = pAllocatableDeclList;  pAllocatableDeclList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    AllocatableDeclList* getAllocatableDeclList() {return pAllocatableDeclList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setAllocatableDeclList(AllocatableDeclList* allocatabledecllist) {pAllocatableDeclList = allocatabledecllist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    AllocatableDeclList* pAllocatableDeclList;
    EOS* pEOS;
};

class AllocatableDecl : public Node
{
 public:
    AllocatableDecl()
      {
         pObjectName = NULL;
         pArraySpec = NULL;
         pCoarraySpec = NULL;
      }
   virtual ~AllocatableDecl();

    AllocatableDecl* newAllocatableDecl()
      {
         AllocatableDecl* node = new AllocatableDecl();
         node->pObjectName = pObjectName;  pObjectName = NULL;
         node->pArraySpec = pArraySpec;  pArraySpec = NULL;
         node->pCoarraySpec = pCoarraySpec;  pCoarraySpec = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Name* getObjectName() {return pObjectName;}
    ArraySpec* getArraySpec() {return pArraySpec;}
    CoarraySpec* getCoarraySpec() {return pCoarraySpec;}

    void setObjectName(Name* objectname) {pObjectName = objectname;}
    void setArraySpec(ArraySpec* arrayspec) {pArraySpec = arrayspec;}
    void setCoarraySpec(CoarraySpec* coarrayspec) {pCoarraySpec = coarrayspec;}

 private:
    Name* pObjectName;
    ArraySpec* pArraySpec;
    CoarraySpec* pCoarraySpec;
};

class AllocatableDeclList : public Node
{
 public:
    AllocatableDeclList()
      {
         pAllocatableDeclList = new std::vector<AllocatableDecl*>();
      }
   virtual ~AllocatableDeclList();

    AllocatableDeclList* newAllocatableDeclList()
      {
         AllocatableDeclList* node = new AllocatableDeclList();
         delete node->pAllocatableDeclList; node->pAllocatableDeclList = pAllocatableDeclList;  pAllocatableDeclList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<AllocatableDecl*>* getAllocatableDeclList() {return pAllocatableDeclList;}

    void appendAllocatableDecl(AllocatableDecl* allocatabledecl) {pAllocatableDeclList->push_back(allocatabledecl);}

 private:
    std::vector<AllocatableDecl*>* pAllocatableDeclList;
};

class AsynchronousStmt : public Statement
{
 public:
    AsynchronousStmt()
      {
         pLabel = NULL;
         pObjectNameList = NULL;
         pEOS = NULL;
      }
   virtual ~AsynchronousStmt();

    AsynchronousStmt* newAsynchronousStmt()
      {
         AsynchronousStmt* node = new AsynchronousStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pObjectNameList = pObjectNameList;  pObjectNameList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    ObjectNameList* getObjectNameList() {return pObjectNameList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setObjectNameList(ObjectNameList* objectnamelist) {pObjectNameList = objectnamelist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    ObjectNameList* pObjectNameList;
    EOS* pEOS;
};

class BindStmt : public Statement
{
 public:
    BindStmt()
      {
         pLabel = NULL;
         pLanguageBindingSpec = NULL;
         pBindEntityList = NULL;
         pEOS = NULL;
      }
   virtual ~BindStmt();

    BindStmt* newBindStmt()
      {
         BindStmt* node = new BindStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pLanguageBindingSpec = pLanguageBindingSpec;  pLanguageBindingSpec = NULL;
         node->pBindEntityList = pBindEntityList;  pBindEntityList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    LanguageBindingSpec* getLanguageBindingSpec() {return pLanguageBindingSpec;}
    BindEntityList* getBindEntityList() {return pBindEntityList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setLanguageBindingSpec(LanguageBindingSpec* languagebindingspec) {pLanguageBindingSpec = languagebindingspec;}
    void setBindEntityList(BindEntityList* bindentitylist) {pBindEntityList = bindentitylist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    LanguageBindingSpec* pLanguageBindingSpec;
    BindEntityList* pBindEntityList;
    EOS* pEOS;
};

class BindEntity : public Node
{
 public:
    BindEntity()
      {
         pCommonBlockName = NULL;
         pEntityName = NULL;
      }
   virtual ~BindEntity();

   enum OptionType
     {
        DEFAULT = 0,
        BindEntity_CBN,
        BindEntity_EN
     };

    BindEntity* newBindEntity()
      {
         BindEntity* node = new BindEntity();
         node->pCommonBlockName = pCommonBlockName;  pCommonBlockName = NULL;
         node->pEntityName = pEntityName;  pEntityName = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Name* getCommonBlockName() {return pCommonBlockName;}
    Name* getEntityName() {return pEntityName;}

    void setCommonBlockName(Name* commonblockname) {pCommonBlockName = commonblockname;}
    void setEntityName(Name* entityname) {pEntityName = entityname;}

 private:
    Name* pCommonBlockName;
    Name* pEntityName;
};

class BindEntityList : public Node
{
 public:
    BindEntityList()
      {
         pBindEntityList = new std::vector<BindEntity*>();
      }
   virtual ~BindEntityList();

    BindEntityList* newBindEntityList()
      {
         BindEntityList* node = new BindEntityList();
         delete node->pBindEntityList; node->pBindEntityList = pBindEntityList;  pBindEntityList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<BindEntity*>* getBindEntityList() {return pBindEntityList;}

    void appendBindEntity(BindEntity* bindentity) {pBindEntityList->push_back(bindentity);}

 private:
    std::vector<BindEntity*>* pBindEntityList;
};

class CodimensionStmt : public Statement
{
 public:
    CodimensionStmt()
      {
         pLabel = NULL;
         pCodimensionDeclList = NULL;
         pEOS = NULL;
      }
   virtual ~CodimensionStmt();

    CodimensionStmt* newCodimensionStmt()
      {
         CodimensionStmt* node = new CodimensionStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pCodimensionDeclList = pCodimensionDeclList;  pCodimensionDeclList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    CodimensionDeclList* getCodimensionDeclList() {return pCodimensionDeclList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setCodimensionDeclList(CodimensionDeclList* codimensiondecllist) {pCodimensionDeclList = codimensiondecllist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    CodimensionDeclList* pCodimensionDeclList;
    EOS* pEOS;
};

class CodimensionDecl : public Node
{
 public:
    CodimensionDecl()
      {
         pCoarrayName = NULL;
         pCoarraySpec = NULL;
      }
   virtual ~CodimensionDecl();

    CodimensionDecl* newCodimensionDecl()
      {
         CodimensionDecl* node = new CodimensionDecl();
         node->pCoarrayName = pCoarrayName;  pCoarrayName = NULL;
         node->pCoarraySpec = pCoarraySpec;  pCoarraySpec = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Name* getCoarrayName() {return pCoarrayName;}
    CoarraySpec* getCoarraySpec() {return pCoarraySpec;}

    void setCoarrayName(Name* coarrayname) {pCoarrayName = coarrayname;}
    void setCoarraySpec(CoarraySpec* coarrayspec) {pCoarraySpec = coarrayspec;}

 private:
    Name* pCoarrayName;
    CoarraySpec* pCoarraySpec;
};

class CodimensionDeclList : public Node
{
 public:
    CodimensionDeclList()
      {
         pCodimensionDeclList = new std::vector<CodimensionDecl*>();
      }
   virtual ~CodimensionDeclList();

    CodimensionDeclList* newCodimensionDeclList()
      {
         CodimensionDeclList* node = new CodimensionDeclList();
         delete node->pCodimensionDeclList; node->pCodimensionDeclList = pCodimensionDeclList;  pCodimensionDeclList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<CodimensionDecl*>* getCodimensionDeclList() {return pCodimensionDeclList;}

    void appendCodimensionDecl(CodimensionDecl* codimensiondecl) {pCodimensionDeclList->push_back(codimensiondecl);}

 private:
    std::vector<CodimensionDecl*>* pCodimensionDeclList;
};

class ContiguousStmt : public Statement
{
 public:
    ContiguousStmt()
      {
         pLabel = NULL;
         pObjectNameList = NULL;
         pEOS = NULL;
      }
   virtual ~ContiguousStmt();

    ContiguousStmt* newContiguousStmt()
      {
         ContiguousStmt* node = new ContiguousStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pObjectNameList = pObjectNameList;  pObjectNameList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    ObjectNameList* getObjectNameList() {return pObjectNameList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setObjectNameList(ObjectNameList* objectnamelist) {pObjectNameList = objectnamelist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    ObjectNameList* pObjectNameList;
    EOS* pEOS;
};

class ObjectNameList : public Node
{
 public:
    ObjectNameList()
      {
         pObjectNameList = new std::vector<Name*>();
      }
   virtual ~ObjectNameList();

    ObjectNameList* newObjectNameList()
      {
         ObjectNameList* node = new ObjectNameList();
         delete node->pObjectNameList; node->pObjectNameList = pObjectNameList;  pObjectNameList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<Name*>* getObjectNameList() {return pObjectNameList;}

    void appendObjectName(Name* objectname) {pObjectNameList->push_back(objectname);}

 private:
    std::vector<Name*>* pObjectNameList;
};

class DataStmt : public Statement
{
 public:
    DataStmt()
      {
         pLabel = NULL;
         pDataStmtSetList = NULL;
         pEOS = NULL;
      }
   virtual ~DataStmt();

    DataStmt* newDataStmt()
      {
         DataStmt* node = new DataStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pDataStmtSetList = pDataStmtSetList;  pDataStmtSetList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    DataStmtSetList* getDataStmtSetList() {return pDataStmtSetList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setDataStmtSetList(DataStmtSetList* datastmtsetlist) {pDataStmtSetList = datastmtsetlist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    DataStmtSetList* pDataStmtSetList;
    EOS* pEOS;
};

class DataStmtSet : public Node
{
 public:
    DataStmtSet()
      {
         pDataStmtObjectList = NULL;
         pDataStmtValueList = NULL;
      }
   virtual ~DataStmtSet();

    DataStmtSet* newDataStmtSet()
      {
         DataStmtSet* node = new DataStmtSet();
         node->pDataStmtObjectList = pDataStmtObjectList;  pDataStmtObjectList = NULL;
         node->pDataStmtValueList = pDataStmtValueList;  pDataStmtValueList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    DataStmtObjectList* getDataStmtObjectList() {return pDataStmtObjectList;}
    DataStmtValueList* getDataStmtValueList() {return pDataStmtValueList;}

    void setDataStmtObjectList(DataStmtObjectList* datastmtobjectlist) {pDataStmtObjectList = datastmtobjectlist;}
    void setDataStmtValueList(DataStmtValueList* datastmtvaluelist) {pDataStmtValueList = datastmtvaluelist;}

 private:
    DataStmtObjectList* pDataStmtObjectList;
    DataStmtValueList* pDataStmtValueList;
};

class DataStmtSetList : public Node
{
 public:
    DataStmtSetList()
      {
         pDataStmtSet = NULL;
      }
   virtual ~DataStmtSetList();

   enum OptionType
     {
        DEFAULT = 0,
        DataStmtSetList_2,
        DataStmtSetList_1
     };

    DataStmtSetList* newDataStmtSetList()
      {
         DataStmtSetList* node = new DataStmtSetList();
         node->pDataStmtSet = pDataStmtSet;  pDataStmtSet = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    DataStmtSet* getDataStmtSet() {return pDataStmtSet;}

    void setDataStmtSet(DataStmtSet* datastmtset) {pDataStmtSet = datastmtset;}

 private:
    DataStmtSet* pDataStmtSet;
};

class DataStmtObject : public Node
{
 public:
    DataStmtObject()
      {
         pDataImpliedDo = NULL;
         pVariable = NULL;
      }
   virtual ~DataStmtObject();

   enum OptionType
     {
        DEFAULT = 0,
        DataStmtObject_DID,
        DataStmtObject_V
     };

    DataStmtObject* newDataStmtObject()
      {
         DataStmtObject* node = new DataStmtObject();
         node->pDataImpliedDo = pDataImpliedDo;  pDataImpliedDo = NULL;
         node->pVariable = pVariable;  pVariable = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    DataImpliedDo* getDataImpliedDo() {return pDataImpliedDo;}
    Variable* getVariable() {return pVariable;}

    void setDataImpliedDo(DataImpliedDo* dataimplieddo) {pDataImpliedDo = dataimplieddo;}
    void setVariable(Variable* variable) {pVariable = variable;}

 private:
    DataImpliedDo* pDataImpliedDo;
    Variable* pVariable;
};

class DataStmtObjectList : public Node
{
 public:
    DataStmtObjectList()
      {
         pDataStmtObjectList = new std::vector<DataStmtObject*>();
      }
   virtual ~DataStmtObjectList();

    DataStmtObjectList* newDataStmtObjectList()
      {
         DataStmtObjectList* node = new DataStmtObjectList();
         delete node->pDataStmtObjectList; node->pDataStmtObjectList = pDataStmtObjectList;  pDataStmtObjectList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<DataStmtObject*>* getDataStmtObjectList() {return pDataStmtObjectList;}

    void appendDataStmtObject(DataStmtObject* datastmtobject) {pDataStmtObjectList->push_back(datastmtobject);}

 private:
    std::vector<DataStmtObject*>* pDataStmtObjectList;
};

class DataImpliedDo : public Node
{
 public:
    DataImpliedDo()
      {
         pDataIDoObjectList = NULL;
         pDataIDoVariable = NULL;
         pExpr = NULL;
      }
   virtual ~DataImpliedDo();

    DataImpliedDo* newDataImpliedDo()
      {
         DataImpliedDo* node = new DataImpliedDo();
         node->pDataIDoObjectList = pDataIDoObjectList;  pDataIDoObjectList = NULL;
         node->pDataIDoVariable = pDataIDoVariable;  pDataIDoVariable = NULL;
         node->pExpr = pExpr;  pExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    DataIDoObjectList* getDataIDoObjectList() {return pDataIDoObjectList;}
    DataIDoVariable* getDataIDoVariable() {return pDataIDoVariable;}
    Expr* getExpr() {return pExpr;}

    void setDataIDoObjectList(DataIDoObjectList* dataidoobjectlist) {pDataIDoObjectList = dataidoobjectlist;}
    void setDataIDoVariable(DataIDoVariable* dataidovariable) {pDataIDoVariable = dataidovariable;}
    void setExpr(Expr* expr) {pExpr = expr;}

 private:
    DataIDoObjectList* pDataIDoObjectList;
    DataIDoVariable* pDataIDoVariable;
    Expr* pExpr;
};

class DataIDoObject : public Node
{
 public:
    DataIDoObject()
      {
         pDataImpliedDo = NULL;
         pStructureComponent = NULL;
         pArrayElement = NULL;
      }
   virtual ~DataIDoObject();

   enum OptionType
     {
        DEFAULT = 0,
        DataIDoObject_DID,
        DataIDoObject_SC,
        DataIDoObject_AE
     };

    DataIDoObject* newDataIDoObject()
      {
         DataIDoObject* node = new DataIDoObject();
         node->pDataImpliedDo = pDataImpliedDo;  pDataImpliedDo = NULL;
         node->pStructureComponent = pStructureComponent;  pStructureComponent = NULL;
         node->pArrayElement = pArrayElement;  pArrayElement = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    DataImpliedDo* getDataImpliedDo() {return pDataImpliedDo;}
    StructureComponent* getStructureComponent() {return pStructureComponent;}
    ArrayElement* getArrayElement() {return pArrayElement;}

    void setDataImpliedDo(DataImpliedDo* dataimplieddo) {pDataImpliedDo = dataimplieddo;}
    void setStructureComponent(StructureComponent* structurecomponent) {pStructureComponent = structurecomponent;}
    void setArrayElement(ArrayElement* arrayelement) {pArrayElement = arrayelement;}

 private:
    DataImpliedDo* pDataImpliedDo;
    StructureComponent* pStructureComponent;
    ArrayElement* pArrayElement;
};

class DataIDoObjectList : public Node
{
 public:
    DataIDoObjectList()
      {
         pDataIDoObjectList = new std::vector<DataIDoObject*>();
      }
   virtual ~DataIDoObjectList();

    DataIDoObjectList* newDataIDoObjectList()
      {
         DataIDoObjectList* node = new DataIDoObjectList();
         delete node->pDataIDoObjectList; node->pDataIDoObjectList = pDataIDoObjectList;  pDataIDoObjectList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<DataIDoObject*>* getDataIDoObjectList() {return pDataIDoObjectList;}

    void appendDataIDoObject(DataIDoObject* dataidoobject) {pDataIDoObjectList->push_back(dataidoobject);}

 private:
    std::vector<DataIDoObject*>* pDataIDoObjectList;
};

class DataIDoVariable : public Node
{
 public:
    DataIDoVariable()
      {
         pDoVariable = NULL;
      }
   virtual ~DataIDoVariable();

    DataIDoVariable* newDataIDoVariable()
      {
         DataIDoVariable* node = new DataIDoVariable();
         node->pDoVariable = pDoVariable;  pDoVariable = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    DoVariable* getDoVariable() {return pDoVariable;}

    void setDoVariable(DoVariable* dovariable) {pDoVariable = dovariable;}

 private:
    DoVariable* pDoVariable;
};

class DataStmtValue : public Node
{
 public:
    DataStmtValue()
      {
         pDataStmtRepeat = NULL;
         pDataStmtConstant = NULL;
      }
   virtual ~DataStmtValue();

    DataStmtValue* newDataStmtValue()
      {
         DataStmtValue* node = new DataStmtValue();
         node->pDataStmtRepeat = pDataStmtRepeat;  pDataStmtRepeat = NULL;
         node->pDataStmtConstant = pDataStmtConstant;  pDataStmtConstant = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    DataStmtRepeat* getDataStmtRepeat() {return pDataStmtRepeat;}
    DataStmtConstant* getDataStmtConstant() {return pDataStmtConstant;}

    void setDataStmtRepeat(DataStmtRepeat* datastmtrepeat) {pDataStmtRepeat = datastmtrepeat;}
    void setDataStmtConstant(DataStmtConstant* datastmtconstant) {pDataStmtConstant = datastmtconstant;}

 private:
    DataStmtRepeat* pDataStmtRepeat;
    DataStmtConstant* pDataStmtConstant;
};

class DataStmtValueList : public Node
{
 public:
    DataStmtValueList()
      {
         pDataStmtValueList = new std::vector<DataStmtValue*>();
      }
   virtual ~DataStmtValueList();

    DataStmtValueList* newDataStmtValueList()
      {
         DataStmtValueList* node = new DataStmtValueList();
         delete node->pDataStmtValueList; node->pDataStmtValueList = pDataStmtValueList;  pDataStmtValueList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<DataStmtValue*>* getDataStmtValueList() {return pDataStmtValueList;}

    void appendDataStmtValue(DataStmtValue* datastmtvalue) {pDataStmtValueList->push_back(datastmtvalue);}

 private:
    std::vector<DataStmtValue*>* pDataStmtValueList;
};

class DataStmtRepeat : public Node
{
 public:
    DataStmtRepeat()
      {
         pIntConstantSubobject = NULL;
         pIcon = NULL;
      }
   virtual ~DataStmtRepeat();

   enum OptionType
     {
        DEFAULT = 0,
        DataStmtRepeat_ICS,
        DataStmtRepeat_IC
     };

    DataStmtRepeat* newDataStmtRepeat()
      {
         DataStmtRepeat* node = new DataStmtRepeat();
         node->pIntConstantSubobject = pIntConstantSubobject;  pIntConstantSubobject = NULL;
         node->pIcon = pIcon;  pIcon = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    IntConstantSubobject* getIntConstantSubobject() {return pIntConstantSubobject;}
    Icon* getIcon() {return pIcon;}

    void setIntConstantSubobject(IntConstantSubobject* intconstantsubobject) {pIntConstantSubobject = intconstantsubobject;}
    void setIcon(Icon* icon) {pIcon = icon;}

 private:
    IntConstantSubobject* pIntConstantSubobject;
    Icon* pIcon;
};

class DataStmtConstant : public Node
{
 public:
    DataStmtConstant()
      {
         pStructureConstructor = NULL;
         pInitialDataTarget = NULL;
         pNullInit = NULL;
         pSignedRealLiteralConstant = NULL;
         pSignedIntLiteralConstant = NULL;
         pConstantSubobject = NULL;
         pConstant = NULL;
      }
   virtual ~DataStmtConstant();

   enum OptionType
     {
        DEFAULT = 0,
        DataStmtConstant_SC,
        DataStmtConstant_IDT,
        DataStmtConstant_NI,
        DataStmtConstant_SRLC,
        DataStmtConstant_SILC,
        DataStmtConstant_CS,
        DataStmtConstant_C
     };

    DataStmtConstant* newDataStmtConstant()
      {
         DataStmtConstant* node = new DataStmtConstant();
         node->pStructureConstructor = pStructureConstructor;  pStructureConstructor = NULL;
         node->pInitialDataTarget = pInitialDataTarget;  pInitialDataTarget = NULL;
         node->pNullInit = pNullInit;  pNullInit = NULL;
         node->pSignedRealLiteralConstant = pSignedRealLiteralConstant;  pSignedRealLiteralConstant = NULL;
         node->pSignedIntLiteralConstant = pSignedIntLiteralConstant;  pSignedIntLiteralConstant = NULL;
         node->pConstantSubobject = pConstantSubobject;  pConstantSubobject = NULL;
         node->pConstant = pConstant;  pConstant = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    StructureConstructor* getStructureConstructor() {return pStructureConstructor;}
    InitialDataTarget* getInitialDataTarget() {return pInitialDataTarget;}
    NullInit* getNullInit() {return pNullInit;}
    SignedRealLiteralConstant* getSignedRealLiteralConstant() {return pSignedRealLiteralConstant;}
    SignedIntLiteralConstant* getSignedIntLiteralConstant() {return pSignedIntLiteralConstant;}
    ConstantSubobject* getConstantSubobject() {return pConstantSubobject;}
    Constant* getConstant() {return pConstant;}

    void setStructureConstructor(StructureConstructor* structureconstructor) {pStructureConstructor = structureconstructor;}
    void setInitialDataTarget(InitialDataTarget* initialdatatarget) {pInitialDataTarget = initialdatatarget;}
    void setNullInit(NullInit* nullinit) {pNullInit = nullinit;}
    void setSignedRealLiteralConstant(SignedRealLiteralConstant* signedrealliteralconstant) {pSignedRealLiteralConstant = signedrealliteralconstant;}
    void setSignedIntLiteralConstant(SignedIntLiteralConstant* signedintliteralconstant) {pSignedIntLiteralConstant = signedintliteralconstant;}
    void setConstantSubobject(ConstantSubobject* constantsubobject) {pConstantSubobject = constantsubobject;}
    void setConstant(Constant* constant) {pConstant = constant;}

 private:
    StructureConstructor* pStructureConstructor;
    InitialDataTarget* pInitialDataTarget;
    NullInit* pNullInit;
    SignedRealLiteralConstant* pSignedRealLiteralConstant;
    SignedIntLiteralConstant* pSignedIntLiteralConstant;
    ConstantSubobject* pConstantSubobject;
    Constant* pConstant;
};

class IntConstantSubobject : public Node
{
 public:
    IntConstantSubobject()
      {
         pConstantSubobject = NULL;
      }
   virtual ~IntConstantSubobject();

    IntConstantSubobject* newIntConstantSubobject()
      {
         IntConstantSubobject* node = new IntConstantSubobject();
         node->pConstantSubobject = pConstantSubobject;  pConstantSubobject = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    ConstantSubobject* getConstantSubobject() {return pConstantSubobject;}

    void setConstantSubobject(ConstantSubobject* constantsubobject) {pConstantSubobject = constantsubobject;}

 private:
    ConstantSubobject* pConstantSubobject;
};

class ConstantSubobject : public Node
{
 public:
    ConstantSubobject()
      {
         pDesignator = NULL;
      }
   virtual ~ConstantSubobject();

    ConstantSubobject* newConstantSubobject()
      {
         ConstantSubobject* node = new ConstantSubobject();
         node->pDesignator = pDesignator;  pDesignator = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Designator* getDesignator() {return pDesignator;}

    void setDesignator(Designator* designator) {pDesignator = designator;}

 private:
    Designator* pDesignator;
};

class DimensionStmt : public Statement
{
 public:
    DimensionStmt()
      {
         pLabel = NULL;
         pArrayNameSpecList = NULL;
         pEOS = NULL;
      }
   virtual ~DimensionStmt();

    DimensionStmt* newDimensionStmt()
      {
         DimensionStmt* node = new DimensionStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pArrayNameSpecList = pArrayNameSpecList;  pArrayNameSpecList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    ArrayNameSpecList* getArrayNameSpecList() {return pArrayNameSpecList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setArrayNameSpecList(ArrayNameSpecList* arraynamespeclist) {pArrayNameSpecList = arraynamespeclist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    ArrayNameSpecList* pArrayNameSpecList;
    EOS* pEOS;
};

class ArrayNameSpec : public Node
{
 public:
    ArrayNameSpec()
      {
         pArrayName = NULL;
         pArraySpec = NULL;
      }
   virtual ~ArrayNameSpec();

    ArrayNameSpec* newArrayNameSpec()
      {
         ArrayNameSpec* node = new ArrayNameSpec();
         node->pArrayName = pArrayName;  pArrayName = NULL;
         node->pArraySpec = pArraySpec;  pArraySpec = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Name* getArrayName() {return pArrayName;}
    ArraySpec* getArraySpec() {return pArraySpec;}

    void setArrayName(Name* arrayname) {pArrayName = arrayname;}
    void setArraySpec(ArraySpec* arrayspec) {pArraySpec = arrayspec;}

 private:
    Name* pArrayName;
    ArraySpec* pArraySpec;
};

class ArrayNameSpecList : public Node
{
 public:
    ArrayNameSpecList()
      {
         pArrayNameSpecList = new std::vector<ArrayNameSpec*>();
      }
   virtual ~ArrayNameSpecList();

    ArrayNameSpecList* newArrayNameSpecList()
      {
         ArrayNameSpecList* node = new ArrayNameSpecList();
         delete node->pArrayNameSpecList; node->pArrayNameSpecList = pArrayNameSpecList;  pArrayNameSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<ArrayNameSpec*>* getArrayNameSpecList() {return pArrayNameSpecList;}

    void appendArrayNameSpec(ArrayNameSpec* arraynamespec) {pArrayNameSpecList->push_back(arraynamespec);}

 private:
    std::vector<ArrayNameSpec*>* pArrayNameSpecList;
};

class IntentStmt : public Statement
{
 public:
    IntentStmt()
      {
         pLabel = NULL;
         pIntentSpec = NULL;
         pDummyArgNameList = NULL;
         pEOS = NULL;
      }
   virtual ~IntentStmt();

    IntentStmt* newIntentStmt()
      {
         IntentStmt* node = new IntentStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pIntentSpec = pIntentSpec;  pIntentSpec = NULL;
         node->pDummyArgNameList = pDummyArgNameList;  pDummyArgNameList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    IntentSpec* getIntentSpec() {return pIntentSpec;}
    DummyArgNameList* getDummyArgNameList() {return pDummyArgNameList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setIntentSpec(IntentSpec* intentspec) {pIntentSpec = intentspec;}
    void setDummyArgNameList(DummyArgNameList* dummyargnamelist) {pDummyArgNameList = dummyargnamelist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    IntentSpec* pIntentSpec;
    DummyArgNameList* pDummyArgNameList;
    EOS* pEOS;
};

class DummyArgNameList : public Node
{
 public:
    DummyArgNameList()
      {
         pDummyArgNameList = new std::vector<DummyArgName*>();
      }
   virtual ~DummyArgNameList();

    DummyArgNameList* newDummyArgNameList()
      {
         DummyArgNameList* node = new DummyArgNameList();
         delete node->pDummyArgNameList; node->pDummyArgNameList = pDummyArgNameList;  pDummyArgNameList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<DummyArgName*>* getDummyArgNameList() {return pDummyArgNameList;}

    void appendDummyArgName(DummyArgName* dummyargname) {pDummyArgNameList->push_back(dummyargname);}

 private:
    std::vector<DummyArgName*>* pDummyArgNameList;
};

class OptionalStmt : public Statement
{
 public:
    OptionalStmt()
      {
         pLabel = NULL;
         pDummyArgNameList = NULL;
         pEOS = NULL;
      }
   virtual ~OptionalStmt();

    OptionalStmt* newOptionalStmt()
      {
         OptionalStmt* node = new OptionalStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pDummyArgNameList = pDummyArgNameList;  pDummyArgNameList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    DummyArgNameList* getDummyArgNameList() {return pDummyArgNameList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setDummyArgNameList(DummyArgNameList* dummyargnamelist) {pDummyArgNameList = dummyargnamelist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    DummyArgNameList* pDummyArgNameList;
    EOS* pEOS;
};

class ParameterStmt : public Statement
{
 public:
    ParameterStmt()
      {
         pLabel = NULL;
         pNamedConstantDefList = NULL;
         pEOS = NULL;
      }
   virtual ~ParameterStmt();

    ParameterStmt* newParameterStmt()
      {
         ParameterStmt* node = new ParameterStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pNamedConstantDefList = pNamedConstantDefList;  pNamedConstantDefList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    NamedConstantDefList* getNamedConstantDefList() {return pNamedConstantDefList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setNamedConstantDefList(NamedConstantDefList* namedconstantdeflist) {pNamedConstantDefList = namedconstantdeflist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    NamedConstantDefList* pNamedConstantDefList;
    EOS* pEOS;
};

class NamedConstantDef : public Node
{
 public:
    NamedConstantDef()
      {
         pNamedConstant = NULL;
         pConstantExpr = NULL;
      }
   virtual ~NamedConstantDef();

    NamedConstantDef* newNamedConstantDef()
      {
         NamedConstantDef* node = new NamedConstantDef();
         node->pNamedConstant = pNamedConstant;  pNamedConstant = NULL;
         node->pConstantExpr = pConstantExpr;  pConstantExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    NamedConstant* getNamedConstant() {return pNamedConstant;}
    ConstantExpr* getConstantExpr() {return pConstantExpr;}

    void setNamedConstant(NamedConstant* namedconstant) {pNamedConstant = namedconstant;}
    void setConstantExpr(ConstantExpr* constantexpr) {pConstantExpr = constantexpr;}

 private:
    NamedConstant* pNamedConstant;
    ConstantExpr* pConstantExpr;
};

class NamedConstantDefList : public Node
{
 public:
    NamedConstantDefList()
      {
         pNamedConstantDefList = new std::vector<NamedConstantDef*>();
      }
   virtual ~NamedConstantDefList();

    NamedConstantDefList* newNamedConstantDefList()
      {
         NamedConstantDefList* node = new NamedConstantDefList();
         delete node->pNamedConstantDefList; node->pNamedConstantDefList = pNamedConstantDefList;  pNamedConstantDefList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<NamedConstantDef*>* getNamedConstantDefList() {return pNamedConstantDefList;}

    void appendNamedConstantDef(NamedConstantDef* namedconstantdef) {pNamedConstantDefList->push_back(namedconstantdef);}

 private:
    std::vector<NamedConstantDef*>* pNamedConstantDefList;
};

class PointerStmt : public Statement
{
 public:
    PointerStmt()
      {
         pLabel = NULL;
         pPointerDeclList = NULL;
         pEOS = NULL;
      }
   virtual ~PointerStmt();

    PointerStmt* newPointerStmt()
      {
         PointerStmt* node = new PointerStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pPointerDeclList = pPointerDeclList;  pPointerDeclList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    PointerDeclList* getPointerDeclList() {return pPointerDeclList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setPointerDeclList(PointerDeclList* pointerdecllist) {pPointerDeclList = pointerdecllist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    PointerDeclList* pPointerDeclList;
    EOS* pEOS;
};

class PointerDecl : public Node
{
 public:
    PointerDecl()
      {
         pProcEntityName = NULL;
         pObjectName = NULL;
         pDeferredShapeSpecList = NULL;
      }
   virtual ~PointerDecl();

   enum OptionType
     {
        DEFAULT = 0,
        PointerDecl_PEN,
        PointerDecl_ON
     };

    PointerDecl* newPointerDecl()
      {
         PointerDecl* node = new PointerDecl();
         node->pProcEntityName = pProcEntityName;  pProcEntityName = NULL;
         node->pObjectName = pObjectName;  pObjectName = NULL;
         node->pDeferredShapeSpecList = pDeferredShapeSpecList;  pDeferredShapeSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Name* getProcEntityName() {return pProcEntityName;}
    Name* getObjectName() {return pObjectName;}
    DeferredShapeSpecList* getDeferredShapeSpecList() {return pDeferredShapeSpecList;}

    void setProcEntityName(Name* procentityname) {pProcEntityName = procentityname;}
    void setObjectName(Name* objectname) {pObjectName = objectname;}
    void setDeferredShapeSpecList(DeferredShapeSpecList* deferredshapespeclist) {pDeferredShapeSpecList = deferredshapespeclist;}

 private:
    Name* pProcEntityName;
    Name* pObjectName;
    DeferredShapeSpecList* pDeferredShapeSpecList;
};

class PointerDeclList : public Node
{
 public:
    PointerDeclList()
      {
         pPointerDeclList = new std::vector<PointerDecl*>();
      }
   virtual ~PointerDeclList();

    PointerDeclList* newPointerDeclList()
      {
         PointerDeclList* node = new PointerDeclList();
         delete node->pPointerDeclList; node->pPointerDeclList = pPointerDeclList;  pPointerDeclList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<PointerDecl*>* getPointerDeclList() {return pPointerDeclList;}

    void appendPointerDecl(PointerDecl* pointerdecl) {pPointerDeclList->push_back(pointerdecl);}

 private:
    std::vector<PointerDecl*>* pPointerDeclList;
};

class ProtectedStmt : public Statement
{
 public:
    ProtectedStmt()
      {
         pLabel = NULL;
         pEntityNameList = NULL;
         pEOS = NULL;
      }
   virtual ~ProtectedStmt();

    ProtectedStmt* newProtectedStmt()
      {
         ProtectedStmt* node = new ProtectedStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pEntityNameList = pEntityNameList;  pEntityNameList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    EntityNameList* getEntityNameList() {return pEntityNameList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setEntityNameList(EntityNameList* entitynamelist) {pEntityNameList = entitynamelist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    EntityNameList* pEntityNameList;
    EOS* pEOS;
};

class EntityNameList : public Node
{
 public:
    EntityNameList()
      {
         pEntityNameList = new std::vector<Name*>();
      }
   virtual ~EntityNameList();

    EntityNameList* newEntityNameList()
      {
         EntityNameList* node = new EntityNameList();
         delete node->pEntityNameList; node->pEntityNameList = pEntityNameList;  pEntityNameList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<Name*>* getEntityNameList() {return pEntityNameList;}

    void appendEntityName(Name* entityname) {pEntityNameList->push_back(entityname);}

 private:
    std::vector<Name*>* pEntityNameList;
};

class SaveStmt : public Statement
{
 public:
    SaveStmt()
      {
         pLabel = NULL;
         pEOS = NULL;
         pSavedEntityList = NULL;
      }
   virtual ~SaveStmt();

   enum OptionType
     {
        DEFAULT = 0,
        SaveStmt_0,
        SaveStmt_SEL
     };

    SaveStmt* newSaveStmt()
      {
         SaveStmt* node = new SaveStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->pSavedEntityList = pSavedEntityList;  pSavedEntityList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    EOS* getEOS() {return pEOS;}
    SavedEntityList* getSavedEntityList() {return pSavedEntityList;}

    void setLabel(Label* label) {pLabel = label;}
    void setEOS(EOS* eos) {pEOS = eos;}
    void setSavedEntityList(SavedEntityList* savedentitylist) {pSavedEntityList = savedentitylist;}

 private:
    Label* pLabel;
    EOS* pEOS;
    SavedEntityList* pSavedEntityList;
};

class SavedEntity : public Node
{
 public:
    SavedEntity()
      {
         pCommonBlockName = NULL;
         pObjectName = NULL;
      }
   virtual ~SavedEntity();

   enum OptionType
     {
        DEFAULT = 0,
        SavedEntity_CBN,
        SavedEntity_AMB
     };

    SavedEntity* newSavedEntity()
      {
         SavedEntity* node = new SavedEntity();
         node->pCommonBlockName = pCommonBlockName;  pCommonBlockName = NULL;
         node->pObjectName = pObjectName;  pObjectName = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Name* getCommonBlockName() {return pCommonBlockName;}
    Name* getObjectName() {return pObjectName;}

    void setCommonBlockName(Name* commonblockname) {pCommonBlockName = commonblockname;}
    void setObjectName(Name* objectname) {pObjectName = objectname;}

 private:
    Name* pCommonBlockName;
    Name* pObjectName;
};

class SavedEntityList : public Node
{
 public:
    SavedEntityList()
      {
         pSavedEntityList = new std::vector<SavedEntity*>();
      }
   virtual ~SavedEntityList();

    SavedEntityList* newSavedEntityList()
      {
         SavedEntityList* node = new SavedEntityList();
         delete node->pSavedEntityList; node->pSavedEntityList = pSavedEntityList;  pSavedEntityList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<SavedEntity*>* getSavedEntityList() {return pSavedEntityList;}

    void appendSavedEntity(SavedEntity* savedentity) {pSavedEntityList->push_back(savedentity);}

 private:
    std::vector<SavedEntity*>* pSavedEntityList;
};

class ProcPointerName : public Node
{
 public:
    ProcPointerName()
      {
         pIdent = NULL;
      }
   virtual ~ProcPointerName();

    ProcPointerName* newProcPointerName()
      {
         ProcPointerName* node = new ProcPointerName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class TargetStmt : public Statement
{
 public:
    TargetStmt()
      {
         pLabel = NULL;
         pTargetDeclList = NULL;
         pEOS = NULL;
      }
   virtual ~TargetStmt();

    TargetStmt* newTargetStmt()
      {
         TargetStmt* node = new TargetStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pTargetDeclList = pTargetDeclList;  pTargetDeclList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    TargetDeclList* getTargetDeclList() {return pTargetDeclList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setTargetDeclList(TargetDeclList* targetdecllist) {pTargetDeclList = targetdecllist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    TargetDeclList* pTargetDeclList;
    EOS* pEOS;
};

class TargetDecl : public Node
{
 public:
    TargetDecl()
      {
         pObjectName = NULL;
         pArraySpec = NULL;
         pCoarraySpec = NULL;
      }
   virtual ~TargetDecl();

    TargetDecl* newTargetDecl()
      {
         TargetDecl* node = new TargetDecl();
         node->pObjectName = pObjectName;  pObjectName = NULL;
         node->pArraySpec = pArraySpec;  pArraySpec = NULL;
         node->pCoarraySpec = pCoarraySpec;  pCoarraySpec = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Name* getObjectName() {return pObjectName;}
    ArraySpec* getArraySpec() {return pArraySpec;}
    CoarraySpec* getCoarraySpec() {return pCoarraySpec;}

    void setObjectName(Name* objectname) {pObjectName = objectname;}
    void setArraySpec(ArraySpec* arrayspec) {pArraySpec = arrayspec;}
    void setCoarraySpec(CoarraySpec* coarrayspec) {pCoarraySpec = coarrayspec;}

 private:
    Name* pObjectName;
    ArraySpec* pArraySpec;
    CoarraySpec* pCoarraySpec;
};

class TargetDeclList : public Node
{
 public:
    TargetDeclList()
      {
         pTargetDeclList = new std::vector<TargetDecl*>();
      }
   virtual ~TargetDeclList();

    TargetDeclList* newTargetDeclList()
      {
         TargetDeclList* node = new TargetDeclList();
         delete node->pTargetDeclList; node->pTargetDeclList = pTargetDeclList;  pTargetDeclList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<TargetDecl*>* getTargetDeclList() {return pTargetDeclList;}

    void appendTargetDecl(TargetDecl* targetdecl) {pTargetDeclList->push_back(targetdecl);}

 private:
    std::vector<TargetDecl*>* pTargetDeclList;
};

class ValueStmt : public Statement
{
 public:
    ValueStmt()
      {
         pLabel = NULL;
         pDummyArgNameList = NULL;
         pEOS = NULL;
      }
   virtual ~ValueStmt();

    ValueStmt* newValueStmt()
      {
         ValueStmt* node = new ValueStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pDummyArgNameList = pDummyArgNameList;  pDummyArgNameList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    DummyArgNameList* getDummyArgNameList() {return pDummyArgNameList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setDummyArgNameList(DummyArgNameList* dummyargnamelist) {pDummyArgNameList = dummyargnamelist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    DummyArgNameList* pDummyArgNameList;
    EOS* pEOS;
};

class VolatileStmt : public Statement
{
 public:
    VolatileStmt()
      {
         pLabel = NULL;
         pObjectNameList = NULL;
         pEOS = NULL;
      }
   virtual ~VolatileStmt();

    VolatileStmt* newVolatileStmt()
      {
         VolatileStmt* node = new VolatileStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pObjectNameList = pObjectNameList;  pObjectNameList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    ObjectNameList* getObjectNameList() {return pObjectNameList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setObjectNameList(ObjectNameList* objectnamelist) {pObjectNameList = objectnamelist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    ObjectNameList* pObjectNameList;
    EOS* pEOS;
};

class ImplicitStmt : public Statement
{
 public:
    ImplicitStmt()
      {
         pLabel = NULL;
         pEOS = NULL;
         pImplicitSpecList = NULL;
      }
   virtual ~ImplicitStmt();

   enum OptionType
     {
        DEFAULT = 0,
        ImplicitStmt_NONE,
     };

    ImplicitStmt* newImplicitStmt()
      {
         ImplicitStmt* node = new ImplicitStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->pImplicitSpecList = pImplicitSpecList;  pImplicitSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    EOS* getEOS() {return pEOS;}
    ImplicitSpecList* getImplicitSpecList() {return pImplicitSpecList;}

    void setLabel(Label* label) {pLabel = label;}
    void setEOS(EOS* eos) {pEOS = eos;}
    void setImplicitSpecList(ImplicitSpecList* implicitspeclist) {pImplicitSpecList = implicitspeclist;}

 private:
    Label* pLabel;
    EOS* pEOS;
    ImplicitSpecList* pImplicitSpecList;
};

class ImplicitSpec : public Node
{
 public:
    ImplicitSpec()
      {
         pDeclarationTypeSpec = NULL;
         pLetterSpecList = NULL;
      }
   virtual ~ImplicitSpec();

    ImplicitSpec* newImplicitSpec()
      {
         ImplicitSpec* node = new ImplicitSpec();
         node->pDeclarationTypeSpec = pDeclarationTypeSpec;  pDeclarationTypeSpec = NULL;
         node->pLetterSpecList = pLetterSpecList;  pLetterSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    DeclarationTypeSpec* getDeclarationTypeSpec() {return pDeclarationTypeSpec;}
    LetterSpecList* getLetterSpecList() {return pLetterSpecList;}

    void setDeclarationTypeSpec(DeclarationTypeSpec* declarationtypespec) {pDeclarationTypeSpec = declarationtypespec;}
    void setLetterSpecList(LetterSpecList* letterspeclist) {pLetterSpecList = letterspeclist;}

 private:
    DeclarationTypeSpec* pDeclarationTypeSpec;
    LetterSpecList* pLetterSpecList;
};

class ImplicitSpecList : public Node
{
 public:
    ImplicitSpecList()
      {
         pImplicitSpecList = new std::vector<ImplicitSpec*>();
      }
   virtual ~ImplicitSpecList();

    ImplicitSpecList* newImplicitSpecList()
      {
         ImplicitSpecList* node = new ImplicitSpecList();
         delete node->pImplicitSpecList; node->pImplicitSpecList = pImplicitSpecList;  pImplicitSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<ImplicitSpec*>* getImplicitSpecList() {return pImplicitSpecList;}

    void appendImplicitSpec(ImplicitSpec* implicitspec) {pImplicitSpecList->push_back(implicitspec);}

 private:
    std::vector<ImplicitSpec*>* pImplicitSpecList;
};

class LetterSpec : public Node
{
 public:
    LetterSpec()
      {
         pLetter = NULL;
      }
   virtual ~LetterSpec();

    LetterSpec* newLetterSpec()
      {
         LetterSpec* node = new LetterSpec();
         node->pLetter = pLetter;  pLetter = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Letter* getLetter() {return pLetter;}

    void setLetter(Letter* letter) {pLetter = letter;}

 private:
    Letter* pLetter;
};

class LetterSpecList : public Node
{
 public:
    LetterSpecList()
      {
         pLetterSpecList = new std::vector<LetterSpec*>();
      }
   virtual ~LetterSpecList();

    LetterSpecList* newLetterSpecList()
      {
         LetterSpecList* node = new LetterSpecList();
         delete node->pLetterSpecList; node->pLetterSpecList = pLetterSpecList;  pLetterSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<LetterSpec*>* getLetterSpecList() {return pLetterSpecList;}

    void appendLetterSpec(LetterSpec* letterspec) {pLetterSpecList->push_back(letterspec);}

 private:
    std::vector<LetterSpec*>* pLetterSpecList;
};

class NamelistStmt : public Statement
{
 public:
    NamelistStmt()
      {
         pLabel = NULL;
         pNamelistEntryList = NULL;
         pEOS = NULL;
      }
   virtual ~NamelistStmt();

    NamelistStmt* newNamelistStmt()
      {
         NamelistStmt* node = new NamelistStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pNamelistEntryList = pNamelistEntryList;  pNamelistEntryList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    NamelistEntryList* getNamelistEntryList() {return pNamelistEntryList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setNamelistEntryList(NamelistEntryList* namelistentrylist) {pNamelistEntryList = namelistentrylist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    NamelistEntryList* pNamelistEntryList;
    EOS* pEOS;
};

class NamelistEntry : public Node
{
 public:
    NamelistEntry()
      {
         pNamelistGroupName = NULL;
         pNamelistGroupObjectList = NULL;
      }
   virtual ~NamelistEntry();

    NamelistEntry* newNamelistEntry()
      {
         NamelistEntry* node = new NamelistEntry();
         node->pNamelistGroupName = pNamelistGroupName;  pNamelistGroupName = NULL;
         node->pNamelistGroupObjectList = pNamelistGroupObjectList;  pNamelistGroupObjectList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Name* getNamelistGroupName() {return pNamelistGroupName;}
    NamelistGroupObjectList* getNamelistGroupObjectList() {return pNamelistGroupObjectList;}

    void setNamelistGroupName(Name* namelistgroupname) {pNamelistGroupName = namelistgroupname;}
    void setNamelistGroupObjectList(NamelistGroupObjectList* namelistgroupobjectlist) {pNamelistGroupObjectList = namelistgroupobjectlist;}

 private:
    Name* pNamelistGroupName;
    NamelistGroupObjectList* pNamelistGroupObjectList;
};

class NamelistEntryList : public Node
{
 public:
    NamelistEntryList()
      {
         pNamelistEntry = NULL;
      }
   virtual ~NamelistEntryList();

   enum OptionType
     {
        DEFAULT = 0,
        NamelistEntryList_0,
        NamelistEntryList_NEL
     };

    NamelistEntryList* newNamelistEntryList()
      {
         NamelistEntryList* node = new NamelistEntryList();
         node->pNamelistEntry = pNamelistEntry;  pNamelistEntry = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    NamelistEntry* getNamelistEntry() {return pNamelistEntry;}

    void setNamelistEntry(NamelistEntry* namelistentry) {pNamelistEntry = namelistentry;}

 private:
    NamelistEntry* pNamelistEntry;
};

class NamelistGroupObject : public Node
{
 public:
    NamelistGroupObject()
      {
         pVariableName = NULL;
      }
   virtual ~NamelistGroupObject();

    NamelistGroupObject* newNamelistGroupObject()
      {
         NamelistGroupObject* node = new NamelistGroupObject();
         node->pVariableName = pVariableName;  pVariableName = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    VariableName* getVariableName() {return pVariableName;}

    void setVariableName(VariableName* variablename) {pVariableName = variablename;}

 private:
    VariableName* pVariableName;
};

class NamelistGroupObjectList : public Node
{
 public:
    NamelistGroupObjectList()
      {
         pNamelistGroupObjectList = new std::vector<NamelistGroupObject*>();
      }
   virtual ~NamelistGroupObjectList();

    NamelistGroupObjectList* newNamelistGroupObjectList()
      {
         NamelistGroupObjectList* node = new NamelistGroupObjectList();
         delete node->pNamelistGroupObjectList; node->pNamelistGroupObjectList = pNamelistGroupObjectList;  pNamelistGroupObjectList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<NamelistGroupObject*>* getNamelistGroupObjectList() {return pNamelistGroupObjectList;}

    void appendNamelistGroupObject(NamelistGroupObject* namelistgroupobject) {pNamelistGroupObjectList->push_back(namelistgroupobject);}

 private:
    std::vector<NamelistGroupObject*>* pNamelistGroupObjectList;
};

class EquivalenceStmt : public Statement
{
 public:
    EquivalenceStmt()
      {
         pLabel = NULL;
         pEquivalenceSetList = NULL;
         pEOS = NULL;
      }
   virtual ~EquivalenceStmt();

    EquivalenceStmt* newEquivalenceStmt()
      {
         EquivalenceStmt* node = new EquivalenceStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pEquivalenceSetList = pEquivalenceSetList;  pEquivalenceSetList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    EquivalenceSetList* getEquivalenceSetList() {return pEquivalenceSetList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setEquivalenceSetList(EquivalenceSetList* equivalencesetlist) {pEquivalenceSetList = equivalencesetlist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    EquivalenceSetList* pEquivalenceSetList;
    EOS* pEOS;
};

class EquivalenceSet : public Node
{
 public:
    EquivalenceSet()
      {
         pEquivalenceObject = NULL;
         pEquivalenceObjectList = NULL;
      }
   virtual ~EquivalenceSet();

    EquivalenceSet* newEquivalenceSet()
      {
         EquivalenceSet* node = new EquivalenceSet();
         node->pEquivalenceObject = pEquivalenceObject;  pEquivalenceObject = NULL;
         node->pEquivalenceObjectList = pEquivalenceObjectList;  pEquivalenceObjectList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    EquivalenceObject* getEquivalenceObject() {return pEquivalenceObject;}
    EquivalenceObjectList* getEquivalenceObjectList() {return pEquivalenceObjectList;}

    void setEquivalenceObject(EquivalenceObject* equivalenceobject) {pEquivalenceObject = equivalenceobject;}
    void setEquivalenceObjectList(EquivalenceObjectList* equivalenceobjectlist) {pEquivalenceObjectList = equivalenceobjectlist;}

 private:
    EquivalenceObject* pEquivalenceObject;
    EquivalenceObjectList* pEquivalenceObjectList;
};

class EquivalenceSetList : public Node
{
 public:
    EquivalenceSetList()
      {
         pEquivalenceSetList = new std::vector<EquivalenceSet*>();
      }
   virtual ~EquivalenceSetList();

    EquivalenceSetList* newEquivalenceSetList()
      {
         EquivalenceSetList* node = new EquivalenceSetList();
         delete node->pEquivalenceSetList; node->pEquivalenceSetList = pEquivalenceSetList;  pEquivalenceSetList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<EquivalenceSet*>* getEquivalenceSetList() {return pEquivalenceSetList;}

    void appendEquivalenceSet(EquivalenceSet* equivalenceset) {pEquivalenceSetList->push_back(equivalenceset);}

 private:
    std::vector<EquivalenceSet*>* pEquivalenceSetList;
};

class EquivalenceObject : public Node
{
 public:
    EquivalenceObject()
      {
         pVariable = NULL;
      }
   virtual ~EquivalenceObject();

    EquivalenceObject* newEquivalenceObject()
      {
         EquivalenceObject* node = new EquivalenceObject();
         node->pVariable = pVariable;  pVariable = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Variable* getVariable() {return pVariable;}

    void setVariable(Variable* variable) {pVariable = variable;}

 private:
    Variable* pVariable;
};

class EquivalenceObjectList : public Node
{
 public:
    EquivalenceObjectList()
      {
         pEquivalenceObjectList = new std::vector<EquivalenceObject*>();
      }
   virtual ~EquivalenceObjectList();

    EquivalenceObjectList* newEquivalenceObjectList()
      {
         EquivalenceObjectList* node = new EquivalenceObjectList();
         delete node->pEquivalenceObjectList; node->pEquivalenceObjectList = pEquivalenceObjectList;  pEquivalenceObjectList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<EquivalenceObject*>* getEquivalenceObjectList() {return pEquivalenceObjectList;}

    void appendEquivalenceObject(EquivalenceObject* equivalenceobject) {pEquivalenceObjectList->push_back(equivalenceobject);}

 private:
    std::vector<EquivalenceObject*>* pEquivalenceObjectList;
};

class CommonStmt : public Statement
{
 public:
    CommonStmt()
      {
         pLabel = NULL;
         pCommonBlockEntry1 = NULL;
         pCommonBlockEntryList = new std::vector<CommonBlockEntry*>();
         pEOS = NULL;
      }
   virtual ~CommonStmt();

    CommonStmt* newCommonStmt()
      {
         CommonStmt* node = new CommonStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pCommonBlockEntry1 = pCommonBlockEntry1;  pCommonBlockEntry1 = NULL;
         delete node->pCommonBlockEntryList; node->pCommonBlockEntryList = pCommonBlockEntryList;  pCommonBlockEntryList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    CommonBlockEntry1* getCommonBlockEntry1() {return pCommonBlockEntry1;}
    std::vector<CommonBlockEntry*>* getCommonBlockEntryList() {return pCommonBlockEntryList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setCommonBlockEntry1(CommonBlockEntry1* commonblockentry1) {pCommonBlockEntry1 = commonblockentry1;}
    void appendCommonBlockEntry(CommonBlockEntry* commonblockentry) {pCommonBlockEntryList->push_back(commonblockentry);}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    CommonBlockEntry1* pCommonBlockEntry1;
    std::vector<CommonBlockEntry*>* pCommonBlockEntryList;
    EOS* pEOS;
};

class CommonBlockEntry1 : public Node
{
 public:
    CommonBlockEntry1()
      {
         pCommonBlockObjectList = NULL;
         pCommonBlockName = NULL;
      }
   virtual ~CommonBlockEntry1();

   enum OptionType
     {
        DEFAULT = 0,
        CBE1,
        CBE1_CBN
     };

    CommonBlockEntry1* newCommonBlockEntry1()
      {
         CommonBlockEntry1* node = new CommonBlockEntry1();
         node->pCommonBlockObjectList = pCommonBlockObjectList;  pCommonBlockObjectList = NULL;
         node->pCommonBlockName = pCommonBlockName;  pCommonBlockName = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    CommonBlockObjectList* getCommonBlockObjectList() {return pCommonBlockObjectList;}
    Name* getCommonBlockName() {return pCommonBlockName;}

    void setCommonBlockObjectList(CommonBlockObjectList* commonblockobjectlist) {pCommonBlockObjectList = commonblockobjectlist;}
    void setCommonBlockName(Name* commonblockname) {pCommonBlockName = commonblockname;}

 private:
    CommonBlockObjectList* pCommonBlockObjectList;
    Name* pCommonBlockName;
};

class CommonBlockEntry : public Node
{
 public:
    CommonBlockEntry()
      {
         pCommonBlockName = NULL;
         pCommonBlockObjectList = NULL;
      }
   virtual ~CommonBlockEntry();

    CommonBlockEntry* newCommonBlockEntry()
      {
         CommonBlockEntry* node = new CommonBlockEntry();
         node->pCommonBlockName = pCommonBlockName;  pCommonBlockName = NULL;
         node->pCommonBlockObjectList = pCommonBlockObjectList;  pCommonBlockObjectList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Name* getCommonBlockName() {return pCommonBlockName;}
    CommonBlockObjectList* getCommonBlockObjectList() {return pCommonBlockObjectList;}

    void setCommonBlockName(Name* commonblockname) {pCommonBlockName = commonblockname;}
    void setCommonBlockObjectList(CommonBlockObjectList* commonblockobjectlist) {pCommonBlockObjectList = commonblockobjectlist;}

 private:
    Name* pCommonBlockName;
    CommonBlockObjectList* pCommonBlockObjectList;
};

class CommonBlockObject : public Node
{
 public:
    CommonBlockObject()
      {
         pVariableName = NULL;
         pArraySpec = NULL;
      }
   virtual ~CommonBlockObject();

    CommonBlockObject* newCommonBlockObject()
      {
         CommonBlockObject* node = new CommonBlockObject();
         node->pVariableName = pVariableName;  pVariableName = NULL;
         node->pArraySpec = pArraySpec;  pArraySpec = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    VariableName* getVariableName() {return pVariableName;}
    ArraySpec* getArraySpec() {return pArraySpec;}

    void setVariableName(VariableName* variablename) {pVariableName = variablename;}
    void setArraySpec(ArraySpec* arrayspec) {pArraySpec = arrayspec;}

 private:
    VariableName* pVariableName;
    ArraySpec* pArraySpec;
};

class CommonBlockObjectList : public Node
{
 public:
    CommonBlockObjectList()
      {
         pCommonBlockObjectList = new std::vector<CommonBlockObject*>();
      }
   virtual ~CommonBlockObjectList();

    CommonBlockObjectList* newCommonBlockObjectList()
      {
         CommonBlockObjectList* node = new CommonBlockObjectList();
         delete node->pCommonBlockObjectList; node->pCommonBlockObjectList = pCommonBlockObjectList;  pCommonBlockObjectList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<CommonBlockObject*>* getCommonBlockObjectList() {return pCommonBlockObjectList;}

    void appendCommonBlockObject(CommonBlockObject* commonblockobject) {pCommonBlockObjectList->push_back(commonblockobject);}

 private:
    std::vector<CommonBlockObject*>* pCommonBlockObjectList;
};

class Designator : public Node
{
 public:
    Designator()
      {
         pSubstring = NULL;
         pDataRef = NULL;
      }
   virtual ~Designator();

   enum OptionType
     {
        DEFAULT = 0,
        Designator_S_AMB,
        DataRef_ot
     };

    Designator* newDesignator()
      {
         Designator* node = new Designator();
         node->pSubstring = pSubstring;  pSubstring = NULL;
         node->pDataRef = pDataRef;  pDataRef = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Substring* getSubstring() {return pSubstring;}
    DataRef* getDataRef() {return pDataRef;}

    void setSubstring(Substring* substring) {pSubstring = substring;}
    void setDataRef(DataRef* dataref) {pDataRef = dataref;}

 private:
    Substring* pSubstring;
    DataRef* pDataRef;
};

class Variable : public Node
{
 public:
    Variable()
      {
         pDesignator = NULL;
      }
   virtual ~Variable();

    Variable* newVariable()
      {
         Variable* node = new Variable();
         node->pDesignator = pDesignator;  pDesignator = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Designator* getDesignator() {return pDesignator;}

    void setDesignator(Designator* designator) {pDesignator = designator;}

 private:
    Designator* pDesignator;
};

class VariableName : public Node
{
 public:
    VariableName()
      {
         pIdent = NULL;
      }
   virtual ~VariableName();

    VariableName* newVariableName()
      {
         VariableName* node = new VariableName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class LogicalVariable : public Node
{
 public:
    LogicalVariable()
      {
         pVariable = NULL;
      }
   virtual ~LogicalVariable();

    LogicalVariable* newLogicalVariable()
      {
         LogicalVariable* node = new LogicalVariable();
         node->pVariable = pVariable;  pVariable = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Variable* getVariable() {return pVariable;}

    void setVariable(Variable* variable) {pVariable = variable;}

 private:
    Variable* pVariable;
};

class CharVariable : public Node
{
 public:
    CharVariable()
      {
         pVariable = NULL;
      }
   virtual ~CharVariable();

    CharVariable* newCharVariable()
      {
         CharVariable* node = new CharVariable();
         node->pVariable = pVariable;  pVariable = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Variable* getVariable() {return pVariable;}

    void setVariable(Variable* variable) {pVariable = variable;}

 private:
    Variable* pVariable;
};

class DefaultCharVariable : public Node
{
 public:
    DefaultCharVariable()
      {
         pVariable = NULL;
      }
   virtual ~DefaultCharVariable();

    DefaultCharVariable* newDefaultCharVariable()
      {
         DefaultCharVariable* node = new DefaultCharVariable();
         node->pVariable = pVariable;  pVariable = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Variable* getVariable() {return pVariable;}

    void setVariable(Variable* variable) {pVariable = variable;}

 private:
    Variable* pVariable;
};

class IntVariable : public Node
{
 public:
    IntVariable()
      {
         pVariable = NULL;
      }
   virtual ~IntVariable();

    IntVariable* newIntVariable()
      {
         IntVariable* node = new IntVariable();
         node->pVariable = pVariable;  pVariable = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Variable* getVariable() {return pVariable;}

    void setVariable(Variable* variable) {pVariable = variable;}

 private:
    Variable* pVariable;
};

class Substring : public Node
{
 public:
    Substring()
      {
         pParentString = NULL;
         pSubstringRange = NULL;
      }
   virtual ~Substring();

    Substring* newSubstring()
      {
         Substring* node = new Substring();
         node->pParentString = pParentString;  pParentString = NULL;
         node->pSubstringRange = pSubstringRange;  pSubstringRange = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    ParentString* getParentString() {return pParentString;}
    SubstringRange* getSubstringRange() {return pSubstringRange;}

    void setParentString(ParentString* parentstring) {pParentString = parentstring;}
    void setSubstringRange(SubstringRange* substringrange) {pSubstringRange = substringrange;}

 private:
    ParentString* pParentString;
    SubstringRange* pSubstringRange;
};

class ParentString : public Node
{
 public:
    ParentString()
      {
         pConstant = NULL;
         pStructureComponent = NULL;
         pCoindexedNamedObject = NULL;
         pArrayElement = NULL;
         pScalarVariableName = NULL;
      }
   virtual ~ParentString();

   enum OptionType
     {
        DEFAULT = 0,
        ParentString_C,
        ParentString_SC,
        ParentString_CNO,
        ParentString_AE,
        ParentString_SVN
     };

    ParentString* newParentString()
      {
         ParentString* node = new ParentString();
         node->pConstant = pConstant;  pConstant = NULL;
         node->pStructureComponent = pStructureComponent;  pStructureComponent = NULL;
         node->pCoindexedNamedObject = pCoindexedNamedObject;  pCoindexedNamedObject = NULL;
         node->pArrayElement = pArrayElement;  pArrayElement = NULL;
         node->pScalarVariableName = pScalarVariableName;  pScalarVariableName = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Constant* getConstant() {return pConstant;}
    StructureComponent* getStructureComponent() {return pStructureComponent;}
    CoindexedNamedObject* getCoindexedNamedObject() {return pCoindexedNamedObject;}
    ArrayElement* getArrayElement() {return pArrayElement;}
    Name* getScalarVariableName() {return pScalarVariableName;}

    void setConstant(Constant* constant) {pConstant = constant;}
    void setStructureComponent(StructureComponent* structurecomponent) {pStructureComponent = structurecomponent;}
    void setCoindexedNamedObject(CoindexedNamedObject* coindexednamedobject) {pCoindexedNamedObject = coindexednamedobject;}
    void setArrayElement(ArrayElement* arrayelement) {pArrayElement = arrayelement;}
    void setScalarVariableName(Name* scalarvariablename) {pScalarVariableName = scalarvariablename;}

 private:
    Constant* pConstant;
    StructureComponent* pStructureComponent;
    CoindexedNamedObject* pCoindexedNamedObject;
    ArrayElement* pArrayElement;
    Name* pScalarVariableName;
};

class SubstringRange : public Node
{
 public:
    SubstringRange()
      {
         pIntExpr = NULL;
      }
   virtual ~SubstringRange();

    SubstringRange* newSubstringRange()
      {
         SubstringRange* node = new SubstringRange();
         node->pIntExpr = pIntExpr;  pIntExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    IntExpr* getIntExpr() {return pIntExpr;}

    void setIntExpr(IntExpr* intexpr) {pIntExpr = intexpr;}

 private:
    IntExpr* pIntExpr;
};

class DataRef : public Node
{
 public:
    DataRef()
      {
         pPartRefList = new std::vector<PartRef*>();
      }
   virtual ~DataRef();

    DataRef* newDataRef()
      {
         DataRef* node = new DataRef();
         delete node->pPartRefList; node->pPartRefList = pPartRefList;  pPartRefList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<PartRef*>* getPartRefList() {return pPartRefList;}

    void appendPartRef(PartRef* partref) {pPartRefList->push_back(partref);}

 private:
    std::vector<PartRef*>* pPartRefList;
};

class PartRef : public Node
{
 public:
    PartRef()
      {
         pPartName = NULL;
         pSectionSubscriptList = NULL;
         pImageSelector = NULL;
      }
   virtual ~PartRef();

    PartRef* newPartRef()
      {
         PartRef* node = new PartRef();
         node->pPartName = pPartName;  pPartName = NULL;
         node->pSectionSubscriptList = pSectionSubscriptList;  pSectionSubscriptList = NULL;
         node->pImageSelector = pImageSelector;  pImageSelector = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Name* getPartName() {return pPartName;}
    SectionSubscriptList* getSectionSubscriptList() {return pSectionSubscriptList;}
    ImageSelector* getImageSelector() {return pImageSelector;}

    void setPartName(Name* partname) {pPartName = partname;}
    void setSectionSubscriptList(SectionSubscriptList* sectionsubscriptlist) {pSectionSubscriptList = sectionsubscriptlist;}
    void setImageSelector(ImageSelector* imageselector) {pImageSelector = imageselector;}

 private:
    Name* pPartName;
    SectionSubscriptList* pSectionSubscriptList;
    ImageSelector* pImageSelector;
};

class StructureComponent : public Node
{
 public:
    StructureComponent()
      {
         pDataRef = NULL;
      }
   virtual ~StructureComponent();

    StructureComponent* newStructureComponent()
      {
         StructureComponent* node = new StructureComponent();
         node->pDataRef = pDataRef;  pDataRef = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    DataRef* getDataRef() {return pDataRef;}

    void setDataRef(DataRef* dataref) {pDataRef = dataref;}

 private:
    DataRef* pDataRef;
};

class CoindexedNamedObject : public Node
{
 public:
    CoindexedNamedObject()
      {
         pDataRef = NULL;
      }
   virtual ~CoindexedNamedObject();

    CoindexedNamedObject* newCoindexedNamedObject()
      {
         CoindexedNamedObject* node = new CoindexedNamedObject();
         node->pDataRef = pDataRef;  pDataRef = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    DataRef* getDataRef() {return pDataRef;}

    void setDataRef(DataRef* dataref) {pDataRef = dataref;}

 private:
    DataRef* pDataRef;
};

class TypeParamInquiry : public Node
{
 public:
    TypeParamInquiry()
      {
         pDesignator = NULL;
         pTypeParamName = NULL;
      }
   virtual ~TypeParamInquiry();

    TypeParamInquiry* newTypeParamInquiry()
      {
         TypeParamInquiry* node = new TypeParamInquiry();
         node->pDesignator = pDesignator;  pDesignator = NULL;
         node->pTypeParamName = pTypeParamName;  pTypeParamName = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Designator* getDesignator() {return pDesignator;}
    Name* getTypeParamName() {return pTypeParamName;}

    void setDesignator(Designator* designator) {pDesignator = designator;}
    void setTypeParamName(Name* typeparamname) {pTypeParamName = typeparamname;}

 private:
    Designator* pDesignator;
    Name* pTypeParamName;
};

class ArrayElement : public Node
{
 public:
    ArrayElement()
      {
         pDataRef = NULL;
      }
   virtual ~ArrayElement();

    ArrayElement* newArrayElement()
      {
         ArrayElement* node = new ArrayElement();
         node->pDataRef = pDataRef;  pDataRef = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    DataRef* getDataRef() {return pDataRef;}

    void setDataRef(DataRef* dataref) {pDataRef = dataref;}

 private:
    DataRef* pDataRef;
};

class Subscript : public Node
{
 public:
    Subscript()
      {
         pIntExpr = NULL;
      }
   virtual ~Subscript();

    Subscript* newSubscript()
      {
         Subscript* node = new Subscript();
         node->pIntExpr = pIntExpr;  pIntExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    IntExpr* getIntExpr() {return pIntExpr;}

    void setIntExpr(IntExpr* intexpr) {pIntExpr = intexpr;}

 private:
    IntExpr* pIntExpr;
};

class SectionSubscript : public Node
{
 public:
    SectionSubscript()
      {
         pSubscriptTriplet = NULL;
         pSubscript = NULL;
      }
   virtual ~SectionSubscript();

   enum OptionType
     {
        DEFAULT = 0,
        SectionSubscript_ST,
        SectionSubscript_S
     };

    SectionSubscript* newSectionSubscript()
      {
         SectionSubscript* node = new SectionSubscript();
         node->pSubscriptTriplet = pSubscriptTriplet;  pSubscriptTriplet = NULL;
         node->pSubscript = pSubscript;  pSubscript = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    SubscriptTriplet* getSubscriptTriplet() {return pSubscriptTriplet;}
    Subscript* getSubscript() {return pSubscript;}

    void setSubscriptTriplet(SubscriptTriplet* subscripttriplet) {pSubscriptTriplet = subscripttriplet;}
    void setSubscript(Subscript* subscript) {pSubscript = subscript;}

 private:
    SubscriptTriplet* pSubscriptTriplet;
    Subscript* pSubscript;
};

class SectionSubscriptList : public Node
{
 public:
    SectionSubscriptList()
      {
         pSectionSubscriptList = new std::vector<SectionSubscript*>();
      }
   virtual ~SectionSubscriptList();

    SectionSubscriptList* newSectionSubscriptList()
      {
         SectionSubscriptList* node = new SectionSubscriptList();
         delete node->pSectionSubscriptList; node->pSectionSubscriptList = pSectionSubscriptList;  pSectionSubscriptList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<SectionSubscript*>* getSectionSubscriptList() {return pSectionSubscriptList;}

    void appendSectionSubscript(SectionSubscript* sectionsubscript) {pSectionSubscriptList->push_back(sectionsubscript);}

 private:
    std::vector<SectionSubscript*>* pSectionSubscriptList;
};

class SubscriptTriplet : public Node
{
 public:
    SubscriptTriplet()
      {
         pSubscript = NULL;
         pStride = NULL;
      }
   virtual ~SubscriptTriplet();

    SubscriptTriplet* newSubscriptTriplet()
      {
         SubscriptTriplet* node = new SubscriptTriplet();
         node->pSubscript = pSubscript;  pSubscript = NULL;
         node->pStride = pStride;  pStride = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Subscript* getSubscript() {return pSubscript;}
    Stride* getStride() {return pStride;}

    void setSubscript(Subscript* subscript) {pSubscript = subscript;}
    void setStride(Stride* stride) {pStride = stride;}

 private:
    Subscript* pSubscript;
    Stride* pStride;
};

class Stride : public Node
{
 public:
    Stride()
      {
         pIntExpr = NULL;
      }
   virtual ~Stride();

    Stride* newStride()
      {
         Stride* node = new Stride();
         node->pIntExpr = pIntExpr;  pIntExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    IntExpr* getIntExpr() {return pIntExpr;}

    void setIntExpr(IntExpr* intexpr) {pIntExpr = intexpr;}

 private:
    IntExpr* pIntExpr;
};

class ImageSelector : public Node
{
 public:
    ImageSelector()
      {
         pCosubscriptList = NULL;
      }
   virtual ~ImageSelector();

    ImageSelector* newImageSelector()
      {
         ImageSelector* node = new ImageSelector();
         node->pCosubscriptList = pCosubscriptList;  pCosubscriptList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    CosubscriptList* getCosubscriptList() {return pCosubscriptList;}

    void setCosubscriptList(CosubscriptList* cosubscriptlist) {pCosubscriptList = cosubscriptlist;}

 private:
    CosubscriptList* pCosubscriptList;
};

class Cosubscript : public Node
{
 public:
    Cosubscript()
      {
         pIntExpr = NULL;
      }
   virtual ~Cosubscript();

    Cosubscript* newCosubscript()
      {
         Cosubscript* node = new Cosubscript();
         node->pIntExpr = pIntExpr;  pIntExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    IntExpr* getIntExpr() {return pIntExpr;}

    void setIntExpr(IntExpr* intexpr) {pIntExpr = intexpr;}

 private:
    IntExpr* pIntExpr;
};

class CosubscriptList : public Node
{
 public:
    CosubscriptList()
      {
         pCosubscriptList = new std::vector<Cosubscript*>();
      }
   virtual ~CosubscriptList();

    CosubscriptList* newCosubscriptList()
      {
         CosubscriptList* node = new CosubscriptList();
         delete node->pCosubscriptList; node->pCosubscriptList = pCosubscriptList;  pCosubscriptList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<Cosubscript*>* getCosubscriptList() {return pCosubscriptList;}

    void appendCosubscript(Cosubscript* cosubscript) {pCosubscriptList->push_back(cosubscript);}

 private:
    std::vector<Cosubscript*>* pCosubscriptList;
};

class AllocateStmt : public Statement
{
 public:
    AllocateStmt()
      {
         pLabel = NULL;
         pTypeSpec = NULL;
         pAllocationList = NULL;
         pAllocOptList = NULL;
         pEOS = NULL;
      }
   virtual ~AllocateStmt();

    AllocateStmt* newAllocateStmt()
      {
         AllocateStmt* node = new AllocateStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pTypeSpec = pTypeSpec;  pTypeSpec = NULL;
         node->pAllocationList = pAllocationList;  pAllocationList = NULL;
         node->pAllocOptList = pAllocOptList;  pAllocOptList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    TypeSpec* getTypeSpec() {return pTypeSpec;}
    AllocationList* getAllocationList() {return pAllocationList;}
    AllocOptList* getAllocOptList() {return pAllocOptList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setTypeSpec(TypeSpec* typespec) {pTypeSpec = typespec;}
    void setAllocationList(AllocationList* allocationlist) {pAllocationList = allocationlist;}
    void setAllocOptList(AllocOptList* allocoptlist) {pAllocOptList = allocoptlist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    TypeSpec* pTypeSpec;
    AllocationList* pAllocationList;
    AllocOptList* pAllocOptList;
    EOS* pEOS;
};

class AllocOpt : public Node
{
 public:
    AllocOpt()
      {
         pStatVariable = NULL;
         pExpr = NULL;
         pErrmsgVariable = NULL;
      }
   virtual ~AllocOpt();

   enum OptionType
     {
        DEFAULT = 0,
        AllocOpt_STAT,
        AllocOpt_SRC,
        AllocOpt_MOLD,
        AllocOpt_ERR
     };

    AllocOpt* newAllocOpt()
      {
         AllocOpt* node = new AllocOpt();
         node->pStatVariable = pStatVariable;  pStatVariable = NULL;
         node->pExpr = pExpr;  pExpr = NULL;
         node->pErrmsgVariable = pErrmsgVariable;  pErrmsgVariable = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    StatVariable* getStatVariable() {return pStatVariable;}
    Expr* getExpr() {return pExpr;}
    ErrmsgVariable* getErrmsgVariable() {return pErrmsgVariable;}

    void setStatVariable(StatVariable* statvariable) {pStatVariable = statvariable;}
    void setExpr(Expr* expr) {pExpr = expr;}
    void setErrmsgVariable(ErrmsgVariable* errmsgvariable) {pErrmsgVariable = errmsgvariable;}

 private:
    StatVariable* pStatVariable;
    Expr* pExpr;
    ErrmsgVariable* pErrmsgVariable;
};

class AllocOptList : public Node
{
 public:
    AllocOptList()
      {
         pAllocOptList = new std::vector<AllocOpt*>();
      }
   virtual ~AllocOptList();

    AllocOptList* newAllocOptList()
      {
         AllocOptList* node = new AllocOptList();
         delete node->pAllocOptList; node->pAllocOptList = pAllocOptList;  pAllocOptList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<AllocOpt*>* getAllocOptList() {return pAllocOptList;}

    void appendAllocOpt(AllocOpt* allocopt) {pAllocOptList->push_back(allocopt);}

 private:
    std::vector<AllocOpt*>* pAllocOptList;
};

class StatVariable : public Node
{
 public:
    StatVariable()
      {
         pIntVariable = NULL;
      }
   virtual ~StatVariable();

    StatVariable* newStatVariable()
      {
         StatVariable* node = new StatVariable();
         node->pIntVariable = pIntVariable;  pIntVariable = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    IntVariable* getIntVariable() {return pIntVariable;}

    void setIntVariable(IntVariable* intvariable) {pIntVariable = intvariable;}

 private:
    IntVariable* pIntVariable;
};

class ErrmsgVariable : public Node
{
 public:
    ErrmsgVariable()
      {
         pDefaultCharVariable = NULL;
      }
   virtual ~ErrmsgVariable();

    ErrmsgVariable* newErrmsgVariable()
      {
         ErrmsgVariable* node = new ErrmsgVariable();
         node->pDefaultCharVariable = pDefaultCharVariable;  pDefaultCharVariable = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    DefaultCharVariable* getDefaultCharVariable() {return pDefaultCharVariable;}

    void setDefaultCharVariable(DefaultCharVariable* defaultcharvariable) {pDefaultCharVariable = defaultcharvariable;}

 private:
    DefaultCharVariable* pDefaultCharVariable;
};

class Allocation : public Node
{
 public:
    Allocation()
      {
         pAllocateObject = NULL;
         pAllocateShapeSpecList = NULL;
         pAllocateCoarraySpec = NULL;
      }
   virtual ~Allocation();

    Allocation* newAllocation()
      {
         Allocation* node = new Allocation();
         node->pAllocateObject = pAllocateObject;  pAllocateObject = NULL;
         node->pAllocateShapeSpecList = pAllocateShapeSpecList;  pAllocateShapeSpecList = NULL;
         node->pAllocateCoarraySpec = pAllocateCoarraySpec;  pAllocateCoarraySpec = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    AllocateObject* getAllocateObject() {return pAllocateObject;}
    AllocateShapeSpecList* getAllocateShapeSpecList() {return pAllocateShapeSpecList;}
    AllocateCoarraySpec* getAllocateCoarraySpec() {return pAllocateCoarraySpec;}

    void setAllocateObject(AllocateObject* allocateobject) {pAllocateObject = allocateobject;}
    void setAllocateShapeSpecList(AllocateShapeSpecList* allocateshapespeclist) {pAllocateShapeSpecList = allocateshapespeclist;}
    void setAllocateCoarraySpec(AllocateCoarraySpec* allocatecoarrayspec) {pAllocateCoarraySpec = allocatecoarrayspec;}

 private:
    AllocateObject* pAllocateObject;
    AllocateShapeSpecList* pAllocateShapeSpecList;
    AllocateCoarraySpec* pAllocateCoarraySpec;
};

class AllocationList : public Node
{
 public:
    AllocationList()
      {
         pAllocationList = new std::vector<Allocation*>();
      }
   virtual ~AllocationList();

    AllocationList* newAllocationList()
      {
         AllocationList* node = new AllocationList();
         delete node->pAllocationList; node->pAllocationList = pAllocationList;  pAllocationList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<Allocation*>* getAllocationList() {return pAllocationList;}

    void appendAllocation(Allocation* allocation) {pAllocationList->push_back(allocation);}

 private:
    std::vector<Allocation*>* pAllocationList;
};

class AllocateObject : public Node
{
 public:
    AllocateObject()
      {
         pStructureComponent = NULL;
      }
   virtual ~AllocateObject();

    AllocateObject* newAllocateObject()
      {
         AllocateObject* node = new AllocateObject();
         node->pStructureComponent = pStructureComponent;  pStructureComponent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    StructureComponent* getStructureComponent() {return pStructureComponent;}

    void setStructureComponent(StructureComponent* structurecomponent) {pStructureComponent = structurecomponent;}

 private:
    StructureComponent* pStructureComponent;
};

class AllocateObjectList : public Node
{
 public:
    AllocateObjectList()
      {
         pAllocateObjectList = new std::vector<AllocateObject*>();
      }
   virtual ~AllocateObjectList();

    AllocateObjectList* newAllocateObjectList()
      {
         AllocateObjectList* node = new AllocateObjectList();
         delete node->pAllocateObjectList; node->pAllocateObjectList = pAllocateObjectList;  pAllocateObjectList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<AllocateObject*>* getAllocateObjectList() {return pAllocateObjectList;}

    void appendAllocateObject(AllocateObject* allocateobject) {pAllocateObjectList->push_back(allocateobject);}

 private:
    std::vector<AllocateObject*>* pAllocateObjectList;
};

class AllocateShapeSpec : public Node
{
 public:
    AllocateShapeSpec()
      {
         pLowerBoundExpr = NULL;
         pUpperBoundExpr = NULL;
      }
   virtual ~AllocateShapeSpec();

    AllocateShapeSpec* newAllocateShapeSpec()
      {
         AllocateShapeSpec* node = new AllocateShapeSpec();
         node->pLowerBoundExpr = pLowerBoundExpr;  pLowerBoundExpr = NULL;
         node->pUpperBoundExpr = pUpperBoundExpr;  pUpperBoundExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    LowerBoundExpr* getLowerBoundExpr() {return pLowerBoundExpr;}
    UpperBoundExpr* getUpperBoundExpr() {return pUpperBoundExpr;}

    void setLowerBoundExpr(LowerBoundExpr* lowerboundexpr) {pLowerBoundExpr = lowerboundexpr;}
    void setUpperBoundExpr(UpperBoundExpr* upperboundexpr) {pUpperBoundExpr = upperboundexpr;}

 private:
    LowerBoundExpr* pLowerBoundExpr;
    UpperBoundExpr* pUpperBoundExpr;
};

class AllocateShapeSpecList : public Node
{
 public:
    AllocateShapeSpecList()
      {
         pAllocateShapeSpecList = new std::vector<AllocateShapeSpec*>();
      }
   virtual ~AllocateShapeSpecList();

    AllocateShapeSpecList* newAllocateShapeSpecList()
      {
         AllocateShapeSpecList* node = new AllocateShapeSpecList();
         delete node->pAllocateShapeSpecList; node->pAllocateShapeSpecList = pAllocateShapeSpecList;  pAllocateShapeSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<AllocateShapeSpec*>* getAllocateShapeSpecList() {return pAllocateShapeSpecList;}

    void appendAllocateShapeSpec(AllocateShapeSpec* allocateshapespec) {pAllocateShapeSpecList->push_back(allocateshapespec);}

 private:
    std::vector<AllocateShapeSpec*>* pAllocateShapeSpecList;
};

class LowerBoundExpr : public Node
{
 public:
    LowerBoundExpr()
      {
         pIntExpr = NULL;
      }
   virtual ~LowerBoundExpr();

    LowerBoundExpr* newLowerBoundExpr()
      {
         LowerBoundExpr* node = new LowerBoundExpr();
         node->pIntExpr = pIntExpr;  pIntExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    IntExpr* getIntExpr() {return pIntExpr;}

    void setIntExpr(IntExpr* intexpr) {pIntExpr = intexpr;}

 private:
    IntExpr* pIntExpr;
};

class UpperBoundExpr : public Node
{
 public:
    UpperBoundExpr()
      {
         pIntExpr = NULL;
      }
   virtual ~UpperBoundExpr();

    UpperBoundExpr* newUpperBoundExpr()
      {
         UpperBoundExpr* node = new UpperBoundExpr();
         node->pIntExpr = pIntExpr;  pIntExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    IntExpr* getIntExpr() {return pIntExpr;}

    void setIntExpr(IntExpr* intexpr) {pIntExpr = intexpr;}

 private:
    IntExpr* pIntExpr;
};

class AllocateCoarraySpec : public Node
{
 public:
    AllocateCoarraySpec()
      {
         pAllocateCoshapeSpecList = NULL;
         pLowerBoundExpr = NULL;
      }
   virtual ~AllocateCoarraySpec();

    AllocateCoarraySpec* newAllocateCoarraySpec()
      {
         AllocateCoarraySpec* node = new AllocateCoarraySpec();
         node->pAllocateCoshapeSpecList = pAllocateCoshapeSpecList;  pAllocateCoshapeSpecList = NULL;
         node->pLowerBoundExpr = pLowerBoundExpr;  pLowerBoundExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    AllocateCoshapeSpecList* getAllocateCoshapeSpecList() {return pAllocateCoshapeSpecList;}
    LowerBoundExpr* getLowerBoundExpr() {return pLowerBoundExpr;}

    void setAllocateCoshapeSpecList(AllocateCoshapeSpecList* allocatecoshapespeclist) {pAllocateCoshapeSpecList = allocatecoshapespeclist;}
    void setLowerBoundExpr(LowerBoundExpr* lowerboundexpr) {pLowerBoundExpr = lowerboundexpr;}

 private:
    AllocateCoshapeSpecList* pAllocateCoshapeSpecList;
    LowerBoundExpr* pLowerBoundExpr;
};

class AllocateCoshapeSpec : public Node
{
 public:
    AllocateCoshapeSpec()
      {
         pLowerBoundExpr = NULL;
         pUpperBoundExpr = NULL;
      }
   virtual ~AllocateCoshapeSpec();

    AllocateCoshapeSpec* newAllocateCoshapeSpec()
      {
         AllocateCoshapeSpec* node = new AllocateCoshapeSpec();
         node->pLowerBoundExpr = pLowerBoundExpr;  pLowerBoundExpr = NULL;
         node->pUpperBoundExpr = pUpperBoundExpr;  pUpperBoundExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    LowerBoundExpr* getLowerBoundExpr() {return pLowerBoundExpr;}
    UpperBoundExpr* getUpperBoundExpr() {return pUpperBoundExpr;}

    void setLowerBoundExpr(LowerBoundExpr* lowerboundexpr) {pLowerBoundExpr = lowerboundexpr;}
    void setUpperBoundExpr(UpperBoundExpr* upperboundexpr) {pUpperBoundExpr = upperboundexpr;}

 private:
    LowerBoundExpr* pLowerBoundExpr;
    UpperBoundExpr* pUpperBoundExpr;
};

class AllocateCoshapeSpecList : public Node
{
 public:
    AllocateCoshapeSpecList()
      {
         pAllocateCoshapeSpecList = new std::vector<AllocateCoshapeSpec*>();
      }
   virtual ~AllocateCoshapeSpecList();

    AllocateCoshapeSpecList* newAllocateCoshapeSpecList()
      {
         AllocateCoshapeSpecList* node = new AllocateCoshapeSpecList();
         delete node->pAllocateCoshapeSpecList; node->pAllocateCoshapeSpecList = pAllocateCoshapeSpecList;  pAllocateCoshapeSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<AllocateCoshapeSpec*>* getAllocateCoshapeSpecList() {return pAllocateCoshapeSpecList;}

    void appendAllocateCoshapeSpec(AllocateCoshapeSpec* allocatecoshapespec) {pAllocateCoshapeSpecList->push_back(allocatecoshapespec);}

 private:
    std::vector<AllocateCoshapeSpec*>* pAllocateCoshapeSpecList;
};

class NullifyStmt : public Statement
{
 public:
    NullifyStmt()
      {
         pLabel = NULL;
         pPointerObjectList = NULL;
         pEOS = NULL;
      }
   virtual ~NullifyStmt();

    NullifyStmt* newNullifyStmt()
      {
         NullifyStmt* node = new NullifyStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pPointerObjectList = pPointerObjectList;  pPointerObjectList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    PointerObjectList* getPointerObjectList() {return pPointerObjectList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setPointerObjectList(PointerObjectList* pointerobjectlist) {pPointerObjectList = pointerobjectlist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    PointerObjectList* pPointerObjectList;
    EOS* pEOS;
};

class PointerObject : public Node
{
 public:
    PointerObject()
      {
         pProcPointerName = NULL;
         pStructureComponent = NULL;
         pVariableName = NULL;
      }
   virtual ~PointerObject();

   enum OptionType
     {
        DEFAULT = 0,
        PointerObject_PPN,
        PointerObject_SC,
        PointerObject_VN
     };

    PointerObject* newPointerObject()
      {
         PointerObject* node = new PointerObject();
         node->pProcPointerName = pProcPointerName;  pProcPointerName = NULL;
         node->pStructureComponent = pStructureComponent;  pStructureComponent = NULL;
         node->pVariableName = pVariableName;  pVariableName = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    ProcPointerName* getProcPointerName() {return pProcPointerName;}
    StructureComponent* getStructureComponent() {return pStructureComponent;}
    VariableName* getVariableName() {return pVariableName;}

    void setProcPointerName(ProcPointerName* procpointername) {pProcPointerName = procpointername;}
    void setStructureComponent(StructureComponent* structurecomponent) {pStructureComponent = structurecomponent;}
    void setVariableName(VariableName* variablename) {pVariableName = variablename;}

 private:
    ProcPointerName* pProcPointerName;
    StructureComponent* pStructureComponent;
    VariableName* pVariableName;
};

class PointerObjectList : public Node
{
 public:
    PointerObjectList()
      {
         pPointerObjectList = new std::vector<PointerObject*>();
      }
   virtual ~PointerObjectList();

    PointerObjectList* newPointerObjectList()
      {
         PointerObjectList* node = new PointerObjectList();
         delete node->pPointerObjectList; node->pPointerObjectList = pPointerObjectList;  pPointerObjectList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<PointerObject*>* getPointerObjectList() {return pPointerObjectList;}

    void appendPointerObject(PointerObject* pointerobject) {pPointerObjectList->push_back(pointerobject);}

 private:
    std::vector<PointerObject*>* pPointerObjectList;
};

class DeallocateStmt : public Statement
{
 public:
    DeallocateStmt()
      {
         pLabel = NULL;
         pAllocateObjectList = NULL;
         pDeallocOptList = NULL;
         pEOS = NULL;
      }
   virtual ~DeallocateStmt();

    DeallocateStmt* newDeallocateStmt()
      {
         DeallocateStmt* node = new DeallocateStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pAllocateObjectList = pAllocateObjectList;  pAllocateObjectList = NULL;
         node->pDeallocOptList = pDeallocOptList;  pDeallocOptList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    AllocateObjectList* getAllocateObjectList() {return pAllocateObjectList;}
    DeallocOptList* getDeallocOptList() {return pDeallocOptList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setAllocateObjectList(AllocateObjectList* allocateobjectlist) {pAllocateObjectList = allocateobjectlist;}
    void setDeallocOptList(DeallocOptList* deallocoptlist) {pDeallocOptList = deallocoptlist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    AllocateObjectList* pAllocateObjectList;
    DeallocOptList* pDeallocOptList;
    EOS* pEOS;
};

class DeallocOpt : public Node
{
 public:
    DeallocOpt()
      {
         pErrmsgVariable = NULL;
         pStatVariable = NULL;
      }
   virtual ~DeallocOpt();

   enum OptionType
     {
        DEFAULT = 0,
        DeallocOpt_ERR,
        DeallocOpt_STAT
     };

    DeallocOpt* newDeallocOpt()
      {
         DeallocOpt* node = new DeallocOpt();
         node->pErrmsgVariable = pErrmsgVariable;  pErrmsgVariable = NULL;
         node->pStatVariable = pStatVariable;  pStatVariable = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    ErrmsgVariable* getErrmsgVariable() {return pErrmsgVariable;}
    StatVariable* getStatVariable() {return pStatVariable;}

    void setErrmsgVariable(ErrmsgVariable* errmsgvariable) {pErrmsgVariable = errmsgvariable;}
    void setStatVariable(StatVariable* statvariable) {pStatVariable = statvariable;}

 private:
    ErrmsgVariable* pErrmsgVariable;
    StatVariable* pStatVariable;
};

class DeallocOptList : public Node
{
 public:
    DeallocOptList()
      {
         pDeallocOptList = new std::vector<DeallocOpt*>();
      }
   virtual ~DeallocOptList();

    DeallocOptList* newDeallocOptList()
      {
         DeallocOptList* node = new DeallocOptList();
         delete node->pDeallocOptList; node->pDeallocOptList = pDeallocOptList;  pDeallocOptList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<DeallocOpt*>* getDeallocOptList() {return pDeallocOptList;}

    void appendDeallocOpt(DeallocOpt* deallocopt) {pDeallocOptList->push_back(deallocopt);}

 private:
    std::vector<DeallocOpt*>* pDeallocOptList;
};

class DefinedOperator : public Node
{
 public:
    DefinedOperator()
      {
         pExtendedIntrinsicOp = NULL;
         pDefinedBinaryOp = NULL;
         pDefinedUnaryOp = NULL;
      }
   virtual ~DefinedOperator();

   enum OptionType
     {
        DEFAULT = 0,
        DefinedOperator_EIO,
        DefinedOperator_DBO,
        DefinedOperator_DUO
     };

    DefinedOperator* newDefinedOperator()
      {
         DefinedOperator* node = new DefinedOperator();
         node->pExtendedIntrinsicOp = pExtendedIntrinsicOp;  pExtendedIntrinsicOp = NULL;
         node->pDefinedBinaryOp = pDefinedBinaryOp;  pDefinedBinaryOp = NULL;
         node->pDefinedUnaryOp = pDefinedUnaryOp;  pDefinedUnaryOp = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    ExtendedIntrinsicOp* getExtendedIntrinsicOp() {return pExtendedIntrinsicOp;}
    DefinedBinaryOp* getDefinedBinaryOp() {return pDefinedBinaryOp;}
    DefinedUnaryOp* getDefinedUnaryOp() {return pDefinedUnaryOp;}

    void setExtendedIntrinsicOp(ExtendedIntrinsicOp* extendedintrinsicop) {pExtendedIntrinsicOp = extendedintrinsicop;}
    void setDefinedBinaryOp(DefinedBinaryOp* definedbinaryop) {pDefinedBinaryOp = definedbinaryop;}
    void setDefinedUnaryOp(DefinedUnaryOp* definedunaryop) {pDefinedUnaryOp = definedunaryop;}

 private:
    ExtendedIntrinsicOp* pExtendedIntrinsicOp;
    DefinedBinaryOp* pDefinedBinaryOp;
    DefinedUnaryOp* pDefinedUnaryOp;
};

class ExtendedIntrinsicOp : public Node
{
 public:
    ExtendedIntrinsicOp()
      {
         pIntrinsicOperator = NULL;
      }
   virtual ~ExtendedIntrinsicOp();

    ExtendedIntrinsicOp* newExtendedIntrinsicOp()
      {
         ExtendedIntrinsicOp* node = new ExtendedIntrinsicOp();
         node->pIntrinsicOperator = pIntrinsicOperator;  pIntrinsicOperator = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    IntrinsicOperator* getIntrinsicOperator() {return pIntrinsicOperator;}

    void setIntrinsicOperator(IntrinsicOperator* intrinsicoperator) {pIntrinsicOperator = intrinsicoperator;}

 private:
    IntrinsicOperator* pIntrinsicOperator;
};

class Primary : public Node
{
 public:
    Primary()
      {
         pTypeParamInquiry = NULL;
         pFunctionReference = NULL;
         pStructureConstructor = NULL;
         pArrayConstructor = NULL;
         pDesignator = NULL;
         pConstant = NULL;
      }
   virtual ~Primary();

   enum OptionType
     {
        DEFAULT = 0,
        Designator_ot,
        Primary_TPI_AMB,
        Primary_FR_AMB,
        Primary_SC_AMB,
        Primary_AC_AMB,
        Constant_ot
     };

    Primary* newPrimary()
      {
         Primary* node = new Primary();
         node->pTypeParamInquiry = pTypeParamInquiry;  pTypeParamInquiry = NULL;
         node->pFunctionReference = pFunctionReference;  pFunctionReference = NULL;
         node->pStructureConstructor = pStructureConstructor;  pStructureConstructor = NULL;
         node->pArrayConstructor = pArrayConstructor;  pArrayConstructor = NULL;
         node->pDesignator = pDesignator;  pDesignator = NULL;
         node->pConstant = pConstant;  pConstant = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    TypeParamInquiry* getTypeParamInquiry() {return pTypeParamInquiry;}
    FunctionReference* getFunctionReference() {return pFunctionReference;}
    StructureConstructor* getStructureConstructor() {return pStructureConstructor;}
    ArrayConstructor* getArrayConstructor() {return pArrayConstructor;}
    Designator* getDesignator() {return pDesignator;}
    Constant* getConstant() {return pConstant;}

    void setTypeParamInquiry(TypeParamInquiry* typeparaminquiry) {pTypeParamInquiry = typeparaminquiry;}
    void setFunctionReference(FunctionReference* functionreference) {pFunctionReference = functionreference;}
    void setStructureConstructor(StructureConstructor* structureconstructor) {pStructureConstructor = structureconstructor;}
    void setArrayConstructor(ArrayConstructor* arrayconstructor) {pArrayConstructor = arrayconstructor;}
    void setDesignator(Designator* designator) {pDesignator = designator;}
    void setConstant(Constant* constant) {pConstant = constant;}

 private:
    TypeParamInquiry* pTypeParamInquiry;
    FunctionReference* pFunctionReference;
    StructureConstructor* pStructureConstructor;
    ArrayConstructor* pArrayConstructor;
    Designator* pDesignator;
    Constant* pConstant;
};

class DefinedUnaryOp : public Node
{
 public:
    DefinedUnaryOp()
      {
         pDop = NULL;
      }
   virtual ~DefinedUnaryOp();

    DefinedUnaryOp* newDefinedUnaryOp()
      {
         DefinedUnaryOp* node = new DefinedUnaryOp();
         node->pDop = pDop;  pDop = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Dop* getDop() {return pDop;}

    void setDop(Dop* dop) {pDop = dop;}

 private:
    Dop* pDop;
};


class DefinedBinaryOp : public Node
{
 public:
    DefinedBinaryOp()
      {
         pDop = NULL;
      }
   virtual ~DefinedBinaryOp();

    DefinedBinaryOp* newDefinedBinaryOp()
      {
         DefinedBinaryOp* node = new DefinedBinaryOp();
         node->pDop = pDop;  pDop = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Dop* getDop() {return pDop;}

    void setDop(Dop* dop) {pDop = dop;}

 private:
    Dop* pDop;
};

class LogicalExpr : public Node
{
 public:
    LogicalExpr()
      {
         pExpr = NULL;
      }
   virtual ~LogicalExpr();

    LogicalExpr* newLogicalExpr()
      {
         LogicalExpr* node = new LogicalExpr();
         node->pExpr = pExpr;  pExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Expr* getExpr() {return pExpr;}

    void setExpr(Expr* expr) {pExpr = expr;}

 private:
    Expr* pExpr;
};

class DefaultCharExpr : public Node
{
 public:
    DefaultCharExpr()
      {
         pExpr = NULL;
      }
   virtual ~DefaultCharExpr();

    DefaultCharExpr* newDefaultCharExpr()
      {
         DefaultCharExpr* node = new DefaultCharExpr();
         node->pExpr = pExpr;  pExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Expr* getExpr() {return pExpr;}

    void setExpr(Expr* expr) {pExpr = expr;}

 private:
    Expr* pExpr;
};

class IntExpr : public Node
{
 public:
    IntExpr()
      {
         pExpr = NULL;
      }
   virtual ~IntExpr();

    IntExpr* newIntExpr()
      {
         IntExpr* node = new IntExpr();
         node->pExpr = pExpr;  pExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Expr* getExpr() {return pExpr;}

    void setExpr(Expr* expr) {pExpr = expr;}

 private:
    Expr* pExpr;
};

class SpecificationExpr : public Node
{
 public:
    SpecificationExpr()
      {
         pIntExpr = NULL;
      }
   virtual ~SpecificationExpr();

    SpecificationExpr* newSpecificationExpr()
      {
         SpecificationExpr* node = new SpecificationExpr();
         node->pIntExpr = pIntExpr;  pIntExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    IntExpr* getIntExpr() {return pIntExpr;}

    void setIntExpr(IntExpr* intexpr) {pIntExpr = intexpr;}

 private:
    IntExpr* pIntExpr;
};

class ConstantExpr : public Node
{
 public:
    ConstantExpr()
      {
         pExpr = NULL;
      }
   virtual ~ConstantExpr();

    ConstantExpr* newConstantExpr()
      {
         ConstantExpr* node = new ConstantExpr();
         node->pExpr = pExpr;  pExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Expr* getExpr() {return pExpr;}

    void setExpr(Expr* expr) {pExpr = expr;}

 private:
    Expr* pExpr;
};

class IntConstantExpr : public Node
{
 public:
    IntConstantExpr()
      {
         pIntExpr = NULL;
      }
   virtual ~IntConstantExpr();

    IntConstantExpr* newIntConstantExpr()
      {
         IntConstantExpr* node = new IntConstantExpr();
         node->pIntExpr = pIntExpr;  pIntExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    IntExpr* getIntExpr() {return pIntExpr;}

    void setIntExpr(IntExpr* intexpr) {pIntExpr = intexpr;}

 private:
    IntExpr* pIntExpr;
};

class AssignmentStmt : public Statement
{
 public:
    AssignmentStmt()
      {
         pLabel = NULL;
         pVariable = NULL;
         pExpr = NULL;
         pEOS = NULL;
      }
   virtual ~AssignmentStmt();

    AssignmentStmt* newAssignmentStmt()
      {
         AssignmentStmt* node = new AssignmentStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pVariable = pVariable;  pVariable = NULL;
         node->pExpr = pExpr;  pExpr = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Variable* getVariable() {return pVariable;}
    Expr* getExpr() {return pExpr;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setVariable(Variable* variable) {pVariable = variable;}
    void setExpr(Expr* expr) {pExpr = expr;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Variable* pVariable;
    Expr* pExpr;
    EOS* pEOS;
};

class PointerAssignmentStmt : public Statement
{
 public:
    PointerAssignmentStmt()
      {
         pLabel = NULL;
         pProcPointerObject = NULL;
         pProcTarget = NULL;
         pEOS = NULL;
         pDataPointerObject = NULL;
         pBoundsRemappingList = NULL;
         pDataTarget = NULL;
         pBoundsSpecList = NULL;
      }
   virtual ~PointerAssignmentStmt();

   enum OptionType
     {
        DEFAULT = 0,
        PointerAssignmentStmt_PPO,
        PointerAssignmentStmt_DPO2,
        PointerAssignmentStmt_DPO1
     };

    PointerAssignmentStmt* newPointerAssignmentStmt()
      {
         PointerAssignmentStmt* node = new PointerAssignmentStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pProcPointerObject = pProcPointerObject;  pProcPointerObject = NULL;
         node->pProcTarget = pProcTarget;  pProcTarget = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->pDataPointerObject = pDataPointerObject;  pDataPointerObject = NULL;
         node->pBoundsRemappingList = pBoundsRemappingList;  pBoundsRemappingList = NULL;
         node->pDataTarget = pDataTarget;  pDataTarget = NULL;
         node->pBoundsSpecList = pBoundsSpecList;  pBoundsSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    ProcPointerObject* getProcPointerObject() {return pProcPointerObject;}
    ProcTarget* getProcTarget() {return pProcTarget;}
    EOS* getEOS() {return pEOS;}
    DataPointerObject* getDataPointerObject() {return pDataPointerObject;}
    BoundsRemappingList* getBoundsRemappingList() {return pBoundsRemappingList;}
    DataTarget* getDataTarget() {return pDataTarget;}
    BoundsSpecList* getBoundsSpecList() {return pBoundsSpecList;}

    void setLabel(Label* label) {pLabel = label;}
    void setProcPointerObject(ProcPointerObject* procpointerobject) {pProcPointerObject = procpointerobject;}
    void setProcTarget(ProcTarget* proctarget) {pProcTarget = proctarget;}
    void setEOS(EOS* eos) {pEOS = eos;}
    void setDataPointerObject(DataPointerObject* datapointerobject) {pDataPointerObject = datapointerobject;}
    void setBoundsRemappingList(BoundsRemappingList* boundsremappinglist) {pBoundsRemappingList = boundsremappinglist;}
    void setDataTarget(DataTarget* datatarget) {pDataTarget = datatarget;}
    void setBoundsSpecList(BoundsSpecList* boundsspeclist) {pBoundsSpecList = boundsspeclist;}

 private:
    Label* pLabel;
    ProcPointerObject* pProcPointerObject;
    ProcTarget* pProcTarget;
    EOS* pEOS;
    DataPointerObject* pDataPointerObject;
    BoundsRemappingList* pBoundsRemappingList;
    DataTarget* pDataTarget;
    BoundsSpecList* pBoundsSpecList;
};

class DataPointerObject : public Node
{
 public:
    DataPointerObject()
      {
         pVariable = NULL;
         pDataPointerComponentName = NULL;
         pVariableName = NULL;
      }
   virtual ~DataPointerObject();

   enum OptionType
     {
        DEFAULT = 0,
        DataPointerObject_V,
        DataPointerObject_VN
     };

    DataPointerObject* newDataPointerObject()
      {
         DataPointerObject* node = new DataPointerObject();
         node->pVariable = pVariable;  pVariable = NULL;
         node->pDataPointerComponentName = pDataPointerComponentName;  pDataPointerComponentName = NULL;
         node->pVariableName = pVariableName;  pVariableName = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Variable* getVariable() {return pVariable;}
    Name* getDataPointerComponentName() {return pDataPointerComponentName;}
    VariableName* getVariableName() {return pVariableName;}

    void setVariable(Variable* variable) {pVariable = variable;}
    void setDataPointerComponentName(Name* datapointercomponentname) {pDataPointerComponentName = datapointercomponentname;}
    void setVariableName(VariableName* variablename) {pVariableName = variablename;}

 private:
    Variable* pVariable;
    Name* pDataPointerComponentName;
    VariableName* pVariableName;
};

class BoundsSpec : public Node
{
 public:
    BoundsSpec()
      {
         pLowerBoundExpr = NULL;
      }
   virtual ~BoundsSpec();

    BoundsSpec* newBoundsSpec()
      {
         BoundsSpec* node = new BoundsSpec();
         node->pLowerBoundExpr = pLowerBoundExpr;  pLowerBoundExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    LowerBoundExpr* getLowerBoundExpr() {return pLowerBoundExpr;}

    void setLowerBoundExpr(LowerBoundExpr* lowerboundexpr) {pLowerBoundExpr = lowerboundexpr;}

 private:
    LowerBoundExpr* pLowerBoundExpr;
};

class BoundsSpecList : public Node
{
 public:
    BoundsSpecList()
      {
         pBoundsSpecList = new std::vector<BoundsSpec*>();
      }
   virtual ~BoundsSpecList();

    BoundsSpecList* newBoundsSpecList()
      {
         BoundsSpecList* node = new BoundsSpecList();
         delete node->pBoundsSpecList; node->pBoundsSpecList = pBoundsSpecList;  pBoundsSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<BoundsSpec*>* getBoundsSpecList() {return pBoundsSpecList;}

    void appendBoundsSpec(BoundsSpec* boundsspec) {pBoundsSpecList->push_back(boundsspec);}

 private:
    std::vector<BoundsSpec*>* pBoundsSpecList;
};

class BoundsRemapping : public Node
{
 public:
    BoundsRemapping()
      {
         pLowerBoundExpr = NULL;
         pUpperBoundExpr = NULL;
      }
   virtual ~BoundsRemapping();

    BoundsRemapping* newBoundsRemapping()
      {
         BoundsRemapping* node = new BoundsRemapping();
         node->pLowerBoundExpr = pLowerBoundExpr;  pLowerBoundExpr = NULL;
         node->pUpperBoundExpr = pUpperBoundExpr;  pUpperBoundExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    LowerBoundExpr* getLowerBoundExpr() {return pLowerBoundExpr;}
    UpperBoundExpr* getUpperBoundExpr() {return pUpperBoundExpr;}

    void setLowerBoundExpr(LowerBoundExpr* lowerboundexpr) {pLowerBoundExpr = lowerboundexpr;}
    void setUpperBoundExpr(UpperBoundExpr* upperboundexpr) {pUpperBoundExpr = upperboundexpr;}

 private:
    LowerBoundExpr* pLowerBoundExpr;
    UpperBoundExpr* pUpperBoundExpr;
};

class BoundsRemappingList : public Node
{
 public:
    BoundsRemappingList()
      {
         pBoundsRemappingList = new std::vector<BoundsRemapping*>();
      }
   virtual ~BoundsRemappingList();

    BoundsRemappingList* newBoundsRemappingList()
      {
         BoundsRemappingList* node = new BoundsRemappingList();
         delete node->pBoundsRemappingList; node->pBoundsRemappingList = pBoundsRemappingList;  pBoundsRemappingList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<BoundsRemapping*>* getBoundsRemappingList() {return pBoundsRemappingList;}

    void appendBoundsRemapping(BoundsRemapping* boundsremapping) {pBoundsRemappingList->push_back(boundsremapping);}

 private:
    std::vector<BoundsRemapping*>* pBoundsRemappingList;
};

class DataTarget : public Node
{
 public:
    DataTarget()
      {
         pExpr = NULL;
      }
   virtual ~DataTarget();

    DataTarget* newDataTarget()
      {
         DataTarget* node = new DataTarget();
         node->pExpr = pExpr;  pExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Expr* getExpr() {return pExpr;}

    void setExpr(Expr* expr) {pExpr = expr;}

 private:
    Expr* pExpr;
};

class ProcPointerObject : public Node
{
 public:
    ProcPointerObject()
      {
         pProcComponentRef = NULL;
         pProcPointerName = NULL;
      }
   virtual ~ProcPointerObject();

   enum OptionType
     {
        DEFAULT = 0,
        ProcPointerObject_PCR,
        ProcPointerObject_PPN
     };

    ProcPointerObject* newProcPointerObject()
      {
         ProcPointerObject* node = new ProcPointerObject();
         node->pProcComponentRef = pProcComponentRef;  pProcComponentRef = NULL;
         node->pProcPointerName = pProcPointerName;  pProcPointerName = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    ProcComponentRef* getProcComponentRef() {return pProcComponentRef;}
    ProcPointerName* getProcPointerName() {return pProcPointerName;}

    void setProcComponentRef(ProcComponentRef* proccomponentref) {pProcComponentRef = proccomponentref;}
    void setProcPointerName(ProcPointerName* procpointername) {pProcPointerName = procpointername;}

 private:
    ProcComponentRef* pProcComponentRef;
    ProcPointerName* pProcPointerName;
};

class ProcComponentRef : public Node
{
 public:
    ProcComponentRef()
      {
         pVariable = NULL;
         pProcedureComponentName = NULL;
      }
   virtual ~ProcComponentRef();

    ProcComponentRef* newProcComponentRef()
      {
         ProcComponentRef* node = new ProcComponentRef();
         node->pVariable = pVariable;  pVariable = NULL;
         node->pProcedureComponentName = pProcedureComponentName;  pProcedureComponentName = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Variable* getVariable() {return pVariable;}
    Name* getProcedureComponentName() {return pProcedureComponentName;}

    void setVariable(Variable* variable) {pVariable = variable;}
    void setProcedureComponentName(Name* procedurecomponentname) {pProcedureComponentName = procedurecomponentname;}

 private:
    Variable* pVariable;
    Name* pProcedureComponentName;
};

class ProcTarget : public Node
{
 public:
    ProcTarget()
      {
         pProcComponentRef = NULL;
         pProcedureName = NULL;
         pExpr = NULL;
      }
   virtual ~ProcTarget();

   enum OptionType
     {
        DEFAULT = 0,
        ProcTarget_PCR,
        ProcTarget_PN,
        ProcTarget_E
     };

    ProcTarget* newProcTarget()
      {
         ProcTarget* node = new ProcTarget();
         node->pProcComponentRef = pProcComponentRef;  pProcComponentRef = NULL;
         node->pProcedureName = pProcedureName;  pProcedureName = NULL;
         node->pExpr = pExpr;  pExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    ProcComponentRef* getProcComponentRef() {return pProcComponentRef;}
    Name* getProcedureName() {return pProcedureName;}
    Expr* getExpr() {return pExpr;}

    void setProcComponentRef(ProcComponentRef* proccomponentref) {pProcComponentRef = proccomponentref;}
    void setProcedureName(Name* procedurename) {pProcedureName = procedurename;}
    void setExpr(Expr* expr) {pExpr = expr;}

 private:
    ProcComponentRef* pProcComponentRef;
    Name* pProcedureName;
    Expr* pExpr;
};

class WhereStmt : public Node
{
 public:
    WhereStmt()
      {
         pLabel = NULL;
         pMaskExpr = NULL;
         pWhereAssignmentStmt = NULL;
      }
   virtual ~WhereStmt();

    WhereStmt* newWhereStmt()
      {
         WhereStmt* node = new WhereStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pMaskExpr = pMaskExpr;  pMaskExpr = NULL;
         node->pWhereAssignmentStmt = pWhereAssignmentStmt;  pWhereAssignmentStmt = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    MaskExpr* getMaskExpr() {return pMaskExpr;}
    WhereAssignmentStmt* getWhereAssignmentStmt() {return pWhereAssignmentStmt;}

    void setLabel(Label* label) {pLabel = label;}
    void setMaskExpr(MaskExpr* maskexpr) {pMaskExpr = maskexpr;}
    void setWhereAssignmentStmt(WhereAssignmentStmt* whereassignmentstmt) {pWhereAssignmentStmt = whereassignmentstmt;}

 private:
    Label* pLabel;
    MaskExpr* pMaskExpr;
    WhereAssignmentStmt* pWhereAssignmentStmt;
};

class WhereConstruct : public Node
{
 public:
    WhereConstruct()
      {
         pWhereConstructStmt = NULL;
         pWhereBodyConstructList = new std::vector<WhereBodyConstruct*>();
         pMaskedElsewhereClauseList = new std::vector<MaskedElsewhereClause*>();
         pElsewhereClause = NULL;
         pEndWhereStmt = NULL;
      }
   virtual ~WhereConstruct();

    WhereConstruct* newWhereConstruct()
      {
         WhereConstruct* node = new WhereConstruct();
         node->pWhereConstructStmt = pWhereConstructStmt;  pWhereConstructStmt = NULL;
         delete node->pWhereBodyConstructList; node->pWhereBodyConstructList = pWhereBodyConstructList;  pWhereBodyConstructList = NULL;
         delete node->pMaskedElsewhereClauseList; node->pMaskedElsewhereClauseList = pMaskedElsewhereClauseList;  pMaskedElsewhereClauseList = NULL;
         node->pElsewhereClause = pElsewhereClause;  pElsewhereClause = NULL;
         node->pEndWhereStmt = pEndWhereStmt;  pEndWhereStmt = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    WhereConstructStmt* getWhereConstructStmt() {return pWhereConstructStmt;}
    std::vector<WhereBodyConstruct*>* getWhereBodyConstructList() {return pWhereBodyConstructList;}
    std::vector<MaskedElsewhereClause*>* getMaskedElsewhereClauseList() {return pMaskedElsewhereClauseList;}
    ElsewhereClause* getElsewhereClause() {return pElsewhereClause;}
    EndWhereStmt* getEndWhereStmt() {return pEndWhereStmt;}

    void setWhereConstructStmt(WhereConstructStmt* whereconstructstmt) {pWhereConstructStmt = whereconstructstmt;}
    void appendWhereBodyConstruct(WhereBodyConstruct* wherebodyconstruct) {pWhereBodyConstructList->push_back(wherebodyconstruct);}
    void appendMaskedElsewhereClause(MaskedElsewhereClause* maskedelsewhereclause) {pMaskedElsewhereClauseList->push_back(maskedelsewhereclause);}
    void setElsewhereClause(ElsewhereClause* elsewhereclause) {pElsewhereClause = elsewhereclause;}
    void setEndWhereStmt(EndWhereStmt* endwherestmt) {pEndWhereStmt = endwherestmt;}

 private:
    WhereConstructStmt* pWhereConstructStmt;
    std::vector<WhereBodyConstruct*>* pWhereBodyConstructList;
    std::vector<MaskedElsewhereClause*>* pMaskedElsewhereClauseList;
    ElsewhereClause* pElsewhereClause;
    EndWhereStmt* pEndWhereStmt;
};

class MaskedElsewhereClause : public Node
{
 public:
    MaskedElsewhereClause()
      {
         pMaskedElsewhereStmt = NULL;
         pWhereBodyConstructList = new std::vector<WhereBodyConstruct*>();
      }
   virtual ~MaskedElsewhereClause();

    MaskedElsewhereClause* newMaskedElsewhereClause()
      {
         MaskedElsewhereClause* node = new MaskedElsewhereClause();
         node->pMaskedElsewhereStmt = pMaskedElsewhereStmt;  pMaskedElsewhereStmt = NULL;
         delete node->pWhereBodyConstructList; node->pWhereBodyConstructList = pWhereBodyConstructList;  pWhereBodyConstructList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    MaskedElsewhereStmt* getMaskedElsewhereStmt() {return pMaskedElsewhereStmt;}
    std::vector<WhereBodyConstruct*>* getWhereBodyConstructList() {return pWhereBodyConstructList;}

    void setMaskedElsewhereStmt(MaskedElsewhereStmt* maskedelsewherestmt) {pMaskedElsewhereStmt = maskedelsewherestmt;}
    void appendWhereBodyConstruct(WhereBodyConstruct* wherebodyconstruct) {pWhereBodyConstructList->push_back(wherebodyconstruct);}

 private:
    MaskedElsewhereStmt* pMaskedElsewhereStmt;
    std::vector<WhereBodyConstruct*>* pWhereBodyConstructList;
};

class ElsewhereClause : public Node
{
 public:
    ElsewhereClause()
      {
         pElsewhereStmt = NULL;
         pWhereBodyConstructList = new std::vector<WhereBodyConstruct*>();
      }
   virtual ~ElsewhereClause();

    ElsewhereClause* newElsewhereClause()
      {
         ElsewhereClause* node = new ElsewhereClause();
         node->pElsewhereStmt = pElsewhereStmt;  pElsewhereStmt = NULL;
         delete node->pWhereBodyConstructList; node->pWhereBodyConstructList = pWhereBodyConstructList;  pWhereBodyConstructList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    ElsewhereStmt* getElsewhereStmt() {return pElsewhereStmt;}
    std::vector<WhereBodyConstruct*>* getWhereBodyConstructList() {return pWhereBodyConstructList;}

    void setElsewhereStmt(ElsewhereStmt* elsewherestmt) {pElsewhereStmt = elsewherestmt;}
    void appendWhereBodyConstruct(WhereBodyConstruct* wherebodyconstruct) {pWhereBodyConstructList->push_back(wherebodyconstruct);}

 private:
    ElsewhereStmt* pElsewhereStmt;
    std::vector<WhereBodyConstruct*>* pWhereBodyConstructList;
};

class WhereConstructStmt : public Statement
{
 public:
    WhereConstructStmt()
      {
         pLabel = NULL;
         pIdent = NULL;
         pMaskExpr = NULL;
         pEOS = NULL;
      }
   virtual ~WhereConstructStmt();

    WhereConstructStmt* newWhereConstructStmt()
      {
         WhereConstructStmt* node = new WhereConstructStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pIdent = pIdent;  pIdent = NULL;
         node->pMaskExpr = pMaskExpr;  pMaskExpr = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Ident* getIdent() {return pIdent;}
    MaskExpr* getMaskExpr() {return pMaskExpr;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setIdent(Ident* ident) {pIdent = ident;}
    void setMaskExpr(MaskExpr* maskexpr) {pMaskExpr = maskexpr;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Ident* pIdent;
    MaskExpr* pMaskExpr;
    EOS* pEOS;
};

class WhereBodyConstruct : public Node
{
 public:
    WhereBodyConstruct()
      {
         pWhereConstruct = NULL;
         pWhereStmt = NULL;
         pWhereAssignmentStmt = NULL;
      }
   virtual ~WhereBodyConstruct();

   enum OptionType
     {
        DEFAULT = 0,
        WhereBodyConstruct_WC,
        WhereBodyConstruct_WS,
        WhereBodyConstruct_WAS
     };

    WhereBodyConstruct* newWhereBodyConstruct()
      {
         WhereBodyConstruct* node = new WhereBodyConstruct();
         node->pWhereConstruct = pWhereConstruct;  pWhereConstruct = NULL;
         node->pWhereStmt = pWhereStmt;  pWhereStmt = NULL;
         node->pWhereAssignmentStmt = pWhereAssignmentStmt;  pWhereAssignmentStmt = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    WhereConstruct* getWhereConstruct() {return pWhereConstruct;}
    WhereStmt* getWhereStmt() {return pWhereStmt;}
    WhereAssignmentStmt* getWhereAssignmentStmt() {return pWhereAssignmentStmt;}

    void setWhereConstruct(WhereConstruct* whereconstruct) {pWhereConstruct = whereconstruct;}
    void setWhereStmt(WhereStmt* wherestmt) {pWhereStmt = wherestmt;}
    void setWhereAssignmentStmt(WhereAssignmentStmt* whereassignmentstmt) {pWhereAssignmentStmt = whereassignmentstmt;}

 private:
    WhereConstruct* pWhereConstruct;
    WhereStmt* pWhereStmt;
    WhereAssignmentStmt* pWhereAssignmentStmt;
};

class WhereAssignmentStmt : public Node
{
 public:
    WhereAssignmentStmt()
      {
         pAssignmentStmt = NULL;
      }
   virtual ~WhereAssignmentStmt();

    WhereAssignmentStmt* newWhereAssignmentStmt()
      {
         WhereAssignmentStmt* node = new WhereAssignmentStmt();
         node->pAssignmentStmt = pAssignmentStmt;  pAssignmentStmt = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    AssignmentStmt* getAssignmentStmt() {return pAssignmentStmt;}

    void setAssignmentStmt(AssignmentStmt* assignmentstmt) {pAssignmentStmt = assignmentstmt;}

 private:
    AssignmentStmt* pAssignmentStmt;
};

class MaskExpr : public Node
{
 public:
    MaskExpr()
      {
         pLogicalExpr = NULL;
      }
   virtual ~MaskExpr();

    MaskExpr* newMaskExpr()
      {
         MaskExpr* node = new MaskExpr();
         node->pLogicalExpr = pLogicalExpr;  pLogicalExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    LogicalExpr* getLogicalExpr() {return pLogicalExpr;}

    void setLogicalExpr(LogicalExpr* logicalexpr) {pLogicalExpr = logicalexpr;}

 private:
    LogicalExpr* pLogicalExpr;
};

class MaskedElsewhereStmt : public Statement
{
 public:
    MaskedElsewhereStmt()
      {
         pLabel = NULL;
         pMaskExpr = NULL;
         pIdent = NULL;
         pEOS = NULL;
      }
   virtual ~MaskedElsewhereStmt();

    MaskedElsewhereStmt* newMaskedElsewhereStmt()
      {
         MaskedElsewhereStmt* node = new MaskedElsewhereStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pMaskExpr = pMaskExpr;  pMaskExpr = NULL;
         node->pIdent = pIdent;  pIdent = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    MaskExpr* getMaskExpr() {return pMaskExpr;}
    Ident* getIdent() {return pIdent;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setMaskExpr(MaskExpr* maskexpr) {pMaskExpr = maskexpr;}
    void setIdent(Ident* ident) {pIdent = ident;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    MaskExpr* pMaskExpr;
    Ident* pIdent;
    EOS* pEOS;
};

class ElsewhereStmt : public Statement
{
 public:
    ElsewhereStmt()
      {
         pLabel = NULL;
         pIdent = NULL;
         pEOS = NULL;
      }
   virtual ~ElsewhereStmt();

    ElsewhereStmt* newElsewhereStmt()
      {
         ElsewhereStmt* node = new ElsewhereStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pIdent = pIdent;  pIdent = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Ident* getIdent() {return pIdent;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setIdent(Ident* ident) {pIdent = ident;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Ident* pIdent;
    EOS* pEOS;
};

class EndWhereStmt : public Statement
{
 public:
    EndWhereStmt()
      {
         pLabel = NULL;
         pIdent = NULL;
         pEOS = NULL;
      }
   virtual ~EndWhereStmt();

    EndWhereStmt* newEndWhereStmt()
      {
         EndWhereStmt* node = new EndWhereStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pIdent = pIdent;  pIdent = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Ident* getIdent() {return pIdent;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setIdent(Ident* ident) {pIdent = ident;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Ident* pIdent;
    EOS* pEOS;
};

class ForallConstruct : public Node
{
 public:
    ForallConstruct()
      {
         pForallConstructStmt = NULL;
         pForallBodyConstructList = new std::vector<ForallBodyConstruct*>();
         pEndForallStmt = NULL;
      }
   virtual ~ForallConstruct();

    ForallConstruct* newForallConstruct()
      {
         ForallConstruct* node = new ForallConstruct();
         node->pForallConstructStmt = pForallConstructStmt;  pForallConstructStmt = NULL;
         delete node->pForallBodyConstructList; node->pForallBodyConstructList = pForallBodyConstructList;  pForallBodyConstructList = NULL;
         node->pEndForallStmt = pEndForallStmt;  pEndForallStmt = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    ForallConstructStmt* getForallConstructStmt() {return pForallConstructStmt;}
    std::vector<ForallBodyConstruct*>* getForallBodyConstructList() {return pForallBodyConstructList;}
    EndForallStmt* getEndForallStmt() {return pEndForallStmt;}

    void setForallConstructStmt(ForallConstructStmt* forallconstructstmt) {pForallConstructStmt = forallconstructstmt;}
    void appendForallBodyConstruct(ForallBodyConstruct* forallbodyconstruct) {pForallBodyConstructList->push_back(forallbodyconstruct);}
    void setEndForallStmt(EndForallStmt* endforallstmt) {pEndForallStmt = endforallstmt;}

 private:
    ForallConstructStmt* pForallConstructStmt;
    std::vector<ForallBodyConstruct*>* pForallBodyConstructList;
    EndForallStmt* pEndForallStmt;
};

class ForallConstructStmt : public Statement
{
 public:
    ForallConstructStmt()
      {
         pLabel = NULL;
         pForallConstructName = NULL;
         pForallHeader = NULL;
         pEOS = NULL;
      }
   virtual ~ForallConstructStmt();

    ForallConstructStmt* newForallConstructStmt()
      {
         ForallConstructStmt* node = new ForallConstructStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pForallConstructName = pForallConstructName;  pForallConstructName = NULL;
         node->pForallHeader = pForallHeader;  pForallHeader = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Name* getForallConstructName() {return pForallConstructName;}
    ForallHeader* getForallHeader() {return pForallHeader;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setForallConstructName(Name* forallconstructname) {pForallConstructName = forallconstructname;}
    void setForallHeader(ForallHeader* forallheader) {pForallHeader = forallheader;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Name* pForallConstructName;
    ForallHeader* pForallHeader;
    EOS* pEOS;
};

class ForallHeader : public Node
{
 public:
    ForallHeader()
      {
         pTypeSpec = NULL;
         pForallTripletSpecList = NULL;
         pMaskExpr = NULL;
      }
   virtual ~ForallHeader();

    ForallHeader* newForallHeader()
      {
         ForallHeader* node = new ForallHeader();
         node->pTypeSpec = pTypeSpec;  pTypeSpec = NULL;
         node->pForallTripletSpecList = pForallTripletSpecList;  pForallTripletSpecList = NULL;
         node->pMaskExpr = pMaskExpr;  pMaskExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    TypeSpec* getTypeSpec() {return pTypeSpec;}
    ForallTripletSpecList* getForallTripletSpecList() {return pForallTripletSpecList;}
    MaskExpr* getMaskExpr() {return pMaskExpr;}

    void setTypeSpec(TypeSpec* typespec) {pTypeSpec = typespec;}
    void setForallTripletSpecList(ForallTripletSpecList* foralltripletspeclist) {pForallTripletSpecList = foralltripletspeclist;}
    void setMaskExpr(MaskExpr* maskexpr) {pMaskExpr = maskexpr;}

 private:
    TypeSpec* pTypeSpec;
    ForallTripletSpecList* pForallTripletSpecList;
    MaskExpr* pMaskExpr;
};

class ForallTripletSpec : public Node
{
 public:
    ForallTripletSpec()
      {
         pIndexName = NULL;
         pForallLimit = NULL;
         pForallStep = NULL;
      }
   virtual ~ForallTripletSpec();

    ForallTripletSpec* newForallTripletSpec()
      {
         ForallTripletSpec* node = new ForallTripletSpec();
         node->pIndexName = pIndexName;  pIndexName = NULL;
         node->pForallLimit = pForallLimit;  pForallLimit = NULL;
         node->pForallStep = pForallStep;  pForallStep = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Name* getIndexName() {return pIndexName;}
    ForallLimit* getForallLimit() {return pForallLimit;}
    ForallStep* getForallStep() {return pForallStep;}

    void setIndexName(Name* indexname) {pIndexName = indexname;}
    void setForallLimit(ForallLimit* foralllimit) {pForallLimit = foralllimit;}
    void setForallStep(ForallStep* forallstep) {pForallStep = forallstep;}

 private:
    Name* pIndexName;
    ForallLimit* pForallLimit;
    ForallStep* pForallStep;
};

class ForallTripletSpecList : public Node
{
 public:
    ForallTripletSpecList()
      {
         pForallTripletSpecList = new std::vector<ForallTripletSpec*>();
      }
   virtual ~ForallTripletSpecList();

    ForallTripletSpecList* newForallTripletSpecList()
      {
         ForallTripletSpecList* node = new ForallTripletSpecList();
         delete node->pForallTripletSpecList; node->pForallTripletSpecList = pForallTripletSpecList;  pForallTripletSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<ForallTripletSpec*>* getForallTripletSpecList() {return pForallTripletSpecList;}

    void appendForallTripletSpec(ForallTripletSpec* foralltripletspec) {pForallTripletSpecList->push_back(foralltripletspec);}

 private:
    std::vector<ForallTripletSpec*>* pForallTripletSpecList;
};

class ForallLimit : public Node
{
 public:
    ForallLimit()
      {
         pIntExpr = NULL;
      }
   virtual ~ForallLimit();

    ForallLimit* newForallLimit()
      {
         ForallLimit* node = new ForallLimit();
         node->pIntExpr = pIntExpr;  pIntExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    IntExpr* getIntExpr() {return pIntExpr;}

    void setIntExpr(IntExpr* intexpr) {pIntExpr = intexpr;}

 private:
    IntExpr* pIntExpr;
};

class ForallStep : public Node
{
 public:
    ForallStep()
      {
         pIntExpr = NULL;
      }
   virtual ~ForallStep();

    ForallStep* newForallStep()
      {
         ForallStep* node = new ForallStep();
         node->pIntExpr = pIntExpr;  pIntExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    IntExpr* getIntExpr() {return pIntExpr;}

    void setIntExpr(IntExpr* intexpr) {pIntExpr = intexpr;}

 private:
    IntExpr* pIntExpr;
};

class ForallBodyConstruct : public Node
{
 public:
    ForallBodyConstruct()
      {
         pForallStmt = NULL;
         pForallConstruct = NULL;
         pWhereConstruct = NULL;
         pWhereStmt = NULL;
         pForallAssignmentStmt = NULL;
      }
   virtual ~ForallBodyConstruct();

   enum OptionType
     {
        DEFAULT = 0,
        ForallBodyConstruct_FS,
        ForallBodyConstruct_FC,
        ForallBodyConstruct_WC,
        ForallBodyConstruct_WS,
        ForallBodyConstruct_FAS
     };

    ForallBodyConstruct* newForallBodyConstruct()
      {
         ForallBodyConstruct* node = new ForallBodyConstruct();
         node->pForallStmt = pForallStmt;  pForallStmt = NULL;
         node->pForallConstruct = pForallConstruct;  pForallConstruct = NULL;
         node->pWhereConstruct = pWhereConstruct;  pWhereConstruct = NULL;
         node->pWhereStmt = pWhereStmt;  pWhereStmt = NULL;
         node->pForallAssignmentStmt = pForallAssignmentStmt;  pForallAssignmentStmt = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    ForallStmt* getForallStmt() {return pForallStmt;}
    ForallConstruct* getForallConstruct() {return pForallConstruct;}
    WhereConstruct* getWhereConstruct() {return pWhereConstruct;}
    WhereStmt* getWhereStmt() {return pWhereStmt;}
    ForallAssignmentStmt* getForallAssignmentStmt() {return pForallAssignmentStmt;}

    void setForallStmt(ForallStmt* forallstmt) {pForallStmt = forallstmt;}
    void setForallConstruct(ForallConstruct* forallconstruct) {pForallConstruct = forallconstruct;}
    void setWhereConstruct(WhereConstruct* whereconstruct) {pWhereConstruct = whereconstruct;}
    void setWhereStmt(WhereStmt* wherestmt) {pWhereStmt = wherestmt;}
    void setForallAssignmentStmt(ForallAssignmentStmt* forallassignmentstmt) {pForallAssignmentStmt = forallassignmentstmt;}

 private:
    ForallStmt* pForallStmt;
    ForallConstruct* pForallConstruct;
    WhereConstruct* pWhereConstruct;
    WhereStmt* pWhereStmt;
    ForallAssignmentStmt* pForallAssignmentStmt;
};

class ForallAssignmentStmt : public Node
{
 public:
    ForallAssignmentStmt()
      {
         pPointerAssignmentStmt = NULL;
         pAssignmentStmt = NULL;
      }
   virtual ~ForallAssignmentStmt();

   enum OptionType
     {
        DEFAULT = 0,
        ForallAssignmentStmt_PAS,
        ForallAssignmentStmt_AS
     };

    ForallAssignmentStmt* newForallAssignmentStmt()
      {
         ForallAssignmentStmt* node = new ForallAssignmentStmt();
         node->pPointerAssignmentStmt = pPointerAssignmentStmt;  pPointerAssignmentStmt = NULL;
         node->pAssignmentStmt = pAssignmentStmt;  pAssignmentStmt = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    PointerAssignmentStmt* getPointerAssignmentStmt() {return pPointerAssignmentStmt;}
    AssignmentStmt* getAssignmentStmt() {return pAssignmentStmt;}

    void setPointerAssignmentStmt(PointerAssignmentStmt* pointerassignmentstmt) {pPointerAssignmentStmt = pointerassignmentstmt;}
    void setAssignmentStmt(AssignmentStmt* assignmentstmt) {pAssignmentStmt = assignmentstmt;}

 private:
    PointerAssignmentStmt* pPointerAssignmentStmt;
    AssignmentStmt* pAssignmentStmt;
};

class EndForallStmt : public Statement
{
 public:
    EndForallStmt()
      {
         pLabel = NULL;
         pForallConstructName = NULL;
         pEOS = NULL;
      }
   virtual ~EndForallStmt();

    EndForallStmt* newEndForallStmt()
      {
         EndForallStmt* node = new EndForallStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pForallConstructName = pForallConstructName;  pForallConstructName = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Name* getForallConstructName() {return pForallConstructName;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setForallConstructName(Name* forallconstructname) {pForallConstructName = forallconstructname;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Name* pForallConstructName;
    EOS* pEOS;
};

class ForallStmt : public Node
{
 public:
    ForallStmt()
      {
         pLabel = NULL;
         pForallHeader = NULL;
         pForallAssignmentStmt = NULL;
      }
   virtual ~ForallStmt();

    ForallStmt* newForallStmt()
      {
         ForallStmt* node = new ForallStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pForallHeader = pForallHeader;  pForallHeader = NULL;
         node->pForallAssignmentStmt = pForallAssignmentStmt;  pForallAssignmentStmt = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    ForallHeader* getForallHeader() {return pForallHeader;}
    ForallAssignmentStmt* getForallAssignmentStmt() {return pForallAssignmentStmt;}

    void setLabel(Label* label) {pLabel = label;}
    void setForallHeader(ForallHeader* forallheader) {pForallHeader = forallheader;}
    void setForallAssignmentStmt(ForallAssignmentStmt* forallassignmentstmt) {pForallAssignmentStmt = forallassignmentstmt;}

 private:
    Label* pLabel;
    ForallHeader* pForallHeader;
    ForallAssignmentStmt* pForallAssignmentStmt;
};

class Block : public Node
{
 public:
    Block()
      {
         pExecutionPartConstructList = new std::vector<ExecutionPartConstruct*>();
      }
   virtual ~Block();

    Block* newBlock()
      {
         Block* node = new Block();
         delete node->pExecutionPartConstructList; node->pExecutionPartConstructList = pExecutionPartConstructList;  pExecutionPartConstructList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<ExecutionPartConstruct*>* getExecutionPartConstructList() {return pExecutionPartConstructList;}

    void appendExecutionPartConstruct(ExecutionPartConstruct* executionpartconstruct) {pExecutionPartConstructList->push_back(executionpartconstruct);}

 private:
    std::vector<ExecutionPartConstruct*>* pExecutionPartConstructList;
};

class AssociateConstruct : public Node
{
 public:
    AssociateConstruct()
      {
         pAssociateStmt = NULL;
         pBlock = NULL;
         pEndAssociateStmt = NULL;
      }
   virtual ~AssociateConstruct();

    AssociateConstruct* newAssociateConstruct()
      {
         AssociateConstruct* node = new AssociateConstruct();
         node->pAssociateStmt = pAssociateStmt;  pAssociateStmt = NULL;
         node->pBlock = pBlock;  pBlock = NULL;
         node->pEndAssociateStmt = pEndAssociateStmt;  pEndAssociateStmt = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    AssociateStmt* getAssociateStmt() {return pAssociateStmt;}
    Block* getBlock() {return pBlock;}
    EndAssociateStmt* getEndAssociateStmt() {return pEndAssociateStmt;}

    void setAssociateStmt(AssociateStmt* associatestmt) {pAssociateStmt = associatestmt;}
    void setBlock(Block* block) {pBlock = block;}
    void setEndAssociateStmt(EndAssociateStmt* endassociatestmt) {pEndAssociateStmt = endassociatestmt;}

 private:
    AssociateStmt* pAssociateStmt;
    Block* pBlock;
    EndAssociateStmt* pEndAssociateStmt;
};

class AssociateStmt : public Statement
{
 public:
    AssociateStmt()
      {
         pLabel = NULL;
         pAssociateConstructName = NULL;
         pAssociationList = NULL;
         pEOS = NULL;
      }
   virtual ~AssociateStmt();

    AssociateStmt* newAssociateStmt()
      {
         AssociateStmt* node = new AssociateStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pAssociateConstructName = pAssociateConstructName;  pAssociateConstructName = NULL;
         node->pAssociationList = pAssociationList;  pAssociationList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Name* getAssociateConstructName() {return pAssociateConstructName;}
    AssociationList* getAssociationList() {return pAssociationList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setAssociateConstructName(Name* associateconstructname) {pAssociateConstructName = associateconstructname;}
    void setAssociationList(AssociationList* associationlist) {pAssociationList = associationlist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Name* pAssociateConstructName;
    AssociationList* pAssociationList;
    EOS* pEOS;
};

class Association : public Node
{
 public:
    Association()
      {
         pAssociateName = NULL;
         pSelector = NULL;
      }
   virtual ~Association();

    Association* newAssociation()
      {
         Association* node = new Association();
         node->pAssociateName = pAssociateName;  pAssociateName = NULL;
         node->pSelector = pSelector;  pSelector = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Name* getAssociateName() {return pAssociateName;}
    Selector* getSelector() {return pSelector;}

    void setAssociateName(Name* associatename) {pAssociateName = associatename;}
    void setSelector(Selector* selector) {pSelector = selector;}

 private:
    Name* pAssociateName;
    Selector* pSelector;
};

class AssociationList : public Node
{
 public:
    AssociationList()
      {
         pAssociationList = new std::vector<Association*>();
      }
   virtual ~AssociationList();

    AssociationList* newAssociationList()
      {
         AssociationList* node = new AssociationList();
         delete node->pAssociationList; node->pAssociationList = pAssociationList;  pAssociationList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<Association*>* getAssociationList() {return pAssociationList;}

    void appendAssociation(Association* association) {pAssociationList->push_back(association);}

 private:
    std::vector<Association*>* pAssociationList;
};

class Selector : public Node
{
 public:
    Selector()
      {
         pVariable = NULL;
         pExpr = NULL;
      }
   virtual ~Selector();

   enum OptionType
     {
        DEFAULT = 0,
        Selector_V,
        Selector_E
     };

    Selector* newSelector()
      {
         Selector* node = new Selector();
         node->pVariable = pVariable;  pVariable = NULL;
         node->pExpr = pExpr;  pExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Variable* getVariable() {return pVariable;}
    Expr* getExpr() {return pExpr;}

    void setVariable(Variable* variable) {pVariable = variable;}
    void setExpr(Expr* expr) {pExpr = expr;}

 private:
    Variable* pVariable;
    Expr* pExpr;
};

class EndAssociateStmt : public Statement
{
 public:
    EndAssociateStmt()
      {
         pLabel = NULL;
         pAssociateConstructName = NULL;
         pEOS = NULL;
      }
   virtual ~EndAssociateStmt();

    EndAssociateStmt* newEndAssociateStmt()
      {
         EndAssociateStmt* node = new EndAssociateStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pAssociateConstructName = pAssociateConstructName;  pAssociateConstructName = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Name* getAssociateConstructName() {return pAssociateConstructName;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setAssociateConstructName(Name* associateconstructname) {pAssociateConstructName = associateconstructname;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Name* pAssociateConstructName;
    EOS* pEOS;
};

class BlockConstruct : public Node
{
 public:
    BlockConstruct()
      {
         pBlockStmt = NULL;
         pSpecificationPart = NULL;
         pBlock = NULL;
         pEndBlockStmt = NULL;
      }
   virtual ~BlockConstruct();

    BlockConstruct* newBlockConstruct()
      {
         BlockConstruct* node = new BlockConstruct();
         node->pBlockStmt = pBlockStmt;  pBlockStmt = NULL;
         node->pSpecificationPart = pSpecificationPart;  pSpecificationPart = NULL;
         node->pBlock = pBlock;  pBlock = NULL;
         node->pEndBlockStmt = pEndBlockStmt;  pEndBlockStmt = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    BlockStmt* getBlockStmt() {return pBlockStmt;}
    SpecificationPart* getSpecificationPart() {return pSpecificationPart;}
    Block* getBlock() {return pBlock;}
    EndBlockStmt* getEndBlockStmt() {return pEndBlockStmt;}

    void setBlockStmt(BlockStmt* blockstmt) {pBlockStmt = blockstmt;}
    void setSpecificationPart(SpecificationPart* specificationpart) {pSpecificationPart = specificationpart;}
    void setBlock(Block* block) {pBlock = block;}
    void setEndBlockStmt(EndBlockStmt* endblockstmt) {pEndBlockStmt = endblockstmt;}

 private:
    BlockStmt* pBlockStmt;
    SpecificationPart* pSpecificationPart;
    Block* pBlock;
    EndBlockStmt* pEndBlockStmt;
};

class BlockStmt : public Statement
{
 public:
    BlockStmt()
      {
         pLabel = NULL;
         pBlockConstructName = NULL;
         pEOS = NULL;
      }
   virtual ~BlockStmt();

    BlockStmt* newBlockStmt()
      {
         BlockStmt* node = new BlockStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pBlockConstructName = pBlockConstructName;  pBlockConstructName = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Name* getBlockConstructName() {return pBlockConstructName;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setBlockConstructName(Name* blockconstructname) {pBlockConstructName = blockconstructname;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Name* pBlockConstructName;
    EOS* pEOS;
};

class EndBlockStmt : public Statement
{
 public:
    EndBlockStmt()
      {
         pLabel = NULL;
         pBlockConstructName = NULL;
         pEOS = NULL;
      }
   virtual ~EndBlockStmt();

    EndBlockStmt* newEndBlockStmt()
      {
         EndBlockStmt* node = new EndBlockStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pBlockConstructName = pBlockConstructName;  pBlockConstructName = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Name* getBlockConstructName() {return pBlockConstructName;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setBlockConstructName(Name* blockconstructname) {pBlockConstructName = blockconstructname;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Name* pBlockConstructName;
    EOS* pEOS;
};

class CriticalConstruct : public Node
{
 public:
    CriticalConstruct()
      {
         pCriticalStmt = NULL;
         pBlock = NULL;
         pEndCriticalStmt = NULL;
      }
   virtual ~CriticalConstruct();

    CriticalConstruct* newCriticalConstruct()
      {
         CriticalConstruct* node = new CriticalConstruct();
         node->pCriticalStmt = pCriticalStmt;  pCriticalStmt = NULL;
         node->pBlock = pBlock;  pBlock = NULL;
         node->pEndCriticalStmt = pEndCriticalStmt;  pEndCriticalStmt = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    CriticalStmt* getCriticalStmt() {return pCriticalStmt;}
    Block* getBlock() {return pBlock;}
    EndCriticalStmt* getEndCriticalStmt() {return pEndCriticalStmt;}

    void setCriticalStmt(CriticalStmt* criticalstmt) {pCriticalStmt = criticalstmt;}
    void setBlock(Block* block) {pBlock = block;}
    void setEndCriticalStmt(EndCriticalStmt* endcriticalstmt) {pEndCriticalStmt = endcriticalstmt;}

 private:
    CriticalStmt* pCriticalStmt;
    Block* pBlock;
    EndCriticalStmt* pEndCriticalStmt;
};

class CriticalStmt : public Statement
{
 public:
    CriticalStmt()
      {
         pLabel = NULL;
         pCriticalConstructName = NULL;
         pEOS = NULL;
      }
   virtual ~CriticalStmt();

    CriticalStmt* newCriticalStmt()
      {
         CriticalStmt* node = new CriticalStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pCriticalConstructName = pCriticalConstructName;  pCriticalConstructName = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Name* getCriticalConstructName() {return pCriticalConstructName;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setCriticalConstructName(Name* criticalconstructname) {pCriticalConstructName = criticalconstructname;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Name* pCriticalConstructName;
    EOS* pEOS;
};

class EndCriticalStmt : public Statement
{
 public:
    EndCriticalStmt()
      {
         pLabel = NULL;
         pCriticalConstructName = NULL;
         pEOS = NULL;
      }
   virtual ~EndCriticalStmt();

    EndCriticalStmt* newEndCriticalStmt()
      {
         EndCriticalStmt* node = new EndCriticalStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pCriticalConstructName = pCriticalConstructName;  pCriticalConstructName = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Name* getCriticalConstructName() {return pCriticalConstructName;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setCriticalConstructName(Name* criticalconstructname) {pCriticalConstructName = criticalconstructname;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Name* pCriticalConstructName;
    EOS* pEOS;
};

class LabelDoStmt : public Statement
{
 public:
    LabelDoStmt()
      {
         pLabel = NULL;
         pDoConstructName = NULL;
         pLblRef = NULL;
         pLoopControl = NULL;
         pEOS = NULL;
      }
   virtual ~LabelDoStmt();

    LabelDoStmt* newLabelDoStmt()
      {
         LabelDoStmt* node = new LabelDoStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pDoConstructName = pDoConstructName;  pDoConstructName = NULL;
         node->pLblRef = pLblRef;  pLblRef = NULL;
         node->pLoopControl = pLoopControl;  pLoopControl = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Name* getDoConstructName() {return pDoConstructName;}
    LblRef* getLblRef() {return pLblRef;}
    LoopControl* getLoopControl() {return pLoopControl;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setDoConstructName(Name* doconstructname) {pDoConstructName = doconstructname;}
    void setLblRef(LblRef* lblref) {pLblRef = lblref;}
    void setLoopControl(LoopControl* loopcontrol) {pLoopControl = loopcontrol;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Name* pDoConstructName;
    LblRef* pLblRef;
    LoopControl* pLoopControl;
    EOS* pEOS;
};

class NonlabelDoStmt : public Statement
{
 public:
    NonlabelDoStmt()
      {
         pLabel = NULL;
         pDoConstructName = NULL;
         pLoopControl = NULL;
         pEOS = NULL;
      }
   virtual ~NonlabelDoStmt();

    NonlabelDoStmt* newNonlabelDoStmt()
      {
         NonlabelDoStmt* node = new NonlabelDoStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pDoConstructName = pDoConstructName;  pDoConstructName = NULL;
         node->pLoopControl = pLoopControl;  pLoopControl = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Name* getDoConstructName() {return pDoConstructName;}
    LoopControl* getLoopControl() {return pLoopControl;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setDoConstructName(Name* doconstructname) {pDoConstructName = doconstructname;}
    void setLoopControl(LoopControl* loopcontrol) {pLoopControl = loopcontrol;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Name* pDoConstructName;
    LoopControl* pLoopControl;
    EOS* pEOS;
};

class LoopControl : public Node
{
 public:
    LoopControl()
      {
         pForallHeader = NULL;
         pExpr = NULL;
         pDoVariable = NULL;
      }
   virtual ~LoopControl();

   enum OptionType
     {
        DEFAULT = 0,
        LoopControl_CONCURRENT,
        LoopControl_WHILE,
        LoopControl_DV
     };

    LoopControl* newLoopControl()
      {
         LoopControl* node = new LoopControl();
         node->pForallHeader = pForallHeader;  pForallHeader = NULL;
         node->pExpr = pExpr;  pExpr = NULL;
         node->pDoVariable = pDoVariable;  pDoVariable = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    ForallHeader* getForallHeader() {return pForallHeader;}
    Expr* getExpr() {return pExpr;}
    DoVariable* getDoVariable() {return pDoVariable;}

    void setForallHeader(ForallHeader* forallheader) {pForallHeader = forallheader;}
    void setExpr(Expr* expr) {pExpr = expr;}
    void setDoVariable(DoVariable* dovariable) {pDoVariable = dovariable;}

 private:
    ForallHeader* pForallHeader;
    Expr* pExpr;
    DoVariable* pDoVariable;
};

class DoVariable : public Node
{
 public:
    DoVariable()
      {
         pScalarIntVariableName = NULL;
      }
   virtual ~DoVariable();

    DoVariable* newDoVariable()
      {
         DoVariable* node = new DoVariable();
         node->pScalarIntVariableName = pScalarIntVariableName;  pScalarIntVariableName = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Name* getScalarIntVariableName() {return pScalarIntVariableName;}

    void setScalarIntVariableName(Name* scalarintvariablename) {pScalarIntVariableName = scalarintvariablename;}

 private:
    Name* pScalarIntVariableName;
};

class EndDoStmt : public Statement
{
 public:
    EndDoStmt()
      {
         pLabel = NULL;
         pDoConstructName = NULL;
         pEOS = NULL;
      }
   virtual ~EndDoStmt();

    EndDoStmt* newEndDoStmt()
      {
         EndDoStmt* node = new EndDoStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pDoConstructName = pDoConstructName;  pDoConstructName = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Name* getDoConstructName() {return pDoConstructName;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setDoConstructName(Name* doconstructname) {pDoConstructName = doconstructname;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Name* pDoConstructName;
    EOS* pEOS;
};

class CycleStmt : public Statement
{
 public:
    CycleStmt()
      {
         pLabel = NULL;
         pDoConstructName = NULL;
         pEOS = NULL;
      }
   virtual ~CycleStmt();

    CycleStmt* newCycleStmt()
      {
         CycleStmt* node = new CycleStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pDoConstructName = pDoConstructName;  pDoConstructName = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Name* getDoConstructName() {return pDoConstructName;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setDoConstructName(Name* doconstructname) {pDoConstructName = doconstructname;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Name* pDoConstructName;
    EOS* pEOS;
};

class IfConstruct : public Node
{
 public:
    IfConstruct()
      {
         pIfThenStmt = NULL;
         pBlock = NULL;
         pElseIfStmtAndBlockList = new std::vector<ElseIfStmtAndBlock*>();
         pElseStmtAndBlock = NULL;
         pEndIfStmt = NULL;
      }
   virtual ~IfConstruct();

    IfConstruct* newIfConstruct()
      {
         IfConstruct* node = new IfConstruct();
         node->pIfThenStmt = pIfThenStmt;  pIfThenStmt = NULL;
         node->pBlock = pBlock;  pBlock = NULL;
         delete node->pElseIfStmtAndBlockList; node->pElseIfStmtAndBlockList = pElseIfStmtAndBlockList;  pElseIfStmtAndBlockList = NULL;
         node->pElseStmtAndBlock = pElseStmtAndBlock;  pElseStmtAndBlock = NULL;
         node->pEndIfStmt = pEndIfStmt;  pEndIfStmt = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    IfThenStmt* getIfThenStmt() {return pIfThenStmt;}
    Block* getBlock() {return pBlock;}
    std::vector<ElseIfStmtAndBlock*>* getElseIfStmtAndBlockList() {return pElseIfStmtAndBlockList;}
    ElseStmtAndBlock* getElseStmtAndBlock() {return pElseStmtAndBlock;}
    EndIfStmt* getEndIfStmt() {return pEndIfStmt;}

    void setIfThenStmt(IfThenStmt* ifthenstmt) {pIfThenStmt = ifthenstmt;}
    void setBlock(Block* block) {pBlock = block;}
    void appendElseIfStmtAndBlock(ElseIfStmtAndBlock* elseifstmtandblock) {pElseIfStmtAndBlockList->push_back(elseifstmtandblock);}
    void setElseStmtAndBlock(ElseStmtAndBlock* elsestmtandblock) {pElseStmtAndBlock = elsestmtandblock;}
    void setEndIfStmt(EndIfStmt* endifstmt) {pEndIfStmt = endifstmt;}

 private:
    IfThenStmt* pIfThenStmt;
    Block* pBlock;
    std::vector<ElseIfStmtAndBlock*>* pElseIfStmtAndBlockList;
    ElseStmtAndBlock* pElseStmtAndBlock;
    EndIfStmt* pEndIfStmt;
};

class ElseIfStmtAndBlock : public Node
{
 public:
    ElseIfStmtAndBlock()
      {
         pElseIfStmt = NULL;
         pBlock = NULL;
      }
   virtual ~ElseIfStmtAndBlock();

    ElseIfStmtAndBlock* newElseIfStmtAndBlock()
      {
         ElseIfStmtAndBlock* node = new ElseIfStmtAndBlock();
         node->pElseIfStmt = pElseIfStmt;  pElseIfStmt = NULL;
         node->pBlock = pBlock;  pBlock = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    ElseIfStmt* getElseIfStmt() {return pElseIfStmt;}
    Block* getBlock() {return pBlock;}

    void setElseIfStmt(ElseIfStmt* elseifstmt) {pElseIfStmt = elseifstmt;}
    void setBlock(Block* block) {pBlock = block;}

 private:
    ElseIfStmt* pElseIfStmt;
    Block* pBlock;
};

class ElseStmtAndBlock : public Node
{
 public:
    ElseStmtAndBlock()
      {
         pElseStmt = NULL;
         pBlock = NULL;
      }
   virtual ~ElseStmtAndBlock();

    ElseStmtAndBlock* newElseStmtAndBlock()
      {
         ElseStmtAndBlock* node = new ElseStmtAndBlock();
         node->pElseStmt = pElseStmt;  pElseStmt = NULL;
         node->pBlock = pBlock;  pBlock = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    ElseStmt* getElseStmt() {return pElseStmt;}
    Block* getBlock() {return pBlock;}

    void setElseStmt(ElseStmt* elsestmt) {pElseStmt = elsestmt;}
    void setBlock(Block* block) {pBlock = block;}

 private:
    ElseStmt* pElseStmt;
    Block* pBlock;
};

class IfThenStmt : public Statement
{
 public:
    IfThenStmt()
      {
         pLabel = NULL;
         pIfConstructName = NULL;
         pLogicalExpr = NULL;
         pEOS = NULL;
      }
   virtual ~IfThenStmt();

    IfThenStmt* newIfThenStmt()
      {
         IfThenStmt* node = new IfThenStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pIfConstructName = pIfConstructName;  pIfConstructName = NULL;
         node->pLogicalExpr = pLogicalExpr;  pLogicalExpr = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Name* getIfConstructName() {return pIfConstructName;}
    LogicalExpr* getLogicalExpr() {return pLogicalExpr;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setIfConstructName(Name* ifconstructname) {pIfConstructName = ifconstructname;}
    void setLogicalExpr(LogicalExpr* logicalexpr) {pLogicalExpr = logicalexpr;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Name* pIfConstructName;
    LogicalExpr* pLogicalExpr;
    EOS* pEOS;
};

class ElseIfStmt : public Statement
{
 public:
    ElseIfStmt()
      {
         pLabel = NULL;
         pLogicalExpr = NULL;
         pIfConstructName = NULL;
         pEOS = NULL;
      }
   virtual ~ElseIfStmt();

    ElseIfStmt* newElseIfStmt()
      {
         ElseIfStmt* node = new ElseIfStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pLogicalExpr = pLogicalExpr;  pLogicalExpr = NULL;
         node->pIfConstructName = pIfConstructName;  pIfConstructName = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    LogicalExpr* getLogicalExpr() {return pLogicalExpr;}
    Name* getIfConstructName() {return pIfConstructName;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setLogicalExpr(LogicalExpr* logicalexpr) {pLogicalExpr = logicalexpr;}
    void setIfConstructName(Name* ifconstructname) {pIfConstructName = ifconstructname;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    LogicalExpr* pLogicalExpr;
    Name* pIfConstructName;
    EOS* pEOS;
};

class ElseStmt : public Statement
{
 public:
    ElseStmt()
      {
         pLabel = NULL;
         pIfConstructName = NULL;
         pEOS = NULL;
      }
   virtual ~ElseStmt();

    ElseStmt* newElseStmt()
      {
         ElseStmt* node = new ElseStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pIfConstructName = pIfConstructName;  pIfConstructName = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Name* getIfConstructName() {return pIfConstructName;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setIfConstructName(Name* ifconstructname) {pIfConstructName = ifconstructname;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Name* pIfConstructName;
    EOS* pEOS;
};

class EndIfStmt : public Statement
{
 public:
    EndIfStmt()
      {
         pLabel = NULL;
         pIfConstructName = NULL;
         pEOS = NULL;
      }
   virtual ~EndIfStmt();

    EndIfStmt* newEndIfStmt()
      {
         EndIfStmt* node = new EndIfStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pIfConstructName = pIfConstructName;  pIfConstructName = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Name* getIfConstructName() {return pIfConstructName;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setIfConstructName(Name* ifconstructname) {pIfConstructName = ifconstructname;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Name* pIfConstructName;
    EOS* pEOS;
};

class IfStmt : public Node
{
 public:
    IfStmt()
      {
         pLabel = NULL;
         pLogicalExpr = NULL;
         pActionStmt = NULL;
      }
   virtual ~IfStmt();

    IfStmt* newIfStmt()
      {
         IfStmt* node = new IfStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pLogicalExpr = pLogicalExpr;  pLogicalExpr = NULL;
         node->pActionStmt = pActionStmt;  pActionStmt = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    LogicalExpr* getLogicalExpr() {return pLogicalExpr;}
    ActionStmt* getActionStmt() {return pActionStmt;}

    void setLabel(Label* label) {pLabel = label;}
    void setLogicalExpr(LogicalExpr* logicalexpr) {pLogicalExpr = logicalexpr;}
    void setActionStmt(ActionStmt* actionstmt) {pActionStmt = actionstmt;}

 private:
    Label* pLabel;
    LogicalExpr* pLogicalExpr;
    ActionStmt* pActionStmt;
};

class CaseConstruct : public Node
{
 public:
    CaseConstruct()
      {
         pSelectCaseStmt = NULL;
         pCaseStmtAndBlockList = new std::vector<CaseStmtAndBlock*>();
         pEndSelectStmt = NULL;
      }
   virtual ~CaseConstruct();

    CaseConstruct* newCaseConstruct()
      {
         CaseConstruct* node = new CaseConstruct();
         node->pSelectCaseStmt = pSelectCaseStmt;  pSelectCaseStmt = NULL;
         delete node->pCaseStmtAndBlockList; node->pCaseStmtAndBlockList = pCaseStmtAndBlockList;  pCaseStmtAndBlockList = NULL;
         node->pEndSelectStmt = pEndSelectStmt;  pEndSelectStmt = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    SelectCaseStmt* getSelectCaseStmt() {return pSelectCaseStmt;}
    std::vector<CaseStmtAndBlock*>* getCaseStmtAndBlockList() {return pCaseStmtAndBlockList;}
    EndSelectStmt* getEndSelectStmt() {return pEndSelectStmt;}

    void setSelectCaseStmt(SelectCaseStmt* selectcasestmt) {pSelectCaseStmt = selectcasestmt;}
    void appendCaseStmtAndBlock(CaseStmtAndBlock* casestmtandblock) {pCaseStmtAndBlockList->push_back(casestmtandblock);}
    void setEndSelectStmt(EndSelectStmt* endselectstmt) {pEndSelectStmt = endselectstmt;}

 private:
    SelectCaseStmt* pSelectCaseStmt;
    std::vector<CaseStmtAndBlock*>* pCaseStmtAndBlockList;
    EndSelectStmt* pEndSelectStmt;
};

class CaseStmtAndBlock : public Node
{
 public:
    CaseStmtAndBlock()
      {
         pCaseStmt = NULL;
         pBlock = NULL;
      }
   virtual ~CaseStmtAndBlock();

    CaseStmtAndBlock* newCaseStmtAndBlock()
      {
         CaseStmtAndBlock* node = new CaseStmtAndBlock();
         node->pCaseStmt = pCaseStmt;  pCaseStmt = NULL;
         node->pBlock = pBlock;  pBlock = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    CaseStmt* getCaseStmt() {return pCaseStmt;}
    Block* getBlock() {return pBlock;}

    void setCaseStmt(CaseStmt* casestmt) {pCaseStmt = casestmt;}
    void setBlock(Block* block) {pBlock = block;}

 private:
    CaseStmt* pCaseStmt;
    Block* pBlock;
};

class SelectCaseStmt : public Statement
{
 public:
    SelectCaseStmt()
      {
         pLabel = NULL;
         pCaseConstructName = NULL;
         pCaseExpr = NULL;
         pEOS = NULL;
      }
   virtual ~SelectCaseStmt();

    SelectCaseStmt* newSelectCaseStmt()
      {
         SelectCaseStmt* node = new SelectCaseStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pCaseConstructName = pCaseConstructName;  pCaseConstructName = NULL;
         node->pCaseExpr = pCaseExpr;  pCaseExpr = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Name* getCaseConstructName() {return pCaseConstructName;}
    CaseExpr* getCaseExpr() {return pCaseExpr;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setCaseConstructName(Name* caseconstructname) {pCaseConstructName = caseconstructname;}
    void setCaseExpr(CaseExpr* caseexpr) {pCaseExpr = caseexpr;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Name* pCaseConstructName;
    CaseExpr* pCaseExpr;
    EOS* pEOS;
};

class CaseStmt : public Statement
{
 public:
    CaseStmt()
      {
         pLabel = NULL;
         pCaseSelector = NULL;
         pCaseConstructName = NULL;
         pEOS = NULL;
      }
   virtual ~CaseStmt();

    CaseStmt* newCaseStmt()
      {
         CaseStmt* node = new CaseStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pCaseSelector = pCaseSelector;  pCaseSelector = NULL;
         node->pCaseConstructName = pCaseConstructName;  pCaseConstructName = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    CaseSelector* getCaseSelector() {return pCaseSelector;}
    Name* getCaseConstructName() {return pCaseConstructName;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setCaseSelector(CaseSelector* caseselector) {pCaseSelector = caseselector;}
    void setCaseConstructName(Name* caseconstructname) {pCaseConstructName = caseconstructname;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    CaseSelector* pCaseSelector;
    Name* pCaseConstructName;
    EOS* pEOS;
};

class EndSelectStmt : public Statement
{
 public:
    EndSelectStmt()
      {
         pLabel = NULL;
         pCaseConstructName = NULL;
         pEOS = NULL;
      }
   virtual ~EndSelectStmt();

    EndSelectStmt* newEndSelectStmt()
      {
         EndSelectStmt* node = new EndSelectStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pCaseConstructName = pCaseConstructName;  pCaseConstructName = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Name* getCaseConstructName() {return pCaseConstructName;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setCaseConstructName(Name* caseconstructname) {pCaseConstructName = caseconstructname;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Name* pCaseConstructName;
    EOS* pEOS;
};

class CaseExpr : public Node
{
 public:
    CaseExpr()
      {
         pExpr = NULL;
      }
   virtual ~CaseExpr();

    CaseExpr* newCaseExpr()
      {
         CaseExpr* node = new CaseExpr();
         node->pExpr = pExpr;  pExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Expr* getExpr() {return pExpr;}

    void setExpr(Expr* expr) {pExpr = expr;}

 private:
    Expr* pExpr;
};

class CaseSelector : public Node
{
 public:
    CaseSelector()
      {
         pCaseValueRangeList = NULL;
      }
   virtual ~CaseSelector();

   enum OptionType
     {
        DEFAULT = 0,
        CaseSelector_DEFAULT,
        CaseSelector_CVRL
     };

    CaseSelector* newCaseSelector()
      {
         CaseSelector* node = new CaseSelector();
         node->pCaseValueRangeList = pCaseValueRangeList;  pCaseValueRangeList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    CaseValueRangeList* getCaseValueRangeList() {return pCaseValueRangeList;}

    void setCaseValueRangeList(CaseValueRangeList* casevaluerangelist) {pCaseValueRangeList = casevaluerangelist;}

 private:
    CaseValueRangeList* pCaseValueRangeList;
};

class CaseValueRange : public Node
{
 public:
    CaseValueRange()
      {
         pCaseValue = NULL;
      }
   virtual ~CaseValueRange();

   enum OptionType
     {
        DEFAULT = 0,
        CaseValueRange_CVCV,
        CaseValueRange_CV
     };

    CaseValueRange* newCaseValueRange()
      {
         CaseValueRange* node = new CaseValueRange();
         node->pCaseValue = pCaseValue;  pCaseValue = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    CaseValue* getCaseValue() {return pCaseValue;}

    void setCaseValue(CaseValue* casevalue) {pCaseValue = casevalue;}

 private:
    CaseValue* pCaseValue;
};

class CaseValueRangeList : public Node
{
 public:
    CaseValueRangeList()
      {
         pCaseValueRangeList = new std::vector<CaseValueRange*>();
      }
   virtual ~CaseValueRangeList();

    CaseValueRangeList* newCaseValueRangeList()
      {
         CaseValueRangeList* node = new CaseValueRangeList();
         delete node->pCaseValueRangeList; node->pCaseValueRangeList = pCaseValueRangeList;  pCaseValueRangeList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<CaseValueRange*>* getCaseValueRangeList() {return pCaseValueRangeList;}

    void appendCaseValueRange(CaseValueRange* casevaluerange) {pCaseValueRangeList->push_back(casevaluerange);}

 private:
    std::vector<CaseValueRange*>* pCaseValueRangeList;
};

class CaseValue : public Node
{
 public:
    CaseValue()
      {
         pConstantExpr = NULL;
      }
   virtual ~CaseValue();

    CaseValue* newCaseValue()
      {
         CaseValue* node = new CaseValue();
         node->pConstantExpr = pConstantExpr;  pConstantExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    ConstantExpr* getConstantExpr() {return pConstantExpr;}

    void setConstantExpr(ConstantExpr* constantexpr) {pConstantExpr = constantexpr;}

 private:
    ConstantExpr* pConstantExpr;
};

class SelectTypeConstruct : public Node
{
 public:
    SelectTypeConstruct()
      {
         pSelectTypeStmt = NULL;
         pTypeGuardStmtAndBlockList = new std::vector<TypeGuardStmtAndBlock*>();
         pEndSelectTypeStmt = NULL;
      }
   virtual ~SelectTypeConstruct();

    SelectTypeConstruct* newSelectTypeConstruct()
      {
         SelectTypeConstruct* node = new SelectTypeConstruct();
         node->pSelectTypeStmt = pSelectTypeStmt;  pSelectTypeStmt = NULL;
         delete node->pTypeGuardStmtAndBlockList; node->pTypeGuardStmtAndBlockList = pTypeGuardStmtAndBlockList;  pTypeGuardStmtAndBlockList = NULL;
         node->pEndSelectTypeStmt = pEndSelectTypeStmt;  pEndSelectTypeStmt = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    SelectTypeStmt* getSelectTypeStmt() {return pSelectTypeStmt;}
    std::vector<TypeGuardStmtAndBlock*>* getTypeGuardStmtAndBlockList() {return pTypeGuardStmtAndBlockList;}
    EndSelectTypeStmt* getEndSelectTypeStmt() {return pEndSelectTypeStmt;}

    void setSelectTypeStmt(SelectTypeStmt* selecttypestmt) {pSelectTypeStmt = selecttypestmt;}
    void appendTypeGuardStmtAndBlock(TypeGuardStmtAndBlock* typeguardstmtandblock) {pTypeGuardStmtAndBlockList->push_back(typeguardstmtandblock);}
    void setEndSelectTypeStmt(EndSelectTypeStmt* endselecttypestmt) {pEndSelectTypeStmt = endselecttypestmt;}

 private:
    SelectTypeStmt* pSelectTypeStmt;
    std::vector<TypeGuardStmtAndBlock*>* pTypeGuardStmtAndBlockList;
    EndSelectTypeStmt* pEndSelectTypeStmt;
};

class TypeGuardStmtAndBlock : public Node
{
 public:
    TypeGuardStmtAndBlock()
      {
         pTypeGuardStmt = NULL;
         pBlock = NULL;
      }
   virtual ~TypeGuardStmtAndBlock();

    TypeGuardStmtAndBlock* newTypeGuardStmtAndBlock()
      {
         TypeGuardStmtAndBlock* node = new TypeGuardStmtAndBlock();
         node->pTypeGuardStmt = pTypeGuardStmt;  pTypeGuardStmt = NULL;
         node->pBlock = pBlock;  pBlock = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    TypeGuardStmt* getTypeGuardStmt() {return pTypeGuardStmt;}
    Block* getBlock() {return pBlock;}

    void setTypeGuardStmt(TypeGuardStmt* typeguardstmt) {pTypeGuardStmt = typeguardstmt;}
    void setBlock(Block* block) {pBlock = block;}

 private:
    TypeGuardStmt* pTypeGuardStmt;
    Block* pBlock;
};

class SelectTypeStmt : public Node
{
 public:
    SelectTypeStmt()
      {
         pSelectConstructName = NULL;
         pAssociateName = NULL;
         pSelector = NULL;
         pEOS = NULL;
      }
   virtual ~SelectTypeStmt();

    SelectTypeStmt* newSelectTypeStmt()
      {
         SelectTypeStmt* node = new SelectTypeStmt();
         node->pSelectConstructName = pSelectConstructName;  pSelectConstructName = NULL;
         node->pAssociateName = pAssociateName;  pAssociateName = NULL;
         node->pSelector = pSelector;  pSelector = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Name* getSelectConstructName() {return pSelectConstructName;}
    Name* getAssociateName() {return pAssociateName;}
    Selector* getSelector() {return pSelector;}
    EOS* getEOS() {return pEOS;}

    void setSelectConstructName(Name* selectconstructname) {pSelectConstructName = selectconstructname;}
    void setAssociateName(Name* associatename) {pAssociateName = associatename;}
    void setSelector(Selector* selector) {pSelector = selector;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Name* pSelectConstructName;
    Name* pAssociateName;
    Selector* pSelector;
    EOS* pEOS;
};

class TypeGuardStmt : public Node
{
 public:
    TypeGuardStmt()
      {
         pSelectConstructName = NULL;
         pEOS = NULL;
         pDerivedTypeSpec = NULL;
         pTypeSpec = NULL;
      }
   virtual ~TypeGuardStmt();

   enum OptionType
     {
        DEFAULT = 0,
        TypeGuardStmt_CLASS_DEF,
        TypeGuardStmt_CLASS,
        TypeGuardStmt_TYPE
     };

    TypeGuardStmt* newTypeGuardStmt()
      {
         TypeGuardStmt* node = new TypeGuardStmt();
         node->pSelectConstructName = pSelectConstructName;  pSelectConstructName = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->pDerivedTypeSpec = pDerivedTypeSpec;  pDerivedTypeSpec = NULL;
         node->pTypeSpec = pTypeSpec;  pTypeSpec = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Name* getSelectConstructName() {return pSelectConstructName;}
    EOS* getEOS() {return pEOS;}
    DerivedTypeSpec* getDerivedTypeSpec() {return pDerivedTypeSpec;}
    TypeSpec* getTypeSpec() {return pTypeSpec;}

    void setSelectConstructName(Name* selectconstructname) {pSelectConstructName = selectconstructname;}
    void setEOS(EOS* eos) {pEOS = eos;}
    void setDerivedTypeSpec(DerivedTypeSpec* derivedtypespec) {pDerivedTypeSpec = derivedtypespec;}
    void setTypeSpec(TypeSpec* typespec) {pTypeSpec = typespec;}

 private:
    Name* pSelectConstructName;
    EOS* pEOS;
    DerivedTypeSpec* pDerivedTypeSpec;
    TypeSpec* pTypeSpec;
};

class EndSelectTypeStmt : public Node
{
 public:
    EndSelectTypeStmt()
      {
         pSelectConstructName = NULL;
         pEOS = NULL;
      }
   virtual ~EndSelectTypeStmt();

    EndSelectTypeStmt* newEndSelectTypeStmt()
      {
         EndSelectTypeStmt* node = new EndSelectTypeStmt();
         node->pSelectConstructName = pSelectConstructName;  pSelectConstructName = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Name* getSelectConstructName() {return pSelectConstructName;}
    EOS* getEOS() {return pEOS;}

    void setSelectConstructName(Name* selectconstructname) {pSelectConstructName = selectconstructname;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Name* pSelectConstructName;
    EOS* pEOS;
};

class ExitStmt : public Statement
{
 public:
    ExitStmt()
      {
         pLabel = NULL;
         pConstructName = NULL;
         pEOS = NULL;
      }
   virtual ~ExitStmt();

    ExitStmt* newExitStmt()
      {
         ExitStmt* node = new ExitStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pConstructName = pConstructName;  pConstructName = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Name* getConstructName() {return pConstructName;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setConstructName(Name* constructname) {pConstructName = constructname;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Name* pConstructName;
    EOS* pEOS;
};

class GotoStmt : public Statement
{
 public:
    GotoStmt()
      {
         pLabel = NULL;
         pLblRef = NULL;
         pEOS = NULL;
      }
   virtual ~GotoStmt();

    GotoStmt* newGotoStmt()
      {
         GotoStmt* node = new GotoStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pLblRef = pLblRef;  pLblRef = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    LblRef* getLblRef() {return pLblRef;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setLblRef(LblRef* lblref) {pLblRef = lblref;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    LblRef* pLblRef;
    EOS* pEOS;
};

class ComputedGotoStmt : public Statement
{
 public:
    ComputedGotoStmt()
      {
         pLabel = NULL;
         pLabelList = NULL;
         pExpr = NULL;
         pEOS = NULL;
      }
   virtual ~ComputedGotoStmt();

    ComputedGotoStmt* newComputedGotoStmt()
      {
         ComputedGotoStmt* node = new ComputedGotoStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pLabelList = pLabelList;  pLabelList = NULL;
         node->pExpr = pExpr;  pExpr = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    LabelList* getLabelList() {return pLabelList;}
    Expr* getExpr() {return pExpr;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setLabelList(LabelList* labellist) {pLabelList = labellist;}
    void setExpr(Expr* expr) {pExpr = expr;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    LabelList* pLabelList;
    Expr* pExpr;
    EOS* pEOS;
};

class ArithmeticIfStmt : public Statement
{
 public:
    ArithmeticIfStmt()
      {
         pLabel = NULL;
         pExpr = NULL;
         pLblRef = NULL;
         pEOS = NULL;
      }
   virtual ~ArithmeticIfStmt();

    ArithmeticIfStmt* newArithmeticIfStmt()
      {
         ArithmeticIfStmt* node = new ArithmeticIfStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pExpr = pExpr;  pExpr = NULL;
         node->pLblRef = pLblRef;  pLblRef = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Expr* getExpr() {return pExpr;}
    LblRef* getLblRef() {return pLblRef;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setExpr(Expr* expr) {pExpr = expr;}
    void setLblRef(LblRef* lblref) {pLblRef = lblref;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Expr* pExpr;
    LblRef* pLblRef;
    EOS* pEOS;
};

class ContinueStmt : public Statement
{
 public:
    ContinueStmt()
      {
         pLabel = NULL;
         pEOS = NULL;
      }
   virtual ~ContinueStmt();

    ContinueStmt* newContinueStmt()
      {
         ContinueStmt* node = new ContinueStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    EOS* pEOS;
};

class StopStmt : public Statement
{
 public:
    StopStmt()
      {
         pLabel = NULL;
         pStopCode = NULL;
         pEOS = NULL;
      }
   virtual ~StopStmt();

    StopStmt* newStopStmt()
      {
         StopStmt* node = new StopStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pStopCode = pStopCode;  pStopCode = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    StopCode* getStopCode() {return pStopCode;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setStopCode(StopCode* stopcode) {pStopCode = stopcode;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    StopCode* pStopCode;
    EOS* pEOS;
};

class ErrorStopStmt : public Statement
{
 public:
    ErrorStopStmt()
      {
         pLabel = NULL;
         pStopCode = NULL;
         pEOS = NULL;
      }
   virtual ~ErrorStopStmt();

    ErrorStopStmt* newErrorStopStmt()
      {
         ErrorStopStmt* node = new ErrorStopStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pStopCode = pStopCode;  pStopCode = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    StopCode* getStopCode() {return pStopCode;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setStopCode(StopCode* stopcode) {pStopCode = stopcode;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    StopCode* pStopCode;
    EOS* pEOS;
};

class StopCode : public Node
{
 public:
    StopCode()
      {
         pIcon = NULL;
         pScon = NULL;
      }
   virtual ~StopCode();

   enum OptionType
     {
        DEFAULT = 0,
        StopCode_SI,
        StopCode_SD
     };

    StopCode* newStopCode()
      {
         StopCode* node = new StopCode();
         node->pIcon = pIcon;  pIcon = NULL;
         node->pScon = pScon;  pScon = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Icon* getIcon() {return pIcon;}
    Scon* getScon() {return pScon;}

    void setIcon(Icon* icon) {pIcon = icon;}
    void setScon(Scon* scon) {pScon = scon;}

 private:
    Icon* pIcon;
    Scon* pScon;
};

class SyncAllStmt : public Statement
{
 public:
    SyncAllStmt()
      {
         pLabel = NULL;
         pEOS = NULL;
         pSyncStatList = NULL;
      }
   virtual ~SyncAllStmt();

   enum OptionType
     {
        DEFAULT = 0,
        SyncAllStmt_0,
        SyncAllStmt_SSL
     };

    SyncAllStmt* newSyncAllStmt()
      {
         SyncAllStmt* node = new SyncAllStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->pSyncStatList = pSyncStatList;  pSyncStatList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    EOS* getEOS() {return pEOS;}
    SyncStatList* getSyncStatList() {return pSyncStatList;}

    void setLabel(Label* label) {pLabel = label;}
    void setEOS(EOS* eos) {pEOS = eos;}
    void setSyncStatList(SyncStatList* syncstatlist) {pSyncStatList = syncstatlist;}

 private:
    Label* pLabel;
    EOS* pEOS;
    SyncStatList* pSyncStatList;
};

class SyncStat : public Node
{
 public:
    SyncStat()
      {
         pErrmsgVariable = NULL;
         pStatVariable = NULL;
      }
   virtual ~SyncStat();

   enum OptionType
     {
        DEFAULT = 0,
        SyncStat_ERRMSG,
        SyncStat_STAT
     };

    SyncStat* newSyncStat()
      {
         SyncStat* node = new SyncStat();
         node->pErrmsgVariable = pErrmsgVariable;  pErrmsgVariable = NULL;
         node->pStatVariable = pStatVariable;  pStatVariable = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    ErrmsgVariable* getErrmsgVariable() {return pErrmsgVariable;}
    StatVariable* getStatVariable() {return pStatVariable;}

    void setErrmsgVariable(ErrmsgVariable* errmsgvariable) {pErrmsgVariable = errmsgvariable;}
    void setStatVariable(StatVariable* statvariable) {pStatVariable = statvariable;}

 private:
    ErrmsgVariable* pErrmsgVariable;
    StatVariable* pStatVariable;
};

class SyncStatList : public Node
{
 public:
    SyncStatList()
      {
         pSyncStatList = new std::vector<SyncStat*>();
      }
   virtual ~SyncStatList();

    SyncStatList* newSyncStatList()
      {
         SyncStatList* node = new SyncStatList();
         delete node->pSyncStatList; node->pSyncStatList = pSyncStatList;  pSyncStatList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<SyncStat*>* getSyncStatList() {return pSyncStatList;}

    void appendSyncStat(SyncStat* syncstat) {pSyncStatList->push_back(syncstat);}

 private:
    std::vector<SyncStat*>* pSyncStatList;
};

class SyncImagesStmt : public Statement
{
 public:
    SyncImagesStmt()
      {
         pLabel = NULL;
         pImageSet = NULL;
         pSyncStatList = NULL;
         pEOS = NULL;
      }
   virtual ~SyncImagesStmt();

    SyncImagesStmt* newSyncImagesStmt()
      {
         SyncImagesStmt* node = new SyncImagesStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pImageSet = pImageSet;  pImageSet = NULL;
         node->pSyncStatList = pSyncStatList;  pSyncStatList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    ImageSet* getImageSet() {return pImageSet;}
    SyncStatList* getSyncStatList() {return pSyncStatList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setImageSet(ImageSet* imageset) {pImageSet = imageset;}
    void setSyncStatList(SyncStatList* syncstatlist) {pSyncStatList = syncstatlist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    ImageSet* pImageSet;
    SyncStatList* pSyncStatList;
    EOS* pEOS;
};

class ImageSet : public Node
{
 public:
    ImageSet()
      {
         pIntExpr = NULL;
      }
   virtual ~ImageSet();

   enum OptionType
     {
        DEFAULT = 0,
        ImageSet_STAR,
        ImageSet_IE
     };

    ImageSet* newImageSet()
      {
         ImageSet* node = new ImageSet();
         node->pIntExpr = pIntExpr;  pIntExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    IntExpr* getIntExpr() {return pIntExpr;}

    void setIntExpr(IntExpr* intexpr) {pIntExpr = intexpr;}

 private:
    IntExpr* pIntExpr;
};

class SyncMemoryStmt : public Statement
{
 public:
    SyncMemoryStmt()
      {
         pLabel = NULL;
         pEOS = NULL;
         pSyncStatList = NULL;
      }
   virtual ~SyncMemoryStmt();

   enum OptionType
     {
        DEFAULT = 0,
        SyncMemoryStmt_0,
        SyncMemoryStmt_SSL
     };

    SyncMemoryStmt* newSyncMemoryStmt()
      {
         SyncMemoryStmt* node = new SyncMemoryStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->pSyncStatList = pSyncStatList;  pSyncStatList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    EOS* getEOS() {return pEOS;}
    SyncStatList* getSyncStatList() {return pSyncStatList;}

    void setLabel(Label* label) {pLabel = label;}
    void setEOS(EOS* eos) {pEOS = eos;}
    void setSyncStatList(SyncStatList* syncstatlist) {pSyncStatList = syncstatlist;}

 private:
    Label* pLabel;
    EOS* pEOS;
    SyncStatList* pSyncStatList;
};

class LockStmt : public Statement
{
 public:
    LockStmt()
      {
         pLabel = NULL;
         pLockVariable = NULL;
         pLockStatList = NULL;
         pEOS = NULL;
      }
   virtual ~LockStmt();

    LockStmt* newLockStmt()
      {
         LockStmt* node = new LockStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pLockVariable = pLockVariable;  pLockVariable = NULL;
         node->pLockStatList = pLockStatList;  pLockStatList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    LockVariable* getLockVariable() {return pLockVariable;}
    LockStatList* getLockStatList() {return pLockStatList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setLockVariable(LockVariable* lockvariable) {pLockVariable = lockvariable;}
    void setLockStatList(LockStatList* lockstatlist) {pLockStatList = lockstatlist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    LockVariable* pLockVariable;
    LockStatList* pLockStatList;
    EOS* pEOS;
};

class LockStat : public Node
{
 public:
    LockStat()
      {
         pSyncStat = NULL;
         pLogicalVariable = NULL;
      }
   virtual ~LockStat();

   enum OptionType
     {
        DEFAULT = 0,
        LockStat_SS,
        LockStat_LV
     };

    LockStat* newLockStat()
      {
         LockStat* node = new LockStat();
         node->pSyncStat = pSyncStat;  pSyncStat = NULL;
         node->pLogicalVariable = pLogicalVariable;  pLogicalVariable = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    SyncStat* getSyncStat() {return pSyncStat;}
    LogicalVariable* getLogicalVariable() {return pLogicalVariable;}

    void setSyncStat(SyncStat* syncstat) {pSyncStat = syncstat;}
    void setLogicalVariable(LogicalVariable* logicalvariable) {pLogicalVariable = logicalvariable;}

 private:
    SyncStat* pSyncStat;
    LogicalVariable* pLogicalVariable;
};

class LockStatList : public Node
{
 public:
    LockStatList()
      {
         pLockStatList = new std::vector<LockStat*>();
      }
   virtual ~LockStatList();

    LockStatList* newLockStatList()
      {
         LockStatList* node = new LockStatList();
         delete node->pLockStatList; node->pLockStatList = pLockStatList;  pLockStatList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<LockStat*>* getLockStatList() {return pLockStatList;}

    void appendLockStat(LockStat* lockstat) {pLockStatList->push_back(lockstat);}

 private:
    std::vector<LockStat*>* pLockStatList;
};

class UnlockStmt : public Statement
{
 public:
    UnlockStmt()
      {
         pLabel = NULL;
         pLockVariable = NULL;
         pSyncStatList = NULL;
         pEOS = NULL;
      }
   virtual ~UnlockStmt();

    UnlockStmt* newUnlockStmt()
      {
         UnlockStmt* node = new UnlockStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pLockVariable = pLockVariable;  pLockVariable = NULL;
         node->pSyncStatList = pSyncStatList;  pSyncStatList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    LockVariable* getLockVariable() {return pLockVariable;}
    SyncStatList* getSyncStatList() {return pSyncStatList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setLockVariable(LockVariable* lockvariable) {pLockVariable = lockvariable;}
    void setSyncStatList(SyncStatList* syncstatlist) {pSyncStatList = syncstatlist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    LockVariable* pLockVariable;
    SyncStatList* pSyncStatList;
    EOS* pEOS;
};

class LockVariable : public Node
{
 public:
    LockVariable()
      {
         pVariable = NULL;
      }
   virtual ~LockVariable();

    LockVariable* newLockVariable()
      {
         LockVariable* node = new LockVariable();
         node->pVariable = pVariable;  pVariable = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Variable* getVariable() {return pVariable;}

    void setVariable(Variable* variable) {pVariable = variable;}

 private:
    Variable* pVariable;
};

class PauseStmt : public Statement
{
 public:
    PauseStmt()
      {
         pLabel = NULL;
         pStopCode = NULL;
         pEOS = NULL;
      }
   virtual ~PauseStmt();

    PauseStmt* newPauseStmt()
      {
         PauseStmt* node = new PauseStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pStopCode = pStopCode;  pStopCode = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    StopCode* getStopCode() {return pStopCode;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setStopCode(StopCode* stopcode) {pStopCode = stopcode;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    StopCode* pStopCode;
    EOS* pEOS;
};

class IoUnit : public Node
{
 public:
    IoUnit()
      {
         pCharVariable = NULL;
         pIntExpr = NULL;
      }
   virtual ~IoUnit();

   enum OptionType
     {
        DEFAULT = 0,
        IoUnit_IFV,
        IoUnit_STAR,
        IoUnit_FUN
     };

    IoUnit* newIoUnit()
      {
         IoUnit* node = new IoUnit();
         node->pCharVariable = pCharVariable;  pCharVariable = NULL;
         node->pIntExpr = pIntExpr;  pIntExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    CharVariable* getCharVariable() {return pCharVariable;}
    IntExpr* getIntExpr() {return pIntExpr;}

    void setCharVariable(CharVariable* charvariable) {pCharVariable = charvariable;}
    void setIntExpr(IntExpr* intexpr) {pIntExpr = intexpr;}

 private:
    CharVariable* pCharVariable;
    IntExpr* pIntExpr;
};

class FileUnitNumber : public Node
{
 public:
    FileUnitNumber()
      {
         pIntExpr = NULL;
      }
   virtual ~FileUnitNumber();

    FileUnitNumber* newFileUnitNumber()
      {
         FileUnitNumber* node = new FileUnitNumber();
         node->pIntExpr = pIntExpr;  pIntExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    IntExpr* getIntExpr() {return pIntExpr;}

    void setIntExpr(IntExpr* intexpr) {pIntExpr = intexpr;}

 private:
    IntExpr* pIntExpr;
};

class OpenStmt : public Statement
{
 public:
    OpenStmt()
      {
         pLabel = NULL;
         pConnectSpecList = NULL;
         pEOS = NULL;
      }
   virtual ~OpenStmt();

    OpenStmt* newOpenStmt()
      {
         OpenStmt* node = new OpenStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pConnectSpecList = pConnectSpecList;  pConnectSpecList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    ConnectSpecList* getConnectSpecList() {return pConnectSpecList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setConnectSpecList(ConnectSpecList* connectspeclist) {pConnectSpecList = connectspeclist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    ConnectSpecList* pConnectSpecList;
    EOS* pEOS;
};

class ConnectSpec : public Node
{
 public:
    ConnectSpec()
      {
         pDefaultCharExpr = NULL;
         pIntExpr = NULL;
         pIntVariable = NULL;
         pIomsgVariable = NULL;
         pFileNameExpr = NULL;
         pLblRef = NULL;
         pFileUnitNumber = NULL;
      }
   virtual ~ConnectSpec();

   enum OptionType
     {
        DEFAULT = 0,
        ConnectSpec_STATUS,
        ConnectSpec_SIGN,
        ConnectSpec_ROUND,
        ConnectSpec_RECL,
        ConnectSpec_POSITION,
        ConnectSpec_PAD,
        ConnectSpec_NEWUNIT,
        ConnectSpec_IOSTAT,
        ConnectSpec_IOMSG,
        ConnectSpec_FORM,
        ConnectSpec_FILE,
        ConnectSpec_ERR,
        ConnectSpec_ENCODING,
        ConnectSpec_DELIM,
        ConnectSpec_DECIMAL,
        ConnectSpec_BLANK,
        ConnectSpec_ASYNC,
        ConnectSpec_ACTION,
        ConnectSpec_ACCESS,
        ConnectSpec_UNIT
     };

    ConnectSpec* newConnectSpec()
      {
         ConnectSpec* node = new ConnectSpec();
         node->pDefaultCharExpr = pDefaultCharExpr;  pDefaultCharExpr = NULL;
         node->pIntExpr = pIntExpr;  pIntExpr = NULL;
         node->pIntVariable = pIntVariable;  pIntVariable = NULL;
         node->pIomsgVariable = pIomsgVariable;  pIomsgVariable = NULL;
         node->pFileNameExpr = pFileNameExpr;  pFileNameExpr = NULL;
         node->pLblRef = pLblRef;  pLblRef = NULL;
         node->pFileUnitNumber = pFileUnitNumber;  pFileUnitNumber = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    DefaultCharExpr* getDefaultCharExpr() {return pDefaultCharExpr;}
    IntExpr* getIntExpr() {return pIntExpr;}
    IntVariable* getIntVariable() {return pIntVariable;}
    IomsgVariable* getIomsgVariable() {return pIomsgVariable;}
    FileNameExpr* getFileNameExpr() {return pFileNameExpr;}
    LblRef* getLblRef() {return pLblRef;}
    FileUnitNumber* getFileUnitNumber() {return pFileUnitNumber;}

    void setDefaultCharExpr(DefaultCharExpr* defaultcharexpr) {pDefaultCharExpr = defaultcharexpr;}
    void setIntExpr(IntExpr* intexpr) {pIntExpr = intexpr;}
    void setIntVariable(IntVariable* intvariable) {pIntVariable = intvariable;}
    void setIomsgVariable(IomsgVariable* iomsgvariable) {pIomsgVariable = iomsgvariable;}
    void setFileNameExpr(FileNameExpr* filenameexpr) {pFileNameExpr = filenameexpr;}
    void setLblRef(LblRef* lblref) {pLblRef = lblref;}
    void setFileUnitNumber(FileUnitNumber* fileunitnumber) {pFileUnitNumber = fileunitnumber;}

 private:
    DefaultCharExpr* pDefaultCharExpr;
    IntExpr* pIntExpr;
    IntVariable* pIntVariable;
    IomsgVariable* pIomsgVariable;
    FileNameExpr* pFileNameExpr;
    LblRef* pLblRef;
    FileUnitNumber* pFileUnitNumber;
};

class ConnectSpecList : public Node
{
 public:
    ConnectSpecList()
      {
         pConnectSpecList = new std::vector<ConnectSpec*>();
      }
   virtual ~ConnectSpecList();

    ConnectSpecList* newConnectSpecList()
      {
         ConnectSpecList* node = new ConnectSpecList();
         delete node->pConnectSpecList; node->pConnectSpecList = pConnectSpecList;  pConnectSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<ConnectSpec*>* getConnectSpecList() {return pConnectSpecList;}

    void appendConnectSpec(ConnectSpec* connectspec) {pConnectSpecList->push_back(connectspec);}

 private:
    std::vector<ConnectSpec*>* pConnectSpecList;
};

class FileNameExpr : public Node
{
 public:
    FileNameExpr()
      {
         pDefaultCharExpr = NULL;
      }
   virtual ~FileNameExpr();

    FileNameExpr* newFileNameExpr()
      {
         FileNameExpr* node = new FileNameExpr();
         node->pDefaultCharExpr = pDefaultCharExpr;  pDefaultCharExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    DefaultCharExpr* getDefaultCharExpr() {return pDefaultCharExpr;}

    void setDefaultCharExpr(DefaultCharExpr* defaultcharexpr) {pDefaultCharExpr = defaultcharexpr;}

 private:
    DefaultCharExpr* pDefaultCharExpr;
};

class IomsgVariable : public Node
{
 public:
    IomsgVariable()
      {
         pDefaultCharExpr = NULL;
      }
   virtual ~IomsgVariable();

    IomsgVariable* newIomsgVariable()
      {
         IomsgVariable* node = new IomsgVariable();
         node->pDefaultCharExpr = pDefaultCharExpr;  pDefaultCharExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    DefaultCharExpr* getDefaultCharExpr() {return pDefaultCharExpr;}

    void setDefaultCharExpr(DefaultCharExpr* defaultcharexpr) {pDefaultCharExpr = defaultcharexpr;}

 private:
    DefaultCharExpr* pDefaultCharExpr;
};

class CloseStmt : public Statement
{
 public:
    CloseStmt()
      {
         pLabel = NULL;
         pCloseSpecList = NULL;
         pEOS = NULL;
      }
   virtual ~CloseStmt();

    CloseStmt* newCloseStmt()
      {
         CloseStmt* node = new CloseStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pCloseSpecList = pCloseSpecList;  pCloseSpecList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    CloseSpecList* getCloseSpecList() {return pCloseSpecList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setCloseSpecList(CloseSpecList* closespeclist) {pCloseSpecList = closespeclist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    CloseSpecList* pCloseSpecList;
    EOS* pEOS;
};

class CloseSpec : public Node
{
 public:
    CloseSpec()
      {
         pDefaultCharExpr = NULL;
         pLblRef = NULL;
         pIomsgVariable = NULL;
         pIntVariable = NULL;
         pFileUnitNumber = NULL;
      }
   virtual ~CloseSpec();

   enum OptionType
     {
        DEFAULT = 0,
        CloseSpec_STATUS,
        CloseSpec_ERR,
        CloseSpec_IOMSG,
        CloseSpec_IOSTAT,
        CloseSpec_UNIT
     };

    CloseSpec* newCloseSpec()
      {
         CloseSpec* node = new CloseSpec();
         node->pDefaultCharExpr = pDefaultCharExpr;  pDefaultCharExpr = NULL;
         node->pLblRef = pLblRef;  pLblRef = NULL;
         node->pIomsgVariable = pIomsgVariable;  pIomsgVariable = NULL;
         node->pIntVariable = pIntVariable;  pIntVariable = NULL;
         node->pFileUnitNumber = pFileUnitNumber;  pFileUnitNumber = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    DefaultCharExpr* getDefaultCharExpr() {return pDefaultCharExpr;}
    LblRef* getLblRef() {return pLblRef;}
    IomsgVariable* getIomsgVariable() {return pIomsgVariable;}
    IntVariable* getIntVariable() {return pIntVariable;}
    FileUnitNumber* getFileUnitNumber() {return pFileUnitNumber;}

    void setDefaultCharExpr(DefaultCharExpr* defaultcharexpr) {pDefaultCharExpr = defaultcharexpr;}
    void setLblRef(LblRef* lblref) {pLblRef = lblref;}
    void setIomsgVariable(IomsgVariable* iomsgvariable) {pIomsgVariable = iomsgvariable;}
    void setIntVariable(IntVariable* intvariable) {pIntVariable = intvariable;}
    void setFileUnitNumber(FileUnitNumber* fileunitnumber) {pFileUnitNumber = fileunitnumber;}

 private:
    DefaultCharExpr* pDefaultCharExpr;
    LblRef* pLblRef;
    IomsgVariable* pIomsgVariable;
    IntVariable* pIntVariable;
    FileUnitNumber* pFileUnitNumber;
};

class CloseSpecList : public Node
{
 public:
    CloseSpecList()
      {
         pCloseSpecList = new std::vector<CloseSpec*>();
      }
   virtual ~CloseSpecList();

    CloseSpecList* newCloseSpecList()
      {
         CloseSpecList* node = new CloseSpecList();
         delete node->pCloseSpecList; node->pCloseSpecList = pCloseSpecList;  pCloseSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<CloseSpec*>* getCloseSpecList() {return pCloseSpecList;}

    void appendCloseSpec(CloseSpec* closespec) {pCloseSpecList->push_back(closespec);}

 private:
    std::vector<CloseSpec*>* pCloseSpecList;
};

class ReadStmt : public Statement
{
 public:
    ReadStmt()
      {
         pLabel = NULL;
         pFormat = NULL;
         pInputItemList = NULL;
         pEOS = NULL;
         pIoControlSpecList = NULL;
      }
   virtual ~ReadStmt();

   enum OptionType
     {
        DEFAULT = 0,
        ReadStmt_F,
        ReadStmt_ICSL
     };

    ReadStmt* newReadStmt()
      {
         ReadStmt* node = new ReadStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pFormat = pFormat;  pFormat = NULL;
         node->pInputItemList = pInputItemList;  pInputItemList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->pIoControlSpecList = pIoControlSpecList;  pIoControlSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Format* getFormat() {return pFormat;}
    InputItemList* getInputItemList() {return pInputItemList;}
    EOS* getEOS() {return pEOS;}
    IoControlSpecList* getIoControlSpecList() {return pIoControlSpecList;}

    void setLabel(Label* label) {pLabel = label;}
    void setFormat(Format* format) {pFormat = format;}
    void setInputItemList(InputItemList* inputitemlist) {pInputItemList = inputitemlist;}
    void setEOS(EOS* eos) {pEOS = eos;}
    void setIoControlSpecList(IoControlSpecList* iocontrolspeclist) {pIoControlSpecList = iocontrolspeclist;}

 private:
    Label* pLabel;
    Format* pFormat;
    InputItemList* pInputItemList;
    EOS* pEOS;
    IoControlSpecList* pIoControlSpecList;
};

class WriteStmt : public Statement
{
 public:
    WriteStmt()
      {
         pLabel = NULL;
         pIoControlSpecList = NULL;
         pOutputItemList = NULL;
         pEOS = NULL;
      }
   virtual ~WriteStmt();

    WriteStmt* newWriteStmt()
      {
         WriteStmt* node = new WriteStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pIoControlSpecList = pIoControlSpecList;  pIoControlSpecList = NULL;
         node->pOutputItemList = pOutputItemList;  pOutputItemList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    IoControlSpecList* getIoControlSpecList() {return pIoControlSpecList;}
    OutputItemList* getOutputItemList() {return pOutputItemList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setIoControlSpecList(IoControlSpecList* iocontrolspeclist) {pIoControlSpecList = iocontrolspeclist;}
    void setOutputItemList(OutputItemList* outputitemlist) {pOutputItemList = outputitemlist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    IoControlSpecList* pIoControlSpecList;
    OutputItemList* pOutputItemList;
    EOS* pEOS;
};

class PrintStmt : public Statement
{
 public:
    PrintStmt()
      {
         pLabel = NULL;
         pFormat = NULL;
         pOutputItemList = NULL;
         pEOS = NULL;
      }
   virtual ~PrintStmt();

    PrintStmt* newPrintStmt()
      {
         PrintStmt* node = new PrintStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pFormat = pFormat;  pFormat = NULL;
         node->pOutputItemList = pOutputItemList;  pOutputItemList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Format* getFormat() {return pFormat;}
    OutputItemList* getOutputItemList() {return pOutputItemList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setFormat(Format* format) {pFormat = format;}
    void setOutputItemList(OutputItemList* outputitemlist) {pOutputItemList = outputitemlist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Format* pFormat;
    OutputItemList* pOutputItemList;
    EOS* pEOS;
};

class IoControlSpec : public Node
{
 public:
    IoControlSpec()
      {
         pIntVariable = NULL;
         pDefaultCharExpr = NULL;
         pExpr = NULL;
         pIomsgVariable = NULL;
         pIdVariable = NULL;
         pLblRef = NULL;
         pNamelistGroupName = NULL;
         pFormat = NULL;
         pIoUnit = NULL;
      }
   virtual ~IoControlSpec();

   enum OptionType
     {
        DEFAULT = 0,
        IoControlSpec_SIZE,
        IoControlSpec_SIGN,
        IoControlSpec_ROUND,
        IoControlSpec_REC,
        IoControlSpec_POS,
        IoControlSpec_PAD,
        IoControlSpec_IOSTAT,
        IoControlSpec_IOMSG,
        IoControlSpec_ID,
        IoControlSpec_ERR,
        IoControlSpec_EOR,
        IoControlSpec_END,
        IoControlSpec_DELIM,
        IoControlSpec_DECIMAL,
        IoControlSpec_BLANK,
        IoControlSpec_ASYNC,
        IoControlSpec_ADVANCE,
        IoControlSpec_NML,
        IoControlSpec_FMT,
        IoControlSpec_UNIT
     };

    IoControlSpec* newIoControlSpec()
      {
         IoControlSpec* node = new IoControlSpec();
         node->pIntVariable = pIntVariable;  pIntVariable = NULL;
         node->pDefaultCharExpr = pDefaultCharExpr;  pDefaultCharExpr = NULL;
         node->pExpr = pExpr;  pExpr = NULL;
         node->pIomsgVariable = pIomsgVariable;  pIomsgVariable = NULL;
         node->pIdVariable = pIdVariable;  pIdVariable = NULL;
         node->pLblRef = pLblRef;  pLblRef = NULL;
         node->pNamelistGroupName = pNamelistGroupName;  pNamelistGroupName = NULL;
         node->pFormat = pFormat;  pFormat = NULL;
         node->pIoUnit = pIoUnit;  pIoUnit = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    IntVariable* getIntVariable() {return pIntVariable;}
    DefaultCharExpr* getDefaultCharExpr() {return pDefaultCharExpr;}
    Expr* getExpr() {return pExpr;}
    IomsgVariable* getIomsgVariable() {return pIomsgVariable;}
    IdVariable* getIdVariable() {return pIdVariable;}
    LblRef* getLblRef() {return pLblRef;}
    Name* getNamelistGroupName() {return pNamelistGroupName;}
    Format* getFormat() {return pFormat;}
    IoUnit* getIoUnit() {return pIoUnit;}

    void setIntVariable(IntVariable* intvariable) {pIntVariable = intvariable;}
    void setDefaultCharExpr(DefaultCharExpr* defaultcharexpr) {pDefaultCharExpr = defaultcharexpr;}
    void setExpr(Expr* expr) {pExpr = expr;}
    void setIomsgVariable(IomsgVariable* iomsgvariable) {pIomsgVariable = iomsgvariable;}
    void setIdVariable(IdVariable* idvariable) {pIdVariable = idvariable;}
    void setLblRef(LblRef* lblref) {pLblRef = lblref;}
    void setNamelistGroupName(Name* namelistgroupname) {pNamelistGroupName = namelistgroupname;}
    void setFormat(Format* format) {pFormat = format;}
    void setIoUnit(IoUnit* iounit) {pIoUnit = iounit;}

 private:
    IntVariable* pIntVariable;
    DefaultCharExpr* pDefaultCharExpr;
    Expr* pExpr;
    IomsgVariable* pIomsgVariable;
    IdVariable* pIdVariable;
    LblRef* pLblRef;
    Name* pNamelistGroupName;
    Format* pFormat;
    IoUnit* pIoUnit;
};

class IoControlSpecList : public Node
{
 public:
    IoControlSpecList()
      {
         pIoControlSpecList = new std::vector<IoControlSpec*>();
      }
   virtual ~IoControlSpecList();

    IoControlSpecList* newIoControlSpecList()
      {
         IoControlSpecList* node = new IoControlSpecList();
         delete node->pIoControlSpecList; node->pIoControlSpecList = pIoControlSpecList;  pIoControlSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<IoControlSpec*>* getIoControlSpecList() {return pIoControlSpecList;}

    void appendIoControlSpec(IoControlSpec* iocontrolspec) {pIoControlSpecList->push_back(iocontrolspec);}

 private:
    std::vector<IoControlSpec*>* pIoControlSpecList;
};

class IdVariable : public Node
{
 public:
    IdVariable()
      {
         pIntVariable = NULL;
      }
   virtual ~IdVariable();

    IdVariable* newIdVariable()
      {
         IdVariable* node = new IdVariable();
         node->pIntVariable = pIntVariable;  pIntVariable = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    IntVariable* getIntVariable() {return pIntVariable;}

    void setIntVariable(IntVariable* intvariable) {pIntVariable = intvariable;}

 private:
    IntVariable* pIntVariable;
};

class Format : public Node
{
 public:
    Format()
      {
         pLabel = NULL;
         pDefaultCharExpr = NULL;
      }
   virtual ~Format();

   enum OptionType
     {
        DEFAULT = 0,
        Format_STAR,
        Format_L,
        Format_DCE
     };

    Format* newFormat()
      {
         Format* node = new Format();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pDefaultCharExpr = pDefaultCharExpr;  pDefaultCharExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    DefaultCharExpr* getDefaultCharExpr() {return pDefaultCharExpr;}

    void setLabel(Label* label) {pLabel = label;}
    void setDefaultCharExpr(DefaultCharExpr* defaultcharexpr) {pDefaultCharExpr = defaultcharexpr;}

 private:
    Label* pLabel;
    DefaultCharExpr* pDefaultCharExpr;
};

class InputItem : public Node
{
 public:
    InputItem()
      {
         pIoImpliedDo = NULL;
         pVariable = NULL;
      }
   virtual ~InputItem();

   enum OptionType
     {
        DEFAULT = 0,
        InputItem_IID,
        InputItem_V
     };

    InputItem* newInputItem()
      {
         InputItem* node = new InputItem();
         node->pIoImpliedDo = pIoImpliedDo;  pIoImpliedDo = NULL;
         node->pVariable = pVariable;  pVariable = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    IoImpliedDo* getIoImpliedDo() {return pIoImpliedDo;}
    Variable* getVariable() {return pVariable;}

    void setIoImpliedDo(IoImpliedDo* ioimplieddo) {pIoImpliedDo = ioimplieddo;}
    void setVariable(Variable* variable) {pVariable = variable;}

 private:
    IoImpliedDo* pIoImpliedDo;
    Variable* pVariable;
};

class InputItemList : public Node
{
 public:
    InputItemList()
      {
         pInputItemList = new std::vector<InputItem*>();
      }
   virtual ~InputItemList();

    InputItemList* newInputItemList()
      {
         InputItemList* node = new InputItemList();
         delete node->pInputItemList; node->pInputItemList = pInputItemList;  pInputItemList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<InputItem*>* getInputItemList() {return pInputItemList;}

    void appendInputItem(InputItem* inputitem) {pInputItemList->push_back(inputitem);}

 private:
    std::vector<InputItem*>* pInputItemList;
};

class OutputItem : public Node
{
 public:
    OutputItem()
      {
         pIoImpliedDo = NULL;
         pExpr = NULL;
      }
   virtual ~OutputItem();

   enum OptionType
     {
        DEFAULT = 0,
        OutputItem_IID,
        OutputItem_E
     };

    OutputItem* newOutputItem()
      {
         OutputItem* node = new OutputItem();
         node->pIoImpliedDo = pIoImpliedDo;  pIoImpliedDo = NULL;
         node->pExpr = pExpr;  pExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    IoImpliedDo* getIoImpliedDo() {return pIoImpliedDo;}
    Expr* getExpr() {return pExpr;}

    void setIoImpliedDo(IoImpliedDo* ioimplieddo) {pIoImpliedDo = ioimplieddo;}
    void setExpr(Expr* expr) {pExpr = expr;}

 private:
    IoImpliedDo* pIoImpliedDo;
    Expr* pExpr;
};

class OutputItemList : public Node
{
 public:
    OutputItemList()
      {
         pOutputItemList = new std::vector<OutputItem*>();
      }
   virtual ~OutputItemList();

    OutputItemList* newOutputItemList()
      {
         OutputItemList* node = new OutputItemList();
         delete node->pOutputItemList; node->pOutputItemList = pOutputItemList;  pOutputItemList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<OutputItem*>* getOutputItemList() {return pOutputItemList;}

    void appendOutputItem(OutputItem* outputitem) {pOutputItemList->push_back(outputitem);}

 private:
    std::vector<OutputItem*>* pOutputItemList;
};

class IoImpliedDo : public Node
{
 public:
    IoImpliedDo()
      {
         pIoImpliedDoObjectList = NULL;
         pIoImpliedDoControl = NULL;
      }
   virtual ~IoImpliedDo();

    IoImpliedDo* newIoImpliedDo()
      {
         IoImpliedDo* node = new IoImpliedDo();
         node->pIoImpliedDoObjectList = pIoImpliedDoObjectList;  pIoImpliedDoObjectList = NULL;
         node->pIoImpliedDoControl = pIoImpliedDoControl;  pIoImpliedDoControl = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    IoImpliedDoObjectList* getIoImpliedDoObjectList() {return pIoImpliedDoObjectList;}
    IoImpliedDoControl* getIoImpliedDoControl() {return pIoImpliedDoControl;}

    void setIoImpliedDoObjectList(IoImpliedDoObjectList* ioimplieddoobjectlist) {pIoImpliedDoObjectList = ioimplieddoobjectlist;}
    void setIoImpliedDoControl(IoImpliedDoControl* ioimplieddocontrol) {pIoImpliedDoControl = ioimplieddocontrol;}

 private:
    IoImpliedDoObjectList* pIoImpliedDoObjectList;
    IoImpliedDoControl* pIoImpliedDoControl;
};

class IoImpliedDoObject : public Node
{
 public:
    IoImpliedDoObject()
      {
         pOutputItem = NULL;
         pInputItem = NULL;
      }
   virtual ~IoImpliedDoObject();

   enum OptionType
     {
        DEFAULT = 0,
        IoImpliedDoObject_OI,
        IoImpliedDoObject_II
     };

    IoImpliedDoObject* newIoImpliedDoObject()
      {
         IoImpliedDoObject* node = new IoImpliedDoObject();
         node->pOutputItem = pOutputItem;  pOutputItem = NULL;
         node->pInputItem = pInputItem;  pInputItem = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    OutputItem* getOutputItem() {return pOutputItem;}
    InputItem* getInputItem() {return pInputItem;}

    void setOutputItem(OutputItem* outputitem) {pOutputItem = outputitem;}
    void setInputItem(InputItem* inputitem) {pInputItem = inputitem;}

 private:
    OutputItem* pOutputItem;
    InputItem* pInputItem;
};

class IoImpliedDoObjectList : public Node
{
 public:
    IoImpliedDoObjectList()
      {
         pIoImpliedDoObjectList = new std::vector<IoImpliedDoObject*>();
      }
   virtual ~IoImpliedDoObjectList();

    IoImpliedDoObjectList* newIoImpliedDoObjectList()
      {
         IoImpliedDoObjectList* node = new IoImpliedDoObjectList();
         delete node->pIoImpliedDoObjectList; node->pIoImpliedDoObjectList = pIoImpliedDoObjectList;  pIoImpliedDoObjectList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<IoImpliedDoObject*>* getIoImpliedDoObjectList() {return pIoImpliedDoObjectList;}

    void appendIoImpliedDoObject(IoImpliedDoObject* ioimplieddoobject) {pIoImpliedDoObjectList->push_back(ioimplieddoobject);}

 private:
    std::vector<IoImpliedDoObject*>* pIoImpliedDoObjectList;
};

class IoImpliedDoControl : public Node
{
 public:
    IoImpliedDoControl()
      {
         pDoVariable = NULL;
         pIntExpr = NULL;
      }
   virtual ~IoImpliedDoControl();

    IoImpliedDoControl* newIoImpliedDoControl()
      {
         IoImpliedDoControl* node = new IoImpliedDoControl();
         node->pDoVariable = pDoVariable;  pDoVariable = NULL;
         node->pIntExpr = pIntExpr;  pIntExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    DoVariable* getDoVariable() {return pDoVariable;}
    IntExpr* getIntExpr() {return pIntExpr;}

    void setDoVariable(DoVariable* dovariable) {pDoVariable = dovariable;}
    void setIntExpr(IntExpr* intexpr) {pIntExpr = intexpr;}

 private:
    DoVariable* pDoVariable;
    IntExpr* pIntExpr;
};

class WaitStmt : public Statement
{
 public:
    WaitStmt()
      {
         pLabel = NULL;
         pWaitSpecList = NULL;
         pEOS = NULL;
      }
   virtual ~WaitStmt();

    WaitStmt* newWaitStmt()
      {
         WaitStmt* node = new WaitStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pWaitSpecList = pWaitSpecList;  pWaitSpecList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    WaitSpecList* getWaitSpecList() {return pWaitSpecList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setWaitSpecList(WaitSpecList* waitspeclist) {pWaitSpecList = waitspeclist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    WaitSpecList* pWaitSpecList;
    EOS* pEOS;
};

class WaitSpec : public Node
{
 public:
    WaitSpec()
      {
         pIntExpr = NULL;
         pIomsgVariable = NULL;
         pLblRef = NULL;
         pFileUnitNumber = NULL;
      }
   virtual ~WaitSpec();

   enum OptionType
     {
        DEFAULT = 0,
        WaitSpec_IOSTAT,
        WaitSpec_IOMSG,
        WaitSpec_ID,
        WaitSpec_ERR,
        WaitSpec_EOR,
        WaitSpec_END,
        WaitSpec_UNIT
     };

    WaitSpec* newWaitSpec()
      {
         WaitSpec* node = new WaitSpec();
         node->pIntExpr = pIntExpr;  pIntExpr = NULL;
         node->pIomsgVariable = pIomsgVariable;  pIomsgVariable = NULL;
         node->pLblRef = pLblRef;  pLblRef = NULL;
         node->pFileUnitNumber = pFileUnitNumber;  pFileUnitNumber = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    IntExpr* getIntExpr() {return pIntExpr;}
    IomsgVariable* getIomsgVariable() {return pIomsgVariable;}
    LblRef* getLblRef() {return pLblRef;}
    FileUnitNumber* getFileUnitNumber() {return pFileUnitNumber;}

    void setIntExpr(IntExpr* intexpr) {pIntExpr = intexpr;}
    void setIomsgVariable(IomsgVariable* iomsgvariable) {pIomsgVariable = iomsgvariable;}
    void setLblRef(LblRef* lblref) {pLblRef = lblref;}
    void setFileUnitNumber(FileUnitNumber* fileunitnumber) {pFileUnitNumber = fileunitnumber;}

 private:
    IntExpr* pIntExpr;
    IomsgVariable* pIomsgVariable;
    LblRef* pLblRef;
    FileUnitNumber* pFileUnitNumber;
};

class WaitSpecList : public Node
{
 public:
    WaitSpecList()
      {
         pWaitSpecList = new std::vector<WaitSpec*>();
      }
   virtual ~WaitSpecList();

    WaitSpecList* newWaitSpecList()
      {
         WaitSpecList* node = new WaitSpecList();
         delete node->pWaitSpecList; node->pWaitSpecList = pWaitSpecList;  pWaitSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<WaitSpec*>* getWaitSpecList() {return pWaitSpecList;}

    void appendWaitSpec(WaitSpec* waitspec) {pWaitSpecList->push_back(waitspec);}

 private:
    std::vector<WaitSpec*>* pWaitSpecList;
};

class BackspaceStmt : public Statement
{
 public:
    BackspaceStmt()
      {
         pLabel = NULL;
         pPositionSpecList = NULL;
         pEOS = NULL;
         pFileUnitNumber = NULL;
      }
   virtual ~BackspaceStmt();

   enum OptionType
     {
        DEFAULT = 0,
        BackspaceStmt_PSL,
        BackspaceStmt_FUN
     };

    BackspaceStmt* newBackspaceStmt()
      {
         BackspaceStmt* node = new BackspaceStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pPositionSpecList = pPositionSpecList;  pPositionSpecList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->pFileUnitNumber = pFileUnitNumber;  pFileUnitNumber = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    PositionSpecList* getPositionSpecList() {return pPositionSpecList;}
    EOS* getEOS() {return pEOS;}
    FileUnitNumber* getFileUnitNumber() {return pFileUnitNumber;}

    void setLabel(Label* label) {pLabel = label;}
    void setPositionSpecList(PositionSpecList* positionspeclist) {pPositionSpecList = positionspeclist;}
    void setEOS(EOS* eos) {pEOS = eos;}
    void setFileUnitNumber(FileUnitNumber* fileunitnumber) {pFileUnitNumber = fileunitnumber;}

 private:
    Label* pLabel;
    PositionSpecList* pPositionSpecList;
    EOS* pEOS;
    FileUnitNumber* pFileUnitNumber;
};

class EndfileStmt : public Statement
{
 public:
    EndfileStmt()
      {
         pLabel = NULL;
         pPositionSpecList = NULL;
         pEOS = NULL;
         pFileUnitNumber = NULL;
      }
   virtual ~EndfileStmt();

   enum OptionType
     {
        DEFAULT = 0,
        EndfileStmt_PSL,
        EndfileStmt_FUN
     };

    EndfileStmt* newEndfileStmt()
      {
         EndfileStmt* node = new EndfileStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pPositionSpecList = pPositionSpecList;  pPositionSpecList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->pFileUnitNumber = pFileUnitNumber;  pFileUnitNumber = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    PositionSpecList* getPositionSpecList() {return pPositionSpecList;}
    EOS* getEOS() {return pEOS;}
    FileUnitNumber* getFileUnitNumber() {return pFileUnitNumber;}

    void setLabel(Label* label) {pLabel = label;}
    void setPositionSpecList(PositionSpecList* positionspeclist) {pPositionSpecList = positionspeclist;}
    void setEOS(EOS* eos) {pEOS = eos;}
    void setFileUnitNumber(FileUnitNumber* fileunitnumber) {pFileUnitNumber = fileunitnumber;}

 private:
    Label* pLabel;
    PositionSpecList* pPositionSpecList;
    EOS* pEOS;
    FileUnitNumber* pFileUnitNumber;
};

class RewindStmt : public Statement
{
 public:
    RewindStmt()
      {
         pLabel = NULL;
         pPositionSpecList = NULL;
         pEOS = NULL;
         pFileUnitNumber = NULL;
      }
   virtual ~RewindStmt();

   enum OptionType
     {
        DEFAULT = 0,
        RewindStmt_PSL,
        RewindStmt_FUN
     };

    RewindStmt* newRewindStmt()
      {
         RewindStmt* node = new RewindStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pPositionSpecList = pPositionSpecList;  pPositionSpecList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->pFileUnitNumber = pFileUnitNumber;  pFileUnitNumber = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    PositionSpecList* getPositionSpecList() {return pPositionSpecList;}
    EOS* getEOS() {return pEOS;}
    FileUnitNumber* getFileUnitNumber() {return pFileUnitNumber;}

    void setLabel(Label* label) {pLabel = label;}
    void setPositionSpecList(PositionSpecList* positionspeclist) {pPositionSpecList = positionspeclist;}
    void setEOS(EOS* eos) {pEOS = eos;}
    void setFileUnitNumber(FileUnitNumber* fileunitnumber) {pFileUnitNumber = fileunitnumber;}

 private:
    Label* pLabel;
    PositionSpecList* pPositionSpecList;
    EOS* pEOS;
    FileUnitNumber* pFileUnitNumber;
};

class PositionSpec : public Node
{
 public:
    PositionSpec()
      {
         pLblRef = NULL;
         pIomsgVariable = NULL;
         pIntVariable = NULL;
         pFileUnitNumber = NULL;
      }
   virtual ~PositionSpec();

   enum OptionType
     {
        DEFAULT = 0,
        PositionSpec_ERR,
        PositionSpec_IOMSG,
        PositionSpec_IOSTAT,
        PositionSpec_UNIT
     };

    PositionSpec* newPositionSpec()
      {
         PositionSpec* node = new PositionSpec();
         node->pLblRef = pLblRef;  pLblRef = NULL;
         node->pIomsgVariable = pIomsgVariable;  pIomsgVariable = NULL;
         node->pIntVariable = pIntVariable;  pIntVariable = NULL;
         node->pFileUnitNumber = pFileUnitNumber;  pFileUnitNumber = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    LblRef* getLblRef() {return pLblRef;}
    IomsgVariable* getIomsgVariable() {return pIomsgVariable;}
    IntVariable* getIntVariable() {return pIntVariable;}
    FileUnitNumber* getFileUnitNumber() {return pFileUnitNumber;}

    void setLblRef(LblRef* lblref) {pLblRef = lblref;}
    void setIomsgVariable(IomsgVariable* iomsgvariable) {pIomsgVariable = iomsgvariable;}
    void setIntVariable(IntVariable* intvariable) {pIntVariable = intvariable;}
    void setFileUnitNumber(FileUnitNumber* fileunitnumber) {pFileUnitNumber = fileunitnumber;}

 private:
    LblRef* pLblRef;
    IomsgVariable* pIomsgVariable;
    IntVariable* pIntVariable;
    FileUnitNumber* pFileUnitNumber;
};

class PositionSpecList : public Node
{
 public:
    PositionSpecList()
      {
         pPositionSpecList = new std::vector<PositionSpec*>();
      }
   virtual ~PositionSpecList();

    PositionSpecList* newPositionSpecList()
      {
         PositionSpecList* node = new PositionSpecList();
         delete node->pPositionSpecList; node->pPositionSpecList = pPositionSpecList;  pPositionSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<PositionSpec*>* getPositionSpecList() {return pPositionSpecList;}

    void appendPositionSpec(PositionSpec* positionspec) {pPositionSpecList->push_back(positionspec);}

 private:
    std::vector<PositionSpec*>* pPositionSpecList;
};

class FlushStmt : public Statement
{
 public:
    FlushStmt()
      {
         pLabel = NULL;
         pFlushSpecList = NULL;
         pEOS = NULL;
         pFileUnitNumber = NULL;
      }
   virtual ~FlushStmt();

   enum OptionType
     {
        DEFAULT = 0,
        FlushStmt_FSL,
        FlushStmt_FUN
     };

    FlushStmt* newFlushStmt()
      {
         FlushStmt* node = new FlushStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pFlushSpecList = pFlushSpecList;  pFlushSpecList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->pFileUnitNumber = pFileUnitNumber;  pFileUnitNumber = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    FlushSpecList* getFlushSpecList() {return pFlushSpecList;}
    EOS* getEOS() {return pEOS;}
    FileUnitNumber* getFileUnitNumber() {return pFileUnitNumber;}

    void setLabel(Label* label) {pLabel = label;}
    void setFlushSpecList(FlushSpecList* flushspeclist) {pFlushSpecList = flushspeclist;}
    void setEOS(EOS* eos) {pEOS = eos;}
    void setFileUnitNumber(FileUnitNumber* fileunitnumber) {pFileUnitNumber = fileunitnumber;}

 private:
    Label* pLabel;
    FlushSpecList* pFlushSpecList;
    EOS* pEOS;
    FileUnitNumber* pFileUnitNumber;
};

class FlushSpec : public Node
{
 public:
    FlushSpec()
      {
         pLblRef = NULL;
         pIomsgVariable = NULL;
         pIntVariable = NULL;
         pFileUnitNumber = NULL;
      }
   virtual ~FlushSpec();

   enum OptionType
     {
        DEFAULT = 0,
        FlushSpec_ERR,
        FlushSpec_IOMSG,
        FlushSpec_IOSTAT,
        FlushSpec_UNIT
     };

    FlushSpec* newFlushSpec()
      {
         FlushSpec* node = new FlushSpec();
         node->pLblRef = pLblRef;  pLblRef = NULL;
         node->pIomsgVariable = pIomsgVariable;  pIomsgVariable = NULL;
         node->pIntVariable = pIntVariable;  pIntVariable = NULL;
         node->pFileUnitNumber = pFileUnitNumber;  pFileUnitNumber = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    LblRef* getLblRef() {return pLblRef;}
    IomsgVariable* getIomsgVariable() {return pIomsgVariable;}
    IntVariable* getIntVariable() {return pIntVariable;}
    FileUnitNumber* getFileUnitNumber() {return pFileUnitNumber;}

    void setLblRef(LblRef* lblref) {pLblRef = lblref;}
    void setIomsgVariable(IomsgVariable* iomsgvariable) {pIomsgVariable = iomsgvariable;}
    void setIntVariable(IntVariable* intvariable) {pIntVariable = intvariable;}
    void setFileUnitNumber(FileUnitNumber* fileunitnumber) {pFileUnitNumber = fileunitnumber;}

 private:
    LblRef* pLblRef;
    IomsgVariable* pIomsgVariable;
    IntVariable* pIntVariable;
    FileUnitNumber* pFileUnitNumber;
};

class FlushSpecList : public Node
{
 public:
    FlushSpecList()
      {
         pFlushSpecList = new std::vector<FlushSpec*>();
      }
   virtual ~FlushSpecList();

    FlushSpecList* newFlushSpecList()
      {
         FlushSpecList* node = new FlushSpecList();
         delete node->pFlushSpecList; node->pFlushSpecList = pFlushSpecList;  pFlushSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<FlushSpec*>* getFlushSpecList() {return pFlushSpecList;}

    void appendFlushSpec(FlushSpec* flushspec) {pFlushSpecList->push_back(flushspec);}

 private:
    std::vector<FlushSpec*>* pFlushSpecList;
};

class InquireStmt : public Statement
{
 public:
    InquireStmt()
      {
         pLabel = NULL;
         pIntVariable = NULL;
         pOutputItemList = NULL;
         pEOS = NULL;
         pInquireSpecList = NULL;
      }
   virtual ~InquireStmt();

   enum OptionType
     {
        DEFAULT = 0,
        InquireStmt_IV,
        InquireStmt_ISL
     };

    InquireStmt* newInquireStmt()
      {
         InquireStmt* node = new InquireStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pIntVariable = pIntVariable;  pIntVariable = NULL;
         node->pOutputItemList = pOutputItemList;  pOutputItemList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->pInquireSpecList = pInquireSpecList;  pInquireSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    IntVariable* getIntVariable() {return pIntVariable;}
    OutputItemList* getOutputItemList() {return pOutputItemList;}
    EOS* getEOS() {return pEOS;}
    InquireSpecList* getInquireSpecList() {return pInquireSpecList;}

    void setLabel(Label* label) {pLabel = label;}
    void setIntVariable(IntVariable* intvariable) {pIntVariable = intvariable;}
    void setOutputItemList(OutputItemList* outputitemlist) {pOutputItemList = outputitemlist;}
    void setEOS(EOS* eos) {pEOS = eos;}
    void setInquireSpecList(InquireSpecList* inquirespeclist) {pInquireSpecList = inquirespeclist;}

 private:
    Label* pLabel;
    IntVariable* pIntVariable;
    OutputItemList* pOutputItemList;
    EOS* pEOS;
    InquireSpecList* pInquireSpecList;
};

class InquireSpec : public Node
{
 public:
    InquireSpec()
      {
         pDefaultCharExpr = NULL;
         pIntVariable = NULL;
         pLogicalVariable = NULL;
         pIomsgVariable = NULL;
         pIntExpr = NULL;
         pLblRef = NULL;
         pFileNameExpr = NULL;
         pFileUnitNumber = NULL;
      }
   virtual ~InquireSpec();

   enum OptionType
     {
        DEFAULT = 0,
        InquireSpec_WRITE,
        InquireSpec_UNFORMATTED,
        InquireSpec_STREAM,
        InquireSpec_SIZE,
        InquireSpec_SIGN,
        InquireSpec_SEQUENTIAL,
        InquireSpec_ROUND,
        InquireSpec_RECL,
        InquireSpec_READWRITE,
        InquireSpec_READ,
        InquireSpec_POSITION,
        InquireSpec_POS,
        InquireSpec_PENDING,
        InquireSpec_PAD,
        InquireSpec_OPENED,
        InquireSpec_NUMBER,
        InquireSpec_NEXTREC,
        InquireSpec_NAMED,
        InquireSpec_NAME,
        InquireSpec_IOSTAT,
        InquireSpec_IOMSG,
        InquireSpec_ID,
        InquireSpec_FORMATTED,
        InquireSpec_FORM,
        InquireSpec_EXIST,
        InquireSpec_ERR,
        InquireSpec_ENCODING,
        InquireSpec_DIRECT,
        InquireSpec_DELIM,
        InquireSpec_DECIMAL,
        InquireSpec_BLANK,
        InquireSpec_ASYNC,
        InquireSpec_ACTION,
        InquireSpec_ACCESS,
        InquireSpec_FILE,
        InquireSpec_UNIT
     };

    InquireSpec* newInquireSpec()
      {
         InquireSpec* node = new InquireSpec();
         node->pDefaultCharExpr = pDefaultCharExpr;  pDefaultCharExpr = NULL;
         node->pIntVariable = pIntVariable;  pIntVariable = NULL;
         node->pLogicalVariable = pLogicalVariable;  pLogicalVariable = NULL;
         node->pIomsgVariable = pIomsgVariable;  pIomsgVariable = NULL;
         node->pIntExpr = pIntExpr;  pIntExpr = NULL;
         node->pLblRef = pLblRef;  pLblRef = NULL;
         node->pFileNameExpr = pFileNameExpr;  pFileNameExpr = NULL;
         node->pFileUnitNumber = pFileUnitNumber;  pFileUnitNumber = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    DefaultCharExpr* getDefaultCharExpr() {return pDefaultCharExpr;}
    IntVariable* getIntVariable() {return pIntVariable;}
    LogicalVariable* getLogicalVariable() {return pLogicalVariable;}
    IomsgVariable* getIomsgVariable() {return pIomsgVariable;}
    IntExpr* getIntExpr() {return pIntExpr;}
    LblRef* getLblRef() {return pLblRef;}
    FileNameExpr* getFileNameExpr() {return pFileNameExpr;}
    FileUnitNumber* getFileUnitNumber() {return pFileUnitNumber;}

    void setDefaultCharExpr(DefaultCharExpr* defaultcharexpr) {pDefaultCharExpr = defaultcharexpr;}
    void setIntVariable(IntVariable* intvariable) {pIntVariable = intvariable;}
    void setLogicalVariable(LogicalVariable* logicalvariable) {pLogicalVariable = logicalvariable;}
    void setIomsgVariable(IomsgVariable* iomsgvariable) {pIomsgVariable = iomsgvariable;}
    void setIntExpr(IntExpr* intexpr) {pIntExpr = intexpr;}
    void setLblRef(LblRef* lblref) {pLblRef = lblref;}
    void setFileNameExpr(FileNameExpr* filenameexpr) {pFileNameExpr = filenameexpr;}
    void setFileUnitNumber(FileUnitNumber* fileunitnumber) {pFileUnitNumber = fileunitnumber;}

 private:
    DefaultCharExpr* pDefaultCharExpr;
    IntVariable* pIntVariable;
    LogicalVariable* pLogicalVariable;
    IomsgVariable* pIomsgVariable;
    IntExpr* pIntExpr;
    LblRef* pLblRef;
    FileNameExpr* pFileNameExpr;
    FileUnitNumber* pFileUnitNumber;
};

class InquireSpecList : public Node
{
 public:
    InquireSpecList()
      {
         pInquireSpecList = new std::vector<InquireSpec*>();
      }
   virtual ~InquireSpecList();

    InquireSpecList* newInquireSpecList()
      {
         InquireSpecList* node = new InquireSpecList();
         delete node->pInquireSpecList; node->pInquireSpecList = pInquireSpecList;  pInquireSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<InquireSpec*>* getInquireSpecList() {return pInquireSpecList;}

    void appendInquireSpec(InquireSpec* inquirespec) {pInquireSpecList->push_back(inquirespec);}

 private:
    std::vector<InquireSpec*>* pInquireSpecList;
};

class FormatStmt : public Statement
{
 public:
    FormatStmt()
      {
         pLabel = NULL;
         pFormatSpecification = NULL;
         pEOS = NULL;
      }
   virtual ~FormatStmt();

    FormatStmt* newFormatStmt()
      {
         FormatStmt* node = new FormatStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pFormatSpecification = pFormatSpecification;  pFormatSpecification = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    FormatSpecification* getFormatSpecification() {return pFormatSpecification;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setFormatSpecification(FormatSpecification* formatspecification) {pFormatSpecification = formatspecification;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    FormatSpecification* pFormatSpecification;
    EOS* pEOS;
};

class FormatSpecification : public Node
{
 public:
    FormatSpecification()
      {
         pFormatItems = NULL;
         pUnlimitedFormatItem = NULL;
      }
   virtual ~FormatSpecification();

   enum OptionType
     {
        DEFAULT = 0,
        FormatSpecification_0,
        FormatSpecification_UFI
     };

    FormatSpecification* newFormatSpecification()
      {
         FormatSpecification* node = new FormatSpecification();
         node->pFormatItems = pFormatItems;  pFormatItems = NULL;
         node->pUnlimitedFormatItem = pUnlimitedFormatItem;  pUnlimitedFormatItem = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    FormatItems* getFormatItems() {return pFormatItems;}
    UnlimitedFormatItem* getUnlimitedFormatItem() {return pUnlimitedFormatItem;}

    void setFormatItems(FormatItems* formatitems) {pFormatItems = formatitems;}
    void setUnlimitedFormatItem(UnlimitedFormatItem* unlimitedformatitem) {pUnlimitedFormatItem = unlimitedformatitem;}

 private:
    FormatItems* pFormatItems;
    UnlimitedFormatItem* pUnlimitedFormatItem;
};

class FormatItems : public Node
{
 public:
    FormatItems()
      {
         pFormatItem = NULL;
      }
   virtual ~FormatItems();

   enum OptionType
     {
        DEFAULT = 0,
        FormatItems_0,
        FormatItems_FIs
     };

    FormatItems* newFormatItems()
      {
         FormatItems* node = new FormatItems();
         node->pFormatItem = pFormatItem;  pFormatItem = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    FormatItem* getFormatItem() {return pFormatItem;}

    void setFormatItem(FormatItem* formatitem) {pFormatItem = formatitem;}

 private:
    FormatItem* pFormatItem;
};

class FormatItem : public Node
{
 public:
    FormatItem()
      {
         pIcon = NULL;
         pFormatItems = NULL;
         pScon = NULL;
         pControlEditDesc = NULL;
         pDataEditDesc = NULL;
      }
   virtual ~FormatItem();

   enum OptionType
     {
        DEFAULT = 0,
        FormatItem_FIs,
        FormatItem_CSED,
        FormatItem_CED,
        FormatItem_RED
     };

    FormatItem* newFormatItem()
      {
         FormatItem* node = new FormatItem();
         node->pIcon = pIcon;  pIcon = NULL;
         node->pFormatItems = pFormatItems;  pFormatItems = NULL;
         node->pScon = pScon;  pScon = NULL;
         node->pControlEditDesc = pControlEditDesc;  pControlEditDesc = NULL;
         node->pDataEditDesc = pDataEditDesc;  pDataEditDesc = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Icon* getIcon() {return pIcon;}
    FormatItems* getFormatItems() {return pFormatItems;}
    Scon* getScon() {return pScon;}
    ControlEditDesc* getControlEditDesc() {return pControlEditDesc;}
    DataEditDesc* getDataEditDesc() {return pDataEditDesc;}

    void setIcon(Icon* icon) {pIcon = icon;}
    void setFormatItems(FormatItems* formatitems) {pFormatItems = formatitems;}
    void setScon(Scon* scon) {pScon = scon;}
    void setControlEditDesc(ControlEditDesc* controleditdesc) {pControlEditDesc = controleditdesc;}
    void setDataEditDesc(DataEditDesc* dataeditdesc) {pDataEditDesc = dataeditdesc;}

 private:
    Icon* pIcon;
    FormatItems* pFormatItems;
    Scon* pScon;
    ControlEditDesc* pControlEditDesc;
    DataEditDesc* pDataEditDesc;
};

class UnlimitedFormatItem : public Node
{
 public:
    UnlimitedFormatItem()
      {
         pFormatItems = NULL;
      }
   virtual ~UnlimitedFormatItem();

    UnlimitedFormatItem* newUnlimitedFormatItem()
      {
         UnlimitedFormatItem* node = new UnlimitedFormatItem();
         node->pFormatItems = pFormatItems;  pFormatItems = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    FormatItems* getFormatItems() {return pFormatItems;}

    void setFormatItems(FormatItems* formatitems) {pFormatItems = formatitems;}

 private:
    FormatItems* pFormatItems;
};

class REditDesc : public Node
{
 public:
    REditDesc()
      {
         pIcon = NULL;
      }
   virtual ~REditDesc();

    REditDesc* newREditDesc()
      {
         REditDesc* node = new REditDesc();
         node->pIcon = pIcon;  pIcon = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Icon* getIcon() {return pIcon;}

    void setIcon(Icon* icon) {pIcon = icon;}

 private:
    Icon* pIcon;
};

class DataEditDesc : public Node
{
 public:
    DataEditDesc()
      {
         pScon = NULL;
         pVEditDescList = NULL;
         pIcon = NULL;
      }
   virtual ~DataEditDesc();

   enum OptionType
     {
        DEFAULT = 0,
        DataEditDesc_DT,
        DataEditDesc_D,
        DataEditDesc_A,
        DataEditDesc_L,
        DataEditDesc_G,
        DataEditDesc_GE,
        DataEditDesc_ES,
        DataEditDesc_EN,
        DataEditDesc_E,
        DataEditDesc_F,
        DataEditDesc_Z,
        DataEditDesc_O,
        DataEditDesc_B,
        DataEditDesc_I
     };

    DataEditDesc* newDataEditDesc()
      {
         DataEditDesc* node = new DataEditDesc();
         node->pScon = pScon;  pScon = NULL;
         node->pVEditDescList = pVEditDescList;  pVEditDescList = NULL;
         node->pIcon = pIcon;  pIcon = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Scon* getScon() {return pScon;}
    VEditDescList* getVEditDescList() {return pVEditDescList;}
    Icon* getIcon() {return pIcon;}

    void setScon(Scon* scon) {pScon = scon;}
    void setVEditDescList(VEditDescList* veditdesclist) {pVEditDescList = veditdesclist;}
    void setIcon(Icon* icon) {pIcon = icon;}

 private:
    Scon* pScon;
    VEditDescList* pVEditDescList;
    Icon* pIcon;
};

class WEditDesc : public Node
{
 public:
    WEditDesc()
      {
         pIcon = NULL;
      }
   virtual ~WEditDesc();

    WEditDesc* newWEditDesc()
      {
         WEditDesc* node = new WEditDesc();
         node->pIcon = pIcon;  pIcon = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Icon* getIcon() {return pIcon;}

    void setIcon(Icon* icon) {pIcon = icon;}

 private:
    Icon* pIcon;
};

class MEditDesc : public Node
{
 public:
    MEditDesc()
      {
         pIcon = NULL;
      }
   virtual ~MEditDesc();

    MEditDesc* newMEditDesc()
      {
         MEditDesc* node = new MEditDesc();
         node->pIcon = pIcon;  pIcon = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Icon* getIcon() {return pIcon;}

    void setIcon(Icon* icon) {pIcon = icon;}

 private:
    Icon* pIcon;
};

class DEditDesc : public Node
{
 public:
    DEditDesc()
      {
         pIcon = NULL;
      }
   virtual ~DEditDesc();

    DEditDesc* newDEditDesc()
      {
         DEditDesc* node = new DEditDesc();
         node->pIcon = pIcon;  pIcon = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Icon* getIcon() {return pIcon;}

    void setIcon(Icon* icon) {pIcon = icon;}

 private:
    Icon* pIcon;
};

class EEditDesc : public Node
{
 public:
    EEditDesc()
      {
         pIcon = NULL;
      }
   virtual ~EEditDesc();

    EEditDesc* newEEditDesc()
      {
         EEditDesc* node = new EEditDesc();
         node->pIcon = pIcon;  pIcon = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Icon* getIcon() {return pIcon;}

    void setIcon(Icon* icon) {pIcon = icon;}

 private:
    Icon* pIcon;
};

class VEditDesc : public Node
{
 public:
    VEditDesc()
      {
         pIcon = NULL;
      }
   virtual ~VEditDesc();

    VEditDesc* newVEditDesc()
      {
         VEditDesc* node = new VEditDesc();
         node->pIcon = pIcon;  pIcon = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Icon* getIcon() {return pIcon;}

    void setIcon(Icon* icon) {pIcon = icon;}

 private:
    Icon* pIcon;
};

class VEditDescList : public Node
{
 public:
    VEditDescList()
      {
         pVEditDescList = new std::vector<VEditDesc*>();
      }
   virtual ~VEditDescList();

    VEditDescList* newVEditDescList()
      {
         VEditDescList* node = new VEditDescList();
         delete node->pVEditDescList; node->pVEditDescList = pVEditDescList;  pVEditDescList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<VEditDesc*>* getVEditDescList() {return pVEditDescList;}

    void appendVEditDesc(VEditDesc* veditdesc) {pVEditDescList->push_back(veditdesc);}

 private:
    std::vector<VEditDesc*>* pVEditDescList;
};

class ControlEditDesc : public Node
{
 public:
    ControlEditDesc()
      {
         pDecimalEditDesc = NULL;
         pRoundEditDesc = NULL;
         pBlankInterpEditDesc = NULL;
         pSignedIntLiteralConstant = NULL;
         pSignEditDesc = NULL;
         pIcon = NULL;
         pPositionEditDesc = NULL;
      }
   virtual ~ControlEditDesc();

   enum OptionType
     {
        DEFAULT = 0,
        ControlEditDesc_DED,
        ControlEditDesc_RED,
        ControlEditDesc_BIED,
        ControlEditDesc_P,
        ControlEditDesc_SED,
        ControlEditDesc_COLON,
        ControlEditDesc_SLASH,
        ControlEditDesc_PED
     };

    ControlEditDesc* newControlEditDesc()
      {
         ControlEditDesc* node = new ControlEditDesc();
         node->pDecimalEditDesc = pDecimalEditDesc;  pDecimalEditDesc = NULL;
         node->pRoundEditDesc = pRoundEditDesc;  pRoundEditDesc = NULL;
         node->pBlankInterpEditDesc = pBlankInterpEditDesc;  pBlankInterpEditDesc = NULL;
         node->pSignedIntLiteralConstant = pSignedIntLiteralConstant;  pSignedIntLiteralConstant = NULL;
         node->pSignEditDesc = pSignEditDesc;  pSignEditDesc = NULL;
         node->pIcon = pIcon;  pIcon = NULL;
         node->pPositionEditDesc = pPositionEditDesc;  pPositionEditDesc = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    DecimalEditDesc* getDecimalEditDesc() {return pDecimalEditDesc;}
    RoundEditDesc* getRoundEditDesc() {return pRoundEditDesc;}
    BlankInterpEditDesc* getBlankInterpEditDesc() {return pBlankInterpEditDesc;}
    SignedIntLiteralConstant* getSignedIntLiteralConstant() {return pSignedIntLiteralConstant;}
    SignEditDesc* getSignEditDesc() {return pSignEditDesc;}
    Icon* getIcon() {return pIcon;}
    PositionEditDesc* getPositionEditDesc() {return pPositionEditDesc;}

    void setDecimalEditDesc(DecimalEditDesc* decimaleditdesc) {pDecimalEditDesc = decimaleditdesc;}
    void setRoundEditDesc(RoundEditDesc* roundeditdesc) {pRoundEditDesc = roundeditdesc;}
    void setBlankInterpEditDesc(BlankInterpEditDesc* blankinterpeditdesc) {pBlankInterpEditDesc = blankinterpeditdesc;}
    void setSignedIntLiteralConstant(SignedIntLiteralConstant* signedintliteralconstant) {pSignedIntLiteralConstant = signedintliteralconstant;}
    void setSignEditDesc(SignEditDesc* signeditdesc) {pSignEditDesc = signeditdesc;}
    void setIcon(Icon* icon) {pIcon = icon;}
    void setPositionEditDesc(PositionEditDesc* positioneditdesc) {pPositionEditDesc = positioneditdesc;}

 private:
    DecimalEditDesc* pDecimalEditDesc;
    RoundEditDesc* pRoundEditDesc;
    BlankInterpEditDesc* pBlankInterpEditDesc;
    SignedIntLiteralConstant* pSignedIntLiteralConstant;
    SignEditDesc* pSignEditDesc;
    Icon* pIcon;
    PositionEditDesc* pPositionEditDesc;
};

class KEditDesc : public Node
{
 public:
    KEditDesc()
      {
         pSignedIntLiteralConstant = NULL;
      }
   virtual ~KEditDesc();

    KEditDesc* newKEditDesc()
      {
         KEditDesc* node = new KEditDesc();
         node->pSignedIntLiteralConstant = pSignedIntLiteralConstant;  pSignedIntLiteralConstant = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    SignedIntLiteralConstant* getSignedIntLiteralConstant() {return pSignedIntLiteralConstant;}

    void setSignedIntLiteralConstant(SignedIntLiteralConstant* signedintliteralconstant) {pSignedIntLiteralConstant = signedintliteralconstant;}

 private:
    SignedIntLiteralConstant* pSignedIntLiteralConstant;
};

class PositionEditDesc : public Node
{
 public:
    PositionEditDesc()
      {
         pIcon = NULL;
      }
   virtual ~PositionEditDesc();

   enum OptionType
     {
        DEFAULT = 0,
        PositionEditDesc_X,
        PositionEditDesc_TR,
        PositionEditDesc_TL,
        PositionEditDesc_T
     };

    PositionEditDesc* newPositionEditDesc()
      {
         PositionEditDesc* node = new PositionEditDesc();
         node->pIcon = pIcon;  pIcon = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Icon* getIcon() {return pIcon;}

    void setIcon(Icon* icon) {pIcon = icon;}

 private:
    Icon* pIcon;
};

class NEditDesc : public Node
{
 public:
    NEditDesc()
      {
         pIcon = NULL;
      }
   virtual ~NEditDesc();

    NEditDesc* newNEditDesc()
      {
         NEditDesc* node = new NEditDesc();
         node->pIcon = pIcon;  pIcon = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Icon* getIcon() {return pIcon;}

    void setIcon(Icon* icon) {pIcon = icon;}

 private:
    Icon* pIcon;
};

class SignEditDesc : public Node
{
 public:
    SignEditDesc()
      {
      }
   virtual ~SignEditDesc();

   enum OptionType
     {
        DEFAULT = 0,
        SignEditDesc_S,
        SignEditDesc_SP,
        SignEditDesc_SS
     };

    SignEditDesc* newSignEditDesc()
      {
         SignEditDesc* node = new SignEditDesc();
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

 private:
};

class BlankInterpEditDesc : public Node
{
 public:
    BlankInterpEditDesc()
      {
      }
   virtual ~BlankInterpEditDesc();

   enum OptionType
     {
        DEFAULT = 0,
        BlonkInterpEditDesc_BZ,
        BlonkInterpEditDesc_BN
     };

    BlankInterpEditDesc* newBlankInterpEditDesc()
      {
         BlankInterpEditDesc* node = new BlankInterpEditDesc();
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

 private:
};

class RoundEditDesc : public Node
{
 public:
    RoundEditDesc()
      {
      }
   virtual ~RoundEditDesc();

   enum OptionType
     {
        DEFAULT = 0,
        RoundEditDesc_RP,
        RoundEditDesc_RC,
        RoundEditDesc_RN,
        RoundEditDesc_RZ,
        RoundEditDesc_RD,
        RoundEditDesc_RU
     };

    RoundEditDesc* newRoundEditDesc()
      {
         RoundEditDesc* node = new RoundEditDesc();
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

 private:
};

class DecimalEditDesc : public Node
{
 public:
    DecimalEditDesc()
      {
      }
   virtual ~DecimalEditDesc();

   enum OptionType
     {
        DEFAULT = 0,
        DecimalEditDesc_DP,
        DecimalEditDesc_DC
     };

    DecimalEditDesc* newDecimalEditDesc()
      {
         DecimalEditDesc* node = new DecimalEditDesc();
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

 private:
};

class CharStringEditDesc : public Node
{
 public:
    CharStringEditDesc()
      {
         pScon = NULL;
      }
   virtual ~CharStringEditDesc();

    CharStringEditDesc* newCharStringEditDesc()
      {
         CharStringEditDesc* node = new CharStringEditDesc();
         node->pScon = pScon;  pScon = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Scon* getScon() {return pScon;}

    void setScon(Scon* scon) {pScon = scon;}

 private:
    Scon* pScon;
};

class MainProgram : public Node
{
 public:
    MainProgram()
      {
         pProgramStmt = NULL;
         pInitialSpecPart = NULL;
         pSpecAndExecPart = NULL;
         pInternalSubprogramPart = NULL;
         pEndProgramStmt = NULL;
      }
   virtual ~MainProgram();

    MainProgram* newMainProgram()
      {
         MainProgram* node = new MainProgram();
         node->pProgramStmt = pProgramStmt;  pProgramStmt = NULL;
         node->pInitialSpecPart = pInitialSpecPart;  pInitialSpecPart = NULL;
         node->pSpecAndExecPart = pSpecAndExecPart;  pSpecAndExecPart = NULL;
         node->pInternalSubprogramPart = pInternalSubprogramPart;  pInternalSubprogramPart = NULL;
         node->pEndProgramStmt = pEndProgramStmt;  pEndProgramStmt = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    ProgramStmt* getProgramStmt() {return pProgramStmt;}
    InitialSpecPart* getInitialSpecPart() {return pInitialSpecPart;}
    SpecAndExecPart* getSpecAndExecPart() {return pSpecAndExecPart;}
    InternalSubprogramPart* getInternalSubprogramPart() {return pInternalSubprogramPart;}
    EndProgramStmt* getEndProgramStmt() {return pEndProgramStmt;}

    void setProgramStmt(ProgramStmt* programstmt) {pProgramStmt = programstmt;}
    void setInitialSpecPart(InitialSpecPart* specificationpart) {pInitialSpecPart = specificationpart;}
    void setSpecAndExecPart(SpecAndExecPart* executionpart) {pSpecAndExecPart = executionpart;}
    void setInternalSubprogramPart(InternalSubprogramPart* internalsubprogrampart) {pInternalSubprogramPart = internalsubprogrampart;}
    void setEndProgramStmt(EndProgramStmt* endprogramstmt) {pEndProgramStmt = endprogramstmt;}

 private:
    ProgramStmt* pProgramStmt;
    InitialSpecPart* pInitialSpecPart;
    SpecAndExecPart* pSpecAndExecPart;
    InternalSubprogramPart* pInternalSubprogramPart;
    EndProgramStmt* pEndProgramStmt;
};

class ProgramStmt : public Statement
{
 public:
    ProgramStmt()
      {
         pLabel = NULL;
         pProgramName = NULL;
         pEOS = NULL;
      }
   virtual ~ProgramStmt();

    ProgramStmt* newProgramStmt()
      {
         ProgramStmt* node = new ProgramStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pProgramName = pProgramName;  pProgramName = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Name* getProgramName() {return pProgramName;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setProgramName(Name* programname) {pProgramName = programname;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Name* pProgramName;
    EOS* pEOS;
};

class EndProgramStmt : public Statement
{
 public:
    EndProgramStmt()
      {
         pLabel = NULL;
         pProgramName = NULL;
         pEOS = NULL;
      }
   virtual ~EndProgramStmt();

    EndProgramStmt* newEndProgramStmt()
      {
         EndProgramStmt* node = new EndProgramStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pProgramName = pProgramName;  pProgramName = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Name* getProgramName() {return pProgramName;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setProgramName(Name* programname) {pProgramName = programname;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Name* pProgramName;
    EOS* pEOS;
};

class Module : public Node
{
 public:
    Module()
      {
         pModuleStmt = NULL;
         pSpecificationPart = NULL;
         pModuleSubprogramPart = NULL;
         pEndModuleStmt = NULL;
      }
   virtual ~Module();

    Module* newModule()
      {
         Module* node = new Module();
         node->pModuleStmt = pModuleStmt;  pModuleStmt = NULL;
         node->pSpecificationPart = pSpecificationPart;  pSpecificationPart = NULL;
         node->pModuleSubprogramPart = pModuleSubprogramPart;  pModuleSubprogramPart = NULL;
         node->pEndModuleStmt = pEndModuleStmt;  pEndModuleStmt = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    ModuleStmt* getModuleStmt() {return pModuleStmt;}
    SpecificationPart* getSpecificationPart() {return pSpecificationPart;}
    ModuleSubprogramPart* getModuleSubprogramPart() {return pModuleSubprogramPart;}
    EndModuleStmt* getEndModuleStmt() {return pEndModuleStmt;}

    void setModuleStmt(ModuleStmt* modulestmt) {pModuleStmt = modulestmt;}
    void setSpecificationPart(SpecificationPart* specificationpart) {pSpecificationPart = specificationpart;}
    void setModuleSubprogramPart(ModuleSubprogramPart* modulesubprogrampart) {pModuleSubprogramPart = modulesubprogrampart;}
    void setEndModuleStmt(EndModuleStmt* endmodulestmt) {pEndModuleStmt = endmodulestmt;}

 private:
    ModuleStmt* pModuleStmt;
    SpecificationPart* pSpecificationPart;
    ModuleSubprogramPart* pModuleSubprogramPart;
    EndModuleStmt* pEndModuleStmt;
};

class ModuleStmt : public Statement
{
 public:
    ModuleStmt()
      {
         pLabel = NULL;
         pModuleName = NULL;
         pEOS = NULL;
      }
   virtual ~ModuleStmt();

    ModuleStmt* newModuleStmt()
      {
         ModuleStmt* node = new ModuleStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pModuleName = pModuleName;  pModuleName = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Name* getModuleName() {return pModuleName;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setModuleName(Name* modulename) {pModuleName = modulename;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Name* pModuleName;
    EOS* pEOS;
};

class EndModuleStmt : public Statement
{
 public:
    EndModuleStmt()
      {
         pLabel = NULL;
         pModuleName = NULL;
         pEOS = NULL;
      }
   virtual ~EndModuleStmt();

    EndModuleStmt* newEndModuleStmt()
      {
         EndModuleStmt* node = new EndModuleStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pModuleName = pModuleName;  pModuleName = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Name* getModuleName() {return pModuleName;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setModuleName(Name* modulename) {pModuleName = modulename;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Name* pModuleName;
    EOS* pEOS;
};

class ModuleSubprogramPart : public Node
{
 public:
    ModuleSubprogramPart()
      {
         pContainsStmt = NULL;
         pModuleSubprogramList = new std::vector<ModuleSubprogram*>();
      }
   virtual ~ModuleSubprogramPart();

    ModuleSubprogramPart* newModuleSubprogramPart()
      {
         ModuleSubprogramPart* node = new ModuleSubprogramPart();
         node->pContainsStmt = pContainsStmt;  pContainsStmt = NULL;
         delete node->pModuleSubprogramList; node->pModuleSubprogramList = pModuleSubprogramList;  pModuleSubprogramList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    ContainsStmt* getContainsStmt() {return pContainsStmt;}
    std::vector<ModuleSubprogram*>* getModuleSubprogramList() {return pModuleSubprogramList;}

    void setContainsStmt(ContainsStmt* containsstmt) {pContainsStmt = containsstmt;}
    void appendModuleSubprogram(ModuleSubprogram* modulesubprogram) {pModuleSubprogramList->push_back(modulesubprogram);}

 private:
    ContainsStmt* pContainsStmt;
    std::vector<ModuleSubprogram*>* pModuleSubprogramList;
};

class ModuleSubprogram : public Node
{
 public:
    ModuleSubprogram()
      {
         pSeparateModuleSubprogram = NULL;
         pSubroutineSubprogram = NULL;
         pFunctionSubprogram = NULL;
      }
   virtual ~ModuleSubprogram();

   enum OptionType
     {
        DEFAULT = 0,
        ModuleSubprogram_SMS,
        ModuleSubprogram_SS,
        ModuleSubprogram_FS
     };

    ModuleSubprogram* newModuleSubprogram()
      {
         ModuleSubprogram* node = new ModuleSubprogram();
         node->pSeparateModuleSubprogram = pSeparateModuleSubprogram;  pSeparateModuleSubprogram = NULL;
         node->pSubroutineSubprogram = pSubroutineSubprogram;  pSubroutineSubprogram = NULL;
         node->pFunctionSubprogram = pFunctionSubprogram;  pFunctionSubprogram = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    SeparateModuleSubprogram* getSeparateModuleSubprogram() {return pSeparateModuleSubprogram;}
    SubroutineSubprogram* getSubroutineSubprogram() {return pSubroutineSubprogram;}
    FunctionSubprogram* getFunctionSubprogram() {return pFunctionSubprogram;}

    void setSeparateModuleSubprogram(SeparateModuleSubprogram* separatemodulesubprogram) {pSeparateModuleSubprogram = separatemodulesubprogram;}
    void setSubroutineSubprogram(SubroutineSubprogram* subroutinesubprogram) {pSubroutineSubprogram = subroutinesubprogram;}
    void setFunctionSubprogram(FunctionSubprogram* functionsubprogram) {pFunctionSubprogram = functionsubprogram;}

 private:
    SeparateModuleSubprogram* pSeparateModuleSubprogram;
    SubroutineSubprogram* pSubroutineSubprogram;
    FunctionSubprogram* pFunctionSubprogram;
};

class UseStmt : public Statement
{
 public:
    UseStmt()
      {
         pLabel = NULL;
         pModuleNature = NULL;
         pModuleName = NULL;
         pOnlyList = NULL;
         pEOS = NULL;
         pRenameList = NULL;
      }
   virtual ~UseStmt();

   enum OptionType
     {
        DEFAULT = 0,
        UseStmt_OL,
        UseStmt_RL
     };

    UseStmt* newUseStmt()
      {
         UseStmt* node = new UseStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pModuleNature = pModuleNature;  pModuleNature = NULL;
         node->pModuleName = pModuleName;  pModuleName = NULL;
         node->pOnlyList = pOnlyList;  pOnlyList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->pRenameList = pRenameList;  pRenameList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    ModuleNature* getModuleNature() {return pModuleNature;}
    Name* getModuleName() {return pModuleName;}
    OnlyList* getOnlyList() {return pOnlyList;}
    EOS* getEOS() {return pEOS;}
    RenameList* getRenameList() {return pRenameList;}

    void setLabel(Label* label) {pLabel = label;}
    void setModuleNature(ModuleNature* modulenature) {pModuleNature = modulenature;}
    void setModuleName(Name* modulename) {pModuleName = modulename;}
    void setOnlyList(OnlyList* onlylist) {pOnlyList = onlylist;}
    void setEOS(EOS* eos) {pEOS = eos;}
    void setRenameList(RenameList* renamelist) {pRenameList = renamelist;}

 private:
    Label* pLabel;
    ModuleNature* pModuleNature;
    Name* pModuleName;
    OnlyList* pOnlyList;
    EOS* pEOS;
    RenameList* pRenameList;
};

class ModuleNature : public Node
{
 public:
    ModuleNature()
      {
      }
   virtual ~ModuleNature();

   enum OptionType
     {
        DEFAULT = 0,
        ModuleNature_NON_INTRINSIC,
        ModuleNature_INTRINSIC
     };

    ModuleNature* newModuleNature()
      {
         ModuleNature* node = new ModuleNature();
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

 private:
};

class Rename : public Node
{
 public:
    Rename()
      {
         pLocalDefinedOperator = NULL;
         pUseDefinedOperator = NULL;
         pLocalName = NULL;
         pUseName = NULL;
      }
   virtual ~Rename();

   enum OptionType
     {
        DEFAULT = 0,
        Rename_OP,
        Rename_LN
     };

    Rename* newRename()
      {
         Rename* node = new Rename();
         node->pLocalDefinedOperator = pLocalDefinedOperator;  pLocalDefinedOperator = NULL;
         node->pUseDefinedOperator = pUseDefinedOperator;  pUseDefinedOperator = NULL;
         node->pLocalName = pLocalName;  pLocalName = NULL;
         node->pUseName = pUseName;  pUseName = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    LocalDefinedOperator* getLocalDefinedOperator() {return pLocalDefinedOperator;}
    UseDefinedOperator* getUseDefinedOperator() {return pUseDefinedOperator;}
    Name* getLocalName() {return pLocalName;}
    Name* getUseName() {return pUseName;}

    void setLocalDefinedOperator(LocalDefinedOperator* localdefinedoperator) {pLocalDefinedOperator = localdefinedoperator;}
    void setUseDefinedOperator(UseDefinedOperator* usedefinedoperator) {pUseDefinedOperator = usedefinedoperator;}
    void setLocalName(Name* localname) {pLocalName = localname;}
    void setUseName(Name* usename) {pUseName = usename;}

 private:
    LocalDefinedOperator* pLocalDefinedOperator;
    UseDefinedOperator* pUseDefinedOperator;
    Name* pLocalName;
    Name* pUseName;
};

class RenameList : public Node
{
 public:
    RenameList()
      {
         pRenameList = new std::vector<Rename*>();
      }
   virtual ~RenameList();

    RenameList* newRenameList()
      {
         RenameList* node = new RenameList();
         delete node->pRenameList; node->pRenameList = pRenameList;  pRenameList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<Rename*>* getRenameList() {return pRenameList;}

    void appendRename(Rename* rename) {pRenameList->push_back(rename);}

 private:
    std::vector<Rename*>* pRenameList;
};

class Only : public Node
{
 public:
    Only()
      {
         pRename = NULL;
         pOnlyUseName = NULL;
      }
   virtual ~Only();

   enum OptionType
     {
        DEFAULT = 0,
        Only_R,
        Only_OUN
     };

    Only* newOnly()
      {
         Only* node = new Only();
         node->pRename = pRename;  pRename = NULL;
         node->pOnlyUseName = pOnlyUseName;  pOnlyUseName = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Rename* getRename() {return pRename;}
    OnlyUseName* getOnlyUseName() {return pOnlyUseName;}

    void setRename(Rename* rename) {pRename = rename;}
    void setOnlyUseName(OnlyUseName* onlyusename) {pOnlyUseName = onlyusename;}

 private:
    Rename* pRename;
    OnlyUseName* pOnlyUseName;
};

class OnlyList : public Node
{
 public:
    OnlyList()
      {
         pOnlyList = new std::vector<Only*>();
      }
   virtual ~OnlyList();

    OnlyList* newOnlyList()
      {
         OnlyList* node = new OnlyList();
         delete node->pOnlyList; node->pOnlyList = pOnlyList;  pOnlyList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<Only*>* getOnlyList() {return pOnlyList;}

    void appendOnly(Only* only) {pOnlyList->push_back(only);}

 private:
    std::vector<Only*>* pOnlyList;
};

class OnlyUseName : public Node
{
 public:
    OnlyUseName()
      {
         pIdent = NULL;
      }
   virtual ~OnlyUseName();

    OnlyUseName* newOnlyUseName()
      {
         OnlyUseName* node = new OnlyUseName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class LocalDefinedOperator : public Node
{
 public:
    LocalDefinedOperator()
      {
         pDefinedBinaryOp = NULL;
         pDefinedUnaryOp = NULL;
      }
   virtual ~LocalDefinedOperator();

   enum OptionType
     {
        DEFAULT = 0,
        LocalDefinedOperator_DBO,
        LocalDefinedOperator_DUO
     };

    LocalDefinedOperator* newLocalDefinedOperator()
      {
         LocalDefinedOperator* node = new LocalDefinedOperator();
         node->pDefinedBinaryOp = pDefinedBinaryOp;  pDefinedBinaryOp = NULL;
         node->pDefinedUnaryOp = pDefinedUnaryOp;  pDefinedUnaryOp = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    DefinedBinaryOp* getDefinedBinaryOp() {return pDefinedBinaryOp;}
    DefinedUnaryOp* getDefinedUnaryOp() {return pDefinedUnaryOp;}

    void setDefinedBinaryOp(DefinedBinaryOp* definedbinaryop) {pDefinedBinaryOp = definedbinaryop;}
    void setDefinedUnaryOp(DefinedUnaryOp* definedunaryop) {pDefinedUnaryOp = definedunaryop;}

 private:
    DefinedBinaryOp* pDefinedBinaryOp;
    DefinedUnaryOp* pDefinedUnaryOp;
};

class UseDefinedOperator : public Node
{
 public:
    UseDefinedOperator()
      {
         pDefinedBinaryOp = NULL;
         pDefinedUnaryOp = NULL;
      }
   virtual ~UseDefinedOperator();

   enum OptionType
     {
        DEFAULT = 0,
        UseDefinedOperator_DBO,
        UseDefinedOperator_DUO
     };

    UseDefinedOperator* newUseDefinedOperator()
      {
         UseDefinedOperator* node = new UseDefinedOperator();
         node->pDefinedBinaryOp = pDefinedBinaryOp;  pDefinedBinaryOp = NULL;
         node->pDefinedUnaryOp = pDefinedUnaryOp;  pDefinedUnaryOp = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    DefinedBinaryOp* getDefinedBinaryOp() {return pDefinedBinaryOp;}
    DefinedUnaryOp* getDefinedUnaryOp() {return pDefinedUnaryOp;}

    void setDefinedBinaryOp(DefinedBinaryOp* definedbinaryop) {pDefinedBinaryOp = definedbinaryop;}
    void setDefinedUnaryOp(DefinedUnaryOp* definedunaryop) {pDefinedUnaryOp = definedunaryop;}

 private:
    DefinedBinaryOp* pDefinedBinaryOp;
    DefinedUnaryOp* pDefinedUnaryOp;
};

class Submodule : public Node
{
 public:
    Submodule()
      {
         pSubmoduleStmt = NULL;
         pSpecificationPart = NULL;
         pModuleSubprogramPart = NULL;
         pEndSubmoduleStmt = NULL;
      }
   virtual ~Submodule();

    Submodule* newSubmodule()
      {
         Submodule* node = new Submodule();
         node->pSubmoduleStmt = pSubmoduleStmt;  pSubmoduleStmt = NULL;
         node->pSpecificationPart = pSpecificationPart;  pSpecificationPart = NULL;
         node->pModuleSubprogramPart = pModuleSubprogramPart;  pModuleSubprogramPart = NULL;
         node->pEndSubmoduleStmt = pEndSubmoduleStmt;  pEndSubmoduleStmt = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    SubmoduleStmt* getSubmoduleStmt() {return pSubmoduleStmt;}
    SpecificationPart* getSpecificationPart() {return pSpecificationPart;}
    ModuleSubprogramPart* getModuleSubprogramPart() {return pModuleSubprogramPart;}
    EndSubmoduleStmt* getEndSubmoduleStmt() {return pEndSubmoduleStmt;}

    void setSubmoduleStmt(SubmoduleStmt* submodulestmt) {pSubmoduleStmt = submodulestmt;}
    void setSpecificationPart(SpecificationPart* specificationpart) {pSpecificationPart = specificationpart;}
    void setModuleSubprogramPart(ModuleSubprogramPart* modulesubprogrampart) {pModuleSubprogramPart = modulesubprogrampart;}
    void setEndSubmoduleStmt(EndSubmoduleStmt* endsubmodulestmt) {pEndSubmoduleStmt = endsubmodulestmt;}

 private:
    SubmoduleStmt* pSubmoduleStmt;
    SpecificationPart* pSpecificationPart;
    ModuleSubprogramPart* pModuleSubprogramPart;
    EndSubmoduleStmt* pEndSubmoduleStmt;
};

class SubmoduleStmt : public Statement
{
 public:
    SubmoduleStmt()
      {
         pLabel = NULL;
         pParentIdentifier = NULL;
         pSubmoduleName = NULL;
         pEOS = NULL;
      }
   virtual ~SubmoduleStmt();

    SubmoduleStmt* newSubmoduleStmt()
      {
         SubmoduleStmt* node = new SubmoduleStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pParentIdentifier = pParentIdentifier;  pParentIdentifier = NULL;
         node->pSubmoduleName = pSubmoduleName;  pSubmoduleName = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    ParentIdentifier* getParentIdentifier() {return pParentIdentifier;}
    Name* getSubmoduleName() {return pSubmoduleName;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setParentIdentifier(ParentIdentifier* parentidentifier) {pParentIdentifier = parentidentifier;}
    void setSubmoduleName(Name* submodulename) {pSubmoduleName = submodulename;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    ParentIdentifier* pParentIdentifier;
    Name* pSubmoduleName;
    EOS* pEOS;
};

class ParentIdentifier : public Node
{
 public:
    ParentIdentifier()
      {
         pAncestorModuleName = NULL;
         pParentSubmoduleName = NULL;
      }
   virtual ~ParentIdentifier();

    ParentIdentifier* newParentIdentifier()
      {
         ParentIdentifier* node = new ParentIdentifier();
         node->pAncestorModuleName = pAncestorModuleName;  pAncestorModuleName = NULL;
         node->pParentSubmoduleName = pParentSubmoduleName;  pParentSubmoduleName = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Name* getAncestorModuleName() {return pAncestorModuleName;}
    Name* getParentSubmoduleName() {return pParentSubmoduleName;}

    void setAncestorModuleName(Name* ancestormodulename) {pAncestorModuleName = ancestormodulename;}
    void setParentSubmoduleName(Name* parentsubmodulename) {pParentSubmoduleName = parentsubmodulename;}

 private:
    Name* pAncestorModuleName;
    Name* pParentSubmoduleName;
};

class EndSubmoduleStmt : public Statement
{
 public:
    EndSubmoduleStmt()
      {
         pLabel = NULL;
         pSubmoduleName = NULL;
         pEOS = NULL;
      }
   virtual ~EndSubmoduleStmt();

    EndSubmoduleStmt* newEndSubmoduleStmt()
      {
         EndSubmoduleStmt* node = new EndSubmoduleStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pSubmoduleName = pSubmoduleName;  pSubmoduleName = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Name* getSubmoduleName() {return pSubmoduleName;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setSubmoduleName(Name* submodulename) {pSubmoduleName = submodulename;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Name* pSubmoduleName;
    EOS* pEOS;
};

class BlockData : public Node
{
 public:
    BlockData()
      {
         pBlockDataStmt = NULL;
         pSpecificationPart = NULL;
         pEndBlockDataStmt = NULL;
      }
   virtual ~BlockData();

    BlockData* newBlockData()
      {
         BlockData* node = new BlockData();
         node->pBlockDataStmt = pBlockDataStmt;  pBlockDataStmt = NULL;
         node->pSpecificationPart = pSpecificationPart;  pSpecificationPart = NULL;
         node->pEndBlockDataStmt = pEndBlockDataStmt;  pEndBlockDataStmt = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    BlockDataStmt* getBlockDataStmt() {return pBlockDataStmt;}
    SpecificationPart* getSpecificationPart() {return pSpecificationPart;}
    EndBlockDataStmt* getEndBlockDataStmt() {return pEndBlockDataStmt;}

    void setBlockDataStmt(BlockDataStmt* blockdatastmt) {pBlockDataStmt = blockdatastmt;}
    void setSpecificationPart(SpecificationPart* specificationpart) {pSpecificationPart = specificationpart;}
    void setEndBlockDataStmt(EndBlockDataStmt* endblockdatastmt) {pEndBlockDataStmt = endblockdatastmt;}

 private:
    BlockDataStmt* pBlockDataStmt;
    SpecificationPart* pSpecificationPart;
    EndBlockDataStmt* pEndBlockDataStmt;
};

class BlockDataStmt : public Statement
{
 public:
    BlockDataStmt()
      {
         pLabel = NULL;
         pBlockDataName = NULL;
         pEOS = NULL;
      }
   virtual ~BlockDataStmt();

    BlockDataStmt* newBlockDataStmt()
      {
         BlockDataStmt* node = new BlockDataStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pBlockDataName = pBlockDataName;  pBlockDataName = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Name* getBlockDataName() {return pBlockDataName;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setBlockDataName(Name* blockdataname) {pBlockDataName = blockdataname;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Name* pBlockDataName;
    EOS* pEOS;
};

class EndBlockDataStmt : public Statement
{
 public:
    EndBlockDataStmt()
      {
         pLabel = NULL;
         pEOS = NULL;
         pBlockDataName = NULL;
      }
   virtual ~EndBlockDataStmt();

   enum OptionType
     {
        DEFAULT = 0,
        EndBlockDataStmt_0,
        EndBlockDataStmt_BDN
     };

    EndBlockDataStmt* newEndBlockDataStmt()
      {
         EndBlockDataStmt* node = new EndBlockDataStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->pBlockDataName = pBlockDataName;  pBlockDataName = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    EOS* getEOS() {return pEOS;}
    Name* getBlockDataName() {return pBlockDataName;}

    void setLabel(Label* label) {pLabel = label;}
    void setEOS(EOS* eos) {pEOS = eos;}
    void setBlockDataName(Name* blockdataname) {pBlockDataName = blockdataname;}

 private:
    Label* pLabel;
    EOS* pEOS;
    Name* pBlockDataName;
};

class InterfaceBlock : public Node
{
 public:
    InterfaceBlock()
      {
         pInterfaceStmt = NULL;
         pInterfaceSpecificationList = new std::vector<InterfaceSpecification*>();
         pEndInterfaceStmt = NULL;
      }
   virtual ~InterfaceBlock();

    InterfaceBlock* newInterfaceBlock()
      {
         InterfaceBlock* node = new InterfaceBlock();
         node->pInterfaceStmt = pInterfaceStmt;  pInterfaceStmt = NULL;
         delete node->pInterfaceSpecificationList; node->pInterfaceSpecificationList = pInterfaceSpecificationList;  pInterfaceSpecificationList = NULL;
         node->pEndInterfaceStmt = pEndInterfaceStmt;  pEndInterfaceStmt = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    InterfaceStmt* getInterfaceStmt() {return pInterfaceStmt;}
    std::vector<InterfaceSpecification*>* getInterfaceSpecificationList() {return pInterfaceSpecificationList;}
    EndInterfaceStmt* getEndInterfaceStmt() {return pEndInterfaceStmt;}

    void setInterfaceStmt(InterfaceStmt* interfacestmt) {pInterfaceStmt = interfacestmt;}
    void appendInterfaceSpecification(InterfaceSpecification* interfacespecification) {pInterfaceSpecificationList->push_back(interfacespecification);}
    void setEndInterfaceStmt(EndInterfaceStmt* endinterfacestmt) {pEndInterfaceStmt = endinterfacestmt;}

 private:
    InterfaceStmt* pInterfaceStmt;
    std::vector<InterfaceSpecification*>* pInterfaceSpecificationList;
    EndInterfaceStmt* pEndInterfaceStmt;
};

class InterfaceSpecification : public Node
{
 public:
    InterfaceSpecification()
      {
         pProcedureStmt = NULL;
         pInterfaceBody = NULL;
      }
   virtual ~InterfaceSpecification();

   enum OptionType
     {
        DEFAULT = 0,
        InterfaceSpecification_PS,
        InterfaceSpecification_IB
     };

    InterfaceSpecification* newInterfaceSpecification()
      {
         InterfaceSpecification* node = new InterfaceSpecification();
         node->pProcedureStmt = pProcedureStmt;  pProcedureStmt = NULL;
         node->pInterfaceBody = pInterfaceBody;  pInterfaceBody = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    ProcedureStmt* getProcedureStmt() {return pProcedureStmt;}
    InterfaceBody* getInterfaceBody() {return pInterfaceBody;}

    void setProcedureStmt(ProcedureStmt* procedurestmt) {pProcedureStmt = procedurestmt;}
    void setInterfaceBody(InterfaceBody* interfacebody) {pInterfaceBody = interfacebody;}

 private:
    ProcedureStmt* pProcedureStmt;
    InterfaceBody* pInterfaceBody;
};

class InterfaceStmt : public Statement
{
 public:
    InterfaceStmt()
      {
         pLabel = NULL;
         pEOS = NULL;
         pGenericSpec = NULL;
      }
   virtual ~InterfaceStmt();

   enum OptionType
     {
        DEFAULT = 0,
        InterfaceStmt_AB,
        InterfaceStmt_GS
     };

    InterfaceStmt* newInterfaceStmt()
      {
         InterfaceStmt* node = new InterfaceStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->pGenericSpec = pGenericSpec;  pGenericSpec = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    EOS* getEOS() {return pEOS;}
    GenericSpec* getGenericSpec() {return pGenericSpec;}

    void setLabel(Label* label) {pLabel = label;}
    void setEOS(EOS* eos) {pEOS = eos;}
    void setGenericSpec(GenericSpec* genericspec) {pGenericSpec = genericspec;}

 private:
    Label* pLabel;
    EOS* pEOS;
    GenericSpec* pGenericSpec;
};

class EndInterfaceStmt : public Statement
{
 public:
    EndInterfaceStmt()
      {
         pLabel = NULL;
         pGenericSpec = NULL;
         pEOS = NULL;
      }
   virtual ~EndInterfaceStmt();

    EndInterfaceStmt* newEndInterfaceStmt()
      {
         EndInterfaceStmt* node = new EndInterfaceStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pGenericSpec = pGenericSpec;  pGenericSpec = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    GenericSpec* getGenericSpec() {return pGenericSpec;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setGenericSpec(GenericSpec* genericspec) {pGenericSpec = genericspec;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    GenericSpec* pGenericSpec;
    EOS* pEOS;
};

class InterfaceBody : public Node
{
 public:
    InterfaceBody()
      {
         pSubroutineStmt = NULL;
         pSpecificationPart = NULL;
         pEndSubroutineStmt = NULL;
         pFunctionStmt = NULL;
         pEndFunctionStmt = NULL;
      }
   virtual ~InterfaceBody();

   enum OptionType
     {
        DEFAULT = 0,
        InterfaceBody_SS,
        InterfaceBody_FS
     };

    InterfaceBody* newInterfaceBody()
      {
         InterfaceBody* node = new InterfaceBody();
         node->pSubroutineStmt = pSubroutineStmt;  pSubroutineStmt = NULL;
         node->pSpecificationPart = pSpecificationPart;  pSpecificationPart = NULL;
         node->pEndSubroutineStmt = pEndSubroutineStmt;  pEndSubroutineStmt = NULL;
         node->pFunctionStmt = pFunctionStmt;  pFunctionStmt = NULL;
         node->pEndFunctionStmt = pEndFunctionStmt;  pEndFunctionStmt = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    SubroutineStmt* getSubroutineStmt() {return pSubroutineStmt;}
    SpecificationPart* getSpecificationPart() {return pSpecificationPart;}
    EndSubroutineStmt* getEndSubroutineStmt() {return pEndSubroutineStmt;}
    FunctionStmt* getFunctionStmt() {return pFunctionStmt;}
    EndFunctionStmt* getEndFunctionStmt() {return pEndFunctionStmt;}

    void setSubroutineStmt(SubroutineStmt* subroutinestmt) {pSubroutineStmt = subroutinestmt;}
    void setSpecificationPart(SpecificationPart* specificationpart) {pSpecificationPart = specificationpart;}
    void setEndSubroutineStmt(EndSubroutineStmt* endsubroutinestmt) {pEndSubroutineStmt = endsubroutinestmt;}
    void setFunctionStmt(FunctionStmt* functionstmt) {pFunctionStmt = functionstmt;}
    void setEndFunctionStmt(EndFunctionStmt* endfunctionstmt) {pEndFunctionStmt = endfunctionstmt;}

 private:
    SubroutineStmt* pSubroutineStmt;
    SpecificationPart* pSpecificationPart;
    EndSubroutineStmt* pEndSubroutineStmt;
    FunctionStmt* pFunctionStmt;
    EndFunctionStmt* pEndFunctionStmt;
};

class ProcedureStmt : public Statement
{
 public:
    ProcedureStmt()
      {
         pLabel = NULL;
         pProcedureNameList = NULL;
         pEOS = NULL;
      }
   virtual ~ProcedureStmt();

   enum OptionType
     {
        DEFAULT = 0,
        ProcedureStmt_P,
        ProcedureStmt_MP
     };

    ProcedureStmt* newProcedureStmt()
      {
         ProcedureStmt* node = new ProcedureStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pProcedureNameList = pProcedureNameList;  pProcedureNameList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    ProcedureNameList* getProcedureNameList() {return pProcedureNameList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setProcedureNameList(ProcedureNameList* procedurenamelist) {pProcedureNameList = procedurenamelist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    ProcedureNameList* pProcedureNameList;
    EOS* pEOS;
};

class ProcedureNameList : public Node
{
 public:
    ProcedureNameList()
      {
         pProcedureNameList = new std::vector<Name*>();
      }
   virtual ~ProcedureNameList();

    ProcedureNameList* newProcedureNameList()
      {
         ProcedureNameList* node = new ProcedureNameList();
         delete node->pProcedureNameList; node->pProcedureNameList = pProcedureNameList;  pProcedureNameList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<Name*>* getProcedureNameList() {return pProcedureNameList;}

    void appendProcedureName(Name* procedurename) {pProcedureNameList->push_back(procedurename);}

 private:
    std::vector<Name*>* pProcedureNameList;
};

class GenericSpec : public Node
{
 public:
    GenericSpec()
      {
         pDefinedIoGenericSpec = NULL;
         pDefinedOperator = NULL;
         pGenericName = NULL;
      }
   virtual ~GenericSpec();

   enum OptionType
     {
        DEFAULT = 0,
        GenericSpec_DIGS,
        GenericSpec_AS,
        GenericSpec_OP,
        GenericSpec_GN
     };

    GenericSpec* newGenericSpec()
      {
         GenericSpec* node = new GenericSpec();
         node->pDefinedIoGenericSpec = pDefinedIoGenericSpec;  pDefinedIoGenericSpec = NULL;
         node->pDefinedOperator = pDefinedOperator;  pDefinedOperator = NULL;
         node->pGenericName = pGenericName;  pGenericName = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    DefinedIoGenericSpec* getDefinedIoGenericSpec() {return pDefinedIoGenericSpec;}
    DefinedOperator* getDefinedOperator() {return pDefinedOperator;}
    Name* getGenericName() {return pGenericName;}

    void setDefinedIoGenericSpec(DefinedIoGenericSpec* definediogenericspec) {pDefinedIoGenericSpec = definediogenericspec;}
    void setDefinedOperator(DefinedOperator* definedoperator) {pDefinedOperator = definedoperator;}
    void setGenericName(Name* genericname) {pGenericName = genericname;}

 private:
    DefinedIoGenericSpec* pDefinedIoGenericSpec;
    DefinedOperator* pDefinedOperator;
    Name* pGenericName;
};

class DefinedIoGenericSpec : public Node
{
 public:
    DefinedIoGenericSpec()
      {
      }
   virtual ~DefinedIoGenericSpec();

   enum OptionType
     {
        DEFAULT = 0,
        DefinedIoGenericSpec_WU,
        DefinedIoGenericSpec_WF,
        DefinedIoGenericSpec_RU,
        DefinedIoGenericSpec_RF
     };

    DefinedIoGenericSpec* newDefinedIoGenericSpec()
      {
         DefinedIoGenericSpec* node = new DefinedIoGenericSpec();
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

 private:
};

class ImportStmt : public Statement
{
 public:
    ImportStmt()
      {
         pLabel = NULL;
         pEOS = NULL;
         pImportNameList = NULL;
      }
   virtual ~ImportStmt();

   enum OptionType
     {
        DEFAULT = 0,
        ImportStmt_0,
        ImportStmt_INL
     };

    ImportStmt* newImportStmt()
      {
         ImportStmt* node = new ImportStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->pImportNameList = pImportNameList;  pImportNameList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    EOS* getEOS() {return pEOS;}
    ImportNameList* getImportNameList() {return pImportNameList;}

    void setLabel(Label* label) {pLabel = label;}
    void setEOS(EOS* eos) {pEOS = eos;}
    void setImportNameList(ImportNameList* importnamelist) {pImportNameList = importnamelist;}

 private:
    Label* pLabel;
    EOS* pEOS;
    ImportNameList* pImportNameList;
};

class ImportNameList : public Node
{
 public:
    ImportNameList()
      {
         pImportNameList = new std::vector<Name*>();
      }
   virtual ~ImportNameList();

    ImportNameList* newImportNameList()
      {
         ImportNameList* node = new ImportNameList();
         delete node->pImportNameList; node->pImportNameList = pImportNameList;  pImportNameList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<Name*>* getImportNameList() {return pImportNameList;}

    void appendImportName(Name* importname) {pImportNameList->push_back(importname);}

 private:
    std::vector<Name*>* pImportNameList;
};

class ExternalStmt : public Statement
{
 public:
    ExternalStmt()
      {
         pLabel = NULL;
         pExternalNameList = NULL;
         pEOS = NULL;
      }
   virtual ~ExternalStmt();

    ExternalStmt* newExternalStmt()
      {
         ExternalStmt* node = new ExternalStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pExternalNameList = pExternalNameList;  pExternalNameList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    ExternalNameList* getExternalNameList() {return pExternalNameList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setExternalNameList(ExternalNameList* externalnamelist) {pExternalNameList = externalnamelist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    ExternalNameList* pExternalNameList;
    EOS* pEOS;
};

class ProcedureDeclarationStmt : public Statement
{
 public:
    ProcedureDeclarationStmt()
      {
         pLabel = NULL;
         pProcInterface = NULL;
         pProcAttrSpecList = NULL;
         pProcDeclList = NULL;
         pEOS = NULL;
      }
   virtual ~ProcedureDeclarationStmt();

    ProcedureDeclarationStmt* newProcedureDeclarationStmt()
      {
         ProcedureDeclarationStmt* node = new ProcedureDeclarationStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pProcInterface = pProcInterface;  pProcInterface = NULL;
         node->pProcAttrSpecList = pProcAttrSpecList;  pProcAttrSpecList = NULL;
         node->pProcDeclList = pProcDeclList;  pProcDeclList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    ProcInterface* getProcInterface() {return pProcInterface;}
    ProcAttrSpecList* getProcAttrSpecList() {return pProcAttrSpecList;}
    ProcDeclList* getProcDeclList() {return pProcDeclList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setProcInterface(ProcInterface* procinterface) {pProcInterface = procinterface;}
    void setProcAttrSpecList(ProcAttrSpecList* procattrspeclist) {pProcAttrSpecList = procattrspeclist;}
    void setProcDeclList(ProcDeclList* procdecllist) {pProcDeclList = procdecllist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    ProcInterface* pProcInterface;
    ProcAttrSpecList* pProcAttrSpecList;
    ProcDeclList* pProcDeclList;
    EOS* pEOS;
};

class ProcInterface : public Node
{
 public:
    ProcInterface()
      {
         pDeclarationTypeSpec = NULL;
         pInterfaceName = NULL;
      }
   virtual ~ProcInterface();

   enum OptionType
     {
        DEFAULT = 0,
        ProcInterface_DTS,
        ProcInterface_IN
     };

    ProcInterface* newProcInterface()
      {
         ProcInterface* node = new ProcInterface();
         node->pDeclarationTypeSpec = pDeclarationTypeSpec;  pDeclarationTypeSpec = NULL;
         node->pInterfaceName = pInterfaceName;  pInterfaceName = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    DeclarationTypeSpec* getDeclarationTypeSpec() {return pDeclarationTypeSpec;}
    InterfaceName* getInterfaceName() {return pInterfaceName;}

    void setDeclarationTypeSpec(DeclarationTypeSpec* declarationtypespec) {pDeclarationTypeSpec = declarationtypespec;}
    void setInterfaceName(InterfaceName* interfacename) {pInterfaceName = interfacename;}

 private:
    DeclarationTypeSpec* pDeclarationTypeSpec;
    InterfaceName* pInterfaceName;
};

class ProcAttrSpec : public Node
{
 public:
    ProcAttrSpec()
      {
         pIntentSpec = NULL;
         pProcLanguageBindingSpec = NULL;
         pAccessSpec = NULL;
      }
   virtual ~ProcAttrSpec();

   enum OptionType
     {
        DEFAULT = 0,
        ProcAttrSpec_SAVE,
        ProcAttrSpec_POINTER,
        ProcAttrSpec_OPTIONAL,
        ProcAttrSpec_INTENT,
        ProcAttrSpec_PLBS,
        ProcAttrSpec_AS
     };

    ProcAttrSpec* newProcAttrSpec()
      {
         ProcAttrSpec* node = new ProcAttrSpec();
         node->pIntentSpec = pIntentSpec;  pIntentSpec = NULL;
         node->pProcLanguageBindingSpec = pProcLanguageBindingSpec;  pProcLanguageBindingSpec = NULL;
         node->pAccessSpec = pAccessSpec;  pAccessSpec = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    IntentSpec* getIntentSpec() {return pIntentSpec;}
    ProcLanguageBindingSpec* getProcLanguageBindingSpec() {return pProcLanguageBindingSpec;}
    AccessSpec* getAccessSpec() {return pAccessSpec;}

    void setIntentSpec(IntentSpec* intentspec) {pIntentSpec = intentspec;}
    void setProcLanguageBindingSpec(ProcLanguageBindingSpec* proclanguagebindingspec) {pProcLanguageBindingSpec = proclanguagebindingspec;}
    void setAccessSpec(AccessSpec* accessspec) {pAccessSpec = accessspec;}

 private:
    IntentSpec* pIntentSpec;
    ProcLanguageBindingSpec* pProcLanguageBindingSpec;
    AccessSpec* pAccessSpec;
};

class ProcAttrSpecList : public Node
{
 public:
    ProcAttrSpecList()
      {
         pProcAttrSpecList = new std::vector<ProcAttrSpec*>();
      }
   virtual ~ProcAttrSpecList();

    ProcAttrSpecList* newProcAttrSpecList()
      {
         ProcAttrSpecList* node = new ProcAttrSpecList();
         delete node->pProcAttrSpecList; node->pProcAttrSpecList = pProcAttrSpecList;  pProcAttrSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<ProcAttrSpec*>* getProcAttrSpecList() {return pProcAttrSpecList;}

    void appendProcAttrSpec(ProcAttrSpec* procattrspec) {pProcAttrSpecList->push_back(procattrspec);}

 private:
    std::vector<ProcAttrSpec*>* pProcAttrSpecList;
};

class ProcDecl : public Node
{
 public:
    ProcDecl()
      {
         pProcedureEntityName = NULL;
         pProcPointerInit = NULL;
      }
   virtual ~ProcDecl();

    ProcDecl* newProcDecl()
      {
         ProcDecl* node = new ProcDecl();
         node->pProcedureEntityName = pProcedureEntityName;  pProcedureEntityName = NULL;
         node->pProcPointerInit = pProcPointerInit;  pProcPointerInit = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Name* getProcedureEntityName() {return pProcedureEntityName;}
    ProcPointerInit* getProcPointerInit() {return pProcPointerInit;}

    void setProcedureEntityName(Name* procedureentityname) {pProcedureEntityName = procedureentityname;}
    void setProcPointerInit(ProcPointerInit* procpointerinit) {pProcPointerInit = procpointerinit;}

 private:
    Name* pProcedureEntityName;
    ProcPointerInit* pProcPointerInit;
};

class ProcDeclList : public Node
{
 public:
    ProcDeclList()
      {
         pProcDeclList = new std::vector<ProcDecl*>();
      }
   virtual ~ProcDeclList();

    ProcDeclList* newProcDeclList()
      {
         ProcDeclList* node = new ProcDeclList();
         delete node->pProcDeclList; node->pProcDeclList = pProcDeclList;  pProcDeclList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<ProcDecl*>* getProcDeclList() {return pProcDeclList;}

    void appendProcDecl(ProcDecl* procdecl) {pProcDeclList->push_back(procdecl);}

 private:
    std::vector<ProcDecl*>* pProcDeclList;
};

class InterfaceName : public Node
{
 public:
    InterfaceName()
      {
         pIdent = NULL;
      }
   virtual ~InterfaceName();

    InterfaceName* newInterfaceName()
      {
         InterfaceName* node = new InterfaceName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class ProcPointerInit : public Node
{
 public:
    ProcPointerInit()
      {
         pInitialProcTarget = NULL;
         pNullInit = NULL;
      }
   virtual ~ProcPointerInit();

   enum OptionType
     {
        DEFAULT = 0,
        ProcPointerInit_IPT,
        ProcPointerInit_NI
     };

    ProcPointerInit* newProcPointerInit()
      {
         ProcPointerInit* node = new ProcPointerInit();
         node->pInitialProcTarget = pInitialProcTarget;  pInitialProcTarget = NULL;
         node->pNullInit = pNullInit;  pNullInit = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    InitialProcTarget* getInitialProcTarget() {return pInitialProcTarget;}
    NullInit* getNullInit() {return pNullInit;}

    void setInitialProcTarget(InitialProcTarget* initialproctarget) {pInitialProcTarget = initialproctarget;}
    void setNullInit(NullInit* nullinit) {pNullInit = nullinit;}

 private:
    InitialProcTarget* pInitialProcTarget;
    NullInit* pNullInit;
};

class InitialProcTarget : public Node
{
 public:
    InitialProcTarget()
      {
         pProcedureName = NULL;
      }
   virtual ~InitialProcTarget();

    InitialProcTarget* newInitialProcTarget()
      {
         InitialProcTarget* node = new InitialProcTarget();
         node->pProcedureName = pProcedureName;  pProcedureName = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Name* getProcedureName() {return pProcedureName;}

    void setProcedureName(Name* procedurename) {pProcedureName = procedurename;}

 private:
    Name* pProcedureName;
};

class IntrinsicStmt : public Statement
{
 public:
    IntrinsicStmt()
      {
         pLabel = NULL;
         pIntrinsicProcedureNameList = NULL;
         pEOS = NULL;
      }
   virtual ~IntrinsicStmt();

    IntrinsicStmt* newIntrinsicStmt()
      {
         IntrinsicStmt* node = new IntrinsicStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pIntrinsicProcedureNameList = pIntrinsicProcedureNameList;  pIntrinsicProcedureNameList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    IntrinsicProcedureNameList* getIntrinsicProcedureNameList() {return pIntrinsicProcedureNameList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setIntrinsicProcedureNameList(IntrinsicProcedureNameList* intrinsicprocedurenamelist) {pIntrinsicProcedureNameList = intrinsicprocedurenamelist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    IntrinsicProcedureNameList* pIntrinsicProcedureNameList;
    EOS* pEOS;
};

class IntrinsicProcedureNameList : public Node
{
 public:
    IntrinsicProcedureNameList()
      {
         pIntrinsicProcedureNameList = new std::vector<Name*>();
      }
   virtual ~IntrinsicProcedureNameList();

    IntrinsicProcedureNameList* newIntrinsicProcedureNameList()
      {
         IntrinsicProcedureNameList* node = new IntrinsicProcedureNameList();
         delete node->pIntrinsicProcedureNameList; node->pIntrinsicProcedureNameList = pIntrinsicProcedureNameList;  pIntrinsicProcedureNameList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<Name*>* getIntrinsicProcedureNameList() {return pIntrinsicProcedureNameList;}

    void appendIntrinsicProcedureName(Name* intrinsicprocedurename) {pIntrinsicProcedureNameList->push_back(intrinsicprocedurename);}

 private:
    std::vector<Name*>* pIntrinsicProcedureNameList;
};

class FunctionReference : public Node
{
 public:
    FunctionReference()
      {
         pProcedureDesignator = NULL;
         pActualArgSpecList = NULL;
      }
   virtual ~FunctionReference();

    FunctionReference* newFunctionReference()
      {
         FunctionReference* node = new FunctionReference();
         node->pProcedureDesignator = pProcedureDesignator;  pProcedureDesignator = NULL;
         node->pActualArgSpecList = pActualArgSpecList;  pActualArgSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    ProcedureDesignator* getProcedureDesignator() {return pProcedureDesignator;}
    ActualArgSpecList* getActualArgSpecList() {return pActualArgSpecList;}

    void setProcedureDesignator(ProcedureDesignator* proceduredesignator) {pProcedureDesignator = proceduredesignator;}
    void setActualArgSpecList(ActualArgSpecList* actualargspeclist) {pActualArgSpecList = actualargspeclist;}

 private:
    ProcedureDesignator* pProcedureDesignator;
    ActualArgSpecList* pActualArgSpecList;
};

class CallStmt : public Statement
{
 public:
    CallStmt()
      {
         pLabel = NULL;
         pProcedureDesignator = NULL;
         pActualArgSpecList = NULL;
         pEOS = NULL;
      }
   virtual ~CallStmt();

    CallStmt* newCallStmt()
      {
         CallStmt* node = new CallStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pProcedureDesignator = pProcedureDesignator;  pProcedureDesignator = NULL;
         node->pActualArgSpecList = pActualArgSpecList;  pActualArgSpecList = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    ProcedureDesignator* getProcedureDesignator() {return pProcedureDesignator;}
    ActualArgSpecList* getActualArgSpecList() {return pActualArgSpecList;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setProcedureDesignator(ProcedureDesignator* proceduredesignator) {pProcedureDesignator = proceduredesignator;}
    void setActualArgSpecList(ActualArgSpecList* actualargspeclist) {pActualArgSpecList = actualargspeclist;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    ProcedureDesignator* pProcedureDesignator;
    ActualArgSpecList* pActualArgSpecList;
    EOS* pEOS;
};

class ProcedureDesignator : public Node
{
 public:
    ProcedureDesignator()
      {
         pDataRef = NULL;
         pBindingName = NULL;
         pProcedureName = NULL;
      }
   virtual ~ProcedureDesignator();

   enum OptionType
     {
        DEFAULT = 0,
        ProcedureDesignator_AMB,
        ProcedureDesignator_PN
     };

    ProcedureDesignator* newProcedureDesignator()
      {
         ProcedureDesignator* node = new ProcedureDesignator();
         node->pDataRef = pDataRef;  pDataRef = NULL;
         node->pBindingName = pBindingName;  pBindingName = NULL;
         node->pProcedureName = pProcedureName;  pProcedureName = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    DataRef* getDataRef() {return pDataRef;}
    Name* getBindingName() {return pBindingName;}
    Name* getProcedureName() {return pProcedureName;}

    void setDataRef(DataRef* dataref) {pDataRef = dataref;}
    void setBindingName(Name* bindingname) {pBindingName = bindingname;}
    void setProcedureName(Name* procedurename) {pProcedureName = procedurename;}

 private:
    DataRef* pDataRef;
    Name* pBindingName;
    Name* pProcedureName;
};

class ActualArgSpec : public Node
{
 public:
    ActualArgSpec()
      {
         pKeyword = NULL;
         pActualArg = NULL;
      }
   virtual ~ActualArgSpec();

    ActualArgSpec* newActualArgSpec()
      {
         ActualArgSpec* node = new ActualArgSpec();
         node->pKeyword = pKeyword;  pKeyword = NULL;
         node->pActualArg = pActualArg;  pActualArg = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Keyword* getKeyword() {return pKeyword;}
    ActualArg* getActualArg() {return pActualArg;}

    void setKeyword(Keyword* keyword) {pKeyword = keyword;}
    void setActualArg(ActualArg* actualarg) {pActualArg = actualarg;}

 private:
    Keyword* pKeyword;
    ActualArg* pActualArg;
};

class ActualArgSpecList : public Node
{
 public:
    ActualArgSpecList()
      {
         pActualArgSpecList = new std::vector<ActualArgSpec*>();
      }
   virtual ~ActualArgSpecList();

    ActualArgSpecList* newActualArgSpecList()
      {
         ActualArgSpecList* node = new ActualArgSpecList();
         delete node->pActualArgSpecList; node->pActualArgSpecList = pActualArgSpecList;  pActualArgSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<ActualArgSpec*>* getActualArgSpecList() {return pActualArgSpecList;}

    void appendActualArgSpec(ActualArgSpec* actualargspec) {pActualArgSpecList->push_back(actualargspec);}

 private:
    std::vector<ActualArgSpec*>* pActualArgSpecList;
};

class ActualArg : public Node
{
 public:
    ActualArg()
      {
         pAltReturnSpec = NULL;
         pExpr = NULL;
      }
   virtual ~ActualArg();

   enum OptionType
     {
        DEFAULT = 0,
        ActualArg_ARS,
        ActualArg_AMB
     };

    ActualArg* newActualArg()
      {
         ActualArg* node = new ActualArg();
         node->pAltReturnSpec = pAltReturnSpec;  pAltReturnSpec = NULL;
         node->pExpr = pExpr;  pExpr = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    AltReturnSpec* getAltReturnSpec() {return pAltReturnSpec;}
    Expr* getExpr() {return pExpr;}

    void setAltReturnSpec(AltReturnSpec* altreturnspec) {pAltReturnSpec = altreturnspec;}
    void setExpr(Expr* expr) {pExpr = expr;}

 private:
    AltReturnSpec* pAltReturnSpec;
    Expr* pExpr;
};

class AltReturnSpec : public Node
{
 public:
    AltReturnSpec()
      {
         pLabel = NULL;
      }
   virtual ~AltReturnSpec();

    AltReturnSpec* newAltReturnSpec()
      {
         AltReturnSpec* node = new AltReturnSpec();
         node->pLabel = pLabel;  pLabel = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}

    void setLabel(Label* label) {pLabel = label;}

 private:
    Label* pLabel;
};

class Prefix : public Node
{
 public:
    Prefix()
      {
         pPrefixSpecList = new std::vector<PrefixSpec*>();
      }
   virtual ~Prefix();

    Prefix* newPrefix()
      {
         Prefix* node = new Prefix();
         delete node->pPrefixSpecList; node->pPrefixSpecList = pPrefixSpecList;  pPrefixSpecList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<PrefixSpec*>* getPrefixSpecList() {return pPrefixSpecList;}

    void appendPrefixSpec(PrefixSpec* prefixspec) {pPrefixSpecList->push_back(prefixspec);}

 private:
    std::vector<PrefixSpec*>* pPrefixSpecList;
};

class PrefixSpec : public Node
{
 public:
    PrefixSpec()
      {
         pDeclarationTypeSpec = NULL;
      }
   virtual ~PrefixSpec();

   enum OptionType
     {
        DEFAULT = 0,
        PrefixSpec_RECURSIVE,
        PrefixSpec_PURE,
        PrefixSpec_MODULE,
        PrefixSpec_IMPURE,
        PrefixSpec_ELEMENTAL,
        PrefixSpec_DTS
     };

    PrefixSpec* newPrefixSpec()
      {
         PrefixSpec* node = new PrefixSpec();
         node->pDeclarationTypeSpec = pDeclarationTypeSpec;  pDeclarationTypeSpec = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    DeclarationTypeSpec* getDeclarationTypeSpec() {return pDeclarationTypeSpec;}

    void setDeclarationTypeSpec(DeclarationTypeSpec* declarationtypespec) {pDeclarationTypeSpec = declarationtypespec;}

 private:
    DeclarationTypeSpec* pDeclarationTypeSpec;
};

class FunctionSubprogram : public Node
{
 public:
    FunctionSubprogram()
      {
         pFunctionStmt = NULL;
         pInitialSpecPart = NULL;
         pSpecAndExecPart = NULL;
         pInternalSubprogramPart = NULL;
         pEndFunctionStmt = NULL;
      }
   virtual ~FunctionSubprogram();

    FunctionSubprogram* newFunctionSubprogram()
      {
         FunctionSubprogram* node = new FunctionSubprogram();
         node->pFunctionStmt = pFunctionStmt;  pFunctionStmt = NULL;
         node->pInitialSpecPart = pInitialSpecPart;  pInitialSpecPart = NULL;
         node->pSpecAndExecPart = pSpecAndExecPart;  pSpecAndExecPart = NULL;
         node->pInternalSubprogramPart = pInternalSubprogramPart;  pInternalSubprogramPart = NULL;
         node->pEndFunctionStmt = pEndFunctionStmt;  pEndFunctionStmt = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    FunctionStmt* getFunctionStmt() {return pFunctionStmt;}
    InitialSpecPart* getInitialSpecPart() {return pInitialSpecPart;}
    SpecAndExecPart* getSpecAndExecPart() {return pSpecAndExecPart;}
    InternalSubprogramPart* getInternalSubprogramPart() {return pInternalSubprogramPart;}
    EndFunctionStmt* getEndFunctionStmt() {return pEndFunctionStmt;}

    void setFunctionStmt(FunctionStmt* functionstmt) {pFunctionStmt = functionstmt;}
    void setInitialSpecPart(InitialSpecPart* specPart) {pInitialSpecPart = specPart;}
    void setSpecAndExecPart(SpecAndExecPart* execPart) {pSpecAndExecPart = execPart;}
    void setInternalSubprogramPart(InternalSubprogramPart* internalsubprogrampart) {pInternalSubprogramPart = internalsubprogrampart;}
    void setEndFunctionStmt(EndFunctionStmt* endfunctionstmt) {pEndFunctionStmt = endfunctionstmt;}

 private:
    FunctionStmt* pFunctionStmt;
    InitialSpecPart* pInitialSpecPart;
    SpecAndExecPart* pSpecAndExecPart;
    InternalSubprogramPart* pInternalSubprogramPart;
    EndFunctionStmt* pEndFunctionStmt;
};

class FunctionStmt : public Statement
{
 public:
    FunctionStmt()
      {
         pLabel = NULL;
         pPrefix = NULL;
         pFunctionName = NULL;
         pDummyArgNameList = NULL;
         pSuffix = NULL;
         pEOS = NULL;
      }
   virtual ~FunctionStmt();

    FunctionStmt* newFunctionStmt()
      {
         FunctionStmt* node = new FunctionStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pPrefix = pPrefix;  pPrefix = NULL;
         node->pFunctionName = pFunctionName;  pFunctionName = NULL;
         node->pDummyArgNameList = pDummyArgNameList;  pDummyArgNameList = NULL;
         node->pSuffix = pSuffix;  pSuffix = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Prefix* getPrefix() {return pPrefix;}
    Name* getFunctionName() {return pFunctionName;}
    DummyArgNameList* getDummyArgNameList() {return pDummyArgNameList;}
    Suffix* getSuffix() {return pSuffix;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setPrefix(Prefix* prefix) {pPrefix = prefix;}
    void setFunctionName(Name* functionname) {pFunctionName = functionname;}
    void setDummyArgNameList(DummyArgNameList* dummyargnamelist) {pDummyArgNameList = dummyargnamelist;}
    void setSuffix(Suffix* suffix) {pSuffix = suffix;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Prefix* pPrefix;
    Name* pFunctionName;
    DummyArgNameList* pDummyArgNameList;
    Suffix* pSuffix;
    EOS* pEOS;
};

class ProcLanguageBindingSpec : public Node
{
 public:
    ProcLanguageBindingSpec()
      {
         pLanguageBindingSpec = NULL;
      }
   virtual ~ProcLanguageBindingSpec();

    ProcLanguageBindingSpec* newProcLanguageBindingSpec()
      {
         ProcLanguageBindingSpec* node = new ProcLanguageBindingSpec();
         node->pLanguageBindingSpec = pLanguageBindingSpec;  pLanguageBindingSpec = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    LanguageBindingSpec* getLanguageBindingSpec() {return pLanguageBindingSpec;}

    void setLanguageBindingSpec(LanguageBindingSpec* languagebindingspec) {pLanguageBindingSpec = languagebindingspec;}

 private:
    LanguageBindingSpec* pLanguageBindingSpec;
};

class DummyArgName : public Node
{
 public:
    DummyArgName()
      {
         pIdent = NULL;
      }
   virtual ~DummyArgName();

    DummyArgName* newDummyArgName()
      {
         DummyArgName* node = new DummyArgName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class Suffix : public Node
{
 public:
    Suffix()
      {
         pResultName = NULL;
         pProcLanguageBindingSpec = NULL;
      }
   virtual ~Suffix();

   enum OptionType
     {
        DEFAULT = 0,
        Suffix_RP,
        Suffix_PR
     };

    Suffix* newSuffix()
      {
         Suffix* node = new Suffix();
         node->pResultName = pResultName;  pResultName = NULL;
         node->pProcLanguageBindingSpec = pProcLanguageBindingSpec;  pProcLanguageBindingSpec = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Name* getResultName() {return pResultName;}
    ProcLanguageBindingSpec* getProcLanguageBindingSpec() {return pProcLanguageBindingSpec;}

    void setResultName(Name* resultname) {pResultName = resultname;}
    void setProcLanguageBindingSpec(ProcLanguageBindingSpec* proclanguagebindingspec) {pProcLanguageBindingSpec = proclanguagebindingspec;}

 private:
    Name* pResultName;
    ProcLanguageBindingSpec* pProcLanguageBindingSpec;
};

class EndFunctionStmt : public Statement
{
 public:
    EndFunctionStmt()
      {
         pLabel = NULL;
         pFunctionName = NULL;
         pEOS = NULL;
      }
   virtual ~EndFunctionStmt();

    EndFunctionStmt* newEndFunctionStmt()
      {
         EndFunctionStmt* node = new EndFunctionStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pFunctionName = pFunctionName;  pFunctionName = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Name* getFunctionName() {return pFunctionName;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setFunctionName(Name* functionname) {pFunctionName = functionname;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Name* pFunctionName;
    EOS* pEOS;
};

class SubroutineSubprogram : public Node
{
 public:
    SubroutineSubprogram()
      {
         pSubroutineStmt = NULL;
         pInitialSpecPart = NULL;
         pSpecAndExecPart = NULL;
         pInternalSubprogramPart = NULL;
         pEndSubroutineStmt = NULL;
      }
   virtual ~SubroutineSubprogram();

    SubroutineSubprogram* newSubroutineSubprogram()
      {
         SubroutineSubprogram* node = new SubroutineSubprogram();
         node->pSubroutineStmt = pSubroutineStmt;  pSubroutineStmt = NULL;
         node->pInitialSpecPart = pInitialSpecPart;  pInitialSpecPart = NULL;
         node->pSpecAndExecPart = pSpecAndExecPart;  pSpecAndExecPart = NULL;
         node->pInternalSubprogramPart = pInternalSubprogramPart;  pInternalSubprogramPart = NULL;
         node->pEndSubroutineStmt = pEndSubroutineStmt;  pEndSubroutineStmt = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    SubroutineStmt* getSubroutineStmt() {return pSubroutineStmt;}
    InitialSpecPart* getInitialSpecPart() {return pInitialSpecPart;}
    SpecAndExecPart* getSpecAndExecPart() {return pSpecAndExecPart;}
    InternalSubprogramPart* getInternalSubprogramPart() {return pInternalSubprogramPart;}
    EndSubroutineStmt* getEndSubroutineStmt() {return pEndSubroutineStmt;}

    void setSubroutineStmt(SubroutineStmt* subroutinestmt) {pSubroutineStmt = subroutinestmt;}
    void setInitialSpecPart(InitialSpecPart* specPart) {pInitialSpecPart = specPart;}
    void setSpecAndExecPart(SpecAndExecPart* execPart) {pSpecAndExecPart = execPart;}
    void setInternalSubprogramPart(InternalSubprogramPart* internalsubprogrampart) {pInternalSubprogramPart = internalsubprogrampart;}
    void setEndSubroutineStmt(EndSubroutineStmt* endsubroutinestmt) {pEndSubroutineStmt = endsubroutinestmt;}

 private:
    SubroutineStmt* pSubroutineStmt;
    InitialSpecPart* pInitialSpecPart;
    SpecAndExecPart* pSpecAndExecPart;
    InternalSubprogramPart* pInternalSubprogramPart;
    EndSubroutineStmt* pEndSubroutineStmt;
};

class SubroutineStmt : public Statement
{
 public:
    SubroutineStmt()
      {
         pLabel = NULL;
         pPrefix = NULL;
         pSubroutineName = NULL;
         pEOS = NULL;
         pDummyArgList = NULL;
         pProcLanguageBindingSpec = NULL;
      }
   virtual ~SubroutineStmt();

    SubroutineStmt* newSubroutineStmt()
      {
         SubroutineStmt* node = new SubroutineStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pPrefix = pPrefix;  pPrefix = NULL;
         node->pSubroutineName = pSubroutineName;  pSubroutineName = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->pDummyArgList = pDummyArgList;  pDummyArgList = NULL;
         node->pProcLanguageBindingSpec = pProcLanguageBindingSpec;  pProcLanguageBindingSpec = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Prefix* getPrefix() {return pPrefix;}
    Name* getSubroutineName() {return pSubroutineName;}
    EOS* getEOS() {return pEOS;}
    DummyArgList* getDummyArgList() {return pDummyArgList;}
    ProcLanguageBindingSpec* getProcLanguageBindingSpec() {return pProcLanguageBindingSpec;}

    void setLabel(Label* label) {pLabel = label;}
    void setPrefix(Prefix* prefix) {pPrefix = prefix;}
    void setSubroutineName(Name* subroutinename) {pSubroutineName = subroutinename;}
    void setEOS(EOS* eos) {pEOS = eos;}
    void setDummyArgList(DummyArgList* dummyarglist) {pDummyArgList = dummyarglist;}
    void setProcLanguageBindingSpec(ProcLanguageBindingSpec* proclanguagebindingspec) {pProcLanguageBindingSpec = proclanguagebindingspec;}

 private:
    Label* pLabel;
    Prefix* pPrefix;
    Name* pSubroutineName;
    EOS* pEOS;
    DummyArgList* pDummyArgList;
    ProcLanguageBindingSpec* pProcLanguageBindingSpec;
};

class DummyArg : public Node
{
 public:
    DummyArg()
      {
         pDummyArgName = NULL;
      }
   virtual ~DummyArg();

   enum OptionType
     {
        DEFAULT = 0,
        DummyArg_STAR,
        DummyArg_DAN
     };

    DummyArg* newDummyArg()
      {
         DummyArg* node = new DummyArg();
         node->pDummyArgName = pDummyArgName;  pDummyArgName = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    DummyArgName* getDummyArgName() {return pDummyArgName;}

    void setDummyArgName(DummyArgName* dummyargname) {pDummyArgName = dummyargname;}

 private:
    DummyArgName* pDummyArgName;
};

class DummyArgList : public Node
{
 public:
    DummyArgList()
      {
         pDummyArgList = new std::vector<DummyArg*>();
      }
   virtual ~DummyArgList();

    DummyArgList* newDummyArgList()
      {
         DummyArgList* node = new DummyArgList();
         delete node->pDummyArgList; node->pDummyArgList = pDummyArgList;  pDummyArgList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<DummyArg*>* getDummyArgList() {return pDummyArgList;}

    void appendDummyArg(DummyArg* dummyarg) {pDummyArgList->push_back(dummyarg);}

 private:
    std::vector<DummyArg*>* pDummyArgList;
};

class EndSubroutineStmt : public Statement
{
 public:
    EndSubroutineStmt()
      {
         pLabel = NULL;
         pSubroutineName = NULL;
         pEOS = NULL;
      }
   virtual ~EndSubroutineStmt();

    EndSubroutineStmt* newEndSubroutineStmt()
      {
         EndSubroutineStmt* node = new EndSubroutineStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pSubroutineName = pSubroutineName;  pSubroutineName = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Name* getSubroutineName() {return pSubroutineName;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setSubroutineName(Name* subroutinename) {pSubroutineName = subroutinename;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Name* pSubroutineName;
    EOS* pEOS;
};

class SeparateModuleSubprogram : public Node
{
 public:
    SeparateModuleSubprogram()
      {
         pMpSubprogramStmt = NULL;
         pInitialSpecPart = NULL;
         pSpecAndExecPart = NULL;
         pInternalSubprogramPart = NULL;
         pEndMpSubprogramStmt = NULL;
      }
   virtual ~SeparateModuleSubprogram();

    SeparateModuleSubprogram* newSeparateModuleSubprogram()
      {
         SeparateModuleSubprogram* node = new SeparateModuleSubprogram();
         node->pMpSubprogramStmt = pMpSubprogramStmt;  pMpSubprogramStmt = NULL;
         node->pInitialSpecPart = pInitialSpecPart;  pInitialSpecPart = NULL;
         node->pSpecAndExecPart = pSpecAndExecPart;  pSpecAndExecPart = NULL;
         node->pInternalSubprogramPart = pInternalSubprogramPart;  pInternalSubprogramPart = NULL;
         node->pEndMpSubprogramStmt = pEndMpSubprogramStmt;  pEndMpSubprogramStmt = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    MpSubprogramStmt* getMpSubprogramStmt() {return pMpSubprogramStmt;}
    InitialSpecPart* getInitialSpecPart() {return pInitialSpecPart;}
    SpecAndExecPart* getSpecAndExecPart() {return pSpecAndExecPart;}
    InternalSubprogramPart* getInternalSubprogramPart() {return pInternalSubprogramPart;}
    EndMpSubprogramStmt* getEndMpSubprogramStmt() {return pEndMpSubprogramStmt;}

    void setMpSubprogramStmt(MpSubprogramStmt* mpsubprogramstmt) {pMpSubprogramStmt = mpsubprogramstmt;}
    void setInitialSpecPart(InitialSpecPart* specPart) {pInitialSpecPart = specPart;}
    void setSpecAndExecPart(SpecAndExecPart* execPart) {pSpecAndExecPart = execPart;}
    void setInternalSubprogramPart(InternalSubprogramPart* internalsubprogrampart) {pInternalSubprogramPart = internalsubprogrampart;}
    void setEndMpSubprogramStmt(EndMpSubprogramStmt* endmpsubprogramstmt) {pEndMpSubprogramStmt = endmpsubprogramstmt;}

 private:
    MpSubprogramStmt* pMpSubprogramStmt;
    InitialSpecPart* pInitialSpecPart;
    SpecAndExecPart* pSpecAndExecPart;
    InternalSubprogramPart* pInternalSubprogramPart;
    EndMpSubprogramStmt* pEndMpSubprogramStmt;
};

class MpSubprogramStmt : public Statement
{
 public:
    MpSubprogramStmt()
      {
         pLabel = NULL;
         pProcedureName = NULL;
         pEOS = NULL;
      }
   virtual ~MpSubprogramStmt();

    MpSubprogramStmt* newMpSubprogramStmt()
      {
         MpSubprogramStmt* node = new MpSubprogramStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pProcedureName = pProcedureName;  pProcedureName = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Name* getProcedureName() {return pProcedureName;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setProcedureName(Name* procedurename) {pProcedureName = procedurename;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Name* pProcedureName;
    EOS* pEOS;
};

class EndMpSubprogramStmt : public Statement
{
 public:
    EndMpSubprogramStmt()
      {
         pLabel = NULL;
         pProcedureName = NULL;
         pEOS = NULL;
      }
   virtual ~EndMpSubprogramStmt();

    EndMpSubprogramStmt* newEndMpSubprogramStmt()
      {
         EndMpSubprogramStmt* node = new EndMpSubprogramStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pProcedureName = pProcedureName;  pProcedureName = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Name* getProcedureName() {return pProcedureName;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setProcedureName(Name* procedurename) {pProcedureName = procedurename;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Name* pProcedureName;
    EOS* pEOS;
};

class EntryStmt : public Statement
{
 public:
    EntryStmt()
      {
         pLabel = NULL;
         pEntryName = NULL;
         pEOS = NULL;
         pDummyArgList = NULL;
         pSuffix = NULL;
      }
   virtual ~EntryStmt();

   enum OptionType
     {
        DEFAULT = 0,
        EntryStmt_0,
        EntryStmt_DAL
     };

    EntryStmt* newEntryStmt()
      {
         EntryStmt* node = new EntryStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pEntryName = pEntryName;  pEntryName = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->pDummyArgList = pDummyArgList;  pDummyArgList = NULL;
         node->pSuffix = pSuffix;  pSuffix = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Name* getEntryName() {return pEntryName;}
    EOS* getEOS() {return pEOS;}
    DummyArgList* getDummyArgList() {return pDummyArgList;}
    Suffix* getSuffix() {return pSuffix;}

    void setLabel(Label* label) {pLabel = label;}
    void setEntryName(Name* entryname) {pEntryName = entryname;}
    void setEOS(EOS* eos) {pEOS = eos;}
    void setDummyArgList(DummyArgList* dummyarglist) {pDummyArgList = dummyarglist;}
    void setSuffix(Suffix* suffix) {pSuffix = suffix;}

 private:
    Label* pLabel;
    Name* pEntryName;
    EOS* pEOS;
    DummyArgList* pDummyArgList;
    Suffix* pSuffix;
};

class ReturnStmt : public Statement
{
 public:
    ReturnStmt()
      {
         pLabel = NULL;
         pExpr = NULL;
         pEOS = NULL;
      }
   virtual ~ReturnStmt();

    ReturnStmt* newReturnStmt()
      {
         ReturnStmt* node = new ReturnStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pExpr = pExpr;  pExpr = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Expr* getExpr() {return pExpr;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setExpr(Expr* expr) {pExpr = expr;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Expr* pExpr;
    EOS* pEOS;
};

class ContainsStmt : public Statement
{
 public:
    ContainsStmt()
      {
         pLabel = NULL;
         pEOS = NULL;
      }
   virtual ~ContainsStmt();

    ContainsStmt* newContainsStmt()
      {
         ContainsStmt* node = new ContainsStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    EOS* pEOS;
};

class StmtFunctionStmt : public Statement
{
 public:
    StmtFunctionStmt()
      {
         pLabel = NULL;
         pFunctionName = NULL;
         pDummyArgNameList = NULL;
         pExpr = NULL;
         pEOS = NULL;
      }
   virtual ~StmtFunctionStmt();

    StmtFunctionStmt* newStmtFunctionStmt()
      {
         StmtFunctionStmt* node = new StmtFunctionStmt();
         node->pLabel = pLabel;  pLabel = NULL;
         node->pFunctionName = pFunctionName;  pFunctionName = NULL;
         node->pDummyArgNameList = pDummyArgNameList;  pDummyArgNameList = NULL;
         node->pExpr = pExpr;  pExpr = NULL;
         node->pEOS = pEOS;  pEOS = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Label* getLabel() {return pLabel;}
    Name* getFunctionName() {return pFunctionName;}
    DummyArgNameList* getDummyArgNameList() {return pDummyArgNameList;}
    Expr* getExpr() {return pExpr;}
    EOS* getEOS() {return pEOS;}

    void setLabel(Label* label) {pLabel = label;}
    void setFunctionName(Name* functionname) {pFunctionName = functionname;}
    void setDummyArgNameList(DummyArgNameList* dummyargnamelist) {pDummyArgNameList = dummyargnamelist;}
    void setExpr(Expr* expr) {pExpr = expr;}
    void setEOS(EOS* eos) {pEOS = eos;}

 private:
    Label* pLabel;
    Name* pFunctionName;
    DummyArgNameList* pDummyArgNameList;
    Expr* pExpr;
    EOS* pEOS;
};

class Name : public Node
{
 public:
    Name()
      {
         pIdent = NULL;
      }
   virtual ~Name();

    Name* newName()
      {
         Name* node = new Name();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

#ifdef OBSOLETE
class AncestorModuleName : public Node
{
 public:
    AncestorModuleName()
      {
         pIdent = NULL;
      }
   virtual ~AncestorModuleName();

    Name* newAncestorModuleName()
      {
         Name* node = new AncestorModuleName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class ArgName : public Node
{
 public:
    ArgName()
      {
         pIdent = NULL;
      }
   virtual ~ArgName();

    Name* newArgName()
      {
         Name* node = new ArgName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class ArrayName : public Node
{
 public:
    ArrayName()
      {
         pIdent = NULL;
      }
   virtual ~ArrayName();

    Name* newArrayName()
      {
         Name* node = new ArrayName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class AssociateConstructName : public Node
{
 public:
    AssociateConstructName()
      {
         pIdent = NULL;
      }
   virtual ~AssociateConstructName();

    Name* newAssociateConstructName()
      {
         Name* node = new AssociateConstructName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class AssociateName : public Node
{
 public:
    AssociateName()
      {
         pIdent = NULL;
      }
   virtual ~AssociateName();

    Name* newAssociateName()
      {
         Name* node = new AssociateName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class BindingName : public Node
{
 public:
    BindingName()
      {
         pIdent = NULL;
      }
   virtual ~BindingName();

    Name* newBindingName()
      {
         Name* node = new BindingName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class BlockConstructName : public Node
{
 public:
    BlockConstructName()
      {
         pIdent = NULL;
      }
   virtual ~BlockConstructName();

    Name* newBlockConstructName()
      {
         Name* node = new BlockConstructName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class BlockDataName : public Node
{
 public:
    BlockDataName()
      {
         pIdent = NULL;
      }
   virtual ~BlockDataName();

    Name* newBlockDataName()
      {
         Name* node = new BlockDataName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class CaseConstructName : public Node
{
 public:
    CaseConstructName()
      {
         pIdent = NULL;
      }
   virtual ~CaseConstructName();

    Name* newCaseConstructName()
      {
         Name* node = new CaseConstructName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class CoarrayName : public Node
{
 public:
    CoarrayName()
      {
         pIdent = NULL;
      }
   virtual ~CoarrayName();

    Name* newCoarrayName()
      {
         Name* node = new CoarrayName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class CommonBlockName : public Node
{
 public:
    CommonBlockName()
      {
         pIdent = NULL;
      }
   virtual ~CommonBlockName();

    Name* newCommonBlockName()
      {
         Name* node = new CommonBlockName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class ComponentName : public Node
{
 public:
    ComponentName()
      {
         pIdent = NULL;
      }
   virtual ~ComponentName();

    Name* newComponentName()
      {
         Name* node = new ComponentName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class ConstructName : public Node
{
 public:
    ConstructName()
      {
         pIdent = NULL;
      }
   virtual ~ConstructName();

    Name* newConstructName()
      {
         Name* node = new ConstructName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class CriticalConstructName : public Node
{
 public:
    CriticalConstructName()
      {
         pIdent = NULL;
      }
   virtual ~CriticalConstructName();

    Name* newCriticalConstructName()
      {
         Name* node = new CriticalConstructName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class DataPointerComponentName : public Node
{
 public:
    DataPointerComponentName()
      {
         pIdent = NULL;
      }
   virtual ~DataPointerComponentName();

    DataPointerComponentName* newDataPointerComponentName()
      {
         DataPointerComponentName* node = new DataPointerComponentName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class DoConstructName : public Node
{
 public:
    DoConstructName()
      {
         pIdent = NULL;
      }
   virtual ~DoConstructName();

    Name* newDoConstructName()
      {
         Name* node = new DoConstructName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class EntityName : public Node
{
 public:
    EntityName()
      {
         pIdent = NULL;
      }
   virtual ~EntityName();

    Name* newEntityName()
      {
         Name* node = new EntityName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class EntryName : public Node
{
 public:
    EntryName()
      {
         pIdent = NULL;
      }
   virtual ~EntryName();

    Name* newEntryName()
      {
         Name* node = new EntryName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class ExternalName : public Node
{
 public:
    ExternalName()
      {
         pIdent = NULL;
      }
   virtual ~ExternalName();

    ExternalName* newExternalName()
      {
         ExternalName* node = new ExternalName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class FinalSubroutineName : public Node
{
 public:
    FinalSubroutineName()
      {
         pIdent = NULL;
      }
   virtual ~FinalSubroutineName();

    Name* newFinalSubroutineName()
      {
         Name* node = new FinalSubroutineName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class ForallConstructName : public Node
{
 public:
    ForallConstructName()
      {
         pIdent = NULL;
      }
   virtual ~ForallConstructName();

    Name* newForallConstructName()
      {
         Name* node = new ForallConstructName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class FunctionName : public Node
{
 public:
    FunctionName()
      {
         pIdent = NULL;
      }
   virtual ~FunctionName();

    Name* newFunctionName()
      {
         Name* node = new FunctionName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class GenericName : public Node
{
 public:
    GenericName()
      {
         pIdent = NULL;
      }
   virtual ~GenericName();

    Name* newGenericName()
      {
         Name* node = new GenericName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class IfConstructName : public Node
{
 public:
    IfConstructName()
      {
         pIdent = NULL;
      }
   virtual ~IfConstructName();

    Name* newIfConstructName()
      {
         Name* node = new IfConstructName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class ImportName : public Node
{
 public:
    ImportName()
      {
         pIdent = NULL;
      }
   virtual ~ImportName();

    Name* newImportName()
      {
         Name* node = new ImportName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class IndexName : public Node
{
 public:
    IndexName()
      {
         pIdent = NULL;
      }
   virtual ~IndexName();

    Name* newIndexName()
      {
         Name* node = new IndexName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class IntrinsicProcedureName : public Node
{
 public:
    IntrinsicProcedureName()
      {
         pIdent = NULL;
      }
   virtual ~IntrinsicProcedureName();

    Name* newIntrinsicProcedureName()
      {
         Name* node = new IntrinsicProcedureName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class LocalName : public Node
{
 public:
    LocalName()
      {
         pIdent = NULL;
      }
   virtual ~LocalName();

    Name* newLocalName()
      {
         Name* node = new LocalName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class ModuleName : public Node
{
 public:
    ModuleName()
      {
         pIdent = NULL;
      }
   virtual ~ModuleName();

    Name* newModuleName()
      {
         Name* node = new ModuleName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class NamelistGroupName : public Node
{
 public:
    NamelistGroupName()
      {
         pIdent = NULL;
      }
   virtual ~NamelistGroupName();

    Name* newNamelistGroupName()
      {
         Name* node = new NamelistGroupName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class ObjectName : public Node
{
 public:
    ObjectName()
      {
         pIdent = NULL;
      }
   virtual ~ObjectName();

    Name* newObjectName()
      {
         Name* node = new ObjectName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class ParentSubmoduleName : public Node
{
 public:
    ParentSubmoduleName()
      {
         pIdent = NULL;
      }
   virtual ~ParentSubmoduleName();

    Name* newParentSubmoduleName()
      {
         Name* node = new ParentSubmoduleName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class ParentTypeName : public Node
{
 public:
    ParentTypeName()
      {
         pIdent = NULL;
      }
   virtual ~ParentTypeName();

    ParentTypeName* newParentTypeName()
      {
         ParentTypeName* node = new ParentTypeName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class PartName : public Node
{
 public:
    PartName()
      {
         pIdent = NULL;
      }
   virtual ~PartName();

    PartName* newPartName()
      {
         PartName* node = new PartName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class ProcedureComponentName : public Node
{
 public:
    ProcedureComponentName()
      {
         pIdent = NULL;
      }
   virtual ~ProcedureComponentName();

    Name* newProcedureComponentName()
      {
         Name* node = new ProcedureComponentName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class ProcedureEntityName : public Node
{
 public:
    ProcedureEntityName()
      {
         pIdent = NULL;
      }
   virtual ~ProcedureEntityName();

    Name* newProcedureEntityName()
      {
         Name* node = new ProcedureEntityName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class ProcedureName : public Node
{
 public:
    ProcedureName()
      {
         pIdent = NULL;
      }
   virtual ~ProcedureName();

    Name* newProcedureName()
      {
         Name* node = new ProcedureName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class ProcEntityName : public Node
{
 public:
    ProcEntityName()
      {
         pIdent = NULL;
      }
   virtual ~ProcEntityName();

    ProcEntityName* newProcEntityName()
      {
         ProcEntityName* node = new ProcEntityName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class ResultName : public Node
{
 public:
    ResultName()
      {
         pIdent = NULL;
      }
   virtual ~ResultName();

    Name* newResultName()
      {
         Name* node = new ResultName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class ScalarIntConstantName : public Node
{
 public:
    ScalarIntConstantName()
      {
         pIdent = NULL;
      }
   virtual ~ScalarIntConstantName();

    ScalarIntConstantName* newScalarIntConstantName()
      {
         ScalarIntConstantName* node = new ScalarIntConstantName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class ScalarIntVariableName : public Node
{
 public:
    ScalarIntVariableName()
      {
         pIdent = NULL;
      }
   virtual ~ScalarIntVariableName();

    Name* newScalarIntVariableName()
      {
         Name* node = new ScalarIntVariableName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class ScalarVariableName : public Node
{
 public:
    ScalarVariableName()
      {
         pIdent = NULL;
      }
   virtual ~ScalarVariableName();

    Name* newScalarVariableName()
      {
         Name* node = new ScalarVariableName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class SelectConstructName : public Node
{
 public:
    SelectConstructName()
      {
         pIdent = NULL;
      }
   virtual ~SelectConstructName();

    Name* newSelectConstructName()
      {
         Name* node = new SelectConstructName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class SubmoduleName : public Node
{
 public:
    SubmoduleName()
      {
         pIdent = NULL;
      }
   virtual ~SubmoduleName();

    Name* newSubmoduleName()
      {
         Name* node = new SubmoduleName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class SubroutineName : public Node
{
 public:
    SubroutineName()
      {
         pIdent = NULL;
      }
   virtual ~SubroutineName();

    SubroutineName* newSubroutineName()
      {
         SubroutineName* node = new SubroutineName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class TypeName : public Node
{
 public:
    TypeName()
      {
         pIdent = NULL;
      }
   virtual ~TypeName();

    TypeName* newTypeName()
      {
         TypeName* node = new TypeName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class TypeParamName : public Node
{
 public:
    TypeParamName()
      {
         pIdent = NULL;
      }
   virtual ~TypeParamName();

    Name* newTypeParamName()
      {
         Name* node = new Name();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};

class UseName : public Node
{
 public:
    UseName()
      {
         pIdent = NULL;
      }
   virtual ~UseName();

    Name* newUseName()
      {
         Name* node = new UseName();
         node->pIdent = pIdent;  pIdent = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    Ident* getIdent() {return pIdent;}

    void setIdent(Ident* ident) {pIdent = ident;}

 private:
    Ident* pIdent;
};
#endif

class ExternalNameList : public Node
{
 public:
    ExternalNameList()
      {
         pExternalNameList = new std::vector<Name*>();
      }
   virtual ~ExternalNameList();

    ExternalNameList* newExternalNameList()
      {
         ExternalNameList* node = new ExternalNameList();
         delete node->pExternalNameList; node->pExternalNameList = pExternalNameList;  pExternalNameList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<Name*>* getExternalNameList() {return pExternalNameList;}

    void appendExternalName(Name* externalname) {pExternalNameList->push_back(externalname);}

 private:
    std::vector<Name*>* pExternalNameList;
};

class LabelList : public Node
{
 public:
    LabelList()
      {
         pLblRefList = new std::vector<LblRef*>();
      }
   virtual ~LabelList();

    LabelList* newLabelList()
      {
         LabelList* node = new LabelList();
         delete node->pLblRefList; node->pLblRefList = pLblRefList;  pLblRefList = NULL;
         node->setOptionType(optionType);
         node->inheritPayload(this);
         return node;
      }

    std::vector<LblRef*>* getLblRefList() {return pLblRefList;}

    void appendLblRef(LblRef* lblref) {pLblRefList->push_back(lblref);}

 private:
    std::vector<LblRef*>* pLblRefList;
};


} // namespace OFP

#endif
