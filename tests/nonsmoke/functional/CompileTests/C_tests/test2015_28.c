
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


void foobar()
   {
     char *name;
     struct rangeset *r;

     if ( name != ((void*)0) )
        {
          ({ ((void)sizeof(struct { int:-!!(!__builtin_types_compatible_p(typeof(r->name), char[])); })); (strlcpy(r->name, name, sizeof(r->name)) >= sizeof(r->name)); });
        }   
}
