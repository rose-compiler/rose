int fac()
{
    int n;
    int result;
    int *np;
    
    np = &n;

 // this assignment kills n; i.e., n is dead before the assignment, but it
 // is live after the assignment because the new value is used in the loop
    *np = 42;

    result = 1;
    while (*np < 0)
    {
        result = result * *np;
        (*np)--;
    }
}
