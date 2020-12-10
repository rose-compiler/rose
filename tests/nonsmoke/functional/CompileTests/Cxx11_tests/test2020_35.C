
struct A 
   {
     int d() & { return 5; }
     int d() const& { return 6; }
     int d()&& { return 7; } // _RVALUE_REF
     int e() & noexcept { return 8; }
     int e() const& noexcept { return 9; }
     int e()&& noexcept { return 10; }
     A() { }
     A s();
     const A s() const;
   };

A sa_ ;

A A::s() { return sa_; }

const A sb_ ;

const A A::s()const { return sb_; }

void foobar()
   {
     A sa ;
     const A sb;

     sa.d();
     sb.d();
     sa.s().d();
     sa.e();
     sb.e();
     sa.s().e();
   }

