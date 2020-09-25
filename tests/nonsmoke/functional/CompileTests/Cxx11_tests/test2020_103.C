

void foobar()
   {
     while (1)
     #pragma omp task 0
        {
        }

#if 1
     for (;;)
     #pragma omp task 1
        {
        }

     switch (1)
     #pragma omp task 2
        {
        }

     do
     #pragma omp task 3
       {
       }
     while (1);
#endif

     if (1)
     #pragma omp task 5
          foobar(); 


   }

