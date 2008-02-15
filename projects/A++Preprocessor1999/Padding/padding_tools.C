#include "padding_tools.h"
#include "padding_conditions.h"


// CW: function returns TRUE if a and b are conforming arrays
bool conformingArrays(const arrayInfo& aInfo, const arrayInfo& bInfo)
{
	if(aInfo.getNumberOfDimensions()!=bInfo.getNumberOfDimensions()) return FALSE;
	else{
		int maxDimension=aInfo.getNumberOfDimensions();
		for(int k=1;k<maxDimension;k++)
		{
			// CW: column sizes must be identical except of the
			// size of the column of the highest dimension
			if(aInfo.getColSize(k)!=bInfo.getColSize(k)) return FALSE;
		}
		return TRUE;
	}
}


// CW: Returns an arrayInfo which has changed column sizes so
// that non of the specified padding conditions is true.
// If the algorithm is not able to find a good padding
// size the original array size is returned
arrayInfo getNeededIntraPadding(const cacheInfo& cInfo, const arrayInfo& aInfo,const List<intra_padding_condition>& ipcList)
{
	// CW: test if a padding is required!
	if(testIntraPaddingConditions(cInfo,aInfo,ipcList)==FALSE) return aInfo;

	int currentPadding=0;
	arrayInfo testInfo(aInfo);
	
	// CW: if our padding is to large we stop
	while( (currentPadding*aInfo.getElementSize() < cInfo.getCacheSize()) && (currentPadding < PADDING_SIZE_MAX))
	{
		int testDim = 1;
		while( testDim < aInfo.getNumberOfDimensions() )
		{
			// CW: change column size of tested dimension
			const int oldColSize = testInfo.getColSize(testDim);
			testInfo.setColSize(testDim,oldColSize+1);

			// is padding sufficient?
			if(testIntraPaddingConditions(cInfo,testInfo,ipcList)==FALSE) return testInfo;
			// CW: if not reset column size
			testInfo.setColSize(testDim,oldColSize);				
			// CW: and test padding of another dimension
			testDim++;			
		};
		
		// CW: increase padding of all columns
		currentPadding++;
		for(int i=1;i<aInfo.getNumberOfDimensions();i++)
		{
			testInfo.setColSize(i,testInfo.getColSize(i)+1);			
		}
	}
	
	// CW: found no appropriate padding...
#ifndef NDEBUG
		cout << "getNeededIntraPadding: found no padding!" << endl;
#endif
	return aInfo;
}

// CW: Returns new arrayInfo for array B with (eventually) changed base address
// so that non of the specified inter array padding conditions is TRUE. 
// If no inter array padding is needed or no good inter array padding can be found
// the original 
arrayInfo getNeededInterPadding(const cacheInfo& cInfo, const arrayInfo& aInfo, const arrayInfo& bInfo, const List<inter_padding_condition>& ipcList)
{
	// CW: do we need a inter array padding at all?
	if(testInterPaddingConditions(cInfo,aInfo,bInfo,ipcList)==FALSE) return bInfo;
	else{
		int currentPadding=0;
		arrayInfo testInfo(bInfo);

		// as long as the padding is not to large...
		while( (currentPadding*bInfo.getElementSize() < cInfo.getCacheSize()) && (currentPadding < PADDING_SIZE_MAX))
		{
			const int oldBaseAddress = testInfo.getBaseAddress();
			// CW: we move the base address of array B by 
			// one element. If the base address was aligned in
			// the beginning then the new base address should be aligned
			// as well
			testInfo.setBaseAddress(oldBaseAddress+testInfo.getElementSize());
			currentPadding++;

			if(testInterPaddingConditions(cInfo,aInfo,testInfo,ipcList)==FALSE) return testInfo;

		}

		// CW: We have not found a good inter array padding. Hence we have to
		// use the old array layout

#ifndef NDEBUG
		cout << "getNeededInterPadding: found no padding!" << endl;
#endif
		return bInfo;
	}
}


// CW: test intra padding conditions for current array size and
// returns FALSE if all of them are FALSE or TRUE otherwise.
bool testIntraPaddingConditions(const cacheInfo& cInfo, const arrayInfo& aInfo,const List<intra_padding_condition>& ipcList)
{
	int currentCondition=0;
	bool condition = FALSE;
	
	while( (condition==FALSE) && (currentCondition < ipcList.getLength()) )
	{
		condition = (ipcList[currentCondition])(cInfo,aInfo);
		currentCondition++;
	};
	return condition;
}

// CW: test inter padding conditions for current array sizes and
// current array base addresses. It returns FALSE if all of the
// inter array padding conditions are FALSE or TRUE otherwise.
bool testInterPaddingConditions(const cacheInfo& cInfo, const arrayInfo& aInfo, const arrayInfo& bInfo,const List<inter_padding_condition>& ipcList)
{
	int currentCondition=0;
	bool condition = FALSE;
	
	while( (condition==FALSE) && (currentCondition < ipcList.getLength()) )
	{
		condition = (ipcList[currentCondition])(cInfo,aInfo,bInfo);
		currentCondition++;
	};
	return condition;
}
