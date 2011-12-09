// 4 matches
// - Array of function pointers (from typedef)
// - typedef for variable declaration and initialization
// ---------------------------------------------------------------------------
//
// Relevant section: cast to function reference
//
// 	SgPointerType > SgFunctionType
//
//	SgAssignInitializer (operand_i)> SgFunctionRefExp
//

int a(int x) {return x;}
int main () {
	// 1
	// SgTypedefDeclaration (base_type)> SgPointerType (base_type)> SgFunctionType
	typedef int(*t_pt2a)(int); /* (+1) SgTypedefDeclaration */

	// 2: SgInitializedName (type)> SgArrayType (base_type)> SgTypedefType
	// SgVariableDeclaration > SgInitializedName > SgAssignInitializer > SgFunctionRefExp
	t_pt2a pt2a[10]; /* (+1) SgInitializedName */

	t_pt2a* pt2a2[10]; /* (+1) SgInitializedName */

	t_pt2a**************** pt2a3[10]; /* (+1) SgInitializedName */
}
