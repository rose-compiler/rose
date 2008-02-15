// this had stopped working and we hadn't a test to reveal it

struct X {
  // sm: this is invalid C++!  see 9.2 para 13
  //ERROR(1): int X;

  int Y;     // ok
};


// another ansi-only rule
//ERROR(2): int arr[0];
