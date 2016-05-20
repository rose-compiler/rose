

void foobar1 ( int size, int array[*] );
void foobar1 ( int size, int array[size] )
   {
   }

#if 0
// void foobar2 ( int sizeVar, int array[sizeVar] );
// void foobar2 ( int sizeVar, int array[*] );
void foobar2 ( int size, int array[*] );
#endif

void foobar2 ( int sizeVar, int array[sizeVar] )
   {
   }

