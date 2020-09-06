#include "sage3basic.h"
#include "RersCounterexample.h"
#include "EState.h"
#include "Diagnostics.h"

using namespace CodeThorn;
using namespace std;
using namespace Sawyer::Message;

/*! 
 * \author Marc Jasper
 * \date 2017.
 */
RersCounterexample 
RersCounterexample::onlyStatesSatisfying(std::function<bool(const EState*)> predicate) const {
  return onlyStatesSatisfyingTemplate<RersCounterexample>(predicate);
}

/*! 
 * \author Marc Jasper
 * \date 2017.
 */
RersCounterexample RersCounterexample::onlyIStates() const {
  function<bool(const EState*)> predicate = [](const EState* s) { 
    return s->io.isStdInIO();
  };
  return onlyStatesSatisfying(predicate);
}

/*! 
 * \author Marc Jasper
 * \date 2017.
 */
RersCounterexample RersCounterexample::onlyIOStates() const {
  function<bool(const EState*)> predicate = [](const EState* s) { 
    return s->io.isStdInIO() || s->io.isStdOutIO();
  };
  return onlyStatesSatisfying(predicate);
}

/*! 
 * \author Marc Jasper
 * \date 2017.
 */
string RersCounterexample::toRersIString() const {
  return toRersIOString(false);
}

/*! 
 * \author Marc Jasper
 * \date 2017.
 */
string RersCounterexample::toRersIOString() const {
  return toRersIOString(true);
}

/*! 
 * \author Marc Jasper
 * \date 2014, 2017.
 */
string RersCounterexample::toRersIOString(bool withOutput) const {
  stringstream result; 
  result << "[";
  RersCounterexample::const_iterator begin = this->begin();
  RersCounterexample::const_iterator end = this->end();
  bool firstSymbol = true;
  for (RersCounterexample::const_iterator i = begin; i != end; i++ ) {
    if ( (*i)->io.isStdInIO() || (withOutput && (*i)->io.isStdOutIO()) ) {
      if (!firstSymbol) {
        result << ";";
      }
      const PState* pstate = (*i)->pstate();
      int inOutVal = pstate->readFromMemoryLocation((*i)->io.var).getIntValue();
      if ((*i)->io.isStdInIO()) {
        result << "i" << toRersChar(inOutVal);
        firstSymbol = false;
      } else if (withOutput && (*i)->io.isStdOutIO()) {
        result << "o" << toRersChar(inOutVal);
        firstSymbol = false;
      } 
    }
  }
  result << "]";
  return result.str();
}

/*! 
 * \author Marc Jasper
 * \date 2017.
 */
char RersCounterexample::toRersChar(int value) const {
  return (char) (value + ((int) 'A') - 1);
}
