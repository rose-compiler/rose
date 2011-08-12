

int main()
{

	int A[100], i, temp;

	#pragma mem_fault_tolerance
	for(i=0; i<100; i++)
		A[i] = 5;

	#pragma mem_fault_tolerance
	for(i=0; i<100; i++)
	{
		temp = A[i];
	}
}
