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
    LOOKUP( roseFreeMemory, func14);
    LOOKUP( roseReallocateMemory, func15);
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

