int main() {
  int s1=0,s2=0;
  do {
    s1=s1+1;
    if(s1>5)
      s1=s1+2;
    do {
      s2=s2+1;
      if(s2>5)
        s2=s2+2;
    } while (s2<10);
  } while (s1<10);
  s2=0;
  s1=0;
  return 0;
}
