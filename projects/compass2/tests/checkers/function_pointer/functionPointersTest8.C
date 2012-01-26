// 14  matches
//
// Return function reference
//

int foo();

typedef int(*t_pt2foo)(); /* (+1) SgTypedefDeclaration */

// SgReturnStmt > SgFunctionRefExp > SgCastExp > SgFunctionRefExp
t_pt2foo bar1(t_pt2foo x) {return foo;} /* (+3) return type; SgInitializedName; return value */
void* bar2() {return foo;} /* (+1) return value (SgCastExp (operand_i)> SgFunctionRefExp */

int bar3() {return 1 + (*bar1(foo))();} /* (+1) SgFunctionCallExp (function)> SgPointerDerefExp */
int bar4(int x) {return 1 + (*(t_pt2foo)bar2())();} /* (+1) SgFunctionCallExp (function)> SgPointerDerefExp (operand_i)> SgCastExp (operand_i)> SgFunctionCallExp*/
// TODO: multiple casts
int bar4_2(int x) {return 1 + (*(t_pt2foo)(void*)((t_pt2foo)bar2()))();} /* (+1) SgFunctionCallExp (function)> SgPointerDerefExp (operand_i)> SgCastExp (operand_i)> SgCastExp (operand_i)> SgCastExp (operand_i)> SgFunctionCallExp */
int bar5(int x) {return (*(t_pt2foo)bar2())() + 3 * 4 + 1 + (*(t_pt2foo)bar2())() - 3;} /* (+2) SgFunctionCallExp (function)> SgPointerDerefExp */

// ensure no false-positives: SgReturnStmt
int bar_false_positive1() {return foo();}
int bar_false_positive2() {return 1 + (*(&foo))();}
int bar_false_positive3() {return 1 + foo();}

int bar(int x) {return x;}
int (*pt2_bar)(int) = bar; /* (+2) SgInitializedName; SgAssignmentInitializer */
int main () {
	bar( (*pt2_bar)((*pt2_bar)(1)) ); /* (+2 = 2x1) SgVarRefExp */

	return foo();
}
