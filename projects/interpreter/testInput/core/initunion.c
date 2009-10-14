union x {
     void *p;
     int i;
};

static union x foo = { 0 };

int test(int i)
   {
     return (foo.p != 0);
   }
