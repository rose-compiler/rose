
const char *foo(int i);

void foobar()
   {
  // Unparsed as:
  // const char *(**p1)(int ) = new const char *(*)(int )(foo);
     const char *(**p1)(int) = new (const char *(*)(int)) (foo);
   }

