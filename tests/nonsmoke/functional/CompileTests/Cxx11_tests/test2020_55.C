
namespace N
   {
     enum E { C };
   }

void foobar()
   {
  // This is a bug specific to the pseudo destructor.
     N::E e = N::C;
     e.N::E::~E();

     N::E &re = e;
     re.::N::E::~E();
   }



