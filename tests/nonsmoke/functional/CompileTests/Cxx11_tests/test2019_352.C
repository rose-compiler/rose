class C 
   {
     C(const C&) = delete;
     C(C&&) = delete;
     C& operator=(const C&) & = delete;
     C& operator=(C&&) & = delete;
     virtual ~C() { }
   };
