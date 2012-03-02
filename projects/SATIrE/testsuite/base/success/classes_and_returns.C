class A
{
public:
    ~A() {}
};

int f(void)
{
    int n = 0;
    A a_in_f;

    while (n < 10)
    {
        A a_in_while;
        if (n % 3 == 0)
        {
            A a_in_while_in_if;
            return n;
        }
        n++;
    }
    return n;
}
