int myGlobal;
int myMax(int a,int b)
{
	int retVal=0;
	if(a>b) 
		retVal=a;
	else
		retVal=b;
	#pragma SPIN_TARGET
	myGlobal=4;
	return retVal;
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
	for (int i=0;i<100;i++)
	{
		a=1;
	}
		#pragma SPIN_TARGET
	c=myMax(a,b);
		#pragma SPIN_TARGET
	if ((d-1)==0)
	{
		even=true;
	}
#pragma	assert(c==10);
	return 0;
}
