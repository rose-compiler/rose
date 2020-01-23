
void foobar()
   {
  // Unparsed as:
  // float (**p1)() = new float (*)();
     float (**p1)() = new (float(*)());
   }

