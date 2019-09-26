struct S_ 
   {
     int d() & { return 5; }
     int d() const& { return 6; }
     int d()&& { return 7; } // _RVALUE_REF
     int e() & noexcept { return 8; }
     int e() const& noexcept { return 9; }
     int e()&& noexcept { return 10; }
     S_() { }
     S_ s();
     const S_ s() const;
   };

S_ sa_ ;
S_ S_::s() { return sa_; }

const S_ sb_ ;
const S_ S_::s()const { return sb_; }


void foobar()
   {
     S_ sa ;
     const S_ sb;
     sa.d();
     sb.d();
     sa.s().d();
     sa.e();
     sb.e();
     sa.s().e();
   }

