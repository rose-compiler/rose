#define NOOP x=x;

int main(int argc, char** argv)
{
/*	int x, y, z;
	for(z=5; z<100; z+=5)
	{ }
	
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
	
	if(x<=10)
	{
		y=1;
	}
	else
	{
		z=2;	
	}*/
	int x=0;
	int y=1;
	int z=0;
	if(x<=0)
	{
		x=x+12;
		y+=1;
		
		if(x<=10)
			z=2;
		else
			z=1;
	}
	else
	{ 
		x+=3;
		y+=1;
	}
	//x=5;
	
	return 1;
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