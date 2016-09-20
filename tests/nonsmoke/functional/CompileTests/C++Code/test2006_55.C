// This is a continuation of test that started with test2005_190.C

int x = 1 + 2;

long double y = 1.0 + 2;

char a = (char)(1 + 2);

enum {X, Y} val = X;

// Note that we don't get the hex representation correct, but then hex is not a formal type, just a representation!
int hex = 0x0001;

enum number
   {
     none = 10,
     one  = 42
   };

void foo (number n)
   {
     foo(none);

     enum {X, Y} val = X;
   }

void foobar (bool b)
   {
     foobar(!! (!true || false) );
   }



