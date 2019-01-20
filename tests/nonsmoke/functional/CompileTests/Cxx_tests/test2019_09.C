int foo(int i);
typedef int (*FunctionPointer)(int);

struct myClass
   {
     int n;
     myClass() : n(42) {}
     myClass(int i) : n(i) {}

     FunctionPointer operator~() { return foo; }
   };

void foobar()
   {
     myClass x;
     x = (~x)(42);

     ~x;
   }
