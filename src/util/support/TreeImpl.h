
#ifndef TREE_IMPL_H
#define TREE_IMPL_H

#include <iostream>
#include <assert.h>
#include <DoublyLinkedList.h>

template<class T>
class TreeNodeImpl
{
  T* parent;
  DoublyLinkedListWrap<T*> children;
  typedef DoublyLinkedEntryWrap<T*> HolderType;
  HolderType *holder;

 protected:
  void Unlink() 
    { if (holder != 0) {
          parent->children.Delete(holder); holder = 0; parent = 0; 
      }
    }

 public:
  TreeNodeImpl() : parent(0), holder(0) {}
  virtual ~TreeNodeImpl() 
    { for (iterator p = ChildrenIterator(); !p.ReachEnd(); ++p) {
         TreeNodeImpl<T>* n = (*p);
         n->holder = 0;
         delete n;
      }
      Unlink(); 
    }
  typedef typename DoublyLinkedListWrap<T*>::iterator iterator;
  typedef typename DoublyLinkedListWrap<T*>::const_iterator const_iterator;

  typedef enum {AsFirstChild,AsLastChild, AsPrevSibling, AsNextSibling} LinkOption;

  T* Parent() const { return parent; }
  T* FirstChild() const { return (ChildCount() > 0)? children.First()->GetEntry() : 0; }
  T* LastChild() const { return (ChildCount() > 0)? children.Last()->GetEntry() : 0; }
  iterator ChildrenIterator() { return iterator(children); }
  const_iterator ChildrenIterator() const { return const_iterator(children); }
  T* NextSibling() const {  HolderType *h = (holder==0)? 0 : parent->children.Next(holder);
                            return (h != 0)? h->GetEntry() : 0; }
  T* PrevSibling() const { HolderType *h = (holder==0)? 0 : parent->children.Prev(holder);
                           return (h != 0)? h->GetEntry() : 0; }
  unsigned ChildCount() const { return children.NumberOfEntries(); }

  void Link(T* pos, LinkOption opt)
     { assert(holder == 0);
       T* entry = static_cast<T*>(this);
       switch (opt) {
       case AsFirstChild: 
           parent = pos; holder = pos->children.PushFirst(entry); break;
       case AsLastChild:
           parent = pos; holder = pos->children.AppendLast(entry); break;
       case AsPrevSibling:
           parent = pos->parent; 
           holder = parent->children.InsertBefore(entry, pos->holder);
           break;
       case AsNextSibling:
           parent = pos->parent;
           holder = parent->children.InsertAfter(entry, pos->holder);
           break;
       default:
           assert(false);
       }
     }
  virtual void write(std::ostream& out) const {
      for (const_iterator p = ChildrenIterator(); !p.ReachEnd(); ++p) 
         (*p)->write(out);
      if (ChildCount() > 0) 
        out << "endtree\n";
  }
  void write() const 
    { write(std::cerr); }
};

template <class T>
class TreeTraverse 
{
 public:
  typedef enum {PreOrder, PostOrder, ChildrenOnly} TraversalOpt;
  static T* FirstNode(T *n, TraversalOpt opt=PreOrder)
    { switch (opt) {
       case PreOrder: return n;
       case PostOrder:
          while (n->FirstChild() != 0)
             n = n->FirstChild();
          return n;
       case ChildrenOnly:
          return n->FirstChild();
       default:
                   { assert(false); /* avoid MSVC warning about path without return stmt. */ return NULL; }
      }
    }
  static T* LastNode( T *n, TraversalOpt opt=PreOrder)
   { switch (opt) {
      case PostOrder: return n;
      case PreOrder:
         while (n->LastChild() != 0)
             n = n->LastChild();
         return n;
      case ChildrenOnly: return n->LastChild();
      default:
                  { assert(false); /* avoid MSVC warning about path without return stmt. */ return NULL; }
      }
   }
  static T* PrevNode( T *n, TraversalOpt opt=PreOrder)
   { T *result = 0;
     switch (opt ) {
      case PreOrder:
          result = n->PrevSibling();
          if ( result != 0)
              return LastNode( result, PreOrder );
          else
              return n->Parent();
      case PostOrder:
          result = n->LastChild();
          if ( result == 0) {
             for ( result = n->PrevSibling(); result == 0;
                   result = n->PrevSibling()) {
                n = n->Parent();
               if (n == 0)
                  break;
             }
          }
          return result;
      case ChildrenOnly:
          return n->PrevSibling();
      default:
                  assert(false);
     }
  /* avoid MSVC warning about path without return stmt. */ 
         return NULL;
   }
  static T* NextNode( T *n, TraversalOpt opt=PreOrder)
   { T *result = 0;
     switch (opt ) {
      case PostOrder:
         result = n->NextSibling();
         if ( result != 0)
            return FirstNode( result, PostOrder );
         else
            return n->Parent();
      case PreOrder:
         result = n->FirstChild();
         if ( result == 0) {
            for ( result = n->NextSibling(); result == 0;
                  result = n->NextSibling()) {
                n = n->Parent();
                if (n == 0)
                  break;
            }
         }
         return result;
     case ChildrenOnly:
          return n->NextSibling();
     default:
          assert(false);
     }
  /* avoid MSVC warning about path without return stmt. */ 
         return NULL;
   }
};

#endif

