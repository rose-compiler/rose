// Snippets that have parameterized types

void swap(char a, char b)
{
    // 'char' will be replaced with the type of the actual argument
    // bound to 'a'. This only works for typedefs.
    typedef char typeof_a;
    typeof_a tmp;

    tmp = a;
    a = b;
    b = tmp;
}
