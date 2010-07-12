#include <stdio.h>

//
// Test union
//
int main(int argc, char *argv[]) {
  struct mystruct { int i, j; } x; 
    union {
        int i;
        float f;
    } abc, *p;

    union U {
        double d;
        char c[2];
        int i;
    } object;
    float f;

    abc.f = 2.0;
    abc.i = 0;
    printf("abc.i =  %i; abc.f = %f\n", abc.i, abc.f);

    object.i = 55;
    object.d = 0;
    printf("object.d = %d; object.i = %f\n", object.d, object.i);

    p = &abc;
    p -> i = 0xFFFFFFFF;
    p -> f = 4.0;
    printf("p -> i =  %i; p -> f = %f\n", p -> i, p -> f);

    f = abc.f;

    return 0;
}

