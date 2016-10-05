
void foobar()
   {
  // There is an implicit cast to "void*" and and explicit cast to "struct { char c; unsigned int member; }*"
     void* x = (struct { char c; unsigned int member; } *) 0;
   }
