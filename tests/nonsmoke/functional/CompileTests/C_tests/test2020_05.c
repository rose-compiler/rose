
void foobar(unsigned long *dst)
   {
     switch (1) 
        {
          default:
               __builtin_memset(dst,0,0);

          case 1:
               break;
        }
   }
