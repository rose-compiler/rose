/*
   The problem is with the second line of each example (the first line is unparsed correctly).

// Original input to ROSE
          double** Lhs_Data_Pointer = &(Array_Descriptor.Array_Data);
          double** Rhs_Data_Pointer = &(((doubleArray &)Rhs).Array_Descriptor.Array_Data);

// Output from unparser
          double * * Lhs_Data_Pointer=&(this -> Array_Descriptor).Array_Data; 
          double * * Rhs_Data_Pointer=&((doubleArray & )Rhs.Array_Descriptor).Array_Data; 

 */

// Use and modify the following code to reproduce the above bug
#if 1
// #include<stdlib.h>
#include<stdio.h>
// #include<assert.h>
#endif

// const void* NULL = 0;

class A
   {
     public:
          int intValue;
          A(){ intValue = 42; };
   };

class B
   {
     public:
          A myBaseClass;
          B(){};
   };

/* Current error in unparsed code:
class B 
{
  private: 
  class A 
myBaseClass;
  public: inline B();
}
*/

int 
main()
   {
#if 1
  // Build object so that we can call the constructor
     B objectB;
     const B & objectBref = objectB;

     int x;

  // will be unparsed as:  
  // x = ((((class B &)(objectBref)).myBaseClass).intValue);
     x = ((B&)objectBref).myBaseClass.intValue;
#endif

  // printf ("Program Terminated Normally! \n");
     return 0;
   }

