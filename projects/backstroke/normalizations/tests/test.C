class AClass
{
public:
	
	int& foo(int& x)
	{
		return x;
	}

	int bar(int x)
	{
		return x * 2;
	}
};

int globalFunc(int a, int b)
{
	return a + b;
}

int main(int argc, char** argv)
{
	int a = 3;
	int& b = a;

	AClass aObject;

	//globalFunc(aObject.bar(a * 2) - 1, aObject.foo(a));


	//Function calls in both the initializer and the test,
	//with declaration in the initializer
	for(int i = globalFunc(a | b, a & b); aObject.bar(aObject.bar(a)) == 0; )
	{
		break;
	}

	//Function calls in both the initializer and the test,
	for(globalFunc(a | b, a & b); aObject.bar(aObject.bar(a)) == 0; )
	{
		break;
	}

	//Function calls in both the initializer and the test,
	//with multiple declaration in the initializer
	for(int i = globalFunc(a | b, a & b), j; aObject.bar(aObject.bar(a)) == 0; )
		break;

	//Function in for-loop increment expression
	for (;; globalFunc(aObject.bar(a), b))
	{
		break;
	}

	//Test with multiple variable declarations in the initializer, each of which needs to be rewritten
	/*for( int z = (aObject.bar(3), aObject.bar(3)), k = 3; ;)
	{
		break;
	}*/

	return 0;
}
