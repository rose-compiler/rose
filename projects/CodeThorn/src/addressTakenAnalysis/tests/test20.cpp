int foo()
{
    int a, b, c;
//    a = b + c;
//    a = b = 10;
//    a = b + (c = 10);
    a = (b = 10) && c + 1;
    return a + b + c;
}
