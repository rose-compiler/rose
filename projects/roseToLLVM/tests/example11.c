#include <stdio.h>
#include <stdlib.h>

struct mystruct {
    double value;
    struct mystruct *next;
    struct substruct {
        double value;
    } sub;
};

//
// Structure assignment tests
//
int main(int argc, char *argv[]) {
    struct mystruct s,
                    t,
                    *p;

    p = (struct mystruct *) malloc(sizeof(struct mystruct));

    s.value = 12.0;

    s.sub.value = 4.0;
    s.next = NULL;

    t = s;
    *p = t;
    p[0] = t;
    s = p[0];

    p -> value = 15.0;
    p -> sub.value = 5.0;
    p -> next = &s;
    for (struct mystruct *q = p; q != NULL; q = q -> next) {
        printf("Looking at element (%f, %f)\n", q -> value, q -> sub.value);
    }

    printf("\n");

    (*p).value = 15.0;
    (*p).sub.value = 5.0;
    (*p).next = &s;
    for (struct mystruct *q = p; q != NULL; q = (*q).next) {
        printf("Looking at element (%f, %f)\n", (*q).value, (*q).sub.value);
    }

    printf("\n");

    p[0].value = 15.0;
    p[0].sub.value = 5.0;
    p[0].next = &s;
    for (struct mystruct *q = p; q != NULL; q = q[0].next) {
        printf("Looking at element (%f, %f)\n", q[0].value, q[0].sub.value);
    }

    free(p);

    return 0;
}
