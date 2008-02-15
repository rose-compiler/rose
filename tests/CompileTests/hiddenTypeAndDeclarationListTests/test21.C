// test namespaces:

namespace A {

	int x;

        class C {

        	public:

                	int u, v, w;

                        void foo();

        };

        struct X {

        	int s, t;

        };

        enum E1 { en11, en12 };

        void goo();

}

namespace B {

	enum E2 { en21, en22 };

        class D { public: int d; };

        void roo() {}

        int b;

        typedef int MyInt;

}

using namespace A;

int main() {

	int x, C, X, E1, en11, goo;

   class C c_var;
   struct X x_var;
   enum E1 e1_enum_var;

       // using B::E2; // causes error :
/*
Building a sourcePositionData using the type in sage_gen_tag_reference()
ERROR: Default reached in sage_gen_using_declaration(): symbol = SgEnumSymbol
HiddenList: ../../../../NEW_ROSE_MAY7/src/frontend/EDG_SAGE_Connection/sage_gen_be.C:33608: SgUsingDeclarationStatement* sage_gen_using_declaration(a_using_decl*): Assertion `false' failed.
Aborted
*/

        using B::b;

        using B::D;

        using B::roo;

        using B::MyInt;

        {
        	int b, B, E2, en21, D, roo, MyInt;

        }

	return 0;

}

void foo()
{
   class Y {};
   typedef A::X stack_Y;
   {
      class Y {};
      typedef Y stack_Y;
      Y y1;
      class Y y2;
   }
}

