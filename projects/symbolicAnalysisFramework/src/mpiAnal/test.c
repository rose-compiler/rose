#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv)
{
	if(argc<2) return -1;
	
	int numIter = atoi(argv[1]);
	for(int i=0; i<numIter; i++)
	{
		//sleep(1);
		for(int j=0; j<1000000; j++)
		{ printf(""); }
		printf("i=%d\n", i);
	}
   return 0;
}
