/*
This bug demonstrates that the unparsing of comments with the AST
and one of the problems with doing this.

Original code:
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("Leaving Array_Domain_Type::operator delete! \n");
#endif

Unparsed code:
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          { printf(("Leaving Array_Domain_Type::operator delete! \n")); } 
} #endif

 */

#define NULL (void*)0
#define myAssert(expression) \
     if (expression) \
          0; \
       else \
          1;

void
foo ( char* c )
   {
     myAssert (c != NULL);
#if 0
     char* x = c;
#else
     char* y = c;
#endif
   }


int
main()
   {
     int X = 42;
     foo ("a");
#if 1
#if 1
     myAssert (X == 0);
     if (1)
          X = 1;
#endif

  // comment
#endif
   }


