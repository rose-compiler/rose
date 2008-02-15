int x;

typedef int MyInt; // causes a valid typedef symbol of double MyInt
typedef double MyDouble;

namespace Y { int x; int moo(); }

namespace Z { using namespace Y; int z; } // test transitive usement of namespaces

class B { public: int b; private: int a;};

class A : B { // test inheritance

	public:



		int a;

		int foo() {
			int x = a;
			int y;
			double goo;
			float MyLongInt;
			return x;

		}

		int y; // valid class variable symbol

		int goo(); // valid class function symbol

		typedef long int MyLongInt; // valid class typedef symbol

	private:

		int b;

};

int A::goo() {

	int a, b; // test class update

	b = 1;

	int wwwwwwwwwwwwww;

	float MyUnsignedInt;

	return 0;

}

typedef unsigned int MyUnsignedInt; // causes a not valid typedef symbol of float MyUnsignedInt

using namespace Y; // causes a valid function symbol for float moo

int main() {

	using Z::x; // valid variable symbol for next scope

	int too; // valid variable symbol, for function-intersection-procedure

	{

		int x, y, z;
		float moo;
		double MyInt;
		int koo;
		void too();
		long int A(unsigned int ui); // will cause a valid Class Symbol in function-intersection-procedure
		float MyDouble(int blabla); // will cause a valid Typedef Symbol in function-intersection-procedure

	}

	int z; // not valid Variable Symbol for former scope

	int MyDouble;

	float koo(); // not valid function Symbol

	return 0;

}

/* proofed output from HiddenList program on April 16, 2007 under ROSE Version: pre-release alpha version: 0.8.10a

Symbol: x with adress(es) : Hidden entry x; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0x9a4d068
 -> Number of entries in hidden list: 1

Symbol: b with adress(es) : Hidden entry b; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0x9a4d140
 -> Number of entries in hidden list: 1

Symbol: MyLongInt with adress(es) : Hidden entry MyLongInt; with SgSymbol-ClassName SgTypedefSymbol with SgSymbol-Address: 0x84da09
Symbol: x with adress(es) : Hidden entry x; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0x9a4d068
Symbol: y with adress(es) : Hidden entry y; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0x9a4d128
Symbol: goo with adress(es) : Hidden entry goo; with SgSymbol-ClassName SgMemberFunctionSymbol with SgSymbol-Address: 0x909dbc8
 -> Number of entries in hidden list: 4

Symbol: a with adress(es) : Hidden entry a; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0x9a4d0e0
Symbol: b with adress(es) : Hidden entry b; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0x9a4d0b0
Hidden entry b; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0x9a4d140
 -> Number of entries in hidden list: 3

Symbol: MyDouble with adress(es) : Hidden entry MyDouble; with SgSymbol-ClassName SgTypedefSymbol with SgSymbol-Address: 0x85dc078
 -> Number of entries in hidden list: 1

Symbol: moo with adress(es) : Hidden entry moo; with SgSymbol-ClassName SgFunctionSymbol with SgSymbol-Address: 0x99eab98
Symbol: x with adress(es) : Hidden entry x; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0x9a4d068
Hidden entry x; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0x9a4d080
Symbol: MyInt with adress(es) : Hidden entry MyInt; with SgSymbol-ClassName SgTypedefSymbol with SgSymbol-Address: 0x99a1a10
 -> Number of entries in hidden list: 4

additional List of Symbols hidden by a FUNCTION (Scope: )
Symbol: too with adress(es) : Unaccessible entry too; with SgSymbol-ClassName SgVariableSymbol with SgSymbol-Address: 0x86884b8
 -> Number of entries in hidden list: 1
 
additional List of Symbols hidden by a FUNCTION (Scope: )
Symbol: A with adress(es) : Hidden entry A; with SgSymbol-ClassName SgClassSymbol with SgSymbol-Address: 0x86a6c28
 -> Number of entries in hidden list: 1

additional List of Symbols hidden by a FUNCTION (Scope: )
Symbol: MyDouble with adress(es) : Hidden entry MyDouble; with SgSymbol-ClassName SgTypedefSymbol with SgSymbol-Address: 0x85dc078
 -> Number of entries in hidden list: 1
*/
