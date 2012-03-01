
/* Function pointer */

/* Loop bounds */

/* Path and interval analysis */

/* Recursion depth */

void fun_a( void )
{
}

/*RPT overide_wcet_calls( "fun_b", "100" ); */
void fun_b( void )
{
}

void call_a_function( void (*function_pointer)( void ) )
{
   (*function_pointer)( );
}

void do_a( void )
{
   call_a_function( fun_a );
}

void do_b( void )
{
   call_a_function( fun_b );
}

void root( void )
{
   do_a( );
   do_b( );
}

int main( void )
{
   root( );
   return 0;
}
