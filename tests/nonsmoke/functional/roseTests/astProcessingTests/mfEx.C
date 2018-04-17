int foo(int q) {
    //int y = 0;
    if (q == 1) {
      //  return 1;
      q = q + 1;
    }
    else {
      q = q + 2;
        //return 0;
    }
/*
    if (0 == 1) {
       y = 1;
    }
    if (x == 0) {
       y = 0;
    }
    else {
       y = 1;
    }
*/
    return q;
}

int main() {
   int x = 1;
   int y = 0;
   int z = foo(x);
   int a = foo(z);
/*   if (0 == 1) {
       x = 0;
   }
   if (y == 0) {
       x = 1;
   }
   else {
       x = 2;
   } */
   return 1;
}
