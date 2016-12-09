// ./qt2-2.3.1-13/qstring-ht7v.i.cpp_out:/home/ballAruns/tmpfiles/./qt2-2.3.1-13/qstring-ht7v.i:5628:8: error: no viable candidate for function call

typedef __builtin_va_list __gnuc_va_list;
typedef __gnuc_va_list va_list;

void sprintf( const char* cformat, ... )
{
    va_list ap;
    __builtin_stdarg_start((ap),cformat);
    int width = __builtin_va_arg( ap, int );
    __builtin_va_arg(ap, char*);
    int* n = __builtin_va_arg(ap, int*);
    double value = __builtin_va_arg(ap, double);
    void* value2 = __builtin_va_arg(ap, void*);
    __builtin_va_end( ap );
}
