union u1 {
     char a;
     short b;
};

union u2 {
     int c;
     long long d;
};

struct s1 {
     void *p;
     union u1 u;
};

struct s2 {
     union u2 u;
     char q;
};

union u {
     struct s1 one;
     struct s2 two;
};

int test(int x)
   {
     union u uu;
     uu.one.p = 0;
     uu.one.u.a = 1;
     uu.one.u.b = 2;
     uu.two.q = 3;
     uu.two.u.d = 5;
     uu.two.u.c = 4;
     char *ua = &uu.one.u.a;
     return uu.two.u.c;
   }
