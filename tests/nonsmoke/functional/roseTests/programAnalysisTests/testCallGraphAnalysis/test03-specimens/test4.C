class A 
{
  public:
        // return a pointer to a function returning a bool
	bool (A::*f1(char c))(int)
	{
		return &A::f2;
	}

	bool f2 (int i);
	bool f3 (int);
	bool f5 (int) {}

	void f4()
	{
		bool b;
		char c;
		int i;
		(this->*f1(c))(2);
	}
};

bool A::f2(int i) { return f3(i); }
bool A::f3(int i) { return 1; }

int main()
{
	bool (A::*(A::*ff)(char))(int);
	bool (A::*pf)(int);
	ff = &A::f1;
	pf = &A::f2;
	A a;
	A *ap;
	(ap->*pf)(1);
	(ap->*(a.*ff)('a'))(2);
}
