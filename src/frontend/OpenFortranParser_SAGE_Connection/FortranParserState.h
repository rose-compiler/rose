#ifndef __FORTRANPARSERSTATE_H_
#define __FORTRANPARSERSTATE_H_

// DQ (10/11/2010): We only require this include to support the type: AstNameListType.
#include "fortran_support.h"

#define DeclAttributes               (FortranParserState::getDeclAttrSpec())
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


namespace Rose {

struct CaseStmt {
   CaseStmt() { reset(); }

   SgExprListExp* key;
   bool isDefault;
   void reset() {key=NULL; isDefault=false;}
};

struct CaseValueRange {
   CaseValueRange() { reset(); }

   int numValues;
   bool hasSuffix;
   bool hasSuffixExpr;
   SgExpression* values[2];
   void reset() {numValues=0; hasSuffix=false; hasSuffixExpr=false; values[0]=NULL; values[1]=NULL;}
};

} // namespace Rose

// Class to record Fortran attributes in a type declaration statement or in a derived type statement
class AttributeRec
{
private:

    SgDeclarationStatement* declaration;
    SgType* entityType;
    SgExpression* charLenExpr;
    bool hasAccessSpec; int accessAttr; int accessType;
    bool isPublic; int publicAttr;
    bool isPrivate; int privateAttr;
    bool isAllocatable; int allocatableAttr;
    bool isAsynchronous; int asyncAttr;
    bool hasCodimension; int codimAttr; SgExprListExp* codimExp;
    bool isContiguous; int contiguousAttr;
    bool hasDimension; int dimAttr; SgExprListExp* dimExp;
    bool isExternal; int externalAttr;
    bool hasIntent; int intentAttr; int intent;
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

public:
    AttributeRec(): declaration(NULL), entityType(NULL), charLenExpr(NULL),
                hasAccessSpec(false), accessAttr(-1), accessType(-1),
                isPublic(false), publicAttr(-1), isPrivate(false), privateAttr(-1),
                isAllocatable(false), allocatableAttr(-1), isAsynchronous(false), asyncAttr(-1),
                hasCodimension(false), codimAttr(-1), codimExp(NULL), isContiguous(false), contiguousAttr(-1),
                hasDimension(false), dimAttr(-1), dimExp(NULL), isExternal(false), externalAttr(-1),
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

    // Setters and getters:
    SgDeclarationStatement* getDeclaration();
    void setDeclaration(SgDeclarationStatement* decl);
    SgType* getBaseType();
    void setBaseType(SgType* newType);
    SgExpression* getLenExpr ();
    void setLenExpr(SgExpression* exp);
    bool getHasAccessSpec ();
    void setHasAccessSpec (bool hasAccSpec);
    int getAccessAttr();
    void setAccessAttr(int accAttr);
    int getAccessType();
    void setAccessType(int accType);
    bool getIsPublic();
    void setIsPublic(int isPub);
    int getPublicAttr();
    void setPublicAttr(int attr);
    bool getIsPrivate();
    void setIsPrivate(bool isPriv);
    int getPrivateAttr();
    void setPrivateAttr(int privAttr);
    bool getIsAllocatable();
    void setIsAllocatable(bool isAlloc);
    int getAllocatableAttr();
    void setAllocatableAttr(int allocAttr);
    bool getIsAsynchronous();
    void setIsAsynchronous(bool isAsync);
    int getAsyncAttr();
    void setAsyncAttr(int attr);
    bool getHasCodimension();
    void setHasCodimension(int hasCodim);
    int getCodimAtt();
    void setCodimAttr(int attr);
    SgExprListExp* getCodimExp();
    void setCodimExp(SgExprListExp* exp);
    bool getIsContiguous() ;
    void setIsContiguous(bool isContig);
    int getContiguousAttr() ;
    void setContiguousAttr(int attr) ;
    bool getHasDimension();
    void setHasDimension(bool hasDim) ;
    int getDimAttr();
    void setDimAttr(int attr) ;
    SgExprListExp* getDimExp();
    void setDimExp(SgExprListExp* exp);
    bool getIsExternal() ;
    void setIsExternal(int isExt) ;
    int getExternalAttr();
    void setExternalAttr(int externAttr);
    bool getHasIntent();
    void setHasIntent(bool hasInt);
    int getIntentAttr();
    void setIntentAttr(int attr);
    int getIntent() ;
    void setIntent(int inout);
    bool getIsIntrinsic();
    void setIsIntrinsic(bool isIntrin) ;
    int getIntrinsicAttr() ;
    void setIntrinsicAttr(int attr);
    bool getHasLangBinding();
    void setHasLangBinding(bool hasBind);
    int getBindingAttr();
    void setBindingAttr(int attr);
    bool getHasBindC();
    void setHasBindC(bool bindC);
    int getBindCAttr();
    void setBindCAttr(int attr);
    bool getIsOptional();
    void setIsOptional(bool isOption) ;
    int getOptionalAttr();
    void setOptionalAttr(int attr);
    bool getHasParameter() ;
    void setHasParameter(bool hasParam);
    int getParameterAttr();
    void setParameterAttr(int attr);
    bool getIsPointer();
    void setIsPointer(bool isPoint);
    int getPointerAttr();
    void setPointerAttr(int attr);
    bool getIsCopointer();
    void setIsCopointer(bool isCopoint);
    int getCopointrAttr();
    void setCopointerAttr(int attr);
    bool getIsProtected();
    void setIsProtected(bool isProt);
    int getProtecedAttr() ;
    void setProtectedAttr(int attr) ;
    bool getIsSave() ;
    void setIsSave(bool saveFlag) ;
    int getSaveAttr() ;
    void setSaveAttr(int attr) ;
    bool getIsTarget() ;
    void setIsTarget(bool targetFlag) ;
    int getTargetAttr() ;
    void setTargetAttr(int attr) ;
    bool getIsCotarget() ;
    void setIsCotarget(bool cotargetFlag) ;
    int getCotargetAttr();
    void setCotargetAttr(int attr) ;
    bool getIsValue() ;
    void setIsValue(bool valueFlag) ;
    int getValueAttr() ;
    void setValueAttr(int attr) ;
    bool getIsVolatile() ;
    void setIsVolatile(bool volatileFlag) ;
    int getVolatileAttr();
    void setVolatileAttr(int attr) ;
    bool getIsPass() ;
    void setIsPass (bool passFlag) ;
    int getPassAttr();
    void setPassAttr(int attr) ;
    bool getIsNoPass() ;
    void setIsNoPass(bool noPassFlag) ;
    int getNoPassAttr() ;
    void setNoPassAttr(int attr);
    bool getIsNonOverridable() ;
    void setIsNonOverridable(bool nonOverrideFlag) ;
    int getNonOverrideAttr();
    void setNonOverrideAttr(int attr) ;
    bool getIsDeferred() ;
    void setIsDeferred(bool isDeferFlag);
    int getDeferredAttr() ;
    void setDeferredAttr(int attr) ;

    // Resets to initial states (as in the default constructor)
    void reset();

    // Sets all the existing attributes for varDeclaration.
    // Pre-condition: varDeclaration != NULL;
    void setDeclAttrSpecs();

    // Builds the appropriate SgExprListExp as the dimension/codimension info
    SgExprListExp* buildDimensionInfo();

    // Builds an SgArrayType object with the current entityType as the base type and with the given dim info
    // Pre-condition: dimInfo != NULL
    SgArrayType* buildArrayType(SgExprListExp* dimInfo);

    // Transforms entityType into a coarray with codimensionAttr as codim_info
    // Pre-condition: codimensionAttr != NULL
    void makeBaseTypeCoArray();

    // Computes the type of the entity_decl with the computed based type and accumulated attributes
    SgType* computeEntityType();
};

class FortranParserState
   {
  // This class supports a stack of the collections of stacks required to represent 
  // the state within the translation of the OFP actions to build the ROSE AST.

     private:
       static stack<FortranParserState*>  statesStack;

       // The declared attributes on the left-hand-side of the "::" in a type declaration statement or in a derived type statement
       static  AttributeRec* DeclAttrSpec;

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

       static AttributeRec& getDeclAttrSpec()
       {
           return *DeclAttrSpec;
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
    //   push "this" object of FortranParserState onto the "statesStack"
       FortranParserState();

       
    // Destructor: 
    //   Pop out the stack, clean all the member stacks 
       ~FortranParserState();

   };

#endif /*__FORTRANPARSERSTATE_H_*/
