
//Your test file code goes here.
void foo( double f )
{
	if ( f == (float)3)
	{
		f = 1.234;
	}

	while(f != 1.23456)
	{
		f += 0.00001;
	}

	do
	{
		f += 0.000001;
	} while ( f != 1.234567);

	for(f = 1.234567; f != 1.2345678; f += 0.0000001)
	{
		int i = f + 1;
	}
}
