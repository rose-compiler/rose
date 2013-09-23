typedef long zlong;

typedef struct hashnode *HashNode;

typedef struct param *Param;


struct hashnode 
   {
     HashNode next;
     char *nam;
     int flags;
   };



typedef const struct gsu_scalar *GsuScalar;
typedef const struct gsu_integer *GsuInteger;
typedef const struct gsu_float *GsuFloat;
typedef const struct gsu_array *GsuArray;
typedef const struct gsu_hash *GsuHash;

struct gsu_scalar {
    char *(*getfn) (Param);
    void (*setfn) (Param, char *);
    void (*unsetfn) (Param, int);
};

struct gsu_integer {
    zlong (*getfn) (Param);
    void (*setfn) (Param, zlong);
    void (*unsetfn) (Param, int);
};

struct gsu_float {
    double (*getfn) (Param);
    void (*setfn) (Param, double);
    void (*unsetfn) (Param, int);
};

struct gsu_array {
    char **(*getfn) (Param);
    void (*setfn) (Param, char **);
    void (*unsetfn) (Param, int);
};

struct gsu_hash {
 // HashTable (*getfn) (Param);
 // void (*setfn) (Param, HashTable);
    void (*unsetfn) (Param, int);
};

extern zlong poundgetfn (Param pm __attribute__((__unused__)));
extern void nullintsetfn (Param pm __attribute__((__unused__)),zlong x __attribute__((__unused__)));
extern void stdunsetfn (Param pm,int exp __attribute__((__unused__)));

extern zlong errnogetfn (Param pm __attribute__((__unused__)));
extern void errnosetfn (Param pm __attribute__((__unused__)),zlong x);

static const struct gsu_integer pound_gsu = { poundgetfn, nullintsetfn, stdunsetfn };
static const struct gsu_integer errno_gsu = { errnogetfn, errnosetfn, stdunsetfn };


struct param 
   {
     struct hashnode node;

     union 
        {
          void *data;
          char **arr;
          char *str;
          zlong val;
          zlong *valptr;
          double dval;

       // HashTable hash;
        } u;

     union 
        {
          GsuScalar s;
          GsuInteger i;
          GsuFloat f;
          GsuArray a;
          GsuHash h;
        } gsu;

     int base;
     int width;
     char *env;
     char *ename;
     Param old;
     int level;
   };


typedef struct param initparam;

static initparam special_params[] = {
   { {((void *)0),"#",       (1<<1)|(1<<22)|(1<<10)},          {((void *)0)}, {(GsuScalar)(void *)(&(pound_gsu))},     10,0,((void *)0),((void *)0),((void *)0),0},
   { {((void *)0),"ERRNO",   (1<<1)|(1<<22)|0},                {((void *)0)}, {(GsuScalar)(void *)(&(errno_gsu))},     10,0,((void *)0),((void *)0),((void *)0),0}
#if 0
   { {((void *)0),"GID",     (1<<1)|(1<<22)|(1<<23) | (1<<24)},{((void *)0)}, {(GsuScalar)(void *)(&(gid_gsu))},       10,0,((void *)0),((void *)0),((void *)0),0},
   { {((void *)0),"EGID",    (1<<1)|(1<<22)|(1<<23) | (1<<24)},{((void *)0)}, {(GsuScalar)(void *)(&(egid_gsu))},      10,0,((void *)0),((void *)0),((void *)0),0},
   { {((void *)0),"HISTSIZE",(1<<1)|(1<<22)|(1<<24)},          {((void *)0)}, {(GsuScalar)(void *)(&(histsize_gsu))},  10,0,((void *)0),((void *)0),((void *)0),0},
   { {((void *)0),"RANDOM",  (1<<1)|(1<<22)|0},                {((void *)0)}, {(GsuScalar)(void *)(&(random_gsu))},    10,0,((void *)0),((void *)0),((void *)0),0},
   { {((void *)0),"SAVEHIST",(1<<1)|(1<<22)|(1<<24)},          {((void *)0)}, {(GsuScalar)(void *)(&(savehist_gsu))},  10,0,((void *)0),((void *)0),((void *)0),0},
   { {((void *)0),"SECONDS", (1<<1)|(1<<22)|0},                {((void *)0)}, {(GsuScalar)(void *)(&(intseconds_gsu))},10,0,((void *)0),((void *)0),((void *)0),0},
   { {((void *)0),"UID",     (1<<1)|(1<<22)|(1<<23) | (1<<24)},{((void *)0)}, {(GsuScalar)(void *)(&(uid_gsu))},       10,0,((void *)0),((void *)0),((void *)0),0},
   { {((void *)0),"EUID",    (1<<1)|(1<<22)|(1<<23) | (1<<24)},{((void *)0)}, {(GsuScalar)(void *)(&(euid_gsu))},      10,0,((void *)0),((void *)0),((void *)0),0},
   { {((void *)0),"TTYIDLE", (1<<1)|(1<<22)|(1<<10)},          {((void *)0)}, {(GsuScalar)(void *)(&(ttyidle_gsu))},   10,0,((void *)0),((void *)0),((void *)0),0}
#endif
};


