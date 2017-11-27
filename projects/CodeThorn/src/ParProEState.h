#ifndef PAR_PRO_ESTATE_H
#define PAR_PRO_ESTATE_H

#include <vector>
#include <string>
#include "ConstraintRepresentation.h"

namespace CodeThorn {

typedef std::vector<Label> ParProLabel;

/*!
 * \brief State which currently consists of a vector of 
 control flow labels (used for parallel process graphs).
 * \author Marc Jasper
 * \date 2016.
 */
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

/*!
 * \author Marc Jasper
 * \date 2016.
 */
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

/*!
 * \author Marc Jasper
 * \date 2016.
 */
class ParProEStateEqualToPred {
   public:
    ParProEStateEqualToPred() {}
    long operator()(ParProEState* s1, ParProEState* s2) const {
      return *s1==*s2;
    }
   private:
};

/*!
 * \author Marc Jasper
 * \date 2016.
 */
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
