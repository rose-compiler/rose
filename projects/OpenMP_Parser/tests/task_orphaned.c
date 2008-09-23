extern void process();
void foo()
{
 #pragma omp task
 process();
}
