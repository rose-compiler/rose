class A
{
public:
	int f1() {}
	int f2();
	int f3(bool);
};

int (A::*k)();

int main()
{
A a;
k = &A::f1;
(a.*k)();
}

