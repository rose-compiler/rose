// test for using declarations for classes/structs: 3

class A {

        public:

		int x;

		enum c { i };

};

class B : A {

        public:

		using A::x;

		using A::i;

		int y;

		int foo();

};

int B::foo() {

        int wwwwww, x, c, i;

        return 1;

}

int main() {



        return 0;

}

/*
NR. OF DIFFERENT HIDDEN SYMBOLS: 3
Symbol: c with adress(es) : Hidden entry c; with SgSymbol-ClassName SgEnumSymbol with SgSymbol-Address: 0x91dc218 / Valid: 1
Symbol: i with adress(es) : Hidden entry i; with SgSymbol-ClassName SgEnumFieldSymbol with SgSymbol-Address: 0x91e2000 / Valid: 1
Symbol: x with adress(es) : Hidden entry x; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0x91ca870 / Valid: 1
 -> Number of entries in hidden list: 3
*/
