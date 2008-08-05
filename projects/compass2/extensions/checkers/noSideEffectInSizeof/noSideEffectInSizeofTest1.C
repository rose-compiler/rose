
//Your test file code goes here.

int bar();

void foo()
{
	int a = 14;
	int b = sizeof(a++);
	int c;

	a = sizeof(b--);
	//a = sizeof(--b);

	//c = sizeof((b=a+1));
	//int a = sizeof((b=bar()));
}

