/* This code demonstrates an error in how comments are woven back into the generated code.
 */
int
main()
   {
     int a;
     if ( true );

#if 0
        {
        }
#endif
     return 0;
   }
