#include <stdio.h>

int main(int argc, char *argv[]) {
    int a[] = { 0, 1, 2, 3, 4};

    for (int k = 0; k < 5; k++)
        printf("a[%i] = %i\n", k, a[k]);
    
    return 0;
}
