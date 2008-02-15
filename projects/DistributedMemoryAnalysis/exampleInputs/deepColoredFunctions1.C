int fib(int n)
{
    return (n <= 0 ? 0
         : (n == 1 ? 1
                   : fib(n-1) + fib(n-1)));
}

int *xform(int *list, unsigned int n, int (*f)(int))
{
    return (n == 0 ? list
                   : (*list = f(*list), xform(list+1, n-1, f)));
}
