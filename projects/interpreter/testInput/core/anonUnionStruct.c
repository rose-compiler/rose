struct s {
     int p;
     union {
          struct {
               char a;
               short b;
          };
          struct {
               int c;
               long long d;
          };
          char e;
     };
     struct {
          char f;
     };
};

int test(int x)
   {
     struct s ss;
     ss.a = 1;
     ss.b = 2;
     ss.c = 3;
     ss.d = 4;
     ss.e = 5;
     ss.f = 6;
     return ss.c;
   }
