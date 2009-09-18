struct A {
     int a;
     int b;
};

struct B { 
     int c;
     int d;
};

struct C : A, B {
     int e, f;
     void incF() { f++; }
};

int answer(int x)
   {
     return x+1;
   }

int test(int p)
   {
     C c;
     c.a = 1;
     c.b = 2;
     c.c = 3;
     c.d = 4;
     c.e = 5;
     c.f = p;
     A *ap = &c;
     ap->a = 7;
     ap->b = 8;
     B *bp = &c;
     bp->c = 9;
     bp->d = 10;
     int *ip = &(ap->b);
     *ip = 11;
     *(ip+1) = 12;
     ip++;
     *(ip+1) = 13;
     c.f = answer(41);
     c.incF();
     return c.a+c.b+c.c+c.d+c.e+c.f;
   }
    

