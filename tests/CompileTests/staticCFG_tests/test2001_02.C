// Unparser Bug

// 2001/1/6 DQ Error in unparsing:
//   Original code:
//      Internal_Index Index_List [MAX_ARRAY_DIMENSION];
//   Unparsed code:
//      class Internal_Index Index_List[]; 

#if 0
#include<stdlib.h>
#include<stdio.h>
#include<assert.h>

class A
   {
     public:
          A() 
             {
             }
   };
#endif

int main()
   {
  // Build object so that we can call the constructor
     int arrayOfIntegers [42];

  // A objectA[2];

  // printf ("Program Terminated Normally! \n");
     return 0;
   }

