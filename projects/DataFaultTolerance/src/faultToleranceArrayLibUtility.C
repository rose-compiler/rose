/*
 * faultToleranceArrayLibUtility.C
 *
 *  Created on: Jun 3, 2011
 *      Author: sharma9
 */

#include <string>
#include <map>
#include <iostream>
#include <vector>
#include <sstream>


using namespace std;

/*
 * Block Parity Algorithm
 */
typedef struct hashVal
{
	vector<bool> parity_col;
	unsigned int parity_row;
};

map< string, hashVal > hashTable;

/*
 * Counts the number of 1 bits set
 * in an unsigned int
 */
int countOneBits(unsigned int val)
{
        int count;
        for( count=0; val!=0; val>>=1)
        {
                if( val & 01 )
                  count++;
        }

        return count;
}


/*
 * Calculates the parity
 */
bool calculateParity(unsigned int data)
{
        int count = countOneBits(data);

        if(count%2==0)
        {
                return false;
        }
        else
        {
                return true;
        }
}

string getName(string arrayName, int startIndex, int endIndex)
{
	stringstream name;
	name << arrayName << "_"<< startIndex << "_" << endIndex ;
	return name.str();
}

/*
 * Update hash entry
 * Check if there exist a value and update it
 * else create a new entry
 */
void updateHashEntry(string arrayEntry, unsigned int parity_row, vector<bool> parity_col)
{
	hashVal hval;

	hval.parity_row = parity_row;
	hval.parity_col = parity_col;

	// Update the value
	hashTable[arrayEntry]= hval;

}

/*
 * Returns an iterator if a hashEntry is found in the hash table
 */
map<string, hashVal>::iterator getHashEntry(string arrayEntry)
{
	map<string, hashVal>::iterator it;

	it=hashTable.find(arrayEntry);

	/* Element not found */
	if(it==hashTable.end())
	{
		cerr << " Array Entry " << arrayEntry << " not found in hash table. " << endl;
		exit(1);
	}

	return it;
}

/*
 * Update the entire array
 * LB: Starting bound inclusive of LB
 * UB: Upper bound exclusive of UB
 * chunk: Chunk size
 */
void processUpdateArray(string arrayName, int *arrayPtr, int index1, int index2, int UB1, int UB2, int LB3, int UB3, int chunk)
{
	/* Adjust the bounds */
	int adjLB = ((int)(LB3/chunk))*chunk;
	int adjUB = ((int)((UB3-1)/chunk) + 1) * chunk;

	vector<bool> local_parity_col;
	unsigned int local_parity_row=0;

	for( int i=adjLB; i<=adjUB ; i++)
	{
		int value = 0;

		/* Check if the array values should be accessible */
		if(i>=LB3 && i<UB3)
			value = *(arrayPtr + index1*UB2*UB3 + index2*UB3 + i);

		if((i%chunk==0) && i!=adjLB)
		{
			/* Calculate parity row's parity */
			local_parity_col.push_back(calculateParity(local_parity_row));

			string name = getName(arrayName, i-chunk, i-1);
			updateHashEntry(name, local_parity_row, local_parity_col);


			/* Reset value */
			local_parity_row = 0;
			local_parity_col.clear();
		}

		/* Update values */
		local_parity_row ^= (unsigned int) value;
		local_parity_col.push_back(calculateParity((unsigned int)value));
	}

}


void processUpdateElem(string arrayName, int index, int newValue, int oldValue, int chunk)
{
	int startChunk = ((int)(index/chunk))*chunk;
	int endChunk = startChunk+chunk-1;
	int chunkIndex = index%chunk;

	string name = getName(arrayName, startChunk, endChunk);

	map<string, hashVal>::iterator it=hashTable.find(name);

	/* Element not found */
	if(it==hashTable.end())
	{
		vector<bool> parity_col(chunk+1);
		bool parityBit = calculateParity((unsigned int)newValue);
		parity_col.at(chunkIndex) = parityBit;
		parity_col.at(chunk) = parityBit;
		updateHashEntry(name, newValue, parity_col);
	}
	else
	{
		// Hash entry for this element
		hashVal hval = hashTable[name];
		hval.parity_row = hval.parity_row ^ oldValue ^ newValue;
		hval.parity_col.at(chunkIndex) = calculateParity((unsigned int)newValue); // Update new value parity
		hval.parity_col.at(chunk) = calculateParity(hval.parity_row); // Update parity row's parity bit
	}
}

/*
 * Determines the column error location
 * TODO: Check the row also
 */
void getErrorCol(vector<bool> parity_col1, vector<bool> parity_col2, int startIndex)
{
	if(parity_col1 == parity_col2)
	{
		cerr << " 2-bit error occurred " << endl;
	}

	for(int i=0; i<parity_col1.size()-1; i++ )
	{
		if(parity_col1.at(i) != parity_col2.at(i))
		{
			cerr << " 1-bit Error occurred at index location: " << startIndex+i << endl;
			return;
		}
	}

}

/*
 * Validates a hash entry for a chunk of array
 */
void checkHashEntry(string arrayEntry, unsigned int parity_row, vector<bool> parity_col, int startIndex)
{
	map<string, hashVal>::iterator it = getHashEntry(arrayEntry);

	hashVal retrievedVal = (*it).second;

	if(retrievedVal.parity_row != parity_row || retrievedVal.parity_col != parity_col)
	{
		cerr << " Parity row or column for array entry " << arrayEntry << " doesn't match " << endl;
		//getErrorCol(parity_col, retrievedVal.parity_col, startIndex);
		exit(1);
	}

}





/*
 * Validate the entire array
 * LB: Starting bound inclusive of LB
 * UB: Upper bound exclusive of UB
 * chunk: Chunk size
 */
void processValidateArray(string arrayName, int *arrayPtr, int index1, int index2, int UB1, int UB2 ,int LB3, int UB3, int chunk)
{
	/* Adjust the bounds */
	int adjLB3 = ((int)(LB3/chunk))*chunk;
	int adjUB3 = ((int)((UB3-1)/chunk) + 1) * chunk;


	vector<bool> local_parity_col;
	unsigned int local_parity_row=0;

	for( int i=adjLB3; i<=adjUB3 ; i++)
	{
		int value = 0;

		/* Check if the array values should be accessible */
		if(i>=LB3 && i<UB3)
			value = *(arrayPtr + index1*UB2*UB3 + index2*UB3 +i);

		if((i%chunk==0) && i!=adjLB3)
		{
			/* Calculate parity row's parity */
			local_parity_col.push_back(calculateParity(local_parity_row));

			string name = getName(arrayName, i-chunk, i-1);
			checkHashEntry(name, local_parity_row, local_parity_col, i-chunk);

			/* Reset value */
			local_parity_row = 0;
			local_parity_col.clear();
		}

		/* Update values */
		local_parity_row ^= (unsigned int) value;
		local_parity_col.push_back(calculateParity((unsigned int)value));
	}

}

