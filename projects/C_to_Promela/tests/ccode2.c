#pragma SPIN_TARGET_ALL
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
		#pragma SPIN_TARGET
	if ((d-1)==0)
	{
		even=true;
		return 1;
	}
#pragma	assert(c==10);
	return 0;
}
