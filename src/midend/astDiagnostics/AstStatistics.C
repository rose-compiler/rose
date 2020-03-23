


// DQ (3/6/2003): added from AstProcessing.h to avoid referencing
// the traversal classes in AstFixes.h before they are defined.
#include "sage3basic.h"
#include "roseInternal.h"

#include <sstream>
#include "AstStatistics.h"

using namespace std;

// avoid qualification of different supporting classes
using namespace ROSE_Statistics;

// ************************************************************************
//                AstNodeTraversalStatistics member functions
// ************************************************************************

//  NodeStatistics Constructors/Destructors
AstNodeTraversalStatistics::AstNodeTraversalStatistics() 
{
  numNodeTypes.resize(V_SgNumVariants);
}

AstNodeTraversalStatistics::~AstNodeTraversalStatistics()
{
}

AstNodeTraversalCSVStatistics::AstNodeTraversalCSVStatistics():minCountToShow(1) {
}

void AstNodeTraversalCSVStatistics::setMinCountToShow(int minValue) {
  minCountToShow=minValue;
}

unsigned int AstNodeTraversalCSVStatistics::getMinCountToShow() {
  return minCountToShow;
}

std::string AstNodeTraversalCSVStatistics::toString(SgNode* node) {
  std::stringstream ss;
  traverse(node,preorder); // traverse input files and all included files
  for(size_t i=0;i!=numNodeTypes.size();++i) {
    if(numNodeTypes.at(i)>=(size_t)getMinCountToShow())
      ss<<getVariantName(VariantT(i)) <<","<<numNodeTypes[i]<<endl;
  }
  return ss.str();
}
 
string
AstNodeTraversalStatistics::toString(SgNode* node) {
  string s;
  traverse(node,preorder); // traverse input files and all included files
  SgProject* n=dynamic_cast<SgProject*>(node);
  if (n != NULL) {
    AstNodeTraversalStatistics stat2;
    stat2.traverseInputFiles(n,preorder); // only traverse .C files specified on the command line
    s = stat2.cmpStatistics(*this); // comparison of the size of the AST including and excluding header files
  } else {
    s = singleStatistics();
  }
  return s;
}

void 
AstNodeTraversalStatistics::visit(SgNode* node) { 
  if(!node) return;
  ROSE_ASSERT(node->variantT()<V_SgNumVariants);
  numNodeTypes.at(node->variantT())++;
}

AstNodeTraversalStatistics::StatisticsContainerType
AstNodeTraversalStatistics::getStatisticsData() {
  return numNodeTypes;
}

string
AstNodeTraversalStatistics::singleStatistics() {
  // we possibly will want to overload << 
  ostringstream ss;
  for (size_t i=0; i != numNodeTypes.size(); i++) {
    if (numNodeTypes.at(i) > 0) {
      if ( SgProject::get_verbose() >= 2 )
        ss << "AST Traversal Statistics: " << setw(8) << numNodeTypes[i] << ": " << getVariantName(VariantT(i)) << endl;
    }
  }
  return ss.str();
}

string
AstNodeTraversalStatistics::cmpStatistics( AstNodeTraversalStatistics & q ) {
  ostringstream ss;
  StatisticsContainerType numNodeTypes2 = q.getStatisticsData();
  ElementType sum1 = 0, sum2 = 0;

  ss << "********************************************************************************************************************\n";
  ss << "AST Traversal Statistics (traversed in current file) : (total in AST) : (percent of total traversed in current file)\n";
  ss << "********************************************************************************************************************\n";

  for(unsigned int i=0; i != numNodeTypes.size(); i++) {
    if (numNodeTypes.at(i)>0 || numNodeTypes2.at(i)>0) {
      ss << generateCMPStatisticsValueString(getVariantName(VariantT(i)), numNodeTypes[i], numNodeTypes2[i]);
    }
    sum1 += numNodeTypes[i]; 
    sum2 += numNodeTypes2[i];
  }
  
  ss << generateCMPStatisticsValueString("TOTAL", sum1, sum2);
  ss << "********************************************************************************************************************\n";

  return ss.str();
}

// if the higher values are input data % is < 100.
string 
AstNodeTraversalStatistics::generateCMPStatisticsValueString(string name, ElementType v1, ElementType v2) {
  ostringstream ss;
  ss << "AST Traversal Statistics:";
  ss << setw(6) << v1 << ":" << setw(6) << v2 << ":";
  if (v2 > 0) {
    ss.setf(ios::fixed|ios::showpoint);
    ss << " " << setprecision(1) << setw(5) << (static_cast<float>(v1)/static_cast<float>(v2))*100.0 << "%";
  } else {
    ss << "-N/A-";
  }
  ss << " " << name;
  ss << endl;

  return ss.str();
}

// ************************************************************************
//                    AstNodeMemoryPoolStatistics member functions
// ************************************************************************



AstNodeMemoryPoolStatistics::AstNodeMemoryPoolStatistics()
   {
  // Initialize the total amount of memory used so that we can report fractional percentage of use per IR node.
     totalMemoryUsed = memoryUsage();
     printf ("Total memory used = %d \n",totalMemoryUsed);
     printf ("numberOfNodes = %" PRIuPTR " \n",numberOfNodes());

  // DQ (5/6/2011): Insure++ reports this as an error in the tests/nonsmoke/functional/RunTests/AstDeleteTests
     counter = 0;
   }

AstNodeMemoryPoolStatistics::~AstNodeMemoryPoolStatistics()
   {
   }

AstNodeMemoryPoolStatistics::ElementType::ElementType(std::string name, int count, int memoryRequirement)
   : name(name), count(count), memoryRequirement(memoryRequirement)
   {
   }

bool
AstNodeMemoryPoolStatistics::ElementType::operator<(const ElementType & x)
   {
     return (memoryRequirement < x.memoryRequirement) ? true : false;
   }

// CPP Macro to implement case for each IR node (we might alternatively
// use a visitor pattern and a function template, maybe?)
#define IR_NODE_VISIT_CASE(X) \
          case V_##X: \
             { \
               X* castNode = is##X(node); \
               ROSE_ASSERT(castNode != NULL); \
               int numberOfNodes   = castNode->numberOfNodes(); \
               int memoryFootprint = castNode->memoryUsage(); \
               double percent = (((double) memoryFootprint) / ((double) totalMemoryUsed)) * 100.0; \
               if ( SgProject::get_verbose() >= 2 ) \
                    printf ("AST Memory Pool Statistics: numberOfNodes = %9d memory consumption = %10d bytes (%6.3f percent of total) sizeof() = %4ld node = %s \n",numberOfNodes,memoryFootprint,percent,sizeof(*castNode),castNode->class_name().c_str());\
               break; \
             }

void AstNodeMemoryPoolStatistics::visit ( SgNode* node)
   {
  // This function reports the number of IR nodes used in the AST (exact numbers are 
  // computed from the memory pools used to allocat IR nodes). These number are 
  // combined with the size of each IR nodes (using sizeof() for each IR nodes type)
  // and the memory consumption for each IR nodes is generated.  This function provides
  // data which supports the attempts to reduce the size of the AST for large applications
  // (particularly for whole program analysis).

  // This is part of a traversal in ROSE which traverses a representative of each type 
  // of IR node instead of all IR nodes.  This allows for construction of simple tools
  // (like this one) which only call static functions on the IR nodes.

     counter++;

  // Using a classic visitor pattern should avoid all this casting,
  // but each function must be created separately (so it is wash if 
  // we want to do all IR nodes, as we do here).
     switch(node->variantT())
        {
#if 0
       // Example of expanded macro
          case V_Sg_File_Info:
             {
               Sg_File_Info* fileInfo = isSg_File_Info(node);
               int numberOfNodes   = fileInfo->numberOfNodes();
               int memoryFootprint = fileInfo->memoryUsage();
               printf ("AstNodeMemoryPoolStatistics::visit: node = %s numberOfNodes = %d memoryFootprint = %d \n",node->class_name().c_str(),numberOfNodes,memoryFootprint); 
               break;
             }
#endif

       // DQ (9/1/2009): Updated this list with a few new IR nodes, but there are many that are missing
       // (binary analysis specific, instructions, and binary format, dwarf, UPC, OpenMP, etc.).

       // DQ (2/13/2006): these are generated by ROSETTA and copied here
          IR_NODE_VISIT_CASE(SgModifier)
          IR_NODE_VISIT_CASE(SgBitAttribute)
          IR_NODE_VISIT_CASE(SgAttribute)
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
          IR_NODE_VISIT_CASE(SgBinaryComposite)
#endif
          IR_NODE_VISIT_CASE(SgSupport)
       // IR_NODE_VISIT_CASE(SgPartialFunctionType)
          IR_NODE_VISIT_CASE(SgMemberFunctionType)
          IR_NODE_VISIT_CASE(SgFunctionType)
          IR_NODE_VISIT_CASE(SgPointerType)
          IR_NODE_VISIT_CASE(SgNamedType)
          IR_NODE_VISIT_CASE(SgType)
          IR_NODE_VISIT_CASE(SgInitializer)
          IR_NODE_VISIT_CASE(SgUnaryOp)
          IR_NODE_VISIT_CASE(SgBinaryOp)
          IR_NODE_VISIT_CASE(SgValueExp)
          IR_NODE_VISIT_CASE(SgExpression)
          IR_NODE_VISIT_CASE(SgClassDeclaration)
          IR_NODE_VISIT_CASE(SgClassDefinition)
          IR_NODE_VISIT_CASE(SgScopeStatement)
          IR_NODE_VISIT_CASE(SgMemberFunctionDeclaration)
          IR_NODE_VISIT_CASE(SgFunctionDeclaration)
          IR_NODE_VISIT_CASE(SgDeclarationStatement)
          IR_NODE_VISIT_CASE(SgStatement)
          IR_NODE_VISIT_CASE(SgFunctionSymbol)
          IR_NODE_VISIT_CASE(SgSymbol)
          IR_NODE_VISIT_CASE(SgLocatedNode)
          IR_NODE_VISIT_CASE(SgNode)
          IR_NODE_VISIT_CASE(SgSymbolTable)
          IR_NODE_VISIT_CASE(SgInitializedName)
          IR_NODE_VISIT_CASE(SgName)
          IR_NODE_VISIT_CASE(SgPragma)
          IR_NODE_VISIT_CASE(SgModifierNodes)
          IR_NODE_VISIT_CASE(SgConstVolatileModifier)
          IR_NODE_VISIT_CASE(SgStorageModifier)
          IR_NODE_VISIT_CASE(SgAccessModifier)
          IR_NODE_VISIT_CASE(SgFunctionModifier)
          IR_NODE_VISIT_CASE(SgUPC_AccessModifier)
          IR_NODE_VISIT_CASE(SgLinkageModifier)
          IR_NODE_VISIT_CASE(SgSpecialFunctionModifier)
          IR_NODE_VISIT_CASE(SgTypeModifier)
          IR_NODE_VISIT_CASE(SgElaboratedTypeModifier)
          IR_NODE_VISIT_CASE(SgBaseClassModifier)
          IR_NODE_VISIT_CASE(SgDeclarationModifier)
          IR_NODE_VISIT_CASE(Sg_File_Info)
          IR_NODE_VISIT_CASE(SgFile)
          IR_NODE_VISIT_CASE(SgFileList)
          IR_NODE_VISIT_CASE(SgDirectory)
          IR_NODE_VISIT_CASE(SgDirectoryList)
          IR_NODE_VISIT_CASE(SgProject)
          IR_NODE_VISIT_CASE(SgOptions)
          IR_NODE_VISIT_CASE(SgUnparse_Info)
          IR_NODE_VISIT_CASE(SgFuncDecl_attr)
          IR_NODE_VISIT_CASE(SgClassDecl_attr)
          IR_NODE_VISIT_CASE(SgTypedefSeq)
          IR_NODE_VISIT_CASE(SgFunctionParameterTypeList)
          IR_NODE_VISIT_CASE(SgTemplateParameter)
          IR_NODE_VISIT_CASE(SgTemplateArgument)
          IR_NODE_VISIT_CASE(SgBaseClass)
          IR_NODE_VISIT_CASE(SgQualifiedName)
          IR_NODE_VISIT_CASE(SgSourceFile)
          IR_NODE_VISIT_CASE(SgTypeUnknown)
          IR_NODE_VISIT_CASE(SgTypeChar)
          IR_NODE_VISIT_CASE(SgTypeSignedChar)
          IR_NODE_VISIT_CASE(SgTypeUnsignedChar)
          IR_NODE_VISIT_CASE(SgTypeShort)
          IR_NODE_VISIT_CASE(SgTypeSignedShort)
          IR_NODE_VISIT_CASE(SgTypeUnsignedShort)
          IR_NODE_VISIT_CASE(SgTypeInt)
          IR_NODE_VISIT_CASE(SgTypeSignedInt)
          IR_NODE_VISIT_CASE(SgTypeUnsignedInt)
          IR_NODE_VISIT_CASE(SgTypeLong)
          IR_NODE_VISIT_CASE(SgTypeSignedLong)
          IR_NODE_VISIT_CASE(SgTypeUnsignedLong)
          IR_NODE_VISIT_CASE(SgTypeVoid)
          IR_NODE_VISIT_CASE(SgTypeGlobalVoid)
          IR_NODE_VISIT_CASE(SgTypeWchar)
          IR_NODE_VISIT_CASE(SgTypeFloat)
          IR_NODE_VISIT_CASE(SgTypeDouble)
          IR_NODE_VISIT_CASE(SgTypeLongLong)
          IR_NODE_VISIT_CASE(SgTypeUnsignedLongLong)
          IR_NODE_VISIT_CASE(SgTypeLongDouble)
          IR_NODE_VISIT_CASE(SgTypeString)
          IR_NODE_VISIT_CASE(SgTypeBool)
          IR_NODE_VISIT_CASE(SgTypeComplex)
          IR_NODE_VISIT_CASE(SgTypeImaginary)
          IR_NODE_VISIT_CASE(SgTypeDefault)
          IR_NODE_VISIT_CASE(SgPointerMemberType)
          IR_NODE_VISIT_CASE(SgReferenceType)
          IR_NODE_VISIT_CASE(SgClassType)
          IR_NODE_VISIT_CASE(SgTemplateType)
          IR_NODE_VISIT_CASE(SgEnumType)
          IR_NODE_VISIT_CASE(SgTypedefType)
          IR_NODE_VISIT_CASE(SgModifierType)
          IR_NODE_VISIT_CASE(SgPartialFunctionModifierType)
          IR_NODE_VISIT_CASE(SgArrayType)
          IR_NODE_VISIT_CASE(SgTypeEllipse)
          IR_NODE_VISIT_CASE(SgQualifiedNameType)
          IR_NODE_VISIT_CASE(SgExprListExp)
          IR_NODE_VISIT_CASE(SgVarRefExp)
          IR_NODE_VISIT_CASE(SgClassNameRefExp)
          IR_NODE_VISIT_CASE(SgFunctionRefExp)
          IR_NODE_VISIT_CASE(SgMemberFunctionRefExp)
          IR_NODE_VISIT_CASE(SgFunctionCallExp)
          IR_NODE_VISIT_CASE(SgSizeOfOp)
          IR_NODE_VISIT_CASE(SgVarArgStartOp)
          IR_NODE_VISIT_CASE(SgVarArgStartOneOperandOp)
          IR_NODE_VISIT_CASE(SgVarArgOp)
          IR_NODE_VISIT_CASE(SgVarArgEndOp)
          IR_NODE_VISIT_CASE(SgVarArgCopyOp)
          IR_NODE_VISIT_CASE(SgTypeIdOp)
          IR_NODE_VISIT_CASE(SgConditionalExp)
          IR_NODE_VISIT_CASE(SgNewExp)
          IR_NODE_VISIT_CASE(SgDeleteExp)
          IR_NODE_VISIT_CASE(SgThisExp)
          IR_NODE_VISIT_CASE(SgRefExp)
          IR_NODE_VISIT_CASE(SgAggregateInitializer)
          IR_NODE_VISIT_CASE(SgConstructorInitializer)
          IR_NODE_VISIT_CASE(SgAssignInitializer)
          IR_NODE_VISIT_CASE(SgExpressionRoot)
          IR_NODE_VISIT_CASE(SgMinusOp)
          IR_NODE_VISIT_CASE(SgUnaryAddOp)
          IR_NODE_VISIT_CASE(SgNotOp)
          IR_NODE_VISIT_CASE(SgPointerDerefExp)
          IR_NODE_VISIT_CASE(SgAddressOfOp)
          IR_NODE_VISIT_CASE(SgMinusMinusOp)
          IR_NODE_VISIT_CASE(SgPlusPlusOp)
          IR_NODE_VISIT_CASE(SgBitComplementOp)
          IR_NODE_VISIT_CASE(SgCastExp)
          IR_NODE_VISIT_CASE(SgThrowOp)
          IR_NODE_VISIT_CASE(SgArrowExp)
          IR_NODE_VISIT_CASE(SgDotExp)
          IR_NODE_VISIT_CASE(SgDotStarOp)
          IR_NODE_VISIT_CASE(SgArrowStarOp)
          IR_NODE_VISIT_CASE(SgEqualityOp)
          IR_NODE_VISIT_CASE(SgLessThanOp)
          IR_NODE_VISIT_CASE(SgGreaterThanOp)
          IR_NODE_VISIT_CASE(SgNotEqualOp)
          IR_NODE_VISIT_CASE(SgLessOrEqualOp)
          IR_NODE_VISIT_CASE(SgGreaterOrEqualOp)
          IR_NODE_VISIT_CASE(SgAddOp)
          IR_NODE_VISIT_CASE(SgSubtractOp)
          IR_NODE_VISIT_CASE(SgMultiplyOp)
          IR_NODE_VISIT_CASE(SgDivideOp)
          IR_NODE_VISIT_CASE(SgIntegerDivideOp)
          IR_NODE_VISIT_CASE(SgModOp)
          IR_NODE_VISIT_CASE(SgAndOp)
          IR_NODE_VISIT_CASE(SgOrOp)
          IR_NODE_VISIT_CASE(SgBitXorOp)
          IR_NODE_VISIT_CASE(SgBitAndOp)
          IR_NODE_VISIT_CASE(SgBitOrOp)
          IR_NODE_VISIT_CASE(SgCommaOpExp)
          IR_NODE_VISIT_CASE(SgLshiftOp)
          IR_NODE_VISIT_CASE(SgRshiftOp)
          IR_NODE_VISIT_CASE(SgPntrArrRefExp)
          IR_NODE_VISIT_CASE(SgScopeOp)
          IR_NODE_VISIT_CASE(SgAssignOp)
          IR_NODE_VISIT_CASE(SgPlusAssignOp)
          IR_NODE_VISIT_CASE(SgMinusAssignOp)
          IR_NODE_VISIT_CASE(SgAndAssignOp)
          IR_NODE_VISIT_CASE(SgIorAssignOp)
          IR_NODE_VISIT_CASE(SgMultAssignOp)
          IR_NODE_VISIT_CASE(SgDivAssignOp)
          IR_NODE_VISIT_CASE(SgModAssignOp)
          IR_NODE_VISIT_CASE(SgXorAssignOp)
          IR_NODE_VISIT_CASE(SgLshiftAssignOp)
          IR_NODE_VISIT_CASE(SgRshiftAssignOp)
          IR_NODE_VISIT_CASE(SgAsmOp)
          IR_NODE_VISIT_CASE(SgBoolValExp)
          IR_NODE_VISIT_CASE(SgStringVal)
          IR_NODE_VISIT_CASE(SgShortVal)
          IR_NODE_VISIT_CASE(SgCharVal)
          IR_NODE_VISIT_CASE(SgUnsignedCharVal)
          IR_NODE_VISIT_CASE(SgWcharVal)
          IR_NODE_VISIT_CASE(SgUnsignedShortVal)
          IR_NODE_VISIT_CASE(SgIntVal)
          IR_NODE_VISIT_CASE(SgEnumVal)
          IR_NODE_VISIT_CASE(SgUnsignedIntVal)
          IR_NODE_VISIT_CASE(SgLongIntVal)
          IR_NODE_VISIT_CASE(SgLongLongIntVal)
          IR_NODE_VISIT_CASE(SgUnsignedLongLongIntVal)
          IR_NODE_VISIT_CASE(SgUnsignedLongVal)
          IR_NODE_VISIT_CASE(SgFloatVal)
          IR_NODE_VISIT_CASE(SgDoubleVal)
          IR_NODE_VISIT_CASE(SgLongDoubleVal)
          IR_NODE_VISIT_CASE(SgNullExpression)
          IR_NODE_VISIT_CASE(SgVariantExpression)
          IR_NODE_VISIT_CASE(SgForStatement)
          IR_NODE_VISIT_CASE(SgForInitStatement)
          IR_NODE_VISIT_CASE(SgCatchStatementSeq)
          IR_NODE_VISIT_CASE(SgFunctionParameterList)
          IR_NODE_VISIT_CASE(SgCtorInitializerList)
          IR_NODE_VISIT_CASE(SgBasicBlock)
          IR_NODE_VISIT_CASE(SgGlobal)
          IR_NODE_VISIT_CASE(SgIfStmt)
          IR_NODE_VISIT_CASE(SgFunctionDefinition)
          IR_NODE_VISIT_CASE(SgWhileStmt)
          IR_NODE_VISIT_CASE(SgDoWhileStmt)
          IR_NODE_VISIT_CASE(SgSwitchStatement)
          IR_NODE_VISIT_CASE(SgCatchOptionStmt)
          IR_NODE_VISIT_CASE(SgVariableDeclaration)
          IR_NODE_VISIT_CASE(SgVariableDefinition)
          IR_NODE_VISIT_CASE(SgEnumDeclaration)
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
          IR_NODE_VISIT_CASE(SgAsmStmt)
#endif
          IR_NODE_VISIT_CASE(SgTypedefDeclaration)
          IR_NODE_VISIT_CASE(SgFunctionTypeTable)
          IR_NODE_VISIT_CASE(SgExprStatement)
          IR_NODE_VISIT_CASE(SgLabelStatement)
          IR_NODE_VISIT_CASE(SgCaseOptionStmt)
          IR_NODE_VISIT_CASE(SgTryStmt)
          IR_NODE_VISIT_CASE(SgDefaultOptionStmt)
          IR_NODE_VISIT_CASE(SgBreakStmt)
          IR_NODE_VISIT_CASE(SgContinueStmt)
          IR_NODE_VISIT_CASE(SgReturnStmt)
          IR_NODE_VISIT_CASE(SgGotoStatement)
          IR_NODE_VISIT_CASE(SgSpawnStmt)
          IR_NODE_VISIT_CASE(SgNullStatement)
          IR_NODE_VISIT_CASE(SgVariantStatement)
          IR_NODE_VISIT_CASE(SgPragmaDeclaration)
          IR_NODE_VISIT_CASE(SgTemplateDeclaration)
          IR_NODE_VISIT_CASE(SgTemplateInstantiationDecl)
          IR_NODE_VISIT_CASE(SgTemplateInstantiationDefn)
          IR_NODE_VISIT_CASE(SgTemplateInstantiationFunctionDecl)
          IR_NODE_VISIT_CASE(SgTemplateInstantiationMemberFunctionDecl)
          IR_NODE_VISIT_CASE(SgClinkageStartStatement)
          IR_NODE_VISIT_CASE(SgNamespaceDeclarationStatement)
          IR_NODE_VISIT_CASE(SgNamespaceAliasDeclarationStatement)
          IR_NODE_VISIT_CASE(SgNamespaceDefinitionStatement)
          IR_NODE_VISIT_CASE(SgUsingDeclarationStatement)
          IR_NODE_VISIT_CASE(SgUsingDirectiveStatement)
          IR_NODE_VISIT_CASE(SgTemplateInstantiationDirectiveStatement)
          IR_NODE_VISIT_CASE(SgVariableSymbol)
          IR_NODE_VISIT_CASE(SgFunctionTypeSymbol)
          IR_NODE_VISIT_CASE(SgClassSymbol)
          IR_NODE_VISIT_CASE(SgTemplateSymbol)
          IR_NODE_VISIT_CASE(SgEnumSymbol)
          IR_NODE_VISIT_CASE(SgEnumFieldSymbol)
          IR_NODE_VISIT_CASE(SgTypedefSymbol)
          IR_NODE_VISIT_CASE(SgMemberFunctionSymbol)
          IR_NODE_VISIT_CASE(SgLabelSymbol)
          IR_NODE_VISIT_CASE(SgDefaultSymbol)
          IR_NODE_VISIT_CASE(SgNamespaceSymbol)
          IR_NODE_VISIT_CASE(SgStatementExpression)

       // DQ (9/1/2009): Added missing IR nodes.
          IR_NODE_VISIT_CASE(SgIntrinsicSymbol)
          IR_NODE_VISIT_CASE(SgModuleSymbol)
          IR_NODE_VISIT_CASE(SgInterfaceSymbol)
          IR_NODE_VISIT_CASE(SgCommonSymbol)
          IR_NODE_VISIT_CASE(SgRenameSymbol)
          IR_NODE_VISIT_CASE(SgAliasSymbol)
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
          IR_NODE_VISIT_CASE(SgAsmBlock)
          IR_NODE_VISIT_CASE(SgAsmOperandList)
          IR_NODE_VISIT_CASE(SgAsmArmInstruction)
          IR_NODE_VISIT_CASE(SgAsmX86Instruction)
          IR_NODE_VISIT_CASE(SgAsmPowerpcInstruction)
          IR_NODE_VISIT_CASE(SgAsmInstruction)
          IR_NODE_VISIT_CASE(SgAsmFunction)
          IR_NODE_VISIT_CASE(SgAsmStatement)
          IR_NODE_VISIT_CASE(SgAsmBinaryAdd)
          IR_NODE_VISIT_CASE(SgAsmBinarySubtract)
          IR_NODE_VISIT_CASE(SgAsmBinaryMultiply)
          IR_NODE_VISIT_CASE(SgAsmBinaryDivide)
          IR_NODE_VISIT_CASE(SgAsmBinaryMod)
          IR_NODE_VISIT_CASE(SgAsmBinaryAddPreupdate)
          IR_NODE_VISIT_CASE(SgAsmBinarySubtractPreupdate)
          IR_NODE_VISIT_CASE(SgAsmBinaryAddPostupdate)
          IR_NODE_VISIT_CASE(SgAsmBinarySubtractPostupdate)
          IR_NODE_VISIT_CASE(SgAsmBinaryLsl)
          IR_NODE_VISIT_CASE(SgAsmBinaryLsr)
          IR_NODE_VISIT_CASE(SgAsmBinaryAsr)
          IR_NODE_VISIT_CASE(SgAsmBinaryRor)
          IR_NODE_VISIT_CASE(SgAsmBinaryExpression)
          IR_NODE_VISIT_CASE(SgAsmUnaryPlus)
          IR_NODE_VISIT_CASE(SgAsmUnaryMinus)
          IR_NODE_VISIT_CASE(SgAsmUnaryRrx)
          IR_NODE_VISIT_CASE(SgAsmUnaryArmSpecialRegisterList)
          IR_NODE_VISIT_CASE(SgAsmUnaryExpression)
          IR_NODE_VISIT_CASE(SgAsmMemoryReferenceExpression)
          IR_NODE_VISIT_CASE(SgAsmControlFlagsExpression)
          IR_NODE_VISIT_CASE(SgAsmCommonSubExpression)
          IR_NODE_VISIT_CASE(SgAsmRegisterReferenceExpression)
          IR_NODE_VISIT_CASE(SgAsmIntegerValueExpression)
          IR_NODE_VISIT_CASE(SgAsmFloatValueExpression)
          IR_NODE_VISIT_CASE(SgAsmValueExpression)
          IR_NODE_VISIT_CASE(SgAsmExprListExp)
          IR_NODE_VISIT_CASE(SgAsmExpression)
          IR_NODE_VISIT_CASE(SgAsmIntegerType)
          IR_NODE_VISIT_CASE(SgAsmFloatType)
          IR_NODE_VISIT_CASE(SgAsmScalarType)
          IR_NODE_VISIT_CASE(SgAsmVectorType)
          IR_NODE_VISIT_CASE(SgAsmType)
          IR_NODE_VISIT_CASE(SgAsmGenericDLL)
          IR_NODE_VISIT_CASE(SgAsmPEImportItemList)
          IR_NODE_VISIT_CASE(SgAsmPEImportDirectoryList)
          IR_NODE_VISIT_CASE(SgAsmGenericFormat)
          IR_NODE_VISIT_CASE(SgAsmGenericFile)
          IR_NODE_VISIT_CASE(SgAsmElfFileHeader)
          IR_NODE_VISIT_CASE(SgAsmPEFileHeader)
          IR_NODE_VISIT_CASE(SgAsmNEFileHeader)
          IR_NODE_VISIT_CASE(SgAsmLEFileHeader)
          IR_NODE_VISIT_CASE(SgAsmDOSFileHeader)
          IR_NODE_VISIT_CASE(SgAsmGenericHeader)
          IR_NODE_VISIT_CASE(SgAsmElfRelocSection)
          IR_NODE_VISIT_CASE(SgAsmElfDynamicSection)
          IR_NODE_VISIT_CASE(SgAsmElfSymbolSection)
          IR_NODE_VISIT_CASE(SgAsmElfStringSection)
          IR_NODE_VISIT_CASE(SgAsmElfEHFrameSection)
          IR_NODE_VISIT_CASE(SgAsmElfNoteSection)
          IR_NODE_VISIT_CASE(SgAsmElfStrtab)
          IR_NODE_VISIT_CASE(SgAsmCoffStrtab)
          IR_NODE_VISIT_CASE(SgAsmGenericStrtab)
          IR_NODE_VISIT_CASE(SgAsmElfSection)
          IR_NODE_VISIT_CASE(SgAsmElfSectionTable)
          IR_NODE_VISIT_CASE(SgAsmElfSegmentTable)
          IR_NODE_VISIT_CASE(SgAsmPEImportSection)
          IR_NODE_VISIT_CASE(SgAsmPEExportSection)
          IR_NODE_VISIT_CASE(SgAsmPEStringSection)
          IR_NODE_VISIT_CASE(SgAsmPESection)
          IR_NODE_VISIT_CASE(SgAsmPESectionTable)
          IR_NODE_VISIT_CASE(SgAsmCoffSymbolTable)
          IR_NODE_VISIT_CASE(SgAsmDOSExtendedHeader)
          IR_NODE_VISIT_CASE(SgAsmNESection)
          IR_NODE_VISIT_CASE(SgAsmNESectionTable)
          IR_NODE_VISIT_CASE(SgAsmNENameTable)
          IR_NODE_VISIT_CASE(SgAsmNEModuleTable)
          IR_NODE_VISIT_CASE(SgAsmNEStringTable)
          IR_NODE_VISIT_CASE(SgAsmNEEntryTable)
          IR_NODE_VISIT_CASE(SgAsmNERelocTable)
          IR_NODE_VISIT_CASE(SgAsmLESection)
          IR_NODE_VISIT_CASE(SgAsmLESectionTable)
          IR_NODE_VISIT_CASE(SgAsmLENameTable)
          IR_NODE_VISIT_CASE(SgAsmLEPageTable)
          IR_NODE_VISIT_CASE(SgAsmLEEntryTable)
          IR_NODE_VISIT_CASE(SgAsmLERelocTable)
          IR_NODE_VISIT_CASE(SgAsmGenericSection)
          IR_NODE_VISIT_CASE(SgAsmCoffSymbol)
          IR_NODE_VISIT_CASE(SgAsmElfSymbol)
          IR_NODE_VISIT_CASE(SgAsmGenericSymbol)
          IR_NODE_VISIT_CASE(SgAsmElfSectionTableEntry)
          IR_NODE_VISIT_CASE(SgAsmElfSegmentTableEntry)
          IR_NODE_VISIT_CASE(SgAsmElfSegmentTableEntryList)
          IR_NODE_VISIT_CASE(SgAsmElfRelocEntry)
          IR_NODE_VISIT_CASE(SgAsmElfRelocEntryList)
          IR_NODE_VISIT_CASE(SgAsmElfDynamicEntry)
          IR_NODE_VISIT_CASE(SgAsmElfDynamicEntryList)
          IR_NODE_VISIT_CASE(SgAsmElfEHFrameEntryCI)
          IR_NODE_VISIT_CASE(SgAsmElfEHFrameEntryCIList)
          IR_NODE_VISIT_CASE(SgAsmElfEHFrameEntryFD)
          IR_NODE_VISIT_CASE(SgAsmElfEHFrameEntryFDList)
          IR_NODE_VISIT_CASE(SgAsmElfSymverEntry)
          IR_NODE_VISIT_CASE(SgAsmElfSymverEntryList)
          IR_NODE_VISIT_CASE(SgAsmElfSymverSection)
          IR_NODE_VISIT_CASE(SgAsmElfSymverDefinedAux)
          IR_NODE_VISIT_CASE(SgAsmElfSymverDefinedAuxList)
          IR_NODE_VISIT_CASE(SgAsmElfSymverDefinedEntry)
          IR_NODE_VISIT_CASE(SgAsmElfSymverDefinedEntryList)
          IR_NODE_VISIT_CASE(SgAsmElfSymverDefinedSection)
          IR_NODE_VISIT_CASE(SgAsmElfSymverNeededAux)
          IR_NODE_VISIT_CASE(SgAsmElfSymverNeededAuxList)
          IR_NODE_VISIT_CASE(SgAsmElfSymverNeededEntry)
          IR_NODE_VISIT_CASE(SgAsmElfSymverNeededEntryList)
          IR_NODE_VISIT_CASE(SgAsmElfSymverNeededSection)
          IR_NODE_VISIT_CASE(SgAsmElfNoteEntry)
          IR_NODE_VISIT_CASE(SgAsmElfNoteEntryList)
          IR_NODE_VISIT_CASE(SgAsmPERVASizePair)
          IR_NODE_VISIT_CASE(SgAsmPEExportDirectory)
          IR_NODE_VISIT_CASE(SgAsmPEExportEntry)
          IR_NODE_VISIT_CASE(SgAsmPEImportDirectory)
          IR_NODE_VISIT_CASE(SgAsmPEImportItem)
          IR_NODE_VISIT_CASE(SgAsmPESectionTableEntry)
          IR_NODE_VISIT_CASE(SgAsmNEEntryPoint)
          IR_NODE_VISIT_CASE(SgAsmNERelocEntry)
          IR_NODE_VISIT_CASE(SgAsmNESectionTableEntry)
          IR_NODE_VISIT_CASE(SgAsmLEPageTableEntry)
          IR_NODE_VISIT_CASE(SgAsmLEEntryPoint)
          IR_NODE_VISIT_CASE(SgAsmLESectionTableEntry)
          IR_NODE_VISIT_CASE(SgAsmGenericSectionList)
          IR_NODE_VISIT_CASE(SgAsmGenericHeaderList)
          IR_NODE_VISIT_CASE(SgAsmGenericSymbolList)
          IR_NODE_VISIT_CASE(SgAsmElfSymbolList)
          IR_NODE_VISIT_CASE(SgAsmCoffSymbolList)
          IR_NODE_VISIT_CASE(SgAsmGenericDLLList)
          IR_NODE_VISIT_CASE(SgAsmPERVASizePairList)
          IR_NODE_VISIT_CASE(SgAsmPEExportEntryList)
          IR_NODE_VISIT_CASE(SgAsmBasicString)
          IR_NODE_VISIT_CASE(SgAsmStoredString)
          IR_NODE_VISIT_CASE(SgAsmGenericString)
          IR_NODE_VISIT_CASE(SgAsmStringStorage)
          IR_NODE_VISIT_CASE(SgAsmDwarfMacro)
          IR_NODE_VISIT_CASE(SgAsmDwarfLine)
          IR_NODE_VISIT_CASE(SgAsmDwarfMacroList)
          IR_NODE_VISIT_CASE(SgAsmDwarfLineList)
          IR_NODE_VISIT_CASE(SgAsmDwarfArrayType)
          IR_NODE_VISIT_CASE(SgAsmDwarfClassType)
          IR_NODE_VISIT_CASE(SgAsmDwarfEntryPoint)
          IR_NODE_VISIT_CASE(SgAsmDwarfEnumerationType)
          IR_NODE_VISIT_CASE(SgAsmDwarfFormalParameter)
          IR_NODE_VISIT_CASE(SgAsmDwarfImportedDeclaration)
          IR_NODE_VISIT_CASE(SgAsmDwarfLabel)
          IR_NODE_VISIT_CASE(SgAsmDwarfLexicalBlock)
          IR_NODE_VISIT_CASE(SgAsmDwarfMember)
          IR_NODE_VISIT_CASE(SgAsmDwarfPointerType)
          IR_NODE_VISIT_CASE(SgAsmDwarfReferenceType)
          IR_NODE_VISIT_CASE(SgAsmDwarfCompilationUnit)
          IR_NODE_VISIT_CASE(SgAsmDwarfStringType)
          IR_NODE_VISIT_CASE(SgAsmDwarfStructureType)
          IR_NODE_VISIT_CASE(SgAsmDwarfSubroutineType)
          IR_NODE_VISIT_CASE(SgAsmDwarfTypedef)
          IR_NODE_VISIT_CASE(SgAsmDwarfUnionType)
          IR_NODE_VISIT_CASE(SgAsmDwarfUnspecifiedParameters)
          IR_NODE_VISIT_CASE(SgAsmDwarfVariant)
          IR_NODE_VISIT_CASE(SgAsmDwarfCommonBlock)
          IR_NODE_VISIT_CASE(SgAsmDwarfCommonInclusion)
          IR_NODE_VISIT_CASE(SgAsmDwarfInheritance)
          IR_NODE_VISIT_CASE(SgAsmDwarfInlinedSubroutine)
          IR_NODE_VISIT_CASE(SgAsmDwarfModule)
          IR_NODE_VISIT_CASE(SgAsmDwarfPtrToMemberType)
          IR_NODE_VISIT_CASE(SgAsmDwarfSetType)
          IR_NODE_VISIT_CASE(SgAsmDwarfSubrangeType)
          IR_NODE_VISIT_CASE(SgAsmDwarfWithStmt)
          IR_NODE_VISIT_CASE(SgAsmDwarfAccessDeclaration)
          IR_NODE_VISIT_CASE(SgAsmDwarfBaseType)
          IR_NODE_VISIT_CASE(SgAsmDwarfCatchBlock)
          IR_NODE_VISIT_CASE(SgAsmDwarfConstType)
          IR_NODE_VISIT_CASE(SgAsmDwarfConstant)
          IR_NODE_VISIT_CASE(SgAsmDwarfEnumerator)
          IR_NODE_VISIT_CASE(SgAsmDwarfFileType)
          IR_NODE_VISIT_CASE(SgAsmDwarfFriend)
          IR_NODE_VISIT_CASE(SgAsmDwarfNamelist)
          IR_NODE_VISIT_CASE(SgAsmDwarfNamelistItem)
          IR_NODE_VISIT_CASE(SgAsmDwarfPackedType)
          IR_NODE_VISIT_CASE(SgAsmDwarfSubprogram)
          IR_NODE_VISIT_CASE(SgAsmDwarfTemplateTypeParameter)
          IR_NODE_VISIT_CASE(SgAsmDwarfTemplateValueParameter)
          IR_NODE_VISIT_CASE(SgAsmDwarfThrownType)
          IR_NODE_VISIT_CASE(SgAsmDwarfTryBlock)
          IR_NODE_VISIT_CASE(SgAsmDwarfVariantPart)
          IR_NODE_VISIT_CASE(SgAsmDwarfVariable)
          IR_NODE_VISIT_CASE(SgAsmDwarfVolatileType)
          IR_NODE_VISIT_CASE(SgAsmDwarfDwarfProcedure)
          IR_NODE_VISIT_CASE(SgAsmDwarfRestrictType)
          IR_NODE_VISIT_CASE(SgAsmDwarfInterfaceType)
          IR_NODE_VISIT_CASE(SgAsmDwarfNamespace)
          IR_NODE_VISIT_CASE(SgAsmDwarfImportedModule)
          IR_NODE_VISIT_CASE(SgAsmDwarfUnspecifiedType)
          IR_NODE_VISIT_CASE(SgAsmDwarfPartialUnit)
          IR_NODE_VISIT_CASE(SgAsmDwarfImportedUnit)
          IR_NODE_VISIT_CASE(SgAsmDwarfMutableType)
          IR_NODE_VISIT_CASE(SgAsmDwarfCondition)
          IR_NODE_VISIT_CASE(SgAsmDwarfSharedType)
          IR_NODE_VISIT_CASE(SgAsmDwarfFormatLabel)
          IR_NODE_VISIT_CASE(SgAsmDwarfFunctionTemplate)
          IR_NODE_VISIT_CASE(SgAsmDwarfClassTemplate)
          IR_NODE_VISIT_CASE(SgAsmDwarfUpcSharedType)
          IR_NODE_VISIT_CASE(SgAsmDwarfUpcStrictType)
          IR_NODE_VISIT_CASE(SgAsmDwarfUpcRelaxedType)
          IR_NODE_VISIT_CASE(SgAsmDwarfUnknownConstruct)
          IR_NODE_VISIT_CASE(SgAsmDwarfConstruct)
          IR_NODE_VISIT_CASE(SgAsmDwarfConstructList)
          IR_NODE_VISIT_CASE(SgAsmDwarfCompilationUnitList)
          IR_NODE_VISIT_CASE(SgAsmDwarfInformation)
          IR_NODE_VISIT_CASE(SgAsmExecutableFileFormat)
          IR_NODE_VISIT_CASE(SgAsmInterpretationList)
          IR_NODE_VISIT_CASE(SgAsmGenericFileList)
          IR_NODE_VISIT_CASE(SgAsmInterpretation)
          IR_NODE_VISIT_CASE(SgAsmNode)
#endif
          IR_NODE_VISIT_CASE(SgOmpOrderedClause)
          IR_NODE_VISIT_CASE(SgOmpNowaitClause)
          IR_NODE_VISIT_CASE(SgOmpUntiedClause)
          IR_NODE_VISIT_CASE(SgOmpMergeableClause)
          IR_NODE_VISIT_CASE(SgOmpDefaultClause)
          IR_NODE_VISIT_CASE(SgOmpCollapseClause)
          IR_NODE_VISIT_CASE(SgOmpIfClause)
          IR_NODE_VISIT_CASE(SgOmpFinalClause)
          IR_NODE_VISIT_CASE(SgOmpPriorityClause)
          IR_NODE_VISIT_CASE(SgOmpNumThreadsClause)
          IR_NODE_VISIT_CASE(SgOmpProcBindClause)
          IR_NODE_VISIT_CASE(SgOmpSafelenClause)
          IR_NODE_VISIT_CASE(SgOmpSimdlenClause)
          IR_NODE_VISIT_CASE(SgOmpAtomicClause)
          IR_NODE_VISIT_CASE(SgOmpInbranchClause)
          IR_NODE_VISIT_CASE(SgOmpNotinbranchClause)
          IR_NODE_VISIT_CASE(SgOmpExpressionClause)
          IR_NODE_VISIT_CASE(SgOmpCopyprivateClause)
          IR_NODE_VISIT_CASE(SgOmpPrivateClause)
          IR_NODE_VISIT_CASE(SgOmpFirstprivateClause)
          IR_NODE_VISIT_CASE(SgOmpSharedClause)
          IR_NODE_VISIT_CASE(SgOmpCopyinClause)
          IR_NODE_VISIT_CASE(SgOmpLastprivateClause)
          IR_NODE_VISIT_CASE(SgOmpReductionClause)
          IR_NODE_VISIT_CASE(SgOmpVariablesClause)
          IR_NODE_VISIT_CASE(SgOmpScheduleClause)
          IR_NODE_VISIT_CASE(SgOmpDependClause)
          IR_NODE_VISIT_CASE(SgOmpClause)
          IR_NODE_VISIT_CASE(SgRenamePair)
          IR_NODE_VISIT_CASE(SgInterfaceBody)
          IR_NODE_VISIT_CASE(SgLocatedNodeSupport)
          IR_NODE_VISIT_CASE(SgToken)

#if 0
Case not handled: SgTemplateMemberFunctionDeclaration 
Case not handled: SgTemplateFunctionDeclaration 
Case not handled: SgTemplateVariableDeclaration 
Case not handled: SgTemplateClassDefinition 
Case not handled: SgTemplateFunctionDefinition 
Case not handled: SgTemplateMemberFunctionRefExp 
No representative for SgShortVal found in memory pools 
No representative for SgUnsignedCharVal found in memory pools 
No representative for SgWcharVal found in memory pools 
No representative for SgUnsignedShortVal found in memory pools 
Case not handled: SgTemplateParameterVal 
Case not handled: SgTemplateClassSymbol 
Case not handled: SgTemplateFunctionSymbol 
Case not handled: SgTemplateMemberFunctionSymbol 
#endif

       // DQ (11/19/2012): Added missing IR nodes.
          IR_NODE_VISIT_CASE(SgTypeTable)
          IR_NODE_VISIT_CASE(SgPartialFunctionType)
          IR_NODE_VISIT_CASE(SgTemplateClassDeclaration)
          IR_NODE_VISIT_CASE(SgTemplateMemberFunctionDeclaration)
          IR_NODE_VISIT_CASE(SgTemplateFunctionDeclaration)
          IR_NODE_VISIT_CASE(SgTemplateVariableDeclaration)
          IR_NODE_VISIT_CASE(SgTemplateClassDefinition)
          IR_NODE_VISIT_CASE(SgTemplateFunctionDefinition)
          IR_NODE_VISIT_CASE(SgTemplateMemberFunctionRefExp)
          IR_NODE_VISIT_CASE(SgTemplateParameterVal)
          IR_NODE_VISIT_CASE(SgTemplateClassSymbol)
          IR_NODE_VISIT_CASE(SgTemplateFunctionSymbol)
          IR_NODE_VISIT_CASE(SgTemplateMemberFunctionSymbol)

       // IR_NODE_VISIT_CASE()

          default:
             {
#if 1
               printf ("Case not handled (must be added to src/midend/astDiagnostics/AstStatistics.C): %s \n",node->class_name().c_str());
#endif
             }
        }
   }


// ************************************************************************
//                    AstNodeStatistics member functions
// ************************************************************************

string
AstNodeStatistics::traversalStatistics( SgNode* node )
   {
  // test statistics
     ROSE_ASSERT(node != NULL);
     string s;

     AstNodeTraversalStatistics stat;
  // if (project->get_verbose() > 1)
     s = stat.toString(node);

     return s;
   }

string
AstNodeStatistics::IRnodeUsageStatistics()
   {
  // printf ("IR node statistics not implemented! \n");
     string s = "\n\n";
     AstNodeMemoryPoolStatistics memoryPoolTraversal;
     memoryPoolTraversal.traverseRepresentativeIRnodes();

  // s = "AstNodeStatistics::IRnodeUsageStatistics(): Not finished being implemented \n";

  // ROSE_ASSERT(false);

     return s;
   }

