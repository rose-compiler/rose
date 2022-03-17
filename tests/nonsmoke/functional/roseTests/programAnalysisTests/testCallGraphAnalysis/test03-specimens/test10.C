// The Call graph has a bug that if C<-B<-A, and B
// had an implicit constructor, then A's constructor
// would not show up on the call graph.  I fixed that,
// and added this test to demonstrate the fix on templates. 
//(test9.C shows it on normal classes) -Jim Leek 2022
 
template<typename TT>
TT a_init(TT in) 
{
    return in + 5;
}


template<typename TT>
class A 
{
public:
    A() : a(a_init(15)) {};
    TT a;
};

template<typename TT>
class B : public A<TT>
{
public:
    TT b;
};

template<typename TT>
class C : public B<TT>
{
public:
    C(TT in_c) : c(in_c) {};
    
    TT c;
};


int user_sysinit() 
{
    C<int> newC(5);
//    B newB;
//    A newA;

    return newC.a;
}

//Have both main and user_sysinit for testing convinience 
int main() 
{
    return user_sysinit();
}

