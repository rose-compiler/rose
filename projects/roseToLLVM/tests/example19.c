#include <stdio.h>
#include <stdlib.h>

//
// Test pointer to functions
//
void f() { printf("Function f was called\n"); }
void g() { printf("Function g was called\n"); }
void h() { printf("Function h was called\n"); }

static void (*function_array[]) (void) = {NULL,
     f, // 1
     g, // 2
     h  // 3
};

struct node {
    int *p;
    void (*member_function) ();
} my_node = { NULL, f};

void (*global_function) () = f;
int main(int argc, char *argv[]) {
    void (*function) () = g;
    struct node my_other_node = { NULL, f};
    my_node.member_function = h;

    f();
    global_function();
    function();
    my_other_node.member_function();

    for (int i = 0; i < 4; i++) {
        if (function_array[i] == NULL)
            printf("No function at index %i\n", i);
        else {
            function_array[i]();
        }
    }
}
