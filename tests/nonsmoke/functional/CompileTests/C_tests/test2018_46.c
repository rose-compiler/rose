// This is a bug in the unparsed code (without the complete type for the struct).
void foobar()
   {
     long long_var = 0;
     char *p = 0;
     p = (char *)(struct incast { int a; int b; } * )p;
     long_var = sizeof(struct incast);	
   }
