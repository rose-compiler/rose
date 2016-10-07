#include <vector>

void foo()
   {
#if 1
     for (int i = 5, j = 7; i != j; i++)
        {
        }
#endif

     std::vector<int> v;
     for (std::vector<int>::iterator nnnn = v.begin(), mmmm = v.end(); nnnn != mmmm; nnnn++)
        {
        }
   }
