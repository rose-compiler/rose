namespace N_ 
   {
     struct X 
        {
          int i;
          X() : i(0) { }
        };
   }

int N_::X::*pdn = &N_::X::i;

