
int main()
   {
	int i;
	int A[100],B[100][100],C[100];

	#pragma mem_fault_tolerance (Chunksize=8)
	for(i=0; i<10; i++)
	{
		A[i] = 0;
	}

	#pragma mem_fault_tolerance (A:<0:100>) (B:<0:100><0:100>) (C:<0:100>) (Chunksize=8)
	for(i=0; i<10; i++)
	{
		B[i+1][i+5]=A[i];
		C[i] = B[i][i+2];
	}

     return 0;
   }
