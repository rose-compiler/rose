//**REACHABLE ERROR NUMBERS: 3; ALL OTHERS UNREACHABLE

#include<stdio.h>
#include <stdbool.h>
#include <assert.h>

extern void __VERIFIER_error(int);

void staticTwoBranches()
{
	int y = 8 + ((9 - 10) * 4 / 3);
	int x = y;
	int b = x + ((y - 10) * x / y);
	if(b < x && x != y)
	{	
		if(x <= 0 || x > 0)
			x = x + y;
		if(b >= y)
		{
			x = 5;	
			x = 11;
			y = x;
			if(x == y)
			{
				x = x + 1;	
			}
			else
			{
				y = y + 1;
				if(x == y)
				{
					x = x + 1;
					__VERIFIER_error(1);			
				}
				else 
				{
					y = y + 1;
				}
			}
	
		}
		else if(b > x)
		{
			x = x + y;	
			x = 13;
			int test = 1;
			test = x;
			if(test == 1)
			{
				test = 4;
			}	
			else 
			{
				test = 5;
				__VERIFIER_error(2);
			}
			y = x;	
		}
		else
		{
			y = y + x;
			x = 7;	
		}
	}
	else 
	{
		int q = 3;
		q = q + 8;
		if(q == 4)
		{
		}
		else
		{
			x = q;
			__VERIFIER_error(3);
		}
		x = 5; 
	}


	int z = 100;
}

