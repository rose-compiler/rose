#include <omp.h>

int main() {
    int A[1] = {1};
#pragma omp task
    {
        A[0];
    }
    return 0;
}
