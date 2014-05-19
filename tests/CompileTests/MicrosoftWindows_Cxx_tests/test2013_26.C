class Internal_Partitioning_Type
   {
     public:
#if 1
       // This causes a failure in the reference to the function in foo().
          static int getReferenceCountBase() { return 64; }
#else
       // This allows the correct code to be generated in the reference to the function in foo().
          int getReferenceCountBase() { return 64; }
#endif
   };

class Partitioning_Type
   {
     public:
          Internal_Partitioning_Type *Internal_Partitioning_Object;

          void foo();
   };

void 
Partitioning_Type::foo()
   {
     Internal_Partitioning_Object->getReferenceCountBase();
   }


