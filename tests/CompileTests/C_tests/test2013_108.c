
void foo()
   {
     if (1) 
        {
          int status;

          status = 1;

       // The problem here is the implicitly defined function call foobar() is causing a closing "}" to be generated in the unparser.
          if (status)
               foobar(status);

          status = 1;
        }
   }
