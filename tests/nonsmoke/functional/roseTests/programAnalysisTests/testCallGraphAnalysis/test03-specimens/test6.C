class A
{
public:
	virtual int f1() = 0;
	virtual int f2() {}
	int f3();
	virtual int f4();
//	int f5();
};

int A::f3() { f1(); return f3();}
int A::f4() {}

class B : public A
{
public:
	virtual int f1();
	virtual int f2() {}
};

int B::f1() {}

class C : public A
{
public:
	virtual int f1() {}
	int f3() {}
};

class D : public B
{
public:
	virtual int f2() {}
};

class E : public D 
{
public:
	virtual int f1() { return 5; }
};

class G : public E
{
public:
	virtual int f1();
};

int G::f1() {}

class F : public D
{
public:
	virtual int f1() {}
	virtual int f2() {return 5;}
	int f3() {return 2;}
};

class H : public C
{
public:
	virtual int f1() {}
	virtual int f2() {}
	int f3() {}
};

/*
void f(A &obj)
{
obj.f1();
obj.f2();
obj.f3();
}
*/

void g(B *o)
{
o->f1();
o->f2();
o->f3();
o->f4();
}

int k()
{
}

//int (*f)();

int main()
{
B b;
C c;
D d;
E e;

//f = k;
//f();
//k();
g(&b);
}

