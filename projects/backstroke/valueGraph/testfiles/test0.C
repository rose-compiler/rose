void foo(int& a)
{
    bool c1, c2, c3;
    if (c1)
    {
        ++a;
        if (c2)
        {
            ++a;
            return;
        }
        else
        {
            //return;
L:
            ++a;
            return;
        }
    }
    else
    {
        a = 0;
        if (c3)
            goto L;
        else
            return;
    }
}
