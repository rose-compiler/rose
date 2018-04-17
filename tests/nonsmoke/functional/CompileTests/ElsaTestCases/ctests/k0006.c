// static inline function implicitly returning int

// originally found in package framerd_2.4.1-1.1

// a.i:4:1: Parse error (state 954) at {

// ERR-MATCH: Parse error.*at {

static inline foo()
{
    return 0;
}

inline static bar()
{
    return 0;
}

const inline static f1()
{
    return 0;
}

int main()
{
    return foo();
}
