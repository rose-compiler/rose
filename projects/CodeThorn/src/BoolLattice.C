
// -*- mode: C++; coding: utf-8; -*-
/*************************************************************
 * Author   : Markus Schordan                                *
 *************************************************************/

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

CodeThorn::BoolLattice::BoolLattice():value(CodeThorn::BoolLattice::BOT) {}

CodeThorn::BoolLattice::BoolLattice(bool val) {if(val) value=CodeThorn::BoolLattice::TRUE; else value=CodeThorn::BoolLattice::FALSE;}

// type conversion
CodeThorn::BoolLattice::BoolLattice(Top e) {value=CodeThorn::BoolLattice::TOP;}

// type conversion
CodeThorn::BoolLattice::BoolLattice(Bot e) {value=CodeThorn::BoolLattice::BOT;}

// type conversion
CodeThorn::BoolLattice::BoolLattice(int x) {if(x==0) value=CodeThorn::BoolLattice::FALSE; else value=CodeThorn::BoolLattice::TRUE;}

bool CodeThorn::BoolLattice::isTop()   const {return value==CodeThorn::BoolLattice::TOP;}
bool CodeThorn::BoolLattice::isTrue()  const {return value==CodeThorn::BoolLattice::TRUE;}
bool CodeThorn::BoolLattice::isFalse() const {return value==CodeThorn::BoolLattice::FALSE;}
bool CodeThorn::BoolLattice::isBot()   const {return value==CodeThorn::BoolLattice::BOT;}

/**
 * CAVEAT:    We define !bot := bot
 * RATIONALE: bot means "not yet analyzed" / "no meaningful result"
 *          top means "could be any value"
 */
CodeThorn::BoolLattice CodeThorn::BoolLattice::operator!() {
  CodeThorn::BoolLattice tmp;
  switch(value) {
  case FALSE: tmp.value=TRUE;break;
  case TRUE: tmp.value=FALSE;break;
  case TOP: tmp.value=TOP;break;
  case BOT: tmp.value=BOT;break;
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
  // all TOP cases
  if(isTop()   && other.isTop())   return Top();
  if(isTop()   && other.isTrue())  return true;
  if(isTrue()  && other.isTop())   return true;
  if(isTop()   && other.isFalse()) return Top();
  if(isFalse() && other.isTop())   return Top();
  // all BOT cases
  if(value==BOT)                   return other;
  if(other.value==BOT)             return *this;
  // usual bool cases
  if(isTrue()  && other.isTrue())  return true;
  if(isTrue()  && other.isFalse()) return true;
  if(isFalse() && other.isTrue())  return true;
  if(isFalse() && other.isFalse()) return false;
  throw CodeThorn::Exception("Error: BoolLattice operation|| failed.");
}

CodeThorn::BoolLattice CodeThorn::BoolLattice::operator&&(CodeThorn::BoolLattice other) {
  // all TOP cases
  if(isTop()   && other.isTop())   return Top();
  if(isTop()   && other.isTrue())  return Top();
  if(isTrue()  && other.isTop())   return Top();
  if(isTop()   && other.isFalse()) return false;
  if(isFalse() && other.isTop())   return false;
  // all BOT cases
  if(value==BOT)                   return other;
  if(other.value==BOT)             return *this;
  // usual bool cases
  if(isTrue()  && other.isTrue())  return true;
  if(isTrue()  && other.isFalse()) return false;
  if(isFalse() && other.isTrue())  return false;
  if(isFalse() && other.isFalse()) return false;
  throw CodeThorn::Exception("Error: BoolLattice operation&& failed.");
}

CodeThorn::BoolLattice CodeThorn::BoolLattice::lub(CodeThorn::BoolLattice other) {
  // all TOP cases
  if(isTop()   || other.isTop())   return Top();
  // all BOT cases
  if(value==BOT)                   return other;
  if(other.value==BOT)             return *this;
  // usual bool cases
  if(isTrue()  && other.isTrue())  return true;
  if(isTrue()  && other.isFalse()) return Top();
  if(isFalse() && other.isTrue())  return Top();
  if(isFalse() && other.isFalse()) return false;
  throw CodeThorn::Exception("Error: BoolLattice lub failed.");
}

CodeThorn::BoolLattice CodeThorn::BoolLattice::glb(CodeThorn::BoolLattice other) {
  // all BOT cases
  if(isBot()   || other.isBot())   return Bot();
  // all BOT cases
  if(value==TOP)                   return other;
  if(other.value==TOP)             return *this;
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
  case TOP: return "⊤" /*"top"*/; /* AP: not sure how portable this is */
  case BOT: return "⊥" /*"bot"*/;
  case TRUE: return "true";
  case FALSE: return "false";
  default:
    cerr<<"VALUE = "<<value<<endl;
    throw CodeThorn::Exception("Error: BoolLattice::toString operation failed.");
  }
}
