int a, b, c;

//Here, we have recursion. So every function call site should define a, b, and c

void B();
void C();

void A()
{
	a = 1;
	B();
}

void B()
{
	b = 1;
	C();
}

void C()
{
	c = 1;
	A();
}
