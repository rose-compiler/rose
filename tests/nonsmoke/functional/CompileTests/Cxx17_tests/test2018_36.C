// Nested namspace definitions.

namespace A::B::C 
   {
     int i;
   }

// The above has the same effect as:

namespace A {
     namespace B {
          namespace C {
               int j;
            }
        }
    }

