#include <initializer_list>

struct A 
   {
     int i;
     A(std::initializer_list<const char*> a);
   };

int foo(A a);

void foobar()
   {
     int a;

  // Unparsed as:   a = foo(({"foo", "bar", "3", "4"}));
  // Original code: a = foo( { "foo", "bar", "3", "4" } );
     a = foo( { "foo", "bar", "3", "4" } );
   }

