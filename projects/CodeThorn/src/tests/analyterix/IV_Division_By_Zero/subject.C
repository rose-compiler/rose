int main(int argc, char** argv) {
  int i2 = 1;
  if(argc == 2) {
    int i = 0;
    i = 1 / i; // division by zero
    i2 = 2; // unreachable code
  }
  int top = static_cast<void*>(argv) > static_cast<void*>(&argc);

  // logical and
  if (argc == 3){
    if(true && 1 / (i2 - 1)) { // division by zero
      int i3 = 3;// unreachable code
    }
    else {
      int i4 = 4;// unreachable code
    }
    int i5 = 5; // unreachable code
  }
  else if (argc == 4){
    if(false && 1 / (i2 - 1)) { // NO division by zero
      int i6 = 6;// unreachable code
    }
    else {
      int i7 = 7;
    }
    int i8 = 8;
  }
  else if (argc == 4){
    if(top && 1 / (i2 - 1)) { // possible division by zero
      int i9 = 9; // unreachable code
    }
    else {
      int i10 = 10;
    }
    int i11 = 11;
  }

  // logical or
  else if (argc == 4){
    if(false || 1 / (i2 - 1)) { // division by zero
      int i13 = 13;// unreachable code
    }
    else {
      int i14 = 14;// unreachable code
    }
    int i15 = 15; // unreachable code
  }
  else if (argc == 5){
    if(true || 1 / (i2 - 1)) { // NO division by zero
      int i16 = 16;
    }
    else {
      int i17 = 17; // unreachable code
    }
    int i18 = 18;
  }
  else if (argc == 6){
    if(top || 1 / (i2 - 1)) { // possible division by zero
      int i19 = 19;
    }
    else {
      int i20 = 20; // unreachable code
    }
    int i21 = 21;
  }
}
