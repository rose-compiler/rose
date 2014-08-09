#ifndef HSET_MAINTAINER_H
#define HSET_MAINTAINER_H

/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/
//#define USE_CUSTOM_HSET
#ifdef USE_CUSTOM_HSET
#include "HSet.h"
using namespace br_stl;
#else
#include <boost/unordered_set.hpp>
#endif

//#include "/usr/include/valgrind/memcheck.h"
//#define HSET_MAINTAINER_DEBUG_MODE

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
template<typename KeyType,typename HashFun, typename EqualToPred>
class HSetMaintainer 
#ifdef USE_CUSTOM_HSET
  : public HSet<KeyType*,HashFun,EqualToPred>
#else
  : public boost::unordered_set<KeyType*,HashFun,EqualToPred>
#endif
  {
public:
  typedef pair<bool,const KeyType*> ProcessingResult;
  bool exists(KeyType& s) { 
    return determine(s)!=0;
  }

  size_t id(const KeyType& s) {
#ifdef USE_CUSTOM_HSET
    typename HSet<KeyType*,HashFun,EqualToPred>::const_iterator i;
#else
    typename boost::unordered_set<KeyType*,HashFun,EqualToPred>::const_iterator i;
#endif
    i=HSetMaintainer<KeyType,HashFun,EqualToPred>::find(s);
    if(i!=HSetMaintainer<KeyType,HashFun,EqualToPred>::end()) {
      // in lack of operator '-' we compute the distance
      size_t pos=0;
#ifdef USE_CUSTOM_HSET
      typename HSet<KeyType*,HashFun,EqualToPred>::const_iterator b;
#else
      typename boost::unordered_set<KeyType*,HashFun,EqualToPred>::const_iterator b;
#endif
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
#ifdef USE_CUSTOM_HSET
      i=HSetMaintainer<KeyType,HashFun,EqualToPred>::find(s);
      if(i!=HSetMaintainer<KeyType,HashFun,EqualToPred>::end()) {
        ret=const_cast<KeyType*>(&(*i));
      } else {
        ret=0;
      }
#else
      i=HSetMaintainer<KeyType,HashFun,EqualToPred>::find(&s);
      if(i!=HSetMaintainer<KeyType,HashFun,EqualToPred>::end()) {
        ret=const_cast<KeyType*>(*i);
      } else {
        ret=0;
      }
#endif
    }
    return ret;
  }

  const KeyType* determine(const KeyType& s) { 
    const KeyType* ret=0;
    typename HSetMaintainer<KeyType,HashFun,EqualToPred>::iterator i;
#pragma omp critical(HASHSET)
    {
#ifdef USE_CUSTOM_HSET
      i=HSetMaintainer<KeyType,HashFun,EqualToPred>::find(s);
      if(i!=HSetMaintainer<KeyType,HashFun,EqualToPred>::end()) {
        ret=&(*i);
      } else {
        ret=0;
      }
#else
      i=HSetMaintainer<KeyType,HashFun,EqualToPred>::find(const_cast<KeyType*>(&s));
      if(i!=HSetMaintainer<KeyType,HashFun,EqualToPred>::end()) {
        ret=const_cast<KeyType*>(*i);
      } else {
        ret=0;
      }
#endif
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
        res=make_pair(iter,false);
      } else {
        res=insert(const_cast<KeyType*>(key)); // TODO: eliminate const_cast
      }
      res2=make_pair(res.second,*res.first);
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
      res=make_pair(iter,false);
    } else {
      // converting the stack allocated object to heap allocated
      // this copies the entire object
      // TODO: this can be avoided by providing a process function with a pointer arg
      //       this requires a more detailed result: pointer exists, alternate pointer with equal object exists, does not exist
      KeyType* keyPtr=new KeyType();
      *keyPtr=key;
      res=insert(keyPtr);
    }
#ifdef HSET_MAINTAINER_DEBUG_MODE
    std::pair<typename HSetMaintainer::iterator, bool> res1;
    res1=insert(key);
    std::pair<typename HSetMaintainer::iterator, bool> res2;
    res2=insert(key);
    if(!(res1==res2)) {
      cerr<< "Error: HsetMaintainer failed:"<<endl;
      cerr<< "res1:"<<(*res1.first).toString()<<":"<<res1.second<<endl;
      cerr<< "res2:"<<(*res2.first).toString()<<":"<<res2.second<<endl;
      exit(1);
    }
    cerr << "HSET insert OK"<<endl;
#endif
    res2=make_pair(res.second,*res.first);
    }
    return res2;
  }
  const KeyType* processNew(KeyType& s) {
    //std::pair<typename HSetMaintainer::iterator, bool> res=process(s);
    ProcessingResult res=process(s);
    if(res.first!=true) {
      cerr<< "Error: HsetMaintainer::processNew failed:"<<endl;
      cerr<< "res:";
      cout <<":"<<res.first<<endl;
      cout <<res.second->toString();
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
#ifdef USE_CUSTOM_HSET
    return HSetMaintainer<KeyType,HashFun,EqualToPred>::max_collisions();
#else
    //MS:2012
    size_t max=0;
    for(size_t i=0; i<HSetMaintainer<KeyType,HashFun,EqualToPred>::bucket_count();++i) {
      if(HSetMaintainer<KeyType,HashFun,EqualToPred>::bucket_size(i)>max) {
        max=HSetMaintainer<KeyType,HashFun,EqualToPred>::bucket_size(i);
      }
    }
    return max;
#endif
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
    }
    return mem+sizeof(*this);
  }

 private:
  //const KeyType* ptr(KeyType& s) {}
};

#endif
