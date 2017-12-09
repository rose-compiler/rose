#ifndef READ_WRITE_ANALYZER_H
#define READ_WRITE_ANALYZER_H

#include "sage3basic.h"
#include "IOAnalyzer.h"
#include "RWState.h"

namespace CodeThorn {

/*! 
  * \author Marc Jasper
  * \date 2017
 */
  class ReadWriteAnalyzer : public IOAnalyzer {

  public:
    //    ReadWriteAnalyzer();

  protected:
    static Sawyer::Message::Facility logger;

  public:
    static void initDiagnostics();

    void initializeSolver(std::string functionToStartAt,SgNode* root, bool oneFunctionOnly) override;

    //! requires init
    void runSolver() override;

  private:
    std::list<RWState> bigStep(const RWState* state);
    std::list<RWState> transfer(RWState& state);
    bool updateAndCheckForDataRaces(ReadWriteHistory& history, const EState* eState,
				    MemLocAccessSet& reads, MemLocAccessSet& writes);
    bool isBeginningOfUnorderedExecutionBlock(const EState* state) const;
    bool isEndOfUnorderedExecutionBlock(const EState* state) const;
    bool isEndOfOrderedExecutionBlock(const EState* state) const;
    bool isExecutedByMultipleWorkers(const EState* state) const;

    // the following list of functions needs to be implemented
    bool isBeginningOfOmpParallel(const EState* state) const;
    bool isEndOfOmpParallel(const EState* state) const;
    bool isOmpWorkShareBarrier(const EState* state) const;
    bool isInsideOmpLoop(const EState* state) const;
    bool isEndOfOmpParallelLoopIteration(const EState* state) const;
    bool isInsideOmpSingle(const EState* state) const;
    bool isEndOfOmpSingle(const EState* state) const;
    bool isInsideOmpSection(const EState* state) const;
    bool isEndOfOmpSection(const EState* state) const;
    // (end of list)

    void addToWorkList(const RWState* state);
    bool isEmptyWorkList();
    const RWState* topWorkList();
    const RWState* popWorkList();

    RWStateSet::ProcessingResult process(RWState& s);

    RWHistorySet rWHistorySet;
    RWStateSet rWStateSet;

    list<const RWState*> workList;
  };
}

#endif
