#ifndef ANALYZER_TOOLS_H
#define ANALYZER_TOOLS_H

#include "Analyzer.h"

namespace CodeThorn {

  class CounterexampleGenerator {
    public:
      CounterexampleGenerator(Analyzer* analyzer);
      ~CounterexampleGenerator();

      static void initDiagnostics();
      EStateWorkList traceBreadthFirst(const EState* source, const EState* target, bool counterexampleWithOutput = false, bool filterIO = true);
      EStateWorkList traceDijkstra(const EState* source, const EState* target, bool counterexampleWithOutput = false, bool filterIO = true);
      EStateWorkList reverseInOutSequenceBreadthFirst(const EState* errorState, const EState* target, bool counterexampleWithOutput, bool filterIO);
      EStateWorkList filterStdInOutOnly(EStateWorkList& states, bool counterexampleWithOutput = false) const;
      EStateWorkList reverseInOutSequenceDijkstra(const EState* source, const EState* target, bool counterexampleWithOutput, bool filterIO);
      std::string reversedInOutRunToString(EStateWorkList& run);
      std::string reversedRunToString(EStateWorkList& run);

    protected:
      static Sawyer::Message::Facility logger;

    private:
      Analyzer* analyzer;
  };

}

#endif
