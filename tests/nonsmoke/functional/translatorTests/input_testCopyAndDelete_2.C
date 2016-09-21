// This test code is copied from: tests/nonsmoke/functional/roseTests/astInterfaceTests/inputdeepDelete.C

#if 1
const int  mnp=100;

//test typedef
typedef double type_double;
type_double a_variable =10;
#endif

#if 1
//test struct declaration
struct school{
    int class_num;
    int division_num;
} sc;

typedef school sch;
#endif

#if 1
//test enum declaration
enum DAY            
{
#if 1
    saturday,
    sunday,      
    monday,         
    tuesday,
    wednesday,      
    thursday,
#endif
    friday
} workday;
#endif

#if 1
DAY wk;
#endif

#if 1
//test class declaration and instantiation
class XYZ {
    private:
	int a,b,c;
    public:
	XYZ();
	int get_a(){return a;} 
        int get_a(int d){return d;} //overloaded member function
        int get_b(int b);
};

int XYZ::get_b(int b){
        return b;
}

XYZ *xyz = new XYZ;
#endif

#if 1
//test overloaded functions
int foo (int x);
int foo (int a[10]);
int foo (double x, int y){
    int a=1;
    return a;
}


//test function call
int globalVar =42;

void foobar_A(){
	int a=4;
	int b=a+2;
	int c=b*globalVar;
	int x;
	x = foo(c);
	int y =x+2;
	int z = globalVar*y;
	{
		int x;
	}
}
void foobar_B(){
int p;
int i=4;
int k= globalVar * (i+2);
p=foo(k);
int r = (p+2) *globalVar;
}


#if 0
// DQ (1/12/13): This has always been commented out (in the original test code).
// But perhaps this is an example of a bug that needs to be fixed.

// test template
template <class T>
class Template_class{
       T a;
       T get(){return a;}
};

Template_class<int> clss;
#endif



//main function
int main()
{
  int i = 0;    
  return 0;
}
#endif

