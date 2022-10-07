// test defuse querying

int main()
{
    int a = 0, b = 1, c;
    c = b + (a = 2);
    return a+c;
}
