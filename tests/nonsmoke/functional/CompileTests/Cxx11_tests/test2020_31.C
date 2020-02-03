
void foobar()
   {
  // Unparsed as (missing parenthesis):
  // float (**p1)() = new float (*)();
  // float (**p1)() = new (float(*)());
     float *p1 = new (float);
   }

