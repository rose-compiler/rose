#if 1
class A
   {
     public:
          int x;
          int y;
          virtual void foo()
             {
               x = 0xcafe0001;
               y = 0xcafe0002;
             }

          A()
             {
               x = 0xcafe0003;
               y = 0xcafe0004;
             }

          virtual ~A()
             {
               x = 0xcafe0005;
               y = 0xcafe0006;
             }
   };

class B : public A
   {
     public:
          virtual void foo()
             {
               x = 0xcafe0011;
               y = 0xcafe0012;
             }

          B()
             {
               A a;
               a.x = 0xcafe0017;
               x = 0xcafe0013;
               y = 0xcafe0014;
             }

          virtual ~B()
             {
               x = 0xcafe0015;
               y = 0xcafe0016;
             }
   };

class C : public B
   {
     public:
          int x;
          void foo() { x = 0xcafe0021; }
   };

A a;
B b;
C c;

class D
   {
     public:
          int x;
          void foo()
             {
               c.x = 0xcafe0031;
               x   = 0xcafe0032;
             }
   };

#endif

// This function will appear as a mangled name in the binary.
void
foobar()
   {
     a.x = 0xcafe0107;
     A aa;
     aa.x = 0xcafe0108;
   }

int abc;

int
main()
   {
      abc = 0xcafe0210;
#if 1
      char  x = 0xff;
      short y = 0xcafe;
      A b;
      b.x = 0xcafe0110;
      b.y = 0xcafe0111;
      b.foo();

      D d;
      d.foo();
      d.x = 0xcafe0112;
#endif

      return 0;
   }
