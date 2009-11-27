#ifndef FUNCTIONAL_OBJECT_H
#define FUNCTIONAL_OBJECT_H

// DQ (11/25/2009): Changed name from SelectObject to SelectObjectBase to avoid SelectObject function ambiguity using Microsoft Visual Studio
template <class T> class SelectObjectBase
{
  public:
   virtual bool operator()(const T& t) const = 0;
   virtual ~SelectObjectBase() {}
};

template <class T> class SelectObject1 : public SelectObjectBase<T>
{
   T sel;
  public:
   SelectObject1(const T& t) : sel(t) {}
   virtual bool operator()(const T& t) const 
    { return t == sel; }
};

template <class T> class SelectObject2 : public SelectObjectBase<T>
{
   T sel1, sel2;
  public:
   SelectObject2(const T& t1, const T& t2) : sel1(t1), sel2(t2) {}
   virtual bool operator()(const T& t) const 
    { return t == sel1 || t == sel2; }
};

template <class T1, class T2> 
class Select2Object
{
  public:
   virtual bool operator()(const T1& t1, const T2& t2) const  = 0;
   virtual ~Select2Object() {}
};
//!A container interface to add elements only
// operator() does the adding operation and its implementation should be provided in its sub classes, such as CollectDoublyLinkedList
template <class T> class CollectObject
{
  public:
   virtual bool operator()(const T& t) = 0;
   virtual ~CollectObject() {}
};

template <class T1, class T2> 
class Collect2Object
{
  public:
   virtual bool operator()(const T1& t1, const T2& t2) = 0;
   virtual ~Collect2Object() {}
};

template <class T1, class T2> class MapObject
{ public:
   virtual T2 operator()( const T1& t) = 0;
   virtual ~MapObject() {}
};

template <class T1, class T2, class T3> class Map2Object
{ public:
   virtual T3 operator()( const T1& t1, const T2& t2) = 0;
   virtual ~Map2Object() {}
};

template <class T> class CompareObject
{ public:
   virtual int operator()( const T& t1, const T& t2) = 0;
   virtual ~CompareObject() {}
};

#endif
