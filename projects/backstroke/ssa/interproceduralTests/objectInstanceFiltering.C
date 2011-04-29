struct A
{
	int x;

	void bar()
	{
		x = 3;
	}

	//Foo ends up with a def for x even though it clearly shouldn't have one
	void foo() const
	{
		A a;
		a.bar();
	}
};
