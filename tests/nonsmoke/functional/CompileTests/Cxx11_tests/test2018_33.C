// typedef unsigned long fexcept_t;
typedef int fexcept_t;

// extern int fesetexceptflag (const fexcept_t *__flagp, int __excepts);
// int fesetexceptflag (const fexcept_t __flagp);
int fesetexceptflag (const fexcept_t __flagp);
