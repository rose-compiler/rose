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

typedef int (N::DEF::*pointer_to_object);

// void foobar( N::DEF::*pointer_to_object )
void foobar( pointer_to_object )
   {
     N::DEF x();
  // int DEF::*pointer_to_integer = &DEF::i;
  // M::ABC N::DEF::*pointer_to_object = &N::DEF::o;
  // pointer_to_object = &N::DEF::o;
   }


void foobar2( int (N::DEF::*pointer_to_object) )
   {
   }
