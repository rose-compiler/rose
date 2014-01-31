// DQ (1/20/2014): Example bug from handling Valgrind.

typedef struct _IRExpr IRExpr;

struct _IRExpr { int x; };

struct _MCEnv;

// ROSE unparses: static IRExpr *expr2vbits(MCEnv *mce,IRExpr *e);
static IRExpr* expr2vbits ( struct _MCEnv* mce, IRExpr* e );

typedef struct _MCEnv {} MCEnv;

static IRExpr* expr2vbits ( MCEnv* mce, IRExpr* e ) {}
