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
void EVENT_break_inside_switch()
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
void EVENT_jump_structures()
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
void EVENT_exception_handling()
{
	try
	{
		EVENT_foobar_UsesArrays();
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

//Should fail. Accesing pointers as arrays not allowed
void EVENT_accessing_arrays(int* state)
{
	state[5] = 3;
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
void EVENT_virtual_function_call(A* obj)
{
	obj->foo();
}


struct State
{
	int x;
};

//Should pass. Writing to the value pointed to by a pointer is allowed.
void EVENT_modify_state_through_pointer(State* state)
{
	state->x = 3;
}

//Should fail. Modification of pointers not allowed.
void EVENT_modify_pointer_value(State* state)
{
	state++;
}

//Should fail. Dynamic memory allocation not allowed.
void EVENT_dynamic_memory_allocation()
{
	EVENT_modify_state_through_pointer(new State);
}

void EVENT_varargs(int i, ...)
{
	
}

struct GoodStruct
{
	int x, y;
};

struct BadStruct
{
	GoodStruct s;
};

//Should pass. Structs only containg primitive types are allowed.
void EVENT_uses_structs(GoodStruct* state)
{
	GoodStruct copy = *state;
	copy.x = 3;
}

//Should fail;
void EVENT_uses_complex_structs(GoodStruct* state)
{
	BadStruct s;
	s.s = *state;
}

//Should fail; early return statement
int EVENT_early_return(int i)
{
	if (i == 0)
		return;

	return i++;
}

int main()
{
	return 0;
}
