int main(int argc,char **argv)
{
	int a=5,b=5;
	int c=a+b;
	int d=0;
	bool even=0;
	#pragma SPIN_TARGET
	while(d<c)
	{
		#pragma SPIN_TARGET
		d++;
	}
	if ((d%2)==0)
	{
		return 1;
	}
#pragma	assert(c==10);
	return 0;
}
