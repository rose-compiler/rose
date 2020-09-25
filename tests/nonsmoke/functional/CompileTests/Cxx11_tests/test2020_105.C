
void foobar()
   {
     while (1)
     #pragma omp task 0
        {
        }

     if (1)
     #pragma omp task 5
       42;
   }

