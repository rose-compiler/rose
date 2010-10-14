/*int globalZ;

int greg=0;
void foo(int& a_arg)
{
	for(; a_arg<10; a_arg+=10)
	{
	}
	//a_arg++;

	globalZ=5;
	while(globalZ<100)
	{
		globalZ*=5;
	}
	
	if(globalZ>=0)
	{
		greg=1;
		if(globalZ<=-1)
		{
			greg=3;
		}
		else
		{
			greg=4;
		}
	}
	else
	{
		greg=2;
		if(globalZ>=5)
		{
			greg=5;
		}
		else
		{
			greg=6;
		}
	}
}

int baz(int& q)
{
	int a, b, c=1;
	a=2;
	for(b=0; b<10; b+=5, c++)
	{
		a=4;
	}
	c=6;
	
	q++;
	
	return ++a;
}
// a: div=2, rem = 1
// b: div=5, rem = 0
// c = 5


int main()
{
	int x, y, z;
	int b=1, c=6, d=3;

	x=0;
	
	foo(x);
	globalZ++;
	
	for(int i=0; i<10; i+=4)
	{
		d*=2;
		d=d*10;
	}
	
	b=c-d;
	
	y=100;
	foo(y);
	
	return 1;
}
// x: div = 10, rem = 1
// b: div = 3, rem = 0
// c = 6
// d: div = 3, rem = 0
// i: div = 4, rem = 0
// globalZ: div = 5, rem = 0
*/

/*void affineInequalitiesPlacerTest()
{
	int q;
	if(q==0) q++;
	else q--;
	
	if(q>=0){}
	else{}
	
	if(q<=0)	{ q*=2;	q*=20; }
	else { q/=2; q/=20;	}
	
	if(q>0){ q-=3;	}
	else { q+=3; }
	
	int i;
	for(i=0; i<10; i++)
	{ i--; }
	i++;
	
	for(int j=100; j>50; j--)
	{}
}*/

int frump(int q)
{
	int a[100];
	int b;

	int i, p=0;
	for(i=0; i<q; i++)
	{
		a[p] = i;
		a[p+1] = i;
		a[p+2] = i;
		a[p+3] = i;
		a[p+4] = i;
		a[p+5] = i;
		a[p+6] = i;
		p+=7;
	}
	
	int bound;
	bound=p/7;
	int l=0;
	for(int j=0; j<bound; j++)
	{
		b = a[l];
		b = a[l+1];
		b = a[l+2];
		b = a[l+3];
		b = a[l+4];
		b = a[l+5];
		b = a[l+6];
		l+=7;
	}

	
	/*int m = 0;
	int a[100];
	int q;
	
	for(int i=0; i<100; i++)
	{
		a[m] = m;
		a[m+1] = m;
		m+=2;
	}*/
	
	/*for(int j=0; j<m; )
	{
		q = a[j];
		q = a[j+1];
		j+=2;
	}*/

	return 0;
}
