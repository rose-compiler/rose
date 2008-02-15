int f_noncompliant(int n)
{
    return (n++, n++, n); // not OK (twice): comma operator
}

int f_compliant(int n)
{
    n++;
    n++;
    return n;
}

class A;

void operator,(const A &a, const A &b); // not OK: comma operator decl

void g(const A &a)
{
    a, a; // the use of the overloaded operator is not flagged
}
