#include "stdio.h"
#include "stdlib.h"
#include "assert.h"

void foo ( int a );

// Used to identify the ends of the list 
// (dummy list elements representing the front and back of the list)
#define TERMINAL_VALUE 1000

class list
   {
  // List class defined similarly to STL List class (but without templates)

     public:
       // fixed element type for list class (to avoid templates)
          typedef int elementType;

     protected:
          struct list_node {
               list_node* next;
               list_node* prev;
               elementType data;
             };

          typedef elementType* pointer;
          typedef elementType& reference;

          typedef list_node* link_type;
          typedef size_t size_type;

     protected:
       // start of list
          link_type first;

       // end of list
          link_type last;

          size_type length;

     public:
          class iterator
             {
               friend class list;
               protected:
                    link_type node;
                    iterator(link_type x) : node(x) {}

               public:
                    iterator() {}
                    bool operator==(const iterator& x) const { return node == x.node; }
                    bool operator!=(const iterator& x) const { return node != x.node; }
                    reference operator*() const { return (*node).data; }
                    iterator& operator++() { 
                         node = (link_type)((*node).next);
                         return *this;
                       }
                    iterator operator++(int) {
                         iterator tmp = *this;
                         ++*this;
                         return tmp;
                       }
             };

         list()
            {
           // build the starting element (something for the last->prev pointer to point at)
              first = new list_node;
              assert (first != NULL);

              first->prev = NULL;
              first->data = -TERMINAL_VALUE;

           // build the terminal element (something for the first->next pointer to point at)
              last  = new list_node;
              assert (last != NULL);

              first->next = last;

              last->next = NULL;
              last->prev = first;
              last->data = TERMINAL_VALUE;

           // force begin() == end() for an empty list
              last = first;
              assert (begin() == end());

              length = 0;
            }

          iterator begin() { return (link_type)((*first).next); }
          iterator end()   { return (link_type)((*last).next); }

          unsigned int size()   { return length; }

          void push_back(const reference x)
             {
                assert (last != NULL);
                list_node* terminalElement = NULL;
                assert (last->next != NULL);
                terminalElement = last->next;
                assert (terminalElement != NULL);
                assert (terminalElement->data == TERMINAL_VALUE);

                last = new list_node;
                assert (last != NULL);

             // initialize the member data
                last->next = NULL;
                last->prev = NULL;
                last->data = x;

                last->next = terminalElement;

                assert (terminalElement->prev != NULL);
                last->prev = terminalElement->prev;
                terminalElement->prev = last;

                assert (last->prev != NULL);
                last->prev->next = last;

                length++;
             }

	  elementType& operator[](int index) {
	    // O(n) access
            int j=0;
	    for(list::iterator i=begin(); i!=end(); i++) {
	      if(j==index) return *i;
	      j++;
	    }
	    // should throw an exception
	    assert(false);
	    return *begin(); // dummy, we need to return something
	  }
   };

class SupportingOpenMPArray
   {
  // This class is used to support the transformation of iterations over STL 
  // containers to a form with which we can use OpenMP to parallize the execution.

     public:
          typedef list::elementType elementType;
          list::elementType** dataPointer;
          unsigned int length;

     public:
          SupportingOpenMPArray(list & l)
             {
               length = l.size();
               dataPointer = new list::elementType* [length];
               assert (dataPointer != NULL);

               list::iterator p;
               int i = 0;
               for (p = l.begin(); p != l.end(); p++)
                  {
                    dataPointer[i++] = &(*p);
                  }
             }

          unsigned int size() { return length; }
          elementType** getDataPointer() { return dataPointer; }
   };


