int addone(int a)
{
	return a + 1;
}

int foo(int a)
{
	return a << 1 + 1;
}

int globalFunc(int a, int b)
{
	return a + b;
}

int main(int argc, char** argv)
{
	globalFunc(addone(2), foo(3));

	return 0;
}
