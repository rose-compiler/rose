// test functions

int x;

int main() {

	double wwwwwwwwwwww;
	int x;
	int foo = 45;

	{

		int foo();

		int a, b;

		// a = foo; // will cause an error
		a = foo();


	}

	int a;

	//int goo();

	return 0;

}

int foo() { return 10; }

/*
NR. OF DIFFERENT HIDDEN SYMBOLS: 1
Symbol: x with adress(es) : Hidden entry x; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0x8b1d320 / Valid: 1
 -> Number of entries in hidden list: 1
 
additional List of Symbols hidden by a FUNCTION (Scope: )
NR. OF DIFFERENT HIDDEN SYMBOLS: 1
Symbol: foo with adress(es) : Unaccessible entry foo; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0x8b1d368 / Valid: 1
 -> Number of entries in hidden list: 1
*/