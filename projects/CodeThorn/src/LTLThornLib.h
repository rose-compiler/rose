#ifndef LTLTHORN_LIB_H
#define LTLTHORN_LIB_H

#include <set>
#include "CodeThornOptions.h"
#include "LTLOptions.h"

namespace CodeThorn {
  void initDiagnosticsLTL();
  void runLTLAnalysis(CodeThornOptions& ctOpt, LTLOptions& ltlOpt,IOAnalyzer* analyzer, TimingCollector& tc);
  void optionallyInitializePatternSearchSolver(CodeThornOptions& ctOpt,IOAnalyzer* analyzer,TimingCollector& timingCollector);
  /* reads and parses LTL rers mapping file into the alphabet
     sets. Returns false if reading fails. Exits with error message if
     format is wrong.
  */
  bool readAndParseLTLRersMappingFile(string ltlRersMappingFileName, std::set<int>& ltlInAlphabet, std::set<int>& ltlInOutAlphabet);
}

#endif
