// test that a label can end a block, in particular that a case can
// end a switch statement
int main() {
  switch(1) {
  default:
  case 1:
  case 2:
  }
  {
  gronk:
  blonk:
  }
}
