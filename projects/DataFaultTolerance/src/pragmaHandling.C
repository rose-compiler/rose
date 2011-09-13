/*
 * PragmaHandling.C
 *
 *  Created on: Jun 1, 2011
 *      Author: sharma9
 */

#include <vector>
#include <string>
#include "AstFromString.h"

using namespace std;
using namespace SageBuilder;
using namespace SageInterface;
using namespace AstFromString;

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
	int dimLB[10], dimUB[10];
	int noDims;
};

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
 * End of ArrayList Class
 */

/*
 * Parses the chunksize argument
 */
void parseChunksizeValue(ArrayList* arrayList)
{
	SgName* name = isSgName(c_parsed_node);

	if(name->getString().compare("Chunksize") == 0)
	{
		if (afs_match_char('='))
		{
			if (afs_match_constant())
			{
				SgIntVal* intVal = isSgIntVal(c_parsed_node);
				arrayList->setChunksize(intVal->get_value());
			}
			else
			{
				cerr << " Expecting int value in chunksize " << endl;
				exit(1);
			}
		}
		else
		{
			cerr << " Expecting = after chunksize " << endl;
			exit(1);
		}
	}
	else
	{
		cerr << " Expecting Chunksize argument " << endl;
		exit(1);
	}
}

/*
 * Parses the array dimensions
 */
void parseArrayDimension(ArrayList* arrayList, ArrayInfo* arrayInfo, string variableName, int index, bool* moreDimensions)
{
	arrayInfo->incrementNoDims();
	if (afs_match_constant())
	{
		SgIntVal* intVal = isSgIntVal(c_parsed_node);
		arrayInfo->setDimLB(index, intVal->get_value());

		if (afs_match_char(':'))
		{
			if (afs_match_constant())
			{
				SgIntVal* intVal = isSgIntVal(c_parsed_node);
				arrayInfo->setDimUB(index, intVal->get_value());

				if (afs_match_char('>'))
				{
					if (afs_match_char('<'))
					{
						*moreDimensions = true;
					}
					else if (afs_match_char(')'))
					{
						*moreDimensions = false;
						arrayList->insertArrayInfo(arrayInfo);
					}
					else
					{
						cerr << " Expecting end of dimension for array :" << variableName << endl;
						exit(1);
					}
				}
				else
				{
					cerr << " Expecting > for array :" << variableName << endl;
					exit(1);
				}
			}
			else
			{
				cerr << " Expecting integer constant as dimension for variable " << variableName << endl;
				exit(1);
			}

		}
		else
		{
			cerr << " Expecting dimension information after : for variable " << variableName << endl;
			exit(1);
		}
	}
	else
	{
		cerr << " Dimension not specified for variable " << variableName << endl;
		exit(1);
	}
}

/*
 *  Parses the array information
 */
void parseArrayInfo(ArrayList* arrayList)
{
	SgVarRefExp* varRefExp = isSgVarRefExp(c_parsed_node);
	SgVariableSymbol* varSymbol = varRefExp->get_symbol();
	string variableName = varSymbol->get_name().getString();

	if (afs_match_char(':'))
	{
		bool moreDimensions = true;
		int index = 0;

		if (afs_match_char('<'))
		{
			ArrayInfo* arrayInfo = new ArrayInfo(variableName);
			while(moreDimensions)
			{
				parseArrayDimension(arrayList, arrayInfo , variableName, index, &moreDimensions);
				index++;
			}
		}
		else
		{
			cerr << " Expecting dimension information (<>) for variable " << variableName << endl;
			exit(1);
		}
	}
}

/*
 * This method recognizes the pragma for memory fault tolerance
 */
bool recognizePragma(SgPragmaDeclaration *pragmaDec, ArrayList* arrayList) {

	string pragmaStr = pragmaDec->get_pragma()->get_pragma();

	c_sgnode = getNextStatement(pragmaDec);
	assert(c_sgnode != NULL);

	c_char = pragmaStr.c_str();

	if (afs_match_substr("mem_fault_tolerance"))
	{
		arrayList->setChunksize(1); // Set Default Chunk size

		while (afs_match_char('('))
		{
			if (afs_match_identifier())
			{
				if(isSgName(c_parsed_node))
				{
					parseChunksizeValue(arrayList);
				}
				else if(isSgVarRefExp(c_parsed_node))
				{
					parseArrayInfo(arrayList);
				}
				else
				{
					cerr << " Unknown value in pragma " << endl;
					exit(1);
				}

			}
		}
		return true;
	}


	return false;
}
