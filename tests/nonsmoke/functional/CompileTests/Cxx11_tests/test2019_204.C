
class ABC
   {
     public:
          ABC(int ii);
   };


struct DEF
   {
     int i;
     ABC o;
     DEF();
   };

void foobar()
   {
     DEF x();
  // int DEF::*pointer_to_integer = &DEF::i;
     ABC DEF::*pointer_to_object = &DEF::o;
   }

