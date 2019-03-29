char ASCII_BUF[10];

int main() {
  char* ASCII_PTR;
  int i;

  // normalization bug here, & should not be normalized
  ASCII_PTR = &(ASCII_BUF[0]);  

  for(i = 0; i < 10; i++) {
    ASCII_PTR[i] = ' ';
  }
  
  return 0;
}
