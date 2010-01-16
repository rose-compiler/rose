/*
 *  test the handling of #endif at the end of a parallel region
 * Liao 1/15/2009
 * */
#define FORCE_TIED_TASKS
void find_queens (int size)
{
        int total_count=0;

        #pragma omp parallel
        {
                #pragma omp single
                {
                        char a[]="123";
                }
#ifdef FORCE_TIED_TASKS
                #pragma omp atomic
                        total_count += 1;
#endif
        }
}

void sort_par (int size)
{
#pragma omp parallel
#pragma omp single nowait
#pragma omp task untied
       sort_par(size);
}

