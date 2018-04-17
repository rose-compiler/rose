/*Test code for examine sizeof operations on different data types.*/
#include <cstdio>
#include <iostream>
#include <vector>
#include <string.h>

using namespace std;

//sample class
class Test 
{
protected:
	double doubleMember;
public:
	static double staticDoubleMember;
	int intMember;
	float floatMember;
	Test();
	virtual int intMemberFunction();
	virtual char* charMemberFunction(); 
};

double Test::staticDoubleMember = 2.0;

Test::Test()
{
	intMember = 0;
	floatMember = 0.0f;
}

int Test::intMemberFunction()
{
	return 0;
}

char* Test::charMemberFunction()
{
	return "a";
}

//sample derived class
class DerivedTest: public Test
{
public:
	DerivedTest();
	virtual void voidMemberFunction();
};

DerivedTest::DerivedTest()
{

}

void DerivedTest::voidMemberFunction()
{

}

//template sample
template <class StackType> class stack
{
	StackType stck[49];
	int tos;
public:
	stack() { tos = 0; }
	void push(StackType object);
	StackType pop();
};

template <class StackType> void stack<StackType>::push(StackType object)
{
	if(tos == 10)
	{
		cout << "Stack is full.\n";
		return;
	}
	stck[tos] = object;
	tos++;
}

template <class StackType> StackType stack<StackType>::pop()
{
	if(tos == 0)
	{
		cout << "Stack is empty.\n";
		return NULL;
	}
	tos--;
	return stck[tos];
}

//sample struct to test struct members and struct pointer
struct structTest
{
	int intMember;
	short int* shortIntPointerMember;
	float floatArrayMember[10];
};

struct structWithBit
{
	int normalMember;
	unsigned firstPart: 2;
	unsigned secondPart: 3;
};

//struct with #pragma pack directive
#pragma pack(1)
struct structWithPack
{
	int intMember;
	char charMember;
	float floatMember;
};


#pragma pack(1)
//nested struct with #pragma pack directive
struct outterStruct
{
	int intMember;
	float floatMember;
	char unpackMember;
#pragma pack(2)
	struct innerStruct
	{
		char charMember;
		double doubleMember;
	}inner;
};

//Restore the default pack mechanism of the system
#pragma pack()

//sample union
union unionTest
{
	int intMember;
	float floatMember;
	double doubleMember;
};

//sample function to test function pointer
int max(int array[], int length)
{
	int max = array[0];
	for(int i = 0; i < length; i++)
	{
		if(array[i] > max)
			max = array[i];
	}
	return max;
}

//Class with packed struct members
class ClassWithPackMember
{
public:
	struct structWithPack packMember;
	struct outterStruct nestedMember;
	ClassWithPackMember() {}
	int memberFunction();
};

int ClassWithPackMember::memberFunction()
{
	return -1;
}

int main(int argc, char *argv[]) 
{
	FILE *fp;
	fp = fopen("test", "w");

	//function pointer
	int (*p) (int[], int);
	p = max;
	int a[3] = {1, 2, 3};
	(*p)(a, 3);

	//class and class pointers
	Test baseClass;
	Test* baseClassPointer = &baseClass;
	baseClassPointer -> intMember = 2;
	(baseClassPointer -> intMember)++;

	DerivedTest derivedClass;
	DerivedTest* derivedClassPointer = &derivedClass;

	//template class test
	stack<char> charStackSample;
	stack<int> intStackSample;

	//class member pointers
	int* intMemberPointer = &(baseClass.intMember);
	float* floatMemberPointer = &(derivedClassPointer -> floatMember);
	*intMemberPointer += 2;
	*floatMemberPointer += 1.0f;

	//struct and struct pointers
	struct structTest structSample;
	struct structTest* structPointer = &structSample;
	structSample.intMember = 2;
	structPointer -> floatArrayMember[0] = 1.0f;

	//struct with bit field
	struct structWithBit structBitSample;
	struct structWithBit* structBitPointer = &structBitSample;
	structBitSample.normalMember = 2;
	structBitSample.firstPart = 1;
	structBitPointer -> secondPart = 3;

	//struct following #pragma pack directive
	struct structWithPack structPackSample;
	struct structWithPack* structPackPointer;

	//nested struct with #pragma pack directive
	struct outterStruct nestedStructSample;
	struct outterStruct* nestedStructPointer;

	//union test
	union unionTest unionSample;
	union unionTest* unionPointer = &unionSample;
	
	//arrays
	int intArray[10];
	float* pointerArray[20];
	Test objectArray[5];
	int twoDimArray[5][5];

	//enumerations
	enum coin {penny, nickel, dime, quarter, half_dollar, dollar};
	enum coin money;

	//strings
	char *string = "This is a sizeof test of string.";
	char str[] = "This is a sizeof test of string in a char array.";
	strcmp(string, str);

	//class with packed struct member
	ClassWithPackMember packMemberClassSample;
	
	//STL sample
	vector<char> charVector(10);
	vector<Test> classVector(5);
	vector<structWithPack> structVector(10);

	//primitive data types
	fprintf(fp, "%d ", sizeof(unsigned char));
	fprintf(fp, "%d ", sizeof(char));
	fprintf(fp, "%d ", sizeof(short int));
	fprintf(fp, "%d ", sizeof(unsigned short));
	fprintf(fp, "%d ", sizeof(int));
	fprintf(fp, "%d ", sizeof(unsigned int));
	fprintf(fp, "%d ", sizeof(long int));
	fprintf(fp, "%d ", sizeof(unsigned long int));
	fprintf(fp, "%d ", sizeof(long long int));
	fprintf(fp, "%d ", sizeof(unsigned long long int));
	fprintf(fp, "%d ", sizeof(float));
	fprintf(fp, "%d ", sizeof(double));
	fprintf(fp, "%d ", sizeof(long double));

	//pointer type
	fprintf(fp, "%d ", sizeof(void*));
	fprintf(fp, "%d ", sizeof(char*));
	fprintf(fp, "%d ", sizeof(double*));
	fprintf(fp, "%d ", sizeof(p));
	
	//class type
	fprintf(fp, "%d ", sizeof(Test));
	fprintf(fp, "%d ", sizeof(DerivedTest));
	fprintf(fp, "%d ", sizeof(Test::staticDoubleMember));
	fprintf(fp, "%d ", sizeof(baseClass));
	fprintf(fp, "%d ", sizeof(baseClassPointer));
	fprintf(fp, "%d ", sizeof(derivedClass));
	fprintf(fp, "%d ", sizeof(derivedClassPointer));
	fprintf(fp, "%d ", sizeof(intMemberPointer));
	fprintf(fp, "%d ", sizeof(floatMemberPointer));
	fprintf(fp, "%d ", sizeof(*intMemberPointer));

	//template class
	fprintf(fp, "%d ", sizeof(charStackSample));
	fprintf(fp, "%d ", sizeof(intStackSample));
	charStackSample.push('a');
	fprintf(fp, "%d ", sizeof(charStackSample));
	intStackSample.push(4);
	fprintf(fp, "%d ", sizeof(intStackSample));
	fprintf(fp, "%d ", sizeof(stack<char>));
	fprintf(fp, "%d ", sizeof(stack<int>));

	//struct
	fprintf(fp, "%d ", sizeof(structSample));
	fprintf(fp, "%d ", sizeof(structPointer));
	fprintf(fp, "%d ", sizeof(*structPointer));
	fprintf(fp, "%d ", sizeof(structSample.intMember));
	fprintf(fp, "%d ", sizeof(structTest));
	fprintf(fp, "%d ", sizeof(structPointer -> floatArrayMember));

	//struct with bit field
	fprintf(fp, "%d ", sizeof(structWithBit));
	fprintf(fp, "%d ", sizeof(structBitSample));
	fprintf(fp, "%d ", sizeof(structBitSample.normalMember));
	fprintf(fp, "%d ", sizeof(structBitPointer));

	//struct with #pragma pack()
	fprintf(fp, "%d ", sizeof(structWithPack));
	fprintf(fp, "%d ", sizeof(structPackSample));
	fprintf(fp, "%d ", sizeof(structPackPointer));
	fprintf(fp, "%d ", sizeof(structPackSample.intMember));
	fprintf(fp, "%d ", sizeof(structPackPointer -> floatMember));

	//nested struct
	fprintf(fp, "%d ", sizeof(outterStruct));
	fprintf(fp, "%d ", sizeof(nestedStructSample));
	fprintf(fp, "%d ", sizeof(nestedStructSample.inner));
	fprintf(fp, "%d ", sizeof(nestedStructSample.intMember));
	fprintf(fp, "%d ", sizeof((nestedStructPointer -> inner).charMember));

	//union
	fprintf(fp, "%d ", sizeof(unionTest));
	fprintf(fp, "%d ", sizeof(unionSample));
	fprintf(fp, "%d ", sizeof(unionPointer));
	unionSample.intMember = 3;
	fprintf(fp, "%d ", sizeof(unionSample));
	unionPointer -> floatMember = 1.5f;
	fprintf(fp, "%d ", sizeof(unionSample));
	unionSample.doubleMember = 2.4;
	fprintf(fp, "%d ", sizeof(unionSample));
	
	//array
	fprintf(fp, "%d ", sizeof(intArray));
	fprintf(fp, "%d ", sizeof(intArray[2]));
	fprintf(fp, "%d ", sizeof(pointerArray));
	fprintf(fp, "%d ", sizeof(pointerArray[3]));
	fprintf(fp, "%d ", sizeof(*pointerArray[4]));
	fprintf(fp, "%d ", sizeof(objectArray));
	fprintf(fp, "%d ", sizeof(objectArray[3]));
	fprintf(fp, "%d ", sizeof(twoDimArray));
	fprintf(fp, "%d ", sizeof(twoDimArray[0]));
	fprintf(fp, "%d ", sizeof(twoDimArray[2][2]));

	//enumerations
	fprintf(fp, "%d ", sizeof(coin));
	fprintf(fp, "%d ", sizeof(penny));
	fprintf(fp, "%d ", sizeof(money));

	//strings
	fprintf(fp, "%d ", sizeof(string));
	fprintf(fp, "%d ", sizeof(*string));
	fprintf(fp, "%d ", sizeof(*(string + 2)));
	fprintf(fp, "%d ", sizeof(str));
	fprintf(fp, "%d ", sizeof(str[0]));
	
	//class with packed struct member
	fprintf(fp, "%d ", sizeof(ClassWithPackMember));
	fprintf(fp, "%d ", sizeof(packMemberClassSample));
	fprintf(fp, "%d ", sizeof(packMemberClassSample.packMember));
	fprintf(fp, "%d ", sizeof(packMemberClassSample.nestedMember));
	fprintf(fp, "%d ", sizeof(packMemberClassSample.nestedMember.inner));
	
	//STL samples
	fprintf(fp, "%d ", sizeof(vector<char>));
	fprintf(fp, "%d ", sizeof(vector<structWithPack>));
	fprintf(fp, "%d ", sizeof(charVector[0]));
	fprintf(fp, "%d ", sizeof(classVector[1].intMember));
	fprintf(fp, "%d ", sizeof(structVector[2].charMember));

	//file
	fprintf(fp, "%d ", sizeof(fp));
	fprintf(fp, "%d ", sizeof(*fp));

	//other
	fprintf(fp, "%d ", sizeof(argc));
	fprintf(fp, "%d ", sizeof(argv));
	fprintf(fp, "%d ", sizeof(*argv));
	fclose(fp);
	return 0;
}
