#include <stdio.h>

//
// Test sizeof
//
int a[] = { 0, 1, 2, 3, 4, 5};
FILE output[10];
int main(int argc, char *argv[]) {
    int i = sizeof(double),
        k = sizeof(a),
        l = sizeof("abc"),
        m = sizeof(output),
        n = sizeof(5 + 4);

    printf("sizeof(double) = %i;  sizeof(int a[] = { 0, 1, 2, 3, 4, 5}) = %i;  sizeof(\"abc\") = %i;  sizeof(output) = %i;  sizeof(5 + 4) = %i;\n", i, k, l, m, n);

    printf("sizeof(void *) is %i\n", sizeof(void *));
    printf("sizeof(char) is %i\n", sizeof(char));
    printf("sizeof(short) is %i\n", sizeof(short));
    printf("sizeof(int) is %i\n", sizeof(int));
    printf("sizeof(long) is %i\n", sizeof(long));
    printf("sizeof(long long) is %i\n", sizeof(long long));
    printf("sizeof(float) is %i\n", sizeof(float));
    printf("sizeof(double) is %i\n", sizeof(double));
    printf("sizeof(long double) is %i\n", sizeof(long double));
    printf("sizeof(FILE) is %i\n", sizeof(FILE));
    printf("sizeof(__sbuf) is %i\n", sizeof(struct __sbuf));

    return 0;
}
