#ifndef PTRSET_H
#define PTRSET_H

#include <set>
#include <FunctionObject.h>

class VoidPtrSet : public std::set<void*> {};

template <class T>
class PtrSetWrap  
{
  VoidPtrSet impl;
 public:
  PtrSetWrap() {}
  PtrSetWrap( const PtrSetWrap<T>& that) : impl(that.impl) {}
  PtrSetWrap<T>& operator = ( const PtrSetWrap<T>& that) { impl = that.impl; return *this; }
  ~PtrSetWrap() {}
  PtrSetWrap<T>* Clone() const { return new PtrSetWrap<T>(*this); }

// DQ (3/8/2006): Removed Boolean macro set to int from use in header files
//Boolean IsMember( T* t) const { return impl.find((void*)t) != impl.end(); }
  int IsMember( T* t) const { return impl.find((void*)t) != impl.end(); }
//Boolean operator()( T* t) const { return IsMember(t); }
  int operator()( T* t) const { return IsMember(t); }

  void Add(T* t) { impl.insert((void*)t); }
  void Delete(T* t) { 
      VoidPtrSet::const_iterator p = impl.find((void*)t);
      if ( p != impl.end())
         impl.erase(p); 
   }
  void operator &= (const PtrSetWrap<T>& that) 
    { VoidPtrSet::iterator p = impl.begin(); 
      while (p != impl.end()) {
         VoidPtrSet::iterator p1 = p;
         ++p;
         if ( that.impl.find(*p1) == that.impl.end())
            impl.erase(p1);
      }
    }
  void operator |= (const PtrSetWrap<T>& that)
    { impl.insert(that.impl.begin(), that.impl.end()); }
  void operator -= (const PtrSetWrap<T>& that)
    { for (VoidPtrSet::iterator p = that.impl.begin();
           p !=  that.impl.end(); ++p) {
         VoidPtrSet::iterator p1 = impl.find(*p);
         if (p1 != impl.end())
           impl.erase(p1);
      }
    }      
  void Clear() { impl.clear(); }

  unsigned NumberOfEntries() const { return impl.size(); }
  unsigned size() const { return impl.size(); }

  class Iterator 
  {
    const VoidPtrSet *impl;
    VoidPtrSet::iterator p;
    Iterator( const VoidPtrSet* _impl) 
            : impl(_impl) { Reset(); }
  public:
    ~Iterator() {}
    Iterator( const Iterator& that) 
     : impl(that.impl), p(that.p) {}
    Iterator& operator = (const Iterator& that)
      { impl = that.impl; p = that.p; return *this; }

    T* Current() const {  return (p == impl->end())? 0 : ((T*)*p); }
    T* operator *() const { return Current(); }
    void Reset() { p = (impl->empty())? impl->end() : impl->begin(); }
    void Advance() { if (p != impl->end()) ++p; } 
    void operator ++() { Advance(); }
    void operator ++(int) { Advance(); }
 // Boolean ReachEnd() const { std::cerr << ""; return p == impl->end(); }
    int ReachEnd() const { std::cerr << ""; return p == impl->end(); }
    friend class PtrSetWrap<T>;
  };

  Iterator GetIterator() const { return Iterator(&impl); }
};

template<class T>
class AppendSTLSet : public CollectObject<T>
{
  std::set<T>& res;
 public:
  AppendSTLSet( std::set<T>& r) : res(r) {}
// Boolean operator()(const T& cur) 
  int operator()(const T& cur) 
   {
      if (res.find(cur) != res.end())
          return false;
      res.insert(cur);
      return true;
   }
};
template<class T>
class AppendPtrSet : public CollectObject<T*>
{
  PtrSetWrap<T>& res;
 public:
  AppendPtrSet( PtrSetWrap<T>& r) : res(r) {}
// Boolean operator()(T* const& cur) 
  int operator()(T* const& cur) 
   {
      if (res.IsMember(cur))
          return false;
      res.Add(cur);
      return true;
   }
};


template<class T>
class SelectSTLSet : public SelectObject<T>
{
  std::set<T> res;
 public:
  SelectSTLSet( const std::set<T> r) : res(r) {}
// Boolean operator()(const T& cur) 
  int operator()(const T& cur) 
   {
      return (res.find(cur) != res.end());
   }
};
template<class T>
class SelectPtrSet : public SelectObject<T*>
{
  PtrSetWrap<T> res;
 public:
  SelectPtrSet( const PtrSetWrap<T>& r) : res(r) {}
// Boolean operator()(T* const& cur)  const
  int operator()(T* const& cur)  const
   {
      return (res.IsMember(cur));
   }
};
template<class T>
class NotSelectPtrSet : public SelectObject<T*>
{
  PtrSetWrap<T> res;
 public:
  NotSelectPtrSet( const PtrSetWrap<T>& r) : res(r) {}
// Boolean operator()(T* const& cur)  const
  int operator()(T* const& cur)  const
   {
      return !(res.IsMember(cur));
   }
};
#endif
