namespace M
   {
     class ABC
        {
          public:
               ABC(int ii);
        };
   }

namespace N 
   {
     struct DEF
        {
          int i;
          M::ABC o;
          DEF();
        };
   }

void foobar()
   {
     N::DEF x();
  // int DEF::*pointer_to_integer = &DEF::i;
     M::ABC N::DEF::*pointer_to_object = &N::DEF::o;
   }

