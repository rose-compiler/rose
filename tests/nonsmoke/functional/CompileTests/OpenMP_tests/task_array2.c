#include <omp.h>

typedef int int_arr[1];

int main() {
    int_arr A[1];
#pragma omp task firstprivate(A)
    {
        A[0][0] = 0;
    }
    return 0;
}
