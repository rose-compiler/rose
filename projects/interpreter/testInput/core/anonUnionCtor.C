struct x {
     x(char c) : c(c) {}
     x(int i) : i(i) {}

     union {
          char c;
          int i;
     };
};

int test(int)
   {
     struct x xs(int(42));
     return xs.i;
   }
