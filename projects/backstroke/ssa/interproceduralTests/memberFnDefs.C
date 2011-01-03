struct A
{
	int x;

	//This function has no definition and is not declared const, so it should count as a def
	void fooNonConst();

	//This function has no definition, but is declared const, so it should not be a def
	void fooConst() const;

	void modify()
	{
		x = 3;
	}

	void implicitModify()
	{
		modify();
	}

};

void bar()
{
	A a;

	a.fooConst();

	a.fooNonConst();

	a.implicitModify();
}
