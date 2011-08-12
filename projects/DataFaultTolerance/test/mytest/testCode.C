

int main()
   {
	int i,j;
	int A[100][100],B[100][100],C[100];

	#pragma mem_fault_tolerance
	for(i=0; i<100; i++)
	{
		for(j=0;j<100;j++)
		   A[i][5] = 0;
		B[i][5] = 5;
	}

	#pragma mem_fault
	for(i=0; i<100; i++)
	{
		A[i][5]=B[i][5]+C[i];
	}

     return 0;
   }
