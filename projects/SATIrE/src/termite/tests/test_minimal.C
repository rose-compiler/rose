int foo(int n) {
	return (n + 1);
}


int bar(int n) {
	if (n <= 0) return n;
	else return bar(n - 1);
}
int main(int argc, char* argv[]) {
	return foo(-1);	
}
