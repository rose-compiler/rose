#ifndef PAR_PRO_ESTATE_H
#define PAR_PRO_ESTATE_H

#include <vector>
#include <string>
#include "ConstraintRepresentation.h"

namespace CodeThorn {

/*! 
  * \author Marc Jasper
  * \date 2016.
 */

typedef std::vector<Label> ParProLabel;

class ParProEState {
  public:
  ParProEState():_parProLabel(ParProLabel()){}
  ParProEState(ParProLabel label):_parProLabel(label){}
  ParProLabel getLabel() const { return _parProLabel; }
  std::string toString() const;
  
  private:
  ParProLabel _parProLabel; 
};

// define order for ParProEState elements (necessary for ParProEStateSet)
bool operator<(const ParProEState& c1, const ParProEState& c2);
bool operator==(const ParProEState& c1, const ParProEState& c2);
bool operator!=(const ParProEState& c1, const ParProEState& c2);

struct ParProEStateLessComp {
  bool operator()(const ParProEState& c1, const ParProEState& c2) {
    return c1<c2;
  }
};

#ifdef USE_CUSTOM_HSET
class ParProEStateHashFun {
   public:
    ParProEStateHashFun(long prime=9999991) : tabSize(prime) {}
    long operator()(ParProEState s) const {
      unsigned int hash=1;
      ParProLabel label=s->getLabel();
      for (ParProLabel::iterator i=label.begin(); i!=label.end(); i++) {
	hash*=(1 + *i.getId());
      }
      return long(hash) % tabSize;
    }
    long tableSize() const { return tabSize;}
   private:
    long tabSize;
};
#else
class ParProEStateHashFun {
   public:
    ParProEStateHashFun() {}
    long operator()(ParProEState* s) const {
      unsigned int hash=1;
      ParProLabel label=s->getLabel();
      for (ParProLabel::iterator i=label.begin(); i!=label.end(); i++) {
	hash*=(1 + i->getId());
      }
      return long(hash);
    }
   private:
};
#endif
class ParProEStateEqualToPred {
   public:
    ParProEStateEqualToPred() {}
    long operator()(ParProEState* s1, ParProEState* s2) const {
      return *s1==*s2;
    }
   private:
};

class ParProEStateSet : public HSetMaintainer<ParProEState,ParProEStateHashFun,ParProEStateEqualToPred> {
  public:
  ParProEStateSet():HSetMaintainer<ParProEState,ParProEStateHashFun,ParProEStateEqualToPred>(),_constraintSetMaintainer(0){}
  ParProEStateSet(bool keepStates):HSetMaintainer<ParProEState,ParProEStateHashFun,ParProEStateEqualToPred>(keepStates),
                                   _constraintSetMaintainer(0){}
  public:
   typedef HSetMaintainer<ParProEState,ParProEStateHashFun,ParProEStateEqualToPred>::ProcessingResult ProcessingResult;
  private:
   CodeThorn::ConstraintSetMaintainer* _constraintSetMaintainer; 
};

} // namespace CodeThorn

#endif
