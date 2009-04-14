// win_buffer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"


int _tmain(int argc, _TCHAR* argv[])
{
	int* arr = (int*)malloc (sizeof(int)*10);
	int i=0;
	for (i=0;i<10;++i) {
		arr[i]=5;
	}
	int x = arr[12];
	printf("All good %d.\n",x);
	return 0;
}

