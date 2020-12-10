// const char *foo(int i);
// char *foo(int i);
float foo(int i);

void foobar()
   {
  // Unparsed as:
  // const char *(**p1)(int ) = new const char *(*)(int )(foo);
  // const char *(**p1)(int) = new (const char *(*)(int)) (foo);
  // char *(**p1)(int) = new (char *(*)(int)) (foo);
     float (**p1)(int) = new (float(*)(int)) (foo);
   }

