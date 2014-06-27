bool func(bool val)
{
    return val;
}

int main()
{
    int a, b;
    return func( (a=0) && (b=1) );
}
