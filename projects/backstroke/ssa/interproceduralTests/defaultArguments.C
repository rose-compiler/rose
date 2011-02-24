int globalVar;

struct A
{
	int y;

	A(int i)
	{
		y = i;
	}
} aObject(3);

void bar(int& x = globalVar, A* a = &aObject)
{
	x = 3;
	a->y = 3;
}

void foo()
{
	//There should be definitions for both aObject and globalVar here
	bar();
}