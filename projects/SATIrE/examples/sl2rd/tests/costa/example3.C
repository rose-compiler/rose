int fa(int);
int fb(int);
int fc(int,int);

  int fc (int m, int n) { 
    return fa(m) + fb(n); 
  }

  int fa (int i) {
    int j=0;
    while (j<i) {
      j++;
    }
    return j;
  } 

  int fb (int i) {
    return 3 + fa(i);
  }
