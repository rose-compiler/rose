int max(int a,int b)
{
	if(a>b) return a;
	else return b;
}

int main(int argc,char **argv)
{
	int a=5,b=5;
	int c=a+b;
	int d;
	bool even=0;
	#pragma SPIN_TARGET
	while(d<c)
	{
		#pragma SPIN_TARGET
		d++;
	}
	c=max(a,b);
		#pragma SPIN_TARGET
	if ((d-1)==0)
	{
		even=true;
	}
#pragma	assert(c==10);
	return 0;
}
