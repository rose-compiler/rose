int global;

void function1(void);
void function2(void);

void f(void)
{
    int y, z;

    if (y)
        z = 0;
    else
        z = 2;
    y = z;
    
    function1();
}

void function1(void)
{
    global = 23;
}

void function2(void)
{
    global = 42;
}

