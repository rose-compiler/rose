// The Call graph has a bug that if C<-B<-A, and B
// had an implicit constructor, then A's constructor
// would not show up on the call graph.  I fixed that,
// and added this test to demonstrate the fix. -Jim Leek 2022

class A 
{
public:
    A(): a(15) {};
    int a;
};


class B : public A
{
public:
    int b;
};

class C : public B
{

public:
    C(int in_c) : c(in_c) {};;
    int c;
private:
    C();
};


int user_sysinit() 
{
    C newC(5);
//    B newB;
//    A newA;

    return newC.a;
}

//Have both main and user_sysinit for testing convinience 
int main() 
{
    return user_sysinit();
}

