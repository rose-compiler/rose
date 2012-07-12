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

int foo()
{
        int b;
        b++;
        
        testFunc0();
        
        return 5;
}

int main()
{
        int a=0;
        
        testFunc1();
        
        foo();
        
        testFunc2();
        
        if(a==0)
                a+=1;
        else
                a+=2;
        
        testFunc3();
        
        a--;
        
        while(a<100)
        {
                a++;
        }
        
        testFunc4();
        
        foo();
        
        testFunc5();
        
        a*=2;
        
        while(a<100)
        {
                a++;
        }
        
        testFunc6();
        
        return 0;
}
