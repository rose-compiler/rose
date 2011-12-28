// 12 matches

class A
  {
	private:

		typedef int (*t_pt2_foo)(); /* (+1) SgTypedefDeclaration */
		typedef void (*t_pt2_bar)(); /* (+1) SgTypedefDeclaration */

		t_pt2_bar pt2_bar; /* (+1) SgInitializedName */
		t_pt2_foo pt2_foo; /* (+1) SgInitializedName */

	public:
		A (t_pt2_foo pt2_foo, t_pt2_bar pt2_bar) : /* (+2 2x1) SgInitializedName */
			pt2_foo( pt2_foo ), pt2_bar( pt2_bar ) {} /* (+4 = 2x2) SgInitializedName; SgVarRefExp */

		int foo () { return 1; }
		virtual void bar() = 0;

		t_pt2_foo get_pt2foo() const { return pt2_foo; } /* (+2) SgFunctionDeclaration function return type; SgVarRefExp */
  };

int main () {
}
