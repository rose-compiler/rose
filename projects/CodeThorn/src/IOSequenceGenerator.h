#ifndef IOSequenceGenerator_H
#define IOSequenceGenerator_H

#include "EState.h"
#include "TransitionGraph.h"
#include <iostream>
#include <sstream>

namespace CodeThorn {
  // predefined predicate
  class InputPath : public std::list<const EState*> {
  public:
    // adds a state to the input path if it is an input state (returns 1 in this case, otherwise 0 (for counting)).
    InputPath();
    int addState(const EState* s);
    std::string toString() const;
    unsigned int numInputStates() const;
  private:
    unsigned int _numInputStates;
  };

  class InputPathSet : public std::set<InputPath> {
    // IO Functions to be added
  public:
    void toFile(std::ofstream& os);
  };
  
  class IOSequenceGenerator {
  public:
    IOSequenceGenerator();
    void computeInputPathSet(unsigned int maxlen, TransitionGraph& g);
    void computeRandomInputPathSet(unsigned int maxlen, TransitionGraph& g, int randomNum);
    void generateFile(string fileName);
  private:
    InputPathSet ipset;
    bool randomSearch;
    const EState* chooseElement(EStatePtrSet& S0);
    InputPath chooseElement(InputPathSet& S0);
    void computeInputPathSet(InputPath p, int len, unsigned int maxlen, const EState* s, TransitionGraph& g, InputPathSet& pathSet);
  };
} // namespace CodeThorn

#endif
