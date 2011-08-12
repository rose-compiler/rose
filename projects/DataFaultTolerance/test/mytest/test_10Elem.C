

int main()
   {
	int i;
	int A[100][100],B[100][100],C[100];
	float Fl[100];

	#pragma mem_fault_tolerance
	for(i=0; i<10; i++)
	{
		A[i][5] = 0;
	}

	#pragma mem_fault_tolerance
	for(i=0; i<10; i++)
	{
		B[i][5]=A[i][5]+C[i];
	}

     return 0;
   }
