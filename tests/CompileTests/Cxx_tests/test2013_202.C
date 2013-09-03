// This test code demonstrates an un-terminated #if..#endif pair.

int ghtonl(int x) 
   {
#if 1
     union
        {
          int result;
        };
#endif

     return result;
   }
