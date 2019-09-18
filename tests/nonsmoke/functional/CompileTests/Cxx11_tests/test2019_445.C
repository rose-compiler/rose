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
   }


using namespace XYZ;

#if 0
class MyClass
   {
     public:
          A::Type foobar();
   };
#endif

A::Type foobar();

using namespace A;
using namespace B;

#if 0
A::Type MyClass::foobar()
   {
     return ERROR_MODE;
   }
#endif

A::Type foobar()
   {
     return ERROR_MODE;
   }
