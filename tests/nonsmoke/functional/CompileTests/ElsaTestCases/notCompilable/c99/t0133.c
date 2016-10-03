// test compound initializers according to C99 p.125
void a() {
  // non-designated
  int *ip = 0;         // NULL
  struct foo {int x; int *xp; int y;};
  struct foo f = {1, ip, 2};

  // designated
  struct foo2 {int x2; int *xp2; int y2;};
  struct foo2 f2 = {y2:2, x2:1, xp2:ip};


  // non-designated compound literal
  struct fooB {int xB; int *xpB; int yB;};
  struct fooB fB;
  fB = (struct fooB) {1, ip, 2};

  // non-designated compound literal
  struct foo2B {int x2B; int *xp2B; int y2B;};
  struct foo2B f2B;
  f2B = (struct foo2B) {y2B:2, x2B:1, xp2B:ip};

  // embedded int array and struct
  // Skip over vars that are methods/ctors/dtors
  struct foo3 {int x3; int y3;};
  struct foo4 {int x4; int y4;};
  struct gronk {
    struct foo3 f3;
    int z[3];
    struct foo4 f4;
  };
  struct gronk g = {
    1, 2,        // f3
    3, 4, 5,     // z
    6, 7,        // f4
  };

  // embedded and nested int array and struct
  struct foo3b {int x3b; int y3b;};
  struct foo4b {int x4b; int y4b;};
  struct gronkb {
    struct foo3b f3b;
    int zb[3];
    struct foo4b f4b;
  };
  struct gronkb gb = {
    1, 2,        // f3b
    {3, 4, 5,},  // zb
    {6, 7,}      // f4b
  };

  // nested int array and struct; top level designated
  struct foo3c {int x3c; int y3c;};
  struct foo4c {int x4c; int y4c;};
  struct gronkc {
    struct foo3c f3c;
    int zc[3];
    struct foo4c f4c;
  };
  struct gronkc gc = {
    zc:{3, 4, 5,},
    f4c:{6, 7,},
    f3c:{y3c:2, x3c:1},
  };

  // array with no size
  struct foo10d {int xd; int yd[];};
  struct foo10d f10d = {3, 4, 5, 6};
  struct foo10e {int xe; int ye[]; int ze; int z2e;};
  struct foo10e f10e = {3, {4, 5}, 6, 7};
}

// testing nontrivial designated initializers
void b() {
  struct foo {
    int fooa;                   // (6)
    int foor;
    int foox;                   // (5)
    double fooy;
    int fooz[2];                // (1)
    double foow[];              // (2)
    int fooq;
  };
  struct gronk {
    struct foo f1[];
    int gronk1;
    struct {
      int x;
      double y;                 // (3)
      int z[];                  // (4)
      int w;
    } f2;
    double gronk2;
    struct foo f3[];
  };
  struct bar {
    struct gronk g[];
    struct foo f;
    struct gronk g2;
  };

  struct bar b[] = {
    [3].g[2].f1[2].fooz[1] = 1, 2, // (1)
    3,                          // should be foo.foow (2)
    [2].g[2].f2.y = 3.2,        // anon(f2).y (3)
    4, 5, 6, 7,                 // anon(f2).z (4)
    [4] = {                     // current_object == bar now
      .g2.f3 = {                // current_object == gronk.f3 now
        [2] = {
          .foox = 18            // foo.foox (5)
        }
      }
    },
    {
      .f = 9                    // bar[5].f, the first elt of which is fooa (6)
    }
  };
}

void c() {
  int x[] = { [1 ... 3] = 8 };
}
