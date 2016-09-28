// This demonstrate a bug in g++

class B
   {
     int integer;

     public:
#if 1
            B() : integer(42) {};
#else
            B();
#endif
   };

