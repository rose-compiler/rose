/*
 * faultToleranceArrayLibUtility.h
 *
 *  Created on: Nov 1, 2011
 *      Author: kamalsharma
 */

#ifndef FAULTTOLERANCEARRAYLIBUTILITY_H_
#define FAULTTOLERANCEARRAYLIBUTILITY_H_

#include <vector>
#include <string>
#include <map>

using namespace std;

/*
 * Block Parity Algorithm
 */
struct hashVal
{
	vector<bool> parity_col;
	unsigned int parity_row;
};

int countOneBits(unsigned int val);
bool calculateParity(unsigned int data);
string getName(string arrayName, int startIndex, int endIndex);
void updateHashEntry(string arrayEntry, unsigned int parity_row, vector<bool> parity_col);
void clearAllHashEntries();
//map<string, hashVal>::iterator getHashEntry(string arrayEntry);
void processUpdateArray(string arrayName, int *arrayPtr, int index1, int index2, int UB1, int UB2, int LB3, int UB3, int chunk);
void processUpdateElem(string arrayName, int index, int newValue, int oldValue, int chunk);
void getErrorCol(vector<bool> parity_col1, vector<bool> parity_col2, int startIndex);
void checkHashEntry(string arrayEntry, unsigned int parity_row, vector<bool> parity_col, int startIndex);
void processValidateArray(string arrayName, int *arrayPtr, int index1, int index2, int UB1, int UB2 ,int LB3, int UB3, int chunk);


#endif /* FAULTTOLERANCEARRAYLIBUTILITY_H_ */
