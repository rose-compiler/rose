class C 
   {
  // DQ (4/13/2019): EDG 4.12 does not mark these internally as being specified using "= default" syntax.

  // These are marked a trivially default AND using the "= default" syntax.
     C& operator=(const C&) & = default;
     C& operator=(C&&) & = default;
     virtual ~C() { }
   };
