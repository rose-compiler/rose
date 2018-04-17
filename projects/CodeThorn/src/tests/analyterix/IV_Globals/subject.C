int global = 1;

void changeGlobal(int arg) {
  global = 4 + arg;
}

int main() {
  global = 2;
  changeGlobal(10);

  if(global == 14) {
    "reachable";
  }
  else {
    "unreachable1";
  }

  if(global < 17 && global > 13) {
    "reachable";
  }
  else {
    "unreachable2";
  }

  global = 5;
  changeGlobal(12);

  if(global == 16) {
    "reachable";
  }
  else {
    "unreachable3";
  }

  if(!(global > 16 || global < 14)) {
    "reachable";
  }
  else {
    "unreachable4";
  }
}
