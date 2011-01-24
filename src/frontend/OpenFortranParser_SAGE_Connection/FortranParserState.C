#include "sage3basic.h"
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
