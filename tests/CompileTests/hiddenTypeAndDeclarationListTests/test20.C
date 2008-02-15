// test for enums, 2

int x;

int main() {

	double x, E1;

        float A;

        typedef int E3;

	{

                enum E3 { A, y, u, v, w };

                int integer = A;
                integer = y;
               // integer = z; // error, z undeclared

               void enum_var_1();

               float z();

               int E2();

	}

        enum E2 { y, z } enum_var_1;

	return 0;
}

enum E1 { A, B, C, D };

/*
NR. OF DIFFERENT HIDDEN SYMBOLS: 1
Symbol: x with adress(es) : Hidden entry x; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0x821f320 / Valid: 1
 -> Number of entries in hidden list: 1
 
NR. OF DIFFERENT HIDDEN SYMBOLS: 2
Symbol: E3 with adress(es) : Unaccessible entry E3; with SgSymbol-ClassName SgTypedefSymbol with SgSymbol-Address: 0x8173060 / Valid: 1
Symbol: A with adress(es) : Unaccessible entry A; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0x821f368 / Valid: 1
 -> Number of entries in hidden list: 2
*/