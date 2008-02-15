// non-typename template argument

// error: dependent name `A<T1>::foo' used as a type, but the 'typename'
// keyword was not suppliedk0004.cc:11:28: error: `f' is a non-type parameter,
// but `<error>' is a type argument (inst from k0004.cc:11:28)

// error: `f' is a non-type parameter, but `<error>' is a type argument (inst
// from k0004.cc:11:28)

// ERR-MATCH: dependent name.*used as a type, but the 'typename'

template<typename T1> struct A {
    static const bool foo = false;
};

template<bool f> struct B {};

template<typename T1> struct C :
        public B<A<T1>::foo> {};

C<int> c;
