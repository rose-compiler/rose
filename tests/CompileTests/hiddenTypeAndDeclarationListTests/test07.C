// test for hiding class name

typedef int B;

class A {

        public:

		int a;

		A() {}

		void foo() {
                        int A;
			double B;
			//A* b;  // would cause an error because of overwritten class A by int A !!!! -> so ok, when int A hides constructor & class A
                }

};

int main() {

        return 0;

}

/*
NR. OF DIFFERENT HIDDEN SYMBOLS: 2
Symbol: A with adress(es) : Hidden entry A; with SgSymbol-ClassName SgMemberFunctionSymbol with SgSymbol-Address: 0x8e8c998 / Valid: 1
Hidden entry A; with SgSymbol-ClassName SgClassSymbol with SgSymbol-Address: 0x8e59ad8 / Valid: 1
Symbol: B with adress(es) : Hidden entry B; with SgSymbol-ClassName SgTypedefSymbol with SgSymbol-Address: 0x8d95060 / Valid: 1
 -> Number of entries in hidden list: 3
*/
