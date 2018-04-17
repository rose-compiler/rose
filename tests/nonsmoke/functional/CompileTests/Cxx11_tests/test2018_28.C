struct struct1 {};
struct struct2 {};

template < typename typename1 = struct1,
typename typename2 >
inline void template_func1( typename2&& parm1 ) {}

void func2() {
template_func1<struct2>([=](int parm2)
{ 
#define MISSING_HASH_DEFINE
}

);
}
