#ifndef PTRSET_H
#define PTRSET_H

#include <set>
#include <FunctionObject.h>

class VoidPtrSet : public std::set<void*, std::less<void*> > {};
//! A custom container based on std::set for arbitrary pointers of type T
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

  class Iterator 
  {
    const VoidPtrSet *impl;
    VoidPtrSet::const_iterator p;
    Iterator( const VoidPtrSet* _impl) 
            : impl(_impl) { Reset(); }
    Iterator( const VoidPtrSet* _impl, const VoidPtrSet::iterator _p) 
            : impl(_impl), p(_p) {}
   protected:
    T* Current() const {  return (p == impl->end())? 0 : ((T*)*p); }
    T*& Current() {  return ((T*&)*p); }
  public:
    ~Iterator() {}
    Iterator( const Iterator& that) 
     : impl(that.impl), p(that.p) {}
    bool operator == (const Iterator& that) const
      { return impl == that.impl && p == that.p; }
    Iterator& operator = (const Iterator& that)
      { impl = that.impl; p = that.p; return *this; }

    void Reset() { p = (impl->empty())? impl->end() : impl->begin(); }
    void Advance() { if (p != impl->end()) ++p; } 
    void operator ++() { Advance(); }
    void operator ++(int) { Advance(); }
    bool ReachEnd() const { return p == impl->end(); }
    friend class PtrSetWrap<T>;
  };
  class const_iterator : public Iterator {
   public:
    const_iterator( const Iterator& that) 
     : Iterator(that) {}
    bool operator == (const const_iterator& that) const
      { return Iterator::operator==(that); }
    const_iterator& operator = (const const_iterator& that)
      { Iterator::operator=(that); }
    T* Current() const {  return Iterator::Current(); }
    T* operator *() const { return Current(); }
  };
  class iterator : public Iterator {
   public:
    iterator( const Iterator& that) : Iterator(that) {}
    bool operator == (const iterator& that) const
      { return Iterator::operator==(that); }
    iterator& operator = (const iterator& that)
      { Iterator::operator=(that); }
    T*& Current() {  return Iterator::Current(); }
    T*& operator *() { return Current(); }
  };

  const_iterator begin() const { return Iterator(&impl); }
#if _MSC_VER
//#if ROSE_MICROSOFT_OS
  const_iterator end() const { return Iterator(&impl); }
#else
  const_iterator end() const { return Iterator(&impl, impl.end()); }
#endif
  const_iterator find(const T* t) const { return Iterator(&impl,impl.find((void*)t)); }
  iterator begin() { return Iterator(&impl); }
  iterator end() { return Iterator(&impl, impl.end()); }

  bool IsMember( const T* t) const 
           { return impl.find((void*)t) != impl.end(); }

  void insert(T* t) { impl.insert((void*)t); }
  void erase(T* t) { 
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
    {
#ifdef _MSC_VER
	  for (VoidPtrSet::const_iterator p = that.impl.begin(); p !=  that.impl.end(); ++p) 
#else
	  for (VoidPtrSet::iterator p = that.impl.begin(); p !=  that.impl.end(); ++p) 
#endif
             {
           VoidPtrSet::iterator p1 = impl.find(*p);
           if (p1 != impl.end())
                impl.erase(p1);
         }
    }      
  void Clear() { impl.clear(); }

  unsigned NumberOfEntries() const { return impl.size(); }
  unsigned size() const { return impl.size(); }

};

template<class T>
class AppendSTLSet : public CollectObject<T>
{
  std::set<T, std::less<T> >& res;
 public:
  AppendSTLSet( std::set<T, std::less<T> >& r) : res(r) {}
  bool operator()(const T& cur) 
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
  bool operator()(T* const& cur) 
   {
      if (res.IsMember(cur))
          return false;
      res.insert(cur);
      return true;
   }
};


// DQ (11/25/2009): Changed name from SelectObject to SelectObjectBase to avoid SelectObject function ambiguity using Microsoft Visual Studio
template<class T>
class SelectSTLSet : public SelectObjectBase<T>
{
  std::set<T> res;
 public:
  SelectSTLSet( const std::set<T> r) : res(r) {}
  bool operator()(const T& cur) 
   {
      return (res.find(cur) != res.end());
   }
};

// DQ (11/25/2009): Changed name from SelectObject to SelectObjectBase to avoid SelectObject function ambiguity using Microsoft Visual Studio
template<class T>
class SelectPtrSet : public SelectObjectBase<T*>
{
  PtrSetWrap<T> res;
 public:
  SelectPtrSet() {}
  SelectPtrSet( const PtrSetWrap<T>& r) : res(r) {}
  typedef typename PtrSetWrap<T>::const_iterator const_iterator;
  const_iterator begin() const { return res.begin(); }
  const_iterator end() const { return res.end(); }
  
  bool operator()(T* const& cur)  const
   {
      return (res.IsMember(cur));
   }
};

// DQ (11/25/2009): Changed name from SelectObject to SelectObjectBase to avoid SelectObject function ambiguity using Microsoft Visual Studio
template<class T>
class NotSelectPtrSet : public SelectObjectBase<T*>
{
  PtrSetWrap<T> res;
 public:
  NotSelectPtrSet( const PtrSetWrap<T>& r) : res(r) {}
  bool operator()(T* const& cur)  const
   {
      return !(res.IsMember(cur));
   }
};
#endif
