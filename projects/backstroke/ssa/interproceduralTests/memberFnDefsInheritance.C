struct A
{
	int x;

	void modifyThis()
	{
		x = 3;
	}
};

struct B : public A
{
	void modifyParent()
	{
		modifyThis();
	}
};

void foo()
{
	B b;
	b.modifyParent();
}