#include "sage3basic.h"
#include "InterProcDataFlowAnalysis.h"
#include <Rose/Diagnostics.h>


void InterProcDataFlowAnalysis::run ()
{
  using Rose::Diagnostics::mlog;

  bool change = false;
  int  iteration = 0;

  do
  {
    ++iteration;
    change = false;

    std::vector<SgFunctionDeclaration*> processingOrder;

    getFunctionDeclarations(processingOrder);

    for (SgFunctionDeclaration* funcDecl : processingOrder) {
      change |= runAndCheckIntraProcAnalysis(funcDecl);
    }
  } while (change);

  mlog[Rose::Diagnostics::INFO] << "Total Interprocedural iterations: " << iteration << std::endl;
}
