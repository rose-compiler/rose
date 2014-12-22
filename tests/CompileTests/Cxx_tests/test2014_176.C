// Original code uses __attribute__ ((__nonnull__ (1))) which is unparsed as __attribute__((no_throw))
// extern "C++"         void *rawmemchr (void *__s, int __c)         throw () __asm ("rawmemchr") __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
// extern "C++" __const void *rawmemchr (__const void *__s, int __c) throw () __asm ("rawmemchr") __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
extern "C++"         void *rawmemchr (void *__s, int __c)         throw () __asm ("rawmemchr") __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
extern "C++" __const void *rawmemchr (__const void *__s, int __c) throw () __asm ("rawmemchr") __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));

