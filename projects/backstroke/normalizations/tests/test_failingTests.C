typedef int (*func)(int);

int foo(int a, int b)
{
	return a + b;
}

int bar(int b)
{
	return b * 2;
}

func getFunc()
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
	//If a variable declared in the init expression is accessed in the test expression,
	//it is not hoisted correctly (fails to compile)
	for (int i = 12; foo(bar(i), 3); )
	{
		
	}

	//Bar(3) is hoisted before this statement, and hence it is called before foo(2,3). It should be called beforehand
	int x = (foo(2, 3), foo(bar(3), 3));

	//We're calling the constructor, which is a function for all intents and purposes. However, its arguments are not hoisted.
	MyObject obj(bar(3));

	return 0;
}

//These are test cases that are not supported, but this is well known and this code should produce an error message
void failingOnPurpose()
{
	//Not supported because we would have to hoist getFunc() before we can hoist bar(3)
	//getFunc()(bar(3));
}
