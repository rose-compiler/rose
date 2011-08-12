

int main()
{

	int A[100][100], B[100][100], i, j;

	#pragma mem_fault_tolerance
	for(i=0; i<100; i++)
	{
		for(j=0; j<100; j++)
		{
		  A[i][j] = i+j;
		  B[i][j] = 0;
		}
	}

	B[10][10] = 15; // no error
	A[10][10] = 21; //1-bit error
	A[30][30] = 63; //2-bit error
	
	#pragma mem_fault_tolerance
	for(i=0; i<100; i++)
	{
		for(j=0; j<100; j++)
		  B[i][j] = A[j][i];
	}
}
