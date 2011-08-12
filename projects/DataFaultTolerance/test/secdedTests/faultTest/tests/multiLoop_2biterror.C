

int main()
{

	int A[100], i, temp;

	#pragma mem_fault_tolerance
	for(i=0; i<100; i++)
		A[i] = 5;

	A[2] = 4;
	A[4] = 6;

	#pragma mem_fault_tolerance
	for(i=0; i<100; i++)
	{
		temp = A[i];
	}
}
