class A
{
public:
	int f1() {}
	int f2();
	int f3(bool);
};

int (A::*k)();
int foo(){

};

//
int bar()
{
foo();
};


int main()
{


A a;
k = &A::f1;
(a.*k)();
}

