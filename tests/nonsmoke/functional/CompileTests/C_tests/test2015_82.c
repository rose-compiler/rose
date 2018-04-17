typedef unsigned char u8;

extern u8 *memnodemap;

static typeof(*memnodemap) _memnodemap[64];

unsigned long memnodemapsize;

void foobar()
   {
     _memnodemap[0];

  // rose_test2015_82.c:9: error: subscripted value is neither array nor pointer
  // if (memnodemapsize <= (sizeof(_memnodemap) / sizeof((_memnodemap)[0]) + sizeof(struct { int:-!!(__builtin_types_compatible_p(typeof(_memnodemap), typeof(&_memnodemap[0]))); })))
     if (memnodemapsize <= (sizeof(_memnodemap) / sizeof((_memnodemap)[0]) + sizeof(struct { int:-!!(__builtin_types_compatible_p(typeof(_memnodemap), typeof(&_memnodemap[0]))); })))
        {
        }
   }
