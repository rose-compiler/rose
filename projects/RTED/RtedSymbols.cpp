#include "rose.h"
#include "RtedSymbols.h"

#define LOOKUP(__x,__v, __z) \
    SgFunctionSymbol* __v = isSgScopeStatement(n)->lookup_function_symbol("RuntimeSystem_" #__x); \
    if( isSgFunctionSymbol( __v)) {\
      __z = isSgFunctionSymbol( __v);\
      cerr << "Found MemberName : " #__x << endl;\
    } 


using namespace std;

void RtedSymbols::visit(SgNode* n) {

  if (n == NULL)
    return;
  // *********************** NEEDED to insert RuntimeSystem runtimeSystem = new RuntimeSystem() ***************

  // ******************** DETECT Member functions in RuntimeSystem.h *************************************************************

  if (isSgScopeStatement(n)) {

    LOOKUP( roseCreateArray, func1,roseCreateArray);
    LOOKUP( roseArrayAccess, func2,roseArrayAccess);
    LOOKUP( roseRtedClose, func3,roseRtedClose);
    LOOKUP( roseFunctionCall, func4,roseFunctionCall);
    LOOKUP( roseConvertIntToString, func5,roseConvertIntToString);
    LOOKUP( roseCallStack, func6,roseCallStack);
    LOOKUP( roseCreateVariable, func7,roseCreateVariable);
    LOOKUP( roseInitVariable, func8,roseInitVariable);
    LOOKUP( roseEnterScope, func9,roseEnterScope);
    LOOKUP( roseExitScope, fund10,roseExitScope);
    LOOKUP( roseAccessVariable, fund11,roseAccessVariable);
    LOOKUP( roseIOFunctionCall, func12,roseIOFunctionCall);
    LOOKUP( roseRegisterTypeCall, func13,roseRegisterTypeCall);


  }

  if (isSgTypedefDeclaration(n)) {
  	SgTypedefDeclaration* typed = isSgTypedefDeclaration(n);
		string name = typed->get_name().str();
		if (name=="size_t") {
			SgType* baseType = typed->get_base_type ();
			size_t_member=baseType;
			cerr << "Found Type : " << name << endl;
		}
  }

  // ******************** DETECT Member functions in RuntimeSystem.h *************************************************************

}

