#ifndef LTLTHORN_LIB_H
#define LTLTHORN_LIB_H

namespace CodeThorn {
  void initDiagnosticsLTL();
}

void runLTLAnalysis(CodeThornOptions& ctOpt, LTLOptions& ltlOpt,IOAnalyzer* analyzer, TimingCollector& tc);
void optionallyInitializePatternSearchSolver(CodeThornOptions& ctOpt,IOAnalyzer* analyzer,TimingCollector& timingCollector);

#endif
