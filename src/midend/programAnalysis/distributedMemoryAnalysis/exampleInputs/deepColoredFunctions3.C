void f()
{
    {
        int n = 42;
        {
            {
            }
            return;
        }
    }
}

int g(int n)
{
    return n + (1 + 1) + 1 + 1 + (1 + 1 + 1) + 1 + 1;
}
