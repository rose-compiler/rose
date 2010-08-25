#ifndef TEST8A_H
#define TEST8A_H


class TestClass
{
        public:
                TestClass();
                ~TestClass();
        
                int & memberFunc1();
                const int & memberFunc1() const;

                int memberFunc2() const;        

                
                virtual int memberFunc3() { return 42;   }
                virtual int memberFunc4() { return 32;    }
                
        protected:
                int someValue;
};


class DerivedClass : public TestClass
{
        public:
                DerivedClass();
                ~DerivedClass();
                
                
                virtual int memberFunc3();
};


#endif


