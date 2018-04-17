// void foobar1 ( int size, int array[*] );
void foobar1 ( int size, int array[size] )
   {
   }

// This should be unparsed using the "[*]" syntax instead of "[size]"
void foobar2 ( int size, int array[*] );

