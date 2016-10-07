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
              //// int x = bar(foo(y, y), y, z);
               int x = bar(y,z,z);
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
int again(int z) {
    return z;
}

int poke(int y) {
   int b = again(y);
   return b;
}





int slow(int y) {
    int a = poke(y);
    return a;
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
    k = z;
    //int c = bar(x, y, z);
   // int d = foo(bar(x, y, z), y);
    //foo(bar(x, y, k), foo(x, k));
   int c = slow(y);// barfoo(y,z);
}
