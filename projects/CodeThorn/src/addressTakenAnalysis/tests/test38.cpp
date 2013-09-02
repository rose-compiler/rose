#include <cstdarg>

double foo(int a[], ...)
{
    va_list args_list, args_list_copy;
    va_start(args_list, a);
    double v = va_arg(args_list, double);
    va_end(args_list);    
    return v;
}

int bar(int a[], ...)
{
    int _v;
    va_list args_list;
    va_start(args_list, a);
    _v = va_arg(args_list, int);
    va_end(args_list);
    return _v;    
}

int foobar(int a, ...)
{
    int* _vp, _v;
    va_list args_list, args_list_copy;
    va_start(args_list, a);
    _vp = va_arg(args_list, int*);
    va_copy(args_list_copy, args_list);
    _v = va_arg(args_list_copy, int);
    va_end(args_list);
    return *_vp;
}

int main()
{
    double f = 10.0;
    int a[5] = {1, 2, 3, 4, 5};
    foo(a, f);
    int val = 7;
    bar(a, val);
    return foobar(val, a, val); 
}
