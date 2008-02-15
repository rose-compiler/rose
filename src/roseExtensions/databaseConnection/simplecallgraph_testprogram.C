// ROSE test prog

// testclass ---

// test class def
class testclass {
public:
	int a;
	float f;
	testclass();
	~testclass() { };

	testclass operator() ( const int &i, const int &j ) const;

	//virtual int testFunc(int a,int b) { };
	virtual int testFunc(int a,int b);
	int splitFunc( void );
};

class derivclass : public testclass {
public:
	derivclass() {};
	~derivclass() {};
	int splitFuncD( void );

	int b;
};

class derivclass2 : public derivclass { };
class derivMult : public derivclass, public derivclass2 { };
class derivComp : public testclass, private derivMult { };
// testclass ---

// test external func def
extern int externalFunc(double d);
extern int globalInt;
int otherTestFunc(float f);

class localClassDef {
public:
	localClassDef() {};
	~localClassDef();

	double blablub;
};


//void testDelete( void ) { globalInt++; } /* del test func */


int testclass::testFunc(int a,int b) {
	return a+b;
};

int testclass::splitFunc( void ) {
	//  rm -f *png; ./dbtest testprog.c; ./dbtest testprog2.c; cat test_cg.dot | ~/rose/A++Code/springgraph.pl -s 0.7 > t.png; make dot
	
	derivclass dv;
	testclass t = dv(1,1);
	dv.splitFuncD();
	/**/
	return 0;
}

int derivclass::splitFuncD( void ) {
	return 10+splitFuncD();
}

//testclass::~testclass() { }

testclass::testclass() :
	a( otherTestFunc(1.0) )
{
	a += 2.0;

	//externalFunc(1.1);
}

// test func, compare to testclass::testFunc
static int testFunc2(int a,int b) {
	return (a+b)*100;
};

	/*char charTestFunc(float flu);
	char charTestFunc(float flu) {
		return -123.0;
	};*/

// standard test function
int otherTestFunc(float f) {
	
	testclass t;
	//return t.testFunc((int)f,(short) charTestFunc(1.0) );
	return t.testFunc((int)f,(short)f );
}


int main(int argc, char *argv[]) {
	testclass t;

	// some calls
	t.testFunc(10,20);
	otherTestFunc( 0.3 );
	testFunc2(1,1);

	//int temp = 10 + otherTestFunc(0.2) + (3* t.testFunc(20,10));
	int temp = 10 + (3* t.testFunc(20,10));
	//temp += externalFunc( 0.4 );

	return temp;
}


