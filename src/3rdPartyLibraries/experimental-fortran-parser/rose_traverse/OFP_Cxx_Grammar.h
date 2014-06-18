#ifndef Cxx_Grammar_H 
#define Cxx_Grammar_H 

enum VariantT 
{
V_SgUntypedUnaryOperator = 693,
V_SgUntypedBinaryOperator = 694,
V_SgUntypedValueExpression = 695,
V_SgUntypedArrayReferenceExpression = 696,
V_SgUntypedOtherExpression = 697,
V_SgUntypedFunctionCallOrArrayReferenceExpression = 698,
V_SgUntypedReferenceExpression = 712,
V_SgUntypedExpression = 692,
V_SgUntypedImplicitDeclaration = 707,
V_SgUntypedVariableDeclaration = 708,
V_SgUntypedProgramHeaderDeclaration = 709,
V_SgUntypedSubroutineDeclaration = 711,
V_SgUntypedFunctionDeclaration = 710,
V_SgUntypedModuleDeclaration = 721,
V_SgUntypedDeclarationStatement = 705,
V_SgUntypedAssignmentStatement = 701,
V_SgUntypedFunctionCallStatement = 702,
V_SgUntypedBlockStatement = 703,
V_SgUntypedNamedStatement = 700,
V_SgUntypedOtherStatement = 704,
V_SgUntypedFunctionScope = 718,
V_SgUntypedModuleScope = 719,
V_SgUntypedGlobalScope = 720,
V_SgUntypedScope = 717,
V_SgUntypedStatement = 699,
V_SgUntypedArrayType = 714,
V_SgUntypedType = 713,
V_SgUntypedAttribute = 715,
V_SgUntypedInitializedName = 706,
V_SgUntypedFile = 716,
V_SgUntypedStatementList = 722,
V_SgUntypedDeclarationList = 723,
V_SgUntypedFunctionDeclarationList = 724,
V_SgUntypedInitializedNameList = 725,
V_SgUntypedNode = 691,
V_SgLocatedNodeSupport = 453,
V_SgToken = 638,
V_SgLocatedNode = 452,
V_SgNode = 488, V_SgNumVariants = 784};

class SgUntypedUnaryOperator;
class SgUntypedBinaryOperator;
class SgUntypedValueExpression;
class SgUntypedArrayReferenceExpression;
class SgUntypedOtherExpression;
class SgUntypedFunctionCallOrArrayReferenceExpression;
class SgUntypedReferenceExpression;
class SgUntypedExpression;
class SgUntypedImplicitDeclaration;
class SgUntypedVariableDeclaration;
class SgUntypedProgramHeaderDeclaration;
class SgUntypedSubroutineDeclaration;
class SgUntypedFunctionDeclaration;
class SgUntypedModuleDeclaration;
class SgUntypedDeclarationStatement;
class SgUntypedAssignmentStatement;
class SgUntypedFunctionCallStatement;
class SgUntypedBlockStatement;
class SgUntypedNamedStatement;
class SgUntypedOtherStatement;
class SgUntypedFunctionScope;
class SgUntypedModuleScope;
class SgUntypedGlobalScope;
class SgUntypedScope;
class SgUntypedStatement;
class SgUntypedArrayType;
class SgUntypedType;
class SgUntypedAttribute;
class SgUntypedInitializedName;
class SgUntypedFile;
class SgUntypedStatementList;
class SgUntypedDeclarationList;
class SgUntypedFunctionDeclarationList;
class SgUntypedInitializedNameList;
class SgUntypedNode;
class SgLocatedNodeSupport;
class SgToken;
class SgLocatedNode;
class SgNode;


ROSE_DLL_API SgUntypedProgramHeaderDeclaration* isSgUntypedProgramHeaderDeclaration(SgNode* node);
ROSE_DLL_API const SgUntypedProgramHeaderDeclaration* isSgUntypedProgramHeaderDeclaration(const SgNode* node);

ROSE_DLL_API SgUntypedDeclarationStatement* isSgUntypedDeclarationStatement(SgNode* node);
ROSE_DLL_API const SgUntypedDeclarationStatement* isSgUntypedDeclarationStatement(const SgNode* node);

ROSE_DLL_API SgUntypedStatement* isSgUntypedStatement(SgNode* node);
ROSE_DLL_API const SgUntypedStatement* isSgUntypedStatement(const SgNode* node);

ROSE_DLL_API SgUntypedType* isSgUntypedType(SgNode* node);
ROSE_DLL_API const SgUntypedType* isSgUntypedType(const SgNode* node);

#ifdef NOT_YET
extern const uint8_t rose_ClassHierarchyCastTable[783][98] ;

#define IS_SgUntypedProgramHeaderDeclaration_FAST_MACRO(node) ( (node) ? (( (rose_ClassHierarchyCastTable[(node)->variantT()][SgUntypedProgramHeaderDeclaration::static_variant >> 3] & (1 << (SgUntypedProgramHeaderDeclaration::static_variant & 7)))) ? ((SgUntypedProgramHeaderDeclaration*) (node)) : NULL) : NULL)

#define IS_SgUntypedDeclarationStatement_FAST_MACRO(node) ( (node) ? (( (rose_ClassHierarchyCastTable[(node)->variantT()][SgUntypedDeclarationStatement::static_variant >> 3] & (1 << (SgUntypedDeclarationStatement::static_variant & 7)))) ? ((SgUntypedDeclarationStatement*) (node)) : NULL) : NULL)

#define IS_SgUntypedStatement_FAST_MACRO(node) ( (node) ? (( (rose_ClassHierarchyCastTable[(node)->variantT()][SgUntypedStatement::static_variant >> 3] & (1 << (SgUntypedStatement::static_variant & 7)))) ? ((SgUntypedStatement*) (node)) : NULL) : NULL)

#define IS_SgUntypedType_FAST_MACRO(node) ( (node) ? (( (rose_ClassHierarchyCastTable[(node)->variantT()][SgUntypedType::static_variant >> 3] & (1 << (SgUntypedType::static_variant & 7)))) ? ((SgUntypedType*) (node)) : NULL) : NULL)

ROSE_DLL_API SgUntypedUnaryOperator* isSgUntypedUnaryOperator(SgNode* node);
ROSE_DLL_API const SgUntypedUnaryOperator* isSgUntypedUnaryOperator(const SgNode* node);
#define IS_SgUntypedUnaryOperator_FAST_MACRO(node) ( (node) ? (( (rose_ClassHierarchyCastTable[(node)->variantT()][SgUntypedUnaryOperator::static_variant >> 3] & (1 << (SgUntypedUnaryOperator::static_variant & 7)))) ? ((SgUntypedUnaryOperator*) (node)) : NULL) : NULL)
ROSE_DLL_API SgUntypedBinaryOperator* isSgUntypedBinaryOperator(SgNode* node);
ROSE_DLL_API const SgUntypedBinaryOperator* isSgUntypedBinaryOperator(const SgNode* node);
#define IS_SgUntypedBinaryOperator_FAST_MACRO(node) ( (node) ? (( (rose_ClassHierarchyCastTable[(node)->variantT()][SgUntypedBinaryOperator::static_variant >> 3] & (1 << (SgUntypedBinaryOperator::static_variant & 7)))) ? ((SgUntypedBinaryOperator*) (node)) : NULL) : NULL)
ROSE_DLL_API SgUntypedValueExpression* isSgUntypedValueExpression(SgNode* node);
ROSE_DLL_API const SgUntypedValueExpression* isSgUntypedValueExpression(const SgNode* node);
#define IS_SgUntypedValueExpression_FAST_MACRO(node) ( (node) ? (( (rose_ClassHierarchyCastTable[(node)->variantT()][SgUntypedValueExpression::static_variant >> 3] & (1 << (SgUntypedValueExpression::static_variant & 7)))) ? ((SgUntypedValueExpression*) (node)) : NULL) : NULL)
ROSE_DLL_API SgUntypedArrayReferenceExpression* isSgUntypedArrayReferenceExpression(SgNode* node);
ROSE_DLL_API const SgUntypedArrayReferenceExpression* isSgUntypedArrayReferenceExpression(const SgNode* node);
#define IS_SgUntypedArrayReferenceExpression_FAST_MACRO(node) ( (node) ? (( (rose_ClassHierarchyCastTable[(node)->variantT()][SgUntypedArrayReferenceExpression::static_variant >> 3] & (1 << (SgUntypedArrayReferenceExpression::static_variant & 7)))) ? ((SgUntypedArrayReferenceExpression*) (node)) : NULL) : NULL)
ROSE_DLL_API SgUntypedOtherExpression* isSgUntypedOtherExpression(SgNode* node);
ROSE_DLL_API const SgUntypedOtherExpression* isSgUntypedOtherExpression(const SgNode* node);
#define IS_SgUntypedOtherExpression_FAST_MACRO(node) ( (node) ? (( (rose_ClassHierarchyCastTable[(node)->variantT()][SgUntypedOtherExpression::static_variant >> 3] & (1 << (SgUntypedOtherExpression::static_variant & 7)))) ? ((SgUntypedOtherExpression*) (node)) : NULL) : NULL)
ROSE_DLL_API SgUntypedFunctionCallOrArrayReferenceExpression* isSgUntypedFunctionCallOrArrayReferenceExpression(SgNode* node);
ROSE_DLL_API const SgUntypedFunctionCallOrArrayReferenceExpression* isSgUntypedFunctionCallOrArrayReferenceExpression(const SgNode* node);
#define IS_SgUntypedFunctionCallOrArrayReferenceExpression_FAST_MACRO(node) ( (node) ? (( (rose_ClassHierarchyCastTable[(node)->variantT()][SgUntypedFunctionCallOrArrayReferenceExpression::static_variant >> 3] & (1 << (SgUntypedFunctionCallOrArrayReferenceExpression::static_variant & 7)))) ? ((SgUntypedFunctionCallOrArrayReferenceExpression*) (node)) : NULL) : NULL)
ROSE_DLL_API SgUntypedReferenceExpression* isSgUntypedReferenceExpression(SgNode* node);
ROSE_DLL_API const SgUntypedReferenceExpression* isSgUntypedReferenceExpression(const SgNode* node);
#define IS_SgUntypedReferenceExpression_FAST_MACRO(node) ( (node) ? (( (rose_ClassHierarchyCastTable[(node)->variantT()][SgUntypedReferenceExpression::static_variant >> 3] & (1 << (SgUntypedReferenceExpression::static_variant & 7)))) ? ((SgUntypedReferenceExpression*) (node)) : NULL) : NULL)
ROSE_DLL_API SgUntypedExpression* isSgUntypedExpression(SgNode* node);
ROSE_DLL_API const SgUntypedExpression* isSgUntypedExpression(const SgNode* node);
#define IS_SgUntypedExpression_FAST_MACRO(node) ( (node) ? (( (rose_ClassHierarchyCastTable[(node)->variantT()][SgUntypedExpression::static_variant >> 3] & (1 << (SgUntypedExpression::static_variant & 7)))) ? ((SgUntypedExpression*) (node)) : NULL) : NULL)
ROSE_DLL_API SgUntypedImplicitDeclaration* isSgUntypedImplicitDeclaration(SgNode* node);
ROSE_DLL_API const SgUntypedImplicitDeclaration* isSgUntypedImplicitDeclaration(const SgNode* node);
#define IS_SgUntypedImplicitDeclaration_FAST_MACRO(node) ( (node) ? (( (rose_ClassHierarchyCastTable[(node)->variantT()][SgUntypedImplicitDeclaration::static_variant >> 3] & (1 << (SgUntypedImplicitDeclaration::static_variant & 7)))) ? ((SgUntypedImplicitDeclaration*) (node)) : NULL) : NULL)
ROSE_DLL_API SgUntypedVariableDeclaration* isSgUntypedVariableDeclaration(SgNode* node);
ROSE_DLL_API const SgUntypedVariableDeclaration* isSgUntypedVariableDeclaration(const SgNode* node);
#define IS_SgUntypedVariableDeclaration_FAST_MACRO(node) ( (node) ? (( (rose_ClassHierarchyCastTable[(node)->variantT()][SgUntypedVariableDeclaration::static_variant >> 3] & (1 << (SgUntypedVariableDeclaration::static_variant & 7)))) ? ((SgUntypedVariableDeclaration*) (node)) : NULL) : NULL)

ROSE_DLL_API SgUntypedSubroutineDeclaration* isSgUntypedSubroutineDeclaration(SgNode* node);
ROSE_DLL_API const SgUntypedSubroutineDeclaration* isSgUntypedSubroutineDeclaration(const SgNode* node);
#define IS_SgUntypedSubroutineDeclaration_FAST_MACRO(node) ( (node) ? (( (rose_ClassHierarchyCastTable[(node)->variantT()][SgUntypedSubroutineDeclaration::static_variant >> 3] & (1 << (SgUntypedSubroutineDeclaration::static_variant & 7)))) ? ((SgUntypedSubroutineDeclaration*) (node)) : NULL) : NULL)
ROSE_DLL_API SgUntypedFunctionDeclaration* isSgUntypedFunctionDeclaration(SgNode* node);
ROSE_DLL_API const SgUntypedFunctionDeclaration* isSgUntypedFunctionDeclaration(const SgNode* node);
#define IS_SgUntypedFunctionDeclaration_FAST_MACRO(node) ( (node) ? (( (rose_ClassHierarchyCastTable[(node)->variantT()][SgUntypedFunctionDeclaration::static_variant >> 3] & (1 << (SgUntypedFunctionDeclaration::static_variant & 7)))) ? ((SgUntypedFunctionDeclaration*) (node)) : NULL) : NULL)
ROSE_DLL_API SgUntypedModuleDeclaration* isSgUntypedModuleDeclaration(SgNode* node);
ROSE_DLL_API const SgUntypedModuleDeclaration* isSgUntypedModuleDeclaration(const SgNode* node);
#define IS_SgUntypedModuleDeclaration_FAST_MACRO(node) ( (node) ? (( (rose_ClassHierarchyCastTable[(node)->variantT()][SgUntypedModuleDeclaration::static_variant >> 3] & (1 << (SgUntypedModuleDeclaration::static_variant & 7)))) ? ((SgUntypedModuleDeclaration*) (node)) : NULL) : NULL)
ROSE_DLL_API SgUntypedDeclarationStatement* isSgUntypedDeclarationStatement(SgNode* node);
ROSE_DLL_API const SgUntypedDeclarationStatement* isSgUntypedDeclarationStatement(const SgNode* node);
#define IS_SgUntypedDeclarationStatement_FAST_MACRO(node) ( (node) ? (( (rose_ClassHierarchyCastTable[(node)->variantT()][SgUntypedDeclarationStatement::static_variant >> 3] & (1 << (SgUntypedDeclarationStatement::static_variant & 7)))) ? ((SgUntypedDeclarationStatement*) (node)) : NULL) : NULL)
ROSE_DLL_API SgUntypedAssignmentStatement* isSgUntypedAssignmentStatement(SgNode* node);
ROSE_DLL_API const SgUntypedAssignmentStatement* isSgUntypedAssignmentStatement(const SgNode* node);
#define IS_SgUntypedAssignmentStatement_FAST_MACRO(node) ( (node) ? (( (rose_ClassHierarchyCastTable[(node)->variantT()][SgUntypedAssignmentStatement::static_variant >> 3] & (1 << (SgUntypedAssignmentStatement::static_variant & 7)))) ? ((SgUntypedAssignmentStatement*) (node)) : NULL) : NULL)
ROSE_DLL_API SgUntypedFunctionCallStatement* isSgUntypedFunctionCallStatement(SgNode* node);
ROSE_DLL_API const SgUntypedFunctionCallStatement* isSgUntypedFunctionCallStatement(const SgNode* node);
#define IS_SgUntypedFunctionCallStatement_FAST_MACRO(node) ( (node) ? (( (rose_ClassHierarchyCastTable[(node)->variantT()][SgUntypedFunctionCallStatement::static_variant >> 3] & (1 << (SgUntypedFunctionCallStatement::static_variant & 7)))) ? ((SgUntypedFunctionCallStatement*) (node)) : NULL) : NULL)
ROSE_DLL_API SgUntypedBlockStatement* isSgUntypedBlockStatement(SgNode* node);
ROSE_DLL_API const SgUntypedBlockStatement* isSgUntypedBlockStatement(const SgNode* node);
#define IS_SgUntypedBlockStatement_FAST_MACRO(node) ( (node) ? (( (rose_ClassHierarchyCastTable[(node)->variantT()][SgUntypedBlockStatement::static_variant >> 3] & (1 << (SgUntypedBlockStatement::static_variant & 7)))) ? ((SgUntypedBlockStatement*) (node)) : NULL) : NULL)
ROSE_DLL_API SgUntypedNamedStatement* isSgUntypedNamedStatement(SgNode* node);
ROSE_DLL_API const SgUntypedNamedStatement* isSgUntypedNamedStatement(const SgNode* node);
#define IS_SgUntypedNamedStatement_FAST_MACRO(node) ( (node) ? (( (rose_ClassHierarchyCastTable[(node)->variantT()][SgUntypedNamedStatement::static_variant >> 3] & (1 << (SgUntypedNamedStatement::static_variant & 7)))) ? ((SgUntypedNamedStatement*) (node)) : NULL) : NULL)
ROSE_DLL_API SgUntypedOtherStatement* isSgUntypedOtherStatement(SgNode* node);
ROSE_DLL_API const SgUntypedOtherStatement* isSgUntypedOtherStatement(const SgNode* node);
#define IS_SgUntypedOtherStatement_FAST_MACRO(node) ( (node) ? (( (rose_ClassHierarchyCastTable[(node)->variantT()][SgUntypedOtherStatement::static_variant >> 3] & (1 << (SgUntypedOtherStatement::static_variant & 7)))) ? ((SgUntypedOtherStatement*) (node)) : NULL) : NULL)
ROSE_DLL_API SgUntypedFunctionScope* isSgUntypedFunctionScope(SgNode* node);
ROSE_DLL_API const SgUntypedFunctionScope* isSgUntypedFunctionScope(const SgNode* node);
#define IS_SgUntypedFunctionScope_FAST_MACRO(node) ( (node) ? (( (rose_ClassHierarchyCastTable[(node)->variantT()][SgUntypedFunctionScope::static_variant >> 3] & (1 << (SgUntypedFunctionScope::static_variant & 7)))) ? ((SgUntypedFunctionScope*) (node)) : NULL) : NULL)
ROSE_DLL_API SgUntypedModuleScope* isSgUntypedModuleScope(SgNode* node);
ROSE_DLL_API const SgUntypedModuleScope* isSgUntypedModuleScope(const SgNode* node);
#define IS_SgUntypedModuleScope_FAST_MACRO(node) ( (node) ? (( (rose_ClassHierarchyCastTable[(node)->variantT()][SgUntypedModuleScope::static_variant >> 3] & (1 << (SgUntypedModuleScope::static_variant & 7)))) ? ((SgUntypedModuleScope*) (node)) : NULL) : NULL)
ROSE_DLL_API SgUntypedGlobalScope* isSgUntypedGlobalScope(SgNode* node);
ROSE_DLL_API const SgUntypedGlobalScope* isSgUntypedGlobalScope(const SgNode* node);
#define IS_SgUntypedGlobalScope_FAST_MACRO(node) ( (node) ? (( (rose_ClassHierarchyCastTable[(node)->variantT()][SgUntypedGlobalScope::static_variant >> 3] & (1 << (SgUntypedGlobalScope::static_variant & 7)))) ? ((SgUntypedGlobalScope*) (node)) : NULL) : NULL)
ROSE_DLL_API SgUntypedScope* isSgUntypedScope(SgNode* node);
ROSE_DLL_API const SgUntypedScope* isSgUntypedScope(const SgNode* node);
#define IS_SgUntypedScope_FAST_MACRO(node) ( (node) ? (( (rose_ClassHierarchyCastTable[(node)->variantT()][SgUntypedScope::static_variant >> 3] & (1 << (SgUntypedScope::static_variant & 7)))) ? ((SgUntypedScope*) (node)) : NULL) : NULL)
ROSE_DLL_API SgUntypedArrayType* isSgUntypedArrayType(SgNode* node);
ROSE_DLL_API const SgUntypedArrayType* isSgUntypedArrayType(const SgNode* node);
#define IS_SgUntypedArrayType_FAST_MACRO(node) ( (node) ? (( (rose_ClassHierarchyCastTable[(node)->variantT()][SgUntypedArrayType::static_variant >> 3] & (1 << (SgUntypedArrayType::static_variant & 7)))) ? ((SgUntypedArrayType*) (node)) : NULL) : NULL)
ROSE_DLL_API SgUntypedAttribute* isSgUntypedAttribute(SgNode* node);
ROSE_DLL_API const SgUntypedAttribute* isSgUntypedAttribute(const SgNode* node);
#define IS_SgUntypedAttribute_FAST_MACRO(node) ( (node) ? (( (rose_ClassHierarchyCastTable[(node)->variantT()][SgUntypedAttribute::static_variant >> 3] & (1 << (SgUntypedAttribute::static_variant & 7)))) ? ((SgUntypedAttribute*) (node)) : NULL) : NULL)
ROSE_DLL_API SgUntypedInitializedName* isSgUntypedInitializedName(SgNode* node);
ROSE_DLL_API const SgUntypedInitializedName* isSgUntypedInitializedName(const SgNode* node);
#define IS_SgUntypedInitializedName_FAST_MACRO(node) ( (node) ? (( (rose_ClassHierarchyCastTable[(node)->variantT()][SgUntypedInitializedName::static_variant >> 3] & (1 << (SgUntypedInitializedName::static_variant & 7)))) ? ((SgUntypedInitializedName*) (node)) : NULL) : NULL)
ROSE_DLL_API SgUntypedFile* isSgUntypedFile(SgNode* node);
ROSE_DLL_API const SgUntypedFile* isSgUntypedFile(const SgNode* node);
#define IS_SgUntypedFile_FAST_MACRO(node) ( (node) ? (( (rose_ClassHierarchyCastTable[(node)->variantT()][SgUntypedFile::static_variant >> 3] & (1 << (SgUntypedFile::static_variant & 7)))) ? ((SgUntypedFile*) (node)) : NULL) : NULL)
ROSE_DLL_API SgUntypedStatementList* isSgUntypedStatementList(SgNode* node);
ROSE_DLL_API const SgUntypedStatementList* isSgUntypedStatementList(const SgNode* node);
#define IS_SgUntypedStatementList_FAST_MACRO(node) ( (node) ? (( (rose_ClassHierarchyCastTable[(node)->variantT()][SgUntypedStatementList::static_variant >> 3] & (1 << (SgUntypedStatementList::static_variant & 7)))) ? ((SgUntypedStatementList*) (node)) : NULL) : NULL)
ROSE_DLL_API SgUntypedDeclarationList* isSgUntypedDeclarationList(SgNode* node);
ROSE_DLL_API const SgUntypedDeclarationList* isSgUntypedDeclarationList(const SgNode* node);
#define IS_SgUntypedDeclarationList_FAST_MACRO(node) ( (node) ? (( (rose_ClassHierarchyCastTable[(node)->variantT()][SgUntypedDeclarationList::static_variant >> 3] & (1 << (SgUntypedDeclarationList::static_variant & 7)))) ? ((SgUntypedDeclarationList*) (node)) : NULL) : NULL)
ROSE_DLL_API SgUntypedFunctionDeclarationList* isSgUntypedFunctionDeclarationList(SgNode* node);
ROSE_DLL_API const SgUntypedFunctionDeclarationList* isSgUntypedFunctionDeclarationList(const SgNode* node);
#define IS_SgUntypedFunctionDeclarationList_FAST_MACRO(node) ( (node) ? (( (rose_ClassHierarchyCastTable[(node)->variantT()][SgUntypedFunctionDeclarationList::static_variant >> 3] & (1 << (SgUntypedFunctionDeclarationList::static_variant & 7)))) ? ((SgUntypedFunctionDeclarationList*) (node)) : NULL) : NULL)
ROSE_DLL_API SgUntypedInitializedNameList* isSgUntypedInitializedNameList(SgNode* node);
ROSE_DLL_API const SgUntypedInitializedNameList* isSgUntypedInitializedNameList(const SgNode* node);
#define IS_SgUntypedInitializedNameList_FAST_MACRO(node) ( (node) ? (( (rose_ClassHierarchyCastTable[(node)->variantT()][SgUntypedInitializedNameList::static_variant >> 3] & (1 << (SgUntypedInitializedNameList::static_variant & 7)))) ? ((SgUntypedInitializedNameList*) (node)) : NULL) : NULL)
ROSE_DLL_API SgUntypedNode* isSgUntypedNode(SgNode* node);
ROSE_DLL_API const SgUntypedNode* isSgUntypedNode(const SgNode* node);
#define IS_SgUntypedNode_FAST_MACRO(node) ( (node) ? (( (rose_ClassHierarchyCastTable[(node)->variantT()][SgUntypedNode::static_variant >> 3] & (1 << (SgUntypedNode::static_variant & 7)))) ? ((SgUntypedNode*) (node)) : NULL) : NULL)
#endif


// Class Definition for SgUntypedNode
class ROSE_DLL_API SgUntypedNode  : public SgLocatedNodeSupport
   {
     public: 

          virtual VariantT variantT() const;
          enum { static_variant = V_SgUntypedNode };

          ROSE_DLL_API friend       SgUntypedNode* isSgUntypedNode(       SgNode * s );
          ROSE_DLL_API friend const SgUntypedNode* isSgUntypedNode( const SgNode * s );

          typedef SgLocatedNodeSupport base_node_type;

     public: 
         virtual ~SgUntypedNode();

     public: 
         SgUntypedNode(Sg_File_Info* startOfConstruct ); 
         SgUntypedNode(); 

    protected:

   };


// Class Definition for SgUntypedExpression
class ROSE_DLL_API SgUntypedExpression  : public SgUntypedNode
   {
     public:
          typedef SgUntypedNode base_node_type;

     public: 
         SgToken::ROSE_Fortran_Keywords get_statement_enum() const;
         void set_statement_enum(SgToken::ROSE_Fortran_Keywords statement_enum);

     public: 
         virtual ~SgUntypedExpression();

     public: 
         SgUntypedExpression(Sg_File_Info* startOfConstruct , SgToken::ROSE_Fortran_Keywords statement_enum = SgToken::FORTRAN_UNKNOWN); 
         SgUntypedExpression(SgToken::ROSE_Fortran_Keywords statement_enum); 

    protected:

         SgToken::ROSE_Fortran_Keywords p_statement_enum;
          
   };

// Class Definition for SgUntypedUnaryOperator
class ROSE_DLL_API SgUntypedUnaryOperator  : public SgUntypedExpression
   {
     public:

         typedef SgUntypedExpression base_node_type;

     public: 
         SgToken::ROSE_Fortran_Operators get_operator_enum() const;
         void set_operator_enum(SgToken::ROSE_Fortran_Operators operator_enum);

     public: 
         std::string get_operator_name() const;
         void set_operator_name(std::string operator_name);

     public: 
         SgUntypedExpression* get_operand() const;
         void set_operand(SgUntypedExpression* operand);

     public: 
         virtual ~SgUntypedUnaryOperator();

     public: 
         SgUntypedUnaryOperator(Sg_File_Info* startOfConstruct , SgToken::ROSE_Fortran_Keywords statement_enum = SgToken::FORTRAN_UNKNOWN, SgToken::ROSE_Fortran_Operators operator_enum = SgToken::FORTRAN_INTRINSIC_PLUS, std::string operator_name = "", SgUntypedExpression* operand = NULL); 
         SgUntypedUnaryOperator(SgToken::ROSE_Fortran_Keywords statement_enum, SgToken::ROSE_Fortran_Operators operator_enum, std::string operator_name, SgUntypedExpression* operand); 

    protected:
         SgToken::ROSE_Fortran_Operators p_operator_enum;
         std::string p_operator_name;
         SgUntypedExpression* p_operand;
   };


// Class Definition for SgUntypedBinaryOperator
class ROSE_DLL_API SgUntypedBinaryOperator  : public SgUntypedExpression
   {
     public:

          typedef SgUntypedExpression base_node_type;

     public: 
         SgToken::ROSE_Fortran_Operators get_operator_enum() const;
         void set_operator_enum(SgToken::ROSE_Fortran_Operators operator_enum);

     public: 
         std::string get_operator_name() const;
         void set_operator_name(std::string operator_name);

     public: 
         SgUntypedExpression* get_lhs_operand() const;
         void set_lhs_operand(SgUntypedExpression* lhs_operand);

     public: 
         SgUntypedExpression* get_rhs_operand() const;
         void set_rhs_operand(SgUntypedExpression* rhs_operand);

     public: 
         virtual ~SgUntypedBinaryOperator();

     public: 
         SgUntypedBinaryOperator(Sg_File_Info* startOfConstruct , SgToken::ROSE_Fortran_Keywords statement_enum = SgToken::FORTRAN_UNKNOWN, SgToken::ROSE_Fortran_Operators operator_enum = SgToken::FORTRAN_INTRINSIC_PLUS, std::string operator_name = "", SgUntypedExpression* lhs_operand = NULL, SgUntypedExpression* rhs_operand = NULL); 
         SgUntypedBinaryOperator(SgToken::ROSE_Fortran_Keywords statement_enum, SgToken::ROSE_Fortran_Operators operator_enum, std::string operator_name, SgUntypedExpression* lhs_operand, SgUntypedExpression* rhs_operand); 

    protected:
         SgToken::ROSE_Fortran_Operators p_operator_enum;
         std::string p_operator_name;
         SgUntypedExpression* p_lhs_operand;
         SgUntypedExpression* p_rhs_operand;
   };


// Class Definition for SgUntypedValueExpression
class ROSE_DLL_API SgUntypedValueExpression  : public SgUntypedExpression
   {
     public:
          typedef SgUntypedExpression base_node_type;

     public: 
         std::string get_value_string() const;
         void set_value_string(std::string value_string);

     public: 
         SgUntypedType* get_type() const;
         void set_type(SgUntypedType* type);

     public: 
         virtual ~SgUntypedValueExpression();

     public: 
         SgUntypedValueExpression(Sg_File_Info* startOfConstruct , SgToken::ROSE_Fortran_Keywords statement_enum = SgToken::FORTRAN_UNKNOWN, std::string value_string = "", SgUntypedType* type = NULL); 
         SgUntypedValueExpression(SgToken::ROSE_Fortran_Keywords statement_enum, std::string value_string, SgUntypedType* type); 

    protected:
         std::string p_value_string;
         SgUntypedType* p_type;
   };


// Class Definition for SgUntypedArrayReferenceExpression
class ROSE_DLL_API SgUntypedArrayReferenceExpression  : public SgUntypedExpression
   {
     public:

          typedef SgUntypedExpression base_node_type;

     public: 
         virtual ~SgUntypedArrayReferenceExpression();

     public: 
         SgUntypedArrayReferenceExpression(Sg_File_Info* startOfConstruct , SgToken::ROSE_Fortran_Keywords statement_enum = SgToken::FORTRAN_UNKNOWN); 
         SgUntypedArrayReferenceExpression(SgToken::ROSE_Fortran_Keywords statement_enum); 

    protected:

   };


// Class Definition for SgUntypedOtherExpression
class ROSE_DLL_API SgUntypedOtherExpression  : public SgUntypedExpression
   {
     public:

          typedef SgUntypedExpression base_node_type;

     public: 
         virtual ~SgUntypedOtherExpression();

     public: 
         SgUntypedOtherExpression(Sg_File_Info* startOfConstruct , SgToken::ROSE_Fortran_Keywords statement_enum = SgToken::FORTRAN_UNKNOWN); 
         SgUntypedOtherExpression(SgToken::ROSE_Fortran_Keywords statement_enum); 

    protected:

   };


// Class Definition for SgUntypedFunctionCallOrArrayReferenceExpression
class ROSE_DLL_API SgUntypedFunctionCallOrArrayReferenceExpression  : public SgUntypedExpression
   {
     public:

          typedef SgUntypedExpression base_node_type;

     public: 
         virtual ~SgUntypedFunctionCallOrArrayReferenceExpression();

     public: 
         SgUntypedFunctionCallOrArrayReferenceExpression(Sg_File_Info* startOfConstruct , SgToken::ROSE_Fortran_Keywords statement_enum = SgToken::FORTRAN_UNKNOWN); 
         SgUntypedFunctionCallOrArrayReferenceExpression(SgToken::ROSE_Fortran_Keywords statement_enum); 

    protected:

   };


// Class Definition for SgUntypedReferenceExpression
class ROSE_DLL_API SgUntypedReferenceExpression  : public SgUntypedExpression
   {
     public:
          typedef SgUntypedExpression base_node_type;

     public: 
         std::string get_name() const;
         void set_name(std::string name);


     public: 
         virtual ~SgUntypedReferenceExpression();

     public: 
         SgUntypedReferenceExpression(Sg_File_Info* startOfConstruct , SgToken::ROSE_Fortran_Keywords statement_enum = SgToken::FORTRAN_UNKNOWN, std::string name = ""); 
         SgUntypedReferenceExpression(SgToken::ROSE_Fortran_Keywords statement_enum, std::string name); 

    protected:
         std::string p_name;
          
   };


// Class Definition for SgUntypedStatement
class ROSE_DLL_API SgUntypedStatement  : public SgUntypedNode
   {
     public:

          typedef SgUntypedNode base_node_type;

     public: 
         std::string get_label_string() const;
         void set_label_string(std::string label_string);

     public: 
         SgToken::ROSE_Fortran_Keywords get_statement_enum() const;
         void set_statement_enum(SgToken::ROSE_Fortran_Keywords statement_enum);


     public: 
         virtual ~SgUntypedStatement();


     public: 
         SgUntypedStatement(Sg_File_Info* startOfConstruct ); 
         SgUntypedStatement(); 

    protected:
// Start of memberFunctionString
std::string p_label_string;
          
// End of memberFunctionString
// Start of memberFunctionString
SgToken::ROSE_Fortran_Keywords p_statement_enum;
          
   };


// Class Definition for SgUntypedDeclarationStatement
class ROSE_DLL_API SgUntypedDeclarationStatement  : public SgUntypedStatement
   {
     public:

          typedef SgUntypedStatement base_node_type;

     public: 
         virtual ~SgUntypedDeclarationStatement();


     public: 
         SgUntypedDeclarationStatement(Sg_File_Info* startOfConstruct ); 
         SgUntypedDeclarationStatement(); 

    protected:

   };


// Class Definition for SgUntypedImplicitDeclaration
class ROSE_DLL_API SgUntypedImplicitDeclaration  : public SgUntypedDeclarationStatement
   {
     public:

          typedef SgUntypedDeclarationStatement base_node_type;

     public: 
         virtual ~SgUntypedImplicitDeclaration();


     public: 
         SgUntypedImplicitDeclaration(Sg_File_Info* startOfConstruct ); 
         SgUntypedImplicitDeclaration(); 

    protected:

   };


// Class Definition for SgUntypedVariableDeclaration
class ROSE_DLL_API SgUntypedVariableDeclaration  : public SgUntypedDeclarationStatement
   {
     public:

          typedef SgUntypedDeclarationStatement base_node_type;

     public: 
         SgUntypedType* get_type() const;
         void set_type(SgUntypedType* type);

     public: 
         SgUntypedInitializedNameList* get_parameters() const;
         void set_parameters(SgUntypedInitializedNameList* parameters);

     public: 
         virtual ~SgUntypedVariableDeclaration();

     public: 
         SgUntypedVariableDeclaration(Sg_File_Info* startOfConstruct , SgUntypedType* type = NULL); 
         SgUntypedVariableDeclaration(SgUntypedType* type); 

    protected:
// Start of memberFunctionString
SgUntypedType* p_type;
          
// End of memberFunctionString
// Start of memberFunctionString
SgUntypedInitializedNameList* p_parameters;

   };


// Class Definition for SgUntypedFunctionDeclaration
class ROSE_DLL_API SgUntypedFunctionDeclaration  : public SgUntypedDeclarationStatement
   {
     public:

          typedef SgUntypedDeclarationStatement base_node_type;

     public: 
         std::string get_name() const;
         void set_name(std::string name);

     public: 
         SgUntypedInitializedNameList* get_parameters() const;
         void set_parameters(SgUntypedInitializedNameList* parameters);

     public: 
         SgUntypedType* get_type() const;
         void set_type(SgUntypedType* type);

     public: 
         SgUntypedFunctionScope* get_scope() const;
         void set_scope(SgUntypedFunctionScope* scope);

     public: 
         SgUntypedNamedStatement* get_end_statement() const;
         void set_end_statement(SgUntypedNamedStatement* end_statement);

     public: 
         virtual ~SgUntypedFunctionDeclaration();

     public: 
         SgUntypedFunctionDeclaration(Sg_File_Info* startOfConstruct , std::string name = ""); 
         SgUntypedFunctionDeclaration(std::string name); 

    protected:
         //TODO-DQ-2014.3.7 need end_statement
         SgUntypedNamedStatement* p_end_statement;

         std::string p_name;
         SgUntypedInitializedNameList* p_parameters;
         SgUntypedType* p_type;
         SgUntypedFunctionScope* p_scope;
   };


// Class Definition for SgUntypedProgramHeaderDeclaration
class ROSE_DLL_API SgUntypedProgramHeaderDeclaration  : public SgUntypedFunctionDeclaration
   {
     public:

          virtual VariantT variantT() const; // MS: new variant used in tree traversal
          enum { static_variant = V_SgUntypedProgramHeaderDeclaration };

          ROSE_DLL_API friend       SgUntypedProgramHeaderDeclaration* isSgUntypedProgramHeaderDeclaration(       SgNode * s );
          ROSE_DLL_API friend const SgUntypedProgramHeaderDeclaration* isSgUntypedProgramHeaderDeclaration( const SgNode * s );

          typedef SgUntypedFunctionDeclaration base_node_type;

     public: 
         virtual ~SgUntypedProgramHeaderDeclaration();

     public: 
         SgUntypedProgramHeaderDeclaration(Sg_File_Info* startOfConstruct , std::string name = ""); 
         SgUntypedProgramHeaderDeclaration(std::string name); 

    protected:

   };


// Class Definition for SgUntypedSubroutineDeclaration
class ROSE_DLL_API SgUntypedSubroutineDeclaration  : public SgUntypedFunctionDeclaration
   {
     public:

          typedef SgUntypedFunctionDeclaration base_node_type;

     public: 
         virtual ~SgUntypedSubroutineDeclaration();


     public: 
         SgUntypedSubroutineDeclaration(Sg_File_Info* startOfConstruct , std::string name = ""); 
         SgUntypedSubroutineDeclaration(std::string name); 

    protected:

   };


// Class Definition for SgUntypedModuleDeclaration
class ROSE_DLL_API SgUntypedModuleDeclaration  : public SgUntypedDeclarationStatement
   {
     public:

          typedef SgUntypedDeclarationStatement base_node_type;

     public: 
         SgUntypedModuleScope* get_scope() const;
         void set_scope(SgUntypedModuleScope* scope);


     public: 
         virtual ~SgUntypedModuleDeclaration();


     public: 
         SgUntypedModuleDeclaration(Sg_File_Info* startOfConstruct ); 
         SgUntypedModuleDeclaration(); 

    protected:
// Start of memberFunctionString
SgUntypedModuleScope* p_scope;
          
   };


// Class Definition for SgUntypedAssignmentStatement
class ROSE_DLL_API SgUntypedAssignmentStatement  : public SgUntypedStatement
   {
     public:

          typedef SgUntypedStatement base_node_type;

     public: 
         SgUntypedExpression* get_lhs_operand() const;
         void set_lhs_operand(SgUntypedExpression* lhs_operand);

     public: 
         SgUntypedExpression* get_rhs_operand() const;
         void set_rhs_operand(SgUntypedExpression* rhs_operand);


     public: 
         virtual ~SgUntypedAssignmentStatement();


     public: 
         SgUntypedAssignmentStatement(Sg_File_Info* startOfConstruct , SgUntypedExpression* lhs_operand = NULL, SgUntypedExpression* rhs_operand = NULL); 
         SgUntypedAssignmentStatement(SgUntypedExpression* lhs_operand, SgUntypedExpression* rhs_operand); 

    protected:
// Start of memberFunctionString
SgUntypedExpression* p_lhs_operand;
          
// End of memberFunctionString
// Start of memberFunctionString
SgUntypedExpression* p_rhs_operand;
          
   };


// Class Definition for SgUntypedFunctionCallStatement
class ROSE_DLL_API SgUntypedFunctionCallStatement  : public SgUntypedStatement
   {
     public:

          typedef SgUntypedStatement base_node_type;

     public: 
         virtual ~SgUntypedFunctionCallStatement();


     public: 
         SgUntypedFunctionCallStatement(Sg_File_Info* startOfConstruct ); 
         SgUntypedFunctionCallStatement(); 

    protected:

   };


// Class Definition for SgUntypedBlockStatement
class ROSE_DLL_API SgUntypedBlockStatement  : public SgUntypedStatement
   {
     public:
          typedef SgUntypedStatement base_node_type;

     public: 
         SgUntypedScope* get_scope() const;
         void set_scope(SgUntypedScope* scope);

     public: 
         virtual ~SgUntypedBlockStatement();


     public: 
         SgUntypedBlockStatement(Sg_File_Info* startOfConstruct ); 
         SgUntypedBlockStatement(); 

    protected:
// Start of memberFunctionString
SgUntypedScope* p_scope;
          
   };


// Class Definition for SgUntypedNamedStatement
class ROSE_DLL_API SgUntypedNamedStatement  : public SgUntypedStatement
   {
     public:

          typedef SgUntypedStatement base_node_type;

     public: 
         std::string get_statement_name() const;
         void set_statement_name(std::string statement_name);


     public: 
         virtual ~SgUntypedNamedStatement();


     public: 
         SgUntypedNamedStatement(Sg_File_Info* startOfConstruct , std::string statement_name = ""); 
         SgUntypedNamedStatement(std::string statement_name); 

    protected:
// Start of memberFunctionString
std::string p_statement_name;
          
   };


// Class Definition for SgUntypedOtherStatement
class ROSE_DLL_API SgUntypedOtherStatement  : public SgUntypedStatement
   {
     public:

          typedef SgUntypedStatement base_node_type;

     public: 
         virtual ~SgUntypedOtherStatement();


     public: 
         SgUntypedOtherStatement(Sg_File_Info* startOfConstruct ); 
         SgUntypedOtherStatement(); 

    protected:

   };


// Class Definition for SgUntypedScope
class ROSE_DLL_API SgUntypedScope  : public SgUntypedStatement
   {
     public:
          typedef SgUntypedStatement base_node_type;

     public: 
         SgUntypedDeclarationList* get_declaration_list() const;
         void set_declaration_list(SgUntypedDeclarationList* declaration_list);

     public: 
         SgUntypedStatementList* get_statement_list() const;
         void set_statement_list(SgUntypedStatementList* statement_list);

     public: 
         SgUntypedFunctionDeclarationList* get_function_list() const;
         void set_function_list(SgUntypedFunctionDeclarationList* function_list);

     public: 
         virtual ~SgUntypedScope();

     public: 
         SgUntypedScope(Sg_File_Info* startOfConstruct ); 
         SgUntypedScope(); 

    protected:
// Start of memberFunctionString
SgUntypedDeclarationList* p_declaration_list;
          
// End of memberFunctionString
// Start of memberFunctionString
SgUntypedStatementList* p_statement_list;
          
// End of memberFunctionString
// Start of memberFunctionString
SgUntypedFunctionDeclarationList* p_function_list;
          
   };


// Class Definition for SgUntypedFunctionScope
class ROSE_DLL_API SgUntypedFunctionScope  : public SgUntypedScope
   {
     public:

          typedef SgUntypedScope base_node_type;

     public: 
         virtual ~SgUntypedFunctionScope();

     public: 
         SgUntypedFunctionScope(Sg_File_Info* startOfConstruct ); 
         SgUntypedFunctionScope(); 

    protected:

   };


// Class Definition for SgUntypedModuleScope
class ROSE_DLL_API SgUntypedModuleScope  : public SgUntypedScope
   {
     public:
          typedef SgUntypedScope base_node_type;

     public: 
         virtual ~SgUntypedModuleScope();

     public: 
         SgUntypedModuleScope(Sg_File_Info* startOfConstruct ); 
         SgUntypedModuleScope(); 

    protected:

   };


// Class Definition for SgUntypedGlobalScope
class ROSE_DLL_API SgUntypedGlobalScope  : public SgUntypedScope
   {
     public:

          typedef SgUntypedScope base_node_type;

     public: 
         virtual ~SgUntypedGlobalScope();

     public: 
         SgUntypedGlobalScope(Sg_File_Info* startOfConstruct ); 
         SgUntypedGlobalScope(); 

    protected:

   };


// Class Definition for SgUntypedType
class ROSE_DLL_API SgUntypedType  : public SgUntypedNode
   {
     public:

          typedef SgUntypedNode base_node_type;

     public: 
         std::string get_type_name() const;
         void set_type_name(std::string type_name);

     public: 
         SgUntypedExpression* get_type_kind() const;
         void set_type_kind(SgUntypedExpression* type_kind);

     public: 
         bool get_has_kind() const;
         void set_has_kind(bool has_kind);

     public: 
         bool get_is_literal() const;
         void set_is_literal(bool is_literal);

     public: 
         bool get_is_class() const;
         void set_is_class(bool is_class);

     public: 
         bool get_is_intrinsic() const;
         void set_is_intrinsic(bool is_intrinsic);

     public: 
         bool get_is_constant() const;
         void set_is_constant(bool is_constant);

     public: 
         bool get_is_user_defined() const;
         void set_is_user_defined(bool is_user_defined);

     public: 
         SgUntypedExpression* get_char_length_expression() const;
         void set_char_length_expression(SgUntypedExpression* char_length_expression);

     public: 
         std::string get_char_length_string() const;
         void set_char_length_string(std::string char_length_string);

     public: 
         bool get_char_length_is_string() const;
         void set_char_length_is_string(bool char_length_is_string);

     public: 
         virtual ~SgUntypedType();

     public: 
         SgUntypedType(Sg_File_Info* startOfConstruct , std::string type_name = ""); 
         SgUntypedType(std::string type_name); 

    protected:
         std::string p_type_name;
         SgUntypedExpression* p_type_kind;
         bool p_has_kind;
         bool p_is_literal;
         bool p_is_class;
         bool p_is_intrinsic;
         bool p_is_constant;
         bool p_is_user_defined;
         SgUntypedExpression* p_char_length_expression;
         std::string p_char_length_string;
         bool p_char_length_is_string;
   };


// Class Definition for SgUntypedArrayType
class ROSE_DLL_API SgUntypedArrayType  : public SgUntypedType
   {
     public:
          typedef SgUntypedType base_node_type;

     public: 
         virtual ~SgUntypedArrayType();


     public: 
         SgUntypedArrayType(Sg_File_Info* startOfConstruct , std::string type_name = ""); 
         SgUntypedArrayType(std::string type_name); 

    protected:

   };


// Class Definition for SgUntypedAttribute
class ROSE_DLL_API SgUntypedAttribute  : public SgUntypedNode
   {
     public:

          typedef SgUntypedNode base_node_type;


// End of memberFunctionString

     public: 
         SgToken::ROSE_Fortran_Keywords get_type_name() const;
         void set_type_name(SgToken::ROSE_Fortran_Keywords type_name);

     public: 
         std::string get_named_attribute() const;
         void set_named_attribute(std::string named_attribute);


     public: 
         virtual ~SgUntypedAttribute();


     public: 
         SgUntypedAttribute(Sg_File_Info* startOfConstruct , SgToken::ROSE_Fortran_Keywords type_name = SgToken::FORTRAN_UNKNOWN); 
         SgUntypedAttribute(SgToken::ROSE_Fortran_Keywords type_name); 

    protected:
// Start of memberFunctionString
SgToken::ROSE_Fortran_Keywords p_type_name;
          
// End of memberFunctionString
// Start of memberFunctionString
std::string p_named_attribute;
          
   };


// Class Definition for SgUntypedInitializedName
class ROSE_DLL_API SgUntypedInitializedName  : public SgUntypedNode
   {
     public:
          typedef SgUntypedNode base_node_type;


// End of memberFunctionString

     public: 
         SgUntypedType* get_type() const;
         void set_type(SgUntypedType* type);

     public: 
         std::string get_name() const;
         void set_name(std::string name);


     public: 
         virtual ~SgUntypedInitializedName();


     public: 
         SgUntypedInitializedName(Sg_File_Info* startOfConstruct , SgUntypedType* type = NULL, std::string name = ""); 
         SgUntypedInitializedName(SgUntypedType* type, std::string name); 

    protected:
// Start of memberFunctionString
SgUntypedType* p_type;
          
// End of memberFunctionString
// Start of memberFunctionString
std::string p_name;
          
   };


// Class Definition for SgUntypedFile
class ROSE_DLL_API SgUntypedFile  : public SgUntypedNode
   {
     public:

          typedef SgUntypedNode base_node_type;

     public: 
         SgUntypedGlobalScope* get_scope() const;
         void set_scope(SgUntypedGlobalScope* scope);

     public: 
         virtual ~SgUntypedFile();

     public: 
         SgUntypedFile(Sg_File_Info* startOfConstruct ); 
         SgUntypedFile(); 

    protected:
// Start of memberFunctionString
SgUntypedGlobalScope* p_scope;
          
   };


// Class Definition for SgUntypedStatementList
class ROSE_DLL_API SgUntypedStatementList  : public SgUntypedNode
   {
     public:

          typedef SgUntypedNode base_node_type;

     public: 
         const SgUntypedStatementPtrList&  get_stmt_list() const;
         SgUntypedStatementPtrList& get_stmt_list(); 


     public: 
         virtual ~SgUntypedStatementList();


     public: 
         SgUntypedStatementList(Sg_File_Info* startOfConstruct ); 
         SgUntypedStatementList(); 

    protected:
// Start of memberFunctionString
SgUntypedStatementPtrList p_stmt_list;

   };


// Class Definition for SgUntypedDeclarationList
class ROSE_DLL_API SgUntypedDeclarationList  : public SgUntypedNode
   {
     public:

          typedef SgUntypedNode base_node_type;

     public: 
         const SgUntypedDeclarationStatementPtrList&  get_decl_list() const;
         SgUntypedDeclarationStatementPtrList& get_decl_list(); 


     public: 
         virtual ~SgUntypedDeclarationList();


     public: 
         SgUntypedDeclarationList(Sg_File_Info* startOfConstruct ); 
         SgUntypedDeclarationList(); 

    protected:
// Start of memberFunctionString
SgUntypedDeclarationStatementPtrList p_decl_list;
          
   };


// Class Definition for SgUntypedFunctionDeclarationList
class ROSE_DLL_API SgUntypedFunctionDeclarationList  : public SgUntypedNode
   {
     public:

          typedef SgUntypedNode base_node_type;

     public: 
         const SgUntypedFunctionDeclarationPtrList&  get_func_list() const;
         SgUntypedFunctionDeclarationPtrList& get_func_list(); 


     public: 
         virtual ~SgUntypedFunctionDeclarationList();

     public: 
         SgUntypedFunctionDeclarationList(Sg_File_Info* startOfConstruct ); 
         SgUntypedFunctionDeclarationList(); 

    protected:
// Start of memberFunctionString
SgUntypedFunctionDeclarationPtrList p_func_list;
          
   };


// Class Definition for SgUntypedInitializedNameList
class ROSE_DLL_API SgUntypedInitializedNameList  : public SgUntypedNode
   {
     public:

          typedef SgUntypedNode base_node_type;

     public: 
         const SgUntypedInitializedNamePtrList&  get_name_list() const;
         SgUntypedInitializedNamePtrList& get_name_list(); 


     public: 
         virtual ~SgUntypedInitializedNameList();


     public: 
         SgUntypedInitializedNameList(Sg_File_Info* startOfConstruct ); 
         SgUntypedInitializedNameList(); 

    protected:
// Start of memberFunctionString
SgUntypedInitializedNamePtrList p_name_list;
          
   };


#endif
