#include <initializer_list>

void func1()
   {
     for (int n : {0, 1, 2, 3, 4, 5}) // the initializer may be a braced-init-list
        {
          42;
        }
   }

