
#ifndef OBSERVE_OBJECT
#define OBSERVE_OBJECT

#include <assert.h>
#include <iostream>
#include <PtrSet.h>

template <class Observer>
class ObserveInfo
{
 public:
  virtual ~ObserveInfo() {};
  virtual void UpdateObserver( Observer& o) const = 0;
  virtual void Dump() {};
};

template <class Observer>
class ObserveObject 
{
    typedef PtrSetWrap<Observer> ObSetType;
    ObSetType obList;
  public:
    typedef typename PtrSetWrap<Observer>::Iterator Iterator;
    virtual ~ObserveObject() {}
    void AttachObserver( Observer *o)
        { assert(!obList.IsMember(o) );
          obList.Add( o ); }
    void DetachObserver( Observer *o)
        { obList.Delete( o ); }

    Iterator GetIterator() const { return obList.GetIterator(); }
    void Notify( const ObserveInfo<Observer> &info) 
       {
        Iterator p = obList.GetIterator();
        Observer* o = 0;
        while ( !p.ReachEnd() ) {
           o = *p;
           p++;
           info.UpdateObserver( *o );
        }
       }
    unsigned NumberOfObservers() const { return obList.NumberOfEntries(); }
};  

#endif
