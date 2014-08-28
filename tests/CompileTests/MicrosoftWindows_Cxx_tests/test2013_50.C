// Need to generate a test code to demonstrate where typedef types can't be 
// resolved and cause instantiated functions to be generated incorrectly.

class map
   {
     public:
         ~map();
   };

void
foobar()
   {
      map m;
      m.~map();
   }
