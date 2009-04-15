struct pair
{
    int a;
    int b;
};

int run()
{
    struct pair p;
    struct pair q;

    p.a = 42;
    p.b = 23;

    q.a = 0;
    q.b = 1;

#if 0
    /* advanced stuff */
    p.a = q.b;

#if 0
    /* even more advanced stuff */
    q = p;
#endif
#endif
}
