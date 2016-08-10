int main(int argc, char** argv) {
  int i = 1;

  i++;
  ++i;

  if(i == 3) {
    "reachable code";
  }
  else {
    "unreachable code 0";
  }

  i--;
  --i;

  if(i == 1) {
    "reachable code";
  }
  else {
    "unreachable code 1";
  }

  i = 11;
  i += 3;
  i -= 4;
  i /= 5;
  i *= 6;
  i %= 7;

  if(i == 5) {
    "reachable code";
  }
  else {
    "unreachable code 2";
  }


  if(i = 1) {
    "reachable code";
  }
  else {
    "unreachable code 3";
  }

  if((i <<= 1) == 2) {
    "reachable code";
  }
  else {
    "unreachable code 4";
  }

  if((i >>= 1) == 1) {
    "reachable code";
  }
  else {
    "unreachable code 5";
  }

  i = 1;

  int* ip = &i;

  *ip += 2;

  if(i == 3) {
    "reachable code";
  }
  else {
    "unreachable code 6";
  }

  if((*ip -= 2) == 1) {
    "reachable code";
  }
  else {
    "unreachable code 7";
  }

}
