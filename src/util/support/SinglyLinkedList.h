
#ifndef SINGLY_LINKED_LIST_H
#define SINGLY_LINKED_LIST_H

#include <FunctionObject.h>

template <class T> class SinglyLinkedListWrap;
template <class T>
class SinglyLinkedEntryWrap 
{
   T o;
   SinglyLinkedEntryWrap<T>* next;
 public:
   SinglyLinkedEntryWrap( const T& _o) : o(_o), next(0) {}
   ~SinglyLinkedEntryWrap() {}
   T& GetEntry() { return o; }
 friend class SinglyLinkedListWrap<T>;
};

template <class T>
class SinglyLinkedListWrap  
{
    SinglyLinkedEntryWrap<T>* head, *end;
    unsigned count;
    void operator = (const SinglyLinkedListWrap<T> & that) {}
  public:
    SinglyLinkedListWrap() : head(0), end(0), count(0) {}

    SinglyLinkedListWrap(const SinglyLinkedListWrap<T> & that)
    {
      head = end = 0;
      count = 0;
      *this += that;
    }
    ~SinglyLinkedListWrap() { DeleteAll(); }

    void operator += (const SinglyLinkedListWrap<T> & that)
    {
      for (Iterator iter(that); !iter.ReachEnd(); iter++) {
         T &c = iter.Current();
         AppendLast(c);
      }
    }

    void Reverse() 
     {  if (count == 0)
              return;
        SinglyLinkedEntryWrap<T>* tmp = head, *tmp1 = tmp->next;
        while (tmp != end) {
          SinglyLinkedEntryWrap<T>* tmp2 = tmp;
          tmp = tmp1; 
          tmp1 = tmp->next;
          tmp->next = tmp2;
        }
        head->next = 0;
        end = head;
        head = tmp;
     }

    SinglyLinkedEntryWrap<T>* AppendLast( const T& o )
     {
       ++count;
       SinglyLinkedEntryWrap<T> *e = new SinglyLinkedEntryWrap<T>( o );
       if (end == 0)
          head = end = e;
       else  {
          end->next = e;
          end = e;
       }
       return e;
     }
     SinglyLinkedEntryWrap<T>* PushFirst( const T& o )
     {
       ++count;
       SinglyLinkedEntryWrap<T> *e = new SinglyLinkedEntryWrap<T>( o );
       if (head == 0)
          head = end = e;
       else {
          e->next = head;
          head = e;
       }
       return e;
     }
     void PopFirst()
     {
        if (count > 0) {
          --count;
          SinglyLinkedEntryWrap<T>* e = head;
          if (head == end)
             head = end = 0;
          else
             head = head->next;
          delete e;
        }
     }
    void DeleteAll()
    {
       while (head != 0)
           PopFirst();
    }

    unsigned size() const { return count; }
    SinglyLinkedEntryWrap<T>* First() const { return head; }
    SinglyLinkedEntryWrap<T>* Last() const { return end; }
    SinglyLinkedEntryWrap<T>* Next(SinglyLinkedEntryWrap<T>* cur) const
          { return cur->next; }

   class Iterator
    {
      const SinglyLinkedListWrap<T> *list;
      SinglyLinkedEntryWrap<T> *cur;
    public:
      Iterator(const SinglyLinkedListWrap<T> &l) : list(&l) { cur = l.First(); }
      Iterator(const Iterator& that) : list(that.list), cur(that.cur) {}
      Iterator& operator = (const Iterator& that) { list = that.list; cur = that.cur; }
      Iterator() : list(0), cur(0) {}
      T& Current() const { return cur->GetEntry(); }
      T& operator *() const { return Current(); }
      bool ReachEnd() const { return cur == 0; }
      void Reset() { if (list != 0) cur = list->First(); }
      void Advance() { if (cur != 0) cur = list->Next(cur); }
      void operator++() { Advance(); }
      void operator ++(int) { Advance(); }
    };
   Iterator GetIterator() const { return Iterator(*this); }
};

template <class T>
class CollectSinglyLinkedList : public CollectObject<T>
{
  SinglyLinkedListWrap<T>& res;
 public:
  CollectSinglyLinkedList(SinglyLinkedListWrap<T>& r) : res(r) {}
  bool operator()(const T& cur)
   {
      res.AppendLast(cur);
      return true;
   }
};

#endif
