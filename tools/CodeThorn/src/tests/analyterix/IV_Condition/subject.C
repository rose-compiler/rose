int main() {
  int i = 0;
  if(1) {
     i = 1;
  }
  else {
    // unreachable
    i = 2;
  }

  if(i > 1) {
    // unreachable
    i = -1;
  }
  else {
    i = 10;
  }

  if(i >= 10) {
    i = -2;
  }
  else {
    // unreachable
    i = (10, 20);
  }

  if(i < -1) {
    i = 0;
  }
  else {
    // unreachable
    i = -2;
  }

  if(i <= -1) {
    // unreachable
    i = -20;
  }
  else {
    i = -10;
  }

  if(i < 0) {
    i = -20;
  }
  else {
    // unreachable
    i = -10;
  }

  if(i < 0 && i >= -20) {
    i = -30;
  }
  else {
    // unreachable
    i = -40;
  }

  if(i > 0 || i >= -80) {
    i = -50;
  }
  else {
    // unreachable
    i = -60;
  }

  if(!(i < -100 || i >= -40) && i != 4) {
    i = -70;
  }
  else {
    // unreachable
    i = -80;
  }

  if(0.5) {

  }

  int l;
}
