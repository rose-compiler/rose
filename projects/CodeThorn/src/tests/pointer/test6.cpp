int add(int first, int second)
{
    return first + second;
}
 
int subtract(int first, int second)
{
    return first - second;
}
 
int operation(int first, int second, int (*functocall)(int, int))
{
    return (*functocall)(first, second);
}
 
int main()
{
    int  a, b;
    int  (*plus)(int, int) = add;
    a = operation(7, 5, plus);
    b = operation(20, a, subtract);
    return 0;
}
