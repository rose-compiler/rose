
void foo()
   {
     if (1)
        {
          int i;
       // extern int (*_gl_verify_function41 (void)) [(!!sizeof (struct { unsigned int _gl_verify_error_if_negative: (1) ? 1 : -1; }))];
       // extern int a [(!!sizeof (struct { unsigned int _gl_verify_error_if_negative: (1) ? 1 : -1; }))];
       // extern int b [(sizeof (struct { unsigned int _gl_verify_error_if_negative:1; }))];
          int (*_gl_verify_function41 (void)) [(!!sizeof (struct { unsigned int _gl_verify_error_if_negative: (42) ? 1 : -1; }))];

          i = 5;
          int j = 6;
          j = 7;
        }

     return;
   }
