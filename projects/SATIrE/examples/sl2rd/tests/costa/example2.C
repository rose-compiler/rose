  int compute_sum(int n) {
      int a=0, b=0,i=n, j, y;
      while (i>0) {
          a=a+1;
          i=i-1;
          j=i;
          while (j>0) {
              b=b+1;
              j=j-1;
          }
      }
      y=a+b;
      return y;
  }
