int main() {
  int s=0;

  // A
  for(int i=0;i<10;i++) {
    s+=1;
  }

  // B
  switch(s) {
  case 0: s+=1;
  case 1: s+=2;break;
  case 2: s+=3;break;
  case 3: if(s==3) {
      break;
    } else {
      s++;
    }
  case 4:
    while(s<10) {
      s+=1;
      if(s==9)
        break;
    } 
    // fall through to default
  default:
    s+=10;
  }

  // C
  for(int i=0;i<10;i++) {
    s+=1;
    if(s>2)
      break;
  }

  // D
  for(int i=0;i<10;i++) {
    s+=1;
    if(s==2)
      continue;
  }

  // E
  while(s<10) {
    s+=1;
    if(s==9)
      break;
    if(s==8)
      continue;
  }

  // F
  do {
    s+=1;
    if(s==2)
      continue;
    s=s+10;
  } while(s<11);

}
