#include "RWState.h"

using namespace std;
using namespace CodeThorn;

bool CodeThorn::operator<(const RWState& s1, const RWState& s2) {
  if(s1.eState()!=s2.eState())
    return (s1.eState()<s2.eState());
  return s1.readWriteHistory()<s2.readWriteHistory();
}

bool CodeThorn::operator==(const RWState& s1, const RWState& s2) {
  return (s1.eState()==s2.eState())
    && (s1.readWriteHistory()==s2.readWriteHistory());
}

bool CodeThorn::operator!=(const RWState& s1, const RWState& s2) {
  return !(s1==s2);
}

string RWState::toString() const {
  stringstream ss;
  ss << "("<<eState()->toString()<<","<<readWriteHistory()->toString()<<")" << endl;
  return ss.str();
}
