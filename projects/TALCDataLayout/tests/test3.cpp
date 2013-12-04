
#include "TALC.hpp"

/*
 * Offset Test
 */

void set(int *arr2)
{
	for(int i=0; i<10; i++)
	{
		arr2[i] = i+10;
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

        set(arr2);
}
