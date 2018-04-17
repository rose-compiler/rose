// Strange cases to worry about when unparsing the CCP directives

#define APP_DEBUG 0
#define COMPILE_DEBUG_STATEMENTS 1

int main ()
   {
     int x = 0;

  // next we have an if statement
     if (0)
        {
          x = 0;
        }
       else
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
               x = 1;
#endif
        }

     return x;
   }

