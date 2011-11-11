/*
 * pragmaHandling.h
 *
 *  Created on: Nov 1, 2011
 *      Author: kamalsharma
 */

#ifndef ARRAYBASE_H_
#define ARRAYBASE_H_

#include <string>
#include <iostream>

using namespace std;

class ArrayInfo
{
public:
	ArrayInfo(string name)
	{
		arrayName = name;
		noDims=0;
	}
	void setDim(int dimIndex, int LB, int UB);
	void setDimLB(int dimIndex, int LB);
	void setDimUB(int dimIndex, int UB);
	void getDim(int dimIndex, int *LB, int *UB);
	int getDimLB(int dimIndex);
	int getDimUB(int dimIndex);
	void incrementNoDims();
	int getNoDims();
	string getArrayName();

private:
	string arrayName;
	// Assuming max of 10 dimensions
	int dimLB[10], dimUB[10];
	int noDims;
};

/*
 * ArrayList Class
 */
class ArrayList
{
public:
	void insertArrayInfo(ArrayInfo* arrayInfo);
	ArrayInfo* getArrayInfo(string arrayName);
	void setChunksize(int value);
	int getChunksize();

private:
	vector<ArrayInfo*> arrayList;
	int chunksize;
};


/*
 * End of ArrayList Class
 */

#endif /* ARRAYBASE_H_ */
