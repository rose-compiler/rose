class X
   {
     public:
          class iterator
             {
               public:
             };
   };


void foo()
   {
     int i = 0;

  // Class types are output as: "for (class X::iterator iter, class X::iterator end; true; i++)"
     for (X::iterator iter, end; true;  i++);

  // Primative types work fine.
     for (int j, k; true;  j++);
   }


