namespace N_
   {
     enum E { A, B, C };
   }

void foobar()
   {
     N_::E e = N_::C;
     e.N_::E::~E();
   }

