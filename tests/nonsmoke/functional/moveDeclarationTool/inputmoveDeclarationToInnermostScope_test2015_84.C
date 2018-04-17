void foobar()
   {
     bool s2;

#if 1
     if (true) 
        {
          if (0) 
             {
               s2 = true ;
             }
#else
          int xxx;
#endif
        }

     return ;
   }
