#include <stdio.h>
#include <stdlib.h>

static int data[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
static int size = 10;

//
// Test break and continue statements
//
int main(int argc, char *argv[]) {
    int i = 0;

    printf("Print even numbers up to 9 with do-while-loop:\n\n");
    do {
        if (i % 2)
            continue;
        printf("    Element %i is %i\n", i, data[i]);
    } while(++i < size);

    printf("\nPrint natural numbers up to 6 with do-while-loop:\n\n");
    i = 0;
    do {
        printf("    Element %i is %i\n", i, data[i]);
        if (i == 6)
            break;
    } while(++i < size);

    printf("\nPrint even numbers up to 9 with for-loop:\n\n");
    for (int i = 0; i < size; i++) {
        if (i % 2)
            continue;
        printf("    Element %i is %i\n", i, data[i]);
    }

    printf("\nPrint natural numbers up to 6 with for-loop:\n\n");
    for (int i = 0; i < size; i++) {
        printf("    Element %i is %i\n", i, data[i]);
        if (i == 6)
            break;
    }

    printf("\nPrint even numbers up to 9 with while-loop:\n\n");
    i = 0;
    while(i < size) {
        if (i % 2) {
            i++;
            continue;
        }
        printf("    Element %i is %i\n", i, data[i]);
        i++;
    }

    printf("\nPrint natural numbers up to 6 with while-loop:\n\n");
    i = 0;
    while(i < size) {
        printf("    Element %i is %i\n", i, data[i]);
        if (i == 6)
            break;
        i++;
    }

    return 0;
}

