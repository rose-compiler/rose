// cc.in22
// bitfields

struct BF {
  int x : 2;
  unsigned y : 4;
  int /*anon*/ : 7;
  //ERROR(1):  /*anon*/ : 7;  // missing type--parse error
};
