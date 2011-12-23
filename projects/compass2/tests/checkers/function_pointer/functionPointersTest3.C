// 11 matches
// - Function pointer- and reference- typedef
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

// Test for possible false positive
typedef int INT;

// 1
// Triple pointer indirection: double indirection to a funciton pointer.
typedef int(**t_pt2a2)(int); /* (+1) SgTypedefDeclaration */

// 2
// SgTypedefDeclaration (base_type)> SgPointerType (base_type)> SgFunctionType
typedef int(*t_pt2a)(int); /* (+1) SgTypedefDeclaration */

// 3
void foo(t_pt2a2 dptr) { /* (+1) SgInitializedName */
	(*(*(*dptr)))(5); /* TODO: (+1) SgVarRefExp */
}

int main () {
	// 4
	// SgVariableDeclaration > SgInitializedName > SgAssignInitializer > SgFunctionRefExp
	t_pt2a pt2a = &a; /* (+2) SgInitializedName, SgAssignInitializer */ // TODO: doesn't work now! only +1

	// 5
	// SgAssignInitializer > SgAddressOfOp > SgVarRefExp >>
	t_pt2a2 pt2a2 = &pt2a; /* (+2) SgInitializedName, Address of function-pointer-type variable */

	// 6
	void* pt2a2_2 = &pt2a; /* (+1) SgVarRefExp: Address of function-pointer-type variable */

	// 7
	foo( &pt2a ); /* (+1) SgVarRefExp: Address of function-pointer-type variable */

	// 8
	// Function reference
	typedef int(&t_ref_pt2a)(int); // (+1) SgTypedefDeclaration
}
