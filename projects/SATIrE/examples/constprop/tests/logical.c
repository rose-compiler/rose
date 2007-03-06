int f()
{
    int x = 7, y = 3, z = 42;

    if (x < y && y > z || x == z)
        z = 23;

    x = (x, y, z);

    return z;
}
