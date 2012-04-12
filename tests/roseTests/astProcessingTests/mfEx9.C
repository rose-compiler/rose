int func(int x) {
    if (x == 1) {
        if (x == 2) {
            if (x == 3) {
                return 0;
            }
            if (x == 4) {
                return 0;
            }
        }
        return 0;
   }
  else {
     return 0;
   }
}

int main() {
    int z = func(0);
    z = func(1);
    int y = func(z);
    return 0;
}
