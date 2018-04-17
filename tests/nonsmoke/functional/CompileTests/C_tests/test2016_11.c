
// void foobar1 ( int size, int array[*] );
void foobar1 ( int size, int array[size] )
   {
   }

// This will be normalized to the function parameter names from the defining declaration.
void foobar2 ( int sizeXX, int array[*] );
void foobar2 ( int sizeVar, int array[sizeVar] )
   {
   }

