//The correct processing order here should be b, a1, a2, main or b, a2, a1, main

void b()
{
	
}

void a1()
{
	b();
}

void a2()
{
	b();
}

int main()
{
	a1();
	a2();
}

