#include <iostream>
#include <cassert>


class A {
    public:
        int x;
};


int main() {

    A a;
    int A::* m_ptr = &A::x;

    // this is legal
    a.*m_ptr = 24601;


    m_ptr = NULL;

    // illegal write
    //  m_ptr does not actually contain the offset to any int member of A
    //  we'll (partially) overwrite a.x
    a.*m_ptr = 24601;

    assert( a.x == 24601 );

    return 0;
}
