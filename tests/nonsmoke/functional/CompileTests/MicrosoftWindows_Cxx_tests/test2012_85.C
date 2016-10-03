
class X
   {
     public:
          int foo() 
             {
            // Declare a local variable but return the class data member (just to test the name resolution!)
            // int isparallel = X::isparallel;
            // Use of isparallel before declaration causes static modifier to be omitted
            // return X::isparallel;
               return isparallel;
             }

       // static int isparallel;
          static int isparallel;
   };

// unparses to be: "int isparallel = -1;" if referenced in class ahead 
// of its declaration (case: DEMONSTRATE_BUG == 1)
// int X::isparallel = -1;

