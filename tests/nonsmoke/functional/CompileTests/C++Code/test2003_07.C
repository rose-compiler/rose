/*
Example program demonstrating failure of EDG parser to handle CR instead of newline:

I have managed to create (totally by accident) a test source file that
contains CR instead of NL.
When I run it through ROSE, I get an EDG parser error (g++ has no
trouble compiling it). I have put the file in my home directory on CASC
machines. It's called cr.c.
--Beata
*/

int main()
{
	int i=3;
	
	if(i==0)
	{
		return 0;
	}
	else if(i>0)
	{
		return 1;
	}
	else
	{
		return 2;
	}
	return 0;
}
