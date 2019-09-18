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

using namespace A;
using namespace B;


A::Type abc;

