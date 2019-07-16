// This code has a simplified representation in test2015_53.c and test2015_93.c.

// This test code demonstrates that the use of the GNU language extension (statement-expression)
// in the default statement of the switch causes the while statement after the switch to not
// be unparsed (maybe even not in the AST).

void foobar()
   {
  // switch( ({ char * x_ptr; x_ptr; }) == 0L)
  // switch( ({ char* x_ptr; x_ptr; }) == 42)
#if 1
     if( ({ char* x_ptr; x_ptr; }) == 42)
        {
        }
#endif

  // switch( ({ char* x_ptr; x_ptr; }) == (void*)0L)
     switch( ({ int x_ptr; x_ptr; }) == 0L)
        {
        }
   }
