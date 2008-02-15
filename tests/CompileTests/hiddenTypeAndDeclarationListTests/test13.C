// test for "attributing" entries of entries of VscopeStack

int main() {

	int a;
	{
		int x; // -> x number 2
		{
			// after intersecting the current symbol table (b,x,y) with the current (not valid!!) scope (a,x,x,y)
			//  we find out that only x number 2 is valid by running through the valid scope (set<SgDeclarationsStatement)
			//   -> !! mark it as valid for the intersection
			int b, x, y;
		}
		int y;
		{
			int x; // after interecting need not verify validity of x number 2 !!! (so don't need to run through the set)
			double b;
		}
	}
	int x; // -> x number 1

	return 0;

}

/*
NR. OF DIFFERENT HIDDEN SYMBOLS: 1
Symbol: x with adress(es) : Unaccessible entry x; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0x8334bf0 / Valid: 1
 -> Number of entries in hidden list: 1
 
NR. OF DIFFERENT HIDDEN SYMBOLS: 1
Symbol: x with adress(es) : Unaccessible entry x; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0x8334bf0 / Valid: 1
 -> Number of entries in hidden list: 1
*/

