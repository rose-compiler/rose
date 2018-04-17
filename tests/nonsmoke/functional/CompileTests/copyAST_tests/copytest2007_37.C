
// The friend function needs to cause a function symbol to 
// be generated when building the symbol table for global scope!

// At present this is not an erro that is caught in ROSE!
class A
   {
     public:
         friend void foo();
   };

