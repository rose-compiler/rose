//const char BLANK[2] = { ' ', ' '};
int rand();
char BLANK[2] = "ab";
char* p=BLANK;
void FOO() {
  &BLANK[0];
}

int BAR() {
  return 4;
}

int BAD() {
  char badarray[3] = "cde";
  badarray[1]='a';
  *p;
  char* p_local=BLANK;
  badarray[1]=*p; // correct
  badarray[3]=1; // definitive error
  int x=rand();
  badarray[x]=2; // error if x not in [0..2]
}
