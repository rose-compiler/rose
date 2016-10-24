
namespace G
   {
     class X 
        {
          public:
            // forward definition of "Z"
               class Z;
        };
   }

namespace G
   {
  // Definition of Z specified with name qualification...
     class X::Z
        {
          public:
        };
   }


G::X::Z abc;

