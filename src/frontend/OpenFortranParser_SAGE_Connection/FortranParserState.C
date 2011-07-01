#include "sage3basic.h"
#include "FortranParserState.h"

AttrSpec* FortranParserState::varDeclAttrSpec = new AttrSpec();
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
      varDeclAttrSpec->reset();
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


// DQ (7/30/2010): Added empty function to if there are entries in the stack
bool emptyFortranStateStack()
   {
     return FortranParserState::empty();
   }

void AttrSpec::setDeclAttrSpecs()
{
    if (hasAccessSpec) setDeclarationAttributeSpec(varDeclaration, accessType);
    if (isPublic) setDeclarationAttributeSpec(varDeclaration, publicAttr);
    if (isPrivate) setDeclarationAttributeSpec(varDeclaration, privateAttr);
    if (isAllocatable) setDeclarationAttributeSpec(varDeclaration, allocatableAttr);
    if (isAsynchronous) setDeclarationAttributeSpec(varDeclaration, asyncAttr);
    if (isContiguous) setDeclarationAttributeSpec(varDeclaration, contiguousAttr);
    if (isExternal) setDeclarationAttributeSpec(varDeclaration, externalAttr);
    if (hasIntent) setDeclarationAttributeSpec(varDeclaration, intentAttr);
//    if (isIn) setDeclarationAttributeSpec(varDeclaration, inAttr);
//    if (isOut) setDeclarationAttributeSpec(varDeclaration, outAttr);
    if (isIntrinsic) setDeclarationAttributeSpec(varDeclaration, intrinsicAttr);
    if (hasLangBinding) setDeclarationAttributeSpec(varDeclaration, bindingAttr);
    if (hasBindC) setDeclarationAttributeSpec(varDeclaration, bindCAttr);
    if (isOptional) setDeclarationAttributeSpec(varDeclaration, optionalAttr);
    if (hasParameter) setDeclarationAttributeSpec(varDeclaration, parameterAttr);
    if (isPointer) setDeclarationAttributeSpec(varDeclaration, pointerAttr);
    if (isCopointer) setDeclarationAttributeSpec(varDeclaration, copointerAttr);
    if (isProtected) setDeclarationAttributeSpec(varDeclaration, protectedAttr);
    if (isSave) setDeclarationAttributeSpec(varDeclaration, saveAttr);
    if (isTarget) setDeclarationAttributeSpec(varDeclaration, targetAttr);
    if (isCotarget) setDeclarationAttributeSpec(varDeclaration, cotargetAttr);
    if (isValue) setDeclarationAttributeSpec(varDeclaration, valueAttr);
    if (isVolatile) setDeclarationAttributeSpec(varDeclaration, volatileAttr);
    if (isPass) setDeclarationAttributeSpec(varDeclaration, passAttr);
    if (isNonOverridable) setDeclarationAttributeSpec(varDeclaration, nonOverrideAttr);
    if (isDeferred) setDeclarationAttributeSpec(varDeclaration, deferredAttr);
}

// The entity type follows the following ordering: pointer < copointer < codimension < dimension
SgType* AttrSpec::computeEntityType()
{
    if (lenExpr)
    {
        // TODO: convert baseType to string type with char length
        baseType = SgTypeString::createType(lenExpr);
    }
    if (hasDimension)
    {
      baseType = buildArrayType();
    }
    if (hasCodimension)
    {
      makeBaseTypeCoArray();
    }
    if (isCopointer)
    {
      baseType = new SgPointerType(baseType);
      baseType->set_isCoArray(true);             // a copointer is a pointer whose isCoArray flag is true.
    }
    if (isPointer)
    {
      baseType = new SgPointerType(baseType);
    }
    return baseType; // TODO
}

SgArrayType* AttrSpec::buildArrayType()
{
    SgExpression* sizeExpression = new SgNullExpression();  // this is the so-called index
    setSourcePosition(sizeExpression);
    SgArrayType* arrayType = new SgArrayType(baseType,sizeExpression);
    sizeExpression->set_parent(arrayType);
    arrayType->set_dim_info(dimensionAttr);
    dimensionAttr->set_parent(arrayType);
    arrayType->set_rank(dimensionAttr->get_expressions().size());
    return arrayType;
}

void AttrSpec::makeBaseTypeCoArray()
{
    if (!isSgArrayType(baseType))
    { // build an array of rank 0 to represent a covariable of a non-array type.
      SgExpression* sizeExpression = new SgNullExpression();
      setSourcePosition(sizeExpression);
      SgArrayType* arrayType = new SgArrayType(baseType,sizeExpression);
      sizeExpression->set_parent(arrayType);
      arrayType->set_dim_info(codimensionAttr);
      codimensionAttr->set_parent(arrayType);
      arrayType->set_rank(0);
      baseType = arrayType;
    }
    baseType->set_isCoArray(true);
}

