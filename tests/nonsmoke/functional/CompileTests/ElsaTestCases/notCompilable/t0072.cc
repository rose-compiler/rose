// t0072.cc
// declmodifiers after type specifier word

const static int x = 4;

//ERROR(1): const const int q;    // duplicate 'const'

unsigned int r;


// some more hairy examples
long unsigned y;
const unsigned volatile long static int z;

long long LL;    // support this because my libc headers use it..

// may as well get the literal notation too
void foo()
{
  LL = 12LL;
  LL = -1LL;
}


//ERROR(3): long float g;    // malformed type
                       
// too many!
//ERROR(4): long long long LLL;
//ERROR(5): long long long long LLLL;
//ERROR(6): long long long long long LLLL;

