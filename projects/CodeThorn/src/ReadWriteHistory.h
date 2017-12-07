#ifndef READ_WRITE_HISTORY_H
#define READ_WRITE_HISTORY_H

#include "HashFun.h"
#include "HSetMaintainer.h"
#include "AbstractValue.h"

#include <stack>
#include <map>

using namespace CodeThorn;

namespace CodeThorn {

  class EState;

  typedef std::set<AbstractValue> MemLocAccessSet;

/*! 
 * \author Marc Jasper
 * \date 2017
 */
  struct ReadsWritesUnorderedBlock {
    MemLocAccessSet previousReads;
    MemLocAccessSet previousWrites;
    MemLocAccessSet currentReads;
    MemLocAccessSet currentWrites;
  };

  bool operator<(const ReadsWritesUnorderedBlock& s1, const ReadsWritesUnorderedBlock& s2);
  bool operator==(const ReadsWritesUnorderedBlock& s1, const ReadsWritesUnorderedBlock& s2);
  bool operator!=(const ReadsWritesUnorderedBlock& s1, const ReadsWritesUnorderedBlock& s2);

/*! 
  * \author Marc Jasper
  * \date 2017
 */
  class ReadWriteHistory {

  public:
    
    typedef std::stack<ReadsWritesUnorderedBlock> StackOfRWBlocks;

    std::string toString() const ;
    StackOfRWBlocks* stackOfUnorderedBlocks() { return &_stack; }
    const StackOfRWBlocks* stackOfUnorderedBlocks() const { return &_stack; }

  private:
    StackOfRWBlocks _stack;
  };

  bool operator<(const ReadWriteHistory& s1, const ReadWriteHistory& s2);
  bool operator==(const ReadWriteHistory& s1, const ReadWriteHistory& s2);
  bool operator!=(const ReadWriteHistory& s1, const ReadWriteHistory& s2);
  
  /*! 
   * \author Marc Jasper
   * \date 2017
   */
  class RWHistoryHashFun {
public:
    RWHistoryHashFun() {}
    //TODO: implement hash function
    long operator()(ReadWriteHistory* h) const {
      unsigned int hash=1;
      return long(hash);
    }
  };
  
  /*! 
   * \author Marc Jasper
   * \date 2017
   */
  class RWHistoryEqualToPred {
  public:
    RWHistoryEqualToPred() {}
    long operator()(ReadWriteHistory* s1, ReadWriteHistory* s2) const {
      return *s1==*s2;
    }
  };
  
  /*! 
   * \author Marc Jasper
   * \date 2017
   */
  class RWHistorySet : public HSetMaintainer<ReadWriteHistory,RWHistoryHashFun,RWHistoryEqualToPred> {
    
  public:
    typedef HSetMaintainer<ReadWriteHistory,RWHistoryHashFun,RWHistoryEqualToPred>::ProcessingResult ProcessingResult;
  }; 
 } // namespace CodeThorn
  
#endif
