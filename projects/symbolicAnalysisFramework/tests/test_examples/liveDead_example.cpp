void func(int val);

int main() {
	int a=1;

	int b=a;

	a=1;
	int c=3;
	
	func(a+b);
	
	func(c);
}