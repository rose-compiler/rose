#include <iostream>
#include <vector>
#include <cstdlib>

using namespace std;

void flopTest()
{
    float a,b,c,d,e,f,g,h;

    a = 9.9 + 50;
    b = 1.0;
    c = a /b;
    d = a * b - c;
}

void longFunction(long bla);

void longFunction(long);

void unknownFunction();

namespace myNameSpace
{
    class classInNameSpace
    {

    };

    void funcInNamespace() {}

    namespace namespaceInNameSpace
    {
        void test() {};
    }
}

class C1
{
    class InnerClass
    {
        public:
            int f1();
    };

    public:
          void  memberFunc() { }

    protected:
        int memberVar;

};

int C1::InnerClass::f1()
{
    int a=5;
    return a/2;
}

void longFunction(long iterations)
{
	double d=1000;
	for(long int i=0;i<iterations; i++)
	{
	    for(int j=0; j<10; j++)
	    {
	        d=d/2+17;
	    }
	}
	cout<< "d=" << d << std::endl;
}

void copyVector(vector<int>&a, vector<int>& b)
{
	for(int i=0; i<a.size(); i++)
	{
		b.push_back(a[i]);
	}
}

void fillVector(vector<int> & v, int size)
{
	v.clear();
	for(int i=0; i<size; i++)
		v.push_back(i);
}


int main(int argc, char**argv)
{
	if(argc<2)
	{
		cout<<"Specify argument size";
	}

	int size=atoi(argv[1]);

	/*
	vector<int> v;
	fillVector(v,size);
	vector<int> copy;
	copyVector(v,copy);
	*/

	longFunction(size*1000);

	return 0;
}
