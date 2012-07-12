

int main()
{

	int A[100][100], B[100][100], i, j, temp;

	#pragma mem_fault_tolerance (A:<0:100><0:100>) (B:<0:100><0:100>) (Chunksize=8)
	for(i=0; i<100; i++)
	{
		for(j=0; j<100; j++)
		{
		  A[i][j] = i+j;
		  B[i][j] = 0;
		}
	}
	
	#pragma mem_fault_tolerance (A:<0:100><0:100>) (B:<0:100><0:100>) (Chunksize=8)
	for(i=0; i<100; i++)
	{
		for(j=0; j<100; j++)
		  B[i][j] = A[j][i];
	}
}
