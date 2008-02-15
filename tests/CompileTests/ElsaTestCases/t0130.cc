// t0130.cc
// some specific cases where we want to allow 0-length arrays,
// despite cppstd 8.3.4 para 1

// dsw: I turned the 0-length ones off since I now allow them to get
// the kernel through.  See in/gnu/dC0006.c

// no
// dsw: this failure is OFF: ERROR(1): int arr[0];
// dsw: the negative version is tested below

// ok
int array[1];

// no
//ERROR(4): int negGlobalArr[-1];

struct F {
  // yes
  int fieldArr[0];

  // no
  //ERROR(3): int negLengthArr[-1];
};

void f()
{
  // no
  // dsw: this failure is OFF: ERROR(2): int localArr[0];
  // dsw: might as well test the negative version though
  //ERROR(2): int localArr[-1];
  
  // yes
  int okLocalArr[1];
}



