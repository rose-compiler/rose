// test function overloading:

int x;

void goo() {}

void goo(int g) {}

int moo() {
	void zoo();
}

void zoo() {
	int t;
}


int main() {

	int x;

	int goo;

	//float goo(float f) ;

	return 0;

}

/*
NR. OF DIFFERENT HIDDEN SYMBOLS: 2
Symbol: x with adress(es) : Hidden entry x; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0x8b85858 / Valid: 1
Symbol: goo with adress(es) : Hidden entry goo; with SgSymbol-ClassName SgFunctionSymbol with SgSymbol-Address: 0x8b22988 / Valid: 1
Hidden entry goo; with SgSymbol-ClassName SgFunctionSymbol with SgSymbol-Address: 0x8b229a0 / Valid: 1
 -> Number of entries in hidden list: 3
*/
