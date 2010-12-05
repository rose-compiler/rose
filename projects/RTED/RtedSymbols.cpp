#include "rose.h"
#include "RtedSymbols.h"

bool RTEDDEBUG() {return false;}

#define LOOKUP( __x ) \
    {\
      SgFunctionSymbol* func = isSgScopeStatement(n)->lookup_function_symbol("RuntimeSystem_" #__x); \
      if( isSgFunctionSymbol( func )) {\
        __x = isSgFunctionSymbol( func );\
        if (RTEDDEBUG())\
        cerr << "Found MemberName : " #__x << endl;\
      }\
    }



using namespace std;

void RtedSymbols::visit(SgNode* n) {

  if (n == NULL)
    return;
  // ******************** DETECT Member functions in RuntimeSystem.h *************************************************************

  if (isSgScopeStatement(n)) {

    LOOKUP( roseCreateHeap );
    LOOKUP( roseAccessHeap );
    LOOKUP( roseCheckpoint );
    LOOKUP( roseFunctionCall );
    LOOKUP( roseAssertFunctionSignature );
    LOOKUP( roseConfirmFunctionSignature );
    LOOKUP( roseConvertIntToString );
    LOOKUP( roseCreateVariable );
#if NOT_YET_IMPLEMENTED
    LOOKUP( roseCreateSharedVariable );
#endif /* NOT_YET_IMPLEMENTED */
    LOOKUP( roseCreateObject );
    LOOKUP( roseInitVariable );
    LOOKUP( roseMovePointer );
    LOOKUP( roseEnterScope );
    LOOKUP( roseExitScope);
    LOOKUP( roseAccessVariable );
    LOOKUP( roseIOFunctionCall );
    LOOKUP( roseRegisterTypeCall );
    LOOKUP( roseFreeMemory );
    LOOKUP( roseReallocateMemory );
    LOOKUP( roseCheckIfThisNULL );
  }

  if (isSgTypedefDeclaration(n)) {
  	SgTypedefDeclaration* typed = isSgTypedefDeclaration(n);
		string name = typed->get_name().str();
		if (name=="size_t") {
			SgType* baseType = typed->get_base_type ();
			size_t_member=baseType;
			if (RTEDDEBUG())
			  cerr << "Found Type : " << name << endl;
		}
  }

  // ******************** DETECT Member functions in RuntimeSystem.h *************************************************************

}
