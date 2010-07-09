typedef int (*func)(int);

int foo(int a, int b)
{
	return a + b;
}

int bar(int b)
{
	return b * 2;
}

func getFunc(int a)
{
	return &bar;
}

class MyObject
{
public:
	MyObject(int a)
	{
		x = a;
	}
	
	int x;
};

int main()
{
	return 0;
}

//These are test cases that are not supported, but this is well known and this code should produce an error message
void failingOnPurpose()
{
	//We're calling the constructor, which is a function for all intents and purposes. However, its arguments are not hoisted.
	MyObject obj(bar(3));

	//Calling a function through a function pointer, when the function expression itself needs to be normalized.
	//bar(3) would be hoisted before bar(5), even though it should be the other way around.
	getFunc(bar(5))(bar(3));
}
