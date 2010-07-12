#include <stdio.h>

struct InnerStruct {
    int i;
    char info[10];
};

struct MyStruct {
    char *head;
    struct InnerStruct inner;
    char *tail;
};


//
// Test structure aggregates
//
int main(int argc, char *argv[]) {
    struct MyStruct m = { "I have", { 2, "machines"}, "at home"};
    struct MyStruct *p = &m;

    printf("%s %i %s %s\n", m.head, m.inner.i, m.inner.info, m.tail);
    printf("%s %i %s %s\n", p -> head, p -> inner.i, p -> inner.info, p -> tail);

    struct InnerStruct *q = &m.inner;
    printf("q -> info = %s\n", q -> info);

    return 0;
}
