#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<vector>
#include<string>
#include <time.h>

using namespace std;

#include "core.h"
#include "multicore.h"


template <typename T>
void high_level_relax ( MulticoreArray<T> & input, MulticoreArray<T> &result );

template <typename T>
void high_level_relax_optimized ( MulticoreArray<T> & input, MulticoreArray<T> &result );

int main(int argc, char *argv[]) {

	int sizeX, sizeY, sizeZ;
	int coreX, coreY, coreZ;

	if(argc != 7) {
		printf("Usage: <executable> sizeX sizeY sizeZ coreX coreY coreZ\n");
		return -1;
	}
	
	sizeX = atoi(argv[1]);
	sizeY = atoi(argv[2]);
	sizeZ = atoi(argv[3]);

	coreX = atoi(argv[4]);
	coreY = atoi(argv[5]);
	coreZ = atoi(argv[6]);
	printf("Testing MultiCoreArray: %dx%dx%d, Core: %dx%dx%d\n", sizeX, sizeY, sizeZ, coreX, coreY, coreZ);

	int inputSize[DIM]              = {sizeX, sizeY, sizeZ};
	int numberOfCoresArraySize[DIM] = {coreX,coreY,coreZ};
        int interArraySubsectionPadding =  0; // Page size to avoid cache conflicts with memory allocted for use by each core
	MulticoreArray<float> input1(inputSize[0],inputSize[1],inputSize[2],numberOfCoresArraySize[0],numberOfCoresArraySize[1],numberOfCoresArraySize[2],interArraySubsectionPadding,false);
	MulticoreArray<float> input2(inputSize[0],inputSize[1],inputSize[2],numberOfCoresArraySize[0],numberOfCoresArraySize[1],numberOfCoresArraySize[2],interArraySubsectionPadding,false);
	MulticoreArray<float> result1(inputSize[0],inputSize[1],inputSize[2],numberOfCoresArraySize[0],numberOfCoresArraySize[1],numberOfCoresArraySize[2],interArraySubsectionPadding,false);
	MulticoreArray<float> result2(inputSize[0],inputSize[1],inputSize[2],numberOfCoresArraySize[0],numberOfCoresArraySize[1],numberOfCoresArraySize[2],interArraySubsectionPadding,false);
     
	srand(time(NULL));

	//initialize the array
	for (int k = 0; k < sizeZ; k++)
		for (int j = 0; j < sizeY; j++)
			for (int i = 0; i < sizeX; i++) {
			      float t = (float)rand()/RAND_MAX;
                              input1(i,j,k) = t ;
                              input2(i,j,k) = t ;
                            }

	assert(input1 == input2);

//	input1.display("Input Array to naive version:");

	// naive version
	high_level_relax(input1, result1);

//	input2.display("Input Array to optimized version:");

	// optimized version
	high_level_relax_optimized(input2, result2);
	
	if(result1 == result2) {
		printf("TEST PASSED!\n");
	} else {

		printf("TEST FAILED.\n");
	}



	return 0;
}
