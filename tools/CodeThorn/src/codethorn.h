#ifndef CODETHORN_H
#define CODETHORN_H

/*************************************************************
 * Author   : Markus Schordan                                *
 *************************************************************/

#include <iostream>
#include <fstream>
#include <set>
#include <string>
#include <sstream>

#include "CFAnalysis.h"
#include "IOAnalyzer.h"
#include "Visualizer.h"
#include "LanguageRestrictor.h"
#include "LanguageRestrictorCollection.h"
#include "Miscellaneous.h"

namespace CodeThorn {

  void initDiagnostics();
  
} // end of namespace CodeThorn

#endif
