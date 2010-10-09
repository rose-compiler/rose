#pragma omp greg

int omp_get_num_threads();
int omp_get_thread_num();
void genericForHeader();

void foo();
void bar();
int a, b, c;

int main(int argc, char** argv)
{
	int d[10], e, f;
	#pragma omp parallel for private(a, d) firstprivate(b, c) default(shared) ordered
	for(int iterator=0; iterator<10; iterator++)
	{
		a=b=c=0;
	}
	
	bar();
	
	#pragma omp parallel private(a, d) firstprivate(b, c) default(shared) 
	{
		d[0] = e = f = 1;
	}
	
	#pragma omp critical
	bar();
	
	#pragma omp single nowait
	bar();
	
	#pragma omp master
	bar();
	
	#pragma omp critical
	{
	}
	
	#pragma omp single
	{
		d[0] = e = f = 1;
	}
	
	#pragma omp master
	{
		d[0] = e = f = 1;
	}

	return 0;
}
