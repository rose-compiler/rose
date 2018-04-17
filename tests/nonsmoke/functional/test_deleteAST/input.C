int foo (int x);

int globalVar =42;

void foobar_A(){
	int a=4;
	int b=a+2;
	int c=b*globalVar;
	int x;
	x = foo(c);
	int y =x+2;
	int z = globalVar*y;
}


void foobar_B(){
int p;
int i=4;
int k= globalVar * (i+2);
p=foo(k);
int r = (p+2) *globalVar;
}

