// This test is simplified from test2007_01.C

struct foo { int bar; };

int main()
   {
     int foo::*v = &foo::bar;
   }
