#ifndef HSET_MAINTAINER_H
#define HSET_MAINTAINER_H

/*************************************************************
 * Author   : Markus Schordan                                *
 *************************************************************/
#include <boost/unordered_set.hpp>

//#define HSET_MAINTAINER_DEBUG_MODE

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
template<typename KeyType,typename HashFun, typename EqualToPred>
class HSetMaintainer 
  : public boost::unordered_set<KeyType*,HashFun,EqualToPred>
  {
public:
  typedef std::pair<bool,const KeyType*> ProcessingResult;

  /*! 
   * \author Marc Jasper
   * \date 2016.
   */
  HSetMaintainer() { _keepStatesDuringDeconstruction = false; }

  /*! 
   * \author Marc Jasper
   * \date 2016.
   */
  HSetMaintainer(bool keepStates) { _keepStatesDuringDeconstruction = keepStates; }

  /*! 
   * \author Marc Jasper
   * \date 2016.
   */
  virtual ~HSetMaintainer() {
    if (!_keepStatesDuringDeconstruction){
      typename HSetMaintainer::iterator i;
      for (i=this->begin(); i!=this->end(); ++i) {
	delete (*i);
      } 
    }
  }

  bool exists(KeyType& s) { 
    return determine(s)!=0;
  }

  size_t id(const KeyType& s) {
    typename boost::unordered_set<KeyType*,HashFun,EqualToPred>::const_iterator i;
    i=HSetMaintainer<KeyType,HashFun,EqualToPred>::find(s);
    if(i!=HSetMaintainer<KeyType,HashFun,EqualToPred>::end()) {
      // in lack of operator '-' we compute the distance
      size_t pos=0;
      typename boost::unordered_set<KeyType*,HashFun,EqualToPred>::const_iterator b;
      b=HSetMaintainer<KeyType,HashFun,EqualToPred>::begin();
      while(b!=i) {
        pos++;
        ++b;
      }
      return pos;
    }
    else
      throw "Error: unknown value. Maintainer cannot determine an id.";
  }

  typename HSetMaintainer<KeyType,HashFun,EqualToPred>::iterator i;

  KeyType* determine(KeyType& s) { 
    KeyType* ret=0;
    typename HSetMaintainer<KeyType,HashFun,EqualToPred>::iterator i;
#pragma omp critical(HASHSET)
    {
      i=HSetMaintainer<KeyType,HashFun,EqualToPred>::find(&s);
      if(i!=HSetMaintainer<KeyType,HashFun,EqualToPred>::end()) {
        ret=const_cast<KeyType*>(*i);
      } else {
        ret=0;
      }
    }
    return ret;
  }

  const KeyType* determine(const KeyType& s) { 
    const KeyType* ret=0;
    typename HSetMaintainer<KeyType,HashFun,EqualToPred>::iterator i;
#pragma omp critical(HASHSET)
    {
      i=HSetMaintainer<KeyType,HashFun,EqualToPred>::find(const_cast<KeyType*>(&s));
      if(i!=HSetMaintainer<KeyType,HashFun,EqualToPred>::end()) {
        ret=const_cast<KeyType*>(*i);
      } else {
        ret=0;
      }
    }
    return ret;
  }

  ProcessingResult process(const KeyType* key) {
    ProcessingResult res2;
#pragma omp critical(HASHSET)
    {
      std::pair<typename HSetMaintainer::iterator, bool> res;
      typename HSetMaintainer::iterator iter=this->find(const_cast<KeyType*>(key)); // TODO: eliminate const_cast
      if(iter!=this->end()) {
        // found it!
        res=std::make_pair(iter,false);
      } else {
        res=this->insert(const_cast<KeyType*>(key)); // TODO: eliminate const_cast
      }
      res2=std::make_pair(res.second,*res.first);
    }
    return res2;
  }
  const KeyType* processNewOrExisting(const KeyType* s) {
    ProcessingResult res=process(s);
    return res.second;
  }

  //! <true,const KeyType> if new element was inserted
  //! <false,const KeyType> if element already existed
  ProcessingResult process(KeyType key) {
    ProcessingResult res2;
#pragma omp critical(HASHSET)
    {
    std::pair<typename HSetMaintainer::iterator, bool> res;
    typename HSetMaintainer::iterator iter=this->find(&key);
    if(iter!=this->end()) {
      // found it!
      res=std::make_pair(iter,false);
    } else {
      // converting the stack allocated object to heap allocated
      // this copies the entire object
      // TODO: this can be avoided by providing a process function with a pointer arg
      //       this requires a more detailed result: pointer exists, alternate pointer with equal object exists, does not exist
      KeyType* keyPtr=new KeyType();
      *keyPtr=key;
      res=this->insert(keyPtr);
      if (!res.second) {
	// this case should never occur, condition "iter!=this->end()" above would have been satisfied and 
	// this else branch would have therefore been ignored
	std::cerr << "ERROR: HSetMaintainer: Element was not inserted even though it could not be found in the set." << std::endl;
	ROSE_ASSERT(0);
	delete keyPtr;
	keyPtr = NULL; 
      } 
    }
#ifdef HSET_MAINTAINER_DEBUG_MODE
    std::pair<typename HSetMaintainer::iterator, bool> res1;
    res1=this->insert(key);
    std::pair<typename HSetMaintainer::iterator, bool> res2;
    res2=this->insert(key);
    if(!(res1==res2)) {
      std::cerr<< "Error: HsetMaintainer failed:"<<std::endl;
      std::cerr<< "res1:"<<(*res1.first).toString()<<":"<<res1.second<<std::endl;
      std::cerr<< "res2:"<<(*res2.first).toString()<<":"<<res2.second<<std::endl;
      exit(1);
    }
    std::cerr << "HSET insert OK"<<std::endl;
#endif
    res2=std::make_pair(res.second,*res.first);
    }
    return res2;
  }

  const KeyType* processNew(KeyType& s) {
    //std::pair<typename HSetMaintainer::iterator, bool> res=process(s);
    ProcessingResult res=process(s);
    if(res.first!=true) {
      std::cerr<< "Error: HsetMaintainer::processNew failed:"<<std::endl;
      std::cerr<< "res:";
      std::cout <<":"<<res.first<<std::endl;
      std::cout <<res.second->toString();
      exit(1);
    }
    return res.second;
  }

  const KeyType* processNewOrExisting(KeyType& s) {
    ProcessingResult res=process(s);
    return res.second;
  }

  long numberOf() { return HSetMaintainer<KeyType,HashFun,EqualToPred>::size(); }

  long maxCollisions() {
    size_t max=0;
    for(size_t i=0; i<HSetMaintainer<KeyType,HashFun,EqualToPred>::bucket_count();++i) {
      if(HSetMaintainer<KeyType,HashFun,EqualToPred>::bucket_size(i)>max) {
        max=HSetMaintainer<KeyType,HashFun,EqualToPred>::bucket_size(i);
      }
    }
    return max;
  }

  double loadFactor() {
    return HSetMaintainer<KeyType,HashFun,EqualToPred>::load_factor();
  }

  long memorySize() const {
    long mem=0;
    for(typename HSetMaintainer<KeyType,HashFun,EqualToPred>::const_iterator i
          =HSetMaintainer<KeyType,HashFun,EqualToPred>::begin();
        i!=HSetMaintainer<KeyType,HashFun,EqualToPred>::end();
        ++i) {
      mem+=(*i)->memorySize();
      mem+=sizeof(*i);
    }
    return mem+sizeof(*this);
  }

 private:
  //const KeyType* ptr(KeyType& s) {}
  bool _keepStatesDuringDeconstruction;
};

#endif
