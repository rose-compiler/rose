
int main() {
  int i = 0;
  label:
    i = i + 1;
    if(i < 5) {
      goto label;
    }
}
