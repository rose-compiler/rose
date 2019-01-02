// std::launder: Replacement of class objects containing reference members

#include<assert.h>

struct X { const int n; };
union U { X x; float f; };
void tong() {
   U u = {{ 1 }};
   u.f = 5.f;                          // OK, creates new subobject of 'u' (9.5)
   X *p = new (&u.x) X {2};            // OK, creates new subobject of 'u'
   assert(p->n == 2);                  // OK
   assert(*std::launder(&u.x.n) == 2); // OK
   assert(u.x.n == 2);                 // undefined behavior, 'u.x' does not name new subobject
}


