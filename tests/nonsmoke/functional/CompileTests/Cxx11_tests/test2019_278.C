struct A_ 
   {
     struct B
        {
          int i;
          B() : i(1) { }
        };
     struct const_B
        {
          int j;
          const_B() : j(2) { }
        };

     B c();
     const_B c() const;
   };

struct C_
   {
     A_ a;
     auto d() const -> decltype(a.c()) { return a.c(); }
   };

A_::B ba_ ;
A_::const_B bc_ ;
A_::B A_::c() { return ba_; }
A_::const_B A_::c() const { return bc_; }
C_ ca_;

void foobar()
   {
     ba_.i;
     bc_.j;
     ca_.d().j;
   }
	
