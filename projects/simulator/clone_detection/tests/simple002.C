extern int gvar_e01, gvar_e02;
static int gvar_s01, gvar_s02;

namespace NAMESPACE {

// File-wide interprocedural optimizations can replace static variables with their initial value if the static variable is
// never written.  Therefore, this function is only here to defeat that optimization. If you comment it out, then some of the
// functions that reference the static global will be placed in a different similarity bucket.
void defeat_static_global_optimizations(int x, int y)
{
    gvar_s01 = x;
    gvar_s02 = y;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Group of functions that all return the value 5
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int five_01()
{
    return 5;
}

int five_02()
{
    return 2 + 3;
}

int five_03()
{
    int i = 5;
    return i;
}

int five_04()
{
    int i = 2;
    return i + 3;
}

int five_05()
{
    int i = 2;
    int j = 3;
    return i + j;
}

int five_06()
{
    int i = 5;
    int j = i;
    return j;
}

int five_07()
{
    int i; // uninitialized
    int j = 5 + i;
    return j - i;
}

int five_08(int x)
{
    return 5;
}

int five_09(int x, int y)
{
    return 5;
}

int five_10(int x)
{
    int i __attribute__((unused)) = x;
    return 5;
}

int five_11(int x)
{
    int i = 5 + x;
    return i - x;
}

int five_12(int x)
{
    // We can't tell in assembly by looking only at this function whether
    // the caller reads 'x' after this function returns; therefore, 'x'
    // must be an output.
    x = 5;
    return x;
}

int five_13(int &x)
{
    return 5;
}

int five_14(int &x)
{
    int i __attribute__((unused)) = x;
    return 5;
}

int five_15(int &x)
{
    int i = 5 + x;
    return i - x;
}

int five_16(int *x)
{
    return 5;
}

int five_17(int *x)
{
    int *i __attribute__((unused)) = x;
    return 5;
}

int five_18(int *x)
{
    int i __attribute__((unused)) = *x;
    return 5;
}

int five_19(int *x)
{
    int i = 5 + *x;
    return i - *x;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Group of functions that all return the value of some global variable without changing the global variable (that would be
// considered another output).  Even though they use different variables, they should be lumped together because they're
// semantically similar: they grab some value from a global variable and return it.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int global_03()
{
    return gvar_e01;
}

int global_04()
{
    return gvar_e02;
}

int global_05()
{
    return gvar_s01;
}

int global_06()
{
    return gvar_s02;
}

int global_07(int x)
{
    return gvar_e01;
}

int global_08()
{
    int x = gvar_s01;
    return x;
}

int global_09()
{
    int x = gvar_s01;
    int y __attribute__((unused)) = gvar_s02; // consumes an extra input, but no output depends on it
    return x;
}



} // namespace
