
#if 0
// This fails
#define MY_MACRO_2(c) c; c; c
#else
// This works
#define MY_MACRO_2(c) c; c; c;
#endif

void foobar()
   {
     double cc;

        {
       // Either of these will work if the macro is defined with a ending ";".
       // MY_MACRO_2(cc)
          MY_MACRO_2(cc);
        }
   }

