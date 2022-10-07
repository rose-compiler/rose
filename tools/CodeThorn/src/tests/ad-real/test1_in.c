long double  h  = PI / (long double)N;
long double  t1 = 0.0, t2, t3;
long double  s1 = 0.0;
long double d1 = 1.0, d2;

long double fun (long double x)
{
    d2 = d1;    // also d1 in original
    t3 = x;     // also t1 in original

    int k;
    for (k = 1; k <= 5; k+=1)
    {
        d2 = 2.0 * d2;
        t3 = t3 + sin (d2 * x) / d2;
    }
    return t3;
}

void do_fun ()
{
    int i;
    for (i = 1; i <= N; i+=1)
    {
        t2 = fun (i * h);
        s1 = s1 + sqrt (h * h + (t2 - t1) * (t2 - t1));
        t1 = t2;
    }
}
