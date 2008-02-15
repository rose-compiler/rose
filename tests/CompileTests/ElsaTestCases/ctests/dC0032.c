//  /home/dsw/oink_extra/ballAruns/tmpfiles/./oaf-0.6.10-5/ac-corba-hKmB.i:4985:57: error: reprSize of a sizeless array

// Open array: you can have an array of no size in a struct and it
// counts as 0.  It is even legal if it is not at the end!

//  #include <stdio.h>
struct foo {
  int x;
  char a[];                     // No size
  // dsw: hmm, not anymore
  //  double y;                     // can even have stuff afterward!
};

struct bar {
  int x;
  // no array here at all
//    char a[];                     // No size
  double y;
};

int main() {
  // gcc 3.4.0 on my x86 machine at least says these both have size
  // 12.
//    printf("sizeof(foo) %d\n", sizeof(foo));
//    printf("sizeof(bar) %d\n", sizeof(bar));
  // force its size to be computed
  int x = sizeof(struct foo);
  return 0;
}
