// test for class access modifiers:

class M {
        public:
                int m;
};

class C {
        public:
                int cc;
};

class A {

        public:

		A() {}

		int a;

		void f1(double y);

                enum E2 { en21, en22 };

        protected:

		int b;
		int d;

		void f2();

		M em;

        private:

		int c;
		int e();

		void f3();

		C ccc;

               enum E1 { en11, en12 };

};

class B : A {

        public :

		B() {}

		int foo() {
                        double a,b,c;  // test for variables: only  a, b are hiding

                        //int z = f1(); // valid
			// f1(a); // not valid because of memberfunction int f1();

                        // A* pointer_A; // not valid because of own member double A
                }

		int f1(); // doesn't hide void f1(double y) of A, FUNCTION OVERLOADING

		unsigned int f2; // hides void f2() of A

		//double A; // hides class A
		double A(); // also valid, hides class A

		void d(); // hides A's protected d

		int e; // must not hide A's private function e

		float ccc; // must not hide A's private member ccc

		double em; // hides A's protected em

                float E1, en11, E2, en21;

};

int main() {

        return 0;

}

/*
NR. OF DIFFERENT HIDDEN SYMBOLS: 6
Symbol: E2 with adress(es) : Hidden entry E2; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0x92b3f58 / Valid: 0
Symbol: em with adress(es) : Hidden entry em; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0x92b3f10 / Valid: 0
Symbol: en21 with adress(es) : Hidden entry en21; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0x92b3f70 / Valid: 0
Symbol: A with adress(es) : Hidden entry A; with SgSymbol-ClassName SgClassSymbol with SgSymbol-Address: 0x92ae028 / Valid: 1
Symbol: d with adress(es) : Hidden entry d; with SgSymbol-ClassName SgMemberFunctionSymbol with SgSymbol-Address: 0x92e21f0 / Valid: 0
Symbol: f2 with adress(es) : Hidden entry f2; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0x92b3ec8 / Valid: 0
 -> Number of entries in hidden list: 6
 
NR. OF DIFFERENT HIDDEN SYMBOLS: 2
Symbol: a with adress(es) : Hidden entry a; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0x92b3df0 / Valid: 0
Symbol: b with adress(es) : Hidden entry b; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0x92b3e08 / Valid: 0
 -> Number of entries in hidden list: 2
*/

