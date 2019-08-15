void foobar()
   {
     int x;
     switch(x)
        {
       // BUG: label before case will eat the case!
          lable1:
          case 0:
               x = 76;
        }
   }
