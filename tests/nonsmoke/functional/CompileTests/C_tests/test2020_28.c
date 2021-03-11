// RC-61:  Tristan

struct _direntry {};

void foo() 
   {
     struct { struct _direntry * next; } anchor;

  // For C langauge support "struct" must be output in generated code.
     for (; (struct _direntry *)( anchor.next ); ) 
        {
        }

   }

