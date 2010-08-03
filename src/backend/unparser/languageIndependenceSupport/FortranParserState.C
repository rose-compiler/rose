
#include "FortranParserState.h"

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
  }

