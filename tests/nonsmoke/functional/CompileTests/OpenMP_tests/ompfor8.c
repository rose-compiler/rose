extern double f (double, double);

void foo(int nthreads, int size, int numiter, double *V, int totalSize)
{
int i, iter; 
for(iter=0; iter<numiter; iter++) {

#pragma omp parallel for default(none) shared(V,totalSize) private(i) schedule(static) ordered
        for (i=0; i<totalSize-1; i++) {
                V[i] = f(V[i],V[i-1]);
                }

        }

}
