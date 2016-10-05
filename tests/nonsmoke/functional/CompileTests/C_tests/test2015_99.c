
struct echo_options 
   {
     int no_newline;
   };

void foobar()
   {
#if 0
     typeof ( ( ( struct echo_options * ) ((void *)0) )->no_newline ) * abc;
     int ( *abc_function_ptr ) ( const char *text, typeof(((struct echo_options *)((void *)0)) -> no_newline) ) ;
#endif

     if ( 0L == ( ( int ( * ) ( const char *text, typeof ( ( ( struct echo_options * ) ((void *)0) )->no_newline ) * ) ) ((void *)0) ) )
        {
        }

     if ( ( ( int ( * ) ( const char *text, typeof ( ( ( struct echo_options * ) ((void *)0) )->no_newline ) * ) ) ((void *)0) ) == 0L )
        {
        }
   }
