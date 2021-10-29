//const char BLANK[2] = { ' ', ' '};
int rand();
const char BLANK[2] = "  ";
void FOO() {
  &BLANK[0];
}

int BAR() {
  return 4;
}

int BAD() {
  char badarray[2] = "  ";
  badarray[3]=1; // definitive
  int x=rand();
  badarray[x]=2; // error if x not in [0..1]
}
