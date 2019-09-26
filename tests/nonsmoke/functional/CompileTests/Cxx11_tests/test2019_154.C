class ABC { public: int x; };

void foobar()
   {
  // The class definition can be in the typedef (C++11 alias typedef).
     using DEF = class ABC;

     DEF abc;

     abc.x = 42;
   }
