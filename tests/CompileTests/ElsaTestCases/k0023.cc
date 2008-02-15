// typedef pointer to member function in template

// originally seen in package aqsis

// ERR-MATCH: Assertion failed: inClassNAT->isCompoundType

template<class T> struct S1 {
    typedef void * (S1<T>::*funcType)();
};
