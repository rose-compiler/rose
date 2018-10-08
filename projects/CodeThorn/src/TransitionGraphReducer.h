#ifndef TRANSITION_GRAPH_REDUCER_H
#define TRANSITION_GRAPH_REDUCER_H

namespace CodeThorn {

  class EState;
  class EStateSet;
  class TransitionGraph;

  /*! 
   * \author Marc Jasper
   * \date 2017.
   */
  class TransitionGraphReducer {
  public:
    TransitionGraphReducer(EStateSet* eStateSet, TransitionGraph* stg);
    void reduceStgToStatesSatisfying(std::function<bool(const EState*)> predicate);

  private:
    std::list<const EState*> successorsOfStateSatisfying(const EState* state, std::function<bool(const EState*)> predicate);

    EStateSet* _states;
    TransitionGraph* _stg;
 };
}
#endif
