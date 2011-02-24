int globalVar;

struct A
{
	int x;

	A()
	{
		globalVar = 3;
		x = 3;
	}

	foo()
	{
		A a;
	}
};

void maine()
{
	A a;
	a.foo();
}


