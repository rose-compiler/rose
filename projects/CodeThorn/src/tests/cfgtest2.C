int main() {
  int x=0;
  int s=0;
  while(x<10) {
    s=s+x;
    x++;
    if(x==1) {
      x++;
      continue;
    }
    if(x==8)
      break;
  }
  do {
    s=s+x;
    if(x==8) {
      x++;
      break;
    }
    if(x==1) {
      x++;
      continue;
    }
    x++;
  } while(x<10);
  for(int x=0;x<10;x++) {
    s=s+x;
    if(x==8) {
      x++;
      break;
    }
    if(x==1) {
      x++;
      continue;
    }
    x++;
  }
}
