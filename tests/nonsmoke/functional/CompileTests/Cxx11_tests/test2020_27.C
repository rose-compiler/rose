
const char *foo(int i) { return i ? "yes" : "no"; }

void foobar()
   {
     typedef const char *(*t)(int);
     const char *(**p1)(int) = new (const char *(*)(int)) (foo);
     (*p1)(1)[1];
     t *p2 = new t (*p1);
     (*p2)(0)[1];
   }

