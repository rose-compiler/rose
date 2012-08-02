int foo(int a, int b) {

    return a+b;
}

int bar(int c, int d, int x) {
    c = c+1;
    d = d+1;
    x = x+1;;
    if (x > 2) {
    int q = x-c;
    if (q > d) {
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
    int ww = x;
    //x += c + d;
    return ww;
}

int barfoo(int y, int z) {
    if (y > 2) {
       y = foo(y, z);
       if (y > 3 && z != 1) {
           z = foo(z, z);
           if (z > y) {
               int x = bar(foo(y, y), y, z);
               if (x > 1) {
                   x = x+1;
               //    return x;
               }
               else {
                   y = y+1;
               //    return y;
               }
           }
       }
       else if (y == z) {
          z = z+1;;
       }
       else if (y > z) {
          y=y+1;
       }
       else {
          z = z+y;;
       }
    }
    return z;
}
           
int emptyfoo() {
    return 0;
}

int almostEmptyFoo(void) {
    int x = 0;
    x = x+1;;
    return x;
}

int main() {
    int x = 1;
    int x1 = 1;
    int y = 2;
    int z = x + y;
    int k = 1;
    k = k + z;
    //foo(x, y);
    //foo(foo(x, y), y);
    //int t = foo(barfoo(x, y), foo(x,y));
    //int t = barfoo(foo(x,y),y);
  //  int t = barfoo(barfoo(x,y),y);
   // int q = barfoo(barfoo(x,y),foo(x,k));
    //int q = foo(barfoo(bar(x, y, k),foo(x,x)), foo(x, k));
    //bar(x,y,k);
   int w = foo(x,y);
   int q = bar(w,y,k);
  
   //barfoo(foo(bar(x,y,k),foo(x,y)), k);
    return 0;
}
