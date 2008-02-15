/*
test preprocessing info before and after a statement
*/
int main (int argc,char** argv)
{
int i;
#if defined(_OPENMP)
  #pragma omp master
  {
    i++;
  }
#endif
return 0;
}
