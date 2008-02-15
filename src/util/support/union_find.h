// Define union-find style groups

#ifndef UNION_FIND_h
#define UNION_FIND_h

#include <stdlib.h>

class UF_elem 
{
   void operator = (const UF_elem& that) {}
 protected:
   UF_elem *p_group;
   unsigned size;
 public:
   UF_elem () : p_group(this), size(1) {}
   UF_elem (const UF_elem& that)  // no copying, can only modify using union_with 
    : p_group(this), size(1) {}
 
   bool in_same_group(UF_elem *that) 
     {
       return find_group() == that->find_group();
     }
   void union_with(UF_elem *that)
     {
       UF_elem *p1 = find_group(), *p2 = that->find_group();
       if (p1 == p2) return;
       
       if (p1->size < p2->size) {
	 p1->p_group = p2;
	 p2->size += p1->size;
       }
       else {
	 p2->p_group = p1;
	 p1->size += p2->size;
       }
     } 
   UF_elem * find_group()
   {
     if (this->p_group == this->p_group->p_group)
       return this->p_group;
     
     p_group = p_group->find_group();
     return p_group;
   }
   unsigned group_size() const { return size; }
};

#endif

