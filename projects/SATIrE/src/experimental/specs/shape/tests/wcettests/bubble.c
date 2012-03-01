
#ifndef LANG_WCET
#define LANG_WCET WCETCC
#endif
#include <wcet.h>

#define N_EL 10

void wcet_buildinfo(char* c) {}

/* Sort an array of 10 elements with bubble-sort */
void bubble (int arr[])
{
	/* Definition of local variables */
	int i, j, temp;

	wcet_buildinfo("Test version");
	/* Main body */
        #pragma	WCET_SCOPE( BS )
	{
		#pragma	WCET_LOOP_BOUND (N_EL - 1)
		for (i=N_EL-1; 
			i > 0; 
			i--) 
		{
			#pragma	WCET_LOOP_BOUND (N_EL - 1)
			for (j = 0; 
				j < i; 
				j++) 
			{
				#pragma WCET_BLOCK_BEGIN("AB",-1);
				#pragma WCET_MARKER (M);
				if (arr[j] > arr[j+1])
				{
					temp = arr[j];
					arr[j] = arr[j+1];
					arr[j+1] = temp;
				}
				#pragma WCET_BLOCK_END("AB");
			}
		}
		#pragma WCET_RESTRICTION (M <= (N_EL*(N_EL-1)/2) );
	}
}

int main() { return 0; }
