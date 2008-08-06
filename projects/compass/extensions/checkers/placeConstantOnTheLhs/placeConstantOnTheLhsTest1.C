
//Your test file code goes here.
void foo()
{
	int a = 0;

	if(a == 10) // a is on the LHS
	{
		a = 1;
	}
	
	while(a == 10) // a is on the LHS
	{
		a++;
	}

	do
	{
		a++;
	}while(a == 12); // a is on the LHS

	for(int i = 0; i == 0; i++) // i is on the LHS
	{
		a = 12;
	}

	if(1 == a) // fine
	{
		a = 2;
	}
}
