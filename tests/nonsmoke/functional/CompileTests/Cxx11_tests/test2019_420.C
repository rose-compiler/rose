

typedef int integer1;

namespace X
   {
     struct A;
   }

typedef X::A integer2;

struct Y
   {
     struct A;
   };

typedef Y::A integer3;

typedef void (*functionPointer)(double);

