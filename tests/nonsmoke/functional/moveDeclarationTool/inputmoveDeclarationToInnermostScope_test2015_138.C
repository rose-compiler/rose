#define MACRO(X)  \
  if (X) \
     {   \
       a = 42 ; \
       b = 42;  \
     };

void
foobar()
   {
     int a,b;
     int n;

  // Note that if the expanded macro is ransformed, the AST is output and we can't also output the macro.
     for (n=0; n<42; n++) 
        {
          MACRO(42)
        }
   }

