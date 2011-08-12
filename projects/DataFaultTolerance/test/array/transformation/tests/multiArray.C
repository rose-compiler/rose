

int main()
{

	int A[100], B[100], i, temp;

	#pragma mem_fault_tolerance (A:<0:100>) (B:<0:100>) (Chunksize=10)
	for(i=0; i<100; i++)
	{
		A[i] = 5;
		B[i] = 0;
	}
	
	#pragma mem_fault_tolerance (A:<0:100>) (B:<0:100>) (Chunksize=10)
	for(i=0; i<100; i++)
	{
		B[i] = B[i] + A[i];
	}
}
