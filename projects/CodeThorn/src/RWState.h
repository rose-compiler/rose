#ifndef R_W_STATE_H
#define R_W_STATE_H

#include "sage3basic.h"
#include "EState.h"
#include "ReadWriteHistory.h"

using namespace CodeThorn;

#include "HashFun.h"
#include "HSetMaintainer.h"

namespace CodeThorn {

/*! 
  * \author Marc Jasper
  * \date 2017
 */
  class RWState {
  public:
    RWState():_eState(nullptr),_readWriteHistory(nullptr){}
    RWState(const EState* eState):_eState(eState),_readWriteHistory(nullptr){}
    RWState(const EState* eState, const ReadWriteHistory* rWHistory):_eState(eState),_readWriteHistory(rWHistory){}

    std::string toString() const ;
    void setReadWriteHistory(const ReadWriteHistory* rWHistory) { _readWriteHistory=rWHistory; }
    const ReadWriteHistory* readWriteHistory() const { return _readWriteHistory; }
    void setEState(const EState* eState) { _eState=eState; }
    const EState* eState() const { return _eState; }

  private:
    const EState* _eState;
    const ReadWriteHistory* _readWriteHistory;
  };

  // define order for RWState elements (necessary for RWStateSet)
  bool operator<(const RWState& s1, const RWState& s2);
  bool operator==(const RWState& s1, const RWState& s2);
  bool operator!=(const RWState& s1, const RWState& s2);
  
  /*! 
   * \author Marc Jasper
   * \date 2017
   */
  class RWStateHashFun {
  public:
    RWStateHashFun() {}
    long operator()(RWState* s) const {
      unsigned int hash=1;
      hash=((((long)s->eState())+1)*(((long)s->readWriteHistory())+1));
      return long(hash);
    }
  };
  
  /*! 
   * \author Marc Jasper
   * \date 2017
   */
  class RWStateEqualToPred {
  public:
    RWStateEqualToPred() {}
    long operator()(RWState* s1, RWState* s2) const {
      return *s1==*s2;
    }
  };
  
  /*! 
   * \author Marc Jasper
   * \date 2017
   */
  class RWStateSet : public HSetMaintainer<RWState,RWStateHashFun,RWStateEqualToPred> {
    
  public:
    typedef HSetMaintainer<RWState,RWStateHashFun,RWStateEqualToPred>::ProcessingResult ProcessingResult;
  };
  
} // namespace CodeThorn

#endif
