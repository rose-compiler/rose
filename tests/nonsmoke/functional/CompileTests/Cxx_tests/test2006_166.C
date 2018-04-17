
class A
   {
     public:
          void foobar()
             {
               this->foo();
             }

         void foo();
   };

// Error: the function symbol here is put into the global symbol table.
// I think this may be fixed now, nee to confirm this!
void A::foo() {}
