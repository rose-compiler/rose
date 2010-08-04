#include <stdio.h>
#include <stdlib.h>

//
// Boolean tests
//
#include <stdio.h>

int f() {
  printf("side-effect\n");
  return 0;
}

struct InnerStruct {
    int i;
    char info[10];
};

struct MyStruct {
    char *head;
    struct InnerStruct inner;
    char *tail;
};

int main(int argc, char *argv[]) {
    int i = 0,
        j = 1,
        k;

    struct MyStruct m = { "I have", { 2, "machines"}, "at home"};
    struct MyStruct *p = &m;

    double d = (i > j ? 0.2 : 0.5);
    printf ("d = %f\n", d);

    char *s = (i > j ? m.head : m.tail);
    printf ("s = %s\n", s);
 
    k = (i < j ? 0 : j);
    printf ("k = %i\n", k);

    k = (i > j ? i : f());
    printf ("k = %i\n", k);

    k = (i <= j ? i == j ? i : i + 1 : j);
    printf ("k = %i\n", k);

    k =  (i < j) && (i + 1 == j);
    printf ("k = %i\n", k);

    k =  (i < j) || (k == i);
    printf ("k = %i\n", k);


    printf("%s %i %s %s\n", m.head, m.inner.i, m.inner.info, m.tail);
    printf("%s %i %s %s\n", p -> head, p -> inner.i, p -> inner.info, p -> tail);

    struct InnerStruct *q = &m.inner;
    printf("q -> info = %s\n", q -> info);

    int a = 0;
 
    printf("a = %i\n", a);
    a = !a;
    printf("!a = %i\n", a);

    return 0;
}
