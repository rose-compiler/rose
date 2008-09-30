/* test pragma under the true body of if statement */
extern void process(int);
int item[100];
void foo(int i)
{
  if (i%2==0)
    #pragma omp task
    process (item[i]);
}


