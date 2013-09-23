

namespace A
   {
     int a;
     namespace B 
        {
#if 1
          namespace C
             {
           // BUG: If we put namespace A, then when C is added to the global scope, the "using namespace A::B::C;" will be unparsed as: "using namespace B::C;"
           // using namespace A;
             }
#endif
        }

      using namespace B::C;
   }

using namespace A::B::C;

#if 0
void foo()
   {
     A::a = 0;
     A::B::C::a = 0;
     A::B::C::B::C::a = 0;
   }
#endif

