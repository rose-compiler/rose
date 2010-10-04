struct State
{
	int x;
	int y;
};

int foo()
{
	return 3;
}

void reverseMe(int in, State* state)
{




#if 1
	int g = 3;
	int u;
	u = foo();
	g += (u ^ 12) + 7;
	

	int l;
	l = foo();
	l += 3;

	//Extact-from use on assign ops
	int x, y;
	x = foo();
	y = x;		//x has to be recursively restored
	x ^= 12;
	y = 3;		//Destroy y. It will be extracted from x


	//Redefine techniuqe valid on floats
	float k = 3.3;
	float h = k * 2;
	k++; //Good example of why we need to apply agkul-style to ++/--
	h = 7;

	//Extract-from-use using + and - semantics
	x = in + 3;
	in = foo();
	x = in - 4;
	in = foo();

	//Test XOR
	x = in ^ 4;
	in = foo();

	//Extract-from use when combining XOR and addition
	x = 7 + (in ^ 4);
	in = foo();

	in;

	int i = in;
	int j = ++in;

	//Testing reaching definitions for plus plus op
	int r = 2;
	r++;
	r = 3;

	//Testing reexecuting the definition of non-standard assignments
	int b = 7;
	b++;
	int a = b * 3;
	a *= 2 & b;
#endif
}


