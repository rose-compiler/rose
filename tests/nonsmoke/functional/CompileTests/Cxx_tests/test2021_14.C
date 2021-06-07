
void foobar();

int main()
   {
     foobar();
#pragma omp for
     for (int i=0; i<1; i++)
        {
          foobar();
        }
     return 0; 
   }
