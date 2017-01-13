
extern int kill (int x, char* s);

void handle_sub(int job, int fg)
   {
     int status;

     for (;;)
        {
          if (2)
             {
               if (2)
                    42;
             }
#if 1
            else
             {
            // Explicit "{}" are important for this bug.
               if (6)
                    kill(((__extension__ ({ union { int i; } u; u.i = 42; }) )),"");
             }
#endif
          break;
        }
   }

