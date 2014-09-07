class iterator {};

void foo()
   {
     int i = 0;

  // Class types are output as: "for (class X::iterator iter, class X::iterator end; true; i++)"
     for (iterator iter, end; true;  i++);
   }
