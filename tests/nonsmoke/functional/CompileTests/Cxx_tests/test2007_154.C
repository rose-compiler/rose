/*
Hi Dan,

Here's another ROSE bug report (I know you love getting them so much  :-)  ):

I'm currently using rose-0.8.10a, and it seems to be an issue with
derefencing a va_list structure, specifically in va_arg (the code is
legal when compiled with gcc):

stonea@vega ~/tmp
$ cat test.c
#include <stdio.h>
#include <stdarg.h>

void bar(va_list *args) {
    int *val;

    val = va_arg(*args, int*);

    printf("val = %d\n", *val);

}


void foo(int n, ...) {
    va_list args;

    va_start(args, n);

    bar(&args);

    va_end(args);
}


int main(char argc, char *argv[]) {
    int val = 123;

    foo(1, &val);
}


stonea@vega ~/tmp
$ gcc test.c

stonea@vega ~/tmp
$ rosec test.c
rosec: /s/bach/e/proj/oadev/ROSE/rose-0.8.10a/ROSE/src/frontend/EDG_SAGE_Connection/sage_gen_be.C:19530:
SgExpression* sage_gen_expr(an_expr_node*, a_boolean,
DataRequiredForComputationOfSourcePostionInformation*): Assertion
`operand_1->kind == (an_expr_node_kind)enk_variable_address' failed.
Aborted

Thanks,
-Andy Stone
*/


#include <stdio.h>
#include <stdarg.h>

void bar(va_list *args) {
    int *val;
#if 1
    val = va_arg(*args, int*);
#endif
    printf("val = %d\n", *val);

}

#if 0
void foo(int n, ...) {
    va_list args;

    va_start(args, n);

    bar(&args);

    va_end(args);
}
#endif
#if 0
int main(char argc, char *argv[]) {
    int val = 123;

    foo(1, &val);
}
#endif

