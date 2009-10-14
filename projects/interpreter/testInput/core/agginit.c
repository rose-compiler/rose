struct A { int a, b, c; };

int test(int i) {
     int x[3] = { 1, 2, 3 };
     struct A y = { 1, 2, 3 };
     return x[0]+x[1]+x[2]+y.a+y.b+y.c;
}
