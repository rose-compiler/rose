#include <iostream>

using namespace std;

void printBits(unsigned int x)
{
	
	for(; x!=0; x>>=1)
		if(x & 01)
		{
			cout << "1";
		}
		else
		{
			cout << "0";
		}
	cout << endl;
}

unsigned int calCheckBits(int x)
{
	unsigned int checkbits = 0, c;
	// Skip 1 and 2 position for check bits
	int count=3;
	int power2 = 2;
	
	for( c=x; c!=0; c>>=1, count++)
	{
		if(count == 2*power2)
		{
			count++;
			power2 = 2*power2;
		}
		if( c & 01 )
		   checkbits ^= count;	
	}

	printBits(checkbits);

	return checkbits;
}

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

void calParity(int data, unsigned int checkbits)
{
	int count = countOneBits(data);
	count += countOneBits(checkbits);
	
	if(count%2==0)
		cout << " Parity 0 " << endl;
	else
		cout << " Parity 1 " << endl;
}

void leftshift()
{
	int bit=1;

	bit<<=5;
	cout << " Left shift: " << bit << endl;
}

void correctIntDataBit(int data, int errorBitLoc)
{
	//int data = *static_cast<int*>(memLoc);;
	int correctData = 0;

	int count=3;
	int power2 = 4;
	int bit = 1;
	unsigned int c;
	
	cout << " Data: " << data << " Errorloc: " << errorBitLoc << endl;

	for(c=data; c!=0 || count <= errorBitLoc; c>>=1, bit<<=1, count++)
	{
			cout << " Bit: " << bit << " count: " << count << endl;
			//correctData |= bit;
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
				{
					correctData |= bit;
					//cout << " Bit: " << bit << endl;
				}
			}
			else if( c & 01)
			{
				correctData |= bit;
				cout << " Bit: " << bit << endl;
			}
	}

	cout << " Retrieved Data: "<< data << " Corrected Data : " << correctData << endl;

}


int main()
{

	int a=1301;
	leftshift();
	printBits(a);
	unsigned int check = calCheckBits(a);
	calParity(a, check);
	int a_flip = 1300;
	unsigned int check_flip = calCheckBits(a_flip);
	calParity(a_flip, check_flip);
	if( check != check_flip )
	{
	  cout << " Check bits do not match " << endl;
	  cout << " Error bit is : " << (check^check_flip) << endl;
	  correctIntDataBit(a_flip, (check^check_flip) );
	}
}
