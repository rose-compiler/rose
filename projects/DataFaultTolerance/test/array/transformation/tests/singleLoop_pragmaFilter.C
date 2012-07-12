

int main()
{

	int A[100], i;

	#pragma mem_fault_tolerance
	for(i=0; i<100; i++)
		A[i] = 5;
}
