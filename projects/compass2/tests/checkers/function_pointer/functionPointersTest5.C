// 7 matches
//
// 1. Function pointer declaration
// 2. Function pointer dereference
// 3. Function invocation through Function pointer (dereference)
//

void f(void* param) {}
// 1
void (*pt2_f)(void*) = &f;		/* (+2) SgInitializedName, SgAssignInitializer */
// TODO: is the AST the same for '&f' and 'f'
void (*pt2_f2)(void*) = f;		/* (+2) SgInitializedName, SgAssignInitializer */

int main () {
	// 2
	(*pt2_f); 					/* (+1) Function-pointer-type variable reference */

	// 3
	(*pt2_f)(0); 				/* (+1) SgVarRefExp */ 

	pt2_f = 0;					/* (+1) SgVarRefExp */

	// ensure no false positives
	int x = 0;
	int* pt2_x = &x;
	pt2_x = 0;
}
