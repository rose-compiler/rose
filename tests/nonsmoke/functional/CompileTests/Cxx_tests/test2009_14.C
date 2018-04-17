#if 0
Hi Dan,
attached is a test file which demonstrates the issues with bit shifting. The comments should explain the problem.

#endif


// using only the include fails
#include <stdint.h>

// having just the typedef instead of the whole include works, without warnings!
// this typedef is directly taken from stdint.h (C99 ISO header file)
// for gcc 4.2.2 tr1/cstdint just includes that file.
// this is not just a C++ issue, the warnings are generated with equivalent expressions
// in C too.
//typedef unsigned long long int uint64_t;

// this test is done, because it initially pointed me to that problem
template< typename T, T t>
class A{};

int main()
{
    uint64_t t( 0 );

    /*std::cout << sizeof( uint64_t ) << std::endl;
    std::cout << sizeof( long long ) << std::endl;*/

    // error
    A<uint64_t, uint64_t( 1 ) << 48> a;
    // ok
    A<uint64_t, ( uint64_t( 1 ) << 31 ) << 17> b;

    // ok
    unsigned long long int l = (unsigned long long int)( 1 ) << 48;

    // warning
    t = uint64_t( 1 ) << 48;
    // ok
    t = ( uint64_t( 1 ) << 31 ) << 17;

    // warning
    t = uint64_t( 1 ) << uint64_t( 48 );

    return 0;
}
