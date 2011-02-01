int& foo();

void bar()
{
	foo() = 3;
	foo()++;
}
