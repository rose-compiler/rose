int add(int a = 23, int b = 42)
{
    return a + b;
}

int main()
{
    int x = add();
    int y = add(10);
    int z = add(20, 40);

    return x + y + z;
}
