#ifndef OCTAVE_ANALYSIS_BOTTOMUPTYPEANALYSIS_H
#define OCTAVE_ANALYSIS_BOTTOMUPTYPEANALYSIS_H

#include <string>
#include <map>
#include <utility>

#include "rose.h"

#include "FastNumericsRoseSupport.h"

namespace MatlabAnalysis
{
  typedef FastNumericsRoseSupport::MatlabFunctionRec    MatlabFunctionRec;
  typedef FastNumericsRoseSupport::MatlabOverloadSet    MatlabOverloadSet;
  typedef FastNumericsRoseSupport::NameToDeclarationMap NameToDeclarationMap;

  struct Ctx
  {
    /// A mapping from function name to function declaration, with a bool flag
    /// to indicate if a copy of the function has been made
    static NameToDeclarationMap  nameToFunctionDeclaration;

    /// A name-decl pair of matlab builtin functions
    static NameToDeclarationMap  matlabBuiltins;
  };

  class FunctionAnalyzer;

  void runBottomUpInference(SgExpression*, SgProject*, FunctionAnalyzer*);

  static inline
  bool hasBeenAnalyzed(const MatlabFunctionRec& rec)
  {
    return rec.second;
  }

  static inline
  void setAnalyzed(MatlabFunctionRec& rec)
  {
    rec.second = true;
  }
}

#endif /* OCTAVE_ANALYSIS_BOTTOMUPTYPEANALYSIS_H */
