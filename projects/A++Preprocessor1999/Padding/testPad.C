#include <iostream.h>
#include <assert.h>
#include <stdlib.h>

#include "padding_conditions.h"
#include "padding_tools.h"
#include "alignData.h"

static void printBool(ostream &os,bool b);
static int testIntra(int argv, char **argc);
static int testInter(int argv, char **argc);
static void initArrayReferences(arrayInfo& aInfo,arrayInfo& bInfo);

int main(int argv, char **argc)
{
	cout << "======= inter =======" << endl;
	testInter(argv,argc);
	
	cout << "======= intra =======" << endl;
	testIntra(argv,argc);
	
	return 0;
	
}

static int testInter(int argv, char **argc)
{
	if(argv<5){
		cout << "Usage: <cache size> <line size> <elem size> [<dim size>]+" << endl;
		return -1;
	}

	cacheInfo cInfo(atoi(argc[1]),atoi(argc[2]));
	cout << cInfo << endl;

	List<const int> sizes;
	List<const int> lowerBounds;

	for(int i=4;i<argv;i++)
	{
		sizes.addElement(*new const int(atoi(argc[i])));
		lowerBounds.addElement(*new const int(0));
	}

	arrayInfo aInfo(atoi(argc[3]),0,sizes,lowerBounds);
	arrayInfo bInfo(atoi(argc[3]),alignData(aInfo.getSize()),sizes,lowerBounds);

	// 2D ???
	if((argv==6)) initArrayReferences(aInfo,bInfo);

	// CW: specify padding conditions...
	List<inter_padding_condition> ipcList;
	//ipcList.addElement(condition_interpad);
	ipcList.addElement(condition_interpadlite);

	arrayInfo newBInfo = getNeededInterPadding(cInfo,aInfo,bInfo,ipcList);
	
	cout << "b: old base address: " << bInfo.getBaseAddress() << endl;
	cout << "b: new base address: " << newBInfo.getBaseAddress() << endl;

	return 0;
}

static int testIntra(int argv, char **argc)
{
	if(argv<5){
		cout << "Usage: <cache size> <line size> <elem size> [<dim size>]+" << endl;
		return -1;
	}
	
	cacheInfo cInfo(atoi(argc[1]),atoi(argc[2]));
	cout << cInfo << endl;

	List<const int> sizes;
	List<const int> lowerBounds;

	for(int i=4;i<argv;i++)
	{
		// CW: I now this is ugly but since the list class sucks that
		// is the only way to be safe
		sizes.addElement(*new const int(atoi(argc[i])));
		lowerBounds.addElement(*new const int(0));
	}

	arrayInfo aInfo(atoi(argc[3]),0,sizes,lowerBounds);
	arrayInfo bInfo(atoi(argc[3]),alignData(aInfo.getSize()),sizes,lowerBounds);

	// 2D ???
	if((argv==6)) initArrayReferences(aInfo,bInfo);

	// CW: specify padding conditions...
	List<intra_padding_condition> ipcList;
	ipcList.addElement(condition_linpad1);
	ipcList.addElement(condition_linpad2);
	ipcList.addElement(condition_intrapadlite);
	ipcList.addElement(condition_intrapad);

	// CW: perform padding...
	arrayInfo newAInfo=getNeededIntraPadding(cInfo,aInfo,ipcList);
	// CW: adjust base address
	bInfo.setBaseAddress(alignData(newAInfo.getSize()));
	arrayInfo newBInfo=getNeededIntraPadding(cInfo,bInfo,ipcList);

	cout << "a: old size: (" << aInfo.getColSize(1);
	for(i=2;i<=aInfo.getNumberOfDimensions();i++)
	{
		 cout << "," << aInfo.getColSize(i);
	}
	cout << ")" << endl;
	
	cout << "a: new size: (" << newAInfo.getColSize(1);
	for(i=2;i<=newAInfo.getNumberOfDimensions();i++)
	{
		 cout << "," << newAInfo.getColSize(i);
	}
	cout << ")" << endl;
	
	cout << "b: old size: (" << bInfo.getColSize(1);
	for(i=2;i<=bInfo.getNumberOfDimensions();i++)
	{
		 cout << "," << bInfo.getColSize(i);
	}
	cout << ")" << endl;
	
	cout << "b: new size: (" << newBInfo.getColSize(1);
	for(i=2;i<=newBInfo.getNumberOfDimensions();i++)
	{
		 cout << "," << newBInfo.getColSize(i);
	}
	cout << ")" << endl;

	if(conformingArrays(newAInfo,newBInfo)){
		cout << "new a and b are conforming!" << endl;

		// CW: specify padding conditions...
		List<inter_padding_condition> ipc2List;
		ipc2List.addElement(condition_interpad);
		ipc2List.addElement(condition_interpadlite);

		arrayInfo new2BInfo = getNeededInterPadding(cInfo,newAInfo,newBInfo,ipc2List);
		cout << "b: old base address: " << newBInfo.getBaseAddress() << endl;
		cout << "b: new base address: " << new2BInfo.getBaseAddress() << endl;

	}
	else{
		cout << "new a and b are not conforming!" << endl;

		// CW: specify padding conditions...
		List<inter_padding_condition> ipc2List;
		ipc2List.addElement(condition_interpadlite);

		arrayInfo new2BInfo = getNeededInterPadding(cInfo,newAInfo,newBInfo,ipc2List);
		cout << "b: old base address: " << newBInfo.getBaseAddress() << endl;
		cout << "b: new base address: " << new2BInfo.getBaseAddress() << endl;
	}

	return 0;
}

static void initArrayReferences(arrayInfo& aInfo,arrayInfo& bInfo)
{
	assert(aInfo.getNumberOfDimensions()==2);
	assert(bInfo.getNumberOfDimensions()==2);

	// CW: the List class implementation only uses
	// pointers instead of copying the list elements.
	// Hence, we need arrayReferences which survive the
	// scope. Since, I'm not deleting it this is a memory leak!!
	arrayReference *xx = new arrayReference[65];

	
	// CW: initialize all the center values
	xx[1][0]=+0;xx[1][1]=+0;
	xx[2][0]=-1;xx[2][1]=+1;
	xx[3][0]=-2;xx[3][1]=+2;
	xx[4][0]=-3;xx[4][1]=+3;
	xx[5][0]=-4;xx[5][1]=+4;
	xx[6][0]=-5;xx[6][1]=+5;
	xx[7][0]=-6;xx[7][1]=+6;
	xx[8][0]=-7;xx[8][1]=+7;
	xx[9][0]=-1;xx[9][1]=+0;
	xx[10][0]=-2;xx[10][1]=+1;
	xx[11][0]=-3;xx[11][1]=+2;
	xx[12][0]=-4;xx[12][1]=+3;
	xx[13][0]=-5;xx[13][1]=+4;
	xx[14][0]=-6;xx[14][1]=+5;
	xx[15][0]=-7;xx[15][1]=+6;
	xx[16][0]=-8;xx[16][1]=+7;
	xx[17][0]=-2;xx[17][1]=+0;
	xx[18][0]=-3;xx[18][1]=+1;
	xx[19][0]=-4;xx[19][1]=+2;
	xx[20][0]=-5;xx[20][1]=+3;
	xx[21][0]=-6;xx[21][1]=+4;
	xx[22][0]=-7;xx[22][1]=+5;
	xx[23][0]=-8;xx[23][1]=+6;
	xx[24][0]=-9;xx[24][1]=+7;
	xx[25][0]=-3;xx[25][1]=+0;
	xx[26][0]=-4;xx[26][1]=+1;
	xx[27][0]=-5;xx[27][1]=+2;
	xx[28][0]=-6;xx[28][1]=+3;
	xx[29][0]=-7;xx[29][1]=+4;
	xx[30][0]=-8;xx[30][1]=+5;
	xx[31][0]=-9;xx[31][1]=+6;
	xx[32][0]=-10;xx[32][1]=+7;
	xx[33][0]=-4;xx[33][1]=+0;
	xx[34][0]=-5;xx[34][1]=+1;
	xx[35][0]=-6;xx[35][1]=+2;
	xx[36][0]=-7;xx[36][1]=+3;
	xx[37][0]=-8;xx[37][1]=+4;
	xx[38][0]=-9;xx[38][1]=+5;
	xx[39][0]=-10;xx[39][1]=+6;
	xx[40][0]=-11;xx[40][1]=+7;
	xx[41][0]=-5;xx[41][1]=+0;
	xx[42][0]=-6;xx[42][1]=+1;
	xx[43][0]=-7;xx[43][1]=+2;
	xx[44][0]=-8;xx[44][1]=+3;
	xx[45][0]=-9;xx[45][1]=+4;
	xx[46][0]=-10;xx[46][1]=+5;
	xx[47][0]=-11;xx[47][1]=+6;
	xx[48][0]=-12;xx[48][1]=+7;
	xx[49][0]=-6;xx[49][1]=+0;
	xx[50][0]=-7;xx[50][1]=+1;
	xx[51][0]=-8;xx[51][1]=+2;
	xx[52][0]=-9;xx[52][1]=+3;
	xx[53][0]=-10;xx[53][1]=+4;
	xx[54][0]=-11;xx[54][1]=+5;
	xx[55][0]=-12;xx[55][1]=+6;
	xx[56][0]=-13;xx[56][1]=+7;
	xx[57][0]=-7;xx[57][1]=+0;
	xx[58][0]=-8;xx[58][1]=+1;
	xx[59][0]=-9;xx[59][1]=+2;
	xx[60][0]=-10;xx[60][1]=+3;
	xx[61][0]=-11;xx[61][1]=+4;
	xx[62][0]=-12;xx[62][1]=+5;
	xx[63][0]=-13;xx[63][1]=+6;
	xx[64][0]=-14;xx[64][1]=+7;

	for(int j=1;j<=64;j++)
	{
		// add center values
		aInfo.addReference(xx[j]);
		bInfo.addReference(xx[j]);

		// CW: add references to surrounding nodes		
		arrayReference *north = new arrayReference;
		(*north)[0]=xx[j](0)+1;
		(*north)[0]=xx[j](1);
		aInfo.addReference((*north));
		
		arrayReference *south = new arrayReference;
		(*south)[0]=xx[j](0)-1;
		(*south)[0]=xx[j](1);
		aInfo.addReference((*south));
		
		arrayReference *east = new arrayReference;
		(*east)[0]=xx[j](0);
		(*east)[0]=xx[j](1)+1;
		aInfo.addReference((*east));
		
		arrayReference *west = new arrayReference;
		(*west)[0]=xx[j](0);
		(*west)[0]=xx[j](1)-1;
		aInfo.addReference((*west));
	};
}

static void printBool(ostream &os,bool b)
{
	if(b) os << "T";
	else os << "F";
}


