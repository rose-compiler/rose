#include "rose.h"
#include "RtedSymbols.h"

#define LOOKUP( __x ) \
    {\
      SgFunctionSymbol* func = isSgScopeStatement(n)->lookup_function_symbol("RuntimeSystem_" #__x); \
      if( isSgFunctionSymbol( func )) {\
        __x = isSgFunctionSymbol( func );\
        cerr << "Found MemberName : " #__x << endl;\
      }\
    }



using namespace std;

void RtedSymbols::visit(SgNode* n) {

  if (n == NULL)
    return;
  // *********************** NEEDED to insert RuntimeSystem runtimeSystem = new RuntimeSystem() ***************

  // ******************** DETECT Member functions in RuntimeSystem.h *************************************************************

  if (isSgScopeStatement(n)) {

    LOOKUP( roseCreateArray );
    LOOKUP( roseArrayAccess );
    LOOKUP( roseCheckpoint );
    LOOKUP( roseFunctionCall );
    LOOKUP( roseAssertFunctionSignature );
    LOOKUP( roseConfirmFunctionSignature );
    LOOKUP( roseConvertIntToString );
    LOOKUP( roseCreateVariable );
    LOOKUP( roseInitVariable );
    LOOKUP( roseMovePointer );
    LOOKUP( roseEnterScope );
    LOOKUP( roseExitScope);
    LOOKUP( roseAccessVariable );
    LOOKUP( roseIOFunctionCall );
    LOOKUP( roseRegisterTypeCall );
    LOOKUP( roseFreeMemory );
    LOOKUP( roseReallocateMemory );
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

