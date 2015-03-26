#define __offsetof(type, member)  __builtin_offsetof (type, member)

typedef unsigned long size_t;

struct list_head {
    struct list_head *next, *prev;
};


static __inline__ void prefetch(const void *x) {;}


struct range {
    struct list_head list;
    unsigned long s, e;
};


struct rangeset {
    struct list_head rangeset_list;
 // struct domain *domain;
    struct list_head range_list;
 // spinlock_t lock;
    char name[32];
    unsigned int flags;
};


static struct range *find_range(
    struct rangeset *r, unsigned long s)
{
    struct range *x = ((void*)0), *y;
 // for (y = ({ typeof( ((typeof(*y) *)0)->list ) *__mptr = ((&r->range_list)->next); (typeof(*y) *)( (char *)__mptr - __builtin_offsetof(typeof(*y),list) );}); 
 // for (y = ({ typeof( ((typeof(*y) *)0)->list ) *__mptr = 0L; (typeof(*y) *)( (char *)__mptr - __builtin_offsetof(typeof(*y),list) );}); 
 // for (y = ({ typeof( ((typeof(*y) *)0)->list ) *__mptr = 0L; (typeof(*y) *) ( (char *)__mptr - 0L );}); 
    for (y = ({ typeof( ((typeof(*y) *)0)->list ) *__mptr = 0L; (typeof(*y) *)( (char *)__mptr - __builtin_offsetof(typeof(*y),list) );}); 
      // prefetch(y->list.next), &y->list != (&r->range_list); 
         0;
      // y = ({ typeof( ((typeof(*y) *)0)->list ) *__mptr = (y->list.next); (typeof(*y) *)( (char *)__mptr - __builtin_offsetof(typeof(*y),list) );}))
         y++)
    {
        if ( y->s > s )
            break;
        x = y;
    }
    return x;
}
