

int main()
{

	int A[100], i;

	for(i=0; i<100; i++)
		A[i] = 5;

#pragma no_mem_fault_tolerance
	for(i=0; i<100; i++)
		A[i] = 5;
}
