struct A
   {
     int i;
     A(int ii) : i(ii) { }
     A &f() { return *this; }
     int foo(int n) { return i + n; }
   };

struct B
   {
     int i;
     B(int ii) : i(ii) { }
     B &g() { return *this; }
     int foo(int n) { return i + 2 * n; }
   };

struct C : public A, public B
   {
     C(int ii) : A(ii-1), B(ii+1) { }
     void bar()
        {
          ((A *)this)->foo(2);
          A::i;
          ((B *)this)->foo(3);
          B::i;
          f().foo(4);
          g().foo(5);
        }
   };

void foobar()
   {
     C c(2);
     c.bar();
   }

