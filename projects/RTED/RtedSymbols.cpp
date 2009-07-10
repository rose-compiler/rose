#include "rose.h"
#include "RtedSymbols.h"

#define LOOKUP(__x,__v) \
    SgFunctionSymbol* __v = isSgScopeStatement(n)->lookup_function_symbol("RuntimeSystem_" #__x); \
    if( isSgFunctionSymbol( __v)) {\
      __x = isSgFunctionSymbol( __v);\
      cerr << "Found MemberName : " #__x << endl;\
    }


using namespace std;

void RtedSymbols::visit(SgNode* n) {

  if (n == NULL)
    return;
  // *********************** NEEDED to insert RuntimeSystem runtimeSystem = new RuntimeSystem() ***************

  // ******************** DETECT Member functions in RuntimeSystem.h *************************************************************

  if (isSgScopeStatement(n)) {

    LOOKUP( roseCreateArray, func1);
    LOOKUP( roseArrayAccess, func2);
    LOOKUP( roseRtedClose, func3);
    LOOKUP( roseFunctionCall, func4);
    LOOKUP( roseConvertIntToString, func5);
    LOOKUP( roseCallStack, func6);
    LOOKUP( roseCreateVariable, func7);
    LOOKUP( roseInitVariable, func8);
    LOOKUP( roseEnterScope, func9);
    LOOKUP( roseExitScope, fund10); 
    LOOKUP( roseAccessVariable, fund11);
    LOOKUP( roseIOFunctionCall, func12);
    LOOKUP( roseRegisterTypeCall, func13);

  }


  // ******************** DETECT Member functions in RuntimeSystem.h *************************************************************

}

