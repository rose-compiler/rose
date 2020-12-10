
void foobar()
   {
#if 1
     while (1)
     #pragma omp task 0
        {
        }
#endif

     if (1)
     #pragma omp task 5
        {
        }
   }

