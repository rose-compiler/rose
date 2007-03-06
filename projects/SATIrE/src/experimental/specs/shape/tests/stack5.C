class List {
public:
  //List(int n):next(0),val(n) {}
  List* next;
  int val;
};

List* reverseList(List* x) {
  List* y;
  List* t;
  y=0;
  while(x!=0) {
    t=y; 
    y=x; 
    x=x->next;
    y->next=t;
  }
  t=0;
  return y;
}

int main() {
  List a;
  List b;
  List* ap;
  List** app;
  List* bp;
  List c;
  List* cp;

  ap=&a;
  app=&ap;
  bp=&b;
  ap->next=bp;
  cp=&c;
  bp->next=cp;


  List* revList;
  revList=reverseList(ap);

  return 0;
}
