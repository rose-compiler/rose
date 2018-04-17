// Demonstration of overzelious use of name qualification

class X
   {
     public:
          typedef int IntegerType;
       // If main is commented out then this is unparsed as "Y::IntegerType foo();"
          IntegerType foo();
          enum people {};         
          void foo (people);
   };

class Y
   {
     public:
          typedef int IntegerType;
          IntegerType foo();
          enum people {};         
          void foo (people);
   };

namespace X_namespace
   {
     typedef int IntegerType;
     IntegerType foo();   
     enum people {};         
     void foo (people);
   }

namespace Y_namespace
   {
     typedef int IntegerType;
     IntegerType foo();   
     enum people {};         
     void foo (people);
   }

enum people {};         
void foo (people);

void foobar1()
   {
   }

void foobar2()
   {
   }

#if 1
int main()
   {
  // X x;
  // Y y;

  // x.foo();
  // y.foo();
   }
#endif
