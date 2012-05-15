int foo(int a, int b) {
    return a+b;
}

int bar(int c, int d, int x) {
    c++;
    d++;
    x++;
    if (x > 2) {
    if (x - c > d) {
       x++;
    }
    else {
       x--;
    }
    }
    else {
       x += 2;
    }
    if (x == 0) {
       x++;
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
          z = z + 1;
       }
       else if (y > z) {
          y = y + 1;
       }
       else {
          z += y;
       }
    }
    return z;
}
           
int emptyfoo() {
    return 0;
}

int almostEmptyFoo(void) {
    int x = 0;
    x++;
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
    k = barfoo(x, y);
    //foo(bar(x, y, k), foo(x, k));
    //barfoo(foo(bar(x,y,k),foo(x,y)), k);
}
