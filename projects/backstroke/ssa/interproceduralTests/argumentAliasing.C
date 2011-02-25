void bar(int& x, int* y)
{
	x = 3;
	*y = 3;
}

void foo()
{
	int w, z;
	//Fixme:
	bar(w, &z);
}
