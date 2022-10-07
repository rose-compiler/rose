void addFour(int& value) {
  value = value + 4;
}

int main() {
  int m = 3;
  int& mr = m;
  addFour(mr);
  if(m == 3){
    "unreachable";
  }
  else {
    "reachable";
  }
}
