int main() {
	int a = 2;
	int* a_p = &a;
	a = a + 1;
	int b = *a_p;
	int c = (volatile const) *a_p;
	c = (volatile) *a_p;
	c = (const) *a_p;
	return 0;
}
