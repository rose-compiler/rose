// t0513.cc
// sizeof a struct with a bitfield

struct S1 {
  unsigned x : 2;
  unsigned y : 30;
};

int arr1[sizeof(S1)==4? +1 : -1];


struct S2 {
  unsigned x : 2;
  unsigned y : 30;
  unsigned z : 1;
};

// edg and icc both say this is 8 ...
int arr2[sizeof(S2)==8? +1 : -1];



// this is from a header file somewhere, it shows up in quite a 
// few of the Debian build failures
//
// actually, it's gcc's implementation of __real__, and then a
// check regarding its size (because gcc must emit machine code
// to manipulate it), and this appears in both gcc and mingw32
struct real_value {
  unsigned int canonical : 1;                       // 1
  signed int exp : (32 - 5);                        // 27
  unsigned long sig[((128 + (8 * 4)) / (8 * 4))];   // 5
};

extern char test_real_width [
  sizeof(struct real_value) <= (((128 + (8 * 4)) + 32)/(8 * 4) + (((128 + (8 * 4)) + 32)%(8 * 4) ? 1 : 0))*sizeof(long) ? 1 : -1
];
