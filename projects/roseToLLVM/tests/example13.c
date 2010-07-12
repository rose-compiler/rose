#include <stdio.h>
#include <stdlib.h>

//
// Test goto and labeled statements
//
static int data[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
static int size = 10;

int binSearch(int element) {
    int min = 0,
        max = size - 1;
top:
    if (min > max)
        goto exit;
    int mid = (min + max) / 2;
    if (element == data[mid])
         return mid;
    else if (element < data[mid])
         max = mid - 1;
    else min = mid + 1;
    goto top;
exit:
    return -1;
}

int main(int argc, char *argv[]) {
    int i = 0;
    do {
        printf("Element %i is at location %i\n", i, binSearch(i));
    } while(++i < size);


    int result = 0;
    for (int k = 0; k < 1000000; k++) {
        result += binSearch(k); // add to result to prevent call from being optimized away
    }

    printf("Interim result: %i\n", result);

    for (int k = 0; k < 1000000; k++) {
        result -= binSearch(k); // add to result to prevent call from being optimized away
    }

    printf("Done: %i\n", result);

    return 0;
}

