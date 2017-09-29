// a header with OpenMP directive
#define PROBLEM_SIZE 1024
static double cuf[PROBLEM_SIZE];
static double q[PROBLEM_SIZE];
static double ue[PROBLEM_SIZE][5];
static double buf[PROBLEM_SIZE][5];

#pragma omp threadprivate(cuf, q, ue, buf)

