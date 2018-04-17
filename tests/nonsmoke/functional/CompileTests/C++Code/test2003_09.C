#define DEMONSTRATE_BUG 1

class intArray;

class intArray_Descriptor_Type
   {
     public:
#if DEMONSTRATE_BUG
          friend void transpose( intArray & X );
#endif
//        void transpose( intArray & X );
   };

class intArray 
   {
     public:
         intArray_Descriptor_Type Array_Descriptor;
//       friend void transpose( intArray & X );
   };

void transpose (intArray & X )
// void intArray_Descriptor_Type::transpose (intArray & X )
   {
      intArray & Result = X; // *(new intArray());
   }


#if 0
// This case works fine.  The code above demonstrates 
// the bug (suynthesized from 50K of A++ source code!!!)
typedef unsigned int size_t;

class A
   { 
     public: 
          A(int) {};
          void *operator new (size_t Size) {};
   };

// A & x = *(new A(i));

void foo ()
   {
     A & Result = *(new A (1));

  // int j;

   }
#endif
