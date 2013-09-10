
class X
   {
     public:
          enum numbers 
             {
               LARGE_NUMBER = 2
             };
            
       // This will unparse as: static const int X_array_1[X::LARGE_NUMBER];
       // If we want it handled better then we will have to force name qualification instead of 
       // pre-computing the qualified name and associating it with the enumValue that is stored 
       // in the array type and thus shared (and referenced inside the class and outside of the class).
          static const int X_array_1[LARGE_NUMBER];
   };

const int X::X_array_1[X::LARGE_NUMBER] = { 1, 2 };

