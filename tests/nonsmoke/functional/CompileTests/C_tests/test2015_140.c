

void foobar()
   {
      int **a;
      int i;
      int x = ({ typeof(a) y; for (i = 0; i < 10; i++) { y++; } 0; });
   }
