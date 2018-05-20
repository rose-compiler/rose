// generated code

AD_real  h  = PI / (AD_real)N;
AD_real  t1 = 0.0, t2, t3;
AD_real  s1 = 0.0;
AD_real d1 = 1.0, d2;

AD_real fun (AD_real x)
{
    d2 = d1;    // also d1 in original
    AD_INTERMEDIATE(d2, "d2");
    t3 = x;     // also t1 in original
    AD_INTERMEDIATE(t3, "t3");

    int k;
    for (k = 1; k <= 5; k+=1)
    {
        d2 = 2.0 * d2;
        AD_INTERMEDIATE(d2, "d2");
        t3 = t3 + sin (d2 * x) / d2;
        AD_INTERMEDIATE(t3, "t3");
    }
    return t3;
}

void do_fun ()
{
    int i;
    for (i = 1; i <= N; i+=1)
    {
        t2 = fun (i * h);
        AD_INTERMEDIATE(t2, "t2");
        s1 = s1 + sqrt (h * h + (t2 - t1) * (t2 - t1));
        AD_INTERMEDIATE(s1, "s1");
        t1 = t2;
        AD_INTERMEDIATE(t1, "t1");
    }
}

