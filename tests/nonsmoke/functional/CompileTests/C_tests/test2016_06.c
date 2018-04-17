
#if 1
void foobar0 ( int size, int array[*] );
#endif

#if 1
// void foobar1 ( int size, int array[*] );
void foobar1 ( int size, int array[size] )
   {
   }
#endif

#if 1
// void foobar2 ( int size, int array[size] );
// void foobar2 ( int size, int array[size] );
void foobar2 ( int size, int array[size+1] )
   {
   }
#endif

#if 1
int global_size = 42;
void foobar3 ( int size, int array[size*12] );
void foobar3 ( int size, int array[size+global_size+42] )
   {
   }
#endif
