/*
 * faultToleranceLib.C
 *
 *  Created on: May 24, 2011
 *      Author: sharma9
 */

#include <string>
#include <map>
#include <iostream>

#include "secded.h"

using namespace std;

/*
 * Implementing SECDED( Single Error Correction
 * Double Error Detection ) Algorithm
 * even parity bit is used for double error detection
 * 		set to 1 if number of 1 bits is odd
 * Checkbits = log2(No. of data type bits)
 * Thus, unsigned short int is sufficient
 */
typedef struct hashVal
{
	bool parity;
	unsigned short int checkbits;
	string type;
};

map< void *, hashVal > hashTable;


unsigned short int calculateCheckBits(void *memLoc, string type, bool *parity)
{
	unsigned int checkbits;

	if(type.compare("int") == 0)
	{
		int data = *static_cast<int*>(memLoc);
		checkbits = calculateIntCheckBits(data);
		*parity = calculateParity(data, checkbits);
	}
	else if(type.compare("float") == 0)
	{
		float data = *static_cast<int*>(memLoc);
		checkbits = calculateIntCheckBits(data);
		*parity = calculateParity(data, checkbits);
	}
	else
	{
		cerr << " Memory Element Type unknown " << endl;
		exit(0);
	}

	return checkbits;
}

void updateCheckBits(void *memLoc, string type)
{
	hashVal hval;
	bool parity;
	hval.checkbits = calculateCheckBits(memLoc, type, &parity);
	hval.parity = parity;
	hval.type = type;

	// Update the value
	hashTable[memLoc]= hval;

	//cout << " Updates checkbit: " << hval.checkbits << " parity: " << parity << endl;
}


void verifyCheckBits(void *memLoc, string type)
{
	map<void *, hashVal>::iterator it;

	it=hashTable.find(memLoc);
	/* Element not found, insert into map */
	if(it==hashTable.end())
	{
		//cout << " Updated hashTable " << endl;
		updateCheckBits(memLoc, type);
		return;
	}

	hashVal retrievedVal = (*it).second;

	bool calParity;
	unsigned short int calCheckBits = calculateCheckBits(memLoc, type, &calParity);
	applySECDED(memLoc, type, retrievedVal.checkbits, retrievedVal.parity, calCheckBits, calParity);

	//cout << " Retreived checkbits: " << calCheckBits << " parity: " << calParity << endl;
}


