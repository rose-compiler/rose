#ifndef ITERATOR_TMPL
#define ITERATOR_TMPL

#include <CountRefHandle.h>
#include <assert.h>
#include <stdlib.h>
#include <string>

template <class T>
class IteratorImpl 
{
 public:
   virtual ~IteratorImpl() {}
   virtual T Current() const = 0;
   virtual void Advance() = 0;
   virtual void Reset() = 0;
   virtual bool ReachEnd() const = 0;
   virtual IteratorImpl<T>* Clone() const = 0;

   T operator *() const { return Current(); }
   void operator ++() {  Advance(); }
   void operator ++(int) {  Advance(); }
};

template <class T, class Iterator>
class IteratorImplTemplate : public IteratorImpl<T>
{
 protected:
  Iterator impl;
 public:
  IteratorImplTemplate( const Iterator& it) : impl(it) {}
  ~IteratorImplTemplate() {}

  virtual T Current() const { return impl.Current();}
  virtual void Advance() { impl.Advance(); }
  virtual void Reset() { impl.Reset(); }
  virtual bool ReachEnd() const { return impl.ReachEnd(); }
  virtual IteratorImpl<T>* Clone() const
            {return new IteratorImplTemplate<T,Iterator>(impl); }
}; 

template <class T>
class SingleIterator : public IteratorImpl<T>
{
  T item; 
  bool end;
 public:
  SingleIterator(T t) : item(t) { end = false; }  
  SingleIterator(const SingleIterator<T>& that) 
      : item(that.item),end(that.end) {}
  T Current() const { assert(!end); return item; }
  T operator *() const { return Current(); }
  void Advance() { end = true; }
  void Reset() { end = false; }
  bool ReachEnd() const { return end; }
  virtual IteratorImpl<T>* Clone() const
            {return new SingleIterator(*this); }
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
  bool ReachEnd() const { return (!ConstPtr() || ConstRef().ReachEnd()); }
  bool IsEmpty() const { return !ConstPtr(); }
  bool operator == (const IteratorWrap<T,Impl> &that) const
    { return ConstPtr() == that.ConstPtr(); }
};

template <class T, class Iterator>
class MultiIterator : public IteratorImpl<T>
{
  Iterator iter, next;
 public:
  MultiIterator(Iterator impl1, Iterator impl2)
     : iter(impl1), next(impl2) {}
  MultiIterator(const MultiIterator& that)
     : iter(that.iter), next(that.next) {}
  ~MultiIterator() {}

  void Advance()
     { if (!iter.ReachEnd()) iter.Advance();
       else next.Advance();
     }
  void operator++() { Advance(); }
  void operator++(int) { Advance(); }
  void Reset() { iter.Reset(); next.Reset(); }
  T Current() const
   { return (iter.ReachEnd())? next.Current() : iter.Current(); }
  T& Current() { return (iter.ReachEnd())? next.Current() : iter.Current(); }
  bool ReachEnd() const { return iter.ReachEnd() && next.ReachEnd();}
  IteratorImpl<T>* Clone() const 
          { return new MultiIterator<T,Iterator>(*this); }
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
  for ( Iter p = iter; !p.ReachEnd(); ++p,++size)
  {
  }
  return size;
} 

template <class Iter>
std::string IteratorToString1( Iter iter)
{
  std::string res;
  for ( Iter p = iter; !p.ReachEnd(); ++p)
     res = res + (*p).toString();
  return res;
} 

template <class Iter>
std::string IteratorToString2( Iter iter)
{
  std::string res;
  for ( Iter p = iter; !p.ReachEnd(); ++p)
     res = res + (*p)->toString();
  return res;
} 

template <class Iter, class Update>
bool FindFirst(Iter& iter, Update T)
{
  for ( ; !iter.ReachEnd(); iter++) {
     if (T(iter.Current()))
        return true;
  }
  return false;
}

#endif
