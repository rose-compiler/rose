// standard test

int foo;

class A {

	public:

		int a;

		A() {}

		void foo();

};

int a;
int x;

typedef int MyInt;

namespace Z {

	int x;

}

int main() {

	float MyInt;

	using Z::x;

	int goo;

	float hoo();

	{

		float x;
		double MyInt;
		double A;

		void goo();

		int hoo;

	}

	return 0;

}

/*
NR. OF DIFFERENT HIDDEN SYMBOLS: 1
Symbol: foo with adress(es) : Hidden entry foo; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0xa0b6320 / Valid: 1
 -> Number of entries in hidden list: 1
 
NR. OF DIFFERENT HIDDEN SYMBOLS: 1
Symbol: x with adress(es) : Hidden entry x; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0xa0b6368 / Valid: 1
 -> Number of entries in hidden list: 1
 
NR. OF DIFFERENT HIDDEN SYMBOLS: 1
Symbol: MyInt with adress(es) : Hidden entry MyInt; with SgSymbol-ClassName SgTypedefSymbol with SgSymbol-Address: 0xa00a060 / Valid: 1
 -> Number of entries in hidden list: 1
 
NR. OF DIFFERENT HIDDEN SYMBOLS: 4
Symbol: A with adress(es) : Hidden entry A; with SgSymbol-ClassName SgClassSymbol with SgSymbol-Address: 0xa0d4870 / Valid: 1
Symbol: x with adress(es) : Hidden entry x; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0xa0b6368 / Valid: 1
Hidden entry x; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0xa0b6380 / Valid: 1
Symbol: MyInt with adress(es) : Unaccessible entry MyInt; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0xa0b6398 / Valid: 1
Hidden entry MyInt; with SgSymbol-ClassName SgTypedefSymbol with SgSymbol-Address: 0xa00a060 / Valid: 1
Symbol: hoo with adress(es) : Hidden entry hoo; with SgSymbol-ClassName SgFunctionSymbol with SgSymbol-Address: 0xa0529a0 / Valid: 1
 -> Number of entries in hidden list: 6
 
additional List of Symbols hidden by a FUNCTION (Scope: )
NR. OF DIFFERENT HIDDEN SYMBOLS: 1
Symbol: goo with adress(es) : Unaccessible entry goo; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0xa0b63b0 / Valid: 1
 -> Number of entries in hidden list: 1
*/



