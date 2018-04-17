void do1();
void do2();
void do3();
void foo()
{
#pragma omp parallel proc_bind(spread)
  {
    do1();
  }
#pragma omp parallel proc_bind(master)
  {
    do1();
  }
#pragma omp parallel proc_bind(close)
  {
    do1();
  }

}
