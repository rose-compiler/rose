// This example fails in ROSE but works if the order of the variable 
// declaration is inverted to be before the member functions.
class Derived
   {
     public:
          Derived (void) : value ('a') {}
          void run (void) { value = 'z'; }

       // Declaration of after use in member functions is an error in ROSE
          char value;
   };
