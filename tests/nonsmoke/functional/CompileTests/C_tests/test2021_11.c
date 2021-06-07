
int foobar()
   {
  // ({ union { int in; } u; u.in = 42; u.in; });
  // union { int in; } u; u.in = 42; u.in;
  // ({ struct { int in; } u; 42; });
  // ({ struct { int in; } u; });
  // ({ 42; });
     ({ struct { int in; } u; });
   }

