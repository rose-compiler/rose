void foobar()
   {
     typedef float (*t)();
     t *pt = new (t);
     delete pt;
     float (**ppf)()= new (float (*)());
     delete ppf;
   }
