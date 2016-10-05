
#if 1

struct X
   {
  // int a;
   };

void foobar()
   {
     int x = sizeof(struct X);

     int y;
     y -= sizeof(const struct X);

     int z;
     z -= sizeof(struct X);
   }

#endif

