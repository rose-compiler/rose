struct echo_options 
   {
     int no_newline;
   };

int global_var;

#if 1
void foobar()
   {
  // Note the bug is that the typeof opertor not output in argument type for function type unparsing.
  // if ( ( ( int ( * ) ( const char *text, typeof ( ( ( struct echo_options * ) ((void *)0) )->no_newline ) * ) ) ((void *)0) ) )
  // if ( ( ( int ( * ) ( const char *text, typeof ( global_var ) * ) ) ((void *)0) ) )
     if ( ( ( int ( * ) ( const char *text, typeof ( global_var ) ) ) ((void *)0) ) )
        {
        }
   }
#endif

#if 1
// This will unparse fine!
// int (foobar_function) ( const char *text, typeof ( ( ( struct echo_options * ) ((void *)0) )->no_newline ) * );
// int (foobar_function) ( const char *text, typeof ( global_var ) * );
int (foobar_function) ( const char *text, typeof ( global_var ) );
#endif
