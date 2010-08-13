struct model
{
    int a[1024];
};

void event0(model* m)
{
    // n is small, irregular. Saving positions and values needs
    // n * 2 integers space.
    // Restore needs n assignment.
    int index[10];
    for (int i = 0; i < 10; ++i)
        m->a[index[i]] = rand();
}

void event1(model* m)
{
    // n is large, index regular and continuous (or almost continuous). Saving range needs
    // 2 + n integers space.
    // Restore needs a memcpy which is fast.
    for (int i = 0; i < 100; ++i)
        m->a[i] = rand();
}

void event2(model* m)
{
    // n is large, index irregular. Saving the whole array needs n integer space.
    // Restore needs a memcpy which is fast.
    int index[200];
    for (int i = 0; i < 10; ++i)
        m->a[index[i]] = rand();
}

void event3(model* m)
{
    // n is large, index irregular. Saving the whole array needs n integer space.
    // Restore needs a memcpy which is fast.
    // Tuning???
    for (int i = 0; i < 1024; ++i)
        if (...)
            m->a[i] = rand();
}

