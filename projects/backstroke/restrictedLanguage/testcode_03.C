//Should fail
int EVENT_foobar_UsesPointers(int x)
{
	int* y = &x;
	return x;
}

//Should fail
int EVENT_foobar_UsesArrays()
{
	char buffer[5];
	return 0;
}

//Should fail
int EVENT_args_by_reference(int& x)
{
	x = 7;
	return 3;
}

//Should pass. This is allowed
int EVENT_break_inside_switch()
{
	switch (3)
	{
		case 1:
			break;
		case 2:
			break;
	}
}

//Should fail. No break allowed inside loops
int EVENT_jump_structures()
{
	switch (1)
	{
		case 1:
			while (true)
			{
				break;
			}
	}
}

//Should fail
int EVENT_exception_handling()
{
	try
	{
		return EVENT_foobar_UsesArrays();
	}
	catch (...)
	{
		
	}
}

//Should fail. Function pointers not allowed
int EVENT_function_pointer(int (*hello)(int))
{
	return hello(3);
}

class A
{
public:
	virtual int foo()
	{
		int x = 3;
	}
};

class B : public A
{
public:
	virtual int foo()
	{
		int x = 5;
	}
};

//Should fail. Virtual functions not allowed
int EVENT_virtual_function_call(A* obj)
{
	obj->foo();
}


int main()
{
	return 0;
}
