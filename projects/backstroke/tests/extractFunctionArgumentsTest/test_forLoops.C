class AClass
{
public:
	AClass()
	{
	}

	AClass(int x, int y)
	{
	}
	
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

	//Constructor initialization inside the for loop
	for (AClass localA(3, 3); aObject.bar(aObject.foo(a)); )
		break;

	//Aggregate initializer inside the for loop
	for (int hello[] = {aObject.bar(a), 3}; aObject.bar(aObject.foo(a)); )
	{
		hello[0] = 7;
		break;
	}

	//More complex aggregate initializer inside the for loop
	for (int hello[][2] = { {1,aObject.bar(aObject.foo(a))}, {1,2}}; aObject.bar(aObject.foo(a)); )
	{
		hello[0][0] = 7;
		break;
	}

	//Moving the increment expression
	for (; ; aObject.bar(aObject.foo(a)))
	{
		globalFunc(1, 2);
	}

	//A variable declared in the init expression is accessed in the test expression,
	for (int i = 12; globalFunc(aObject.bar(i), 3); )
	{

	}

	return 0;
}
