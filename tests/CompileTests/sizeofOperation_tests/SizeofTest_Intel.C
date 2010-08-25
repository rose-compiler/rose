/*Test for sizeof operations using Intel C++ compiler.*/
#include "rose.h"
#include <iostream>
#include <cstdio>
#include <string>

using namespace std;

FILE *fp;
bool flag = true;

class visitorTraversal : public AstSimpleProcessing
{
	public:
		visitorTraversal();
		virtual void visit(SgNode* currentNode);
}; 

visitorTraversal::visitorTraversal()
{

}

void visitorTraversal::visit(SgNode* currentNode)
{
	if(isSgSizeOfOp(currentNode) != NULL)
	{
		SgNode* parent = currentNode -> get_parent();
		ROSE_ASSERT(parent != NULL);
		//sizeof operation with an extra IR node (SgUnsignedIntVal type node)
		if(isSgUnsignedIntVal(parent) != NULL)
		{
			cout << "Testing sizeof operation on " << currentNode -> unparseToString() << ": ";
			//computedSize is the value in the generated IR node.
			unsigned int computedSize = ((SgUnsignedIntVal*)parent) -> get_value();
			//directSize is the actual value from the program execution.
			unsigned int directSize;
			fscanf(fp, "%d", &directSize);
			if(directSize == computedSize)
			{
				cout << " EQUAL: " << directSize << " and " << computedSize << endl;
			}
			else
			{
				cout << "\n DIFFERENT! Compile time size is " << directSize 
                                         << " and Computed size is " << computedSize << "\n" << endl;
                                flag = false;
			}
		}
	}
}

int main(int argc, char* argv[])
{
	SgProject* project = frontend(argc, argv);
	ROSE_ASSERT(project != NULL);
	cout << "Running sizeof operation tests...\n";
	system("rm -f test");
	//Run the input file first to get all the sizeof results and write them into a file.
	string command("icc -o inputCode_SizeofTest ");
	command += argv[2];
	int result = system(command.c_str());
	if(result != 0)
	{
		cout << "Intel compiler not found or not executed. Test aborted.\n";
		return 0;
	}
	result = system("./inputCode_SizeofTest");
	if(result != 0)
	{
		cout << "Input file corruption. Test aborted.\n";
		return 0;
	}
	//Read the file serially and compare each sizeof result with the value in the extra IR node.
	fp = fopen("test", "r");
	if(fp == NULL)
	{
		cout << "Test result file not generated or not found. Test aborted.\n";
		return 0;
	}
	visitorTraversal findSizeOf;
	findSizeOf.traverseInputFiles(project, preorder);
	fclose(fp);
	if(flag == true)
	{
		cout << "All tests are passed.\n";
	}
	else
	{
		cout << "Differences in sizeof operation found.\n" << endl;
	}
	return 0;
}
