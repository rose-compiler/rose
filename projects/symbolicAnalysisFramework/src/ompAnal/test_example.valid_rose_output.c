#pragma omp greg
extern int omp_get_num_threads();
extern int omp_get_thread_num();
extern void genericForHeader();
extern void foo();
extern void bar();
int a;
int b;
int c;

int main(int argc,char **argv)
{
  int d[10];
  int e;
  int f;
  
#pragma omp parallel private(a, d) firstprivate(b, c) default(shared)
{
    foo();
    foo();
    
#pragma omp for ordered
    for (int iterator = 0; iterator < 10; iterator++) {
      a = (b = (c = 0));
    }
  }
  bar();
  
#pragma omp parallel private ( a, d ) firstprivate ( b, c ) default ( shared )
{
{
      d[0] = (e = (f = 1));
    }
  }
  
#pragma omp critical
{
    foo();
    bar();
  }
  
#pragma omp for nowait
  for (int i = 0; i < 1; ++i) {
    bar();
  }
  if (omp_get_thread_num() == 0) {
    bar();
  }
  else {
    omp_get_thread_num();
  }
  
#pragma omp critical
{
    foo();
{
    }
  }
  
#pragma omp for
  for (int i = 0; i < 1; ++i) {{
      d[0] = (e = (f = 1));
    }
  }
  if (omp_get_thread_num() == 0) {{
      d[0] = (e = (f = 1));
    }
  }
  else {
    omp_get_thread_num();
  }
  return 0;
}

