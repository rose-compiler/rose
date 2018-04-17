// test some of the funkier gcc attribute things:
// http://gcc.gnu.org/onlinedocs/gcc-2.95.3/gcc_4.html#SEC90

// pretty normal
int x0 __attribute__ ((nocommon));
int x0b __attribute ((nocommon));

// I don't know what this means but we parse it
int x1 __attribute__ ((mode (byte)));
int x2 __attribute__ ((mode (__byte__)));
int x3 __attribute__ ((mode (word)));
int x4 __attribute__ ((mode (__word__)));
int x5 __attribute__ ((mode (pointer)));
int x6 __attribute__ ((mode (__pointer__)));

int z0 __attribute__ ((model (small)));
int z1 __attribute__ ((model (medium)));
int z2 __attribute__ ((model (large)));

int w0 __attribute__ ((aligned (__alignof (int))));
struct A {} __attribute__ ((aligned (__alignof (int))));

// currently crashes gcc 3.4
//  struct A {} __attribute__ ((mode (byte)));
//  struct A {} __attribute__ ((mode (__byte__)));
//  struct A {} __attribute__ ((mode (word)));
//  struct A {} __attribute__ ((mode (__word__)));
//  struct A {} __attribute__ ((mode (pointer)));
//  struct A {} __attribute__ ((mode (__pointer__)));
