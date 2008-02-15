#include "stdio.h"
#include "stdlib.h"
#include "assert.h"
#include "UserDefList.h"

void foo ( int a ) {
  printf ("a = %d \n",a);
}

int main() {
  int array[10] = { 0,1,2,3,4,5,6,7,8,9 };

  //   printf ("Building the list object \n");
  list l;
  
  //   printf ("After construction: *(l.begin()) = %d \n",*(l.begin()));
  //   printf ("After construction: *(l.end())   = %d \n",*(l.end()));
  
  //for (int i = 0; i < 10; i++)
  //  l.push_back(array[i]);
  
  // ***********************************************
  // Demonstrate original code before transformation
  // ***********************************************
  
  printf ("Demonstration of original code using STL iterators \n");
  
  for (list::iterator p = l.begin(); p != l.end(); p++) {
    foo(*p);
  }

#if 0
  // *******************************
  // Now generate the transformation
  // *******************************
  
  // Demonstrate the parallel OpenMP transformation of the STL statement
  printf ("Demonstration of parallel STL transformation using OpenMP \n");
  
  // #pragma omp parallel for
  for (int i = 0; i < l.size(); i++) {
    foo( l[i] );
  }
  
#if 0
  
  // **************************************
  // Demonstrate alternative transformation
  // **************************************
  
  printf ("Demonstration of alternative parallel STL transformation using OpenMP \n");
  
  list::iterator q;
#pragma omp private(q)
  for (q = l.begin(); q != l.end(); q++)
    {
#pragma omp single nowait
      foo(*q);
    }
  
#endif

#endif
  
  return 0;
}









