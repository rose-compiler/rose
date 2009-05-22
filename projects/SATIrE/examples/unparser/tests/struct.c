struct {
    struct {
      int b1;
      int b2;
    };
    union {
      float f1;
      int i1;
    };
    int b3;
  } foo = {{31, 17}, {3.2}, 13};

struct a {int b} c;

  int
  main ()
  {
    int b1 = foo.b1;
    int b3 = foo.b3;
    return 0;
  }
