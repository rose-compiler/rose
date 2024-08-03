#ifndef PTRMAP_H
#define PTRMAP_H

#include <map>
#include <iostream>

struct PointerHolder
{
  void* p;
  PointerHolder() : p(0) { }
  PointerHolder( void *_p) : p(_p) {}
};

class VoidPtrMap : public std::map<const void*, PointerHolder, std::less<const void*> > {};

template <class T1, class T2>
class PtrMapWrap 
{
  VoidPtrMap *impl;
  PtrMapWrap( const PtrMapWrap<T1,T2>& that) : impl{that.impl} {}
  void operator = (const PtrMapWrap<T1,T2> &) {}
 public:
  PtrMapWrap() { impl = new VoidPtrMap; }
  ~PtrMapWrap() { delete impl; }
  T2* Map(const T1* t) const { return (T2*) (*impl)[t].p; }
  void InsertMapping( const T1* t1, T2* t2) 
       { (*impl)[t1] = PointerHolder(t2); }
  void RemoveMapping( const T1 *t) { (*impl)[t].p = 0; }

  class Iterator
  {
    const VoidPtrMap *impl;
    VoidPtrMap::const_iterator p;
    Iterator( const VoidPtrMap* _impl)
            : impl(_impl) { Reset(); }
    void SetCurrent() 
     {
        while (p != impl->end() && (*p).second.p == 0)
           ++p;
     }
  public:
    ~Iterator() {}
    Iterator( const Iterator& that)
     : impl(that.impl), p(that.p) {}
    Iterator& operator = (const Iterator& that)
      { impl = that.impl; p = that.p; return *this; }

    const T1* Current() const 
       {
      // DQ (11/3/2011): Warning about type qualifier is meaningless on cast type (caught by new EDG compiling ROSE).
      // return (p == impl->end())? 0 : reinterpret_cast<const T1* const>((*p).first);
         return (p == impl->end())? 0 : reinterpret_cast<const T1*>((*p).first);
       }

    T2* Current2() const { return (p == impl->end())? 0 : reinterpret_cast<T2*>((*p).second.p); }
    const T1* operator *() const { return Current(); }
    void Reset() 
       { 
         if (impl->empty())
            p = impl->end() ;
         else {
            p = impl->begin(); 
            SetCurrent(); 
         }
       }
    void Advance() { if (p != impl->end()) { ++p; SetCurrent(); } }
    void operator ++() { Advance(); }
    void operator ++(int) { Advance(); }
    bool ReachEnd() const { std::cerr << ""; return p == impl->end(); }
    friend class PtrMapWrap<T1,T2>;
  };

  Iterator GetIterator() const { return Iterator(impl); }

};

#endif
