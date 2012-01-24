/*
 * arrayBase.C
 *
 *  Created on: Nov 1, 2011
 *      Author: kamalsharma
 */

#include <vector>
#include <string>

#include "arrayBase.h"

using namespace std;

/*
 *   Start of ArrayInfo Implementation
 */

/*
 * Setters
 */
void ArrayInfo::setDim(int dimIndex, int LB, int UB)
{
	setDimLB(dimIndex, LB);
	setDimUB(dimIndex, UB);
}

void ArrayInfo::setDimLB(int dimIndex, int LB)
{
	dimLB[dimIndex] = LB;
}

void ArrayInfo::setDimUB(int dimIndex, int UB)
{
	dimUB[dimIndex] = UB;
}

/*
 * Getters
 */
void ArrayInfo::getDim(int dimIndex, int *LB, int *UB)
{
	*LB = getDimLB(dimIndex);
	*UB = getDimUB(dimIndex);
}

int ArrayInfo::getDimLB(int dimIndex)
{
	return dimLB[dimIndex];
}

int ArrayInfo::getDimUB(int dimIndex)
{
	return dimUB[dimIndex];
}

void ArrayInfo::incrementNoDims()
{
	noDims++;
}

int ArrayInfo::getNoDims()
{
	return noDims;
}

string ArrayInfo::getArrayName()
{
	return arrayName;
}

/*
 *   End of ArrayInfo Implementation
 */


void ArrayList::insertArrayInfo(ArrayInfo* arrayInfo)
{
	arrayList.push_back(arrayInfo);
}

ArrayInfo* ArrayList::getArrayInfo(string arrayName)
{
	vector<ArrayInfo*>::iterator it;

	for ( it=arrayList.begin() ; it < arrayList.end(); it++ )
	{
		ArrayInfo* arrayInfo = *it;

		if(arrayInfo->getArrayName().compare(arrayName) == 0)
		{
			return arrayInfo;
		}
	}

	return NULL;
}

void ArrayList::setChunksize(int value)
{
	chunksize = value;
}

int ArrayList::getChunksize()
{
	return chunksize;
}

/*
 *   Start of ArrayList Implementation
 */
