// 2 matches

void foo() {}

struct {
	void (*fptr)(); /* (+1) SgInitializedName */
} A;

int main () {
	A.fptr; /* (+1) SgVarRefExp */
}
