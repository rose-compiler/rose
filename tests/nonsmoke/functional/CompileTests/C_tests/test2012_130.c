
void dotrap (int sig);

void update_job()
   {
     if (1)
        {
       // int x = (signed char) (__extension__ ({ union { int __in; int __i; } __u; __u.__in = 41; __u.__i; }));
          int x = (signed char) (__extension__ ({ union { int i; } u; u.i = 42; }));

          if (2)
             {
               int y = x;
             }
        }
   }


