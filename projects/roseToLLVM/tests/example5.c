#include <stdio.h>
#include <stdlib.h>

//
// Test Aggregate Arrays
//
int one_dim[10],
    three_dim[10][10][10];
int main(int argc, char *argv[]) {
    int a[] = { 0, 1, 2, 3, 4};

    int b[3][5] = { {0, 1, 2, 3, 4},
                    {2, 4, 6, 8, 10},
                    {3, 6, 9, 12, 15}
                  };

    int c[2][3][5] = {
                         { {0, 1, 2, 3, 4},
                           {2, 4, 6, 8, 10},
                           {3, 6, 9, 12, 15}
                         },

                         { {0, 1, 2, 3, 4},
                           {2, 4, 6, 8, 10},
                           {3, 6, 9, 12, 15}
                         }
                     };

    for (int k = 0; k < 5; k++)
        a[k] = a[k] * 2;
    for (int k = 0; k < 5; k++)
      printf("a[%i] = %i\n", k, a[k]);

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 5; j++)
            b[i][j] = b[i][j] + 1;

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 5; j++)
            printf("b[%i][%i] = %i\n", i, j, b[i][j]);

    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 3; j++)
            for (int k = 0; k < 5; k++)
                printf("c[%i][%i][%i] = %i\n", i, j, k, c[i][j][k]);

    for (int i = 0; i < 10; i++) {
        one_dim[i] = i;
    }

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            for (int k = 0; k < 10; k++) {
                three_dim[i][j][k] = (i * 10 + j) * 10 + k;
            }
	}
    }

    for (int i = 0; i < 10; i++) {
        printf("one_dim[%i] = %i\n", i, one_dim[i]);
    }

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            for (int k = 0; k < 10; k++) {
                printf("three_dim[%i][%i][%i] = %i\n", i, j, k, three_dim[i][j][k]);
            }
	}
    }

    return 0;
}
