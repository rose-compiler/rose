// This is a simplification of the test2005_103.C code.
class X
   {
     public:
          int A() const;

          int getValue() const
             { 
               return valueArray[int(A())];
             }

          static const int arraySize = 16;
          static const int valueArray[arraySize+10];
   };

