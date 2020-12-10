void f(const int*);
void g()
{
    const int N = 10;
    [=]{ 
        int arr[N]; // not an odr-use: refers to g's const int N
        f(&N); // odr-use: causes N to be captured (by copy)
               // &N is the address of the closure object's member N, not g's N
    }();
}

