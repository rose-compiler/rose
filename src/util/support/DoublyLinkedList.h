
#ifndef DOUBLYLINKEDLIST_H
#define DOUBLYLINKEDLIST_H

#include <stdlib.h>
#include <assert.h>
#include <FunctionObject.h>
#include <iostream>

template <class T> class DoublyLinkedListWrap;

template <class T>
class DoublyLinkedEntryWrap 
{
   T *o;
   DoublyLinkedEntryWrap<T>* next, *prev;
 public:
   DoublyLinkedEntryWrap<T>() : o(0), next(0), prev(0) {}
   DoublyLinkedEntryWrap( const T& _o) : o(new T(_o)), next(0), prev(0) {}
   DoublyLinkedEntryWrap& operator = ( const T& _o)
         { 
           if (o == 0) 
              o = new T(_o);
           else 
              *o = _o; 
           return *this; 
         }
   ~DoublyLinkedEntryWrap() { if (o != 0) delete o; }
   T& GetEntry() { return *o; }
 friend class DoublyLinkedListWrap<T>;
};

template <class T>
class DoublyLinkedListWrap 
{
   DoublyLinkedEntryWrap<T> head;
   unsigned count;
 public:
   void LinkBefore( DoublyLinkedEntryWrap<T> *e, DoublyLinkedEntryWrap<T>* pos)
     {
       assert(e->prev == 0 && e->next == 0 && e->o != 0);    
       ++count;
       e->next = pos;
       e->prev = pos->prev;
       pos->prev->next = e;
       pos->prev = e;
     }
   void LinkAfter( DoublyLinkedEntryWrap<T> *e, DoublyLinkedEntryWrap<T>* pos)
     {
       assert(e->prev == 0 && e->next == 0 && e->o != 0);
       ++count;
       e->prev = pos;
       e->next = pos->next;
       pos->next->prev = e;
       pos->next = e;
     }
    void Unlink( DoublyLinkedEntryWrap<T>* e)
     {
       --count;
       if (e->prev != 0)
           e->prev->next = e->next;
       if (e->next != 0)
          e->next->prev = e->prev;
       e->prev = e->next = 0;
     }
    DoublyLinkedListWrap() : count(0) 
         { head.prev = head.next = &head; }
    DoublyLinkedListWrap(const DoublyLinkedListWrap<T> & that)
      : count(0)
    {
      head.prev = head.next = &head;
      for (iterator iter(that); !iter.ReachEnd(); iter++) {
         T &c = iter.Current();
         AppendLast(c);
      }
    }
    void operator = (const DoublyLinkedListWrap<T> & that) 
     {
      DeleteAll();
      for (const_iterator iter(that); !iter.ReachEnd(); iter++) {
         const T &c = iter.Current();
         AppendLast(c);
      }
     }
    ~DoublyLinkedListWrap() { DeleteAll(); }

    DoublyLinkedEntryWrap<T>* InsertBefore( const T& o , DoublyLinkedEntryWrap<T>* pos)
     {
       DoublyLinkedEntryWrap<T> *e = new DoublyLinkedEntryWrap<T>( o );
       LinkBefore(e, pos);
       return e;
     }
    DoublyLinkedEntryWrap<T>* InsertAfter( const T& o , DoublyLinkedEntryWrap<T>* pos)
     { 
       DoublyLinkedEntryWrap<T> *e = new DoublyLinkedEntryWrap<T>( o );
       LinkAfter(e, pos);
       return e;
     }
    DoublyLinkedEntryWrap<T>* AppendLast( const T& o )
     {
       DoublyLinkedEntryWrap<T> *e = new DoublyLinkedEntryWrap<T>( o );
       LinkAfter(e, head.prev);
       return e;
     }
    DoublyLinkedEntryWrap<T>* PushFirst( const T& o )
     { 
       DoublyLinkedEntryWrap<T> *e = new DoublyLinkedEntryWrap<T>( o );
       LinkBefore(e, head.next);
       return e;
     }
    void push_front(const T& o) { PushFirst(o); }

    void Sort( MapObject<T, int>& f)
     {
       DoublyLinkedEntryWrap<T> **buf = new DoublyLinkedEntryWrap<T>*[count];
       for (size_t i = 0; i < count; ++i)
            buf[i] = 0;
       for (DoublyLinkedEntryWrap<T> *p = First(); p; p = Next(p)) {
           unsigned index = f(p->GetEntry());
           while (index < count && buf[index] != 0)
               ++index;
           assert( index < count);
           buf[index] = p;
       }
       DoublyLinkedEntryWrap<T> *cur = buf[count-1];
       for ( int j = count-2; j >= 0; --j) {
          Unlink(buf[j]);
          LinkBefore( buf[j], cur);
          cur = buf[j];
       }
       delete [] buf;
     }

    void Sort( CompareObject<T> & f)
     { bool done = false;
       DoublyLinkedEntryWrap<T> *h = head.next;
       while (!done) {
         done = true;
         DoublyLinkedEntryWrap<T> *p = head.prev, *p1 = p->prev; 
         while (p != h) {
           if ( f (p->GetEntry(), p1->GetEntry()) < 0 ) {
              Unlink(p);
              LinkBefore(p, p1);
              done = false;
              if (p1 == h) 
                 break;
           }
           else
              p = p1;
           p1 = p->prev;
         } 
         h = p->next;
       }
     }

    void Delete( DoublyLinkedEntryWrap<T>* e)
     {
       Unlink(e);
       delete e;
     }
    void DeleteAll()
    {
       while (count)
           Delete(head.next);
    }
    void clear() { DeleteAll(); }

    unsigned NumberOfEntries() const { return count; }
    unsigned size() const { return count; }
    DoublyLinkedEntryWrap<T>* First() const 
          { 
            DoublyLinkedEntryWrap<T>* r = (count == 0)? 0 : head.next; 
            assert( r == 0 || r->o != 0); 
            return r;
          }
    T& front() const { return First()->GetEntry(); }
    DoublyLinkedEntryWrap<T>* Last() const 
          { 
            DoublyLinkedEntryWrap<T>* r = (count == 0)? 0 : head.prev; 
            assert( r == 0 || r->o != 0); return r;
          }
    T& back() const { return Last()->GetEntry(); }
         
    DoublyLinkedEntryWrap<T>* Next(const DoublyLinkedEntryWrap<T>* cur) const
          { 
            DoublyLinkedEntryWrap<T>* r = (cur == head.prev)? 0 : cur->next; 
            assert( r == 0 || r->o != 0); return r;
          }
    DoublyLinkedEntryWrap<T>* Prev(DoublyLinkedEntryWrap<T>* cur) const
          { DoublyLinkedEntryWrap<T>* r = (cur == head.next)? 0 : cur->prev; 
            assert( r == 0 || r->o != 0); return r;
          }

    void Reverse()
     {  if (count == 0)
              return;
        DoublyLinkedEntryWrap<T>* tmp = &head, *tmp1 = tmp->next;
        head.next = head.prev;
        head.prev = tmp1;
        while (tmp != head.next) {
          DoublyLinkedEntryWrap<T>* tmp2 = tmp;
          tmp = tmp1;
          tmp1 = tmp1->next;
          tmp->next = tmp2;
          tmp->prev = tmp1;
        }
     }
    void reverse() { Reverse(); }

    class Iterator 
    {
      const DoublyLinkedListWrap<T> *list;
      DoublyLinkedEntryWrap<T> *cur;
     public:
      Iterator(const DoublyLinkedListWrap<T> &l)
          : list(&l) { cur = l.First(); }
      Iterator(const DoublyLinkedListWrap<T> &l, DoublyLinkedEntryWrap<T>* c) 
          : list(&l), cur(c) {}
      bool operator == (const Iterator& that) const
            { return list == that.list && cur == that.cur; }
      bool operator != (const Iterator& that) const
             { return !operator ==(that); }
      Iterator(const Iterator& that) : list(that.list), cur(that.cur) {}
      Iterator() : list(0), cur(0) {}
      DoublyLinkedEntryWrap<T>* CurrentPtr() const { return cur; }
      bool ReachEnd() const 
        { return cur == 0; }
      void Reset() { if (list != 0) cur = list->First(); }
      void Advance() { if (cur != 0) cur = list->Next(cur); }
      void operator++() { Advance(); }
      void operator ++(int) { Advance(); }
    };
    class const_iterator : public Iterator
    { public:
      using Iterator::CurrentPtr;
      const_iterator(const Iterator& that) : Iterator(that) {}
      const_iterator() : Iterator() {}
      const_iterator(const DoublyLinkedListWrap<T> &l) : Iterator(l) {}
      const_iterator(const DoublyLinkedListWrap<T> &l, 
                    DoublyLinkedEntryWrap<T>* c) : Iterator(l,c) {}
      bool operator == (const const_iterator& that) const
            { return Iterator::operator==(that); }
      bool operator != (const const_iterator& that) const
             { return !operator ==(that); }
      const T& Current() const { return CurrentPtr()->GetEntry(); }
      const T& operator *() const { return Current(); }
    };
    class iterator : public Iterator
    { public:
      using Iterator::CurrentPtr;
      iterator(const Iterator& that) : Iterator(that) {}
      iterator() : Iterator() {}
      iterator(const DoublyLinkedListWrap<T> &l) : Iterator(l) {}
      iterator(const DoublyLinkedListWrap<T> &l, 
                    DoublyLinkedEntryWrap<T>* c) : Iterator(l,c) {}
      bool operator == (const iterator& that) const
            { return Iterator::operator==(that); }
      bool operator != (const iterator& that) const
             { return !operator ==(that); }
      T& Current() const { return CurrentPtr()->GetEntry(); }
      T& operator *() const { return Current(); }
    };

    const_iterator begin() const { return Iterator(*this); }
    const_iterator end() const { return Iterator(*this, 0); }
    iterator begin() { return Iterator(*this); }
    iterator end() { return Iterator(*this, 0); }

    void erase( iterator& p) { Delete(p.CurrentPtr()); }
    void push_back( const T& o ) { AppendLast(o); }
};

template <class T> 
void writeList( const DoublyLinkedListWrap<T>& list, std::ostream& out)
{ for ( typename DoublyLinkedListWrap<T>::Iterator iter(list); !iter.ReachEnd(); 
       iter.Advance()) 
     iter.Current().write(out);
}

template <class T>
class CollectDoublyLinkedList : public CollectObject<T>
{
  DoublyLinkedListWrap<T>& res;
 public:
  CollectDoublyLinkedList(DoublyLinkedListWrap<T>& r) : res(r) {}
  bool operator()(const T& cur)
   {
      res.AppendLast(cur);
      return true;
   }
};

#endif
