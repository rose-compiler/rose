//Phi function joins three nodes
//The control dependence of two of the defs is nested.
//The other def dominates phi
int foo()
{
	int x = 1;
	if (x > 0)
	{
		if (x < 10)
			x = 2;
		else
			x = 3;
	}

	return x;
}


