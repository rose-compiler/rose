

int main()
{

	int A[100], B[100], i;

	#pragma mem_fault_tolerance
	for(i=0; i<100; i++)
	{
		A[i] = -5;
		B[i] = -i;
	}
	
	#pragma mem_fault_tolerance
	for(i=0; i<100; i++)
	{
		B[i] = B[i] + A[i];
	}
}
