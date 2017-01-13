

#if 1
void foobar1 ( int size, int array[*] );
void foobar1 ( int size, int array[size] )
   {
   }
#endif

#if 0
void foobar1 ( int size1, int array1[size1] );
void foobar2 ( int size, int array[size] );
#endif

#if 0
void foobar2 ( int size, int array[size] );
#endif

#if 1
// void foobar2 ( int sizeVar, int array[sizeVar] );
// void foobar2 ( int sizeVar, int array[*] );
void foobar2 ( int size, int array[*] );
#endif
#if 1
void foobar2 ( int sizeVar, int array[sizeVar] )
   {
   }
#endif

