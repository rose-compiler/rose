namespace N_
   {
     enum E { A, B, C };
   }

void foobar()
   {
     N_::E e = N_::C;
     N_::C;
     e.N_::E::~E();
     N_::E &re = e;
     re.::N_::E::~E();
   }


