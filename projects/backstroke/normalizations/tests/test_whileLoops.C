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
	int x = 3;

	//While loop with test condition that needs to be normalized
	while (globalFunc(foo(3), addone(x)))
	{

	}

	//While loop with declaration in the test condition
	while (int i = globalFunc(foo(3), addone(x)))
		break;

	do
	{
		globalFunc(addone(2), foo(3));
	} while (globalFunc(foo(3), addone(x)));


	return 0;
}
