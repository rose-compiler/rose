struct T3
{
    int mem1;
    T3() { } // user-provided default constructor
};

#if 0
struct T4
{
  T4(T3 x); // user-provided default constructor
};
#endif

void foobar(T3 x); // user-provided default constructor

 
int main()
{
#if 0
    int n{};                // scalar => zero-initialization, the value is 0
    double f = double();    // scalar => zero-initialization, the value is 0.0
    int* a = new int[10](); // array => value-initialization of each element
                            //          the value of each element is 0
    T1 t1{};                // class with implicit default constructor =>
                            //     t1.mem1 is zero-initialized, the value is 0
                            //     t1.mem2 is default-initialized, the value is ""
//  T2 t2{};                // error: class with no default constructor
    T3 t3{};                // class with user-provided default constructor =>
                            //     t3.mem1 is default-initialized to indeterminate value
                            //     t3.mem2 is default-initialized, the value is ""
    delete[] a;
#endif

    foobar({});

 // T4 x({});
}
