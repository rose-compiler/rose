/*
 * test implicitly declared C function prototypes
 *  
 * Liao
 * 4/16/2010
 * */
int main()
{
  int k;

  {
 /* MSVC does not allow implicit functions: GNU specific */
 /* k = omp_get_num_threads(); */
  }
  return 0;
}

