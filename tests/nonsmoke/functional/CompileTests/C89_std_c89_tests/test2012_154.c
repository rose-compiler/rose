void foobar()
   {
     int i;
     for(i; i < 10; i++)
# define MACRO() break
        {
          MACRO();
        }
   }
