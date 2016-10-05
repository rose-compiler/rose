// The ellipsis WAS previouly output correctly for a function prototype.
// Note that any un-named parameters in the function prototype are filled in with 
// their name from the defining function declaration.  We might want to fix this
// to be more precise as a source-to-source representation.
void testFunction (int ,...);

// The ellipsis WAS also previouly output correctly for defining function declarations.
void testFunction (int x,...)
   {
   }

// The ellipsis was previously NOT output correctly in the function type of the pointer to a function.
void msg_rate_delay( void (*log_fn) (const char *,...) )
   {
  // char* buf;
  // log_fn("%s", buf);
     log_fn("%s", 0);
   }
