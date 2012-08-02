// 8 matches

int foo() {}
int (*pt2foo)() = foo; /* (+2) SgInitializedName, SgAssignInitializer */
typedef int(*t_pt2foo)(); /* (+1) function pointer typedef */

t_pt2foo bar(int x); /* (+1) SgFunctionDefinition > SgFunctionDeclaration */

t_pt2foo bar(int x) {return foo;} /* (+2) return; return value */

int* numPtr(int* i) { return i; }

int main () {
	(*pt2foo)(); /* (+1) SgVarRefExp */

	1 + (*bar(1))(); /* (+1):SgFunctionCallExp (function)> SgPointerDerefExp */


	/*  !! TODO: Unable to detect !!

	     Same AST:

	     SgExprStatement > SgFunctionCallExp
    */
	(*(&foo))(); 
	foo();


	// ensure no false-positives
	int i = 5;
	9 + (*numPtr(&i));
}
