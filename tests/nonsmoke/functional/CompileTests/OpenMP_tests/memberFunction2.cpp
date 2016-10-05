typedef double real8;
typedef real8 Real_t;
 
struct st {
    void foo( ) {}
};

void foo()
{
    Real_t  gamma[1];

#pragma omp parallel
    gamma[0] = 0.;
}
