
void foo(int *ptr)
{
}


int main()
{

	int A[100], B[100][100][100], C[100][100];
	int i;
	
	foo(C[0]+0);

	#pragma mem
	foo(&A[0]);

	#pragma mem
	foo(&B[0][0][0]);

	i = B[0][0][0];
}
