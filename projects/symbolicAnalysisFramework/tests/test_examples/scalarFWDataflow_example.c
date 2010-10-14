#define NOOP a=x;

void func(int& var, int var2)
{
	//var += var2;
	var++;
	//var=1;
}

void bar(int& var);

int main(int argc, char** argv)
{
	int x=1, y, z, a, b=1, c=2;
	
	for(z=5; z<100; z+=5)
	{ c=1; }
	
	//bar(c);
	//c=16;
	
	func(b, c);
	
	while(x==1){NOOP;}
	
	if(x<=0)
	{NOOP;}
	else
	{NOOP;}
	
	if(x<=5)
	{
		y=1;
	}
	else
	{NOOP;}
	y=2;
	
	/*if(x<=10)
	{
		y=1;
	}
	else
	{
		z=2;	
	}*/
}


/*int main(int argc, char** argv)
{
	int x, y, z;
	z=5;
	x=z + 6;
	y = -4* x;
	while(z<10)
	{
		z = z+5;
	}
	if(x<10)
	{
		z=z+10;
	}
	else
	{
		y = y*2;
	}
}
*/
