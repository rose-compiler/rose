#ifndef __FORTRANPARSERSTATE_H_
#define __FORTRANPARSERSTATE_H_

// DQ (10/11/2010): We only require this include to support the type: AstNameListType.
#include "fortran_support.h"

#define VarDeclAttrSpec               (FortranParserState::getVarDeclAttrSpec())
#define astScopeStack                 (*(FortranParserState::getCurrentScopeStack()))
#define astExpressionStack            (*(FortranParserState::getCurrentExpressionStack()))
#define astNodeStack                  (*(FortranParserState::getCurrentNodeStack()))
#define astNameStack                  (*(FortranParserState::getCurrentNameStack()))
#define astTypeStack                  (*(FortranParserState::getCurrentTypeStack()))
#define astBaseTypeStack              (*(FortranParserState::getCurrentBaseTypeStack()))
#define astIntentSpecStack            (*(FortranParserState::getCurrentIntentSpecStack()))
#define astAttributeSpecStack         (*(FortranParserState::getCurrentAttributeSpecStack()))
#define astInitializerStack           (*(FortranParserState::getCurrentInitializerStack()))
#define astTypeKindStack              (*(FortranParserState::getCurrentTypeKindStack()))
#define astTypeParameterStack         (*(FortranParserState::getCurrentTypeParameterStack()))
#define astLabelSymbolStack           (*(FortranParserState::getCurrentLabelSymbolStack()))
#define astIfStatementStack           (*(FortranParserState::getCurrentIfStatementStack()))
#define astActualArgumentNameStack    (*(FortranParserState::getCurrentActualArgumentNameStack()))
#define astFunctionAttributeStack     (*(FortranParserState::getCurrentFunctionAttributeStack()))

// DQ (12/29/2010): Note that this stack is not cleared in the FortranParserState::clearStacks() function (is this an error).
#define astIncludeStack               (*(FortranParserState::getCurrentIncludeStack()))

// DQ (9/11/2010): Added support for lists of unresolved functions.
#define astUnresolvedFunctionsList    (*(FortranParserState::getCurrentUnresolvedFunctionsList()))

// DQ (12/29/2010): Added support for R612 & R613 handling.
#define astHasSelectionSubscriptStack (*(FortranParserState::getCurrentHasSelectionSubscriptStack()))
#define astMultipartReferenceStack (*(FortranParserState::getCurrentMultipartReferenceStack()))


using std::string;
using std::map;
using std::stack;
using std::list;
using std::vector;

// DXN (05/04/2011): class to represent Fortran attributes
class AttrSpec
{
public:

    SgVariableDeclaration* varDeclaration;
    SgType* baseType;
    SgExpression* lenExpr;
    bool hasAccessSpec; int accessAttr; int accessType;
    bool isPublic; int publicAttr;
    bool isPrivate; int privateAttr;
    bool isAllocatable; int allocatableAttr;
    bool isAsynchronous; int asyncAttr;
    bool hasCodimension; SgExprListExp* codimensionAttr;  // for the entity on the left hand side of :: in each type declaration
    bool isContiguous; int contiguousAttr;
    bool hasDimension; SgExprListExp* dimensionAttr;  // for the entity on the left hand side of :: in each type declaration
    bool isExternal; int externalAttr;
    bool hasIntent; int intentAttr; int intent;
//    bool isIn; int inAttr;
//    bool isOut; int outAttr;
    bool isIntrinsic; int intrinsicAttr;
    bool hasLangBinding;  int bindingAttr;
    bool hasBindC; int bindCAttr;
    bool isOptional; int optionalAttr;
    bool hasParameter; int parameterAttr;
    bool isPointer; int pointerAttr;
    bool isCopointer; int copointerAttr;
    bool isProtected; int protectedAttr;
    bool isSave; int saveAttr;
    bool isTarget; int targetAttr;
    bool isCotarget; int cotargetAttr;
    bool isValue; int valueAttr;
    bool isVolatile; int volatileAttr;
    bool isPass; int passAttr;
    bool isNoPass; int noPassAttr;
    bool isNonOverridable; int nonOverrideAttr;
    bool isDeferred; int deferredAttr;
    // SgExpression* kindExp; // TODO

    AttrSpec(): varDeclaration(NULL), baseType(NULL), lenExpr(NULL),
                hasAccessSpec(false), accessAttr(-1), accessType(-1),
                isPublic(false), publicAttr(-1), isPrivate(false), privateAttr(-1),
                isAllocatable(false), allocatableAttr(-1), isAsynchronous(false), asyncAttr(-1),
                hasCodimension(false), codimensionAttr(NULL), isContiguous(false), contiguousAttr(-1),
                hasDimension(false), dimensionAttr(NULL), isExternal(false), externalAttr(-1),
                hasIntent(false), intentAttr(-1), intent(-1), isIntrinsic(false), intrinsicAttr(-1),
                hasLangBinding(false), bindingAttr(-1), hasBindC(false), bindCAttr(-1),
                isOptional(false), optionalAttr(-1), hasParameter(false), parameterAttr(-1),
                isPointer(false), pointerAttr(-1), isCopointer(false), copointerAttr(-1),
                isProtected(false), protectedAttr(-1), isSave(false), saveAttr(-1),
                isTarget(false), targetAttr(-1), isCotarget(false), cotargetAttr(-1),
                isValue(false), valueAttr(-1), isVolatile(false), volatileAttr(-1),
                isPass(false), passAttr(-1), isNoPass(false), noPassAttr(-1),
                isNonOverridable(false), nonOverrideAttr(-1), isDeferred(false), deferredAttr(-1)
    { }

    void reset()
    {
        varDeclaration = NULL; baseType = NULL; lenExpr = NULL;
        hasAccessSpec = false; accessAttr = -1; accessType = -1;
        isPublic = false; publicAttr = -1; isPrivate = false; privateAttr = -1;
        isAllocatable = false; allocatableAttr = -1; isAsynchronous = false; asyncAttr = -1;
        hasCodimension = false; codimensionAttr = NULL; isContiguous = false; contiguousAttr = -1;
        hasDimension = false; dimensionAttr = NULL; isExternal = false; externalAttr = -1;
        hasIntent = false; intentAttr = -1; intent = -1; isIntrinsic = false; intrinsicAttr = -1;
        hasLangBinding = false; bindingAttr = -1; hasBindC = false; bindCAttr = -1;
        isOptional = false; optionalAttr = -1; hasParameter = false; parameterAttr = -1;
        isPointer = false; pointerAttr = -1; isCopointer = false; copointerAttr = -1;
        isProtected = false; protectedAttr = -1; isSave = false; saveAttr = -1;
        isTarget = false; targetAttr = -1; isCotarget = false; cotargetAttr = -1;
        isValue = false; valueAttr = -1; isVolatile = false; volatileAttr = -1;
        isPass = false; passAttr = -1; isNoPass = false; noPassAttr = -1;
        isNonOverridable = false; nonOverrideAttr = -1; isDeferred = false; deferredAttr = -1;
    }

    // Sets all the existing attributes for varDeclaration.
    // Pre-condition: varDeclaration != NULL;
    void setDeclAttrSpecs();

    SgType* computeEntityType();

    // Builds a SgArrayType object with dimensionAttr as dim_info
    // Pre-condition: dimensionAttr != NULL
    SgArrayType* buildArrayType();

    // Transforms baseType into a coarray with codimensionAttr as codim_info
    // Pre-condition: codimensionAttr != NULL
    void makeBaseTypeCoArray();
};

class FortranParserState
   {
  // This class supports a stack of the collections of stacks required to represent 
  // the state within the translation of the OFP actions to build the ROSE AST.

     private:
       static stack<FortranParserState*>  statesStack;

       // DXN (05/12/2011): to record the attributes on the left-hand-side of the :: in a variable declaration
       static  AttrSpec* varDeclAttrSpec;

     private:
       list<SgScopeStatement*> currScopeStack;
       list<SgExpression*>     currExpressionStack;
       list<SgNode*>           currNodeStack;
       AstNameListType         currNameStack;
       list<SgType*>           currTypeStack;
       list<SgType*>           currBaseTypeStack;
       list<int>               currIntentSpecStack;
       list<int>               currAttributeSpecStack;
       list<SgExpression*>     currInitializerStack;
       list<SgExpression*>     currTypeKindStack;
       list<SgExpression*>     currTypeParameterStack;
       list<SgLabelSymbol*>    currLabelSymbolStack;
       list<SgIfStmt*>         currIfStatementStack;
       AstNameListType         currActualArgumentNameStack;
       AstNameListType         currFunctionAttributeStack;
       vector<string>          currAstIncludeStack;

    // DQ (9/11/2010): Added support for lists of unresolved functions.
       list<SgStatement*>      currUnresolvedFunctionsList;

    // DQ (12/29/2010): Added support for R612 & R613 handling.
       list<bool>              currAstHasSelectionSubscriptStack;

    // DQ (12/29/2010): Added support for R612 & R613 handling.
       list<MultipartReferenceType> currAstMultipartReferenceStack;

       void clearStacks();

     public:

       static AttrSpec& getVarDeclAttrSpec()
       {
           return *varDeclAttrSpec;
       }

     // DQ (7/30/2010): Added empty function to if there are entries in the stack
        static bool empty()
          { return statesStack.empty(); }

    // DQ (7/30/2010): Added assertions to all the functions below.
    // following functions will called by macro in ofp-rose connection files
       static  list<SgScopeStatement*>  *getCurrentScopeStack()
          {
            ROSE_ASSERT(statesStack.empty() == false);
            return &(statesStack.top()->currScopeStack);
          }

       static  list<SgExpression*>      *getCurrentExpressionStack()
          {
            ROSE_ASSERT(statesStack.empty() == false);
            return &(statesStack.top()->currExpressionStack);
          }

       static  list<SgNode*>            *getCurrentNodeStack()
          { 
            ROSE_ASSERT(statesStack.empty() == false);
            return &(statesStack.top()->currNodeStack);
          }

       static  AstNameListType          *getCurrentNameStack()
          {
            ROSE_ASSERT(statesStack.empty() == false);
            return &(statesStack.top()->currNameStack);
          }

       static  list<SgType*>            *getCurrentTypeStack()
          {
            ROSE_ASSERT(statesStack.empty() == false);
            return &(statesStack.top()->currTypeStack);
          }

       static  list<SgType*>            *getCurrentBaseTypeStack()
          {
            ROSE_ASSERT(statesStack.empty() == false);
            return &(statesStack.top()->currBaseTypeStack);
          }

       static  list<int>                *getCurrentIntentSpecStack()
          {
            ROSE_ASSERT(statesStack.empty() == false);
            return &(statesStack.top()->currIntentSpecStack);
          }

       static  list<int>                *getCurrentAttributeSpecStack()
          {
            ROSE_ASSERT(statesStack.empty() == false);
            return &(statesStack.top()->currAttributeSpecStack);
          }

       static  list<SgExpression*>      *getCurrentInitializerStack()
          {
            ROSE_ASSERT(statesStack.empty() == false);
            return &(statesStack.top()->currInitializerStack);
          }

       static  list<SgExpression*>      *getCurrentTypeKindStack()
          {
            ROSE_ASSERT(statesStack.empty() == false);
            return &(statesStack.top()->currTypeKindStack);
          }

       static  list<SgExpression*>      *getCurrentTypeParameterStack()
          {
            ROSE_ASSERT(statesStack.empty() == false);
            return &(statesStack.top()->currTypeParameterStack);
          }

       static  list<SgLabelSymbol*>     *getCurrentLabelSymbolStack()
          {
            ROSE_ASSERT(statesStack.empty() == false);
            return &(statesStack.top()->currLabelSymbolStack);
          }

       static  list<SgIfStmt*>          *getCurrentIfStatementStack()
          {
            ROSE_ASSERT(statesStack.empty() == false);
            return &(statesStack.top()->currIfStatementStack);
          }

       static  AstNameListType          *getCurrentActualArgumentNameStack()
          {
            ROSE_ASSERT(statesStack.empty() == false);
            return &(statesStack.top()->currActualArgumentNameStack);
          }

       static  AstNameListType          *getCurrentFunctionAttributeStack()
          {
            ROSE_ASSERT(statesStack.empty() == false);
            return &(statesStack.top()->currFunctionAttributeStack);
          }

       static  vector<string>          *getCurrentIncludeStack()
          {
            ROSE_ASSERT(statesStack.empty() == false);
            return &(statesStack.top()->currAstIncludeStack);
          }

    // DQ (9/11/2010): Added support for lists of unresolved functions.
       static  list<SgStatement*>      *getCurrentUnresolvedFunctionsList()
        {
          ROSE_ASSERT(statesStack.empty() == false);
          return &(statesStack.top()->currUnresolvedFunctionsList);
        }

       static  list<bool>              *getCurrentHasSelectionSubscriptStack()
          {
            ROSE_ASSERT(statesStack.empty() == false);
            return &(statesStack.top()->currAstHasSelectionSubscriptStack);
          }


       static  list<MultipartReferenceType> *getCurrentMultipartReferenceStack()
          {
            ROSE_ASSERT(statesStack.empty() == false);
            return &(statesStack.top()->currAstMultipartReferenceStack);
          }

    // Constructor:
    //   push "this" object of FortranParserState into the "statesStack"
       FortranParserState();

       
    // Destructor: 
    //   Pop out the stack, clean all the member stacks 
       ~FortranParserState();
   };

#endif /*__FORTRANPARSERSTATE_H_*/
