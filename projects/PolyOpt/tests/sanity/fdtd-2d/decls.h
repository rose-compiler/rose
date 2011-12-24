#define CONST_tmax 500
#define CONST_nx 1000
#define CONST_ny 1000

static int tmax = CONST_tmax;
static int nx = CONST_nx;
static int ny = CONST_ny;

double ex[CONST_nx][CONST_ny+1];
double ey[CONST_nx+1][CONST_ny];
double hz[CONST_nx][CONST_ny];
