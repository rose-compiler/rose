#define __offsetof(type, member)  __builtin_offsetof (type, member)

typedef unsigned long size_t;

struct list_head {};

struct range 
   {
     struct list_head list;
   };

void foobar()
   {
     int i;
  // The bug is that this is unparsed as: i = (__offsetof(struct range ,( *0) . list));
     i = __builtin_offsetof(struct range,list);
   }
