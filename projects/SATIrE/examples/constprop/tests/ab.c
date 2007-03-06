void a(void), b(void);

void global_init(void)
{
    int x = 23, y = 23;

    a();
    b();
}

void a(void)
{
    int x = 7;
}

void b(void)
{
    int y = 42;
}
