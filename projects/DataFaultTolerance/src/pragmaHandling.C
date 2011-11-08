/*
 * PragmaHandling.C
 *
 *  Created on: Jun 1, 2011
 *      Author: sharma9
 */

#include <vector>
#include <string>
#include "stdlib.h"
#include "AstFromString.h"
#include "arrayBase.h"

using namespace std;
using namespace SageBuilder;
using namespace SageInterface;
using namespace AstFromString;




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
