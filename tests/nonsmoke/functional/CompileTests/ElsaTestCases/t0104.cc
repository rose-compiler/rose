// ambiguity error message
// -*-c++-*-

typedef void *__gnuc_va_list;
typedef __gnuc_va_list va_list;
union my_error_arguments {
    int i;
    char *s;
};
void *foo(int);
#pragma boxvararg("my_error",sizeof(union my_error_arguments))
#pragma boxvararg("__my_error",sizeof(union my_error_arguments))
void my_error(int severity, ...) {
    va_list ap;
    ( ap  = ((__gnuc_va_list) /*__builtin_next_arg*/foo (  severity ))) ;
}
