#ifndef ITERATOR_TMPL
#define ITERATOR_TMPL

#include <CountRefHandle.h>
#include <assert.h>
#include <stdlib.h>
#include <string>

template <class T>
class IteratorImpl 
{
   virtual T CurrentImpl() const = 0;
   virtual IteratorImpl<T>* CloneImpl() const =0;
   virtual void AdvanceImpl() = 0;
   virtual void ResetImpl() = 0;
// virtual Boolean ReachEndImpl() const = 0;
   virtual int ReachEndImpl() const = 0;
 public:
   virtual ~IteratorImpl() {}
   T Current() const { assert(!ReachEnd()); return CurrentImpl(); }
   T operator *() const { return Current(); }
   void Advance() { AdvanceImpl(); }
   void Reset() { ResetImpl(); }
   void operator ++() { return Advance(); }
   void operator ++(int) { return Advance(); }
// Boolean ReachEnd() const { return ReachEndImpl(); }
   int ReachEnd() const { return ReachEndImpl(); }
   IteratorImpl<T>* Clone() const { return CloneImpl(); }
};

template <class T, class Iterator>
class IteratorImplTemplate : public IteratorImpl<T>
{
  Iterator impl;
  virtual T CurrentImpl() const { return impl.Current();}
  virtual void AdvanceImpl() { impl.Advance(); }
  virtual void ResetImpl() { impl.Reset(); }
  virtual IteratorImpl<T>* CloneImpl() const
            {return new IteratorImplTemplate<T,Iterator>(impl); }
//virtual Boolean ReachEndImpl() const { return impl.ReachEnd(); }
  virtual int ReachEndImpl() const { return impl.ReachEnd(); }
 public:
  IteratorImplTemplate( const Iterator& it) : impl(it) {}
  ~IteratorImplTemplate() {}
}; 

template <class T1, class T2>
class Iterator2Impl : public IteratorImpl<T1>
{
   virtual T1 CurrentImpl() const { return CurrentImpl2(); }
   virtual IteratorImpl<T1>* CloneImpl() const { return CloneImpl2(); }
   virtual T2 CurrentImpl2()  const = 0;
   virtual Iterator2Impl<T1,T2>* CloneImpl2() const = 0;
 public:
   virtual ~Iterator2Impl() {}
   IteratorImpl<T1>::ReachEnd;
   T2 Current() const { assert(!ReachEnd()); return CurrentImpl2(); }
   Iterator2Impl<T1,T2>* Clone() const { return  CloneImpl2(); }
};

template <class T1, class T2, class Iterator>
class Iterator2ImplTemplate : public Iterator2Impl<T1,T2> 
{
  Iterator impl;
  virtual T2 CurrentImpl2() const { return impl.Current();}
  virtual void AdvanceImpl() { impl.Advance(); }
  virtual void ResetImpl() { impl.Reset(); }
  virtual Iterator2Impl<T1,T2>* CloneImpl2() const
            {return new Iterator2ImplTemplate<T1,T2,Iterator>(impl); }
//virtual Boolean ReachEndImpl() const { return impl.ReachEnd(); }
  virtual int ReachEndImpl() const { return impl.ReachEnd(); }
 public:
  Iterator2ImplTemplate( const Iterator& it) : impl(it) {}
  ~Iterator2ImplTemplate() {}
};

template <class T>
class SingleIterator 
{
  T item; 
//Boolean end;
  int end;
 public:
  SingleIterator(T t) : item(t) { end = false; }  
  SingleIterator(const SingleIterator<T>& that) 
      : item(that.item),end(that.end) {}
  T Current() const { assert(!end); return item; }
  T operator *() const { return Current(); }
  void Advance() { end = true; }
  void Reset() { end = false; }
//Boolean ReachEnd() const { return end; }
  int ReachEnd() const { return end; }
};

template <class T, class Impl>
class IteratorWrap : public CountRefHandle <Impl>
{
 protected:
  CountRefHandle <Impl>::ConstRef;
  CountRefHandle <Impl>::ConstPtr;
  CountRefHandle <Impl>::UpdatePtr;
  CountRefHandle <Impl>::UpdateRef;
 public:
  IteratorWrap<T,Impl>& operator = (const IteratorWrap<T,Impl> &that)
     { CountRefHandle <Impl> :: operator=(that); return *this; }
  IteratorWrap(const IteratorWrap<T,Impl> &that)
    : CountRefHandle <Impl> (that) {}
  IteratorWrap() {}
  IteratorWrap(Impl *_impl)
    : CountRefHandle <Impl>(_impl) {}
  IteratorWrap(const Impl &_impl)
    : CountRefHandle <Impl>(_impl) {}
  ~IteratorWrap() {}

  T Current()  const
    { return ConstRef().Current(); }
  T operator *() const { return Current(); }
  void Advance() { Impl *impl = UpdatePtr();
                       if (impl) impl->Advance(); }
  void Reset() { Impl *impl = UpdatePtr();
                 if (impl) impl->Reset(); }
  void operator++() { Advance(); }
  void operator++(int) { Advance(); }
//Boolean ReachEnd() const { return (!ConstPtr() || ConstRef().ReachEnd()); }
  int ReachEnd() const { return (!ConstPtr() || ConstRef().ReachEnd()); }
//Boolean IsEmpty() const { return !ConstPtr(); }
  int IsEmpty() const { return !ConstPtr(); }
//Boolean operator == (const IteratorWrap<T,Impl> &that) const
  int operator == (const IteratorWrap<T,Impl> &that) const
    { return ConstPtr() == that.ConstPtr(); }
};

template <class T1, class T2>
class Iterator2Wrap : public IteratorWrap <T2, Iterator2Impl<T1,T2> >
{
 protected:
  IteratorWrap <T2, Iterator2Impl<T1,T2> >::ConstRef;
  IteratorWrap <T2, Iterator2Impl<T1,T2> >::ConstPtr;
  IteratorWrap <T2, Iterator2Impl<T1,T2> >::UpdatePtr;
  IteratorWrap <T2, Iterator2Impl<T1,T2> >::UpdateRef;
 public:
  typedef IteratorImpl<T1> Impl1;
  typedef Iterator2Impl<T1,T2> Impl2;

  Iterator2Wrap<T1,T2>& operator = (const Iterator2Wrap<T1,T2> &that)
     { IteratorWrap<T2,Impl2> :: operator=(that); return *this; }
  Iterator2Wrap(const Iterator2Wrap<T1,T2> &that)
    : IteratorWrap<T2,Impl2> (that) {}
  Iterator2Wrap() {}
  Iterator2Wrap(Impl2 *_impl) : IteratorWrap<T2,Impl2>(_impl) {}
  Iterator2Wrap(const Impl2 &_impl)
    : IteratorWrap<T2,Impl2>(_impl) {}
  ~Iterator2Wrap() {}

  operator IteratorWrap<T1, Impl1>() 
    { return (ConstPtr() == 0)? IteratorWrap<T1,Impl1>() 
                              : IteratorWrap<T1,Impl1>(ConstRef().Clone()); }

};

template <class Iter, class Update>
void UpdateEach(Iter iter, Update T)
{
  for ( ; !iter.ReachEnd(); iter++) {
     T(iter.Current());
  }
}

template <class Iter>
unsigned CountIteratorSize( Iter iter)
{
  unsigned size = 0;
  for ( Iter p = iter; !p.ReachEnd(); ++p,++size) {}
  return size;
} 

template <class Iter>
std::string IteratorToString1( Iter iter)
{
  std::string res;
  for ( Iter p = iter; !p.ReachEnd(); ++p)
     res = res + (*p).ToString();
  return res;
} 

template <class Iter>
std::string IteratorToString2( Iter iter)
{
  std::string res;
  for ( Iter p = iter; !p.ReachEnd(); ++p)
     res = res + (*p)->ToString();
  return res;
} 

template <class Iter, class Update>
// Boolean FindFirst(Iter& iter, Update T)
int FindFirst(Iter& iter, Update T)
{
  for ( ; !iter.ReachEnd(); iter++) {
     if (T(iter.Current()))
        return true;
  }
  return false;
}

#endif
