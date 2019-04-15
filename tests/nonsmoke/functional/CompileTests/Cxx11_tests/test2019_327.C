
#if 0
namespace M
   {
     class ABC
        {
          public:
               ABC(int ii);
        };
   }
#endif

// namespace N 
//  {
     struct DEF
        {
          int i;
       // M::ABC o;
          DEF();
        };
// }

typedef int (DEF::*pointer_to_object);

// void foobar( DEF::*pointer_to_object )
void foobar( pointer_to_object )
   {
     DEF x();
  // int DEF::*pointer_to_integer = &DEF::i;
  // M::ABC N::DEF::*pointer_to_object = &N::DEF::o;
  // pointer_to_object = &N::DEF::o;
   }

