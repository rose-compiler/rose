
// example program using __builtin_va_* functions

// ERR-MATCH: __builtin_va

int printf(const char*, ...);

int foo(int count, ...) {
    __builtin_va_list v;

    __builtin_va_start(v, count);

    while (count--) {
        const char* p = __builtin_va_arg(v, const char*);
        printf("%s ", p);
    }

    __builtin_va_list v2;
    __builtin_va_copy(v2, v);                       // C99 has va_copy

    __builtin_va_end(v2);
}

int main()
{
    foo(4, "a", "b", "c", "d");
}
