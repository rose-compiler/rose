

int main()
   {
	int i;
	int A[100][100],B[100][100],C[100];
	float Fl[100];

	#pragma mem_fault_tolerance
	for(i=0; i<100; i++)
	{
		A[i][5] = 0;
		B[i][5] = 5;
		Fl[i] = 10.99;
	}

	#pragma mem_fault_tolerance
	for(i=0; i<100; i++)
	{
		A[i][5]=B[i][5]+C[i];
	}

	#pragma mem_fault_tolerance
	for(i=0; i<99; i++)
		Fl[i] = Fl[i+1];

     return 0;
   }
