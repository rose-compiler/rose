#define VAR 42

void foo1()
   {
     int x;
     x = 1;
   }

void foo2()
   {
     int x;
     if (1)
        {}
     int a;
     int b;
     int c;
     int d = VAR;
     int e = VAR;
     x = VAR;
     a = VAR;
     b = VAR;
     c = VAR;
     d = VAR;
     e = x;
   }

void foo3()
   {
     int x;
     int a;
     int b;
     int c;
     x = 1;
     a = x;
     b = x;
     c = x;
   }

