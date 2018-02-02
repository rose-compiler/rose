class X
   {
     public:
          int i;
          X(int j) : i(j) { }
   };

int operator * (X  a)

void foobar()
   {
     X a (1);
     const X * pc = &a;

     int i3 = **pc;
   }
