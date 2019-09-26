class C 
   {
     C(const C&) = default;
     C(C&&) = default;
     C& operator=(const C&) & = default;
     C& operator=(C&&) & = default;
     virtual ~C() { }
   };
