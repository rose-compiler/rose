
#include "TALC.hpp"

/*
 * Function Parameter Test
 */

void copy(int *arr1, int *arr2)
{
	for(int i=0; i<10; i++)
	{
		arr2[i] = arr1[i];
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
