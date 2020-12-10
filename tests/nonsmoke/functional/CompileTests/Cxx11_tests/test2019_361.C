class C 
   {
  // DQ (4/13/2019): EDG 4.12 does not mark these internally as being specified using "= delete" syntax.

  // These are marked using the "= delete" syntax.
     C& operator=(const C&) & = delete;
     C& operator=(C&&) & = delete;
     virtual ~C() { }
   };
