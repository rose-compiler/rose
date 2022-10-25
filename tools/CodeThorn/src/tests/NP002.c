int BLK[16];
void FOO(int SUB, int CNT, int *DST_PTR) {
  int ii;
  for(ii=0;ii<CNT;++ii) {
    DST_PTR[ii]=0;
  }
  BLK[SUB] = 1;
  return;
}

int main() {
  int DST[16];
  FOO(0,2,0); // pass in 0 ptr
  return 0;
}
