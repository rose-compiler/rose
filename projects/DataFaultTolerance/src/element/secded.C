/*
 * hammingCode.C
 *
 *  Created on: May 25, 2011
 *      Author: sharma9
 */

/*
 * This file contains implementation methods
 * for Hamming Code (SECDED) algorithm.
 *
 */

#include <iostream>
#include <string>

using namespace std;

void printBits(unsigned int x)
{
		cout << "\n Bit Pattern : ";
        for(; x!=0; x>>=1)
                if(x & 01)
                        cout << "1";
                else
                        cout << "0";
        cout << endl;
}


/*
 * Counts the number of 1 bits set
 * in an unsigned int
 */
int countOneBits(int intVal)
{
        int count;
        unsigned int val = intVal;

        for( count=0; val!=0; val>>=1)
        {
                if( val & 01 )
                  count++;
        }

        return count;
}

/*
 * Calculates the Even parity bit based on data
 * and check bits
 * Set to 1 if number of 1 bits in (data+check) is odd
 */
bool calculateParity(int data, unsigned int checkbits)
{
        int count = countOneBits(data);
        count += countOneBits(checkbits);

        if(count%2==0)
        {
                //cout << " Parity 0 " << endl;
                return false;
        }
        else
        {
                //cout << " Parity 1 " << endl;
                return true;
        }
}


unsigned short int calculateIntCheckBits(int value)
{
	unsigned int checkbits = 0, c;
	// Skip 1 and 2 position for check bits
	int count=3;
	int power2 = 4;

	for( c=value; c!=0; c>>=1, count++)
	{
			// Skip powers of 2 for check bits
			if(count == power2)
			{
					count++;
					power2 = 2*power2;
			}

			if( c & 01 )
			   checkbits ^= count;
	}

	//printBits(checkbits);

	return checkbits;
}

void correctIntDataBit(void *memLoc, int errorBitLoc)
{
	int data = *static_cast<int*>(memLoc);;
	int correctData = 0;

	int count=3;
	int power2 = 4;
	int bit = 1;
	unsigned int c;

	for(c=data; c!=0 || count <= errorBitLoc; c>>=1, bit<<=1, count++)
	{
			// Skip powers of 2 for check bits
			if(count == power2)
			{
					count++;
					power2 = 2*power2;
			}

			if(count == errorBitLoc)
			{
				// Flip the bit
				if(!(c & 01))
					correctData |= bit;
			}
			else if( c & 01)
			{
				correctData |= bit;
			}
	}

	//cout << " Retrieved Data: "<< data << " Corrected Data : " << correctData << endl;

	*static_cast<int*>(memLoc) = correctData;
}

void correctDataBit(void *memLoc, string type, unsigned int errorBitLoc)
{
	if(type.compare("int") == 0)
		correctIntDataBit(memLoc, errorBitLoc);
	else if(type.compare("float") == 0)
		correctIntDataBit(memLoc, errorBitLoc);
}

void applySECDED(void *memLoc, string type, unsigned short int retrievedCheckBits, bool retrievedParity ,unsigned short int calCheckBits, bool calParity)
{
	bool isParityEqual = (retrievedParity == calParity);
	bool isCheckBitsEqual = (retrievedCheckBits == calCheckBits);

	/* Data is correct */
	if( isParityEqual && isCheckBitsEqual )
	{
		//cout << " Data is correct " << endl;
		return;
	}
	else if( isParityEqual && !isCheckBitsEqual)
	{
		/* 2 bit error */
		cerr << " 2-bit Error Detected. Correction not possible. Memory Location: " << memLoc << endl;
		exit(0);
	}
	else
	{
		cout << " Data is incorrect. Correcting using error correction algorithm " << endl;
		/* Parity is incorrect, 1-bit error, Correct it using checkbits */
		int errorBitLoc = (int)(retrievedCheckBits ^ calCheckBits);
		correctDataBit(memLoc, type, errorBitLoc);
	}

}

