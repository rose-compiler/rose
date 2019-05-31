// This is a catagory of possible errors (fixed for enums)
// that may existin in the name qualification support in ROSE.

namespace A
   {
  // Put any construct here.
     class Type
        {
          int ERROR_MODE;
        };
   }

namespace B
   {
  // Put the same (or maybe different) any construct here, but using the same name.
     class Type {};
   }

using namespace A;
using namespace B;

// Reference the name qualified construct in the return type (but should not be sensative to where it is referenced).
A::Type abc;

