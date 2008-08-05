double f(int n, double d) {
    n -= 0.0 + 1.0 * 42.0;
    int a = 0.1 * n + 0.2 * d; // OK: within an initializer
    a -= -0.333 * -4;   // the constants are 0.333 and 4 (not signed)!
    int b = 5000;              // OK: within an initializer
    double c;
    c = 60;
    a = 7 * b - 8 / c + 9999;
 
    return d / 2;
}

int f_noncompliant(int n)
{
    int x;
    x = 42; // not OK: magic number
    return x + n;
}

int f_compliant(int n)
{
    int x = 42; // OK: constant only used in initializer
    return x + n;
}
