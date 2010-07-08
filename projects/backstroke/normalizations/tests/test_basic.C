int addone(int a)
{
	return a + 1;
}

int foo(int a)
{
	return a << 1 + 1;
}

int bar(int a)
{
	return a;
}

int globalFunc(int a, int b)
{
	return a + b;
}

typedef int (*func)(int);
func getFunc(int a)
{
	return &bar;
}

int main(int argc, char** argv)
{
	globalFunc(addone(2), foo(3));

	//Comma op. Evaluation order should be foo(2), bar(3), globalFunc(_,3)
	int x = (foo(2), globalFunc(bar(3), 3));

	//Calling a function through a function pointer
	func f = getFunc(bar(5));
	f(bar(3));

	return 0;
}
