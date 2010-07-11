#include <stdio.h>
#include <stdlib.h>

//
// Test incomplete aggregate specification
//
struct Pair {
    int left;
    int right;
};


char blockb[22] = { '/', '.' };
int int_array[10] = { 0, 1, 2, 3, 4 };
char *string_array[10] = { "a", "b", "c", "d", "e" };

struct Pair pair_array[10] = { {0, 0}, {1, 2}, {2, 4}, {3, 6}, {4, 8} };

int b[3][5] = { {0, 1, 2, 3, 4},
                {2, 4, 6, 8, 10},
                {3, 6, 9, 12, 15}
              };

int c[2][3][5] = {
                     { {0, 1, 2, 3, 4},
                       {2, 4, 6, 8, 10},
                     },
                     { {0, 1, 2, 3, 4},
                       {2, 4, 6, 8, 10},
                       {3, 6, 9, 12}
                     }
                 };

int main(int argc, char *argv[10]) {
    printf("blockb = %s\n", blockb);

    for (int i = 0; i < 10; i++)
      printf("int_array[%i] = %i\n", i, int_array[i]);

    pair_array[5].left = 5;
    pair_array[5].right = 10;
    int k = pair_array[5].right;
    printf("k = %i\n", k);
    for (int i = 0; i < 10; i++)
        printf("pair_array[%i] = {%i, %i}\n", i, pair_array[i].left, pair_array[i].right);

    return 0;
}
