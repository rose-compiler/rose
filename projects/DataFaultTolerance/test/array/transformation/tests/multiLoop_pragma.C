

int main()
{

	int A[100], i, temp;

	#pragma mem_fault_tolerance (A:<0:100>) (Chunksize=8)
	for(i=0; i<100; i++)
		A[i] = 5;

	#pragma mem_fault_tolerance (A:<0:100>) (Chunksize=8)
	for(i=0; i<100; i++)
	{
		temp = A[i];
	}
}
