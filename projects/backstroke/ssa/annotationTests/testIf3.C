//The control dependence of one of the defs is nested.
//The other def dominates phi
int foo()
{
	int x;
	if (x > 0)
	{
		if (x < 10)
			x = 3;
	}

	return x;
}

