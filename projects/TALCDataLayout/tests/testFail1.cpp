
#include "TALC.hpp"

/*
 * Function Parameter Test
 */

void copy(int *x, int *y)
{
	for(int i=0; i<10; i++)
	{
		y[i] = x[i];
	}
}

int main()
{
        int *arr1 = new int[10];
        int *arr2 = new int[10];

        for(int i=0; i<10; i++)
        {
        	arr1[i] = 0;
        }

        copy(arr1, arr2);
}
