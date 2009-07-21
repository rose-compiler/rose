#include "test8a.h"
#include <iostream>

using namespace std;


TestClass::TestClass()
{
	cout << "Constructor Base Class" << endl;
}

TestClass::~TestClass()
{
	cout << "Destructor Base Class" << endl;	
}

int & TestClass::memberFunc1()              
{ 
	return someValue; 
}

const int & TestClass::memberFunc1() const  
{ 
	return someValue; 
}

int TestClass::memberFunc2() const
{
	//should call the const variant of memberFunc1
	return memberFunc1();
}




// -----------------------------------------------------

DerivedClass::DerivedClass()
{
	cout << "Constructor Derived Class" << endl;
}

DerivedClass::~DerivedClass()
{
	cout << "Destructor Derived Class" << endl;		
}


int DerivedClass::memberFunc3() 
{ 
	int a=4; 
	a+= TestClass::memberFunc3(); 
	return a; 
}

