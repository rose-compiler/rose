int foo(int a, int b) {
    return a+b;
}

int bar(int c, int d, int x) {
    int cp = c+1;
    int dp = d+1;
    int xp = x+1;
    if (xp > 2) {
    if (xp > dp) {
       xp = xp+1;;
    }
    else {
       xp = xp-1;
    }
    }
    else {
       xp = 2;
    }
    if (xp == 0) {
       xp++;
    }
    int y = xp;
    //x += c + d;
    return y;
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
          z++;
       }
       else if (y > z) {
          y++;
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
    //int x1 = foo(x, y);
    int x2 = bar(x, y, z);
    //foo(bar(x, y, k), foo(x, k));
    //barfoo(foo(bar(x,y,k),foo(x,y)), k);
    return 0;
}
