int f_(int i) { return i + 1; }

void foobar()
   {
     int (**pf)(int) = new (int (*)(int)) (f_);
   }


