
void foobar( char x );

void foo()
   {
     char x = '\\';
     char y = "\\"[0];

     foobar('\\');

     foobar("\\"[0]);
   }
