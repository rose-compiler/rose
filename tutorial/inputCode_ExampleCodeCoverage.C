void foo()
   {
  // Should detect that foo IS called
  if (true) {
      int x = 3;
  }
  else {
      int x = 4;
   }
}

void foobar()
   {
      int y =4;
      switch (y) {
         case 1:
            //hello world
            break;
         case 2:

         case 3:

         default: {
            //
         }
      }

  // Should detect that foobar is NOT called
   }

int main()
   {
      if (true) {
      }
     foo();
     return 0;
   }
