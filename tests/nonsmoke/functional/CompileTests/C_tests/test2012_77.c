

#define __builtin_offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#define alignof(type) __builtin_offsetof (struct { char c; type member; }, member)

// typedef unsigned long uint32_t;
typedef unsigned long size_t;

void foobar(const void *buffer)
   {
#if 1
// # define UNALIGNED_P(p) ((uintptr_t) (p) % alignof (uint32_t) != 0)
#define UNALIGNED_P(p) ((unsigned long) (p) % alignof (unsigned int) != 0)
      if (UNALIGNED_P(buffer))
         {
         }
#endif
      if ((unsigned long) (buffer) % alignof (unsigned int) != 0)
         {
         }
   }
