#ifndef FUNCTIONAL_OBJECT_H
#define FUNCTIONAL_OBJECT_H

// DQ (3/8/2006): Change "Boolean" to "int" explicit to avoid 
// this in our header files and having it effect other code.
// #define Boolean int

template <class T> class SelectObject
{
  public:
// virtual Boolean operator()(const T& t) const = 0;
   virtual int operator()(const T& t) const = 0;
};

template <class T> class SelectObject1 : public SelectObject<T>
{
   T sel;
  public:
   SelectObject1(const T& t) : sel(t) {}
// virtual Boolean operator()(const T& t) const 
   virtual int operator()(const T& t) const 
    { return t == sel; }
};

template <class T> class SelectObject2 : public SelectObject<T>
{
   T sel1, sel2;
  public:
   SelectObject2(const T& t1, const T& t2) : sel1(t1), sel2(t2) {}
// virtual Boolean operator()(const T& t) const 
   virtual int operator()(const T& t) const 
    { return t == sel1 || t == sel2; }
};

template <class T1, class T2> 
class Select2Object
{
  public:
// virtual Boolean operator()(const T1& t1, const T2& t2) const  = 0;
   virtual int operator()(const T1& t1, const T2& t2) const  = 0;
};

template <class T> class CollectObject
{
  public:
// virtual Boolean operator()(const T& t) = 0;
   virtual int operator()(const T& t) = 0;
};

template <class T1, class T2> 
class Collect2Object
{
  public:
// virtual Boolean operator()(const T1& t1, const T2& t2) = 0;
   virtual int operator()(const T1& t1, const T2& t2) = 0;
};

template <class T1, class T2> class MapObject
{ public:
   virtual T2 operator()( const T1& t) = 0;
};

template <class T1, class T2, class T3> class Map2Object
{ public:
   virtual T3 operator()( const T1& t1, const T2& t2) = 0;
};

template <class T> class CompareObject
{ public:
   virtual int operator()( const T& t1, const T& t2) = 0;
};

#endif
