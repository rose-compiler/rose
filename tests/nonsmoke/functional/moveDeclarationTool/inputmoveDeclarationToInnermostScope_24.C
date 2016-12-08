void foo1();
int foo2();

int foobar()
   {
     int pid;

#if 1
     if ( 1 ) {
#if 0
          pid = foo2();
#else
          pid = foo2();
#endif

          if ( pid == -1 ) { }
     }
#else
     foo1();
#endif

     return( 0 ) ;
   }

