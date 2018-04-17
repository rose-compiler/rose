// test for enums, 1

int x;

enum E1 {

	A, B, C, D

};

enum E2 {

	y, z

} enum_var_1;

int main() {

	double x, E1;

        float A;

        typedef int enum_var_1;

	return 0;

}

/*
NR. OF DIFFERENT HIDDEN SYMBOLS: 3
Symbol: E1 with adress(es) : Hidden entry E1; with SgSymbol-ClassName SgEnumSymbol with SgSymbol-Address: 0x9c00c70 / Valid: 1
Symbol: enum_var_1 with adress(es) : Hidden entry enum_var_1; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0x9bef338 / Valid: 1
Symbol: x with adress(es) : Hidden entry x; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0x9bef320 / Valid: 1
 -> Number of entries in hidden list: 3
*/
