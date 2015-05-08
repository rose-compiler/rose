
struct echo_options 
   {
     int no_newline;
   };

void foobar()
   {
  // Note the bug is that the typeof opertor not output in argument type for function type unparsing.
     if ( ( ( int ( * ) ( const char *text, typeof ( ( ( struct echo_options * ) ((void *)0) )->no_newline ) * ) ) ((void *)0) ) )
        {
        }
   }
