
void foo()
   {
     if (1) 
        {
          int status;

          status = 1;

#if 0
       // Example using implicit functions (NOT allowed for C++).
          if (status)
               status = foobar_1(status) + foobar_2(status);
#endif
          status = 1;
        }
   }
