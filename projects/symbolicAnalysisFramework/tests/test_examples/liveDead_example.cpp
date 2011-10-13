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

int main() {
        testFunc0();
        int a=1;

        int b=a;

        a=1;
        int c=3;
        testFunc1();
        int d;
        int e=1;
        
        testFunc2();
                b++;
                b*=e;
        
                useFunc((a+b)*(a+b));
                useFunc(c);
        
        testFunc3();
        
                c++;
                useFunc(c);
        
        testFunc4();
        
                c=0;
                d=1;
                int arr[2] = {c, d};
        testFunc5();
                useFunc(arr[0]);
        
        testFunc6();
                
                c=0;
                d=1;
                int arr2[2] = {c, d};
        
        testFunc7();
}
