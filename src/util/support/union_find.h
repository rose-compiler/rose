// Define union-find style groups
// two sets: x1->x1(point-to-self)  and y1->y2->y3(point-to-self) 
// after union 
//   x1->x1->y3  
//    y1->y2->y3(point-to-self) 
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
   // Create a set containing a single element. Its group id is a pointer to itself

// DQ (11/29/2009): MSVC reports a warning when "this" is used in the preinitialization list.
// UF_elem () : p_group(this), size(1) {}
   UF_elem () : size(1) { p_group = this; }

   UF_elem (const UF_elem& that)  // no copying, can only modify using union_with 
 // : p_group(this), size(1) {}
    : size(1) { p_group = this; }

   // Check if two elements belong to the same group
   bool in_same_group(UF_elem *that) 
     {
       return find_group() == that->find_group();
     }
   // Union with another element's group  
   void union_with(UF_elem *that)
     {
       // Skip if two elements are already within the same group
       UF_elem *p1 = find_group(), *p2 = that->find_group();
       if (p1 == p2) return;
       // Set the group id of smaller group to the larger group's group id      
       if (p1->size < p2->size) {
         p1->p_group = p2;
         p2->size += p1->size;
       }
       else {
         p2->p_group = p1;
         p1->size += p2->size;
       }
     } 
   // Return the top group link: self-pointing  
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

