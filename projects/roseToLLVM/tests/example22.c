#include <stdio.h>
#include <stdlib.h>

//
// structure tests
//
struct mystruct {
       double value;
       struct mystruct *next;
} s;

// This function takes a structure as argument and returns a structure
struct mystruct function(struct mystruct a) {
    a.value *= 2;
    return a;
}

int main(int argc, char *argv[]) {

    struct mystruct t,
                    u;

    t.value = 12;
    u = function(t);

    printf("t.value = %f; u.value = %f\n", t.value, u.value);
 
    return 0;
}
