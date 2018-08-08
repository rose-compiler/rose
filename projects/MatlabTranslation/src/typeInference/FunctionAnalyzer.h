#ifndef OCTAVE_ANALYSIS_FUNCTIONANALYZER_H
#define OCTAVE_ANALYSIS_FUNCTIONANALYZER_H

#include <map>
#include <string>

#include "rose.h"

#include "BottomUpTypeAnalysis.h"

namespace MatlabAnalysis
{
  struct FunctionAnalyzer
  {
      explicit
      FunctionAnalyzer(SgProject* proj)
      : project(proj)
      {}

      void analyse_function(SgFunctionDeclaration *function);

    private:
      SgFunctionType* buildFunctionType_Custom(SgType *returnType, SgFunctionParameterList *argList);

      SgProject* project;
  };
}
#endif
