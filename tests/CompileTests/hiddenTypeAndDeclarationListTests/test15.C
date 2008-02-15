// test for function overloading outside a class

class foo{

	public:

		int t;

		foo() {}

		int moo(int mmmmmmmmmmm) {
			int y;
			void t(); // "normal" (not a member function) function hides a member of a class
			return y;
		}

};

void foo();

int foo(int x);

double foo(double wwwwwwwww);

int main() {

	long int foo(long int li);

	// foo s(); // will cause an error
	class foo s(); // only this is possible -> elaboration mechanism!!

	return 0;

}

/*
additional List of Symbols hidden by a FUNCTION (Scope: )
NR. OF DIFFERENT HIDDEN SYMBOLS: 1
Symbol: t with adress(es) : Hidden entry t; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0x969e888 / Valid: 1
 -> Number of entries in hidden list: 1
 
additional List of Symbols hidden by a FUNCTION (Scope: )
NR. OF DIFFERENT HIDDEN SYMBOLS: 1
Symbol: foo with adress(es) : Hidden entry foo; with SgSymbol-ClassName SgClassSymbol with SgSymbol-Address: 0x9698ac0 / Valid: 1
 -> Number of entries in hidden list: 1
 
additional List of Symbols hidden by a FUNCTION (Scope: )
NR. OF DIFFERENT HIDDEN SYMBOLS: 1
Symbol: foo with adress(es) : Hidden entry foo; with SgSymbol-ClassName SgClassSymbol with SgSymbol-Address: 0x9698ac0 / Valid: 1
 -> Number of entries in hidden list: 1
 
additional List of Symbols hidden by a FUNCTION (Scope: )
NR. OF DIFFERENT HIDDEN SYMBOLS: 1
Symbol: foo with adress(es) : Hidden entry foo; with SgSymbol-ClassName SgClassSymbol with SgSymbol-Address: 0x9698ac0 / Valid: 1
 -> Number of entries in hidden list: 1

additional List of Symbols hidden by a FUNCTION (Scope: )
NR. OF DIFFERENT HIDDEN SYMBOLS: 1
Symbol: foo with adress(es) : Hidden entry foo; with SgSymbol-ClassName SgClassSymbol with SgSymbol-Address: 0x9698ac0 / Valid: 1
 -> Number of entries in hidden list: 1
*/
