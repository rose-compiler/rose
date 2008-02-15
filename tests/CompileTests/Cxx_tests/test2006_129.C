/*
Hi Dan,

If a default argument to a nondefining function declaration is a temporary
instantiation via the object's constructor, and it is the last declaration
in the program, then I get the following assertion error:

identityTranslator: ../../../../ROSE/src/frontend/EDG_SAGE_Connection/sage_gen_be.C:19287: SgExpression* sage_gen_expr(an_expr_node*, int): Assertion `curr_source_sequence_entry != __null' failed.

This seems to occur for both implicit and explicit constructors.

Here is a sample code demonstrating the bug:

------------------------------------------------------------
class A{

     public:

          A() {}
          A(int x, int y, int z) {}

};

class B {};

B x;

void foo(A a = A(1, 2, 3));
void foo(B b = B());
void bar(B b = B(x));
//void dummy();
//void dummy() {}
//int dummy;
//class dummy;
//class dummy {};
//enum dummy { one, two };
//typedef int dummy;
//namespace dummy {}
//using namespace std;
//template <typename T> class dummy {};
------------------------------------------------------------

If you leave uncommented one or more of the foo/bar declarations then
the bug manifests itself; it can be suppressed by uncommenting any of
the dummy declarations.

Thanks,
-- Peter 
*/


class A{

     public:

          A() {}
          A(int x, int y, int z) {}

};

class B {};

B x;

void foo(A a = A(1, 2, 3));
void foo(B b = B());
void bar(B b = B(x));

//void dummy();
//void dummy() {}
//int dummy;
//class dummy;
//class dummy {};
//enum dummy { one, two };
//typedef int dummy;
//namespace dummy {}
//using namespace std;
//template <typename T> class dummy {};
