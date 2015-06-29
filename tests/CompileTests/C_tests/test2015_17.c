
#define __offsetof(type, member)  __builtin_offsetof (type, member)

typedef unsigned long size_t;


struct list_head {
    struct list_head *next, *prev;
};


struct range {
    struct list_head list;
 // unsigned long s, e;
};


void foobar()
   {
     struct range *y;
     y = ({ typeof( ((typeof(*y) *)0)->list ) *__mptr = 0L; (typeof(*y) *)( (char *)__mptr - __builtin_offsetof(typeof(*y),list) );}); 

  // struct range* __mptr = 0L; 
  // (struct range*) ( (char *)__mptr - __builtin_offsetof(typeof(*y),list) );}); 
   }
