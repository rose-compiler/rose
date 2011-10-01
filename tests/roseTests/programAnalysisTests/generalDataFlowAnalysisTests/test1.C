// Function used to identify ultimate uses of expressions
void useFunc(int val) {}
// Function used by the self-tester to check that the set of variables at the call is correct
void testFunc() {}
void testFunc0() {}
void testFunc1() {}
void testFunc2() {}
void testFunc3() {}
void testFunc4() {}
void testFunc5() {}
void testFunc6() {}
void testFunc7() {}

int sourceOfTaint() { return 0; }

int main(int argc, char **argv) {
	testFunc0();
	int a = 1;
	int b = 4;
	int c = a + b;
        int d = 3;

        for (int i = 0; i < argc; ++i)
          d += b;

	int e = d * 5;
        int f = e;
        int foo = f++;
        int g = f + 4;

        int h = sourceOfTaint();

	testFunc1();
	
        int ret = foo + a + b + c + d + e + f + g + h;

        return ret;
}
