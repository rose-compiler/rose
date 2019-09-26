#include "sage3basic.h"
#include "FortranParserState.h"

AttributeRec* FortranParserState::DeclAttrSpec = new AttributeRec();
stack<FortranParserState*>  FortranParserState::statesStack;

// Constructor:
FortranParserState::FortranParserState()
  {
    clearStacks();
     statesStack.push(this);
     //printf("FortranParserState:: push in the stack currScopeStack = %p \n",&currScopeStack);
  }


// Destructor:
// Pop out the top of the stack, clean all the member stacks 
FortranParserState::~FortranParserState()
  {
//      delete varDeclAttrSpec;
      DeclAttrSpec->reset();
      statesStack.pop(); 
      //printf("FortranParserState:: pop out  the stack \n");
      clearStacks();
  }


void
FortranParserState::clearStacks() 
   {
     currScopeStack.clear();
     currExpressionStack.clear();
     currNodeStack.clear();
     currNameStack.clear();
     currTypeStack.clear();
     currBaseTypeStack.clear();
     currIntentSpecStack.clear();
     currAttributeSpecStack.clear();
     currInitializerStack.clear();
     currTypeKindStack.clear();
     currTypeParameterStack.clear();
     currLabelSymbolStack.clear();
     currIfStatementStack.clear();
     currActualArgumentNameStack.clear();
     currFunctionAttributeStack.clear();

  // DQ (9/11/2010): Added support for lists of unresolved functions.
     currUnresolvedFunctionsList.clear();

  // DQ (12/29/2010): Added support for R612 & R613 handling.
     currAstHasSelectionSubscriptStack.clear();

  // DQ (12/29/2010): Added support for R612 & R613 handling.
     currAstMultipartReferenceStack.clear();
   }


// DQ (7/30/2010): Added empty function to see if there are entries in the stack
bool emptyFortranStateStack()
   {
     return FortranParserState::empty();
   }

// Rasmussen (7/17/2019): Added function to see if there are entries in scope stack
bool emptyFortranScopeStack()
   {
     return astScopeStack.empty();
   }

void AttributeRec::setDeclAttrSpecs()
{
 // this sets public/private because parser grammar rules bundles public/private into access
    if (hasAccessSpec) setDeclarationAttributeSpec(declaration, accessType);

    if (isPublic) setDeclarationAttributeSpec(declaration, publicAttr);
    if (isPrivate) setDeclarationAttributeSpec(declaration, privateAttr);
    if (isAllocatable) setDeclarationAttributeSpec(declaration, allocatableAttr);
    if (isAsynchronous) setDeclarationAttributeSpec(declaration, asyncAttr);
    if (hasCodimension) setDeclarationAttributeSpec(declaration, codimAttr);
    if (isContiguous) setDeclarationAttributeSpec(declaration, contiguousAttr);
    if (hasDimension) setDeclarationAttributeSpec(declaration, dimAttr);
    if (isExternal) setDeclarationAttributeSpec(declaration, externalAttr);
    if (hasIntent) setDeclarationAttributeSpec(declaration, intentAttr);
//    if (isIn) setDeclarationAttributeSpec(varDeclaration, inAttr);
//    if (isOut) setDeclarationAttributeSpec(varDeclaration, outAttr);
    if (isIntrinsic) setDeclarationAttributeSpec(declaration, intrinsicAttr);
    if (hasLangBinding) setDeclarationAttributeSpec(declaration, bindingAttr);
    if (hasBindC) setDeclarationAttributeSpec(declaration, bindCAttr);
    if (isOptional) setDeclarationAttributeSpec(declaration, optionalAttr);
    if (hasParameter) setDeclarationAttributeSpec(declaration, parameterAttr);
    if (isPointer) setDeclarationAttributeSpec(declaration, pointerAttr);
    if (isCopointer) setDeclarationAttributeSpec(declaration, copointerAttr);
    if (isProtected) setDeclarationAttributeSpec(declaration, protectedAttr);
    if (isSave) setDeclarationAttributeSpec(declaration, saveAttr);
    if (isTarget) setDeclarationAttributeSpec(declaration, targetAttr);
    if (isCotarget) setDeclarationAttributeSpec(declaration, cotargetAttr);
    if (isValue) setDeclarationAttributeSpec(declaration, valueAttr);
    if (isVolatile) setDeclarationAttributeSpec(declaration, volatileAttr);
    if (isPass) setDeclarationAttributeSpec(declaration, passAttr);
    if (isNonOverridable) setDeclarationAttributeSpec(declaration, nonOverrideAttr);
    if (isDeferred) setDeclarationAttributeSpec(declaration, deferredAttr);
}

// The entity type follows the following ordering: pointer < copointer < codimension < dimension
SgType* AttributeRec::computeEntityType()
{
    if (charLenExpr)
    {
        // convert entityType to string type with char length and the existing type kind
        SgExpression* typeKind = entityType->get_type_kind();
        entityType = SgTypeString::createType(charLenExpr, typeKind);
    }
    if (hasDimension)
    {
      entityType = buildArrayType(dimExp);
    }
    if (hasCodimension)
    {
        entityType = buildArrayType(codimExp);
        entityType->set_isCoArray(true);
    }
    if (isCopointer)
    {
      entityType = new SgPointerType(entityType);
      entityType->set_isCoArray(true);             // a copointer is a pointer whose isCoArray flag is true.
    }
    if (isPointer)
    {
      entityType = new SgPointerType(entityType);
    }
    return entityType;
}

SgArrayType* AttributeRec::buildArrayType(SgExprListExp* dimInfo)
{
    SgExpression* sizeExpression = new SgNullExpression();  // this is the so-called index
    setSourcePosition(sizeExpression);
    SgArrayType* arrayType = new SgArrayType(entityType,sizeExpression);
    sizeExpression->set_parent(arrayType);
    arrayType->set_dim_info(dimInfo);
    dimInfo->set_parent(arrayType);
    arrayType->set_rank(dimInfo->get_expressions().size());
    return arrayType;
}

void AttributeRec::makeBaseTypeCoArray()
{
    if (!isSgArrayType(entityType))
    { // build an array of rank 0 to represent a covariable of a non-array type.
      SgExpression* sizeExpression = new SgNullExpression();
      setSourcePosition(sizeExpression);
      SgArrayType* arrayType = new SgArrayType(entityType,sizeExpression);
      sizeExpression->set_parent(arrayType);
      arrayType->set_dim_info(codimExp);
      codimExp->set_parent(arrayType);
      arrayType->set_rank(0);
      entityType = arrayType;
    }
    entityType->set_isCoArray(true);
}

SgDeclarationStatement* AttributeRec::getDeclaration() { return declaration; }
void AttributeRec::setDeclaration(SgDeclarationStatement* decl) { declaration = decl; }
SgType* AttributeRec::getBaseType() { return entityType; }
void AttributeRec::setBaseType(SgType* newType) { entityType = newType; }
SgExpression* AttributeRec::AttributeRec::getLenExpr () { return charLenExpr; }
void AttributeRec::setLenExpr(SgExpression* exp) { charLenExpr = exp; }
bool AttributeRec::getHasAccessSpec () { return hasAccessSpec; }
void AttributeRec::setHasAccessSpec (bool hasAccSpec) { hasAccessSpec = hasAccSpec; }
int AttributeRec::getAccessAttr() { return accessAttr; }
void AttributeRec::setAccessAttr(int accAttr) { accessAttr = accAttr; }
int AttributeRec::getAccessType() { return accessType; }
void AttributeRec::setAccessType(int accType) { accessType = accType; }
bool AttributeRec::getIsPublic() {return isPublic; }
void AttributeRec::setIsPublic(int isPub) { isPublic = isPub; }
int AttributeRec::getPublicAttr() {return publicAttr; }
void AttributeRec::setPublicAttr(int attr) { publicAttr = attr; }
bool AttributeRec::getIsPrivate() { return isPrivate; }
void AttributeRec::setIsPrivate(bool isPriv) { isPrivate = isPriv; }
int AttributeRec::getPrivateAttr() { return privateAttr; }
void AttributeRec::setPrivateAttr(int privAttr) { privateAttr = privAttr; }

// DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=return-type.
// bool AttributeRec::getIsAllocatable() { isAllocatable; }
bool AttributeRec::getIsAllocatable() { return isAllocatable; }

void AttributeRec::setIsAllocatable(bool isAlloc) { isAllocatable = isAlloc; }
int AttributeRec::getAllocatableAttr() { return allocatableAttr; }
void AttributeRec::setAllocatableAttr(int allocAttr) { allocatableAttr = allocAttr; }
bool AttributeRec::getIsAsynchronous() {return isAsynchronous; }
void AttributeRec::setIsAsynchronous(bool isAsync) { isAsynchronous = isAsync; }
int AttributeRec::getAsyncAttr() { return asyncAttr; }
void AttributeRec::setAsyncAttr(int attr) { asyncAttr = attr; }
bool AttributeRec::getHasCodimension() { return hasCodimension; }
void AttributeRec::setHasCodimension(int hasCodim) { hasCodimension = hasCodim; }
int AttributeRec::getCodimAtt() {return codimAttr; }
void AttributeRec::setCodimAttr(int attr) { codimAttr = attr; }
SgExprListExp* AttributeRec::getCodimExp() { return codimExp; }
void AttributeRec::setCodimExp(SgExprListExp* exp) { codimExp = exp; }
bool AttributeRec::getIsContiguous() { return isContiguous; }
void AttributeRec::setIsContiguous(bool isContig) { isContiguous = isContig; }
int AttributeRec::getContiguousAttr() { return contiguousAttr; }
void AttributeRec::setContiguousAttr(int attr) { contiguousAttr = attr; }
bool AttributeRec::getHasDimension() { return hasDimension; }
void AttributeRec::setHasDimension(bool hasDim) { hasDimension = hasDim; }
int AttributeRec::getDimAttr() { return dimAttr; }
void AttributeRec::setDimAttr(int attr) { dimAttr = attr; }
SgExprListExp* AttributeRec::getDimExp() {return dimExp; }
void AttributeRec::setDimExp(SgExprListExp* exp) { dimExp = exp; }
bool AttributeRec::getIsExternal() { return isExternal; }
void AttributeRec::setIsExternal(int isExt) { isExternal = isExt; }
int AttributeRec::getExternalAttr() { return externalAttr; }
void AttributeRec::setExternalAttr(int externAttr) { externalAttr = externAttr; }
bool AttributeRec::getHasIntent() {return hasIntent; }
void AttributeRec::setHasIntent(bool hasInt) { hasIntent = hasInt; }
int AttributeRec::getIntentAttr() { return intentAttr; }
void AttributeRec::setIntentAttr(int attr) { intentAttr = attr;}
int AttributeRec::getIntent() { return intent; }
void AttributeRec::setIntent(int inout) { intent = inout; }
bool AttributeRec::getIsIntrinsic() { return isIntrinsic; }
void AttributeRec::setIsIntrinsic(bool isIntrin) { isIntrinsic = isIntrin; }
int AttributeRec::getIntrinsicAttr() { return intrinsicAttr; }
void AttributeRec::setIntrinsicAttr(int attr) { intrinsicAttr = attr; }
bool AttributeRec::getHasLangBinding() { return hasLangBinding; }
void AttributeRec::setHasLangBinding(bool hasBind) { hasLangBinding = hasBind; }
int AttributeRec::getBindingAttr() {return bindingAttr; }
void AttributeRec::setBindingAttr(int attr) { bindingAttr = attr; }
bool AttributeRec::getHasBindC() { return hasBindC; }
void AttributeRec::setHasBindC(bool bindC) { hasBindC = bindC; }
int AttributeRec::getBindCAttr() {return bindCAttr; }
void AttributeRec::setBindCAttr(int attr) { bindCAttr = attr; }
bool AttributeRec::getIsOptional() { return isOptional; }
void AttributeRec::setIsOptional(bool isOption) { isOptional = isOption; }
int AttributeRec::getOptionalAttr() { return optionalAttr; }
void AttributeRec::setOptionalAttr(int attr) { optionalAttr = attr; }
bool AttributeRec::getHasParameter() { return hasParameter; }
void AttributeRec::setHasParameter(bool hasParam) { hasParameter = hasParam; }
int AttributeRec::getParameterAttr() { return parameterAttr; }
void AttributeRec::setParameterAttr(int attr) { parameterAttr = attr; }
bool AttributeRec::getIsPointer() {return isPointer; }
void AttributeRec::setIsPointer(bool isPoint) { isPointer = isPoint; }
int AttributeRec::getPointerAttr() { return pointerAttr; }
void AttributeRec::setPointerAttr(int attr) { pointerAttr = attr; }
bool AttributeRec::getIsCopointer() { return isCopointer; }
void AttributeRec::setIsCopointer(bool isCopoint) { isCopointer = isCopoint; }
int AttributeRec::getCopointrAttr() { return copointerAttr; }
void AttributeRec::setCopointerAttr(int attr) { copointerAttr = attr; }
bool AttributeRec::getIsProtected() { return isProtected; }
void AttributeRec::setIsProtected(bool isProt) { isProtected = isProt; }
int AttributeRec::getProtecedAttr() { return protectedAttr; }
void AttributeRec::setProtectedAttr(int attr) { protectedAttr = attr; }
bool AttributeRec::getIsSave() {return isSave; }
void AttributeRec::setIsSave(bool saveFlag) { isSave = saveFlag; }
int AttributeRec::getSaveAttr() {return saveAttr; }
void AttributeRec::setSaveAttr(int attr) { saveAttr = attr; }
bool AttributeRec::getIsTarget() { return isTarget; }
void AttributeRec::setIsTarget(bool targetFlag) { isTarget = targetFlag; }
int AttributeRec::getTargetAttr() {return targetAttr; }
void AttributeRec::setTargetAttr(int attr) { targetAttr = attr; }
bool AttributeRec::getIsCotarget() { return isCotarget; }
void AttributeRec::setIsCotarget(bool cotargetFlag) { isCotarget = cotargetFlag; }
int AttributeRec::getCotargetAttr() { return cotargetAttr; }
void AttributeRec::setCotargetAttr(int attr) {cotargetAttr = attr; }
bool AttributeRec::getIsValue() { return isValue; }
void AttributeRec::setIsValue(bool valueFlag) { isValue = valueFlag; }
int AttributeRec::getValueAttr() { return valueAttr; }
void AttributeRec::setValueAttr(int attr) { valueAttr = attr; }
bool AttributeRec::getIsVolatile() { return isVolatile; }
void AttributeRec::setIsVolatile(bool volatileFlag) { isVolatile = volatileFlag; }
int AttributeRec::getVolatileAttr() { return volatileAttr; }
void AttributeRec::setVolatileAttr(int attr) { volatileAttr = attr; }
bool AttributeRec::getIsPass() { return isPass; }
void AttributeRec::setIsPass (bool passFlag) { isPass = passFlag; }
int AttributeRec::getPassAttr() { return passAttr; }
void AttributeRec::setPassAttr(int attr) { passAttr = attr; }
bool AttributeRec::getIsNoPass() { return isNoPass; }
void AttributeRec::setIsNoPass(bool noPassFlag) { isNoPass = noPassFlag; }
int AttributeRec::getNoPassAttr() {return noPassAttr; }
void AttributeRec::setNoPassAttr(int attr) { noPassAttr = attr; }
bool AttributeRec::getIsNonOverridable() { return isNonOverridable; }
void AttributeRec::setIsNonOverridable(bool nonOverrideFlag) { isNonOverridable = nonOverrideFlag; }
int AttributeRec::getNonOverrideAttr() {return nonOverrideAttr; }
void AttributeRec::setNonOverrideAttr(int attr) { nonOverrideAttr = attr; }
bool AttributeRec::getIsDeferred() { return isDeferred; }
void AttributeRec::setIsDeferred(bool isDeferFlag) { isDeferred = isDeferFlag; }
int AttributeRec::getDeferredAttr() { return deferredAttr; }
void AttributeRec::setDeferredAttr(int attr) { deferredAttr = attr; }

void AttributeRec::reset()
{
    declaration = NULL; entityType = NULL; charLenExpr = NULL;
    hasAccessSpec = false; accessAttr = -1; accessType = -1;
    isPublic = false; publicAttr = -1; isPrivate = false; privateAttr = -1;
    isAllocatable = false; allocatableAttr = -1; isAsynchronous = false; asyncAttr = -1;
    hasCodimension = false; codimAttr = -1; codimExp = NULL; isContiguous = false; contiguousAttr = -1;
    hasDimension = false; dimAttr = -1; dimExp = NULL; isExternal = false; externalAttr = -1;
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

SgExprListExp* AttributeRec::buildDimensionInfo()
{
    // transfer the array/coarray spec from the astExpressionStack to this AttrSpec record.
    SgIntVal * intVal = isSgIntVal(astExpressionStack.front());
    ROSE_ASSERT(intVal);  // astExpressionStack.front() contains the number of array spec elements for the dimension info
    int count = intVal->get_value();
    delete intVal;  // must remove it from AST
    astExpressionStack.pop_front();
    processMultidimensionalSubscriptsIntoExpressionList(count);  // the dimension info is now on top of astExpressionStack
    SgExprListExp* dimInfo = isSgExprListExp(astExpressionStack.front());
    ROSE_ASSERT(dimInfo);
//    setDimExp(dimInfo);
//    setHasDimension(true);
    astExpressionStack.pop_front();
    return dimInfo;
}

