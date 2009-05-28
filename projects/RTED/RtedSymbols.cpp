#include "rose.h"
#include "RtedSymbols.h"

using namespace std;

void RtedSymbols::visit(SgNode* n) {

  if (n == NULL)
    return;
  // *********************** NEEDED to insert RuntimeSystem runtimeSystem = new RuntimeSystem() ***************

  // ******************** DETECT Member functions in RuntimeSystem.h *************************************************************

  if (isSgScopeStatement(n)) {

    SgFunctionSymbol* func = isSgScopeStatement(n)->lookup_function_symbol("RuntimeSystem_roseCreateArray");
    if (isSgFunctionSymbol(func)) {
      roseCreateArray = isSgFunctionSymbol(func);
      cerr << ">>>>>>>>>>>>> Found MemberName : roseCreateArray" <<  endl;
    }
    SgFunctionSymbol* func2 = isSgScopeStatement(n)->lookup_function_symbol("RuntimeSystem_roseArrayAccess");
    if (isSgFunctionSymbol(func2)) {
      roseArrayAccess = isSgFunctionSymbol(func2);
      cerr << ">>>>>>>>>>>>> Found MemberName : roseArrayAccess" <<  endl;
    }
    SgFunctionSymbol* func3 = isSgScopeStatement(n)->lookup_function_symbol("RuntimeSystem_roseRtedClose");
    if (isSgFunctionSymbol(func3)) {
      roseRtedClose = isSgFunctionSymbol(func3);
      cerr << ">>>>>>>>>>>>> Found MemberName : roseRtedClose" <<  endl;
    }
    SgFunctionSymbol* func4 = isSgScopeStatement(n)->lookup_function_symbol("RuntimeSystem_roseFunctionCall");
    if (isSgFunctionSymbol(func4)) {
      roseFunctionCall = isSgFunctionSymbol(func4);
      cerr << ">>>>>>>>>>>>> Found MemberName : roseFunctionCall" <<  endl;
    }
    SgFunctionSymbol* func5 = isSgScopeStatement(n)->lookup_function_symbol("RuntimeSystem_roseConvertIntToString");
    if (isSgFunctionSymbol(func5)) {
      roseConvertIntToString = isSgFunctionSymbol(func5);
      string symbolName3 = roseConvertIntToString->get_name().str();
      cerr << " >>>>>>>> Symbol Member: " << symbolName3 << endl;
    }
    SgFunctionSymbol* func6 = isSgScopeStatement(n)->lookup_function_symbol("RuntimeSystem_roseCallStack");
    if (isSgFunctionSymbol(func6)) {
      roseCallStack = isSgFunctionSymbol(func6);
      cerr << ">>>>>>>>>>>>> Found MemberName : roseCallStack" <<  endl;
    }
    SgFunctionSymbol* func7 = isSgScopeStatement(n)->lookup_function_symbol("RuntimeSystem_roseCreateVariable");
    if (isSgFunctionSymbol(func7)) {
      roseCreateVariable = isSgFunctionSymbol(func7);
      cerr << ">>>>>>>>>>>>> Found MemberName : roseCreateVariable" <<  endl;
    }
    SgFunctionSymbol* func8 = isSgScopeStatement(n)->lookup_function_symbol("RuntimeSystem_roseInitVariable");
    if (isSgFunctionSymbol(func8)) {
      roseInitVariable = isSgFunctionSymbol(func8);
      cerr << ">>>>>>>>>>>>> Found MemberName : roseInitVariable" <<  endl;
    }


  }


  // ******************** DETECT Member functions in RuntimeSystem.h *************************************************************

}

