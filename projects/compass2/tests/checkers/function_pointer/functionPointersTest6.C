// 13 matches

int a(int x) {return x;}
int main () {
	typedef int(*t_pt2a)(int); /* (+1) function pointer typedef */

	typedef t_pt2a* pt2_typedef; /* (+1) pointer to function-pointer-typedef */

	t_pt2a pt2a; /* (+1) function pointer */

	t_pt2a* pt2_typedef2 = &pt2a; /* (+2) pointer to function-pointer-typedef */

	t_pt2a& ref2_fptr = pt2a; /* (+2) reference to function-pointer-typedef */

	t_pt2a* ptr2_ref2_fptr = &ref2_fptr; /* (+2) pointer to reference of function-pointer-typedef (pointer to function pointer) */

	t_pt2a** pt2_typedef3 = &pt2_typedef2; /* (+2) pointer to pointer-to--function-pointer-typedef */

	pt2_typedef pt2_typedef_arr[10]; /* (+1) array of pointers to function-pointer-typedefs typedefs */

	pt2_typedef* pt2_typedef_arr2[10]; /* (+1) array of pointers to pointers-to--function-pointer-typedefs--typedefs */
}
