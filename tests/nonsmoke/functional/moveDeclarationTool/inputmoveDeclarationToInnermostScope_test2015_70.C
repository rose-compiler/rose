#include <list>

void foobar()
   {
     std::list<int*> *my_list;
     int* a;
     
     std::list<int*>::iterator it;

  // The problem is that the increment and test are marked as compiler generated.
     for (it = my_list->begin(); it != my_list->end(); it++) {
         a = *it;
     }

   }
