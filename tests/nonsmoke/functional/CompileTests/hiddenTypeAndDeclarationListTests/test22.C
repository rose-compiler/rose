// test for namespace aliases:

int x;

namespace A {

	int a;

        namespace B {

        	int b;

        }

}

// This is the namespace alias that fails
// Error: unable to lookup namespace symbol using aliasedName = B in scope = 0x2a9723b010 = SgGlobal
namespace C = A::B;

using namespace C;

int main() {

     // This needs to be commented out if we change "namespace C = A;"
        b = 2;

        int x;

        return 0;

}
