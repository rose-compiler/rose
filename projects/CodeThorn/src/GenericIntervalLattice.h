#ifndef GENERICINTERVALLATTICE_H
#define GENERICINTERVALLATTICE_H

#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <cassert>
#include "AType.h"
#include "SprayException.h"

// Author: Markus Schordan, 2014.

/* represents one interval
  Requirements: Type must support algorithms: min, max; operators: '<','==', binary +,-,*,/,%, ; values: -1, +1.
  Binary operations on bot remains bot (this should be used for error checking)
*/

// we may want to make this type a template parameter of IntervalLattice for handling relational operators
typedef CodeThorn::AType::BoolLattice BoolLatticeType;
typedef CodeThorn::AType::Top BoolLatticeTop;
typedef CodeThorn::AType::Bot BoolLatticeBot;


namespace SPRAY {

  enum JoinMode {
      JM_Exact,
      JM_InfinityIfUnequal,
      JM_InfinityAsymmetric
    };

template<typename Type>
class GenericIntervalLattice {
  // creates an interval with a known left and right boundary
 public:
 GenericIntervalLattice() { setTop();}
 GenericIntervalLattice(Type number):_low(number),_high(number),_isLowInf(false),_isHighInf(false) {}
 GenericIntervalLattice(Type left, Type right):_low(left),_high(right),_isLowInf(false),_isHighInf(false) {}


 static GenericIntervalLattice createFromBoolLattice(const BoolLatticeType& boolValue) {
   if(boolValue.isTop()) {
     GenericIntervalLattice gil;
     gil.setTop();
     return gil;
   }
   else if(boolValue.isTrue()) {
     return GenericIntervalLattice(1);
   }
   else if(boolValue.isFalse()) {
     return GenericIntervalLattice(0);
   }
   else if(boolValue.isBot()) {
    GenericIntervalLattice gil;
    gil.setBot();
    return gil;
   }
   else {
     throw SPRAY::Exception("createFromBoolLattice: internal error.");
   }
 }

  static GenericIntervalLattice highInfInterval(Type left) {
    GenericIntervalLattice t;
    t.setIsLowInf(false);
    t._low=left;
    return t;
  }
  static GenericIntervalLattice lowInfInterval(Type right) {
    GenericIntervalLattice t;
    t.setIsHighInf(false);
    t._high=right;
    return t;
  }
  bool isTop() const {
    return isLowInf() && isHighInf();
  }
  void setTop() {
    setIsLowInf(true);
    setIsHighInf(true);
    _low=-1;
    _high=+1;
  }
  bool includesZero() const {
    GenericIntervalLattice zero(0);
    return isSubIntervalOf(zero,*this);
  }
  // definitely represents a value equal to 0
  bool isFalse() const {
    GenericIntervalLattice zero(0);
    BoolLatticeType bool3=isEqual(zero,*this);
    return bool3.isTrue();
  }
  // definitely represents a value not equal to 0
  bool isTrue() const {
    GenericIntervalLattice zero(0);
    BoolLatticeType bool3=isEqual(zero,*this);
    return bool3.isFalse();
  }

  bool isBot() const {
    return isEmpty();
  }
  void setBot() {
    setEmpty();
  }
  bool isEmpty() const {
    bool empty=!isLowInf()&&!isHighInf()&&_low>_high;
    if(empty) {
      assert(_low==+1 && _high==-1);
    }
    return empty;
  }
  void setEmpty() {
    // unified empty interval [+1,-1] (also to ensure proper operation of default operator==)
    setIsLowInf(false);
    setIsHighInf(false);
    _low=+1;
    _high=-1;
  }
  bool isInfLength() const {
    return isLowInf()||isHighInf();
  }
  Type length() {
    assert(!isInfLength());
    return _high-_low;
  }

  bool isConst() const {
    return !isLowInf()&&!isHighInf()&&_low==_high;
  }
  Type getConst() {
    assert(isConst());
    return _low;
  }
  bool isLowInf() const {
    return _isLowInf;
  }
  bool isHighInf() const {
    return _isHighInf;
  }
  void setLow(Type val) {
    setIsLowInf(false);
    _low=val;
    unifyEmptyInterval();
  }
  void setHigh(Type val) {
    setIsHighInf(false);
    _high=val;
    unifyEmptyInterval();
  }
  void setLowInf() {
    setIsLowInf(true);
    _low=-1;
  }
  void setHighInf() {
    setIsHighInf(true);
    _high=+1;
  }
  Type getLow() { assert(!isLowInf()); return _low; }
  Type getHigh() { assert(!isHighInf()); return _high; }


  static bool haveOverlap(GenericIntervalLattice l1, GenericIntervalLattice l2) {
    return !meet(l1,l2).isEmpty();
  }
  

  // checks whether interval l1 is a subinterval of l2
  static bool isSubIntervalOf(GenericIntervalLattice l1, GenericIntervalLattice l2) {
    if(l1.isEmpty())
      return true;
    else if(l2.isEmpty())
      return false;
    else if(l2.isTop())
      return true;
    else {
      bool lowOk=(l2.isLowInf() || (!l1.isLowInf() && !l2.isLowInf() && l1.getLow()>=l2.getLow()) );
      bool highOk=(l2.isHighInf() || (!l1.isHighInf() && l1.getHigh()<=l2.getHigh()) );
      return lowOk && highOk;
    }
    assert(0);
  }

  static GenericIntervalLattice top() {
    GenericIntervalLattice t;
    t.setTop();
    return t;
  }
  static GenericIntervalLattice bot() {
    GenericIntervalLattice t;
    t.setBot();
    return t;
  }
  static GenericIntervalLattice join(GenericIntervalLattice l1, GenericIntervalLattice l2) {
    GenericIntervalLattice l3=l1;
    l3.join(l2);
    return l3;
  }
  static GenericIntervalLattice meet(GenericIntervalLattice l1, GenericIntervalLattice l2) {
    GenericIntervalLattice l3=l1;
    l3.meet(l2);
    return l3;
  }
  static GenericIntervalLattice arithAdd(GenericIntervalLattice l1, GenericIntervalLattice l2) {
    GenericIntervalLattice l3=l1;
    l3.arithAdd(l2);
    return l3;
  }
  static GenericIntervalLattice arithSub(GenericIntervalLattice l1, GenericIntervalLattice l2) {
    GenericIntervalLattice l3=l1;
    l3.arithSub(l2);
    return l3;
  }
  static GenericIntervalLattice arithMul(GenericIntervalLattice l1, GenericIntervalLattice l2) {
    GenericIntervalLattice l3=l1;
    l3.arithMul(l2);
    return l3;
  }
  static GenericIntervalLattice arithDiv(GenericIntervalLattice l1, GenericIntervalLattice l2) {
    GenericIntervalLattice l3=l1;
    l3.arithDiv(l2);
    return l3;
  }
  static GenericIntervalLattice arithMod(GenericIntervalLattice l1, GenericIntervalLattice l2) {
    GenericIntervalLattice l3=l1;
    l3.arithMod(l2);
    return l3;
  }
  static GenericIntervalLattice bitwiseShiftLeft(GenericIntervalLattice l1, GenericIntervalLattice l2) {
    GenericIntervalLattice l3=l1;
    l3.bitwiseShiftLeft(l2);
    return l3;
  }
  static GenericIntervalLattice bitwiseShiftRight(GenericIntervalLattice l1, GenericIntervalLattice l2) {
    GenericIntervalLattice l3=l1;
    l3.bitwiseShiftRight(l2);
    return l3;
  }

  // schroder3 (2016-07-05): Checks for division by zero and adjusts the divisor
  static bool checkForDivisionByZero(const GenericIntervalLattice& dividend, GenericIntervalLattice& divisor) {
    if(isSubIntervalOf(GenericIntervalLattice(static_cast<Type>(0))/*[0, 0]*/, divisor)) {
      // Other interval contains 0 and this is therefore a possible division by zero.
      std::cout << "WARNING: division by interval that is containing zero (dividend: " << dividend.toString() << ", divisor: " << divisor.toString() << ")." << std::endl;
      // Adjust interval limits if they are zero to avoid division-by-zero error while
      //  computing the new interval limits:
      if(divisor._low == 0 && divisor._high == 0) {
        // We know that the variable always has the value zero and we divide by it. This will
        // always yield an error and the following code is never executed.
        std::cout << "WARNING: found unreachable code behind division by zero." << std::endl;
        divisor.setBot();
      }
      else if(divisor._low == 0) {
        // Zero is no longer a valid interval value after the division:
        divisor._low += 1;
      }
      else if(divisor._high == 0) {
        // Zero is no longer a valid interval value after the division:
        divisor._high -= 1;
      }
      return true;
    }
    return false;
  }

  void meet(GenericIntervalLattice other) {
    // 1. handle lower bounds
    if(isLowInf() && other.isLowInf()) {
      // OK
    } else if(isLowInf() && !other.isLowInf()) {
      setIsLowInf(false);
      _low=other._low;
    } else if(!isLowInf() && other.isLowInf()) {
      // OK
    } else {
      _low=std::max(_low,other._low);
    }
    // 2. handle upper bounds
    if(isHighInf() && other.isHighInf()) {
      // OK
    } else if(isHighInf() && !other.isHighInf()) {
      setIsHighInf(false);
      _high=other._high;
    } else if(!isHighInf() && other.isHighInf()) {
      // OK
    } else {
      _high=std::min(_high,other._high);
    }
    // 3. finalize special cases
    unifyEmptyInterval();
  }

  void join(GenericIntervalLattice other, JoinMode joinMode = JM_Exact) {
    // subsumption in case none of the two intervals has an unknown bound
//    if(!isInfLength()&&!other.isInfLength()) {
//      // handle 2 cases i) subset (2 variants), ii) constants (interval-length==1 for both)
//      if((_low>=other._low && _high<=other._high)
//         ||(other._low>=_low && other._high<=_high)
//         ||(isConst()&&other.isConst()) ) {
//        _low=std::min(_low,other._low);
//        _high=std::max(_high,other._high);
//        return;
//      }
//    }
    if(isLowInf()||other.isLowInf()) {
      setIsLowInf(true);
    }
    else {
      if(joinMode == JM_Exact) {
        _low=std::min(_low,other._low);
      }
      else if(joinMode == JM_InfinityIfUnequal) {
        if(_low!=other._low) {
          setIsLowInf(true);
        } else {
          // keep _low
        }
      }
      else if(joinMode == JM_InfinityAsymmetric) {
        if(other._low < _low) {
          setIsLowInf(true);
        } else {
          // keep _low
        }
      }
      else {
        throw SPRAY::Exception("Invalid join mode.");
      }
    }
    if(isHighInf()||other.isHighInf()) {
      setIsHighInf(true);
    }
    else {
      if(joinMode == JM_Exact) {
        _high=std::max(_high,other._high);
      }
      else if(joinMode == JM_InfinityIfUnequal) {
        if(_high!=other._high) {
          setIsHighInf(true);
        } else {
          // keep _high
        }
      }
      else if(joinMode == JM_InfinityAsymmetric) {
        if(other._high > _high) {
          setIsHighInf(true);
        } else {
          // keep _high
        }
      }
      else {
        throw SPRAY::Exception("Invalid join mode.");
      }
    }
  }

  BoolLatticeType toBoolLattice() const {
    if(isBot()) {
      return BoolLatticeType(BoolLatticeBot());
    }
    else if(isTrue()) {
      return BoolLatticeType(true);
    }
    else if(isFalse()) {
      return BoolLatticeType(false);
    }
    else {
      return BoolLatticeType(BoolLatticeTop());
    }
  }

  std::string toString() const {
    std::stringstream ss;
    if(isTop()) {
      ss<<"top";
      return ss.str();
    } else if(isBot()) {
      ss<<"bot";
      return ss.str();
    }
    if(isLowInf())
      ss<<"inf";
    else
      ss<<_low.toString();
    ss<<",";
    if(isHighInf())
      ss<<"inf";
    else
      ss<<_high.toString();
    return "["+ss.str()+"]";
  }

  bool operationOnBot() {
    return isBot();
  }
  bool binaryOperationOnBot(GenericIntervalLattice other) {
    if(isBot()||other.isBot()) {
      setBot();
      return true;
    } else {
      return false;
    }
  }

  void arithAdd(Type val) {
    if(operationOnBot())
      return;
    if(!isLowInf())
      _low+=val;
    if(!isHighInf())
      _high+=val;
  }

  void arithSub(Type val) {
    if(operationOnBot())
      return;
    if(!isLowInf())
      _low-=val;
    if(!isHighInf())
      _high-=val;
  }

  void arithMul(Type val) {
    if(operationOnBot())
      return;
    if(!isLowInf())
      _low*=val;
    if(!isHighInf())
      _high*=val;
  }

  void arithDiv(Type val) {
    // TODO: handle division by zero
    if(operationOnBot())
      return;
    if(!isLowInf())
      _low/=val;
    if(!isHighInf())
      _high/=val;
  }

  void arithMod(Type val) {
    if(operationOnBot())
      return;
    if(!isLowInf())
      _low%=val;
    if(!isHighInf())
      _high%=val;
  }

  void min(GenericIntervalLattice other) {
    if(isLowInf()) {
      return;
    } else if(other.isLowInf()) {
      setLowInf(true);
      return;
    } else {
      ROSE_ASSERT(_low<=_high);
      return std::min(std::min(_low,other._low),other._high);
    }
  }

  void max(GenericIntervalLattice other) {
    if(isHighInf()) {
      return;
    } else if(other.isHighInf()) {
      setHighInf(true);
      return;
    } else {
      ROSE_ASSERT(_low<=_high);
      return std::max(std::max(_high,other._low),other._high);
    }
  }

  // [a,b]+[c,d]=[a+c,b+d]
  void arithAdd(GenericIntervalLattice other) {
    if(binaryOperationOnBot(other))
      return;
    if(!isLowInf() && !other.isLowInf())
      _low+=other._low;
    if(!isHighInf() && !other.isHighInf())
      _high+=other._high;
    if(isLowInf()||other.isLowInf())
      setIsLowInf(true);
    if(isHighInf()||other.isHighInf())
      setIsHighInf(true);
  }

  // [a,b]-[c,d]=[a-d,b-c]
  void arithSub(GenericIntervalLattice other) {
    if(binaryOperationOnBot(other))
      return;
    if(!isLowInf() && !other.isHighInf())
      _low-=other._high;
    if(!isHighInf() && !other.isLowInf())
      _high-=other._low;
    if(isLowInf()||other.isHighInf())
      setIsLowInf(true);
    if(isHighInf()||other.isLowInf())
      setIsHighInf(true);
  }

  // TODO: not finished for top/bot
  // [a,b]*[c,d]=[min(a*c,a*d,b*c,b*d),max(a*c,a*d,b*c,b*d)]
  void arithMul(GenericIntervalLattice other) {
    if(binaryOperationOnBot(other)) {
      return;
    } else if(!isLowInf() 
              && !other.isLowInf() 
              && !isHighInf() 
              && !other.isHighInf()) {
      Type n1=_low;
      Type n2=_high;
      Type n3=other._low;
      Type n4=other._high;
      Type nmin=std::min(n1*n3,std::min(n1*n4,std::min(n2*n3,n2*n4)));
      Type nmax=std::max(n1*n3,std::max(n1*n4,std::max(n2*n3,n2*n4)));
      setLow(nmin);
      setHigh(nmax);
      return;
    } else {
      setIsLowInf(true);
      setIsHighInf(true);
    }
  }

  // TODO: not finished for top/bot
  // [a,b]/[c,d]=[min(a/c,a/d,b/c,b/d),max(a/c,a/d,b/c,b/d)]
  void arithDiv(GenericIntervalLattice other) {
    // schroder3 (2016-07-05): TODO: make "other" a reference.
    checkForDivisionByZero(*this, other);

    if(binaryOperationOnBot(other)) {
      return;
    } else if(!isLowInf() 
              && !other.isLowInf() 
              && !isHighInf() 
              && !other.isHighInf()) {
      Type n1=_low;
      Type n2=_high;
      Type n3=other._low;
      Type n4=other._high;
      if(n3==0) {
        // schroder3 (2016-07-05): should be unreachable because of checkForDivisionByZero(...):
        ROSE_ASSERT(false && &_low /* to make ROSE_ASSERT dependent of template parameter (necessary in case of Sawyer assert) */);
        if(n4!=0) {
          std::cout<<"INFO: division by zero interval adjustment."<<std::endl;
          n3+=1;
        } else {
          other.setBot(); // schroder3 (2016-07-05): TODO: this does not have any effect because
                          //  "other" is a local variable that is not used behind this point.
          std::cout<<"INFO: division by zero interval (lengh=1). Continue with bot."<<std::endl;
        }
      }

      Type nmin=std::min(n1/n3,std::min(n1/n4,std::min(n2/n3,n2/n4)));
      Type nmax=std::max(n1/n3,std::max(n1/n4,std::max(n2/n3,n2/n4)));
      setLow(nmin);
      setHigh(nmax);
      return;
    } else {
      setIsLowInf(true);
      setIsHighInf(true);
    }
  }

  // TODO: not finished for top/bot
  // [a,b]%[c,d]=[min(a%c,a%d,b%c,b%d),max(a%c,a%d,b%c,b%d)]
  void arithMod(GenericIntervalLattice other) {
    // schroder3 (2016-07-05): TODO: make "other" a reference.
    checkForDivisionByZero(*this, other);

    if(binaryOperationOnBot(other)) {
      return;
    } else if(!isLowInf() 
              && !other.isLowInf() 
              && !isHighInf() 
              && !other.isHighInf()) {
      Type n1=_low;
      Type n2=_high;
      Type n3=other._low;
      Type n4=other._high;
      Type nmin=std::min(n1%n3,std::min(n1%n4,std::min(n2%n3,n2%n4)));
      Type nmax=std::max(n1%n3,std::max(n1%n4,std::max(n2%n3,n2%n4)));
      setLow(nmin);
      setHigh(nmax);
      return;
    } else {
      setIsLowInf(true);
      setIsHighInf(true);
    }
  }

  // TODO: not finished for top/bot
// [a,b]<<[c,d]= same as multiply but with '<<' (no neg. shift)
  void bitwiseShiftLeft(GenericIntervalLattice other) {
    if(binaryOperationOnBot(other)) {
      return;
    } else if(!isLowInf() 
              && !other.isLowInf() 
              && !isHighInf() 
              && !other.isHighInf()) {
      Type n1=_low;
      Type n2=_high;
      Type n3=other._low;
      Type n4=other._high;
      Type nmin=std::min(n1<<n3,std::min(n1<<n4,std::min(n2<<n3,n2<<n4)));
      Type nmax=std::max(n1<<n3,std::max(n1<<n4,std::max(n2<<n3,n2<<n4)));
      setLow(nmin);
      setHigh(nmax);
      return;
    } else {
      setIsLowInf(true);
      setIsHighInf(true);
    }
  }

  // TODO: not finished for top/bot
  // [a,b]<<[c,d]= same as division but with '>>' (no neg. shift)
  void bitwiseShiftRight(GenericIntervalLattice other) {
    if(binaryOperationOnBot(other)) {
      return;
    } else if(!isLowInf() 
              && !other.isLowInf() 
              && !isHighInf() 
              && !other.isHighInf()) {
      Type n1=_low;
      Type n2=_high;
      Type n3=other._low;
      Type n4=other._high;
      Type nmin=std::min(n1>>n3,std::min(n1>>n4,std::min(n2>>n3,n2>>n4)));
      Type nmax=std::max(n1>>n3,std::max(n1>>n4,std::max(n2>>n3,n2>>n4)));
      setLow(nmin);
      setHigh(nmax);
      return;
    } else {
      setIsLowInf(true);
      setIsHighInf(true);
    }
  }

  static BoolLatticeType isEqual(GenericIntervalLattice l1, GenericIntervalLattice l2) {
    if(l1.isConst()&&l2.isConst()) {
      return BoolLatticeType(l1.getConst()==l2.getConst());
    }
    if(haveOverlap(l1,l2)) {
      // overlap with more than one value we do not know
      return BoolLatticeType(BoolLatticeType(CodeThorn::AType::Top()));
    } else {
      // no overlap mean they cannot be equal
      return BoolLatticeType(BoolLatticeType(false));
    }
  }

  static GenericIntervalLattice isEqualInterval(GenericIntervalLattice l1, GenericIntervalLattice l2) {
    BoolLatticeType res=isEqual(l1,l2);
    if(res.isTop()) { GenericIntervalLattice el; el.setTop(); return el; }
    else if(res.isTrue()) return GenericIntervalLattice(1);
    else if(res.isFalse()) return GenericIntervalLattice(0);
    else if(res.isBot()) { GenericIntervalLattice el; el.setBot(); return el; }
    else {
      std::cerr<<"GenericIntervalLattice: isEqualInterval: internal error."<<std::endl;
      exit(1);
    }
  }

  static GenericIntervalLattice isNotEqualInterval(GenericIntervalLattice l1, GenericIntervalLattice l2) {
    GenericIntervalLattice res=isEqualInterval(l1,l2);
    if(res.isTop()||res.isBot()) { return res; }
    else if(res.getConst()==0) { return GenericIntervalLattice(1); }
    else if(res.getConst()==1) { return GenericIntervalLattice(0); }
    else {
      std::cerr<<"GenericIntervalLattice: isNotEqualInterval: internal error."<<std::endl;
      exit(1);
    }
  }

  static BoolLatticeType isSmaller(GenericIntervalLattice l1, GenericIntervalLattice l2) {
    // 0. handle special case when both intervals are of length 1
    // 1. check for overlap (if yes, we do not know)
    // 2. if no overlap check bounds    
    if(l1.isConst()&&l2.isConst()) {
      return BoolLatticeType(l1.getConst()<l2.getConst());
    }
    if(haveOverlap(l1,l2)) {
      return BoolLatticeType(CodeThorn::AType::Top());
    } else {
      if(l1.isHighInf())
        return false;
      if(l1.isLowInf()&&!l2.isLowInf())
        return true;
      if(l2.isLowInf())
        return false;
      if(l2.isHighInf()&&!l1.isHighInf())
        return true;
      if(l1.getHigh()<l2.getLow())
	return BoolLatticeType(true);
      else
	return BoolLatticeType(false);
    }
  }

  static GenericIntervalLattice isSmallerInterval(GenericIntervalLattice l1, GenericIntervalLattice l2) {
    BoolLatticeType res = isSmaller(l1,l2);
    return GenericIntervalLattice::createFromBoolLattice(res);
  }

  static BoolLatticeType isSmallerOrEqual(GenericIntervalLattice l1, GenericIntervalLattice l2) {
    return isSmaller(l1,l2)||isEqual(l1,l2);
  }

  static GenericIntervalLattice isSmallerOrEqualInterval(GenericIntervalLattice l1, GenericIntervalLattice l2) {
    BoolLatticeType res = isSmallerOrEqual(l1,l2);
    return GenericIntervalLattice::createFromBoolLattice(res);
  }

  static BoolLatticeType isGreater(GenericIntervalLattice l1, GenericIntervalLattice l2) {
    // a > b <=> !(a <= b)
    return !isSmallerOrEqual(l1,l2);
  }

  static GenericIntervalLattice isGreaterInterval(GenericIntervalLattice l1, GenericIntervalLattice l2) {
    BoolLatticeType res = isGreater(l1,l2);
    return GenericIntervalLattice::createFromBoolLattice(res);
  }

  static BoolLatticeType isGreaterOrEqual(GenericIntervalLattice l1, GenericIntervalLattice l2) {
    // a >= b <=> !(a < b)
    return !isSmaller(l1,l2);
  }

  static GenericIntervalLattice isGreaterOrEqualInterval(GenericIntervalLattice l1, GenericIntervalLattice l2) {
    BoolLatticeType res = isGreaterOrEqual(l1,l2);
    return GenericIntervalLattice::createFromBoolLattice(res);
  }

  static GenericIntervalLattice logicalAndInterval(GenericIntervalLattice l1, GenericIntervalLattice l2) {
    BoolLatticeType res = l1.toBoolLattice() && l2.toBoolLattice();
    return GenericIntervalLattice::createFromBoolLattice(res);
  }

  static GenericIntervalLattice logicalOrInterval(GenericIntervalLattice l1, GenericIntervalLattice l2) {
    BoolLatticeType res = l1.toBoolLattice() || l2.toBoolLattice();
    return GenericIntervalLattice::createFromBoolLattice(res);
  }

  bool operator==(GenericIntervalLattice l2) {
    return (isTop() && l2.isTop())
    || (isBot() && l2.isBot())
    || (getLow()==l2.getLow() && getHigh()==l2.getHigh())
    ;
  }

 private:
  void setIsLowInf(bool val) {
    _isLowInf=val;
  }
  void setIsHighInf(bool val) {
    _isHighInf=val;
  }
  void unifyEmptyInterval() {
    if(!isLowInf()&&!isHighInf()&&_low>_high)
      setEmpty();
  }
  //protected:
 protected:
  Type _low;
  Type _high;
  bool _isLowInf;
  bool _isHighInf;
};

}

#endif
