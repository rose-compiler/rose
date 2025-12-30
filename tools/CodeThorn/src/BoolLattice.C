#include "sage3basic.h"
#include "BoolLattice.h"
#include "AbstractValue.h"
#include "assert.h"
#include "CommandLineOptions.h"
#include <iostream>
#include <climits>
#include "Miscellaneous.h"
#include "Miscellaneous2.h"
#include "CodeThornException.h"

using namespace std;

using namespace CodeThorn;

ostream& CodeThorn::operator<<(ostream& os, const BoolLattice& value) {
  os << value.toString();
  return os;
}

ostream& CodeThorn::operator<<(ostream& os, const AbstractValue& value) {
  os << value.toString();
  return os;
}

CodeThorn::BoolLattice::BoolLattice():value(CodeThorn::BoolLattice::BL_BOT) {}

CodeThorn::BoolLattice::BoolLattice(bool val) {if(val) value=CodeThorn::BoolLattice::BL_TRUE; else value=CodeThorn::BoolLattice::BL_FALSE;}

// type conversion
CodeThorn::BoolLattice::BoolLattice(Top /*e*/) {value=CodeThorn::BoolLattice::BL_TOP;}

// type conversion
CodeThorn::BoolLattice::BoolLattice(Bot /*e*/) {value=CodeThorn::BoolLattice::BL_BOT;}

// type conversion
CodeThorn::BoolLattice::BoolLattice(int x) {if(x==0) value=CodeThorn::BoolLattice::BL_FALSE; else value=CodeThorn::BoolLattice::BL_TRUE;}

bool CodeThorn::BoolLattice::isTop()   const {return value==CodeThorn::BoolLattice::BL_TOP;}
bool CodeThorn::BoolLattice::isTrue()  const {return value==CodeThorn::BoolLattice::BL_TRUE;}
bool CodeThorn::BoolLattice::isFalse() const {return value==CodeThorn::BoolLattice::BL_FALSE;}
bool CodeThorn::BoolLattice::isBot()   const {return value==CodeThorn::BoolLattice::BL_BOT;}

/**
 * CAVEAT:    We define !bot := bot
 * RATIONALE: bot means "not yet analyzed" / "no meaningful result"
 *          top means "could be any value"
 */
CodeThorn::BoolLattice CodeThorn::BoolLattice::operator!() {
  CodeThorn::BoolLattice tmp;
  switch(value) {
  case BL_FALSE: tmp.value=BL_TRUE;break;
  case BL_TRUE: tmp.value=BL_FALSE;break;
  case BL_TOP: tmp.value=BL_TOP;break;
  case BL_BOT: tmp.value=BL_BOT;break;
  default:
    throw CodeThorn::Exception("Error: BoolLattice operation '!' failed.");
  }
  return tmp;
}

bool CodeThorn::BoolLattice::operator==(CodeThorn::BoolLattice other) const {
  return other.value == value;
}

bool CodeThorn::BoolLattice::operator!=(CodeThorn::BoolLattice other) const {
  return other.value != value;
}

bool CodeThorn::BoolLattice::operator<(BoolLattice other) const {
  if (isBot()) {
    if (other.isBot()) return false;
    return true;
  }
  if (isFalse()) {
    if (other.isBot())   return false;
    if (other.isFalse()) return false;
    return true;
  }
  if (isTrue()) {
    if (other.isBot())   return false;
    if (other.isFalse()) return false;
    if (other.isTrue())  return false;
    return true;
  }
  if (isTop()) {
    return false;
  }
  throw CodeThorn::Exception("Error: BoolLattice operation< failed.");
}

CodeThorn::BoolLattice CodeThorn::BoolLattice::operator||(CodeThorn::BoolLattice other) {
  CodeThorn::BoolLattice tmp;
  // all BL_TOP cases
  if(isTop()   && other.isTop())   return Top();
  if(isTop()   && other.isTrue())  return true;
  if(isTrue()  && other.isTop())   return true;
  if(isTop()   && other.isFalse()) return Top();
  if(isFalse() && other.isTop())   return Top();
  // all BL_BOT cases
  if(value==BL_BOT)                   return other;
  if(other.value==BL_BOT)             return *this;
  // usual bool cases
  if(isTrue()  && other.isTrue())  return true;
  if(isTrue()  && other.isFalse()) return true;
  if(isFalse() && other.isTrue())  return true;
  if(isFalse() && other.isFalse()) return false;
  throw CodeThorn::Exception("Error: BoolLattice operation|| failed.");
}

CodeThorn::BoolLattice CodeThorn::BoolLattice::operator&&(CodeThorn::BoolLattice other) {
  // all BL_TOP cases
  if(isTop()   && other.isTop())   return Top();
  if(isTop()   && other.isTrue())  return Top();
  if(isTrue()  && other.isTop())   return Top();
  if(isTop()   && other.isFalse()) return false;
  if(isFalse() && other.isTop())   return false;
  // all BL_BOT cases
  if(value==BL_BOT)                   return other;
  if(other.value==BL_BOT)             return *this;
  // usual bool cases
  if(isTrue()  && other.isTrue())  return true;
  if(isTrue()  && other.isFalse()) return false;
  if(isFalse() && other.isTrue())  return false;
  if(isFalse() && other.isFalse()) return false;
  throw CodeThorn::Exception("Error: BoolLattice operation&& failed.");
}

CodeThorn::BoolLattice CodeThorn::BoolLattice::lub(CodeThorn::BoolLattice other) {
  // all BL_TOP cases
  if(isTop()   || other.isTop())   return Top();
  // all BL_BOT cases
  if(value==BL_BOT)                   return other;
  if(other.value==BL_BOT)             return *this;
  // usual bool cases
  if(isTrue()  && other.isTrue())  return true;
  if(isTrue()  && other.isFalse()) return Top();
  if(isFalse() && other.isTrue())  return Top();
  if(isFalse() && other.isFalse()) return false;
  throw CodeThorn::Exception("Error: BoolLattice lub failed.");
}

CodeThorn::BoolLattice CodeThorn::BoolLattice::glb(CodeThorn::BoolLattice other) {
  // all BL_BOT cases
  if(isBot()   || other.isBot())   return Bot();
  // all BL_BOT cases
  if(value==BL_TOP)                   return other;
  if(other.value==BL_TOP)             return *this;
  // usual bool cases
  if(isTrue()  && other.isTrue())  return true;
  if(isTrue()  && other.isFalse()) return Bot();
  if(isFalse() && other.isTrue())  return Bot();
  if(isFalse() && other.isFalse()) return false;
  throw CodeThorn::Exception("Error: BoolLattice glb failed.");
}

// operator= : C++ default used
// operator== : C++ default used
string CodeThorn::BoolLattice::toString() const {
  switch(value) {
  case BL_TOP: return "⊤" /*"top"*/; /* AP: not sure how portable this is */
  case BL_BOT: return "⊥" /*"bot"*/;
  case BL_TRUE: return "true";
  case BL_FALSE: return "false";
  default:
    cerr<<"VALUE = "<<value<<endl;
    throw CodeThorn::Exception("Error: BoolLattice::toString operation failed.");
  }
}
