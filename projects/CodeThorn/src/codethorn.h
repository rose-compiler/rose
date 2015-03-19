#ifndef CODETHORN_H
#define CODETHORN_H

/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include <iostream>
#include <fstream>
#include <set>
#include <string>
#include <sstream>

#include "CFAnalysis.h"
#include "Analyzer.h"
#include "Visualizer.h"
#include "LanguageRestrictor.h"
#include "LanguageRestrictorCollection.h"
#include "Miscellaneous.h"

namespace CodeThorn {

  class CodeThornLanguageRestrictor : public LanguageRestrictorCppSubset1 {
  public:
    void initialize();
  };

  void printAnalyzerStatistics(Analyzer& analyzer, double totalRunTime, 
                               std::string title = "state transition system computed");
  // prints #transitions and details about states
  void printStgSize(TransitionGraph* model, std::string optionalComment="");
  
} // end of namespace CodeThorn

#endif
