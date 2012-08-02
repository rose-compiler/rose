struct List {
  struct List *next;
};

main() {
  List *a,*b,*ap,*bp;
  int i,j,n;
  n = 5; //n=read();
  a=new List();
  b=new List();
  ap=a;
  bp=b;
  i=n;
  j=n;
  while (i>0) {
    ap->next=new List();
    ap=ap->next;
    i=i-1;
    j=i;
    while (j>0) {
      bp->next=new List();
      bp=bp->next;
      j=j-1;
    }
  }
  ap->next = b; // conc
  List *result=a;
  // write(length(result)-2)
}
