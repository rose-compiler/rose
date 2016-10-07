
typedef struct builtin *Builtin;



#if 0
typedef struct hashnode *HashNode;
struct hashnode {
    HashNode next;
    char *nam;
    int flags;
};
#endif

#if 0
typedef struct options *Options;
struct options {
    unsigned char ind[128];
    char **args;
    int argscount, argsalloc;
};
#endif

// typedef int (*HandlerFunc) (char *, char **, Options, int);
typedef int (*HandlerFunc)();

struct builtin {
 // struct hashnode node;
 // HandlerFunc handlerfunc;
    int *handlerfunc;
    int minargs;
    int maxargs;
    int funcid;
    char *optstr;
    char *defopts;
};

// static int bin_zpty (char*nam,char**args,Options ops,int func __attribute__((__unused__)));
static int bin_zpty (char*nam,char**args,int func __attribute__((__unused__)));

// static struct builtin bintab[] = { { { ((void *)0), "zpty", 0 }, bin_zpty, 0, -1, 0, "ebdmrwLnt", ((void *)0) }, };
static struct builtin bintab[] = { { bin_zpty, 0, -1, 0, "ebdmrwLnt", ((void *)0) }, };

