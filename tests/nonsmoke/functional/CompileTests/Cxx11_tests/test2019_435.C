
namespace XYZ
   {
     namespace A
        {
          enum Type
             {
               ERROR_MODE = 0,
             };
        }

     namespace B
        {
          enum Type {};
        }

     namespace C
        {
          enum Type {};
        }

     namespace D
        {
          enum Type {};
        }

     namespace E
        {
          enum Type {};
        }
   }


using namespace XYZ;

class MyClass
   {
     public:
          A::Type foobar();
   };

using namespace A;
using namespace B;
using namespace C;
using namespace D;
using namespace E;

A::Type MyClass::foobar()
   {
     return ERROR_MODE;
   }
