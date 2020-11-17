
void foobar()
   {
     do
     #pragma omp task 3
       {
       }
     #pragma omp task 4
     while (1);
   }

