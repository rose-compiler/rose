#ifndef TRANSITION_GRAPH_REDUCER_H
#define TRANSITION_GRAPH_REDUCER_H

#include "EState.h"

namespace CodeThorn {

  class EStateSet;
  class TransitionGraph;

  /*! 
   * \author Marc Jasper
   * \date 2017.
   */
  class TransitionGraphReducer {
  public:
    TransitionGraphReducer(EStateSet* eStateSet, TransitionGraph* stg);
    void reduceStgToStatesSatisfying(std::function<bool(EStatePtr)> predicate);

  private:
    std::list<EStatePtr> successorsOfStateSatisfying(EStatePtr state, std::function<bool(EStatePtr)> predicate);

    EStateSet* _states;
    TransitionGraph* _stg;
 };
}
#endif
