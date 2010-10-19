int array[10][20][30];
int indexes[10];

void foo()
{
	int i=0,j=0, k, l;
	
	for(i=0; i<10; i++)
	{
		array[indexes[i]][i][9] = 1;
	}
	
	l=5;
	k=l;
	l=k;
	
	i=j+23;
	
	i=j+3;
}

void bar()
{
	int a=0, b=0, c=0, d=0, e=0, f=9;
	
	a=b+1;
	b = array[b][1][1];
	a=b+2;
	
	indexes[b] = indexes[b+1];
	a=b+3;
	
	if(c<d+1){}
	a=b+4;
		
	if(e==d){}
	
	a=b-f;
	
	for(d=0; d<10; d++)
		indexes[a]=b;
}
