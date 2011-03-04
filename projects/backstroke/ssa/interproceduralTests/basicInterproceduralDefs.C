int globalInt;
char globalChar;

void a()
{
	int a = 3;
	globalInt = 4;
}

void b()
{
	globalChar = 'v';
	a();
}

void c()
{
	b();
	globalInt = 2;
}
