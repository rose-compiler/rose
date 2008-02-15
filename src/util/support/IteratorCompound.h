
#ifndef ITERATORS_COMPOUND
#define ITERATORS_COMPOUND

template <class T, class TU, class Iterator, class Select>
class SelectIterator 
{
  Iterator iter;
  Select sel;
  void SetIterator()
   {
     while (!iter.ReachEnd() && !sel(iter.Current()) )
       iter.Advance();
   }
 public:
  SelectIterator( Iterator it, const Select& s) 
         : iter(it), sel(s) { SetIterator(); }
  SelectIterator( const Iterator& it) : iter(it), sel() { SetIterator(); }
  SelectIterator( const SelectIterator<T,TU,Iterator,Select>& that)
    : iter(that.iter), sel(that.sel) {}
  ~SelectIterator() {}

  void Advance() { iter.Advance(); SetIterator(); }
  void Reset() { iter.Reset(); SetIterator(); }
  T Current() const { return iter.Current(); }
  TU Current() { return iter.Current(); }
  void operator++() { Advance(); }
  void operator++(int) { Advance(); }
//Boolean ReachEnd() const { return iter.ReachEnd(); }
  int ReachEnd() const { return iter.ReachEnd(); }
};

template <class T1, class T2, class Iterator1, class Cross>
class SingleCrossIterator 
{
  Iterator1 iter;
  Cross cross;
 public:
  SingleCrossIterator(Iterator1 it, Cross c) : iter(it), cross(c) {}
  SingleCrossIterator(Iterator1 it) : iter(it), cross() {}
  SingleCrossIterator(const SingleCrossIterator& that)
     : iter(that.iter), cross(that.cross)  {}
  T2 Current() const { return cross(iter.Current()); }
  void Reset() { iter.Reset(); }
  void Advance() { iter.Advance(); }
  void operator++() { Advance(); }
  void operator++(int) { Advance(); }
//Boolean ReachEnd() const { return iter.ReachEnd(); }
  int ReachEnd() const { return iter.ReachEnd(); }
};

template <class T, class Iterator1, class Iterator2, class Cross>
class MultiCrossIterator 
{
  Iterator1 iter1;
  Iterator2 iter2;
  Cross cross;
  void SetIterator()
   {
     while (!iter1.ReachEnd()) {
      iter2 = cross(iter1.Current());
      if (!iter2.ReachEnd()) break;
      iter1.Advance();
     }
   }

 public:
  MultiCrossIterator( Iterator1 it, Cross c) 
     : iter1(it), cross(c) { SetIterator(); }
  MultiCrossIterator( Iterator1 it) : iter1(it),cross() { SetIterator(); }
  MultiCrossIterator( const MultiCrossIterator& that)
      : iter1(that.iter1), iter2(that.iter2), cross(that.cross)
       {  SetIterator(); }
  ~MultiCrossIterator() {}

  void Advance()
   {
     iter2.Advance();
     if (iter2.ReachEnd()) {
       iter1.Advance();
       SetIterator();
     }
   }
  void operator++() { Advance(); }
  void operator++(int) { Advance(); }
  void Reset() { iter1.Reset(); SetIterator(); }
  T Current()  const { return iter2.Current(); }
//Boolean ReachEnd() const { return iter1.ReachEnd() && iter2.ReachEnd(); }
  int ReachEnd() const { return iter1.ReachEnd() && iter2.ReachEnd(); }
};

template <class T, class TU, class Iterator>
class MultiIterator 
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
  TU Current() { return (iter.ReachEnd())? next.Current() : iter.Current(); }
//Boolean ReachEnd() const { return iter.ReachEnd() && next.ReachEnd();}
  int ReachEnd() const { return iter.ReachEnd() && next.ReachEnd();}
};

#endif

