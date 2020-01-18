
namespace N
   {
     static union { int a; char *p; };
   }

void foobar()
   {
     &N::a;
     &N::p;
   }

