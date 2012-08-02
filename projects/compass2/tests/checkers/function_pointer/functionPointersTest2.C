// 3 matches
// Function dereference as a variable initializer.
// ---------------------------------------------------------------------------
//
// Relevant section: cast to function reference
//
// 	SgCastExp > SgFunctionRefExp
//

void f(int param) {}
int main () {
// ---------------------------------------------------------------------------
// AST:
//
// 	SgAssignInitializer > SgCastExp (explicit) (operand_i)> SgFunctionRefExp
//
// ---------------------------------------------------------------------------
	// 1
	void* pt2f = &f; /* (+1) SgCastExp > SgFunctionRefExp */

	// 2
	#define t_pt2f void(*t_pt2f)(int)
	t_pt2f = &f; /* (+2) SgInitializedName, SgAssignInitializer */
}
