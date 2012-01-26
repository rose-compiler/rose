// 2 matches
// Function dereference as a function parameter
// ---------------------------------------------------------------------------
//
// Relevant section: cast to function reference
//
// 	SgCastExp > SgFunctionRefExp
//
// (we can ignore originalExpressionTree since we can use operand_i)
//
// functionPointersTest1.C:25: error: invalid conversion from ‘void (*)(void*)’ to ‘void*’
// functionPointersTest1.C:25: error:   initializing argument 1 of ‘void f(void*)’
//

void f(void* param) {}
int main () {
// 1
// Function dereference as a function parameter (implicit cast)
// ---------------------------------------------------------------------------
// AST:
//
// SgExprStatement > SgFunctionCallExp > SgFunctionRefExp
//									   > SgExprListExp > SgCastExp (explicit) (operand_i)> SgFunctionRefExp
//																 (originalExpressionTree)> SgCastExp (implicit) (operand_i)> SgFunctionRefExp
//																 								   (originalExpressionTree)> nil
// Note: originExpressionTree added by EDG (automatic implicit SgCastExp)
//
	// EDG adds implicit cast (originalExpressionTree)
	f( &f ); /* (+1) SgCastExp > SgFunctionRefExp */

// 2
// Function dereference as a function parameter (explicit cast)
// ---------------------------------------------------------------------------
// AST:
//
// SgExprStatement > SgFunctionCallExp > SgFunctionRefExp
//									   > SgExprListExp > SgCastExp (explicit) (operand_i)> SgFunctionRefExp
//																 (originalExpressionTree)> nil
//
	// explicit cast => no EDG cast
	f( (void*) &f );  /* (+1) SgCastExp > SgFunctionRefExp */
}
