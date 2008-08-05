int f(int n, float f, unsigned int u)
{
    return -n  // OK: int is signed
         + -f  // OK: float is signed
         + -u; // not OK: unary minus on unsigned expression
}

unsigned int f_noncompliant(unsigned int u)
{
    return -u;
}

int f_compliant(int n)
{
    return -n;
}
