void
foo()
   {
#if 1
     if (int i = 0);

     for (; int b = 1; );

     for (int b = 1;; );

     do ; while (0);  // this should work (and does)
#endif

#if 1
     switch(int y = 1); // { default: ; }
#endif

#if 1
     while(int y = 1); // { ; }
#endif

#if 1
     switch(0) { int y; default: y = 0; }     

     switch(0)
        {
          int y;

       // Error: In the unparsed code the "default:" is not unparsed.
          default:
               y = 0;
        }
#endif
   }
