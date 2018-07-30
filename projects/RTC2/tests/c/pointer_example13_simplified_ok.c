#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define PTR1_SIZE 100
#define PTR2_SIZE 10
#define ARRAY1_ACCESS 29
#define ARRAY2_ACCESS 5

int main()
{
    unsigned int  array1[PTR1_SIZE];
    unsigned int  array2[PTR2_SIZE];

    array1[ARRAY1_ACCESS] = array2[ARRAY2_ACCESS]++;

    return 0;
}
