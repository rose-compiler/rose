int deref(int ***p, int **q)
{
    return **p[**q];
}
