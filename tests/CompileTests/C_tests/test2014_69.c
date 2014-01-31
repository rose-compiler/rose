#define BOUND -5
#define POSITIVE_BOUND 5

// These will be expanded to be: int x[-(-5)]; and int y[-(-5)];
int a[-BOUND];
int b[- -5];

void foo()
   {
  // Note that in a function the macro will be expanded differently: very strange.
  // Expanded to be: int x[--5]; and int y[--5];
     int x[-BOUND];
     int y[- -5];

     int z = -POSITIVE_BOUND;
   }

