#include <stdio.h>
#include <stdlib.h>

//
// Test pointer to functions with parameter and return value
//
int f(int n) { printf("Function f was called with %i\n", n); return n * 2; }
int g(int n) { printf("Function g was called with %i\n", n); return n * 3; }
int h(int n) { printf("Function h was called with %i\n", n); return n * 4; }

static int (*function_array[]) (int) = {NULL,
     f, // 1
     g, // 2
     h  // 3
};

struct node {
    int *p;
    int (*member_function) (int);
} my_node = { NULL, f};

int (*global_function) (int) = f;
int main(int argc, char *argv[]) {
    int (*function) (int) = g;
    struct node my_other_node = { NULL, f};
    my_node.member_function = h;

    f(1);
    global_function(10);
    function(100);
    my_other_node.member_function(1000);

    for (int i = 0; i < 4; i++) {
        if (function_array[i] == NULL)
            printf("No function at index %i\n", i);
        else {
            int k = function_array[i](i);
            printf("Function %i returned  %i\n", i, k);
        }
    }
}
