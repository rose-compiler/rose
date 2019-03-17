
void foobar()
   {
     struct A
        {
          A() : b(3) { }
          int b;
        };

  // Original code: A c = { };
  // Unparsed as: struct A c();
     A c = { };

     c.b;
   }


