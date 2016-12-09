// t0459.cc
// invalid C++ causes ambiguity to remain, then cloning dies

typedef int my_size_t;    //ERRORIFMISSING(1): also needs to be gone to trigger problem

template<bool t> class C {
    void foo() {
        int x;            //ERRORIFMISSING(1): if this is missing, input is ambiguously erroneous
        (x) + (my_size_t)0;
    }
};
template class C<true>;
