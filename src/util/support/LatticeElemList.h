
#ifndef LATTICE_ELEM_LIST_H
#define LATTICE_ELEM_LIST_H

#include <DoublyLinkedList.h>
#include <stdlib.h>

template <class T>
class LatticeElemMerge
{
 public:
virtual bool MergeElem(const T &e1, const T &e2, T& result)=0;
virtual bool IsTop(const T& e) = 0;
virtual ~LatticeElemMerge() {}
};

template <class T> 
class LatticeElemList : public DoublyLinkedListWrap<T>
{
  public:
   LatticeElemList<T> () {}
   LatticeElemList <T> (const LatticeElemList <T> &that)
     : DoublyLinkedListWrap<T> (that) {}

  typedef typename DoublyLinkedListWrap<T>::iterator iterator;
  typedef typename DoublyLinkedListWrap<T>::const_iterator const_iterator;
  DoublyLinkedListWrap<T>::First;

   bool AddElem( const T& _item, LatticeElemMerge<T> *Merge = 0)
   { if (Merge != 0 && Merge->IsTop(_item))
       return false;
     bool mod = false, add = true;
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

  void UpdateElem( bool (*Update)(T & info) )
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
  bool operator |= (const LatticeElemList <T> &that)
  {
   bool result = false;
    for ( iterator iter(that); !iter.ReachEnd(); iter++) {
      T &c = iter.Current();
      if (AddElem(c))
         result = true;
    }
    return result;
  }
};

#endif
