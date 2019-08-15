struct A_ 
   {
     struct B
        {
          B();
        };

     struct const_B
        {
          const_B();
        };

     B c();
     const_B c() const;
   };

struct C_
   {
     A_ a;
     auto d() const -> decltype(a.c()) { return a.c(); }
   };

