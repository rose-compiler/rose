
void foobar()
   {
     class ABC;

  // The class definition can be in the typedef (C++11 alias typedef).
     using DEF = class ABC { public: int x; };

     DEF abc;

     abc.x = 42;
   }
