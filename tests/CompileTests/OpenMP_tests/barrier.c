extern void do_sth();
void foo()
{
#pragma omp parallel
  {
    do_sth();
#pragma omp barrier
    do_sth();
  }
}
