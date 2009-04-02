#include "rose.h"
#include "RtedSymbols.h"

using namespace std;

void RtedSymbols::visit(SgNode* n) {

  if (n == NULL)
    return;
  // *********************** NEEDED to insert RuntimeSystem runtimeSystem = new RuntimeSystem() ***************

  // ******************** DETECT Member functions in RuntimeSystem.h *************************************************************

  if (isSgScopeStatement(n)) {
    SgFunctionSymbol* func = isSgScopeStatement(n)->lookup_function_symbol("roseCreateArray");
    if (isSgMemberFunctionSymbol(func)) {
      roseCreateArray = isSgMemberFunctionSymbol(func);
      cerr << ">>>>>>>>>>>>> Found MemberName : roseCreateArray" <<  endl;
    }
    SgFunctionSymbol* func2 = isSgScopeStatement(n)->lookup_function_symbol("roseArrayAccess");
    if (isSgMemberFunctionSymbol(func2)) {
      roseArrayAccess = isSgMemberFunctionSymbol(func2);
      cerr << ">>>>>>>>>>>>> Found MemberName : roseArrayAccess" <<  endl;
    }
    SgFunctionSymbol* func3 = isSgScopeStatement(n)->lookup_function_symbol("roseRtedClose");
    if (isSgMemberFunctionSymbol(func3)) {
      roseRtedClose = isSgMemberFunctionSymbol(func3);
      cerr << ">>>>>>>>>>>>> Found MemberName : roseRtedClose" <<  endl;
    }
  }


  // ******************** DETECT Member functions in RuntimeSystem.h *************************************************************

}

