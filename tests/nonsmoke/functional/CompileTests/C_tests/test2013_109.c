

void foo()
   {
     if (1) 
        {
          int status;

          status = 1;

       // The problem here is the implicitly defined function call foobar() is causing a closing "}" to be generated in the unparser.
          if (status)
               status = foobar_1(status) + foobar_2(status);

          status = 1;
        }
   }
