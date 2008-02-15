
#ifndef LATTICE_ELEM_LIST_H
#define LATTICE_ELEM_LIST_H

#include <DoublyLinkedList.h>
#include <stdlib.h>

template <class T>
class LatticeElemMerge
{
 public:
//virtual Boolean MergeElem(const T &e1, const T &e2, T& result)=0;
virtual int MergeElem(const T &e1, const T &e2, T& result)=0;
//virtual Boolean IsTop(const T& e) = 0;
virtual int IsTop(const T& e) = 0;
};

template <class T> 
class LatticeElemList : public DoublyLinkedListWrap<T>
{
  public:
   LatticeElemList<T> () {}
   LatticeElemList <T> (const LatticeElemList <T> &that)
     : DoublyLinkedListWrap<T> (that) {}

  typedef typename DoublyLinkedListWrap<T>::Iterator Iterator;
  DoublyLinkedListWrap<T>::First;

// Boolean AddElem( const T& _item, LatticeElemMerge<T> *Merge = 0)
   int AddElem( const T& _item, LatticeElemMerge<T> *Merge = 0)
   { if (Merge != 0 && Merge->IsTop(_item))
       return false;
  // Boolean mod = false, add = true;
     int mod = false, add = true;
     T item = _item;
     for (DoublyLinkedEntryWrap<T> *e = First(); e != 0; ) {
        DoublyLinkedEntryWrap<T>* tmp = e;
        e = Next(e);
        T& d = tmp->GetEntry();
        T result;
        if (Merge != 0 && Merge->MergeElem(d, item, result))  {
          if (result != d)  {
             Delete(tmp);
             mod = true;
             if (!Merge->IsTop(result))
                item = result;
             else {
                add = false;
                break;
             }
          }
          else {
             add = false;
             break;
          }
        }
    }
    if (add)  {
       AppendLast(item);
       mod = true;
    }
    return mod;
  }

//void UpdateElem( Boolean (*Update)(T & info) )
  void UpdateElem( int (*Update)(T & info) )
  {
    for (DoublyLinkedEntryWrap<T> *e = First(); e != 0; e = Next(e)) {
       T& d = e->GetEntry();
       if (! Update(d)) {
          DoublyLinkedEntryWrap<T> *tmp = Prev(e);
          Delete(e);
          e = (tmp == 0)? First() : tmp;
       }
    }
  }
//Boolean operator |= (const LatticeElemList <T> &that)
  int operator |= (const LatticeElemList <T> &that)
  {
// Boolean result = false;
   int result = false;
    for ( Iterator iter(that); !iter.ReachEnd(); iter++) {
      T &c = iter.Current();
      if (AddElem(c))
         result = true;
    }
    return result;
  }
};

#endif
