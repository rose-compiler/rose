//The correct order to process functions here would be a,b,main or b,a, main

void b();

void a()
{
	b();
}

void b()
{
	a();
}

int main()
{
	a();
}
