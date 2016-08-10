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

}
