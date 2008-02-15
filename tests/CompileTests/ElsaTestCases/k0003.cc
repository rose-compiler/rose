// template argument from static const member of template class

// originally found in package apt

// error: ` (A<T1, T2>::foo)' must lookup to a variable with a value for it to
// be a variable template reference argument

template<typename T1, typename T2> struct A {
    static const bool foo = false;
};

template<bool f> struct B {};

template<typename T1, typename T2> struct C :
        public B<A<T1, T2>::foo> {};

C<int,float> c;
