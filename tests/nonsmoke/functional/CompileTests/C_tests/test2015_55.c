
struct X {};

typeof(struct X) X_array[42];

void foobar(void)
   {
     typeof(&X_array) __ptr;
   }
