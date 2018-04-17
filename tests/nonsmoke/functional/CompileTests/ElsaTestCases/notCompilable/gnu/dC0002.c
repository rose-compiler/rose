// arrays of no size are assumed to have size one in gcc, but not g++

// sm: 2005-03-12: This testcase has been improperly minimized; as
// noted below, gcc does *not* accept it!  Someone should try to find
// out what example this came from and minimize it properly.

struct S {
  int x;
  //int a[];
};

int a[];
int f() {
  sizeof a;                // though it seems to not tolerate this; whatever

  sizeof(struct S);
}
int a[3];
int g() {
  sizeof a;
}

struct Incomplete;
void h() {
  //ERROR(1): struct Incomplete inc;
}


