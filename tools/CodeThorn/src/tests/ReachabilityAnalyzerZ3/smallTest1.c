//**REACHABLE ERROR NUMBERS: 1, 4, 6; ALL OTHERS UNREACHABLE

#include<stdio.h>
#include <stdbool.h>
#include <assert.h>

extern void __VERIFIER_error(int);

void staticTwoBranches()
{
	int x = 12345;
	int y;
	scanf("%d",&y);
	__VERIFIER_error(1);
	int z;
	if(x < 0 && x != -8)
	{	
		y = x;
		z = 4;
		scanf("%d",&z);
		__VERIFIER_error(2);
	}
	else if(x == y)
	{
		z = 8;
		z = z + 1;
		__VERIFIER_error(3);
		int z = 12;
		x = z;	
		z = z + 1;
	}
	else 
	{
		__VERIFIER_error(4);
		y = 5;
		int y = x * x / 2;	
		y = 8;
		if(y == -8 || y == x * y - 124)
		{
			scanf("%d",&x);
			__VERIFIER_error(5);
			z = 8;
			y = z;
		}	
		else
		{
			__VERIFIER_error(6);	
		}
		y = 8;
	}
	x = y;
	x = x + 1;		
}





















