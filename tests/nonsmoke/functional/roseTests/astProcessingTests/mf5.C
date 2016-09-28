int foo(int a, int b) {
    return a+b;
}

int bar(int c, int d, int x) {
    c = c +1;
    d = d+1;
    x = x+1;
    if (x > 2) {
    if (c > d) {
       x = x+1;
    }
    else {
       x = x-1;
    }
    }
    else {
       x = x + 2;
    }
    if (x == 0) {
       x = x+1;
    }
    //x += c + d;
    return x;
}

int barfoo(int y, int z) {
    if (y > 2) {
       y = foo(y, z);
       if (y > 3 && z != 1) {
           z = foo(z, z);
           if (z > y) {
               int x = bar(foo(y, y), y, z);
               if (x > 1) {
                   return x;
               }
               else {
                   return y;
               }
           }
       }
       else if (y == z) {
          z = z+1;
       }
       else if (y > z) {
          y = y+1;
       }
       else {
          z = z+ y;
       }
    }
    return z;
}
           
int emptyfoo() {
    return 0;
}

int almostEmptyFoo(void) {
    int x = 0;
    x = x+1;
    return x;
}

int main() {
    int x = 1;
    int y = 2;
    int z = x + y;
    int k = 1;
    k = k + z;
    //foo(x, y);
    //foo(foo(x, y), y);
    //foo(bar(x, y, k), foo(x, k));
    barfoo(foo(bar(x,y,k),foo(x,y)), k);
}
