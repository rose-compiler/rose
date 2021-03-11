#ifndef LTLTHORN_LIB_H
#define LTLTHORN_LIB_H

#include <set>
#include "CodeThornOptions.h"
#include "LTLOptions.h"
#include "LTLRersMapping.h"

namespace CodeThorn {

  void initDiagnosticsLTL();
  void runLTLAnalysis(CodeThornOptions& ctOpt, LTLOptions& ltlOpt,IOAnalyzer* analyzer, TimingCollector& tc);
}

#endif
